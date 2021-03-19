using System;

namespace Unity.BuildTools
{
    public static class StringExtensions
    {
        public static string StripEnd(this string input, string thingToStrip)
        {
            if (!input.EndsWith(thingToStrip))
                throw new ArgumentException($"String {input} does not end with {thingToStrip}");
            return input.Substring(0, input.Length - thingToStrip.Length);
        }

        public static string StripStart(this string input, string thingToStrip)
        {
            if (!input.StartsWith(thingToStrip))
                throw new ArgumentException($"String {input} does not start with {thingToStrip}");
            return input.Substring(thingToStrip.Length);
        }
    }
}
