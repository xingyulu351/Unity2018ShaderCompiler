using System;
using System.Collections.Generic;
using System.Linq;
using Bee.Toolchain.VisualStudio;
using NiceIO;
using Unity.BuildSystem.NativeProgramSupport;
using Unity.BuildSystem.VisualStudio.MsvcVersions;

namespace Bee.Toolchain.UWP
{
    public sealed class UWPCompiler : MsvcCompiler
    {
        public UWPCompiler(UWPToolchain toolchain) : base(toolchain)
        {
            DefaultSettings = new UWPCompilerSettings(this)
                .WithStaticStandardLibrary(false)
                .WithDefines(new[]
                {
                    "__WRL_NO_DEFAULT_LIB__",
                    "WINAPI_FAMILY=WINAPI_FAMILY_APP"
                });
        }
    }

    public sealed class UWPCompilerSettings : MsvcCompilerSettings
    {
        public UWPCompilerSettings(CLikeCompiler compiler) : base(compiler) {}

        protected override IEnumerable<string> CommandLineInfluencingCompilationForInternal(
            NPath outputObjectFile,
            NPath inputCppFile,
            NPath createOutputPchAt,
            NPath headerToCreateOutputPCHFrom)
        {
            foreach (var value in base.CommandLineInfluencingCompilationForInternal(outputObjectFile, inputCppFile, createOutputPchAt, headerToCreateOutputPCHFrom))
                yield return value;

            var useWinRT = (WinRTExtensions || WinRTAssemblies.Any()) && Language != Language.C;
            if (useWinRT)
            {
                if (!Exceptions)
                    throw new Exception("Using WinRTExtensions or WinRTAssemblies requires C++ exceptions to be enabled");

                yield return "/D_WINRT_DLL";
                yield return "/ZW";
                yield return "/ZW:nostdlib";

                yield return $"/FU{((UWPSdk)Toolchain.Sdk).WinRTPlatformMetadata.InQuotes(SlashMode.Native)}";
                yield return $"/FU{WindowsSDKs.WinRTWindowsMetadataPath.InQuotes(SlashMode.Native)}";

                foreach (var referencedAssembly in WinRTAssemblies)
                    yield return $"/FU{referencedAssembly.InQuotes(SlashMode.Native)}";
            }
        }
    }
}
