using System;
using System.Collections.Concurrent;
using System.Collections.Generic;
using System.Diagnostics;
using System.IO;
using System.Linq;
using System.Reflection;
using System.Runtime.InteropServices;
using System.Threading;
using System.Threading.Tasks;
using Bee.Core;
using Bee.NativeProgramSupport.Running;
using Newtonsoft.Json;
using NiceIO;
using Unity.BuildTools;

namespace Bee.ProgramRunner.CLI
{
    internal class DaemonProgram : IDisposable
    {
        public const int DaemonVersion = 1;

        public static readonly Lazy<Dictionary<string, int>> RunnerEnvironmentVersions = new Lazy<Dictionary<string, int>>(() =>
            DynamicLoader.GetAllTypesInAssemblies(RunnerEnvironmentProvider.AssemblyFilter)
                .Where(t => !t.IsAbstract && t.IsSubclassOf(typeof(RunnerEnvironmentProvider)))
                .ToDictionary(t => t.Name, t => t.GetCustomAttribute<RunnerEnvironmentProviderVersion>()?.Version ?? -1)
        );

        public static readonly NPath DaemonProcessInfoFile = RunnerEnvironment.RunnerEnvironmentFilesDirectory.Combine("DaemonProcessInfo.json");

        private static readonly NPath DaemonProcessLockFile = RunnerEnvironment.RunnerEnvironmentFilesDirectory.Combine(".DaemonLock");
        private const int MaxNumProviderFailures = 4;
        private FileStream lockFile;
        private DaemonProcessInfo processInfo;

        class ProviderData
        {
            public ProviderData(RunnerEnvironmentProvider provider)
            {
                this.Provider = provider;
                this.WrittenFiles = new Dictionary<NPath, string>();
            }

            public RunnerEnvironmentProvider Provider;
            public Dictionary<NPath, string> WrittenFiles;
        }

        public DaemonProgram()
        {
            var currentProcess = Process.GetCurrentProcess();
            var args = Environment.GetCommandLineArgs();

            processInfo = new DaemonProcessInfo()
            {
                PID = currentProcess.Id,
                ProgramExecutable = args[0],
                Arguments = args.Skip(1).SeparateWithSpace(),
                DaemonVersion = DaemonVersion,
                RunnerEnvironmentProviderVersions = RunnerEnvironmentVersions.Value,
                DaemonStatus = DaemonStatus.StartingUp
            };
        }

        private bool AcquireDaemonLock()
        {
            try
            {
                RunnerEnvironment.RunnerEnvironmentFilesDirectory.CreateDirectory();
                lockFile = File.Create(DaemonProcessLockFile.ToString());
                lockFile.Lock(0, 0);
            }
            catch
            {
                lockFile = null;
                return false;
            }

            return true;
        }

        // Tries to replace an existing file "as transactionally as possible"
        // This operatoion is indeed transactional on Linux/Mac, but not fully so on Windows.
        // See also ProgramRunnerDaemon.SafeReadAllTextFromExistingFile
        private static void TransactionalFileOverwrite(NPath fileToBeReplaced, string data)
        {
            NPath tempFile = fileToBeReplaced + "_tmp";
            tempFile.WriteAllText(data);

            // Using a backup file allows windows to do a rename on the old file.
            // If we do not specify it, the replaced file may not inherit all file properties!
            // See https://docs.microsoft.com/en-us/windows/desktop/api/winbase/nf-winbase-replacefilew
            // (which is what the windows implementation of File.Replace usually uses)
            string backupFile = (tempFile + ".bak").ToString();

            // Can't do WriteAllText since that wouldn't be transactional. So instead create an empty file with which we can swap places.
            if (!fileToBeReplaced.Exists())
                fileToBeReplaced.CreateFile();

            // Replacing the file can fail if somebody is reading the old file right now (very spuriously observed on Windows).
            const int maxNumAttempts = 8;
            const int waitPerAttemptMs = 25;
            Exception exception = null;
            for (int i = 0; i < maxNumAttempts; ++i)
            {
                try
                {
                    // Note that File.Delete does not throw if the file does not exist!
                    File.Delete(backupFile);
                    File.Replace(tempFile.ToString(), fileToBeReplaced.ToString(), backupFile, true);
                    File.Delete(backupFile);
                    return;
                }
                catch (IOException e)
                {
                    exception = e;
                    Thread.Sleep(waitPerAttemptMs);
                }
            }

            throw exception;
        }

        private static void WriteRunnerEnvironmentFile(RunnerEnvironment environment, ProviderData providerData)
        {
            string newJson = environment.SerializeToJson();
            NPath targetPath = environment.RunnerEnvironmentFilePath;

            if (providerData.WrittenFiles.TryGetValue(targetPath, out string existingJson))
            {
                if (existingJson == newJson)
                    return;
                providerData.WrittenFiles[targetPath] = newJson;
            }
            else
                providerData.WrittenFiles.Add(targetPath, newJson);

            TransactionalFileOverwrite(environment.RunnerEnvironmentFilePath, newJson);
        }

        private void UpdateDaemonInfoFile()
        {
            string json = JsonConvert.SerializeObject(processInfo, RunnerEnvironment.JsonSerializerSettings);
            TransactionalFileOverwrite(DaemonProcessInfoFile, json);
        }

