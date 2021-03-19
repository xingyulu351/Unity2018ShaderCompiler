using Bee.Core.Tests;
using Bee.NativeProgramAndToolchainTests.IntegrationTests;
using NUnit.Framework;
using Unity.BuildSystem.CompilerTests.Attributes;
using Unity.BuildSystem.NativeProgramSupport;

namespace Unity.BuildSystem.CompilerTests
{
    public class StaticAssertTests : NativeProgramAndToolchainTestBase
    {
        [TestCaseSource(nameof(AllToolchains_FastestBackend))]
        [CompilerFeatureName("'static_assert' is supported")]
        [CompilerFeatureDescription("Trivial test to see if static_assert does indeed exist for all toolchains")]
        public void StaticAssert_Pass(ToolChain toolchain, BeeAPITestBackend<ToolChain> testBackend)
        {
            const string code = @"
static_assert(1 == 1, ""This is so true"");
int main() { }
";

            ManualFixtureSetup(toolchain, testBackend);
            MainCpp.WriteAllText(code);
            Build(toolChain => SetupNativeProgram(toolChain));
        }

        [TestCaseSource(nameof(AllToolchains_FastestBackend))]
        [CompilerFeatureName("'static_assert' is supported")]
        [CompilerTestFailureByDefault]
        public void StaticAssert_Fail(ToolChain toolchain, BeeAPITestBackend<ToolChain> testBackend)
        {
            const string code = @"
static_assert(1 == 0, ""This assert should fail"");
int main() { }
";

            ManualFixtureSetup(toolchain, testBackend);
            MainCpp.WriteAllText(code);
            var result = Build(
                toolChain => SetupNativeProgram(toolChain),
                throwOnFailure: false);
            Assert.That(result.StdOut, Does.Contain("This assert should fail"),
                "No error message from failed static_assert");
        }
    }
}
