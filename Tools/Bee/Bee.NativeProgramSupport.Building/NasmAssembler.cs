using System;
using System.Collections.Generic;
using Unity.BuildSystem.NativeProgramSupport;
using NiceIO;
using Unity.BuildTools;

namespace Unity.BuildSystem.NativeProgramSupport
{
    public class NasmAssembler : AssemblerBase
    {
        public override string ObjectExtension { get; } = "o";
        public override string[] SupportedExtensions { get; } = {".asm"};

        protected override string AssemblerExecutable => GetNasmExecutableForCurrentPlatform();

        public NasmAssembler(ToolChain toolchain) : base(toolchain)
        {
        }

        protected override IEnumerable<string> CommandLineArgumentsFor(NPath objectFile, AssemblerObjectFileProductionInstructions instructions)
        {
            if (Toolchain.Architecture is x64Architecture)
                yield return "-DBUILD_X64";

            if (Toolchain.Platform is MacOSXPlatform)
            {
                if (Toolchain.Architecture is x86Architecture)
                    yield return "-f macho";
                else if (Toolchain.Architecture is x64Architecture)
                    yield return "-f macho64";

                //todo: dont hardcode this in the assemblier.
                yield return "-DUNITY_OSX";
            }

            if (Toolchain.Platform is MacOSXPlatform ||
                ((Toolchain.Platform is WindowsPlatform || Toolchain.Platform is UniversalWindowsPlatform) && Toolchain.Architecture is x86Architecture))
            {
                yield return "--prefix _";
            }

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

                yield return "-DUNITY_WIN";
            }

            yield return instructions.InputFile.InQuotes(SlashMode.Native);
            yield return "-o";
            yield return objectFile.InQuotes(SlashMode.Native);
        }

        string GetNasmExecutableForCurrentPlatform()
        {
            string ret = "";

            if (Toolchain.Platform is LinuxPlatform) ret = "External/Nasm/builds/linux64/nasm";
            if (Toolchain.Platform is MacOSXPlatform) ret = "External/Nasm/builds/OSX/nasm";
            if (Toolchain.Platform is WindowsPlatform || Toolchain.Platform is UniversalWindowsPlatform) ret = "External/Nasm/builds/Win32/nasm.exe";
            if (ret != String.Empty)
                return Paths.UnityRoot.Combine(ret).ToString(SlashMode.Native);

            throw new NotSupportedException("Nasm is not supported on this platform");
        }
    }
}
