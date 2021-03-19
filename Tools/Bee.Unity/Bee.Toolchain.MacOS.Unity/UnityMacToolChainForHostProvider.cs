using Bee.Toolchain.Xcode;
using Unity.BuildSystem.MacSDKSupport;
using Unity.BuildSystem.NativeProgramSupport;
using Unity.BuildTools;

namespace Bee.Toolchain.MacOS.Unity
{
    public class UnityMacToolChainForHostProvider
    {
        public ToolChain Provide()
        {
            return Provide(Architecture.BestThisMachineCanRun);
        }

        public XcodeToolchain Provide(IntelArchitecture architecture, string minOSVersion = null, StdLib stdlib = StdLib.libcpp)
        {
            if (UnityMacSDKToolChain.EmbeddedSdkPath.DirectoryExists())
                return new UnityMacSDKToolChain(architecture, minOSVersion, stdlib);
            return new MacToolchain(MacSdk.Locator.UserDefaultOrDummy, architecture, minOSVersion, stdlib);
        }
    }
}
