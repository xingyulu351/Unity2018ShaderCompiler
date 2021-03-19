using System;
using System.Collections.Generic;
using System.Threading;
using Bee.NativeProgramSupport.Core;
using Bee.NativeProgramSupport.Running;
using Bee.TestHelpers;
using NiceIO;
using Unity.BuildSystem.NativeProgramSupport;

namespace Bee.ProgramRunner.Tests
{
    public class DummyPlatform : Platform
    {
        public override bool HasPosix => false;
    }

    public class DummyRunnerEnvironment : RunnerEnvironment
    {
        public string SomeProperty { get; set; } = "nothing";
        public const string TestEnvironmentId = "dummy_$ %^;.123 \n@\r|/:/<?>\\";

        public DummyRunnerEnvironment() : base(new DummyPlatform(), new Architecture[] { new Arm64Architecture() }, TestEnvironmentId)
        {
        }

        public override Runner InstantiateRunner(string runnerArgs = null) => new DummyRunner(this, runnerArgs);
    }

    public class DummyRunner : RunnerImplementation<DummyRunnerEnvironment>
    {
        private readonly string runnerArgs;

        public DummyRunner(DummyRunnerEnvironment data, string runnerArgs) : base(data)
        {
            this.runnerArgs = runnerArgs;
        }

        public override Tuple<LaunchResult, int> Launch(NPath appExecutable, InvocationResult appOutputFiles, bool forwardConsoleOutput, IEnumerable<string> args = null, int timeoutMs = -1)
        {
            Console.WriteLine(runnerArgs);
            return new Tuple<LaunchResult, int>(LaunchResult.Success, 0);
        }
    }

    [RunnerEnvironmentProviderVersion(1)]
    public class ControllableDummyRunnerEnvironmentProvider : RunnerEnvironmentProviderImplementation<DummyRunnerEnvironment>
    {
        public static readonly NPath ProviderControlFile_EnabledUpdateStream = TempDirectory.BeeTestTempPath.Combine("DaemonProcessTests/Active");
        public static readonly NPath ProviderControlFile_SingleCreateEnvironment = TempDirectory.BeeTestTempPath.Combine("DaemonProcessTests/Create");
        public static readonly NPath ProviderControlFile_SingleUpdateEnvironment = TempDirectory.BeeTestTempPath.Combine("DaemonProcessTests/Update");
        public const int ReactionTimeoutMs = 1000;

        public static void DeleteAllControlFiles()
        {
            ProviderControlFile_EnabledUpdateStream.DeleteIfExists();
            ProviderControlFile_SingleCreateEnvironment.DeleteIfExists();
            ProviderControlFile_SingleUpdateEnvironment.DeleteIfExists();
        }

        public override IEnumerable<DummyRunnerEnvironment> Provide()
        {
            if (ProviderControlFile_SingleCreateEnvironment.Exists())
            {
                ProviderControlFile_SingleCreateEnvironment.DeleteIfExists();
                yield return new DummyRunnerEnvironment();
            }
        }

        public override IEnumerable<DummyRunnerEnvironment> UpdateStream()
        {
            while (ProviderControlFile_EnabledUpdateStream.Exists())
            {
                if (ProviderControlFile_SingleCreateEnvironment.Exists())
                {
                    ProviderControlFile_SingleCreateEnvironment.DeleteIfExists();
                    yield return new DummyRunnerEnvironment();
                }
                if (ProviderControlFile_SingleUpdateEnvironment.Exists())
                {
                    ProviderControlFile_SingleUpdateEnvironment.DeleteIfExists();
                    yield return new DummyRunnerEnvironment() {SomeProperty = "something"};
                }
                Thread.Sleep(0); // Fine for testing I suppose. The alternative would be a filewatcher which can be quite fiddily to get right, see DaemonUtils.EnsureCorrectDaemonIsRunning
            }
        }
    }
}
