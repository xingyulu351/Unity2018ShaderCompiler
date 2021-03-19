using System.Collections.Generic;
using Bee.NativeProgramSupport.Running;
using Unity.BuildSystem.NativeProgramSupport;
using Unity.BuildTools;

namespace Bee.Toolchain.MacOS
{
    public class MacOsRunnerEnvironment : RunnerEnvironment
    {
        public MacOsRunnerEnvironment(Architecture[] supportedArchitectures) : base(new MacOSXPlatform(), supportedArchitectures, null)
        {
        }

        public override NativeProgramSupport.Running.Runner InstantiateRunner(string runnerArgs = null) => new MacOsRunner(this);
    }

    internal class MacOsRunner : HostMachineRunner<MacOsRunnerEnvironment>
    {
        internal MacOsRunner(MacOsRunnerEnvironment data) : base(data)
        {
        }
    }

    [RunnerEnvironmentProviderVersion(1)]
    public class MacOsRunnerEnvironmentProvider : RunnerEnvironmentProviderImplementation<MacOsRunnerEnvironment>
    {
        public override IEnumerable<MacOsRunnerEnvironment> Provide()
        {
            if (!HostPlatform.IsOSX)
                yield break;

            yield return new MacOsRunnerEnvironment(new Architecture[] { new x64Architecture(), new x86Architecture() });
        }
    }
}
