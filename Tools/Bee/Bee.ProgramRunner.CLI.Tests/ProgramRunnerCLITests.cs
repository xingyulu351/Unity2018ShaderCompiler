using System.Linq;
using Unity.BuildSystem.NativeProgramSupport;
using Bee.ProgramRunner.CLI;
using Bee.TestHelpers;
using Bee.TundraBackend.Tests;
using JamSharp.Runtime;
using NiceIO;
using NUnit.Framework;

namespace Bee.ProgramRunner.Tests
{
    [TestFixture]
    public class ProgramRunnerCLITests
    {
        [Test]
        public void ListAvailableEnvironments_ListsAtLeastHostEnvironment()
        {
            var result = Shell.Execute($"{ProgramRunnerCLI.InvocationString} --list-available-environments");
            Assert.That(result.StdOut, Does.Contain(Platform.HostPlatform.Name));
        }

        [Test]
        public void CanLaunch_Succeeds_ForHostEnvironment()
        {
            Assert.That(Shell.Execute($"{ProgramRunnerCLI.InvocationString} --environment={Platform.HostPlatform.Name} --can-launch").ExitCode, Is.Zero);
        }

        [Test]
        public void CanLaunch_Succeeds_ForHostEnvironmentWithSpecificArchitectures()
        {
            Assert.That(Shell.Execute($"{ProgramRunnerCLI.InvocationString} --environment={Platform.HostPlatform.Name}@{Architecture.x64.Name},{Architecture.x86.DisplayName} --can-launch").ExitCode, Is.Zero);
        }

        [Test]
        public void CanLaunch_Fails_ForNonExistingEnvironment()
        {
            Assert.That(Shell.Execute($"{ProgramRunnerCLI.InvocationString} --environment=HAL9000 --can-launch").ExitCode, Is.Not.Zero);
        }

        // Note that launching on all platforms + reading stdout/err is tested by Bee.NativeProgramSupport.Running.Tests.LaunchIntegrationTests already (however by using InstantiateRunner directly)
        // The Launch tests here test primarily for specific flags that can be passed to the runner process.

        private NPath simpleProgramPath;
        private readonly NPath tempDir = new TempDirectory(nameof(ProgramRunnerCLITests));

        private NPath GetOrBuildSimpleProgramWithOutput()
        {
            if (simpleProgramPath != null)
                return simpleProgramPath;

            var hostToolChain = ToolchainsForTestsDiscovery.All.First(x => x.Platform == Platform.HostPlatform);

            var beeTestBackend = new BeeAPITestBackend_Tundra<ToolChain>();
            beeTestBackend.Start();
            NPath mainCppPath = beeTestBackend.TestRoot.Combine("main.cpp");

            beeTestBackend.Build(() => hostToolChain,
                toolChain =>
                {
                    mainCppPath.WriteAllText(@"
                                    #include <stdio.h>

                                    int main(int argc, char* argv[]) {
                                        printf(""appstdout"");
                                        fprintf(stderr, ""appstderr"");
                                        if (argc > 1) puts(argv[1]);
                                        if (argc > 2) puts(argv[2]);
                                        return 21;
                                    }
                    ");
                    var config = new NativeProgramConfiguration(CodeGen.Debug, toolChain, false);
                    var program = new NativeProgram("simpleprogram.exe") {Sources = {mainCppPath}};
                    simpleProgramPath = program.SetupSpecificConfiguration(config, toolChain.ExecutableFormat, targetDirectory: beeTestBackend.TestRoot);
                });

            return simpleProgramPath;
        }

        [Test]
        public void Launch_WritesOutFiles_ForwardsExitCode_DoesNotForwardStdout()
        {
            NPath stdOut = tempDir.Combine("stdout.txt");
            NPath stdErr = tempDir.Combine("stderr.txt");
            NPath exitCode = tempDir.Combine("exitcode.txt");

            var result = Shell.Execute($"{ProgramRunnerCLI.InvocationString} --environment={Platform.HostPlatform.Name} --launch" +
                $" --app-path={GetOrBuildSimpleProgramWithOutput().InQuotes()}" +
                $" --app-std={stdOut.InQuotes()}" +
                $" --app-err={stdErr.InQuotes()}" +
                $" --app-exit={exitCode.InQuotes()}");

            Assert.That(result.ExitCode, Is.EqualTo(21));
            Assert.That(result.StdOut, Does.Not.Contain("appstd"));
            Assert.That(result.StdErr, Does.Not.Contain("appstd"));

            Assert.That(stdOut.ReadAllText(), Is.EqualTo("appstdout\n"));  // Todo: There should be no newline
            Assert.That(stdErr.ReadAllText(), Is.EqualTo("appstderr\n"));  // Todo: There should be no newline
            Assert.That(exitCode.ReadAllText(), Is.EqualTo("21"));
        }

        [Test]
        public void Launch_WithIgnoreFailure_ExistsWithZero()
        {
            Assert.That(Shell.Execute($"{ProgramRunnerCLI.InvocationString} --environment={Platform.HostPlatform.Name} --launch" +
                $" --app-path={GetOrBuildSimpleProgramWithOutput().InQuotes()}" +
                $" --app-ignore-failure").ExitCode, Is.Zero);
        }

        [Test]
        public void Launch_WithForwardConsoleOutput()
        {
            var result = Shell.Execute($"{ProgramRunnerCLI.InvocationString} --environment={Platform.HostPlatform.Name} --launch" +
                $" --app-path={GetOrBuildSimpleProgramWithOutput().InQuotes()}" +
                $" --app-forward-console-output");

            Assert.That(result.StdOut, Does.Contain("appstdout"));
            Assert.That(result.StdErr, Does.Contain("appstderr"));
        }

        [Test]
        public void Launch_WithArguments()
        {
            var result = Shell.Execute($"{ProgramRunnerCLI.InvocationString} --environment={Platform.HostPlatform.Name} --launch" +
                $" --app-path={GetOrBuildSimpleProgramWithOutput().InQuotes()} --app-forward-console-output" +
                $" -- argument1 argument2");
            Assert.That(result.StdOut, Does.Contain("argument1"));
            Assert.That(result.StdOut, Does.Contain("argument2"));
        }
    }
}
