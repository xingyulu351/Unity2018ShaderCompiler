using System;
using System.Collections.Generic;
using System.IO;
using System.Text;

namespace UnityBindingsParser
{
    public partial class Parser
    {
        public static void Parse(TextReader reader, IBindingsConsumer consumer)
        {
            new Parser(new PushBackReader(reader), consumer).Run();
        }

        private static ArgumentException ParsingError(string line)
        {
            var lineBytes = Encoding.ASCII.GetBytes(line);
            var hexString = BitConverter.ToString(lineBytes);

            return new ArgumentException(string.Format("Can't parse line: '{0}' (length: {1}, hex: {2})", line, line.Length, hexString));
        }

        private readonly PushBackReader m_Reader;
        private readonly IBindingsConsumer m_Consumer;
        private PatternBasedDispatcher m_CurrentDispatcher;
        private readonly List<string> m_AccumulatedContent = new List<string>();
        private ParsingState m_State = ParsingState.None;

        internal Parser(PushBackReader reader, IBindingsConsumer consumer)
        {
            m_Reader = reader;
            m_Consumer = consumer;
            InitRules();
        }

        private void ReadSimpleComment(string st)
        {
            m_Consumer.OnPlainContent(st);
        }

        private void HandlePoundIf(string st)
        {
            m_Consumer.OnPlainContent(st);
        }

        private void HandleEndIf(string st)
        {
            m_Consumer.OnPlainContent(st);
        }

        private void EnterState(ParsingState state, string startingLine)
        {
            m_State = state;
            Accumulate(startingLine);
        }

        private void ReadExample(string startingLine)
        {
            ReadUntilNextMarkerMatches(ParserUtil.EndExMarker.IsMatch, startingLine, m_Consumer.OnExample);
            m_Reader.PopHead(); // consume the END EX marker
        }

        private void ReadMultilineDocumentation(string startingLine)
        {
            var previousState = m_State;

            EnterState(ParsingState.Documentation, startingLine);

            var cancellationToken = new CancellationToken();
            ParseWith(GetMultilineDocDispatcher(cancellationToken), cancellationToken);

            m_State = previousState;
        }

        private void FlushAccumulatedContent()
        {
            var originalContent = m_AccumulatedContent.ToArray();
            var trimmedContent = m_AccumulatedContent.ConsumeAndTrim();
            m_AccumulatedContent.Clear();
            if (trimmedContent.IsEmpty())
                return;

            switch (m_State)
            {
                case ParsingState.Documentation:
                    m_Consumer.OnDocumentation(trimmedContent);
                    break;

                case ParsingState.CsRaw:
                    m_Consumer.OnCsRaw(trimmedContent);
                    m_Consumer.OnCsRaw(originalContent);
                    break;

                case ParsingState.CppRaw:
                    m_Consumer.OnCppRaw(trimmedContent);
                    m_Consumer.OnCppRaw(originalContent);
                    break;

                default:
                    throw new InvalidOperationException("Invalid state for accumulated content: " + trimmedContent);
            }
        }

        internal void Run()
        {
            ParseWith(m_TopLevelRules, new CancellationToken());
        }

        private void ParseWith(PatternBasedDispatcher dispatcher, CancellationToken cancellationToken)
        {
            ParseWith(dispatcher, cancellationToken, ThrowOnUnrecognizedLine);
        }

        private void ParseWith(PatternBasedDispatcher dispatcher, CancellationToken cancellationToken, Action<string> unrecognizedLineHandler)
        {
            var previousDispatcher = m_CurrentDispatcher;
            m_CurrentDispatcher = dispatcher;
            string line;
            while (TryReadLine(out line))
            {
                var trimmedLine = line.Trim();
                if (trimmedLine.IsEmpty())
                {
                    unrecognizedLineHandler(line);
                    continue;
                }
                if (!dispatcher.Dispatch(trimmedLine, line))
                    unrecognizedLineHandler(line);
                if (cancellationToken.IsCancelled)
                    break;
            }
            m_CurrentDispatcher = previousDispatcher;

            FlushAccumulatedContent();
        }

        private void ThrowOnUnrecognizedLine(string line)
        {
            if (m_State == ParsingState.None)
                if (IsComment(line))
                    return;
                else if (!line.Trim().IsEmpty())
                    throw ParsingError(line);

            Accumulate(line);
        }

        private void Accumulate(string line)
        {
            m_AccumulatedContent.Add(line);
        }

        private static bool IsComment(string line)
        {
            return line.TrimStart().StartsWith("//");
        }

        private Action<string> MarkerReaderFor(Action action)
        {
            return line => ConsumeMarker(line, action);
        }

        private void ConsumeMarker(string line, Action action)
        {
            if (m_CurrentDispatcher.MatchesAny(line))
                m_Reader.PushBack(line);
            action();
        }

