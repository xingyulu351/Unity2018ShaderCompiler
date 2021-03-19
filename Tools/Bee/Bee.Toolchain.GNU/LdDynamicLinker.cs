using System.Collections.Generic;
using System.Linq;
using NiceIO;
using Unity.BuildSystem.NativeProgramSupport;
using Unity.BuildTools;

namespace Bee.Toolchain.GNU
{
    public static class LdDynamicLinkerExtensions
    {
        // Linker settings
        public static T AsPositionIndependentExecutable<T>(this T _this, bool value) where T : LdDynamicLinker
            => _this.With<T>(l => l.PositionIndependentExecutable = value);

        public static T WithStaticCppRuntime<T>(this T _this, bool staticCppRuntime) where T : LdDynamicLinker
            => _this.With<T>(l => l.UseStaticCppRuntime = staticCppRuntime);

        public static T WithDynamicLibraryLoadPath<T>(this T _this, params string[] paths) where T : LdDynamicLinker
            => _this.With<T>(l => l.DynamicLibraryLoadPaths = l.DynamicLibraryLoadPaths.Concat(paths));

        public static T WithDynamicLibraryLinkPath<T>(this T _this, params string[] paths) where T : LdDynamicLinker
            => _this.With<T>(l => l.DynamicLibraryLinkPaths = l.DynamicLibraryLinkPaths.Concat(paths));

        public static T WithWrappedSymbol<T>(this T _this, params string[] symbols) where T : LdDynamicLinker
            => _this.With<T>(l => l.WrappedSymbols = l.WrappedSymbols.Concat(symbols));

        public static T WithAllStaticLibrarySymbolsHidden<T>(this T _this, bool hide) where T : LdDynamicLinker
            => _this.With<T>(l => l.HideAllStaticLibrarySymbols = hide);
    }

    public class LdDynamicLinker : LdLikeDynamicLinker
    {
        public override bool DynamicLibrary { get; set;  }

        internal bool PositionIndependentExecutable { get; set; }
        internal bool HideAllStaticLibrarySymbols { get; set; }
        public bool UseStaticCppRuntime { get; set; }

        // These paths are embedded in shared objects and used at run-time
        internal IEnumerable<string> DynamicLibraryLoadPaths { get; set; } = Enumerable.Empty<string>();
        // These paths are used by the linker at link-time to search for shared objects
        internal IEnumerable<string> DynamicLibraryLinkPaths { get; set; } = Enumerable.Empty<string>();

        internal IEnumerable<string> WrappedSymbols { get; set; } = Enumerable.Empty<string>();
        protected virtual string LdLinkerName { get; } = null;

        public LdDynamicLinker(ToolChain toolchain, bool runThroughCompilerFrontend) : base(toolchain, runThroughCompilerFrontend) {}

        protected virtual string StaticLinkCppRuntimeFlag { get { return "-static-libstdc++"; } }

        protected override IEnumerable<string> CommandLineFlagsFor(NPath target, CodeGen codegen, IEnumerable<NPath> inputFiles,
            IEnumerable<PrecompiledLibrary> nonToolchainSpecificLibraries, IEnumerable<PrecompiledLibrary> allLibraries)
        {
            foreach (var flag in base.CommandLineFlagsFor(target, codegen, inputFiles, nonToolchainSpecificLibraries, allLibraries))
                yield return flag;

            if (RunningThroughCompilerFronted && LdLinkerName != null)
                yield return $"-fuse-ld={LdLinkerName}{HostPlatform.Exe}";

            // dead code stripping
            yield return AdjustLinkerFlagToCompilerFrontend("--gc-sections");

            // treat warnings as errors
            yield return AdjustLinkerFlagToCompilerFrontend("--fatal-warnings");

            // we may want this as an option (configurable name)
            yield return AdjustLinkerFlagToCompilerFrontend("-soname=" + target.FileName);

            // add setting for this when needed
            yield return AdjustLinkerFlagToCompilerFrontend("--no-undefined");

            // always generate a SHA1 build id
            yield return AdjustLinkerFlagToCompilerFrontend("--build-id");

            // no execstack is a good default (add setting if needed)
            yield return AdjustLinkerFlagToCompilerFrontend("-z,noexecstack");

            if (DynamicLibrary)
                yield return "-shared";
            else if (PositionIndependentExecutable)
                yield return "-pie";

            if (HideAllStaticLibrarySymbols)
                yield return AdjustLinkerFlagToCompilerFrontend("--exclude-libs,ALL");

            foreach (var path in DynamicLibraryLoadPaths)
                yield return AdjustLinkerFlagToCompilerFrontend($"-rpath {path}");

            foreach (var path in DynamicLibraryLinkPaths)
                yield return AdjustLinkerFlagToCompilerFrontend($"-rpath-link {path}");

            foreach (var symbol in WrappedSymbols)
                yield return AdjustLinkerFlagToCompilerFrontend($"--wrap,{symbol}");

            if (UseStaticCppRuntime)
                yield return StaticLinkCppRuntimeFlag;

            // link time optimizations?
            // if (codegen == CodeGen.Master)
            // yield return AdjustLinkerFlagToCompilerFrontend("-O1");
        }
    }
}
