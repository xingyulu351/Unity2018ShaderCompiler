using System;
using System.Collections.Generic;
using Bee.NativeProgramSupport.Core;
using Bee.Toolchain.Emscripten;
using NiceIO;
using Unity.BuildSystem.NativeProgramSupport;

namespace Bee.Toolchain.Emscripten
{
    public sealed class EmscriptenExecutableFormat : NativeProgramFormat<EmscriptenDynamicLinker>
    {
        public EmscriptenExecutableFormat AsMainModule(bool value) => WithLinkerSetting(f => f.MainModule = value);
        public EmscriptenExecutableFormat WithNoExitRuntime(bool value) => WithLinkerSetting(f => f.NoExitRuntime = value);
        public EmscriptenExecutableFormat WithSeperateAsm(bool value) => WithLinkerSetting(f => f.SeperateAsm = value);
        public EmscriptenExecutableFormat WithMemoryInitFile(bool value) => WithLinkerSetting(f => f.MemoryInitFile = value);
        public EmscriptenExecutableFormat WithEmitSymbolMap(bool value) => WithLinkerSetting(f => f.EmitSymbolMap = value);
        public EmscriptenExecutableFormat WithExtraExportedRuntimeMethods(string[] methods) => WithLinkerSetting(f => f.ExtraExportedRuntimeMethods = methods);
        public EmscriptenExecutableFormat WithJSTransforms(string[] transforms) => WithLinkerSetting(f => f.JSTransforms = transforms);
        public EmscriptenExecutableFormat WithLinkTimeOptLevel(int value) => WithLinkerSetting(f => f.LinkTimeOptLevel = value);
        public EmscriptenExecutableFormat WithDebugLevel(string value) => WithLinkerSetting(f => f.DebugLevel = value);
        public EmscriptenExecutableFormat WithOptLevel(string value) => WithLinkerSetting(f => f.OptLevel = value);

        public EmscriptenExecutableFormat WithEmscriptenSetting(string setting, string value) => WithLinkerSetting(f =>
        {
            var n = new Dictionary<string, string>(f.EmscriptenSettings);
            n[setting] = value;
            f.EmscriptenSettings = n;
        });

        public EmscriptenExecutableFormat WithoutEmscriptenSetting(string setting) => WithLinkerSetting(f =>
        {
            var n = new Dictionary<string, string>(f.EmscriptenSettings);
            n.Remove(setting);
            f.EmscriptenSettings = n;
        });

        public EmscriptenExecutableFormat WithEmscriptenSettings(Dictionary<string, string> settings) => WithLinkerSetting(f => {
            var n = new Dictionary<string, string>();
            if (settings != null)
            {
                foreach (var item in settings)
                {
                    n[item.Key] = item.Value;
                }
            }
            f.EmscriptenSettings = n;
        });

        public override string Extension { get; } = "js";

        public EmscriptenExecutableFormat(ToolChain toolchain) : base(
            new EmscriptenDynamicLinker(toolchain))
        {
        }

        private EmscriptenExecutableFormat WithLinkerSetting(Action<EmscriptenDynamicLinker> callback) => WithLinkerSetting<EmscriptenExecutableFormat>(callback);

        // Pass node.js exe from sdk on to the runner so we run with a compatible node version.
        protected override string RunnerArgs => (Toolchain.Sdk as EmscriptenSdk)?.NodeExe?.ToString();
    }
}
