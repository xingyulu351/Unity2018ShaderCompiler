using NiceIO;
using Unity.BuildSystem.NativeProgramSupport;
using Unity.BuildTools;
using Bee.Toolchain.GNU;
using Bee.Toolchain.LLVM;

namespace Bee.Toolchain.Linux
{
    public class LinuxLLVMDynamicLinker : LLVMDynamicLinker
    {
        protected override bool SupportsResponseFile => !(Toolchain.Sdk is WSLSdk);

        public LinuxLLVMDynamicLinker(LinuxToolchain toolchain) : base(toolchain, true) {}
    }

    public class LinuxLdDynamicLinker : LdDynamicLinker
    {
        protected override bool SupportsResponseFile => !(Toolchain.Sdk is WSLSdk);

        public LinuxLdDynamicLinker(LinuxToolchain toolchain) : base(toolchain, true) {}
    }
}
