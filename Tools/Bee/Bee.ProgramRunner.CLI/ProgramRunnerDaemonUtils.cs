using System;
using System.Collections.Generic;
using System.Diagnostics;
using System.IO;
using System.Linq;
using System.Runtime.InteropServices;
using System.Runtime.Remoting.Messaging;
using System.Security.Principal;
using System.Text;
using System.Threading;
using Bee.DotNet;
using Bee.NativeProgramSupport.Running;
using JamSharp.Runtime;
using Newtonsoft.Json;
using Newtonsoft.Json.Converters;
using NiceIO;
using Unity.BuildTools;

namespace Bee.ProgramRunner.CLI
{
    internal static class DaemonUtils
    {
        private const int DaemonStartupWaitTimeoutMs = 10000;

        public static DaemonProcessInfo ReadDaemonProcessInfoFile()
        {
            try
            {
                return (DaemonProcessInfo)JsonConvert.DeserializeObject(DaemonProgram.DaemonProcessInfoFile.ReadAllText(), RunnerEnvironment.JsonSerializerSettings);
            }
            catch
            {
                return new DaemonProcessInfo()
                {
                    PID = 0,
                    ProgramExecutable = "",
                    Arguments = "",
                    DaemonVersion = 0,
                    RunnerEnvironmentProviderVersions = new Dictionary<string, int>(),
                    DaemonStatus = DaemonStatus.NotRunning,
                };
            }
        }

        private static Process TryGetProcess(int pid)
        {
            try
            {
                return Process.GetProcessById(pid);
            }
            catch (ArgumentException)
            {
                return null;
            }
        }

        private static void TryKillProcess(int pid)
        {
            try
            {
                using (var p = Process.GetProcessById(pid))
                {
                    p.Kill();
                    p.WaitForExit(); // Wait for exit since it may still hold the file handle.
                }
            }
            catch {}
        }

        private static bool IsDaemonCompatible(DaemonProcessInfo daemonProcessInfo, string arguments)
        {
            if (daemonProcessInfo.Arguments != arguments)
                return false;
            if (daemonProcessInfo.DaemonVersion < DaemonProgram.DaemonVersion)
                return false;
            if (daemonProcessInfo.RunnerEnvironmentProviderVersions == null)
                return false;

            foreach (KeyValuePair<string, int> localProvider in DaemonProgram.RunnerEnvironmentVersions.Value)
            {
                if (!daemonProcessInfo.RunnerEnvironmentProviderVersions.TryGetValue(localProvider.Key, out int version) || version < localProvider.Value)
                    return false; // Running Daemon does not support one of our providers or has an older version of it.
            }

            return true;
        }

        private static bool WaitForInfoFileToBeInRunningState(int timeout, Process daemonProcess)
        {
            const int maxTimeBetweenChecksMs = 250;       // Sometimes the filewatcher may just be unreliable, we don't have time for this and check the file again.

            var timer = Stopwatch.StartNew();

            // Make sure directory exists so we can actually watch it.
            RunnerEnvironment.RunnerEnvironmentFilesDirectory.CreateDirectory();

            bool fileWatcherError = false;
            var changeEventLock = new object();
            var changeEvent = new AutoResetEvent(false);
            Action setChangeEventSafe = () =>
            {
                lock (changeEventLock)
                    changeEvent?.Set();
            };
            try
            {
                // On linux we don't get any events for the file itself, likely because it is renamed from a temp version of the file.
                // (this is the case on all platforms, but on Windows/Mac the FileSystemWatcher worked rather reliably anyways)
                // To work around this, we don't explicitely watch for a specific file but for everything that "looks" like the file including it's temp version.
                using (var filewatcher = new FileSystemWatcher(DaemonProgram.DaemonProcessInfoFile.Parent.ToString(), DaemonProgram.DaemonProcessInfoFile.FileName + "*"))
                {
                    EventHandler exitEvent = (o, e) => setChangeEventSafe();
                    FileSystemEventHandler fileEvent = (o, e) => setChangeEventSafe();
                    RenamedEventHandler fileRenamedEvent = (o, e) => setChangeEventSafe();
                    ErrorEventHandler watcherErrorEvent = (o, e) =>
                    {
                        fileWatcherError = true;
                        setChangeEventSafe();
                    };

                    daemonProcess.Exited += exitEvent;
                    filewatcher.Changed += fileEvent;
                    filewatcher.Created += fileEvent;
                    filewatcher.Renamed += fileRenamedEvent;
                    filewatcher.Error += watcherErrorEvent;
                    filewatcher.NotifyFilter = NotifyFilters.LastWrite | NotifyFilters.CreationTime | NotifyFilters.FileName;
                    filewatcher.EnableRaisingEvents = true;

                    // Now that we started watching, first thing to do is to check the file again in case the the daemonStatus changed meanwhile.
                    DaemonProcessInfo info = ReadDaemonProcessInfoFile();
                    while (!fileWatcherError && (info.DaemonStatus != DaemonStatus.Running || info.PID != daemonProcess.Id))
                    {
                        int currentTimeout = Math.Min(maxTimeBetweenChecksMs, Math.Max(0, timeout - (int)timer.ElapsedMilliseconds));

                        if (!changeEvent.WaitOne(currentTimeout))
                            return false;
                        if (daemonProcess.HasExited)
                            return false;
                        info = ReadDaemonProcessInfoFile();
                    }
                }
            }
            finally
            {
                lock (changeEventLock)
                {
                    changeEvent.Dispose();
                    changeEvent = null;
                }
            }

            return !fileWatcherError;
        }

