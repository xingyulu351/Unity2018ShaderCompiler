using System;
using System.Collections.Generic;
using System.IO;
using Unity.PackageManager.Ivy;

#if STANDALONE
namespace UnityEditorInternal
#else
namespace Unity.PackageManager.Internal.BackingStore
#endif
{
    internal static class InternalEditorUtility
    {
        public static void SetupCustomDll(string dllName, string dllLocation)
        {
        }

        public static bool inBatchMode { get { return false; } }
    }
}

#if STANDALONE
namespace UnityEditor
#else
namespace Unity.PackageManager.Internal.BackingStore
#endif
{
    internal class EditorPrefs
    {
        static Dictionary<string, object> store = new Dictionary<string, object>();

        static void SetValue(string key, object value)
        {
            if (!store.ContainsKey(key))
                store.Add(key, value);
            else
                store[key] = value;
        }

        static T GetValue<T>(string key)
        {
            if (store.ContainsKey(key))
                return (T)store[key];
            return default(T);
        }

        public static void SetInt(string key, int value)
        {
            SetValue(key, value);
        }

        public static int GetInt(string key)
        {
            return GetValue<int>(key);
        }

        public static void SetString(string key, string value)
        {
            SetValue(key, value);
        }

        public static string GetString(string key)
        {
            return GetValue<string>(key);
        }
    }
}
