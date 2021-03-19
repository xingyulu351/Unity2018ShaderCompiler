using System.Collections.Generic;
using System.Linq;

namespace UnityBindingsParser
{
    public static class EnumerableExtensions
    {
        public static IEnumerable<T> Prepend<T>(this IEnumerable<T> items, T prepended)
        {
            return new[] {prepended}.Concat(items);
        }

        public static string CommaSeparatedList(this IEnumerable<string> items)
        {
            return string.Join(", ", items.ToArray());
        }
    }
}
