using System.Collections.Generic;
using NiceIO;
using Unity.BuildSystem.NativeProgramSupport;

// TODO: DebugLevel

namespace Bee.Toolchain.ARM
{
    public class ArmCompiler5 : CLikeCompiler
    {
        public override string ObjectExtension { get; } = "o";
        public override string WarningStringForUnusedVariable { get; } = "177";
        protected override bool SupportsResponseFile { get; } = false;
        public override bool SupportsPCH { get; } = false;

        public ArmCompiler5(ToolChain toolchain) : base(toolchain) {}
    }

    public class ArmCompiler5Settings : CLikeCompilerSettings
    {
        public ArmCompiler5Settings(CLikeCompiler compiler) : base(compiler) {}

        public override IEnumerable<string> CommandLineInfluencingCompilationFor(NPath outputObjectFile, NPath inputCppFile,
            NPath createOutputPchAt = null, NPath headerToCreateOutputPchFrom = null)
        {
            // don't link
            yield return "-c";

            if (Language == Language.Cpp)
                yield return CppLanguageVersion > 03 ? "--cpp11" : "--cpp";

            yield return RTTI ? "--rtti --rtti_data" : "--no_rtti --no_rtti_data";
            yield return Exceptions ? "--exceptions" : "--no_exceptions";

            switch (Optimization)
            {
                case OptimizationLevel.None:
                    yield return "-O0";
                    break;
                case OptimizationLevel.Speed:
                case OptimizationLevel.SpeedMax:
                    yield return "-O3 -Otime";
                    break;
                case OptimizationLevel.Size:
                    yield return "-O3 -Osize";
                    break;
            }

            foreach (var wap in WarningPolicies)
            {
                switch (wap.Policy)
                {
                    case WarningPolicy.Silent:
                        yield return $"--diag_suppress={wap.Warning}";
                        break;
                    case WarningPolicy.AsWarning:
                        yield return $"--diag_warning={wap.Warning}";
                        break;
                    case WarningPolicy.AsError:
                        yield return $"--diag_error={wap.Warning}";
                        break;
                }
            }

            foreach (var define in Defines)
                yield return $"-D{define}";

            foreach (var path in IncludeDirectories)
                yield return $"-I{path.InQuotes()}";

            foreach (var path in Toolchain.Sdk.IncludePaths)
                yield return $"-J{path.InQuotes()}";

            if (Toolchain.Sdk.SysRoot != null)
                yield return $"--configure_sysroot={Toolchain.Sdk.SysRoot.InQuotes()}";

            if (OutputASM)
                yield return "-asm";

            //"--debug", "--debug_info=line_inlining_extensions", "--no_debug_macros"

            // stripping
            yield return "--split_sections";
            yield return "--vfe";

            // enable gnu features
            yield return "--gnu";

            // enable multibyte char string in non-BOM tagged files
            yield return "--multibyte_chars";

            // msvc style messages
            yield return "--diag_style=ide";

            // always re-order data regardless of optimization level
            yield return "--data_reorder";

            // disable "last line of file ends without a newline"
            yield return "--diag_suppress=1";

            yield return "-o";
            yield return outputObjectFile.InQuotes();
            yield return inputCppFile.InQuotes();
        }
    }
}
