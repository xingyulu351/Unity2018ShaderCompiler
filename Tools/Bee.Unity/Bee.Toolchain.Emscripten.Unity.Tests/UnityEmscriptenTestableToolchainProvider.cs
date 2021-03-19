using System;
using System.Collections.Generic;
using Bee.Toolchain.Emscripten;
using Unity.BuildSystem.NativeProgramSupport;
using Unity.BuildTools;

namespace Unity.BuildSystem.WebGLSupport
{
    class UnityEmscriptenTestableToolchainProvider : TestableToolchainProvider
    {
        private static readonly EmscriptenToolchain asmjs = new EmscriptenToolchain(new UnityEmscriptenSdk(), new AsmJsArchitecture()) {CreatingFunc = () => asmjs};

        public override IEnumerable<ToolChain> Provide()
        {
            if (HostPlatform.IsLinux)
                return Array.Empty<ToolChain>();
            return new[] {asmjs};
        }
    }
}
