using System;
using System.Collections.Generic;
using Bee.NativeProgramSupport.Core;
using Bee.NativeProgramSupport.Running;
using NiceIO;
using Unity.BuildTools;

namespace Bee.Toolchain.Android
{
    internal class AndroidRunner : RunnerImplementation<AndroidRunnerEnvironment>
    {
        internal AndroidRunner(AndroidRunnerEnvironment data) : base(data)
        {
        }

        public override Tuple<LaunchResult, int> Launch(NPath appExecutable, InvocationResult appOutputFiles, bool forwardConsoleOutput, IEnumerable<string> args = null, int timeoutMs = -1)
        {
            if (ADB.Path == null || !ADB.Path.Exists())
                return new Tuple<LaunchResult, int>(LaunchResult.DeploymentFailed, -1);

            var startTime = DateTime.Now;
            var deviceExecutable = appExecutable.FileName.InQuotes();
            var deviceDirectory = new NPath("/data/local/tmp/").Combine(appExecutable.Parent).InQuotes(); // todo: make this unique?

            using (var outputForwarder = new LaunchUtils.OutputToConsoleAndFileForwarder(forwardConsoleOutput))
            {
                var adbResult = ADB.Execute($@"shell mkdir -p {deviceDirectory}", Data.DeviceId, outputForwarder, GetRemainingTimeout(startTime, timeoutMs));
                if (adbResult.ReachedTimeout)
                    return new Tuple<LaunchResult, int>(LaunchResult.Timeout, 1);
                if (adbResult.ExitCode != 0)
                    return new Tuple<LaunchResult, int>(LaunchResult.DeploymentFailed, 1);

                adbResult = ADB.Execute($@"push {appExecutable} {deviceDirectory}", Data.DeviceId, null, GetRemainingTimeout(startTime, timeoutMs)); // Ignore output here
                if (adbResult.ReachedTimeout)
                    return new Tuple<LaunchResult, int>(LaunchResult.Timeout, 1);
                if (adbResult.ExitCode != 0)
                    return new Tuple<LaunchResult, int>(LaunchResult.DeploymentFailed, 1);
            }

            return LaunchUtils.LaunchLocal(ADB.Path, appOutputFiles, forwardConsoleOutput, new[]
            {
                "-s", Data.DeviceId,
                "shell",
                $@"""(cd {deviceDirectory} && chmod +x {deviceExecutable} && ./{deviceExecutable} {args?.SeparateWithSpace() ?? ""})"""
            }, GetRemainingTimeout(startTime, timeoutMs));
        }
    }
}
