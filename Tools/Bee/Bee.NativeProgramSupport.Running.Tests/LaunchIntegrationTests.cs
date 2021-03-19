using System.Collections.Generic;
using System.Linq;
using Bee.Core.Tests;
using Bee.NativeProgramSupport.Core;
using Bee.ProgramRunner.CLI;
using Unity.BuildSystem.NativeProgramSupport;
using Bee.TundraBackend.Tests;
using NiceIO;
using NUnit.Framework;

namespace Bee.NativeProgramSupport.Running.Tests
{
    [TestFixture]
    public class LaunchIntegrationTests
    {
        private static IEnumerable<TestCaseData> RunnableRuntimeEnvironment()
        {
            foreach (var runnerEnvironment in ProgramRunnerDaemon.LoadAllRunnerEnvironments().Where(x => x.Status == RunnerEnvironment.EnvironmentStatus.FullAccess))
            {
                // Pick any toolchain that can produce an executable that works with this runnerEnvironment.
                // We do not want to test all architectures, since we assume that the actual launching code is usually the same.
                var toolChain = ToolchainsForTestsDiscovery.All.FirstOrDefault(x => x.CanBuild && x.Platform == runnerEnvironment.Platform && runnerEnvironment.SupportedArchitectures.Contains(x.Architecture));

                if (toolChain != null)
                    yield return new TestCaseData(runnerEnvironment, toolChain);
            }
        }

        private BeeAPITestBackend<ToolChain> beeTestBackend;

        [OneTimeSetUp]
        public void Init()
        {
            beeTestBackend = new BeeAPITestBackend_Tundra<ToolChain>();
        }

        [Test, TestCaseSource(nameof(RunnableRuntimeEnvironment))]
        public void LaunchTest(RunnerEnvironment runnerEnvironment, ToolChain toolChain)
        {
            beeTestBackend.Start($"{nameof(LaunchIntegrationTests)}_{runnerEnvironment.Identifier}");
            NPath mainCppPath = beeTestBackend.TestRoot.Combine("RunnerLaunchTestMain.cpp");
            NPath programPath = null;

            beeTestBackend.Build(() => toolChain,
                _ =>
                {
                    // Build a binary that we can launch.
                    mainCppPath.WriteAllText(@"
                                    #include <stdio.h>

                                    int main(int argc, char* argv[]) {
                                        puts(argv[1]);
                                        fprintf(stderr, ""stderr"");
                                        return 21;
                                    }
                    ");

                    var config = new NativeProgramConfiguration(CodeGen.Debug, toolChain, false);
                    var program = new NativeProgram("program") {Sources = {mainCppPath}};
                    programPath = program.SetupSpecificConfiguration(config, toolChain.ExecutableFormat, targetDirectory: beeTestBackend.TestRoot);
                });

            var results = InvocationResult.For(programPath);
            runnerEnvironment.InstantiateRunner().Launch(programPath, results, true, new[] { "stdout" });

            Assert.AreEqual("21", results.Result.ReadAllText());
            Assert.That(results.Stderr.ReadAllText(), Does.Contain("stderr")); // Todo: There should be no newline, should be Is.EqualTo
            Assert.That(results.Stdout.ReadAllText(), Does.Contain("stdout")); // Todo: There should be no newline, should be Is.EqualTo
        }
    }
}
