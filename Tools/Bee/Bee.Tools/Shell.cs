using System;
using System.Collections.Generic;
using System.Collections.Specialized;
using System.Diagnostics;
using System.IO;
using System.Text;
using System.Text.RegularExpressions;
using System.Threading.Tasks;
using NiceIO;

namespace JamSharp.Runtime
{
    public static class Shell
    {
        static readonly object PipeLocker = new object();

        public static void SplitInvocationStringToExecutableAndArgs(string commandLine, out string executable, out string arguments)
        {
            var commandAndArgs = new Regex(@"(""[^""]*""|\S*) (.*)").Matches(commandLine);
            if (commandAndArgs.Count > 0)
            {
                executable = commandAndArgs[0].Groups[1].Value;
                arguments = commandAndArgs[0].Groups[2].Value;
            }
            else
            {
                executable = commandLine;
                arguments = "";
            }
        }

        public static ExecuteResult Execute(NPath filename, string arguments, Dictionary<string, string> envVars = null)
        {
            return Execute(filename.ToString(), arguments, envVars);
        }

        public static ExecuteResult Execute(string executable, string arguments, Dictionary<string, string> envVars = null)
        {
            var executeArgs = new ExecuteArgs()
            {
                Executable = executable,
                Arguments = arguments,
                EnvVars = envVars
            };
            return Execute(executeArgs);
        }

        public static ExecuteResult Execute(string commandLine, Dictionary<string, string> envVars = null)
        {
            var executeArgs = ExecuteArgs.FromCommandLine(commandLine);
            executeArgs.EnvVars = envVars;
            return Execute(executeArgs);
        }

        public static Task<ExecuteResult> ExecuteAsync(ExecuteArgs executeArgs)
        {
            return Task.Run(() => Execute(executeArgs));
        }

        public static ExecuteResult Execute(ExecuteArgs executeArgs, IExecuteController controller = null, int timeoutMilliseconds = -1)
        {
            using (var p = NewProcess(executeArgs))
            {
                var stdout = new StringBuilder();
                var stderr = new StringBuilder();

                var stopWatch = new Stopwatch();
                stopWatch.Start();

                if (executeArgs.StdMode == StdMode.Capture)
                {
                    p.OutputDataReceived += (sender, args) =>
                    {
                        if (args.Data == null) return;
                        stdout.AppendLine(args.Data);
                        controller?.OnStdoutReceived(args.Data);
                    };
                    p.ErrorDataReceived += (sender, args) =>
                    {
                        if (args.Data == null) return;
                        stderr.AppendLine(args.Data);
                        controller?.OnStderrReceived(args.Data);
                    };
                }

                lock (PipeLocker)
                {
                    p.Start();
                    if (executeArgs.StdMode == StdMode.Capture)
                    {
                        p.BeginOutputReadLine();
                        p.BeginErrorReadLine();
                    }
                }

                bool reachedTimeout = false;
                if (timeoutMilliseconds != -1)
                {
                    if (!p.WaitForExit(timeoutMilliseconds))
                    {
                        reachedTimeout = true;
                        try
                        {
                            p.Kill();
                        }
                        catch (InvalidOperationException)
                        {
                            // may occur if the process died after our timeout but before the Kill() call.
                        }
                    }
                }
                p.WaitForExit();

                stopWatch.Stop();

                return new ExecuteResult
                {
                    ExecuteArgs = executeArgs,
                    ExitCode = p.ExitCode,
                    StdOut = stdout.ToString().TrimEnd('\n', '\r'),
                    StdErr = stderr.ToString().TrimEnd('\n', '\r'),
                    Duration = TimeSpan.FromMilliseconds(stopWatch.ElapsedMilliseconds),
                    ReachedTimeout = reachedTimeout,
                };
            }
        }

        public static Process NewProcess(ExecuteArgs executeArgs)
        {
            var p = new Process
            {
                StartInfo =
                {
                    Arguments = executeArgs.Arguments,
                    CreateNoWindow = executeArgs.StdMode == StdMode.Capture,
                    UseShellExecute = false,
                    RedirectStandardOutput = executeArgs.StdMode == StdMode.Capture,
                    RedirectStandardInput = executeArgs.StdMode == StdMode.Capture,
                    RedirectStandardError = executeArgs.StdMode == StdMode.Capture,
                    FileName = executeArgs.Executable,
                    WorkingDirectory = executeArgs.WorkingDirectory
                }
            };
            if (executeArgs.StdMode == StdMode.Capture)
            {
                p.StartInfo.StandardErrorEncoding = Encoding.UTF8;
                p.StartInfo.StandardOutputEncoding = Encoding.UTF8;
            }

            if (executeArgs.EnvVars != null)
            {
                foreach (var envVar in executeArgs.EnvVars)
                    p.StartInfo.EnvironmentVariables[envVar.Key] = envVar.Value;
            }

            return p;
        }

        public class ExecuteArgs
        {
            public static ExecuteArgs FromCommandLine(string commandLine)
            {
                SplitInvocationStringToExecutableAndArgs(commandLine, out string executable, out string arguments);
                return new ExecuteArgs() {Executable = executable, Arguments = arguments};
            }

            public string Executable { get; set; }
            public string Arguments { get; set; }
            public Dictionary<string, string> EnvVars { get; set; } = new Dictionary<string, string>();
            public string WorkingDirectory { get; set; }
            public StdMode StdMode { get; set; } = StdMode.Capture;
        }

        public enum StdMode
        {
            Stream,
            Capture
        }

        public class ExecuteResult
        {
            public string StdOut { get; set; }
            public string StdErr { get; set; }
            public int ExitCode { get; set; }
            public TimeSpan Duration { get; set; }
            public bool ReachedTimeout { get; set; } = false; // Whether the application was killed by timeout.

            public ExecuteArgs ExecuteArgs { get; set; }

            public ExecuteResult ThrowOnFailure()
            {
                if (ExitCode != 0)
                    throw new Exception($"Failed running {ExecuteArgs.Executable} {ExecuteArgs.Arguments}. ExitCode: {ExitCode}, Output was: {StdOut+StdErr}");
                return this;
            }
        }

        public interface IExecuteController
        {
            void OnStdoutReceived(string line);
            void OnStderrReceived(string line);
        }
    }
}
