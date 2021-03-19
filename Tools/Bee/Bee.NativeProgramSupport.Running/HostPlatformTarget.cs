using System;
using System.Collections.Generic;
using Bee.NativeProgramSupport.Core;
using Unity.BuildSystem.NativeProgramSupport;
using NiceIO;

namespace Bee.NativeProgramSupport.Running
{
    public abstract class HostMachineRunner<T> : RunnerImplementation<T>
        where T : RunnerEnvironment
    {
        protected HostMachineRunner(T data) : base(data)
        {
        }

        public override Tuple<LaunchResult, int> Launch(
            NPath appExecutable,
            InvocationResult appOutputFiles,
            bool forwardConsoleOutput,
            IEnumerable<string> args = null,
            int timeoutMs = -1)
        {
            return LaunchUtils.LaunchLocal(appExecutable, appOutputFiles, forwardConsoleOutput, args, timeoutMs);
        }
    }
}
