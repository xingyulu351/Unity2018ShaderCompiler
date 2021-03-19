using Bee.Toolchain.GNU;
using Unity.BuildSystem.NativeProgramSupport;
using System.Collections.Generic;
using System.Linq;

namespace Bee.Toolchain.Linux
{
    public class LinuxGccToolchain : LinuxToolchain
    {
        public override CLikeCompiler CppCompiler => new LinuxGccCompiler(this);
        public override LdDynamicLinker Linker => new LinuxLdDynamicLinker(this);
        public override NativeProgramFormat StaticLibraryFormat => new GNUStaticLibraryFormat(this);

        // WSL needs to be explicitly enable to not cause issues for windows users by default :)
        public LinuxGccToolchain(Bee.NativeProgramSupport.Sdk sdk, IntelArchitecture architecture, bool enableWSLSupport = false)
            : base(sdk, architecture, enableWSLSupport)
        {
        }
    }
}
