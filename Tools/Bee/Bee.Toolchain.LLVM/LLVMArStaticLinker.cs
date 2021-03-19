using System.Collections.Generic;
using NiceIO;
using Unity.BuildSystem.NativeProgramSupport;

namespace Bee.Toolchain.LLVM
{
    public class LLVMArStaticLinker : StaticLinker
    {
        protected override bool SupportsResponseFile => false;

        public LLVMArStaticLinker(ToolChain toolchain) : base(toolchain)
        {
        }

        protected override IEnumerable<string> CommandLineFlagsFor(NPath destination, CodeGen codegen, IEnumerable<NPath> objectFiles, IEnumerable<PrecompiledLibrary> nonToolchainSpecificLibraries, IEnumerable<PrecompiledLibrary> allLibraries)
        {
            yield return "rcsu";

            yield return destination.InQuotes();

            foreach (var objectFile in objectFiles)
                yield return objectFile.InQuotes();
        }
    }
}
