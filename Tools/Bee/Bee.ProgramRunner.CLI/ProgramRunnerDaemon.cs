using System;
using System.Collections.Generic;
using System.Diagnostics;
using System.IO;
using System.Linq;
using System.Text.RegularExpressions;
using System.Threading;
using Bee.NativeProgramSupport.Running;
using NiceIO;
using Unity.BuildSystem.NativeProgramSupport;
using Unity.BuildTools;

namespace Bee.ProgramRunner.CLI
{
    // Public interface for interaction with the ProgramRunnerDaemon
    public static class ProgramRunnerDaemon
    {
        // Ensures that the daemon runs and returns its process id.
        public static int EnsureCorrectDaemonIsRunning()
        {
            string arguments = "run --daemon";
            return DaemonUtils.EnsureCorrectDaemonIsRunning(ProgramRunnerCLI.ProgramRunnerExePath, arguments);
        }

        public static void KillDaemonIfRunning()
        {
            var daemonProcessInfo = DaemonUtils.ReadDaemonProcessInfoFile();
            if (daemonProcessInfo.DaemonStatus == DaemonStatus.NotRunning)
                return;
            try
            {
                using (var p = Process.GetProcessById(daemonProcessInfo.PID))
                {
                    if (p.HasExited) return;
                    p.Kill();
                    p.WaitForExit();
                }
            }
            // Ignore if there is no process.
            catch (ArgumentException) {}            // This can happen because the PID may no longer exists at the time we call GetProcessById
            catch (InvalidOperationException) {}    // This can happen because the process may have died between GetProcessById and Kill (in which case Kill will complain)
        }

        private static Architecture FindArchitecture(string architectureIdentifer)
        {
            var architecture = Architecture.All.FirstOrDefault(x => x.Name.Equals(architectureIdentifer, StringComparison.InvariantCultureIgnoreCase));
            if (architecture == null)
                architecture = Architecture.All.FirstOrDefault(x => x.DisplayName.Equals(architectureIdentifer, StringComparison.InvariantCultureIgnoreCase));
            return architecture;
        }

        private static Platform FindPlatform(string platformIdentifer)
        {
            var platform = Platform.All.FirstOrDefault(x => x.Name.Equals(platformIdentifer, StringComparison.InvariantCultureIgnoreCase));
            if (platform == null)
                platform = Platform.All.FirstOrDefault(x => x.DisplayName.Equals(platformIdentifer, StringComparison.InvariantCultureIgnoreCase));
            return platform;
        }

        private static readonly Regex queryIdentifierRegex = new Regex(@"(?<platform>\w+)(\.(?<identifier>.+))?(@(?<architectures>(\w+)(,\w+)*))?", RegexOptions.ExplicitCapture);

        // A query identifier has the form: <Platform>[.<environmentId>][@<requiredArchitecture>,<requiredArchitecture>,...]
        public static IEnumerable<RunnerEnvironment> FindRunnerEnvironments(string queryIdentifier)
        {
            var matchResult = queryIdentifierRegex.Match(queryIdentifier);
            if (!matchResult.Success)
                throw new ArgumentException($"Invalid environment identifier \"{queryIdentifier}\". Expected is either a string of the form <Platform>[.<environmentId>][@<architecture>,<architecture>,...]!", nameof(queryIdentifier));

            string platformIdentifer = matchResult.Groups["platform"].Value;
            var platform = FindPlatform(platformIdentifer);
            if (platform == null)
                throw new Exception($"Did not find matching platform with identifier \"{platformIdentifer}\"");

            string environmentId = matchResult.Groups["identifier"].Value;

            IEnumerable<Architecture> requiredSupportedArchitectures = null;
            if (matchResult.Groups["architectures"].Success)
            {
                string[] architectureIdentifiers = matchResult.Groups["architectures"].Value.Split(',');
                requiredSupportedArchitectures = architectureIdentifiers.Select(x =>
                {
                    var arch = FindArchitecture(x);
                    if (arch == null)
                        throw new Exception($"Did not find matching architecture with identifier \"{x}\"");
                    return arch;
                });
            }

            return FindRunnerEnvironments(platform, requiredSupportedArchitectures, environmentId);
        }

