using Bee.NativeProgramSupport.Running;
using Unity.BuildSystem.NativeProgramSupport;

namespace Bee.Toolchain.Emscripten
{
    [RunnerEnvironmentProviderVersion(1)]
    public class EmscriptenRunnerEnvironment : RunnerEnvironment
    {
        public EmscriptenRunnerEnvironment(Architecture[] supportedArchitectures) : base(new WebGLPlatform(), supportedArchitectures, "nodejs")
        {
        }

        public override NativeProgramSupport.Running.Runner InstantiateRunner(string nodeExecutableOverride = null) => new EmscriptenRunner(this, nodeExecutableOverride);
    }
}
