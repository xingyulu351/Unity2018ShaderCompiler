using System;
using System.Linq;
using NUnit.Framework;

namespace Unity.BindingsGenerator.TestFramework
{
    public class AssertingStringMatcher
    {
        private string _str;
        private int _pos;

        public AssertingStringMatcher(string str)
        {
            _str = str;
            _pos = 0;
        }

        public AssertingStringMatcher Find(string str)
        {
            var i = _str.IndexOf(str, _pos, StringComparison.Ordinal);

            if (i == -1)
            {
                Assert.Fail(String.Format("Expected '{0}' in '{1}'", str, _str.Substring(_pos)));
            }

            _pos = i + str.Length;

            return this;
        }

        public void End()
        {
            if (_pos != _str.Length)
                Assert.Fail(String.Format("Expected EOF in '{0}'", _str.Substring(_pos)));
        }

        private static bool IsWhiteSpace(char c)
        {
            return " \t\n\r".Contains(c);
        }

        public AssertingStringMatcher OptionalWhitespace()
        {
            while (_pos < _str.Length && IsWhiteSpace(_str[_pos]))
                _pos++;

            return this;
        }

        public AssertingStringMatcher ExpectWhitespace()
        {
            if (!IsWhiteSpace(_str[_pos]))
                Assert.Fail(String.Format("Expected whitespace, but found '{0}'", _str.Substring(_pos)));

            return OptionalWhitespace();
        }

        public AssertingStringMatcher Expect(string str)
        {
            if (_str.IndexOf(str, _pos) != _pos)
                Assert.Fail(String.Format("Expected '{0}', but found '{1}'", str, _str.Substring(_pos)));

            _pos += str.Length;

            return this;
        }
    }
}
