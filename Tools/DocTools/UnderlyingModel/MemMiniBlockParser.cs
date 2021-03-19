using System.Collections.Generic;
using System.Linq;
using System.Text;
using UnderlyingModel;
using UnderlyingModel.MemDoc;

namespace MemDoc
{
    internal class MemMiniBlockParser : MiniBlockParser
    {
        protected internal MemMiniBlockParser(MemberSubSection block)
            : base(block)
        {
        }

        internal override void ProcessOneMeaningfulBlock(ref List<string> remainingLines)
        {
            var accumulator = new StringBuilder();

            var exampleProps = new ExampleProperties(false, false, CompLang.JavaScript);
            int nSigBlocksFound = 0;

            m_CurrentState = DefaultMemFileState;

            while (remainingLines.Any() && nSigBlocksFound < 2)
            {
                var line = remainingLines.First();
                var shortLine = line.TrimStart();

                if (shortLine.StartsWith(MemToken.SignatureOpen))
                {
                    nSigBlocksFound++;
                    //if we detect a second SignatureOpen token, this is the beginning of a new block,
                    //so we don't consume this line
                    if (nSigBlocksFound == 2)
                        continue;

                    remainingLines.RemoveAt(0);
                    TerminateChunk(accumulator, MemFileState.Signatures);
                    continue;
                }
                remainingLines.RemoveAt(0);

                if (shortLine.StartsWith(MemToken.SignatureClose))
                {
                    //Assert.AreEqual(MemFileState.Signatures, m_CurrentState);
                    TerminateChunk(accumulator, DefaultMemFileState);
                    continue;
                }
                if (shortLine.StartsWith(MemToken.TxtTagOpen))
                    continue;
                if (shortLine.StartsWith(MemToken.TxtTagClose))
                    continue;

                if (shortLine.StartsWith(TxtToken.ConvertExample) || shortLine.StartsWith(TxtToken.NoCheck))
                {
                    exampleProps.ConvertExample = shortLine.StartsWith(TxtToken.ConvertExample);
                    exampleProps.NoCheck = shortLine.StartsWith(TxtToken.NoCheck);
                    if (m_CurrentState != MemFileState.Example)
                        TerminateChunk(exampleProps, accumulator, MemFileState.ExampleMarkup);
                    continue;
                }

                if (shortLine.StartsWith(TxtToken.BeginEx))
                {
                    if (shortLine.Contains(TxtToken.NoCheck))
                        exampleProps.NoCheck = true;
                    if (m_CurrentState == MemFileState.ExampleMarkup)
                        m_CurrentState = MemFileState.Example;
                    TerminateChunk(accumulator, MemFileState.Example);
                    continue;
                }
                if (shortLine.StartsWith(TxtToken.EndEx))
                {
                    //Assert.AreEqual(MemFileState.Example, m_CurrentState);

                    TerminateChunk(exampleProps, accumulator, DefaultMemFileState);
                    exampleProps.ConvertExample = exampleProps.NoCheck = false;

                    continue;
                }
                if (shortLine.StartsWith(TxtToken.Param))
                {
                    //Assert.AreNotEqual(MemFileState.Example, m_CurrentState);
                    //Assert.AreNotEqual(MemFileState.ExampleMarkup, m_CurrentState);
                    TerminateChunk(accumulator, MemFileState.Param);
                }
                if (shortLine.StartsWith(TxtToken.Return))
                {
                    //Assert.AreNotEqual(MemFileState.ExampleMarkup, m_CurrentState);
                    TerminateChunk(accumulator, MemFileState.Return);
                }

                if (shortLine.StartsWith(TxtToken.CsNone))
                {
                    m_TheBlock.IsDocOnly = true;
                    continue;
                }
                if (shortLine.StartsWith(MemToken.Undoc))
                {
                    m_TheBlock.IsUndoc = true;
                    continue;
                }
                accumulator.AppendUnixLine(line);
            }

            //make sure we're not in the middle of an example when we reached EOF or next signature marker
            //Assert.AreNotEqual(MemFileState.Example, m_CurrentState, "unclosed example detected");
            //Assert.AreNotEqual(MemFileState.Signatures, m_CurrentState, "unclosed signatures block detected");
            TerminateChunk(accumulator, DefaultMemFileState);
        }
    }
}
