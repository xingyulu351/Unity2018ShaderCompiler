using System;
using System.Collections.Generic;
using System.Linq;
using NiceIO;
using Bee.Toolchain.GNU;
using Bee.Toolchain.LLVM;
using Unity.BuildSystem.NativeProgramSupport;

namespace Bee.Toolchain.Lumin
{
    public class LuminCompiler : ClangCompiler
    {
        protected override bool SupportsCCache => true;

        public LuminCompiler(LuminToolchain toolchain) : base(toolchain)
        {
            DefaultSettings = new LuminCompilerSettings(this)
                .WithDefines(BuiltinDefines().ToArray())
                .WithPositionIndependentCode(true)
                .WithExplicitlyRequireCPlusPlusIncludes(true);
        }

        private IEnumerable<string> BuiltinDefines()
        {
            if (Architecture is Arm64Architecture)
            {
                yield return "LUMIN_64";
                yield return "__arm64__";
            }

            yield return "LUMIN";

            // Lumin is android-derived, so re-using the existing android defines reduces the
            // amount of code changes required when porting.
            yield return "ANDROID";
            yield return "__ANDROID__";
        }
    }

    public class LuminCompilerSettings : ClangCompilerSettings
    {
        protected override NPath GccToolchain => (Toolchain.Sdk as LuminSdk)?.ToolchainRoot;
        protected override string Target => (Toolchain.Sdk as LuminSdk)?.Target;

        public LuminCompilerSettings(LuminCompiler compiler) : base(compiler)
        {
        }

        public override IEnumerable<string> CommandLineFlagsFor(NPath target)
        {
            return base.CommandLineFlagsFor(target)
                .Concat(CompilerFlags());
        }

        private IEnumerable<string> CompilerFlags()
        {
            if (Compiler.Architecture is Arm64Architecture)
            {
                yield return "-march=armv8-a";
                yield return "-mcpu=cortex-a57+crypto";
            }

            yield return "-no-canonical-prefixes";
            yield return "-fno-limit-debug-info";
            yield return "-fno-omit-frame-pointer";
            yield return "-fno-short-enums";
            yield return "-fstack-protector-strong";
            yield return "-ftemplate-backtrace-limit=0";
            yield return "-funsigned-char";
            yield return "-funwind-tables";
        }
    }
}