        private static void RunnerEnvironmentUpdateThread(ProviderData providerData, int numAttemptsLeft)
        {
            try
            {
                foreach (var environment in providerData.Provider.UpdateStreamInternal())
                    WriteRunnerEnvironmentFile(environment, providerData);
            }
            catch (Exception updateException)
            {
                if (numAttemptsLeft > 0)
                {
                    ProgramRunnerCLI.WriteLogLine($"Update of {providerData.Provider.GetType().Name} failed with an exception. Will recreate type now. " +
                        $"Only {numAttemptsLeft} attempts left before stopping update thread completely. Exception:\n{updateException}");
                }
                else
                {
                    ProgramRunnerCLI.WriteLogLine($"Update of {providerData.Provider.GetType().Name} failed with an exception. No more retries left. Exception:\n{updateException}");
                }

                // Provider might be in a delicate state, so let's guard the Dispose as well as we don't want one broken provider to tear down everything.
                try
                {
                    providerData.Provider.Dispose();
                }
                catch (Exception disposeException)
                {
                    ProgramRunnerCLI.WriteLogLine($"Dispose of {providerData.Provider.GetType().Name} after update failure failed:\n{disposeException}");
                }

                if (numAttemptsLeft <= 0)
                    return;

                // No exception handler here since this worked before, reasonable to assume it works again.
                providerData.Provider = (RunnerEnvironmentProvider)Activator.CreateInstance(providerData.Provider.GetType());
                RunnerEnvironmentUpdateThread(providerData, numAttemptsLeft - 1);
            }
        }

        private static void RunnerEnvironmentUpdateThreadEntry(object environmentProviderObj)
        {
            RunnerEnvironmentUpdateThread((ProviderData)environmentProviderObj, MaxNumProviderFailures);
        }

        [DllImport("libc", SetLastError = true)]
        public static extern int setsid();
        [DllImport("libc", SetLastError = true)]
        public static extern int close(int fd);
        [DllImport("libc", SetLastError = true)]
        public static extern int open(string file, int access);

        public void CleanupStaleRunnerEnvironmentFiles(IEnumerable<NPath> updatesFiles)
        {
            Parallel.ForEach(RunnerEnvironment.RunnerEnvironmentFilesDirectory.Files()
                .Where(x => x.FileName.StartsWith(RunnerEnvironment.RunnerEnvironmentFilePrefix))
                .Except(updatesFiles),
                file =>
                {
                    try
                    {
                        RunnerEnvironment runnerEnvironment = RunnerEnvironment.DeserializeFromJson(file.ReadAllText());
                        runnerEnvironment.Status = RunnerEnvironment.EnvironmentStatus.Offline;
                        TransactionalFileOverwrite(file, runnerEnvironment.SerializeToJson());
                    }
                    catch
                    {
                        // We don't really care what went wrong, the file doesn't seem to be usable so away with it.
                        try
                        {
                            file.Delete();
                        }
                        catch
                        {
                        }
                    }
                });
        }

        public void Run()
        {
            // Acquire write lock on the lock file. If we can't, there is another process already and we trust that that one is fine as well.
            if (!AcquireDaemonLock())
                return;
            UpdateDaemonInfoFile();

            // First round of updates.
            var updatedRunnerEnvFiles = new ConcurrentBag<NPath>();
            var allProviders = DynamicLoader.FindAndCreateInstanceOfEachInAllAssemblies<RunnerEnvironmentProvider>("Bee.*.dll")
                .Select(x => new ProviderData(x))
                .ToArray();
            Parallel.ForEach(allProviders, providerData =>
            {
                foreach (var env in providerData.Provider.ProvideInternal())
                {
                    try
                    {
                        WriteRunnerEnvironmentFile(env, providerData);
                    }
                    catch{}
                    updatedRunnerEnvFiles.Add(env.RunnerEnvironmentFilePath);
                }
            });

            // Cleanup all the files we didn't update.
            CleanupStaleRunnerEnvironmentFiles(updatedRunnerEnvFiles);

            // On windows using ShellExecute isolates us enough from the parent process.
            // But on Unix we need to free this process of its parent by creating a new session and closing off stdin/out/err.
            // Do this only now, to ensure that FindAndCreateInstanceOfEachInAllAssemblies actually finds any assemblies - it will no longer be able to after the setsid call.
            if (!HostPlatform.IsWindows)
            {
                setsid();

                close(0);
                close(1);
                close(2);
                open("/dev/null", 0);
                open("/dev/null", 1);
                open("/dev/null", 2);
            }

            // Report that we are now running.
            processInfo.DaemonStatus = DaemonStatus.Running;
            UpdateDaemonInfoFile();

            // Create threads for continious update.
            foreach (var provider in allProviders)
            {
                var updateThread = new Thread(RunnerEnvironmentUpdateThreadEntry);
                updateThread.Start(provider);
            }

            // Sleep forever. Todo: Is there any way meaningful way of graceful exit?
            using (var shutdownEvent = new ManualResetEvent(false))
            {
                shutdownEvent.WaitOne();
            }
        }

        public void Dispose()
        {
            if (lockFile == null)
                return;

            processInfo.DaemonStatus = DaemonStatus.NotRunning;
            UpdateDaemonInfoFile();
            lockFile.Dispose();
            // It would be nice to delete the lock file, but for that we would need to raise our write lock, so we don't want to do that since during that time somebody else could pick up the lock.
            // Alternatively, the presence of the lock file alone would indicate a lock but this is not preferable since we could easily end up with stale files due to crashes.
        }
    }
}
