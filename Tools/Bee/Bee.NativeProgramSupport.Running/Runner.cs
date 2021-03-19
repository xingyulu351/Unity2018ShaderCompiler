using System;
using System.Collections.Generic;
using Bee.NativeProgramSupport.Core;
using Unity.BuildSystem.NativeProgramSupport;
using NiceIO;

namespace Bee.NativeProgramSupport.Running
{
    public enum LaunchResult
    {
        Success,
        DeploymentFailed,
        QuitWithError,
        Timeout,
        // Failed to launch is indicated by exception
    }

    public abstract class Runner
    {
        // Todo: Need .Net4.7 to do (LaunchResult result, int appExitCode)
        public abstract Tuple<LaunchResult, int> Launch(
            NPath appExecutable,
            InvocationResult appOutputFiles,
            bool forwardConsoleOutput,
            IEnumerable<string> args = null,
            int timeoutMs = -1);

        // Todo: Overload that takes an applicationId - implementation of applicationId is platform specific
        // Todo: Deploy, check if app with given id lives, etc.
        // Not sure if the high level launch should be implemented here and use more granular platforms specific funcs. There's pro and con to that.
    }

    public abstract class RunnerImplementation<T> : Runner
        where T : RunnerEnvironment
    {
        protected T Data { get; }

        protected RunnerImplementation(T data)
        {
            this.Data = data;
        }

        // Helper for implementing timeout in Launch().
        protected static int GetRemainingTimeout(DateTime startTime, int timeoutMs)
        {
            return timeoutMs > 0 ? Math.Max(timeoutMs - (DateTime.Now - startTime).Milliseconds, 0) : -1;
        }

        protected void WriteLogLine(string line = "")
        {
            Console.Error.Write($"[{GetType().Name}] {line}\n");
        }
    }
}
