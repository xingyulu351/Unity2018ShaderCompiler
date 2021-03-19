using Unity.BuildSystem.NativeProgramSupport;
using Unity.BuildTools;
using Bee.Toolchain.Linux;

namespace Bee.Toolchain.Linux.Unity
{
    public class UnityLinuxToolchain : LinuxGccToolchain
    {
        // WSL needs to be explicitly enable to not cause issues for windows users by default :)
        public UnityLinuxToolchain(
            Bee.NativeProgramSupport.Sdk sdk,
            IntelArchitecture architecture,
            System.Func<ToolChain> creationFunc,
            bool enableWSLSupport = false)
            : base(sdk, architecture, enableWSLSupport)
        {
            CreatingFunc = creationFunc;
        }
    }
}
