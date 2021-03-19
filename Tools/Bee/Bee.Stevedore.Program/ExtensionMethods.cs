using System.Collections;
using System.Collections.Generic;
using System.Text;

namespace Bee.Stevedore.Program
{
    public static class IDictionaryExtensions
    {
        /// <summary>
        /// Gets the value associated with the specified key and returns true,
        /// if the key exists. Otherwise gets default(T) and returns false.
        /// </summary>
        public static bool TryGetValue<T>(this IDictionary dictionary, object key, out T value)
        {
            // Because MS never got around to updating their old APIs after adding generics...

            if (dictionary.Contains(key))
            {
                value = (T)dictionary[key];
                return true;
            }

            value = default(T);
            return false;
        }
    }
}
