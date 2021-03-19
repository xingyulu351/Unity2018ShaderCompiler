using System.Text.RegularExpressions;

namespace Cs2us.Tests
{
    static class StringUtils
    {
        //replaces any number of consecutive whitespace characters with one whitespace
        //(to ignore tabs vs. spaces issues)
        public static string CollapseWhitespace(this string self)
        {
            return Regex.Replace(self, @"\s+", " ");
        }
    }
}
