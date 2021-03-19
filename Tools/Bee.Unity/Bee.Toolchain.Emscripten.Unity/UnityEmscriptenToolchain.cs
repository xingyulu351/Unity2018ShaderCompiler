using System;
using System.Collections.Generic;
using Bee.Core;
using Bee.Toolchain.Emscripten;
using NiceIO;
using Unity.BuildSystem.NativeProgramSupport;
using Unity.BuildTools;

namespace Unity.BuildSystem.WebGLSupport
{
    public sealed class UnityEmscriptenSdk : EmscriptenSdk
    {
        public UnityEmscriptenSdk()
        {
            Path = Paths.UnityRoot.Combine("External/Emscripten/Emscripten/builds");
            Emcc = Path.Combine("emcc");
            EmccCommand = new[]
            {
                "perl",
                Paths.UnityRoot.Combine("PlatformDependent/WebGL/emcc_with_config.pl").InQuotes(SlashMode.Native),
                Paths.UnityRoot.Combine("PlatformDependent/WebGL/emscripten.config").InQuotes(SlashMode.Native),
                HostPlatform.IsWindows
                ? Paths.UnityRoot
                    .Combine("External/Emscripten/EmscriptenSdk_Win/builds/python/2.7.5.3_64bit/python.exe")
                    .InQuotes(SlashMode.Native)
                : "python",
                Emcc.InQuotes(SlashMode.Native)
            }.SeparateWithSpace();
        }

        protected override NPath Emcc { get; }
        protected override string EmccCommand { get; }
        public override NPath Path { get; }
        public override Version Version { get; } = new Version();
        public override IEnumerable<NPath> TundraInputs { get; } = Array.Empty<NPath>();

        public override void Register(IBackend backend)
        {
            // Don't want the registration inherited from EmscriptenSdk (or any registration at all).
        }

        public override Dictionary<string, string> EnvironmentVariables { get; } = new Dictionary<string, string>();
    }
}