        internal static string SafeReadAllTextFromExistingFile(NPath file)
        {
            Exception lastException = null;

            // Despite our efforts to do transactional file writes in DaemonProgram.TransactionalFileOverwrite, they are simply not transactional on windows.
            // To my knowledge there is no way to solve this without any kind of busylooping to wait on the the replace operation to finish. So here we go.
            const int maxNumAttempts = 8;
            const int waitPerAttemptMs = 5;
            for (int attempt = 0; attempt < maxNumAttempts; ++attempt)
            {
                try
                {
                    // Don't use file.ReadAllText since it lowers our change of success and may block the Daemon from replacing a file.
                    // This is because ReadAllText uses File.Share.Read only. Instead, we enforce less locking here.
                    using (var stream = new StreamReader(new FileStream(file.ToString(), FileMode.Open, FileAccess.Read, FileShare.Write | FileShare.Read | FileShare.Delete, 4096, FileOptions.SequentialScan)))
                        return stream.ReadToEnd();
                }
                catch (System.IO.IOException e)
                {
                    lastException = e;
                    Thread.Sleep(waitPerAttemptMs);
                }
            }

            throw lastException;
        }

        public static IEnumerable<RunnerEnvironment> FindRunnerEnvironments(Platform platform, IEnumerable<Architecture> requiredSupportedArchitecture = null, string environmentId = null)
        {
            EnsureCorrectDaemonIsRunning();

            string filePrefixFilter = RunnerEnvironment.RunnerEnvironmentFilePrefix + RunnerEnvironment.CreateFileIdentifier(platform, environmentId);
            IEnumerable<RunnerEnvironment> environmentQuery = RunnerEnvironment.RunnerEnvironmentFilesDirectory.Files()
                .Where(file => file.FileName.StartsWith(filePrefixFilter))
                .Select(SafeReadAllTextFromExistingFile)
                .CatchExceptions()                              // Silently ignore files we can't read
                .ToArray()                                      // Don't delay file accesses indefinitely since our file list gets outdated too quickly.
                .Select(RunnerEnvironment.DeserializeFromJson); // If we can't deserialize then something is really wrong here, because the Daemon makes sure that there are only files that itself can read.

            // Checking platform and identifier again is somewhat redundant since it was already in the file description.
            // Left here to be on the safe side since we have the data anyways.

            environmentQuery = environmentQuery.Where(x => x.Platform == platform);

            var requiredSupportedArchitectureArray = requiredSupportedArchitecture?.ToArray();
            if (requiredSupportedArchitectureArray != null && !requiredSupportedArchitectureArray.Empty())
            {
                environmentQuery = environmentQuery.Where(x => x.SupportedArchitectures
                    .Intersect(requiredSupportedArchitectureArray)
                    .Any());
            }

            if (!string.IsNullOrEmpty(environmentId))
                environmentQuery = environmentQuery.Where(x => x.EnvironmentId == environmentId);

            return environmentQuery;
        }

        public static IEnumerable<RunnerEnvironment> LoadAllRunnerEnvironments()
        {
            EnsureCorrectDaemonIsRunning();

            return RunnerEnvironment.RunnerEnvironmentFilesDirectory.Files()
                .Where(file => file.FileName.StartsWith(RunnerEnvironment.RunnerEnvironmentFilePrefix))
                .Select(SafeReadAllTextFromExistingFile)
                .CatchExceptions()                              // Silently ignore files we can't read
                .ToArray()                                      // Don't delay file accesses indefinitely since our file list gets outdated too quickly.
                .Select(RunnerEnvironment.DeserializeFromJson); // If we can't deserialize then something is really wrong here, because the Daemon makes sure that there are only files that itself can read.
        }
    }
}
