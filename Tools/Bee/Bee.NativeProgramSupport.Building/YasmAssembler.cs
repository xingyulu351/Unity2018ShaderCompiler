using System;
using System.Collections.Generic;
using Unity.BuildSystem.NativeProgramSupport;
using NiceIO;
using Unity.BuildTools;

namespace Unity.BuildSystem.NativeProgramSupport
{
    public class YasmAssembler : AssemblerBase
    {
        public override string ObjectExtension { get; } = "o";
        public override string[] SupportedExtensions { get; } = {".yasm"};

        protected override string AssemblerExecutable => GetYasmExecutableForCurrentPlatform();

        public YasmAssembler(ToolChain toolchain) : base(toolchain)
        {
        }

        protected override IEnumerable<string> CommandLineArgumentsFor(NPath objectFile, AssemblerObjectFileProductionInstructions instructions)
        {
            if (Toolchain.Architecture is x64Architecture)
                yield return "-DBUILD_X64";

            if (Toolchain.Platform is MacOSXPlatform)
            {
                if (Toolchain.Architecture is x86Architecture)
                    yield return "-f macho32";
                else if (Toolchain.Architecture is x64Architecture)
                    yield return "-f macho64";

                yield return "-DUNITY_OSX";
            }

            yield return $"-I {Paths.UnityRoot.InQuotes()}";

            if (Toolchain.Platform is LinuxPlatform)
            {
                if (Toolchain.Architecture is x64Architecture)
                    yield return "-f elf64";
                else if (Toolchain.Architecture is x86Architecture)
                    yield return "-f elf32";
            }

            if (Toolchain.Platform is WindowsPlatform || Toolchain.Platform is UniversalWindowsPlatform)
            {
                if (Toolchain.Architecture is x64Architecture)
                    yield return "-f win64";
                else
                    yield return "-f win32";
            }

            yield return instructions.InputFile.InQuotes();
            yield return "-o";
            yield return objectFile.InQuotes();
        }

        string GetYasmExecutableForCurrentPlatform()
        {
            string ret = "";

            if (Toolchain.Platform is LinuxPlatform) ret = "External/Yasm/builds/linux64/yasm";
            if (Toolchain.Platform is MacOSXPlatform) ret = "External/Yasm/builds/OSX/yasm";
            if (Toolchain.Platform is WindowsPlatform || Toolchain.Platform is UniversalWindowsPlatform) ret = "External/Yasm/builds/Win32/yasm.exe";

            if (ret != String.Empty)
                return Paths.UnityRoot.Combine(ret).ToString(SlashMode.Native);

            throw new NotSupportedException("Yasm is not supported on this platform");
        }
    }
}
