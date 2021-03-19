using System.Collections.Generic;
using Unity.BuildSystem.NativeProgramSupport;

namespace Bee.Toolchain.Linux.Tests
{
    class LinuxTestableToolchainProvider : TestableToolchainProvider
    {
        private static readonly LinuxToolchain x86 = new LinuxGccToolchain(new LinuxSystemGccSdk(Architecture.x86), new x86Architecture(), true);
        private static readonly LinuxToolchain x64 = new LinuxGccToolchain(new LinuxSystemGccSdk(Architecture.x64), new x64Architecture(), true);

        public override IEnumerable<ToolChain> Provide() => new[] {x86, x64};
    }
}
