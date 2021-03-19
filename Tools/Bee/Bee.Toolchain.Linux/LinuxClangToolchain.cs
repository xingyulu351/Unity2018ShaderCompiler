using Bee.Toolchain.GNU;
using Bee.Toolchain.LLVM;
using Unity.BuildSystem.NativeProgramSupport;
using System.Collections.Generic;
using System.Linq;

namespace Bee.Toolchain.Linux
{
    public class LinuxClangToolchain : LinuxToolchain
    {
        public override CLikeCompiler CppCompiler => new LinuxClangCompiler(this);
        public override LdDynamicLinker Linker => new LinuxLdDynamicLinker(this); // petele: todo: lld won't link with GTK right now, so fall back to ld for now
        public override NativeProgramFormat StaticLibraryFormat => new LLVMStaticLibraryFormat(this);

        // WSL needs to be explicitly enable to not cause issues for windows users by default :)
        public LinuxClangToolchain(Bee.NativeProgramSupport.Sdk sdk, IntelArchitecture architecture, bool enableWSLSupport = false)
            : base(sdk, architecture, enableWSLSupport)
        {
        }
    }
}
