using System;
using System.Collections.Generic;
using System.Diagnostics;
using System.Linq;
using System.Text;

namespace JamSharp.Runtime.Parser
{
    public class Scanner
    {
        static readonly char[] s_NewLineCharacters = { '\n', '\r'};
        readonly string _input;
        readonly ParserMode _parserMode;

        readonly StringBuilder _builder = new StringBuilder();
        bool _enterringActions;
        int _insideVariableExpansionDepth;
        bool _insideVariableExpansionModifierSpan;

        readonly List<ScanToken> _previouslyScannedTokens = new List<ScanToken>();
        bool isInsideQuote;
        int nextChar;

        public Scanner(string input, ParserMode parserMode = ParserMode.Normal)
        {
            _input = input;
            _parserMode = parserMode;
        }

        public ScanResult Scan()
        {
            return new ScanResult(ScanAllTokens().ToArray());
        }

        public IEnumerable<ScanToken> ScanAllTokens()
        {
            while (true)
            {
                var sr = ScanToken();

                if (sr.tokenType == TokenType.Literal)
                    sr.literal = sr.literal.Replace("\r\n", "\n");

                yield return sr;
                if (sr.tokenType == TokenType.EOF)
                    yield break;
            }
        }

        public ScanToken ScanToken()
        {
            var scanToken = ScanTokenImpl();
            _previouslyScannedTokens.Add(scanToken);
            return scanToken;
        }

        ScanToken ScanTokenImpl()
        {
            if (nextChar >= _input.Length)
                return new ScanToken { literal = "", tokenType = TokenType.EOF , textLocation = _input.Length};

            var c = _input[nextChar];

            if (_enterringActions)
            {
                if (_previouslyScannedTokens.Last().tokenType == TokenType.AccoladeOpen)
                {
                    var sb = new StringBuilder();
                    var textLocation = nextChar;
                    while (true)
                    {
                        nextChar++;
                        if (_input[nextChar] == '}')
                            break;
                        sb.Append(_input[nextChar]);
                    }
                    _enterringActions = false;
                    return new ScanToken { literal = sb.ToString(), tokenType = TokenType.Literal, textLocation = textLocation };
                }
            }

            if (_insideVariableExpansionModifierSpan)
            {
                if (c == '=')
                {
                    _insideVariableExpansionModifierSpan = false;

                    var scanToken = new ScanToken { tokenType = TokenType.Assignment, literal = c.ToString() , textLocation = nextChar};
                    nextChar++;
                    return scanToken;
                }
                if (char.IsLetter(c) || c == '\\' || c == '/')
                {
                    var scanToken = new ScanToken { tokenType = TokenType.VariableExpansionModifier, literal = c.ToString(), textLocation = nextChar};
                    nextChar++;
                    return scanToken;
                }
                if (c == ')')
                    _insideVariableExpansionModifierSpan = false;
            }

            if (!isInsideQuote && char.IsWhiteSpace(c))
                return new ScanToken { tokenType = TokenType.WhiteSpace, textLocation = nextChar, literal = ReadWhiteSpace().Replace("\r", "") };

            bool hasMoreCharacters = nextChar + 1 < _input.Length;
            if (c == '$' && hasMoreCharacters && _input[nextChar + 1] == '(')
            {
                ++_insideVariableExpansionDepth;
                var scanToken = new ScanToken { tokenType = TokenType.VariableDereferencerOpen, literal = "$(" , textLocation = nextChar};
                nextChar += 2;
                return scanToken;
            }

            if (c == '@' && hasMoreCharacters && _input[nextChar + 1] == '(')
            {
                ++_insideVariableExpansionDepth;
                var scanTokenImpl = new ScanToken { tokenType = TokenType.LiteralExpansionOpen, literal = "@(", textLocation = nextChar};
                nextChar += 2;
                return scanTokenImpl;
            }

            if (c == ')')
            {
                bool processAsParenthesisClose = false;
                if (_insideVariableExpansionDepth > 0)
                {
                    --_insideVariableExpansionDepth;
                    processAsParenthesisClose = true;
                }

                if (_previouslyScannedTokens.Any() && _previouslyScannedTokens.Last().tokenType == TokenType.WhiteSpace && NextCharIfAnyIsWhiteSpace())
                    processAsParenthesisClose = true;

                if (processAsParenthesisClose)
                {
                    var scanTokenImpl = new ScanToken { tokenType = TokenType.ParenthesisClose, literal = ")", textLocation = nextChar };
                    ++nextChar;
                    return scanTokenImpl;
                }
            }

            if (c == '#')
            {
                var oldPos = nextChar++;
                var scanTokenImpl = new ScanToken { tokenType = TokenType.Comment, textLocation = oldPos, literal = ReadUntilEndOfLine() };
                return scanTokenImpl;
            }

            ////FIXME: do this properly; ReadLiteral should not recognize what isn't a literal

            var oldPosition = nextChar;

            var literal = ReadLiteral(_insideVariableExpansionDepth == 0);

            var isUnquotedLiteral = nextChar - oldPosition == literal.Length;
            if (isUnquotedLiteral && literal == ":" && nextChar < _input.Length && !char.IsWhiteSpace(_input[nextChar]) && _insideVariableExpansionDepth > 0)
                _insideVariableExpansionModifierSpan = true;

            var tokenType = TokenType.Literal;
            if (isUnquotedLiteral)
            {
                bool hasWhitespaceBefore = oldPosition == 0 || char.IsWhiteSpace(_input[oldPosition - 1]);
                bool hasWhitespaceAfter = nextChar == _input.Length || char.IsWhiteSpace(_input[nextChar]);

                tokenType = TokenTypeFor(literal, hasWhitespaceAfter && hasWhitespaceBefore);
            }

            if (tokenType == TokenType.Actions)
                _enterringActions = true;
            return new ScanToken { tokenType = tokenType, literal = literal, textLocation = oldPosition };
        }

