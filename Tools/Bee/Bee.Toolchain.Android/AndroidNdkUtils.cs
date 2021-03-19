using System.Collections.Generic;
using Bee.Core;
using NiceIO;
using Unity.BuildSystem.NativeProgramSupport;
using Unity.BuildTools;

namespace Bee.Toolchain.Android
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
            StripOptions options, string sectionName = null, NPath sectionFile = null, NPath debugFile = null)
        {
            var ndk = (AndroidNdk)toolchain.Sdk;
            var objcopy = $"{ndk.GNUTools}/objcopy";
            var inputFiles = new List<NPath> { inputFile };
            var args = new List<string>
            {
                FlagForOptions(options),
                inputFile.InQuotes(),
                targetFile.InQuotes(),
            };
            if (sectionName != null && sectionFile != null)
            {
                inputFiles.Add(sectionFile);
                args.Add("--add-section");
                args.Add($"{sectionName}={sectionFile.InQuotes()}");
            }
            if (debugFile != null)
            {
                inputFiles.Add(debugFile);
                args.Add($"--add-gnu-debuglink={debugFile.InQuotes()}");
            }
            Backend.Current.AddAction(
                "NdkObjCopy",
                new[] {targetFile},
                inputFiles.ToArray(),
                objcopy.InQuotes(),
                args.ToArray());
            return targetFile;
        }

        public static string FlagForOptions(StripOptions options)
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
