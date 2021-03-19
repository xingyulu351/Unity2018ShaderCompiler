using System;
using System.Collections.Generic;
using Bee.Core;
using Bee.Toolchain.GNU;
using NiceIO;
using Unity.BuildSystem.NativeProgramSupport;

namespace Bee.Toolchain.LLVM
{
    public class ClangCompiler : GccLikeCompiler
    {
        public ClangCompiler(ToolChain toolchain) : base(toolchain) {}
    }

    public enum ClangSanitizerMode
    {
        None,
        Address,
        Undefined,
        Thread,
    }

    public class ClangCompilerSettings : GccLikeCompilerSettings
    {
        public bool ExplicitlyRequireCPlusPlusIncludes { get; internal set; } = false;

        protected virtual NPath GccToolchain => null;
        protected virtual string Target => null;
        internal ClangSanitizerMode Sanitizer = ClangSanitizerMode.None;

        public ClangCompilerSettings(GccLikeCompiler compiler) : base(compiler)
        {
        }

        public override IEnumerable<string> CommandLineFlagsFor(NPath target)
        {
            foreach (var flag in base.CommandLineFlagsFor(target))
                yield return flag;

            if (Backend.Current.CanHandleAnsiColors)
                yield return "-fcolor-diagnostics";

            if (GccToolchain != null)
                yield return $"--gcc-toolchain={GccToolchain.InQuotes()}";

            if (Target != null)
                yield return $"-target {Target}";

            if (OutputASM)
                yield return $"-save-temps=obj";

            if (ExplicitlyRequireCPlusPlusIncludes)
                yield return "-nostdinc++";

            switch (Sanitizer)
            {
                case ClangSanitizerMode.Address:
                    yield return "-fsanitize=address";
                    break;
                // TODO: OSX clang has linker error when undefined is used without address sanitizer flag
                case ClangSanitizerMode.Undefined:
                    yield return "-fsanitize=address,undefined -fno-sanitize=vptr,enum,alignment,null";
                    break;
                case ClangSanitizerMode.Thread:
                    yield return "-fsanitize=thread";
                    break;
            }

            var commonSanitizerFlags = new[]
            {
                "-fsanitize-recover=all",
                "-fno-omit-frame-pointer",
                "-fno-optimize-sibling-calls",
                "-fsanitize-blacklist=clang-sanitizer-blacklist.txt",
            };

            if (Sanitizer != ClangSanitizerMode.None)
                foreach (var sanitizerFlag in commonSanitizerFlags)
                    yield return sanitizerFlag;
        }

        public sealed override IEnumerable<string> CommandLineInfluencingCompilationFor(NPath outputObjectFile, NPath inputCppFile, NPath createOutputPchAt,
            NPath headerToCreateOutputPchFrom)
        {
            foreach (var flag in base.CommandLineInfluencingCompilationFor(outputObjectFile, inputCppFile, createOutputPchAt, headerToCreateOutputPchFrom))
                yield return flag;

            if (OutputPCH == null)
                yield break;

            yield return "-include-pch";
            yield return OutputPCH.InQuotes();
            if (createOutputPchAt != null)
                throw new ArgumentException("Building a pch while also trying to use one!");
        }
    }

    public static class ClangCompilerSettingExtensions
    {
        public static T WithExplicitlyRequireCPlusPlusIncludes<T>(this T _this, bool value)
            where T : ClangCompilerSettings
        {
            return _this.With(m => m.ExplicitlyRequireCPlusPlusIncludes = value);
        }

        public static T WithSanitizer<T>(this T _this, ClangSanitizerMode value) where T : ClangCompilerSettings
        {
            return _this.With(m => m.Sanitizer = value);
        }
    }
}
