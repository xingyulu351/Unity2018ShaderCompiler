using System;
using System.Collections.Generic;
using System.IO;
using Bee.NativeProgramSupport.Core;
using Unity.BuildSystem.NativeProgramSupport;
using JamSharp.Runtime;
using NiceIO;
using Unity.BuildTools;

namespace Bee.NativeProgramSupport.Running
{
    public static class LaunchUtils
    {
        public class OutputToConsoleAndFileForwarder : Shell.IExecuteController, IDisposable
        {
            private readonly bool forwardConsoleOutput;
            private readonly StreamWriter stdOutFile;
            private readonly StreamWriter stdErrFile;
            private bool receivedAnyStdOut;
            private bool receivedAnyStdErr;

            protected bool RecievedAnyStdOut => receivedAnyStdOut;
            protected bool RecievedAnyStdErr => receivedAnyStdErr;

            public OutputToConsoleAndFileForwarder(bool forwardConsoleOutput, NPath stdOutFilePath = null, NPath stdErrFilePath = null)
            {
                this.forwardConsoleOutput = forwardConsoleOutput;
                stdOutFile = stdOutFilePath != null ? new StreamWriter(stdOutFilePath.ToString()) : null;
                stdErrFile = stdErrFilePath != null ? new StreamWriter(stdErrFilePath.ToString()) : null;
            }

            public virtual void OnStdoutReceived(string line)
            {
                receivedAnyStdOut = true;
                stdOutFile?.Write($"{line}\n");
                if (forwardConsoleOutput)
                    Console.Out.Write($"{line}\n");
            }

            public virtual void OnStderrReceived(string line)
            {
                receivedAnyStdErr = true;
                stdErrFile?.Write($"{line}\n");
                if (forwardConsoleOutput)
                    Console.Error.Write($"{line}\n");
            }

            public void Dispose()
            {
                stdOutFile?.Dispose();
                stdErrFile?.Dispose();
            }
        }

        // Helper function to do a local launch using the same parameters as in Launch
        public static Tuple<LaunchResult, int> LaunchLocal(
            NPath appExecutable,
            InvocationResult appOutputFiles,
            bool forwardConsoleOutput,
            IEnumerable<string> args = null,
            int timeoutMs = -1,
            NPath workingDirectory = null)
        {
            using (var executeController = new OutputToConsoleAndFileForwarder(forwardConsoleOutput, appOutputFiles.Stdout, appOutputFiles.Stderr))
                return LaunchLocal(appExecutable, appOutputFiles, executeController, args, timeoutMs, workingDirectory);
        }

        // Helper function to do a local launch using the same parameters as in Launch
        public static Tuple<LaunchResult, int> LaunchLocal(
            NPath appExecutable,
            InvocationResult appOutputFiles,
            Shell.IExecuteController executeController,
            IEnumerable<string> args = null,
            int timeoutMs = -1,
            NPath workingDirectory = null)
        {
            var executeResult = Shell.Execute(new Shell.ExecuteArgs
            {
                Executable = appExecutable.ToString(),
                Arguments = args?.SeparateWithSpace() ?? "",    // TODO: Proper command line argument escaping
                WorkingDirectory = workingDirectory?.ToString()
            }, executeController, timeoutMs);

            if (executeResult.ReachedTimeout)
                return new Tuple<LaunchResult, int>(LaunchResult.Timeout, 1);

            if (appOutputFiles.Result != null)
                appOutputFiles.Result.WriteAllText(executeResult.ExitCode.ToString());

            return new Tuple<LaunchResult, int>(executeResult.ExitCode == 0 ? LaunchResult.Success : LaunchResult.QuitWithError, executeResult.ExitCode);
        }
    }
}
