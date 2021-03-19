using Unity.BuildSystem.NativeProgramSupport;

namespace Bee.Toolchain.Xcode
{
    public class LibtoolBundledStaticLibraryFormat : NativeProgramFormat
    {
        public override string Extension { get; } = "a";

        public LibtoolBundledStaticLibraryFormat(ToolChain toolchain) : base(
            new LibtoolBundleStaticLinker(toolchain))
        {
        }
    }
}
