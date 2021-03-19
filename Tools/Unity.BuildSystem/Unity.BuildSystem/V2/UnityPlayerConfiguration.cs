using System;
using System.Linq;
using Unity.BuildSystem.NativeProgramSupport;

namespace Unity.BuildSystem
{
    public class UnityPlayerConfiguration : NativeProgramConfiguration
    {
        public ScriptingBackend ScriptingBackend { get; }
        public bool DevelopmentPlayer { get; }
        public bool EnableAsserts => ConvertedJamFile.Vars.ASSERTS_ENABLED.Any() ? ConvertedJamFile.Vars.ASSERTS_ENABLED : CodeGen == CodeGen.Debug;
        public bool EnableUnitTests { get; }
        public string HumanTargetName { get; set; }
        public bool HeadlessPlayer { get; }
        public bool IsThreaded { get; }
        public bool StaticLibraryPlayer { get; } // TODO: move this to AndroidPlayerConfiguration
        public FlagRequirement FlagRequirement { get; }

        public override string JamArgs => $"{base.JamArgs} -sDEVELOPMENT_PLAYER={(DevelopmentPlayer ? "1":"0")} -sSCRIPTING_BACKEND={ScriptingBackend.ToString().ToLower()}";

        public override string Identifier => IdentifierFor(ToolChain.ActionName);
        /* This is a hack to dramatically shorten the pathnames to get around the
         * Windows max path length. (This identifier appears twice in certain artifacts.)
         */
        private string IdentifierFor(string toolchainIdentifier)
        {
            var ret = $"{toolchainIdentifier}";
            ret += $"{(HeadlessPlayer ? "_hl" : string.Empty)}";
            ret += $"{(StaticLibraryPlayer ? "_st" : string.Empty)}";
            ret += $"_{(DevelopmentPlayer ? "dev" : "nondev")}";
            ret += $"_{ScriptingBackend.ToString().ToLower()[0]}";
            ret += $"{(EnableUnitTests ? "_ut" : string.Empty)}";
            ret += $"_{CodeGen.ToString().ToLower()[0]}";
            if (!IsThreaded)
                ret += "_nothreads";
            if (!Lump)
                ret += "_nolump";
            return ret;
        }

        public UnityPlayerConfiguration(
            CodeGen codeGen,
            ToolChain toolChain,
            bool lump,
            ScriptingBackend scriptingBackend,
            bool developmentPlayer,
            bool enableUnitTests = false,
            bool headlessPlayer = false,
            bool staticLibraryPlayer = false,
            FlagRequirement flagRequirement = null,
            bool isThreaded = true)
            : base(codeGen, toolChain, lump)
        {
            ScriptingBackend = scriptingBackend;
            DevelopmentPlayer = developmentPlayer;
            EnableUnitTests = enableUnitTests;
            HeadlessPlayer = headlessPlayer;
            IsThreaded = isThreaded;
            StaticLibraryPlayer = staticLibraryPlayer;
            FlagRequirement = flagRequirement;
        }

        protected override int GetSimilarityScore(string platform, NativeProgramConfiguration other)
        {
            var score = base.GetSimilarityScore(platform, other);
            var otherPlayer = other as UnityPlayerConfiguration;
            if (otherPlayer != null)
            {
                if (DevelopmentPlayer == otherPlayer.DevelopmentPlayer)
                    score++;
                if (ScriptingBackend == otherPlayer.ScriptingBackend)
                    score++;
                if (HeadlessPlayer == otherPlayer.HeadlessPlayer)
                    score++;
                if (StaticLibraryPlayer == otherPlayer.StaticLibraryPlayer)
                    score++;
            }
            return score;
        }

        private string IdentifierSuffix
        {
            get
            {
                // dev|nondev is only relevant for Release configs
                var dev = CodeGen == CodeGen.Release
                    ? (DevelopmentPlayer ? "dev_" : "nondev_")
                    : "";
                var scripting = ScriptingBackend.ToString().ToLowerInvariant();
                return dev + scripting;
            }
        }

        public override String IdentifierForProjectFile
        {
            get
            {
                if (HumanTargetName != null) return HumanTargetName + "_" + CodeGen;
                return CodeGen.ToString().ToLower() + "_" + ToolChain.LegacyPlatformIdentifier + "_" + IdentifierSuffix;
            }
        }

        public override string IdentifierNoPlatform => $"{base.IdentifierNoPlatform}_{IdentifierSuffix}";
    }

    public class FlagRequirement
    {
        public string Flag;
        public string Value;
    }
}
