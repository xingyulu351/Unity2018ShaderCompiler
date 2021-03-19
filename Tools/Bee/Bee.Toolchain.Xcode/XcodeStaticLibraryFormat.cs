using System.Collections.Generic;
using NiceIO;
using Unity.BuildSystem.NativeProgramSupport;

namespace Bee.Toolchain.Xcode
{
    public class XcodeStaticLibraryFormat : NativeProgramFormat
    {
        public override string Extension { get; } = "a";

        protected internal XcodeStaticLibraryFormat(XcodeToolchain toolchain) : base(
            new _Linker(toolchain))
        {
        }

        private sealed class _Linker : StaticLinker
        {
            protected override bool SupportsResponseFile => false; // libtool does not support response files

            public _Linker(ToolChain toolChain) : base(toolChain) {}

            protected override IEnumerable<string> CommandLineFlagsFor(NPath destination, CodeGen codegen, IEnumerable<NPath> objectFiles,
                IEnumerable<PrecompiledLibrary> nonToolchainSpecificLibraries, IEnumerable<PrecompiledLibrary> allLibraries)
            {
                yield return "-static";

                foreach (var objectFile in objectFiles)
                    yield return objectFile.InQuotes();

                yield return "-o";
                yield return destination.InQuotes();
            }
        }
    }
}
