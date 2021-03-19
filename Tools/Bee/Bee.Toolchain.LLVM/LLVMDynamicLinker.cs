using System.Collections.Generic;
using Bee.Toolchain.GNU;
using NiceIO;
using Unity.BuildSystem.NativeProgramSupport;
using Unity.BuildTools;

namespace Bee.Toolchain.LLVM
{
    public class LLVMDynamicLinker : LdDynamicLinker
    {
        protected override string LdLinkerName { get; } = "lld" + HostPlatform.Exe; // TODO: LdDynamicLinker already adds ".exe"?

        public LLVMDynamicLinker(ToolChain toolchain, bool runThroughCompilerFrontend) : base(toolchain, runThroughCompilerFrontend) {}

        protected override IEnumerable<string> CommandLineFlagsFor(NPath target, CodeGen codegen, IEnumerable<NPath> inputFiles,
            IEnumerable<PrecompiledLibrary> nonToolchainSpecificLibraries, IEnumerable<PrecompiledLibrary> allLibraries)
        {
            foreach (var value in base.CommandLineFlagsFor(target, codegen, inputFiles, nonToolchainSpecificLibraries, allLibraries))
                yield return value;
        }
    }
}
