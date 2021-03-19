using System;
using System.Collections.Generic;
using System.Linq;
using Bee.NativeProgramSupport.Core;
using Bee.NativeProgramSupport.Running;
using NiceIO;
using Unity.BuildTools;

namespace Bee.Toolchain.Linux
{
    // Also handles execution in WSL
    internal class LinuxRunner : HostMachineRunner<LinuxRunnerEnvironment>
    {
        public override Tuple<LaunchResult, int> Launch(NPath appExecutable, InvocationResult appOutputFiles, bool forwardConsoleOutput, IEnumerable<string> args = null, int timeoutMs = -1)
        {
            if (!Data.IsWSL)
                return LaunchUtils.LaunchLocal(appExecutable, appOutputFiles, forwardConsoleOutput, args, timeoutMs);

            return LaunchUtils.LaunchLocal(WSLSdk.GetOrCreateProxyScriptFile(), appOutputFiles, forwardConsoleOutput,
                appExecutable.MakeAbsolute().InQuotes(SlashMode.Native).AsEnumerable().Concat(args ?? Array.Empty<string>()), timeoutMs);
        }

        internal LinuxRunner(LinuxRunnerEnvironment data) : base(data)
        {
        }
    }
}