        bool NextCharIfAnyIsWhiteSpace()
        {
            if (nextChar + 1 >= _input.Length)
                return true;
            return char.IsWhiteSpace(_input[nextChar + 1]);
        }

        TokenType TokenTypeFor(string literal, bool hasWhitespaceAround)
        {
            switch (literal)
            {
                case ";":
                    if (hasWhitespaceAround)
                        return TokenType.Terminator;
                    break;
                case "[":
                    return TokenType.BracketOpen;
                case "]":
                    return TokenType.BracketClose;
                case ":":
                    if (_insideVariableExpansionDepth > 0 || hasWhitespaceAround)
                        return TokenType.Colon;
                    break;
                case "{":
                    if (hasWhitespaceAround)
                        return TokenType.AccoladeOpen;
                    break;
                case "}":
                    if (hasWhitespaceAround)
                        return TokenType.AccoladeClose;
                    break;
                case "=":
                    if (_insideVariableExpansionModifierSpan || hasWhitespaceAround)
                        return TokenType.Assignment;
                    break;
                case "if":
                    return TokenType.If;
                case "rule":
                    return TokenType.Rule;
                case "return":
                    return TokenType.Return;
                case "actions":
                    return TokenType.Actions;
                case "include":
                case "Include":
                    return TokenType.Include;
                case "on":
                    return TokenType.On;
                case "!":
                    return TokenType.Not;
                case "else":
                    return TokenType.Else;
                case "while":
                    return TokenType.While;
                case "+=":
                    return TokenType.AppendOperator;
                case "-=":
                    return TokenType.SubtractOperator;
                case "?=":
                    return TokenType.AssignmentIfEmpty;
                case "<":
                    if (hasWhitespaceAround)
                        return TokenType.LessThan;
                    break;
                case ">":
                    if (hasWhitespaceAround)
                        return TokenType.GreaterThan;
                    break;
                case ">=":
                    return TokenType.GreaterThanOrEqual;
                case "<=":
                    return TokenType.LessThanOrEqual;
                case "for":
                    return TokenType.For;
                case "in":
                    return TokenType.In;
                case "continue":
                    return TokenType.Continue;
                case "break":
                    return TokenType.Break;
                case "case":
                    return TokenType.Case;
                case "switch":
                    return TokenType.Switch;
                case "local":
                    return TokenType.Local;
                case "&&":
                    return TokenType.And;
                case "||":
                    return TokenType.Or;
                case "!=":
                    return TokenType.NotEqual;
            }
            return TokenType.Literal;
        }

