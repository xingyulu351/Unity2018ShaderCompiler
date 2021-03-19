using System;
using System.Collections.Generic;
using Bee.Core;
using NiceIO;
using Unity.BuildSystem.NativeProgramSupport;
using System.Linq;

namespace Bee.Toolchain.GNU
{
    public class GccCompiler : GccLikeCompiler
    {
        public override string PCHExtension { get; } = "h.gch";

        public GccCompiler(ToolChain toolchain) : base(toolchain) {}
    }

    public class GccCompilerSettings : GccLikeCompilerSettings
    {
        internal IEnumerable<string> BinarySearchPaths { get; set; } = Enumerable.Empty<string>();

        public GccCompilerSettings(GccLikeCompiler compiler) : base(compiler)
        {
        }

        public override IEnumerable<string> CommandLineFlagsFor(NPath target)
        {
            foreach (var flag in base.CommandLineFlagsFor(target))
                yield return flag;

            if (Backend.Current.CanHandleAnsiColors && ((GccCompiler)Compiler).SupportsColorDiagnostics)
                yield return "-fdiagnostics-color=always";

            if (OutputASM)
                yield return $"-save-temps=obj";

            foreach (var path in BinarySearchPaths)
                yield return $"-B {path}";
        }

        public sealed override IEnumerable<string> CommandLineInfluencingCompilationFor(NPath outputObjectFile, NPath inputCppFile, NPath createOutputPchAt, NPath headerToCreateOutputPchFrom)
        {
            foreach (var value in base.CommandLineInfluencingCompilationFor(outputObjectFile, inputCppFile, createOutputPchAt, headerToCreateOutputPchFrom))
                yield return value;

            if (OutputPCH == null)
                yield break;

            yield return "-include";
            yield return OutputPCH.ChangeExtension("" /* strip .gch */).InQuotes();
            if (createOutputPchAt != null)
                throw new ArgumentException("Building a pch while also trying to use one!");
        }
    }

    public static class GccCompilerExtensions
    {
        public static T WithBinarySearchPath<T>(this T _this, params string[] paths) where T : GccCompilerSettings
            => _this.With<T>(l => l.BinarySearchPaths = l.BinarySearchPaths.Concat(paths));
    }
}
