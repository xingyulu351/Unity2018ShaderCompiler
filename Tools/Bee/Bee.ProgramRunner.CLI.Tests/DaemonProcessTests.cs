using System;
using System.Collections.Generic;
using System.Diagnostics;
using System.Linq;
using System.Threading;
using Bee.Core;
using Bee.NativeProgramSupport.Running;
using Bee.ProgramRunner.CLI;
using Newtonsoft.Json;
using NUnit.Framework;
using NUnit.Framework.Constraints;
using Unity.BuildSystem.NativeProgramSupport;

namespace Bee.ProgramRunner.Tests
{
    [TestFixture]
    public class DaemonProcessTests
    {
        [OneTimeSetUp, TearDown]
        public void Cleanup()
        {
            ProgramRunnerDaemon.KillDaemonIfRunning();
            ControllableDummyRunnerEnvironmentProvider.DeleteAllControlFiles();
            new DummyRunnerEnvironment().RunnerEnvironmentFilePath.DeleteIfExists();
        }

        private static bool IsProcessAlive(int pid)
        {
            try
            {
                using (var p = Process.GetProcessById(pid))
                    return !p.HasExited;
            }
            catch (ArgumentException)
            {
                return false;
            }
        }

        private void ManipulateDaemonRunnerEnvironmentProviderInfo(Func<DaemonProcessInfo, DaemonProcessInfo> manipulateFunc)
        {
            var info = DaemonUtils.ReadDaemonProcessInfoFile();
            info = manipulateFunc(info);
            var json = JsonConvert.SerializeObject(info, RunnerEnvironment.JsonSerializerSettings);
            DaemonProgram.DaemonProcessInfoFile.WriteAllText(json);
        }

        private void ManipulateDaemonRunnerEnvironmentProviderVersionInfo(Action<Dictionary<string, int>> manipulateFunc)
        {
            ManipulateDaemonRunnerEnvironmentProviderInfo(info =>
            {
                manipulateFunc(info.RunnerEnvironmentProviderVersions);
                return info;
            });
        }

        [Test]
        public void EnsureCorrectDaemonIsRunning_SpawnsProcess()
        {
            int pid = ProgramRunnerDaemon.EnsureCorrectDaemonIsRunning();
            Assert.That(pid, Is.GreaterThan(0));

            var globalDaemonStatus = DaemonUtils.ReadDaemonProcessInfoFile();
            Assert.That(globalDaemonStatus.DaemonStatus, Is.EqualTo(DaemonStatus.Running));
            Assert.That(globalDaemonStatus.PID, Is.EqualTo(pid));
            Assert.That(IsProcessAlive(globalDaemonStatus.PID));
        }

        [Test]
        public void EnsureCorrectDaemonIsRunning_DoesNotSpawnNewProcessIfAlreadyRunning()
        {
            int firstPid = ProgramRunnerDaemon.EnsureCorrectDaemonIsRunning();
            int secondPid = ProgramRunnerDaemon.EnsureCorrectDaemonIsRunning();
            Assert.That(secondPid, Is.EqualTo(firstPid));
            Assert.That(DaemonUtils.ReadDaemonProcessInfoFile().PID, Is.EqualTo(firstPid));
        }

        [Test]
        public void EnsureCorrectDaemonIsRunning_DoesNotKillDaemon_WithNewerProvider()
        {
            int firstPid = ProgramRunnerDaemon.EnsureCorrectDaemonIsRunning();
            ManipulateDaemonRunnerEnvironmentProviderVersionInfo(v => v[typeof(ControllableDummyRunnerEnvironmentProvider).Name] = 2);
            int secondPid = ProgramRunnerDaemon.EnsureCorrectDaemonIsRunning();

            Assert.That(secondPid, Is.EqualTo(firstPid));
        }

        [Test]
        public void EnsureCorrectDaemonIsRunning_DoesNotKillDaemon_WithExtraProvider()
        {
            int firstPid = ProgramRunnerDaemon.EnsureCorrectDaemonIsRunning();
            ManipulateDaemonRunnerEnvironmentProviderVersionInfo(v => v.Add("UnreachableProvider", 1));
            int secondPid = ProgramRunnerDaemon.EnsureCorrectDaemonIsRunning();

            Assert.That(secondPid, Is.EqualTo(firstPid));
        }

        [Test]
        public void EnsureCorrectDaemonIsRunning_KillsDaemon_WithMissingProvider()
        {
            int firstPid = ProgramRunnerDaemon.EnsureCorrectDaemonIsRunning();
            ManipulateDaemonRunnerEnvironmentProviderVersionInfo(v => v.Remove(typeof(ControllableDummyRunnerEnvironmentProvider).Name));
            int secondPid = ProgramRunnerDaemon.EnsureCorrectDaemonIsRunning();

            Assert.That(secondPid, Is.Not.EqualTo(firstPid));
        }

