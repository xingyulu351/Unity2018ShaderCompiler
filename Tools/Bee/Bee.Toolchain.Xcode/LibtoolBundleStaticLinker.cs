using System.Collections.Generic;
using NiceIO;
using Unity.BuildSystem.NativeProgramSupport;

namespace Bee.Toolchain.Xcode
{
    public class LibtoolBundleStaticLinker : StaticLinker
    {
        protected override string ActionName => $"Libtool_{Toolchain.ActionName}";

        protected override bool SupportsResponseFile => false;

        public LibtoolBundleStaticLinker(ToolChain toolChain) : base(toolChain)
        {
        }

        protected override IEnumerable<string> CommandLineFlagsFor(NPath destination, CodeGen codegen, IEnumerable<NPath> objectFiles,
            IEnumerable<PrecompiledLibrary> nonToolchainSpecificLibraries, IEnumerable<PrecompiledLibrary> allLibraries)
        {
            if (Toolchain.Architecture is ARMv7Architecture)
            {
                yield return "-arch_only";
                yield return "armv7";
            }
            if (Toolchain.Architecture is Arm64Architecture)
            {
                yield return "-arch_only";
                yield return "arm64";
            }

            yield return "-static";
            yield return "-o";
            yield return destination.InQuotes();
            foreach (var of in objectFiles)
                yield return of.InQuotes();
            foreach (var l in nonToolchainSpecificLibraries)
                yield return l.InQuotes();
        }
    }
}
