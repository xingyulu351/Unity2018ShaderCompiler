using System.Collections.Generic;
using Unity.BuildSystem.NativeProgramSupport;

namespace Bee.Toolchain.IOS.Tests
{
    class IOSTestableToolchainProvider : TestableToolchainProvider
    {
        static ToolChain x64_sim() => new IOSToolchain(IOSSimulatorSdk.Locator.UserDefaultOrDummy, new x64Architecture()) { CreatingFunc = x64_sim};
        static ToolChain x86_sim() => new IOSToolchain(IOSSimulatorSdk.Locator.UserDefaultOrDummy, new x86Architecture()) { CreatingFunc = x86_sim};
        static ToolChain arm64_regular() => new IOSToolchain(IOSSdk.Locator.UserDefaultOrDummy, new Arm64Architecture()) { CreatingFunc = arm64_regular};
        static ToolChain armv7_regular() =>  new IOSToolchain(IOSSdk.Locator.UserDefaultOrDummy, new ARMv7Architecture()) { CreatingFunc = armv7_regular};

        public override IEnumerable<ToolChain> Provide() => new[] {x64_sim(), x86_sim(), arm64_regular(), armv7_regular()};
    }
}
