using Bee.Core;
using NiceIO;
using Unity.BuildSystem.NativeProgramSupport;
using Unity.BuildTools;

namespace Bee.Toolchain.Lumin
{
    public enum StripOptions
    {
        Full,
        StripDebug,
        OnlyKeepDebug,
        StripUnneeded
    }

    public static class ObjCopy
    {
        public static NPath Setup(ToolChain toolchain, NPath inputFile, NPath targetFile,
            StripOptions options)
        {
            var sdk = (LuminSdk)toolchain.Sdk;
            var objcopy = sdk.PathForTool("objcopy");
            Backend.Current.AddAction(
                "LuminSDK.ObjCopy",
                new[] {targetFile},
                new[] {inputFile},
                objcopy.InQuotes(),
                new[]
                {
                    FlagForOptions(options),
                    inputFile.InQuotes(),
                    targetFile.InQuotes()
                });
            return targetFile;
        }

        private static string FlagForOptions(StripOptions options)
        {
            switch (options)
            {
                case StripOptions.Full: return "--strip-all";
                case StripOptions.StripDebug: return "--strip-debug";
                case StripOptions.OnlyKeepDebug: return "--only-keep-debug";
                case StripOptions.StripUnneeded: return "--strip-unneeded";
                default: Errors.Exit($"Unknown symbol strip option {options}"); return "";
            }
        }
    }
}
