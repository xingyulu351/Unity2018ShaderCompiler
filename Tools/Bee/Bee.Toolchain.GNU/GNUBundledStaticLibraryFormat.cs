using Unity.BuildSystem.NativeProgramSupport;

namespace Bee.Toolchain.GNU
{
    public class GNUBundledStaticLibraryFormat : NativeProgramFormat
    {
        public override string Extension { get; } = "a";

        public GNUBundledStaticLibraryFormat(ToolChain toolchain) : base(
            new ArBundleStaticLinker(toolchain))
        {
        }
    }
}
