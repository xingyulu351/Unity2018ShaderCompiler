using System;

namespace Unity.BuildTools
{
    public static class LazyExtensions
    {
        public static T EnsureValueIsCreated<T>(this Lazy<T> lazy) where T : class
        {
            return lazy.Value;
        }
    }
}