        [Test]
        public void EnsureCorrectDaemonIsRunning_KillsDaemon_WithOlderProvider()
        {
            int firstPid = ProgramRunnerDaemon.EnsureCorrectDaemonIsRunning();
            ManipulateDaemonRunnerEnvironmentProviderVersionInfo(v => v[typeof(ControllableDummyRunnerEnvironmentProvider).Name] = 0);
            int secondPid = ProgramRunnerDaemon.EnsureCorrectDaemonIsRunning();

            Assert.That(secondPid, Is.Not.EqualTo(firstPid));
        }

        [Test]
        public void EnsureCorrectDaemonIsRunning_KillsDaemon_WithOlderVersion()
        {
            int firstPid = ProgramRunnerDaemon.EnsureCorrectDaemonIsRunning();
            ManipulateDaemonRunnerEnvironmentProviderInfo(i => { i.DaemonVersion--; return i; });
            int secondPid = ProgramRunnerDaemon.EnsureCorrectDaemonIsRunning();

            Assert.That(secondPid, Is.Not.EqualTo(firstPid));
        }

        [Test]
        public void EnsureCorrectDaemonIsRunning_DoesNotKillDaemon_WithNewerVersion()
        {
            int firstPid = ProgramRunnerDaemon.EnsureCorrectDaemonIsRunning();
            ManipulateDaemonRunnerEnvironmentProviderInfo(i => { i.DaemonVersion++; return i; });
            int secondPid = ProgramRunnerDaemon.EnsureCorrectDaemonIsRunning();

            Assert.That(secondPid, Is.EqualTo(firstPid));
        }

        [Test]
        public void EnsureCorrectDaemonIsRunning_ParallelCallsSpawnOnlySingleDaemon()
        {
            const int numThreads = 8;
            Thread[] threads = new Thread[numThreads];
            int[] pids = new int[numThreads];
            for (int i = 0; i < numThreads; ++i)
            {
                int index = i;
                threads[i] = new Thread(() => pids[index] = ProgramRunnerDaemon.EnsureCorrectDaemonIsRunning());
                threads[i].Start();
            }

            foreach (var thread in threads)
                thread.Join();

            Assert.That(pids, Is.All.GreaterThan(0));
            Assert.That(pids, Is.All.EqualTo(pids[0]));
            Assert.That(IsProcessAlive(pids[0]));
        }

        [Test]
        public void CreatesRunnerEnvironmentFilesDuringStartup()
        {
            if (RunnerEnvironment.RunnerEnvironmentFilesDirectory.DirectoryExists())
                RunnerEnvironment.RunnerEnvironmentFilesDirectory.Delete();

            ProgramRunnerDaemon.EnsureCorrectDaemonIsRunning();

            Assert.That(RunnerEnvironment.RunnerEnvironmentFilesDirectory.Files().Select(x => x.FileName), Has.Some.StartWith(RunnerEnvironment.RunnerEnvironmentFilePrefix));
        }

        [Test]
        public void OnStartup_DeletesInvalidRunnerEnvironmentFile()
        {
            RunnerEnvironment.RunnerEnvironmentFilesDirectory.CreateDirectory();

            var garbageFile = RunnerEnvironment.RunnerEnvironmentFilesDirectory.Combine(RunnerEnvironment.RunnerEnvironmentFilePrefix + "garbage.json");
            garbageFile.WriteAllText("{ Not readable }");

            ProgramRunnerDaemon.EnsureCorrectDaemonIsRunning();

            Assert.That(garbageFile.Exists(), Is.False);
        }

        [Test]
        public void OnStartup_ResetsStatus()
        {
            RunnerEnvironment dummyRunnerEnvironment = new DummyRunnerEnvironment { Status = RunnerEnvironment.EnvironmentStatus.FullAccess };
            dummyRunnerEnvironment.RunnerEnvironmentFilePath.WriteAllText(dummyRunnerEnvironment.SerializeToJson());

            ProgramRunnerDaemon.EnsureCorrectDaemonIsRunning();

            dummyRunnerEnvironment = ProgramRunnerDaemon.FindRunnerEnvironments(new DummyPlatform()).FirstOrDefault();
            Assert.That(dummyRunnerEnvironment, Is.Not.Null);
            Assert.That(dummyRunnerEnvironment.Status, Is.EqualTo(RunnerEnvironment.EnvironmentStatus.Offline));
        }

