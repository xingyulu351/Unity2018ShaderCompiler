using Unity.BuildSystem.NativeProgramSupport;

namespace Bee.Toolchain.LLVM
{
    public class LLVMStaticLibraryFormat : NativeProgramFormat
    {
        public override string Extension { get; } = "a";

        public LLVMStaticLibraryFormat(ToolChain toolchain) : base(
            new LLVMArStaticLinker(toolchain))
        {
        }
    }
}
