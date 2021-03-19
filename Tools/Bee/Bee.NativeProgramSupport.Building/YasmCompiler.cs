using System;
using System.Collections.Generic;
using NiceIO;
using Unity.BuildSystem.NativeProgramSupport;
using Unity.BuildTools;

namespace Bee.NativeProgramSupport
{
    public abstract class YasmCompiler : AssemblyCompilerBase
    {
        public YasmCompiler(ToolChain toolchain)
            : base(toolchain) {}

        public override string ObjectExtension { get; } = "o";
        public override string[] SupportedExtensions { get; } = {".yasm"};

        protected override string AssemblerExecutable => GetYasmExecutableForCurrentHostPlatform();

        protected override IEnumerable<string> CommandLineArgumentsFor(NPath objectFile, AssemblerObjectFileProductionInstructions instructions)
        {
            foreach (var arg in base.CommandLineArgumentsFor(objectFile, instructions))
            {
                yield return arg;
            }

            yield return $"-I {Paths.UnityRoot.InQuotes()}";

            yield return instructions.InputFile.InQuotes();
            yield return "-o";
            yield return objectFile.InQuotes();
        }

        static string GetYasmExecutableForCurrentHostPlatform()
        {
            var ret = "";
            if (HostPlatform.IsLinux) ret = "External/Yasm/builds/linux64/yasm";
            if (HostPlatform.IsOSX) ret = "External/Yasm/builds/OSX/yasm";
            if (HostPlatform.IsWindows) ret = "External/Yasm/builds/Win32/yasm.exe";

            if (ret != string.Empty)
                return Paths.UnityRoot.Combine(ret).ToString(SlashMode.Native);

            throw new NotSupportedException("Yasm is not supported on this host OS");
        }
    }
}
