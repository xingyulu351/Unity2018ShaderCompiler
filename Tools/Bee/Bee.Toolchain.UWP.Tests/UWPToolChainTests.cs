using Bee.ToolchainTestInfrastructure;
using NUnit.Framework;
using Unity.BuildSystem.NativeProgramSupport;
using Unity.BuildSystem.VisualStudio.MsvcVersions;

namespace Bee.Toolchain.UWP.Tests
{
    [TestFixture]
    class UWPToolChainTests : SingleToolchainTests
    {
        protected override ToolChain ProduceToolchainToTest() => new UWPToolchain(UWPSdk.Locatorx64.UserDefaultOrDummy);

        [Test]
        public void WinRTExtensionsFlag_EnablesReferencingWinRTAPIs()
        {
            MainCpp.WriteAllText("using namespace Windows::Foundation;\n int main() { return 0; }");
            Build(toolchain => SetupNativeProgram(toolchain, np =>
            {
                np.Exceptions.Set(true);
                np.WinRTExtensions.Set(true);
            }));
        }
    }
}
