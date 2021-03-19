using System;
using System.Collections.Generic;
using System.Diagnostics.Eventing.Reader;
using Bee.NativeProgramSupport;
using Bee.Toolchain.GNU;
using Bee.Toolchain.LLVM;
using NiceIO;
using Unity.BuildSystem.NativeProgramSupport;
using Unity.BuildTools;

namespace Bee.Toolchain.Emscripten
{
    public class EmscriptenCompiler : ClangCompiler
    {
        public EmscriptenCompiler(EmscriptenToolchain toolchain) : base(toolchain)
        {
            DefaultSettings = new EmscriptenCompilerSettings(this);
        }

        public override string[] SupportedExtensions { get; } = {"cpp", "c", "cc"};

        protected override Dictionary<string, string> EnvironmentVariables => ((EmscriptenSdk)Toolchain.Sdk).EnvironmentVariables;

        public override string ObjectExtension { get; } = "o";
    }

    public class EmscriptenCompilerSettings : ClangCompilerSettings
    {
        public EmscriptenCompilerSettings(GccLikeCompiler compiler) : base(compiler)
        {
        }

        public override IEnumerable<string> CommandLineFlagsFor(NPath target)
        {
            //Todo: move these warnings into the common warning system, when the warning system is made more ready for v2 buildcode.
            yield return "-D__webgl__";
            yield return "-Wno-warn-absolute-paths";
            yield return
                "-Wno-c++11-extensions"; // This new warning seems to print incorrectly on our build agents. It might not be working correctly
            yield return "-Wno-nonportable-include-path"; //-nostdinc
            yield return "-ffunction-sections";
            yield return "-fno-unwind-tables";
            yield return "-fomit-frame-pointer";
            if (!Exceptions)
                yield return "-fno-exceptions";
            // WebGL has no threads, so turn off C++11 thread safe static initializers
            if (Language == Language.Cpp || Language == Language.ObjectiveCpp)
                yield return "-fno-threadsafe-statics";

            foreach (var item in base.CommandLineFlagsFor(target))
                yield return item;
        }

        protected override IEnumerable<string> GetLanguageArgument(NPath createOutputPchAt)
        {
            var buildingPchPostFix = createOutputPchAt != null ? "-header" : String.Empty;

            //as of 05/18/2018, emscripten does not handle language passed as "-x c" correctly; you have to say
            //"-xc" or it'll incorrectly pass -std=c++03 also
            switch (Language)
            {
                case Language.C:
                    yield return "-xc" + buildingPchPostFix;
                    break;
                case Language.Cpp:
                    yield return "-xc++" + buildingPchPostFix;
                    break;
                default:
                    throw new NotSupportedException($"Unsupported language: {Language}");
            }
        }

        protected override CLikeCompilerSettings CompilerSpecificCompilerSettingsFor(NativeProgram nativeProgram,
            NativeProgramConfiguration config, NPath file, NPath artifactsPath)
        {
            var settings = (EmscriptenCompilerSettings)base.CompilerSpecificCompilerSettingsFor(nativeProgram, config, file, artifactsPath);
            return settings.WithDebugMode(DebugMode.None).WithOptimizationLevel(OptimizationLevel.Speed);
        }
    }
}
