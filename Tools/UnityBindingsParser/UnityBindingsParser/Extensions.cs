using System;
using System.Collections.Generic;
using System.Text;
using System.Text.RegularExpressions;

namespace UnityBindingsParser
{
    internal static class Extensions
    {
        public static bool IsEmpty(this string self)
        {
            return string.IsNullOrEmpty(self);
        }

        public static string ConsumeAndTrim(this List<string> builder)
        {
            var result = new StringBuilder();
            foreach (var line in builder)
            {
                if (line.Trim().IsEmpty())
                    continue;
                result.AppendLine(line);
            }
            builder.Clear();
            return result.ToString().Trim();
        }

        public static string WithUnixLineEndings(this string self)
        {
            return self.Replace("\r\n", "\n");
        }

        public static string WithNativeLineEndings(this string self)
        {
            return self.Replace("\r\n", Environment.NewLine);
        }

        public static string[] SplitLines(this string self)
        {
            return Regex.Split(self, @"\r?\n");
        }

        public static string TrimEndAndNewlines(this string self)
        {
            var result = self.TrimStart('\n');
            return result.TrimEnd();
        }
    }
}
