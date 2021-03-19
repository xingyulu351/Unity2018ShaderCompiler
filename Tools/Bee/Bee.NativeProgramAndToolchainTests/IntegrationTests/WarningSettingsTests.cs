using System.Linq;
using Bee.Core.Tests;
using Bee.NativeProgramAndToolchainTests.IntegrationTests;
using JamSharp.Runtime;
using NUnit.Framework;

namespace Unity.BuildSystem.NativeProgramSupport.Tests.IntegrationTests
{
    public class WarningSettingsTests : NativeProgramAndToolchainTestBase
    {
        [TestCaseSource(nameof(AllToolchains_FastestBackend))]
        public void WarningAsWarning(ToolChain toolchain, BeeAPITestBackend<ToolChain> testBackend)
        {
            ManualFixtureSetup(toolchain, testBackend);

            WriteMainCppWithUnusedVariableWarning();
            var executeResult = Build(
                toolChain => SetupNativeProgram(toolChain, np => Tester(WarningPolicy.AsWarning, np)));
            Assert.IsTrue(DoesOutputContainAnyOf(executeResult,
                @"warning: unused variable",
                @"warning C4189: 'b': local variable is initialized but not referenced",
                @"warning  #177-D: variable ""b"" was declared but never referenced"));
            Assert.Zero(executeResult.ExitCode);
        }

        private static bool DoesOutputContainAnyOf(Shell.ExecuteResult executeResult, params string[] needles)
        {
            var combinedOuptut = executeResult.StdOut + executeResult.StdErr;
            return needles.Any(needle => combinedOuptut.Contains(needle));
        }

        [TestCaseSource(nameof(AllToolchains_FastestBackend))]
        public void WarningAsError(ToolChain toolchain, BeeAPITestBackend<ToolChain> testBackend)
        {
            ManualFixtureSetup(toolchain, testBackend);
            WriteMainCppWithUnusedVariableWarning();
            var executeResult = Build(
                toolChain => SetupNativeProgram(toolChain, np => Tester(WarningPolicy.AsError, np)),
                throwOnFailure: false);
            Assert.IsTrue(DoesOutputContainAnyOf(executeResult,
                @"error: unused variable",
                @"error C4189: 'b': local variable is initialized but not referenced",
                @"error  #177-D: variable ""b"" was declared but never referenced"));
            Assert.NotZero(executeResult.ExitCode);
        }

        [TestCaseSource(nameof(AllToolchains_FastestBackend))]
        public void WarningAsSilent(ToolChain toolchain, BeeAPITestBackend<ToolChain> testBackend)
        {
            ManualFixtureSetup(toolchain, testBackend);
            WriteMainCppWithUnusedVariableWarning();
            var executeResult = Build(
                toolChain => SetupNativeProgram(toolChain, np => Tester(WarningPolicy.Silent, np)));
            Assert.IsFalse(DoesOutputContainAnyOf(executeResult,
                @"warning: unused variable",
                @"warning C4189: 'b': local variable is initialized but not referenced",
                @"warning  #177-D: variable ""b"" was declared but never referenced"));
            Assert.Zero(executeResult.ExitCode);
        }

        private static void Tester(WarningPolicy warningPolicy, NativeProgram np)
        {
            np.CompilerSettings().Add(c =>
                c.WithWarningPolicies(new[]
                {
                    new WarningAndPolicy(c.Compiler.WarningStringForUnusedVariable, warningPolicy)
                }));
        }

        private void WriteMainCppWithUnusedVariableWarning()
        {
            MainCpp.WriteAllText("int main() { int b = 23; return 4; }");
        }
    }
}
