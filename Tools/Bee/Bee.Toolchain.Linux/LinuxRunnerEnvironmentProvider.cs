using System;
using System.Collections.Generic;
using Bee.NativeProgramSupport.Running;
using JamSharp.Runtime;
using Unity.BuildSystem.NativeProgramSupport;
using Unity.BuildTools;

namespace Bee.Toolchain.Linux
{
    [RunnerEnvironmentProviderVersion(1)]
    public class LinuxRunnerEnvironmentProvider : RunnerEnvironmentProviderImplementation<LinuxRunnerEnvironment>
    {
        private static bool IsWslInstalled()
        {
            if (!HostPlatform.IsWindows || Architecture.BestThisMachineCanRun is x86Architecture)
                return false;

            return Shell.Execute(WSLSdk.GetOrCreateProxyScriptFile(), "exit").ExitCode == 0;
        }

        public override IEnumerable<LinuxRunnerEnvironment> Provide()
        {
            if (HostPlatform.IsLinux)
                yield return new LinuxRunnerEnvironment(new Architecture[] { new x64Architecture(), new x86Architecture() }, false);
            else if (IsWslInstalled())
                yield return new LinuxRunnerEnvironment(new Architecture[] { new x64Architecture()}, true); // There is no support for launching x86 executables in Wsl
        }
    }
}
