using System.Collections.Generic;
using Bee.NativeProgramSupport.Running;
using Unity.BuildSystem.NativeProgramSupport;
using Unity.BuildTools;

namespace Bee.Toolchain.UWP
{
    [RunnerEnvironmentProviderVersion(2)]
    public class UWPRunnerEnvironmentProvider : RunnerEnvironmentProviderImplementation<UWPRunnerEnvironment>
    {
        public override IEnumerable<UWPRunnerEnvironment> Provide()
        {
            if (!HostPlatform.IsWindows)
                yield break;

            yield return new UWPRunnerEnvironment(new Architecture[] { new x86Architecture() });
            yield return new UWPRunnerEnvironment(new Architecture[] { new x64Architecture() });
        }
    }
}