        private Action<string> SimpleBlockReaderFor(Action<string> consumer)
        {
            return line => ReadUntilNextMarkerOrComment(line, consumer);
        }

        private Action<string> BlockReaderFor(Action<string> consumer)
        {
            return line =>  ReadUntilNextMarker(line, consumer);
        }

        private void ReadUntilNextMarker(string text, Action<string> action)
        {
            ReadUntilNextMarkerMatches(m_CurrentDispatcher.MatchesAny, text, action);
        }

        private void ReadUntilNextMarkerOrComment(string text, Action<string> action)
        {
            ReadUntilNextMarkerMatches(m_CurrentDispatcher.MatchesAnyAndComments, text, action);
        }

        private void ReadUntilNextMarkerMatches(Predicate<string> predicate, string initialText, Action<string> action)
        {
            ReadUntilNextMarkerMatches(predicate, initialText, line => line, action);
        }

        private void ReadUntilNextMarkerMatches(Predicate<string> predicate, string initialText, Func<string, string> lineProcessor, Action<string> action)
        {
            var buffer = new StringBuilder();
            buffer.AppendUnixLine(initialText.TrimStart());
            string line;
            while (TryReadLine(out line))
            {
                if (predicate(line))
                {
                    m_Reader.PushBack(line);
                    break;
                }
                buffer.AppendUnixLine(lineProcessor(line));
            }
            action(buffer.ToString().TrimEnd());
        }

        private void ReadEnum(string startingLine)
        {
            m_Consumer.StartEnum(startingLine);

            var cancellationToken = new CancellationToken();

            var enumDispatcher = GetEnumDispatcher(cancellationToken);

            ParseWith(enumDispatcher, cancellationToken);

            m_Consumer.EndEnum();
        }

        private void ReadClass(string startingLine)
        {
            //_consumer.
            m_Consumer.StartClass(startingLine);
            ParseTypeBody();
            m_Consumer.EndClass();
        }

        private void ReadNonSealedClass(string startingLine)
        {
            m_Consumer.StartNonSealedClass(startingLine);
            ParseTypeBody();
            m_Consumer.EndNonSealedClass();
        }

        private void ReadStaticClass(string startingLine)
        {
            m_Consumer.StartStaticClass(startingLine);
            ParseTypeBody();
            m_Consumer.EndStaticClass();
        }

        private void ReadStruct(string startingLine)
        {
            m_Consumer.StartStruct(startingLine);
            ParseTypeBody();
            m_Consumer.EndStruct();
        }

        private void ReadStructNoLayout(string startingLine)
        {
            m_Consumer.StartStructNoLayout(startingLine);
            ParseTypeBody();
            m_Consumer.EndStructNoLayout();
        }

        private void ParseTypeBody()
        {
            var cancellationToken = new CancellationToken();

            ParseWith(GetTypeBodyRules(cancellationToken), cancellationToken);
        }

        void ReadCustomProp(string text)
        {
            var idx = text.LastIndexOf('}') + 1;
            m_Consumer.OnCustomProp(text.Substring(0, idx));
            var newline = "\n";
            if (text.Contains("\r\n"))
                newline = "\r\n";
            var marker = m_Reader.PopHead();
            foreach (var line in text.Substring(idx).Split(new[] {newline}, StringSplitOptions.None))
                m_Reader.PushBack(line);
            m_Reader.PushBack(marker);
        }

        private void ReadObsolete(string content)
        {
            var parts = content.Split(new[] {' '}, 2);
            m_Consumer.OnObsolete(parts[0], parts.Length == 2 ? parts[1] : "");
        }

        private void ThrowThreadSafeNotSupportedException(string line)
        {
            throw new NotSupportedException("THREAD_SAFE is deprecated. Use THREAD_AND_SERIALIZATION_SAFE instead");
        }

        private void ThrowUnsafeThrowExceptionNotSupportedException(string line)
        {
            throw new NotSupportedException("THREAD_AND_SERIALIZATION_UNSAFE_THROW_EXCEPTION is deprecated. Methods now throw an exception by default.");
        }

        private bool TryReadLine(out string line)
        {
            return (line = m_Reader.ReadLine()) != null;
        }

        private void ReadDocumentation(string startingLine)
        {
            ReadUntilNextMarkerMatches(line => !ParserUtil.DocumentationMarker.IsMatch(line), startingLine, TrimDocumentationLine, m_Consumer.OnDocumentation);
        }

        private static string TrimDocumentationLine(string line)
        {
            var m = ParserUtil.DocumentationMarker.Match(line);
            if (!m.Success)
                throw ParsingError(line);
            return ParserUtil.TextAfterMarkerMatch(m);
        }

        private void ImportDocFile(string startingLine)
        {
            m_Reader.OpenDocFile(startingLine);
        }
    }
}
