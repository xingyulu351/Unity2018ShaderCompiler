using System;
using System.Collections.Generic;
using NiceIO;
using Unity.BuildSystem.NativeProgramSupport;
using Unity.BuildTools;

namespace Bee.NativeProgramSupport
{
    public abstract class NasmCompiler : AssemblyCompilerBase
    {
        public NasmCompiler(ToolChain toolchain)
            : base(toolchain) {}

        public override string ObjectExtension { get; } = "o";
        public override string[] SupportedExtensions { get; } = {".asm"};

        protected virtual bool RequiresUnderscorePrefix => true;

        protected override string AssemblerExecutable => GetNasmExecutableForCurrentHostPlatform();

        protected override IEnumerable<string> CommandLineArgumentsFor(NPath objectFile, AssemblerObjectFileProductionInstructions instructions)
        {
            foreach (var arg in base.CommandLineArgumentsFor(objectFile, instructions))
            {
                yield return arg;
            }

            if (RequiresUnderscorePrefix)
            {
                yield return "--prefix _";
            }

            yield return instructions.InputFile.InQuotes(SlashMode.Native);
            yield return "-o";
            yield return objectFile.InQuotes(SlashMode.Native);
        }

        static string GetNasmExecutableForCurrentHostPlatform()
        {
            var ret = "";
            if (HostPlatform.IsLinux) ret = "External/Nasm/builds/linux64/nasm";
            if (HostPlatform.IsOSX) ret = "External/Nasm/builds/OSX/nasm";
            if (HostPlatform.IsWindows) ret = "External/Nasm/builds/Win32/nasm.exe";

            if (ret != string.Empty)
                return Paths.UnityRoot.Combine(ret).ToString(SlashMode.Native);

            throw new NotSupportedException("Nasm is not supported on this host OS");
        }
    }
}
