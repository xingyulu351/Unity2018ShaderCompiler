using System;
using System.Text.RegularExpressions;

namespace ScriptRefBase
{
    internal class LinkRegexHelper
    {
        internal static Regex FormRegexDouble(string openPattern, string closePattern)
        {
            var firstCharClosePattern = closePattern[0];
            return new Regex(String.Format(@"{0}{0}[^{2}]+{1}{1}", openPattern, closePattern, firstCharClosePattern));
        }

        internal static Regex FormRegexEscapedClosingPattern(string openPattern, string closePattern)
        {
            var firstCharClosePattern = FirstCharClosePattern(closePattern);
            return new Regex(String.Format(@"{0}[^\{2}]+{1}", openPattern, closePattern, firstCharClosePattern));
        }

        public static Regex FormSingleRegex(string openPattern, string closePattern)
        {
            var firstCharClosePattern = FirstCharClosePattern(closePattern);
            return new Regex(String.Format(@"{0}[^{1}]+{2}", openPattern, firstCharClosePattern, closePattern));
        }

        //the open and close patterns are taken as is
        internal static Regex FormSingleRegexWithSeparator(string openPattern, string closePattern, string separator)
        {
            var firstCharClosePattern = FirstCharClosePattern(closePattern);
            return new Regex(String.Format(@"{0}[^{1}]+{2}[^{1}]+{3}", openPattern, firstCharClosePattern, separator, closePattern));
        }

        //the open and close patterns are doubles, e.g. [ => [[, ]=>]]
        internal static Regex FormDoubleRegexWithSeparator(string openPattern, string closePattern, string separator)
        {
            var firstCharClosePattern = FirstCharClosePattern(closePattern);
            return new Regex(String.Format(@"{0}{0}[^{1}]+{2}[^{1}]+{3}{3}", openPattern, firstCharClosePattern, separator, closePattern));
        }

        private static char FirstCharClosePattern(string closePattern)
        {
            return closePattern[0] != '\\' ? closePattern[0] : closePattern[1];
        }

        public static IMemDocElement ResolverDoubleCharStyle(string arg, char doubleChar, StyleType st)
        {
            var name = arg.TrimStart(doubleChar).TrimEnd(doubleChar);
            return new MemDocElementStyle(name, st);
        }
    }
}
