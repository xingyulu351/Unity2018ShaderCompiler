using System;
using Bee.Core;
using NiceIO;
using NUnit.Framework;
using Unity.BuildSystem.NativeProgramSupport;
using Bee.Core.Tests;

namespace Bee.NativeProgramAndToolchainTests.IntegrationTests
{
    public class RunCrashingProgram : NativeProgramAndToolchainTestBase
    {
        [TestCaseSource(nameof(RunnableToolchains_AllBackends))]
        public void Test(ToolChain toolchain, BeeAPITestBackend<ToolChain> testBackend)
        {
            ManualFixtureSetup(toolchain, testBackend);

            if (!(toolchain.Platform is WindowsPlatform || toolchain.Platform is LinuxPlatform || toolchain.Platform is MacOSXPlatform))
                Assert.Ignore("This test only needs to run for toolchains that compile for the host platform");

            TestRoot.Combine("main.cpp").WriteAllText("int main() { int* harry = nullptr; return *harry; }");

            var result = Build(toolChain =>
            {
                var generatedProgram =
                    new NativeProgram("mycrashingprogram") { Sources = {"main.cpp"} }
                    .SetupSpecificConfiguration(
                    new NativeProgramConfiguration(CodeGen.Debug, toolChain, false),
                    toolChain.ExecutableFormat);

                NPath fileThatWillNeverBeMade = "file_that_will_never_be_made";

                Backend.Current.AddAction(
                    actionName: "run_crashing_program",
                    targetFiles: new[] {fileThatWillNeverBeMade},
                    inputs: Array.Empty<NPath>(),
                    executableStringFor: generatedProgram.MakeAbsolute().ToString(SlashMode.Native),
                    commandLineArguments: new[] {"this_is_a_passed_argument"});

                Backend.Current.AddDependency(fileThatWillNeverBeMade, generatedProgram);
            }, throwOnFailure: false);

            Assert.NotZero(result.ExitCode);
            StringAssert.Contains("failed", result.StdOut);
            StringAssert.Contains("this_is_a_passed_argument", result.StdOut);
        }
    }
}
