using System;

namespace Unity.CommonTools.JsonWrapper
{
    public class Json
    {
        public static object Deserialize(string json)
        {
            return UnityEditor.Json.Deserialize(json);
        }

        public static string Serialize(object obj)
        {
            return UnityEditor.Json.Serialize(obj);
        }
    }
}
