using Bee.NativeProgramSupport.Building;
using Unity.BuildSystem.MacSDKSupport;
using Unity.BuildSystem.NativeProgramSupport;
using Unity.BuildTools;

namespace Bee.Toolchain.MacOS
{
    //used through reflection
    public class MacToolchainForHostProvider : ToolChainForHostProvider
    {
        public override ToolChain Provide() => HostPlatform.IsOSX ? new MacToolchain(MacSdk.Locator.UserDefaultOrDummy, Architecture.x64) : null;
    }
}
