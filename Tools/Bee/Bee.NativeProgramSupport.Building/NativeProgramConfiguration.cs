using System;
using System.Collections.Generic;
using System.Linq;
using Unity.BuildSystem.NativeProgramSupport;
using Unity.BuildTools;

namespace Unity.BuildSystem.NativeProgramSupport
{
    public class NativeProgramConfiguration
    {
        public NativeProgramConfiguration(CodeGen codeGen, ToolChain toolChain, bool lump)
        {
            CodeGen = codeGen;
            ToolChain = toolChain;
            Lump = lump;
        }

        public CodeGen CodeGen { get; private set; }
        public ToolChain ToolChain { get; private set; }
        public bool Lump { get; private set; }
        public Platform Platform => ToolChain.Platform;

        public string PlatformName => ToolChain.LegacyPlatformIdentifier;
        public CodeGen Config => CodeGen;

        public virtual string JamArgs =>
            $"-sCONFIG={CodeGen.ToJam()} -sPLATFORM={ToolChain.LegacyPlatformIdentifier} -sLUMP={(Lump ? "1" : "0")}";

        public virtual string Identifier =>
            $"{CodeGen.ToJam()}_{ToolChain.LegacyPlatformIdentifier}{(Lump ? "" : "_nonlump")}";

        public virtual string IdentifierNoPlatform => $"{CodeGen.ToJam()}{(Lump ? "" : "_nonlump")}";

        public virtual string IdentifierForProjectFile => Identifier;

        public override string ToString()
        {
            return Identifier;
        }

        public NativeProgramConfiguration WithCodeGen(CodeGen codeGen)
        {
            var result = (NativeProgramConfiguration)MemberwiseClone();
            result.CodeGen = codeGen;
            return result;
        }

        public NativeProgramConfiguration WithLump(bool lump)
        {
            var result = (NativeProgramConfiguration)MemberwiseClone();
            result.Lump = lump;
            return result;
        }

        public static bool IsWindows(NativeProgramConfiguration c)
        {
            return c.ToolChain.Platform is WindowsPlatform;
        }

        public static bool IsWindows32(NativeProgramConfiguration c)
        {
            return c.ToolChain.Platform is WindowsPlatform && c.ToolChain.Architecture is x86Architecture;
        }

        public static bool IsWindows64(NativeProgramConfiguration c)
        {
            return c.ToolChain.Platform is WindowsPlatform && c.ToolChain.Architecture is x64Architecture;
        }

        public static bool IsMac(NativeProgramConfiguration c)
        {
            return c.ToolChain.Platform is MacOSXPlatform;
        }

        public static bool IsLinux(NativeProgramConfiguration c)
        {
            return c.ToolChain.Platform is LinuxPlatform;
        }

        public static bool IsLinux32(NativeProgramConfiguration c)
        {
            return IsLinux(c) && c.ToolChain.Architecture is x86Architecture;
        }

        public static bool IsLinux64(NativeProgramConfiguration c)
        {
            return IsLinux(c) && c.ToolChain.Architecture is x64Architecture;
        }

        public static bool IsWebGL(NativeProgramConfiguration c)
        {
            return c.ToolChain.Platform.GetType().Name == "WebGLPlatform";
        }

        public static bool IsAndroid(NativeProgramConfiguration c)
        {
            return c.ToolChain.Platform is AndroidPlatform;
        }

        public static bool IsIosOrTvos(NativeProgramConfiguration c)
        {
            return c.ToolChain.Platform is IosPlatform || c.ToolChain.Platform is TvosPlatform;
        }

        public static bool IsIosOrTvosNoSimulator(NativeProgramConfiguration c)
        {
            return (c.ToolChain.Platform is IosPlatform || c.ToolChain.Platform is TvosPlatform) && c.ToolChain.Architecture is ArmArchitecture;
        }

        public static bool IsIosOrTvosSimulator(NativeProgramConfiguration c)
        {
            return (c.ToolChain.Platform is IosPlatform || c.ToolChain.Platform is TvosPlatform) && c.ToolChain.Architecture is IntelArchitecture;
        }

        public static bool IsUWP(NativeProgramConfiguration c)
        {
            return c.ToolChain.Platform is UniversalWindowsPlatform;
        }

        protected virtual int GetSimilarityScore(string platform, NativeProgramConfiguration other)
        {
            var score = 0;
            // note: platform is not taken from "this", but passed externally and can be different.
            // put more weight into platform matches than most other factors.
            if (platform == other.ToolChain.LegacyPlatformIdentifier)
                score += 5;
            if (Lump == other.Lump)
                score++;
            if (CodeGen == other.CodeGen)
                score++;
            if (ToolChain.Architecture.Bits == other.ToolChain.Architecture.Bits)
                score++;

            // we want "master" and "release" configs to be treated as fairly similar
            var nonDebug = CodeGen != CodeGen.Debug;
            var otherNonDebug = other.CodeGen != CodeGen.Debug;
            if (nonDebug == otherNonDebug)
                score++;

            return score;
        }

        public NativeProgramConfiguration FindBestMatching(string platform,
            IEnumerable<NativeProgramConfiguration> configs)
        {
            // Note: it might be tempting to do an configs.OrderBy(c => cfg.GetSimilarityScore(platform, c)).Last()
            // instead of this function, but that would not guarantee that in presence of configs with the same
            // similarity score, the first one on the original list would be returned.
            NativeProgramConfiguration bestConfig = null;
            var bestScore = -1;
            foreach (var c in configs)
            {
                var score = GetSimilarityScore(platform, c);
                if (score > bestScore)
                {
                    bestScore = score;
                    bestConfig = c;
                }
            }

            return bestConfig;
        }
    }
}
