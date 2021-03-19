using System;
using System.Collections.Generic;
using System.IO;
using System.Linq;

namespace Bee.Why
{
    internal static class TokenDiffer
    {
        internal class Token
        {
            private string _source;
            private readonly int _start;

            public Token(string source, int start, int length)
            {
                _source = source;
                _start = start;
                Length = length;
            }

            public string Value => _source.Substring(_start, Length);

            public ConsoleColor Color { get; set; } = ConsoleColor.Gray;
            public int Index { get; set; }
            public bool Matched { get; set; }

            public int Length { get; }
        }

        private static IEnumerable<Token> Tokenize(string str)
        {
            int tokenStart = -1;
            while (tokenStart < str.Length)
            {
                // Skip whitespace
                do
                    ++tokenStart;
                while (tokenStart < str.Length && char.IsWhiteSpace(str[tokenStart]));

                // Scan to next whitespace
                var tokenEnd = tokenStart;
                while (tokenEnd < str.Length && !char.IsWhiteSpace(str[tokenEnd]))
                {
                    if (str[tokenEnd] == '"')
                    {
                        // Scan to the end of quoted strings
                        ++tokenEnd;
                        while (tokenEnd < str.Length && str[tokenEnd] != '"')
                        {
                            // Skip escaped characters
                            if (str[tokenEnd] == '\\')
                                ++tokenEnd;
                            ++tokenEnd;
                        }
                    }

                    ++tokenEnd;
                }

                if (tokenStart < str.Length)
                    yield return new Token(str, tokenStart, tokenEnd - tokenStart);

                tokenStart = tokenEnd;
            }
        }

        private static void PaintMissingTokensAndIndex(Token[] tokens, Token[] compareTo, ConsoleColor color)
        {
            int nextIndex = 0;
            foreach (var token in tokens)
            {
                var matchingToken = compareTo.FirstOrDefault(t => t.Value == token.Value && !t.Matched);
                if (matchingToken == null)
                {
                    token.Color = color;
                    token.Index = -1;
                }
                else
                {
                    token.Index = nextIndex++;
                    matchingToken.Matched = true;
                }
            }
        }

        private static void PaintReorderedTokens(Token[] tokens, Token[] compareTo, ConsoleColor color)
        {
            foreach (var token in tokens.Where(t => t.Index != -1))
                if (compareTo.First(t => t.Index == token.Index).Value != token.Value)
                    token.Color = color;
        }

        public static void Diff(string oldValue, string newValue, out Token[] oldTokens, out Token[] newTokens)
        {
            if (oldValue == newValue)
                throw new ArgumentException("Strings are identical!");

            oldTokens = Tokenize(oldValue).ToArray();
            newTokens = Tokenize(newValue).ToArray();

            if (oldTokens.Length == newTokens.Length &&
                oldTokens.Zip(newTokens, (o, n) => o.Value == n.Value).All(b => b))
            {
                // The token lists appear to be identical, so the difference must be whitespace
                throw new InvalidDataException();
            }

            PaintMissingTokensAndIndex(oldTokens, compareTo: newTokens, color: ConsoleColor.Red);
            PaintMissingTokensAndIndex(newTokens, compareTo: oldTokens, color: ConsoleColor.Green);

            PaintReorderedTokens(oldTokens, compareTo: newTokens, color: ConsoleColor.Yellow);
            PaintReorderedTokens(newTokens, compareTo: oldTokens, color: ConsoleColor.Yellow);
        }
    }
}
