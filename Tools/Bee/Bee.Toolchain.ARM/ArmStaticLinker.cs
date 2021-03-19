using System.Collections.Generic;
using NiceIO;
using Unity.BuildSystem.NativeProgramSupport;

namespace Bee.Toolchain.ARM
{
    public class ArmStaticLinker : StaticLinker
    {
        protected bool BundleStaticDependencies { get; }
        protected override bool SupportsResponseFile { get; } = false;

        public ArmStaticLinker(ToolChain toolchain, bool bundleDependencies = false) : base(toolchain)
        {
            BundleStaticDependencies = bundleDependencies;
        }

        protected override IEnumerable<string> CommandLineFlagsFor(NPath destination, CodeGen codegen,
            IEnumerable<NPath> objectFiles,
            IEnumerable<PrecompiledLibrary> nonToolchainSpecificLibraries, IEnumerable<PrecompiledLibrary> allLibraries)
        {
            yield return $"--create {destination}";

            foreach (var objectFile in objectFiles)
                yield return objectFile.InQuotes();

            if (!BundleStaticDependencies)
                yield break;

            foreach (var lib in nonToolchainSpecificLibraries)
                yield return lib.InQuotes();
        }
    }
}
