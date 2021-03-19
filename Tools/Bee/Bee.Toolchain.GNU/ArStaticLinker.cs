using System.Collections.Generic;
using NiceIO;
using Unity.BuildSystem.NativeProgramSupport;

namespace Bee.Toolchain.GNU
{
    public class ArStaticLinker : StaticLinker
    {
        protected override bool SupportsResponseFile => false;

        public ArStaticLinker(ToolChain toolchain) : base(toolchain)
        {
        }

        protected override IEnumerable<string> CommandLineFlagsFor(NPath destination, CodeGen codegen, IEnumerable<NPath> objectFiles, IEnumerable<PrecompiledLibrary> nonToolchainSpecificLibraries, IEnumerable<PrecompiledLibrary> allLibraries)
        {
            yield return "-rcs";

            yield return destination.InQuotes();

            foreach (var objectFile in objectFiles)
                yield return objectFile.InQuotes();
        }
    }
}