        [Test]
        public void CreatesRunnerEnvironmentFileForAllRunnerEnvironments()
        {
            var allEnvironmentsFromDaemon = ProgramRunnerDaemon.LoadAllRunnerEnvironments().Select(x => x.SerializeToJson()).ToArray();
            var allEnvironmentsFromProvider = DynamicLoader.FindAndCreateInstanceOfEachInAllAssemblies<RunnerEnvironmentProvider>("Bee.Toolchain.*.dll").SelectMany(provider => provider.ProvideInternal());

            foreach (var json in allEnvironmentsFromProvider.Select(x => x.SerializeToJson()))
                Assert.Contains(json, allEnvironmentsFromDaemon);
        }

        [Test]
        public void CanRetrieveRunnerEnvironment_WithEnvironmentIdThatHasNonPathCharacters()
        {
            ControllableDummyRunnerEnvironmentProvider.ProviderControlFile_EnabledUpdateStream.CreateFile();
            ControllableDummyRunnerEnvironmentProvider.ProviderControlFile_SingleCreateEnvironment.CreateFile();
            RunnerEnvironment[] environments = null;

            Assert.That(() => environments = ProgramRunnerDaemon.FindRunnerEnvironments(new DummyPlatform()).ToArray(), Is.Not.Empty.After(ControllableDummyRunnerEnvironmentProvider.ReactionTimeoutMs, 5));
            Assert.That(environments[0], Is.Not.Null);
            Assert.That(environments[0].EnvironmentId, Is.EqualTo(DummyRunnerEnvironment.TestEnvironmentId));
        }

        [Test]
        public void UpdatesAddedRunnerEnvironment()
        {
            ControllableDummyRunnerEnvironmentProvider.ProviderControlFile_EnabledUpdateStream.CreateFile();
            Assert.That(ProgramRunnerDaemon.FindRunnerEnvironments(new DummyPlatform()), Is.Empty);    // This starts the daemon.
            ControllableDummyRunnerEnvironmentProvider.ProviderControlFile_SingleCreateEnvironment.CreateFile();
            Assert.That(() => ProgramRunnerDaemon.FindRunnerEnvironments(new DummyPlatform()), Is.Not.Empty.After(ControllableDummyRunnerEnvironmentProvider.ReactionTimeoutMs, 5));
        }

        [Test]
        public void UpdatesChangedRunnerEnvironment()
        {
            ControllableDummyRunnerEnvironmentProvider.ProviderControlFile_EnabledUpdateStream.CreateFile();
            ControllableDummyRunnerEnvironmentProvider.ProviderControlFile_SingleCreateEnvironment.CreateFile();
            ProgramRunnerDaemon.EnsureCorrectDaemonIsRunning();

            ActualValueDelegate<string> queryDummysProperty = () => (ProgramRunnerDaemon.FindRunnerEnvironments(new DummyPlatform()).FirstOrDefault() as DummyRunnerEnvironment)?.SomeProperty;
            Assert.That(queryDummysProperty, Is.EqualTo("nothing"));
            ControllableDummyRunnerEnvironmentProvider.ProviderControlFile_SingleUpdateEnvironment.CreateFile();
            Assert.That(queryDummysProperty, Is.EqualTo("something").After(ControllableDummyRunnerEnvironmentProvider.ReactionTimeoutMs, 5));
        }

        [Test]
        public void ExistingRunnerEnvFile_KeepsCorrectState_While_NewDaemonStarting()
        {
            var originalEnv = ProgramRunnerDaemon.FindRunnerEnvironments(Platform.HostPlatform).First();

            using (var cancelThreadTokenSource = new CancellationTokenSource())
            {
                // NUnit doesn't work over multiple threads, so save failure result here.
                bool noFullAccessAtSomePoint = false;
                var busyReadingThread = new Thread(() =>
                {
                    while (!noFullAccessAtSomePoint && !cancelThreadTokenSource.IsCancellationRequested)
                    {
                        try
                        {
                            var runEnv = RunnerEnvironment.DeserializeFromJson(ProgramRunnerDaemon.SafeReadAllTextFromExistingFile(originalEnv.RunnerEnvironmentFilePath));
                            noFullAccessAtSomePoint = runEnv.Status != RunnerEnvironment.EnvironmentStatus.FullAccess;
                        }
                        catch
                        {
                            noFullAccessAtSomePoint = true;
                        }
                    }
                });
                busyReadingThread.Start();

                ProgramRunnerDaemon.KillDaemonIfRunning();
                ProgramRunnerDaemon.EnsureCorrectDaemonIsRunning();

                cancelThreadTokenSource.Cancel();
                busyReadingThread.Join();

                Assert.That(noFullAccessAtSomePoint, Is.False);
            }
        }
    }
}
