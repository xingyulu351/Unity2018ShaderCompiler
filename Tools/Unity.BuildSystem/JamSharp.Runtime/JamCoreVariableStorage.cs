using System;
using Unity.TinyProfiling;

namespace JamSharp.Runtime
{
    class JamCoreVariableStorage
    {
        public static string[] AllGlobalVariables => JamCore.InvokeRule("GetAllVariableNames", new string[0][]);

        public static string[][] GetGlobalVariables(string[] names)
        {
            using (TinyProfiler.Section("JamCoreVariableStorage.GetGlobalVariables"))
            {
                return JamCore.GetVarBatch(names);
            }
        }

        public static void SetGlobalVariables(string[] names, string[][] values)
        {
            using (TinyProfiler.Section("JamCoreVariableStorage.SetGlobalVariables"))
            {
                JamCore.SetVarBatch(names, Operator.VAR_SET, values);
            }
        }
    }
}
