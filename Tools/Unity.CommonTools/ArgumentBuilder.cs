using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Text.RegularExpressions;

namespace Unity.CommonTools
{
    // https://blogs.msdn.microsoft.com/twistylittlepassagesallalike/2011/04/23/everyone-quotes-command-line-arguments-the-wrong-way/
    public class ArgumentBuilder
    {
        private static readonly Regex s_QuotingNeeded = new Regex(@"([\(\)\%!\^\""<>&\|]|\s)", RegexOptions.Compiled);

        public string CreateArgumentString(params string[] arguments)
        {
            return CreateArgumentString((IEnumerable<string>)arguments);
        }

        public string CreateArgumentString(IEnumerable<string> arguments)
        {
            Ensure.IsNotNull(arguments, nameof(arguments));

            return string.Join(" ", QuoteArguments(arguments).ToArray());
        }

        public string[] QuoteArguments(params string[] arguments)
        {
            return QuoteArguments((IEnumerable<string>)arguments).ToArray();
        }

        public IEnumerable<string> QuoteArguments(IEnumerable<string> arguments)
        {
            Ensure.IsNotNull(arguments, nameof(arguments));

            return arguments.Select(QuoteArgument);
        }

        public string QuoteArgument(string argument)
        {
            Ensure.IsNotNull(argument, nameof(argument));

            if (!s_QuotingNeeded.IsMatch(argument))
            {
                return argument;
            }

            var backslashCount = 0;
            var quotedArgument = new StringBuilder(argument.Length * 2);
            quotedArgument.Append('"');
            foreach (var c in argument)
            {
                if (c == '\\')
                {
                    backslashCount++;
                }
                else if (c == '"')
                {
                    quotedArgument.Append(new string('\\', backslashCount * 2 + 1));
                    quotedArgument.Append(c);
                    backslashCount = 0;
                }
                else if (backslashCount > 0)
                {
                    quotedArgument.Append(new string('\\', backslashCount));
                    quotedArgument.Append(c);
                    backslashCount = 0;
                }
                else
                {
                    quotedArgument.Append(c);
                }
            }

            if (backslashCount > 0)
            {
                quotedArgument.Append(new string('\\', backslashCount * 2));
            }
            quotedArgument.Append('"');

            return quotedArgument.ToString();
        }
    }
}
