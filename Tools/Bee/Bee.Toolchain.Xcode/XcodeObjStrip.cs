using Bee.Core;
using NiceIO;

namespace Bee.Toolchain.Xcode
{
    public static class ObjStrip
    {
        public static NPath Setup(XcodeSdk sdk, NPath inputFile, NPath targetFile)
        {
            Backend.Current.AddAction(
                actionName: "XcodeObjStrip",
                targetFiles: new[] {targetFile},
                inputs: new[] {inputFile},
                executableStringFor: sdk.BinPath.Combine("strip").InQuotes(),
                commandLineArguments: new string[]
                {
                    "-x",
                    inputFile.InQuotes(),
                    "-o",
                    targetFile.InQuotes()
                });
            return targetFile;
        }
    }
}
