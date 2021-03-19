using System.Collections.Generic;
using Bee.NativeProgramSupport.Running;
using Unity.BuildSystem.NativeProgramSupport;
using Unity.BuildTools;

namespace Bee.Toolchain.Windows
{
    public class WindowsRunnerEnvironment : RunnerEnvironment
    {
        public WindowsRunnerEnvironment(Architecture[] supportedArchitectures) : base(new WindowsPlatform(), supportedArchitectures, null)
        {
        }

        public override NativeProgramSupport.Running.Runner InstantiateRunner(string runnerArgs = null) => new WindowsRunner(this);
    }

    internal class WindowsRunner : HostMachineRunner<WindowsRunnerEnvironment>
    {
        internal WindowsRunner(WindowsRunnerEnvironment data) : base(data)
        {
        }
    }

    [RunnerEnvironmentProviderVersion(1)]
    public class WindowsRunnerEnvironmentProvider : RunnerEnvironmentProviderImplementation<WindowsRunnerEnvironment>
    {
        public override IEnumerable<WindowsRunnerEnvironment> Provide()
        {
            if (!HostPlatform.IsWindows)
                yield break;

            yield return new WindowsRunnerEnvironment(new Architecture[] {new x64Architecture(), new x86Architecture()});
        }
    }
}
