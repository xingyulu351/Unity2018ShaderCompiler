using System.Collections.Generic;

namespace Unity.BindingsGenerator.Core
{
    public static class GlobalContext
    {
        private static readonly List<string> Defines = new List<string>();

        public static bool HasDefine(string define)
        {
            return Defines.Contains(define);
        }

        public static void SetDefine(string define)
        {
            if (!Defines.Contains(define))
                Defines.Add(define);
        }

        public static void UnsetDefine(string define)
        {
            Defines.RemoveAll(s => s == define);
        }
    }
}
