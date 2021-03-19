using Unity.BuildSystem.NativeProgramSupport;

namespace Bee.Toolchain.GNU
{
    public class GNUStaticLibraryFormat : NativeProgramFormat
    {
        public override string Extension { get; } = "a";

        public GNUStaticLibraryFormat(ToolChain toolchain) : base(
            new ArStaticLinker(toolchain))
        {
        }
    }
}
