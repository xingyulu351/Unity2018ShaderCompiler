using System.Text.RegularExpressions;

namespace UnityBindingsParser
{
    public static class ParserUtil
    {
        internal static readonly Regex DocumentationMarker = new Regex(@"^\s*///\s*(.*)\s*");
        internal static readonly Regex SimpleCommentMarker = new Regex(@"^\s*//[^/]\s*(.*)\s*");
        internal static readonly Regex DocumentationFile   = new Regex(@"\s*DOCFILE\s*(.*)\s*");
        internal static readonly Regex EndExMarker = CreateMarker("END EX");
        internal static readonly Regex AttributeMarker = new Regex(@"^\s*\[(.*)\]\s*$");

        public static Regex CreateMarker(string marker)
        {
            return new Regex(@"^\s*" + Regex.Escape(marker) + @"\b\s*(.*)");
        }

        public static string TextAfterMarkerMatch(Match match)
        {
            return match.Groups[1].Value;
        }
    }
}
