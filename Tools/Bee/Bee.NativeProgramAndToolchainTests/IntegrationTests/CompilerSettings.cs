using Bee.Core.Tests;
using Bee.Toolchain.Emscripten;
using NUnit.Framework;
using Unity.BuildSystem.NativeProgramSupport;

namespace Bee.NativeProgramAndToolchainTests.IntegrationTests
{
    [TestFixture]
    public class CompilerSettings : NativeProgramAndToolchainTestBase
    {
        [TestCaseSource(nameof(AllToolchains_FastestBackend))]
        public void WithOutputASM_OutputsAssemblyFile(ToolChain toolchain, BeeAPITestBackend<ToolChain> testBackend)
        {
            if (toolchain is EmscriptenToolchain)
                Assert.Ignore("Emscripten compiler doesn't emit assembly files");

            ManualFixtureSetup(toolchain, testBackend);
            MainCpp.WriteAllText(@"int main() { int i = 3; i = 0; return i; }");
            Build(toolChain => SetupNativeProgram(
                toolChain,
                np => np.CompilerSettings().Add(c => c.WithOutputASM(true))));
            Assert.That(TestRoot.Combine("artifacts/program/main.s").ReadAllText(), Contains.Substring("main"));
        }

        [TestCaseSource(nameof(AllToolchains_FastestBackend))]
        public void CLanguageSettingIsRespected(ToolChain toolchain, BeeAPITestBackend<ToolChain> testBackend)
        {
            ManualFixtureSetup(toolchain, testBackend);
            var mainC = TestRoot.Combine("main.c")
                .WriteAllText(@"
int main() {
#if __cplusplus
    #error ""Got into '#if __cplusplus' for Language.C""
#endif
}");
            Build(toolChain => SetupNativeProgram(
                toolChain,
                np => np.CompilerSettings().Add(c => c.WithLanguage(Language.C), mainC)));
        }

        [TestCaseSource(nameof(AllToolchains_FastestBackend))]
        public void CppLanguageSettingIsRespected(ToolChain toolchain, BeeAPITestBackend<ToolChain> testBackend)
        {
            ManualFixtureSetup(toolchain, testBackend);
            TestRoot.Combine("main.c")
                .WriteAllText(@"
int main() {
#if !__cplusplus
    #error ""__cplusplus macro is not defined for Language.Cpp""
#endif
}");
            Build(toolChain => SetupNativeProgram(
                toolChain,
                np => np.CompilerSettings().Add(c => c.WithLanguage(Language.Cpp))));
        }
    }
}
