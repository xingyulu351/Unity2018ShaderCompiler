using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;

namespace Unity.BuildTools
{
    public static class EnumerableExtensions
    {
        public static IEnumerable<T> Prepend<T>(this IEnumerable<T> collection, T element)
        {
            yield return element;
            foreach (var e in collection)
                yield return e;
        }

        public static IEnumerable<T> Append<T>(this IEnumerable<T> collection, T element)
        {
            foreach (var e in collection)
                yield return e;

            yield return element;
        }

        public static IEnumerable<T> Append<T>(this IEnumerable<T> collection, params T[] elements)
        {
            return collection.Concat(elements);
        }

        public static IEnumerable<T> Exclude<T>(this IEnumerable<T> collection, T element) where T : class
        {
            return collection.Where(e => e != element);
        }

        public static IEnumerable<T> Exclude<T>(this IEnumerable<T> collection, IEnumerable<T> elements) where T : class
        {
            var h = new HashSet<T>(elements);
            return collection.Where(e => !h.Contains(e));
        }

        public static bool Empty<T>(this IEnumerable<T> collection)
        {
            return !collection.Any();
        }

        public static string SeparateWithSpace(this IEnumerable<String> values)
        {
            return values.SeparateWith(" ");
        }

        public static string SeparateWith(this IEnumerable<string> values, string separator)
        {
            return string.Join(separator, values);
        }

        public static string SeparateWithComma(this IEnumerable<String> values)
        {
            return values.SeparateWith(",");
        }

        public static string InQuotes(this string value)
        {
            return "\"" + value + "\"";
        }

        public static IEnumerable<string> InQuotes(this IEnumerable<string> value)
        {
            return value.Select(InQuotes);
        }

        public static int IndexOf<T>(this IEnumerable<T> collection, T element)
        {
            var i = 0;
            foreach (var e in collection)
            {
                if (e.Equals(element))
                    return i;
                i++;
            }
            return -1;
        }

        public static IEnumerable<IEnumerable<T>> Chunked<T>(this IEnumerable<T> collection, int chunkSize)
        {
            var result = new List<T>(chunkSize);
            foreach (var e in collection)
            {
                result.Add(e);

                if (result.Count() == chunkSize)
                {
                    yield return result.ToArray();
                    result.Clear();
                }
            }
            if (result.Any())
                yield return result;
        }

        public static IEnumerable<T> DistinctBy<T, T2>(this IEnumerable<T> @this, Func<T, T2> selector)
        {
            var processed = new HashSet<T2>();
            foreach (var e in @this)
            {
                var by = selector(e);
                if (processed.Contains(by))
                    continue;
                yield return e;
                processed.Add(by);
            }
        }

        public static IEnumerable<T> DistinctKeepLast<T>(this IEnumerable<T> collection)
        {
            return collection.Reverse().Distinct().Reverse();
        }

        public static IEnumerable<T> AsEnumerable<T>(this T value)
        {
            yield return value;
        }

        public static IEnumerable<T> ThrowIfEmpty<T>(this IEnumerable<T> value)
        {
            if (value.Any())
                return value;
            throw new ArgumentException("Expected a non empty collection");
        }

        public static IEnumerable<T> ForEach<T>(this IEnumerable<T> @this, Action<T> action)
        {
            var items = @this.ToList();
            foreach (var e in items)
                action(e);
            return items;
        }

        //Having an .Add that takes multiple elements, allows using multiple elements in collection initializers, which feels very nice
        public static void Add<T>(this List<T> list, IEnumerable<T> elements)
        {
            list.AddRange(elements);
        }

        public static IEnumerable<T> CatchExceptions<T>(this IEnumerable<T> @this, Action<Exception> action = null)
        {
            using (var enumerator = @this.GetEnumerator())
            {
                bool next = true;

                while (next)
                {
                    try
                    {
                        next = enumerator.MoveNext();
                    }
                    catch (Exception ex)
                    {
                        action?.Invoke(ex);
                        continue;
                    }

                    if (next)
                        yield return enumerator.Current;
                }
            }
        }

        public static void FilterByBool<T>(this IEnumerable<T> elements, Func<T, bool> predicate, out List<T> matching, out List<T> notmatching)
        {
            matching = new List<T>();
            notmatching = new List<T>();
            foreach (var e in elements)
            {
                if (predicate(e))
                    matching.Add(e);
                else
                    notmatching.Add(e);
            }
        }
    }
}

namespace Bee.Tools
{
    public static class IEnumerableByteExtensions
    {
        /// <summary>
        /// Convert this buffer to a hexadecimal string (lowercase, no separators).
        /// </summary>
        public static string ToHexString(this IEnumerable<byte> bytes)
        {
            var sb = new StringBuilder();
            foreach (var b in bytes)
                sb.Append(b.ToString("x2"));
            return sb.ToString();
        }
    }
}