        // Do not use this overload directly, it is mainly meant for easier testing.
        // Note that we don't pass a DotNetRunnableProgram, since we need the original executable path to verify potentially running daemons.
        public static int EnsureCorrectDaemonIsRunning(NPath daemonExecutable, string daemonProgramArguments)
        {
            var timer = Stopwatch.StartNew();
            while (timer.ElapsedMilliseconds < DaemonStartupWaitTimeoutMs)
            {
                var info = ReadDaemonProcessInfoFile();

                Process process = null;
                try
                {
                    if (info.DaemonStatus != DaemonStatus.NotRunning)
                    {
                        if (IsDaemonCompatible(info, daemonProgramArguments))
                        {
                            // Don't trust the file, it might be stale and the daemon who wrote there long dead.
                            process = TryGetProcess(info.PID);
                            if (process != null && !process.HasExited)
                            {
                                // Best/normal case: The file says the correct process is in running state and we verified that this process exists.
                                // Note though that it might still be starting up, thus the check for status == DaemonStatus.Running
                                if (info.DaemonStatus == DaemonStatus.Running)
                                    return info.PID;
                            }
                            else
                            {
                                process?.Dispose();
                                process = null;
                            }
                        }
                        else
                            TryKillProcess(info.PID);
                    }

                    // At this point there is either no or the supposedly right Daemon running and still starting up.

                    // Create directory if it does not exist yet so we can watch it later.
                    DaemonProgram.DaemonProcessInfoFile.Parent.CreateDirectory();

                    if (process == null)
                    {
                        // On windows it is not possibly to change an executable/assembly that is already running.
                        // This is a huge problem for development since whenever a Daemon is running we cannot build.
                        // To work around this we copy all the files to the folder where we store the runnerenvironments and run from there instead.
                        NPath redirectedDaemonExecutable = daemonExecutable;
                        if (HostPlatform.IsWindows)
                        {
                            redirectedDaemonExecutable = DaemonProgram.DaemonProcessInfoFile.Parent.Combine("RunningInstance").Combine(daemonExecutable.FileName);
                            try
                            {
                                // Deleting the old folder might be a good idea due to stale/renamed assemblies, but another process/thread might attempt to delete it then shortly after we started the process, giving us a lot of trouble if some of the assemblies weren't loaded yet.
                                //redirectedDaemonExecutable.Parent.DeleteIfExists();
                                daemonExecutable.Parent.CopyFiles(redirectedDaemonExecutable.Parent, false, path => path.Extension == "exe" || path.Extension == "dll");
                            }
                            catch
                            {
                                // If copying fails, then most likely because someone else is attempting the same right now or the process was already started meanwhile, so we just retry checking for the process etc.
                                continue;
                            }
                        }

                        string invocationString = new DotNetRunnableProgram(new DotNetAssembly(redirectedDaemonExecutable, Framework.Framework46)).InvocationString;
                        Shell.SplitInvocationStringToExecutableAndArgs($"{invocationString} {daemonProgramArguments}", out string executable, out string arguments); // The invocation string from DotNetRunnableProgram may already contain arguments.

                        process = new Process();
                        process.StartInfo = new ProcessStartInfo()
                        {
                            Arguments = arguments.Trim(),
                            CreateNoWindow = true,
                            FileName = executable,

                            // Use ShellExecute so we don't inherit any file handles etc.
                            UseShellExecute = true,
                            WindowStyle = ProcessWindowStyle.Hidden,
                        };
                        process.EnableRaisingEvents = true;
                        process.Start();
                    }

                    if (WaitForInfoFileToBeInRunningState(Math.Max(0, DaemonStartupWaitTimeoutMs - (int)timer.ElapsedMilliseconds), process))
                        return process.Id;

                    // If the process exited cleanly, it means usually means that it exited because someone else took the lock already.
                    // In that case we just try the whole procedure again until we timeout (there is just nothing we can safely assume at this point).
                    // If it exited and failed on the other hand we give up immediately:
                    if (process.HasExited && process.ExitCode != 0 &&
                        process.ExitCode != 259) // ExitCode 259 is STILL_ACTIVE, can apparently happen (did during stress-testing on Mac) if the process is about to be killed, but not quite dead (?).
                    {
                        return -process.ExitCode;
                    }
                }
                finally
                {
                    process?.Dispose();
                }
            }

            return Int32.MinValue;
        }
    }
}
