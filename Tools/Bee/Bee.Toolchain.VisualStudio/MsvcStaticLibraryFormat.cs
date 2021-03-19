using System.Collections.Generic;
using NiceIO;
using Unity.BuildSystem.NativeProgramSupport;

namespace Unity.BuildSystem.VisualStudio
{
    public class MsvcStaticLibraryFormat : NativeProgramFormat
    {
        public override string Extension { get; } = "lib";

        public MsvcStaticLibraryFormat(ToolChain toolchain) : base(
            new MsvcStaticLinker(toolchain))
        {
        }

        private sealed class MsvcStaticLinker : StaticLinker
        {
            public MsvcStaticLinker(ToolChain toolChain) : base(toolChain) {}

            protected override IEnumerable<string> CommandLineFlagsFor(NPath destination, CodeGen codegen, IEnumerable<NPath> objectFiles,
                IEnumerable<PrecompiledLibrary> nonToolchainSpecificLibraries, IEnumerable<PrecompiledLibrary> allLibraries)
            {
                yield return "/nologo";
                foreach (var objectFile in objectFiles)
                    yield return objectFile.InQuotes();
                yield return $"/OUT:{destination.InQuotes()}";
            }
        }
    }
}
