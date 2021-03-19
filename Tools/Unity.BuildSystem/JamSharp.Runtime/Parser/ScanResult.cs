using System;
using System.Linq;
using System.Text;

namespace JamSharp.Runtime.Parser
{
    public class ScanResult
    {
        public ScanToken[] ScanTokens { get; }
        public int _cursor;

        public ScanResult(ScanToken[] scanTokens)
        {
            if (scanTokens.Length == 0)
                throw new ArgumentException("scanTokens shouldn't be empty");
            if (scanTokens.Last().tokenType != TokenType.EOF)
                throw new ArgumentException("last token needs to be EOF");

            ScanTokens = scanTokens;
        }

        public bool IsAtEnd => _cursor >= ScanTokens.Length;


        public ScanToken Next(bool skipWhiteSpace = true)
        {
            if (IsAtEnd)
                throw new InvalidOperationException("Reached end of stream while expecting more tokens");

            if (ScanTokens[_cursor].tokenType == TokenType.WhiteSpace && skipWhiteSpace)
            {
                _cursor++;
                return Next();
            }

            return ScanTokens[_cursor++];
        }

        public ScanToken Peek(bool skipWhiteSpace = true)
        {
            if (IsAtEnd)
                throw new InvalidOperationException();

            int tempCursor = _cursor;
            while (ScanTokens[tempCursor].tokenType == TokenType.WhiteSpace && skipWhiteSpace)
            {
                tempCursor++;
            }

            return ScanTokens[tempCursor];
        }

        public int GetCursor()
        {
            return _cursor;
        }

        public void SetCursor(int cursor)
        {
            _cursor = cursor;
        }

        public string ProduceStringUntilEndOfLine()
        {
            var sb = new StringBuilder();
            while (true)
            {
                var token = Peek(false);
                if (token.tokenType == TokenType.WhiteSpace && Scanner.IsNewLineCharacter(token.literal[0]))
                {
                    Next(false);
                    return sb.ToString();
                }
                if (token.tokenType == TokenType.EOF)
                    return sb.ToString();

                sb.Append(Next(false).literal);
            }
        }
    }
}
