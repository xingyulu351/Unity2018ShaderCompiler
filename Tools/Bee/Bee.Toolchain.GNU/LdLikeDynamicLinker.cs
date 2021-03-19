using System.Collections.Generic;
using System.Text.RegularExpressions;
using NiceIO;
using Unity.BuildSystem.NativeProgramSupport;

namespace Bee.Toolchain.GNU
{
    public abstract class LdLikeDynamicLinker : DynamicLinker
    {
        protected bool RunningThroughCompilerFronted { get; }
        private CLikeCompilerSettings CompilerSettings { get; set; }

        // Use this when invoking linker directly
        protected LdLikeDynamicLinker(ToolChain toolchain, bool runThroughCompilerFrontend) : base(toolchain)
        {
            RunningThroughCompilerFronted = runThroughCompilerFrontend;
        }

        protected override IEnumerable<string> CommandLineFlagsFor(NPath target,
            CodeGen codegen,
            IEnumerable<NPath> inputFiles,
            IEnumerable<PrecompiledLibrary> nonToolchainSpecificLibraries,
            IEnumerable<PrecompiledLibrary> allLibraries)
        {
            if (RunningThroughCompilerFronted)
            {
                foreach (var value in CompilerSettings.CommandLineFlagsFor(target))
                    yield return value;
            }
            else
            {
                yield return "-o";
                yield return target.InQuotes();
            }

            foreach (var inputFile in inputFiles)
                yield return inputFile.InQuotes();

            foreach (var l in Toolchain.Sdk.LibraryPaths)
                yield return "-L " + l.InQuotes();

            foreach (var library in nonToolchainSpecificLibraries)
            {
                if (library.System)
                    yield return $"-l{library.InQuotes()}";
                else
                    yield return library.InQuotes();
            }
        }

        protected override ObjectFileLinker LinkerSpecificSettingsFor(NativeProgram nativeProgram, NativeProgramConfiguration config,
            CLikeCompilerSettings settings, NPath file, NPath artifacts)
        {
            var linker = (LdLikeDynamicLinker)base.LinkerSpecificSettingsFor(nativeProgram, config, settings, file, artifacts);
            return linker.With<LdLikeDynamicLinker>(l => l.CompilerSettings = settings);
        }

        private static readonly Regex Whitespaces = new Regex("\\s+", RegexOptions.Compiled);
        protected string AdjustLinkerFlagToCompilerFrontend(string flag) =>
            !RunningThroughCompilerFronted ? flag : $"-Wl,{Whitespaces.Replace(flag, ",")}";
    }
}
