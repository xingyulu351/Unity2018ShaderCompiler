using System.Collections.Generic;
using Bee.NativeProgramSupport.Running;
using Unity.BuildSystem.NativeProgramSupport;

namespace Bee.Toolchain.Emscripten
{
    [RunnerEnvironmentProviderVersion(1)]
    public class EmscriptenRunnerEnvironmentProvider : RunnerEnvironmentProviderImplementation<EmscriptenRunnerEnvironment>
    {
        public override IEnumerable<EmscriptenRunnerEnvironment> Provide()
        {
            yield return new EmscriptenRunnerEnvironment(new Architecture[] { new AsmJsArchitecture() /*, new WasmArchitecture() */ });
        }
    }
}