        string ReadLiteral(bool allowColon)
        {
            int i;

            for (i = nextChar; i != _input.Length; i++)
            {
                //dont allow colons as the first character
                bool reallyAllowCon = allowColon; // && nextChar != i;
                bool hasMoreCharacters = i + 1 < _input.Length;
                char ch = _input[i];
                if (ch == '\\' && hasMoreCharacters && _parserMode == ParserMode.Normal)
                {
                    ++i;
                    if ((_input[i] == '@' || _input[i] == '$') && i + 1 < _input.Length && _input[i + 1] == '(')
                        break;

                    _builder.Append(_input[i]);
                }
                else if ((ch == '$' || ch == '@') && hasMoreCharacters && _input[i + 1] == '(')
                {
                    Debug.Assert(i > nextChar);
                    break;
                }
                else if (ch == ')' && _insideVariableExpansionDepth > 0)
                {
                    Debug.Assert(i > nextChar);
                    break;
                }
                else if (isInsideQuote && (_insideVariableExpansionDepth == 0 || char.IsWhiteSpace(ch)))
                {
                    if (ch == '"')
                        isInsideQuote = false;
                    else
                        _builder.Append(ch);
                }
                else if (ch == '"' && _parserMode == ParserMode.Normal)
                {
                    isInsideQuote = !isInsideQuote;
                }
                else if (IsLiteral(ch, reallyAllowCon) || ((ch == '$' || ch == '@') && hasMoreCharacters && _input[i + 1] != '('))   // Prevent single $ inside literal being treated as DereferenceVariable token.
                {
                    _builder.Append(ch);
                }
                else
                {
                    break;
                }
            }

            // Return special characters recognized by TokenForLiteral from here
            // even though that doesn't make any sense.
            if (i == nextChar)
            {
                nextChar++;
                return _input[i].ToString();
            }

            var result = _builder.ToString();
            _builder.Clear();

            nextChar = i;
            return result;
        }

        bool IsLiteral(char c, bool treatColonAsLiteral)
        {
            if (c == '}')
                return false;
            if (c == '{')
                return false;
            if (c == ':')
                return treatColonAsLiteral;
            if (c == '[')
                return false;
            if (c == ']')
                return false;
            if (c == '#')
                return false;

            return !char.IsWhiteSpace(c);
        }

        string ReadWhiteSpace()
        {
            bool? wasPreviousWhiteSpaceNewLine = null;
            for (int i = nextChar; i != _input.Length; i++)
            {
                bool isNewLine = IsNewLineCharacter(_input[i]);

                if (!char.IsWhiteSpace(_input[i]) || (wasPreviousWhiteSpaceNewLine.HasValue && isNewLine != wasPreviousWhiteSpaceNewLine.Value))
                {
                    var result = _input.Substring(nextChar, i - nextChar);
                    nextChar = i;
                    return result;
                }
                wasPreviousWhiteSpaceNewLine = isNewLine;
            }
            var result2 = _input.Substring(nextChar);
            nextChar = _input.Length;
            return result2;
        }

        string ReadUntilEndOfLine()
        {
            bool inNewLineSequence = false;
            var originalNextChar = nextChar;
            for (int i = nextChar; i != _input.Length; i++)
            {
                var c = _input[i];
                if (IsNewLineCharacter(c))
                    inNewLineSequence = true;
                else
                {
                    if (inNewLineSequence)
                    {
                        nextChar = i;
                        return _input.Substring(originalNextChar, nextChar - originalNextChar - 1);
                    }
                }
            }
            nextChar = _input.Length;
            return _input.Substring(originalNextChar, nextChar - originalNextChar - 1);
        }

        public static bool IsNewLineCharacter(char c)
        {
            return s_NewLineCharacters.Contains(c);
        }
    }

    public class ScanToken : IEquatable<ScanToken>
    {
        public string literal;
        public TokenType tokenType;
        public int textLocation;

        public bool Equals(ScanToken other)
        {
            return other != null && other.literal == literal && other.tokenType == tokenType;
        }

        public ScanToken Is(TokenType tokenType)
        {
            if (this.tokenType != tokenType)
                throw new ParsingException($"Expected token {tokenType}, but got {this.tokenType}");
            return this;
        }

        public override string ToString()
        {
            return tokenType.ToString() + ": " + literal;
        }
    }

    public enum TokenType
    {
        Literal,
        Terminator,
        WhiteSpace,
        BracketClose,
        Colon,
        BracketOpen,
        VariableDereferencerOpen,
        LiteralExpansionOpen,
        ParenthesisClose,
        Assignment,
        AccoladeOpen,
        AccoladeClose,
        If,
        Rule,
        VariableExpansionModifier,
        Return,
        AppendOperator,
        Actions,
        On,
        EOF,
        Not,
        Else,
        While,
        SubtractOperator,
        For,
        In,
        Continue,
        Break,
        Switch,
        Case,
        Local,
        AssignmentIfEmpty,
        And,
        Or,
        NotEqual,
        Include,
        GreaterThan,
        LessThan,
        LessThanOrEqual,
        GreaterThanOrEqual,
        Comment,
    }
}
