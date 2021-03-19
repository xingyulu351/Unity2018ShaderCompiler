using Bee.NativeProgramSupport.Running;
using Unity.BuildSystem.NativeProgramSupport;

namespace Bee.Toolchain.Android
{
    public class AndroidRunnerEnvironment : RunnerEnvironment
    {
        internal string DeviceId => EnvironmentId;

        public AndroidRunnerEnvironment(Architecture[] supportedArchitectures, string environmentId) : base(new AndroidPlatform(), supportedArchitectures, environmentId)
        {
        }

        public override NativeProgramSupport.Running.Runner InstantiateRunner(string runnerArgs = null) => new AndroidRunner(this);
    }
}
