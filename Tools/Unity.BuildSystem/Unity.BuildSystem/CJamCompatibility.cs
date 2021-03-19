using System;
using Unity.BuildSystem.NativeProgramSupport;
using Unity.BuildTools;

namespace Unity.BuildSystem
{
    public class CJamCompatibility
    {
        public static CodeGen CodeGenFromString(string s)
        {
            // some console platforms still use legacy config names (e.g. "debugil2cpp"), so do string prefix checks
            if (s.StartsWith("debug")) return CodeGen.Debug;
            if (s.StartsWith("release")) return CodeGen.Release;
            if (s.StartsWith("develop")) return CodeGen.Release; // note: only used on Switch, means kinda the same as "Release"
            if (s.StartsWith("master")) return CodeGen.Master;
            Errors.Exit($"Unknown CONFIG value: '{s}'");
            return CodeGen.Debug;
        }

        public static ToolChain CreateToolChainEmulatingV1BuildCode(string platform)
        {
            if (platform == null)
                throw new ArgumentNullException();
            return V1BuildCodeEmulation.CreateToolChainEmulatingV1BuildCode(platform);
        }
    }
}
