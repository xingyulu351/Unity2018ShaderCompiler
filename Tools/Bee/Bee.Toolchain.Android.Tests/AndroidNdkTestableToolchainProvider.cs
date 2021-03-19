using System.Collections.Generic;
using Unity.BuildSystem.NativeProgramSupport;

namespace Bee.Toolchain.Android
{
    class AndroidNdkTestableToolchainProvider : TestableToolchainProvider
    {
        static ToolChain arm7() => new AndroidNdkToolchain(AndroidNdk.LocatorArmv7.UserDefaultOrLatest) {CreatingFunc = arm7};
        static ToolChain arm64() => new AndroidNdkToolchain(AndroidNdk.LocatorArm64.UserDefaultOrLatest) {CreatingFunc = arm64};
        static ToolChain x64() => new AndroidNdkToolchain(AndroidNdk.Locatorx86.UserDefaultOrLatest) {CreatingFunc = x64};

        public override IEnumerable<ToolChain> Provide() => new[] {arm7(), arm64(), x64()};
    }
}
