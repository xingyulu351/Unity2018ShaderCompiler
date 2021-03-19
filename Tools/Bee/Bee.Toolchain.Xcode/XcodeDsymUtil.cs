using Bee.Core;
using NiceIO;
using Unity.BuildSystem.NativeProgramSupport;
using Unity.BuildTools;

namespace Bee.Toolchain.Xcode
{
    public static class DsymUtil
    {
        public static NPath Setup(XcodeSdk sdk, NPath inputFile, NPath targetFile)
        {
            Backend.Current.AddAction(
                actionName: "XcodeDsymUtil",
                targetFiles: new[] {targetFile},
                inputs: new[] {inputFile},
                executableStringFor: sdk.BinPath.Combine("dsymutil").InQuotes(),
                commandLineArguments: new string[]
                {
                    inputFile.InQuotes(),
                    "-o",
                    targetFile.InQuotes(),
                },
                allowUnwrittenOutputFiles: true);
            return targetFile;
        }
    }
}
