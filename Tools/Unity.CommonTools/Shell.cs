using System;
using System.Collections.Generic;
using System.Diagnostics;
using System.IO;
using System.Text;

namespace Unity.CommonTools
{
    public class ShellException : ApplicationException
    {
        private string m_StdOutOutput;
        private string m_StdErrOutput;

        public ShellException(string message, string stdOut, string stdErr)
            : base(message)
        {
            m_StdOutOutput = stdOut;
            m_StdErrOutput = stdErr;
        }

        public string StdOut
        {
            get
            {
                return m_StdOutOutput;
            }
        }

        public string StdErr
        {
            get
            {
                return m_StdErrOutput;
            }
        }

        public override string ToString()
        {
            StringBuilder message = new StringBuilder();
            message.AppendLine(base.ToString());
            message.AppendLine("StdOut: '" + (m_StdOutOutput == null ? "<null>" : m_StdOutOutput) + "'");
            message.AppendLine("StdErr: '" + (m_StdErrOutput == null ? "<null>" : m_StdErrOutput) + "'");
            return message.ToString();
        }
    }

    public static class Shell
    {
        public static Process Launch(string executable, string arguments)
        {
            return Launch(executable, arguments, null);
        }

        public static Process Launch(string executable, string arguments, Dictionary<string, string> environmentVariables)
        {
            var psi = StartInfoFor(executable, arguments, environmentVariables);
            return Process.Start(psi);
        }

        public static Process LaunchRedirectingOutput(string executable, string arguments, Dictionary<string, string> environmentVariables)
        {
            var psi = StartInfoFor(executable, arguments, environmentVariables);
            psi.RedirectStandardError = true;
            psi.RedirectStandardOutput = true;
            return Process.Start(psi);
        }

        public static Process LaunchRedirectingOutput(string executable, string arguments)
        {
            var psi = new ProcessStartInfo(executable)
            {
                Arguments = arguments,
                RedirectStandardError = true,
                RedirectStandardOutput = true,
                UseShellExecute = false,
                CreateNoWindow = true,
            };
            return Process.Start(psi);
        }

        public static Process LaunchRedirectingOutputReadAsync(string executable, string arguments, OutputHandler handler, OutputHandler errorHandler, string workingDirectory)
        {
            return LaunchRedirectingOutputReadAsync(executable, arguments, handler, errorHandler, workingDirectory, null);
        }

        public static Process LaunchRedirectingOutputReadAsync(string executable, string arguments, OutputHandler handler, OutputHandler errorHandler, string workingDirectory, Action<ProcessStartInfo> setupAdditionalProperties)
        {
            var p = new Process();
            p.StartInfo.FileName = executable;
            p.StartInfo.Arguments = arguments;
            p.StartInfo.RedirectStandardError = true;
            p.StartInfo.RedirectStandardOutput = true;
            p.StartInfo.UseShellExecute = false;
            p.StartInfo.CreateNoWindow = true;
            if (setupAdditionalProperties != null)
            {
                setupAdditionalProperties(p.StartInfo);
            }
            if (workingDirectory != null)
                p.StartInfo.WorkingDirectory = workingDirectory;
            p.OutputDataReceived += handler.OutputEventHandler;
            if (null != errorHandler)
                p.ErrorDataReceived += errorHandler.OutputEventHandler;
            p.Start();
            return p;
        }

        public static Process LaunchWithElevatedPrivileges(string executable, string arguments, string workingDirectory)
        {
            var p = new Process();
            p.StartInfo.FileName = executable;
            p.StartInfo.Arguments = arguments;
            p.StartInfo.UseShellExecute = true; // Note: Can't use ShellExecute = false, when using Verb = "runas"
            p.StartInfo.Verb = "runas";
            if (workingDirectory != null)
                p.StartInfo.WorkingDirectory = workingDirectory;
            p.Start();
            return p;
        }

        public static string ExecuteCapturingStdout(Action code)
        {
            var origStdout = Console.Out;
            var stdout = new StringWriter();
            try
            {
                Console.SetOut(stdout);
                code();
                return stdout.ToString();
            }
            finally
            {
                Console.SetOut(origStdout);
            }
        }

        public class ExecuteProgramParams
        {
            public ExecuteProgramParams()
            {
                TimeOut = TimeSpan.FromMinutes(5);
                WorkingDirectory = null;
                RedirectStdOutToConsole = false;
                RedirectStdErrToConsole = false;
                StdOutPrefix = "";
                StdErrPrefix = "";
            }

            public TimeSpan TimeOut { set; get; }
            public string WorkingDirectory { set; get; }

            // Redirect incoming StdOut messages to console, this useful when you want to see incoming messages while process is running
            public bool RedirectStdOutToConsole { set; get; }
            public bool RedirectStdErrToConsole { set; get; }

            // Adds a string prefix while outputing log to console
            public string StdOutPrefix { set; get; }
            public string StdErrPrefix { set; get; }
        }

        private static void KillNoException(this Process process)
        {
            try
            {
                Console.WriteLine("Killing process...");
                process.Kill();
            }
            catch (Exception ex)
            {
                Console.WriteLine("Failed to kill process: " + ex.Message);
            }
            finally
            {
                Console.WriteLine("Done");
            }
        }

