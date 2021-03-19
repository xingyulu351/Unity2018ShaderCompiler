using Bee.NativeProgramSupport.Running;
using Unity.BuildSystem.NativeProgramSupport;

namespace Bee.Toolchain.Linux
{
    public class LinuxRunnerEnvironment : RunnerEnvironment
    {
        public bool IsWSL { get; }

        public LinuxRunnerEnvironment(Architecture[] supportedArchitectures, bool isWsl) : base(new LinuxPlatform(), supportedArchitectures, isWsl ? "wsl" : null)
        {
            // Value of this property is actually obvious from the current host platform.
            // But I'd like to have it in the json files
            // .. also we might want to support remoting at some point.
            IsWSL = isWsl;
        }

        public override NativeProgramSupport.Running.Runner InstantiateRunner(string runnerArgs = null) => new LinuxRunner(this);
    }
}
