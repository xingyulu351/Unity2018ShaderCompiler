using Bee.Core.Tests;
using Bee.NativeProgramAndToolchainTests.IntegrationTests;
using NUnit.Framework;
using Unity.BuildSystem.CompilerTests.Attributes;
using Unity.BuildSystem.NativeProgramSupport;
using Unity.BuildSystem.VisualStudio;

namespace Unity.BuildSystem.CompilerTests
{
    public class MacrosTests : NativeProgramAndToolchainTestBase
    {
        private const string MacroTestFeatureName = "Macro behavior consistency";

        private const string MacroTestFeatureDescription =
            "Does various macro checks to ensure they behave the same on all toolchains";

        [TestCaseSource(nameof(RunnableToolchains_FastestBackend))]
        [CompilerFeatureName(MacroTestFeatureName)]
        [CompilerFeatureDescription(MacroTestFeatureDescription)]
        public void MacrosBehaviourTest(ToolChain toolchain, BeeAPITestBackend<ToolChain> testBackend)
        {
            const string code = @"
const int _Q = 1;
#define PP_CAT(a, b) a##b
#define PP_CAT2(a, b) PP_CAT(a,b)
#define PP_CAT3(a, b) PP_CAT2(a,b)
#define E(a) QQ a
#define QQ() Q
#define T2() PP_CAT2(_,E(()))
#define T3() PP_CAT3(_,E(()))
int main()
{
    if (T2() == T3())
        return 18;
    return 42;
}
";
            ManualFixtureSetup(toolchain, testBackend);
            MainCpp.WriteAllText(code);
            var isVisualStudioToolchain = toolchain is VisualStudioToolchain;
            var result = Build(
                toolChain => SetupNativeProgramAndRun(toolChain, ignoreProgramExitCode: true),
                throwOnFailure: !isVisualStudioToolchain);
            if (isVisualStudioToolchain)
            {
                Assert.That(
                    result.StdOut.Contains("error C3861: '_QQ': identifier not found"),
                    "No expected compilation failure message for VisualStudioToolchain.");
            }
            else
            {
                Assert.AreEqual("18", TestRoot.Combine("program.result").ReadAllText().Trim());
            }
        }

        [TestCaseSource(nameof(AllToolchains_FastestBackend))]
        [CompilerFeatureName(MacroTestFeatureName)]
        [CompilerFeatureDescription(MacroTestFeatureDescription)]
        public void DebugConfigGets_DEBUG(ToolChain toolchain, BeeAPITestBackend<ToolChain> testBackend)
        {
            const string code = @"
#ifdef _DEBUG
int main() {
    #pragma message(""Test message: OK"")
}
#endif
";
            ManualFixtureSetup(toolchain, testBackend);
            MainCpp.WriteAllText(code);
            var result = Build(toolChain => SetupNativeProgram(toolChain));
            Assert.That(result.StdOut, Does.Contain("Test message: OK"), "Looks like _DEBUG wasn't defined");
        }
    }
}