        private static void PrintProcessExitInfo(Process process)
        {
            StringBuilder msg = new StringBuilder();
            if (process != null)
            {
                msg.AppendFormat("'{0} {1}' ", process.StartInfo.FileName, process.StartInfo.Arguments);

                try
                {
                    msg.AppendFormat("({0}) ", process.Id.ToString());
                }
                catch (Exception ex)
                {
                    msg.AppendFormat("(({0})) ", ex.GetType().Name);
                }

                msg.Append("has exited with code ");

                try
                {
                    msg.AppendFormat("{0}.", process.ExitCode.ToString());
                }
                catch (Exception ex)
                {
                    msg.AppendFormat("({0}).", ex.GetType().Name);
                }
            }
            var result = msg.Length > 0 ? msg.ToString() : "'Unknown' has exited";
            Console.WriteLine(result);
        }

        public static string ExecuteProgramAndGetStdout(string command, string args, out string error, out int exitCode)
        {
            return ExecuteProgramAndGetStdout(command, args, out error, out exitCode, new ExecuteProgramParams());
        }

        public static string ExecuteProgramAndGetStdout(string command, string args, out string error, out int exitCode, TimeSpan timeout)
        {
            return ExecuteProgramAndGetStdout(command, args, out error, out exitCode,
                new ExecuteProgramParams
                {
                    TimeOut = timeout
                });
        }

        public static string ExecuteProgramAndGetStdout(string command, string args, out string error, out int exitCode, TimeSpan timeout, string workingDirectory)
        {
            return ExecuteProgramAndGetStdout(command, args, out error, out exitCode,
                new ExecuteProgramParams
                {
                    TimeOut = timeout,
                    WorkingDirectory = workingDirectory
                });
        }

        public static string ExecuteProgramAndGetStdout(string command, string args, out string error, out int exitCode, ExecuteProgramParams executeParams)
        {
            Console.WriteLine("Executing {0} {1}", command, args);
            // Rearranged to avoid deadlocking, see:
            // http://msdn.microsoft.com/en-us/library/system.diagnostics.process.standardoutput.aspx
            var handler = new OutputHandler(executeParams.RedirectStdOutToConsole, executeParams.StdOutPrefix);
            var errorHandler = new OutputHandler(executeParams.RedirectStdErrToConsole, executeParams.StdErrPrefix);
            var p = LaunchRedirectingOutputReadAsync(command, args, handler, errorHandler, executeParams.WorkingDirectory);

            // Start async read of output
            p.BeginOutputReadLine();
            p.BeginErrorReadLine();

            // Wait for child to exit
            if (!p.WaitForExit((int)executeParams.TimeOut.TotalMilliseconds))
            {
                p.KillNoException();
                throw new ShellException("Command '" + command + " " + args + "' timed out (" + (int)executeParams.TimeOut.TotalMilliseconds + " ms)",
                    handler.ToString(),
                    errorHandler.ToString());
            }
            // See: http://msdn.microsoft.com/en-us/library/ty0d8k56.aspx
            // We need to wait to ensure that asynchronous event handling has been completed
            p.WaitForExit();
            exitCode = p.ExitCode;
            PrintProcessExitInfo(p);
            p.Close();
            error = errorHandler.ToString();
            return handler.ToString();
        }

        // Note: If you use this function on the build agent, you have to set UAC to NeverNotify, otherwise a dialog will popup asking for permission to run
        //       with administrator privileges
        public static int ExecuteProgramWithElevatedPrivileges(string command, string args, ExecuteProgramParams executeParams)
        {
            var p = LaunchWithElevatedPrivileges(command, args, executeParams.WorkingDirectory);

            // Wait for child to exit
            if (!p.WaitForExit((int)executeParams.TimeOut.TotalMilliseconds))
            {
                p.KillNoException();
                throw new ApplicationException("Command '" + command + " " + args + "' timed out (" + (int)executeParams.TimeOut.TotalMilliseconds + " ms)");
            }

            p.WaitForExit();
            int exitCode = p.ExitCode;
            PrintProcessExitInfo(p);
            p.Close();
            return exitCode;
        }

        public class OutputHandler
        {
            private StringBuilder OutputReader = new StringBuilder();
            private bool redirectToConsole;
            private string logPrefix;
            public OutputHandler(bool redirectToConsole, string logPrefix)
            {
                this.redirectToConsole = redirectToConsole;
                this.logPrefix = logPrefix;
            }

            public void OutputEventHandler(object sendingProcess, DataReceivedEventArgs outLine)
            {
                // Collect command output
                if (!string.IsNullOrEmpty(outLine.Data))
                {
                    if (redirectToConsole)
                        Console.WriteLine(logPrefix + outLine.Data);
                    OutputReader.Append(outLine.Data + "\n");
                }
            }

            public override string ToString()
            {
                return OutputReader.ToString();
            }
        }

        public static string ExecuteProgramAndGetStdout(string command, string args)
        {
            string error;
            int exitCode;
            return ExecuteProgramAndGetStdout(command, args, out error, out exitCode);
        }

        public static ProcessStartInfo StartInfoFor(string executable, string arguments, Dictionary<string, string> environmentVariables)
        {
            var psi = new ProcessStartInfo(executable)
            {
                Arguments = arguments,
                CreateNoWindow = true,
                UseShellExecute = false
            };
            if (environmentVariables != null)
                foreach (var kvp in environmentVariables)
                    psi.EnvironmentVariables[kvp.Key] = kvp.Value;
            return psi;
        }
    }
}
