using System;
using System.Collections.Generic;
using Unity.BuildSystem.NativeProgramSupport;
using Unity.BuildTools;

namespace Bee.Toolchain.Emscripten
{
    class EmscriptenTestableToolchainProvider : TestableToolchainProvider
    {
        private static readonly ToolChain asmjs = new EmscriptenToolchain(EmscriptenSdk.Locator.LatestSdk, new AsmJsArchitecture()) {CreatingFunc = () => asmjs};
        public override IEnumerable<ToolChain> Provide()
        {
            if (HostPlatform.IsLinux)
                return Array.Empty<ToolChain>();
            return new[] {asmjs};
        }
    }
}
