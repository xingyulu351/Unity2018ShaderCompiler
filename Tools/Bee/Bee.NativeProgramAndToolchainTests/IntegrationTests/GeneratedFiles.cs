using Bee.Core;
using Bee.Core.Tests;
using Bee.NativeProgramAndToolchainTests.IntegrationTests;
using NiceIO;
using NUnit.Framework;
using Unity.BuildTools;

namespace Unity.BuildSystem.NativeProgramSupport.Tests.IntegrationTests
{
    public class GeneratedFiles : NativeProgramAndToolchainTestBase
    {
        [TestCaseSource(nameof(SingleToolchain_AllBackends))]
        public void GeneratedCppFile(ToolChain toolchain, BeeAPITestBackend<ToolChain> testBackend)
        {
            ManualFixtureSetup(toolchain, testBackend);
            TestRoot.Combine("template").WriteAllText(@"
int main() {
    #pragma message(""Test message encountered"")
}");

            var result = Build(toolChain =>
            {
                var generated = new NPath("GeneratedWithCapitalLettersToMakeItHardOnJam.cpp");
                var template = new NPath("template");
                Backend.Current.AddAction(
                    "Copy",
                    new[] {generated},
                    new[] {template},
                    HostPlatform.IsWindows ? "copy" : "cp",
                    new[] {template.ToString(), generated.ToString()});
                SetupNativeProgram(toolChain, np => np.Sources.Add(generated));
            });
            Assert.That(result.StdOut, Does.Contain("Test message encountered"));
        }

        [TestCaseSource(nameof(SingleToolchain_AllBackends))]
        public void GeneratedHeader(ToolChain toolchain, BeeAPITestBackend<ToolChain> testBackend)
        {
            ManualFixtureSetup(toolchain, testBackend);
            TestRoot.Combine("template").WriteAllText(@"#define MYVALUE 42");
            MainCpp.WriteAllText(@"
#include ""GeneratedWithCapitalLettersToMakeItHardOnJam.h""
int main() {
#if (MYVALUE != 42)
    #error ""Generated file wasn't included""
#endif
}");

            Build(toolChain =>
            {
                var generated1 = new NPath("GeneratedWithCapitalLettersToMakeItHardOnJam.h");
                var template1 = new NPath("template");
                Backend.Current.AddAction(
                    "Copy",
                    new[] {generated1},
                    new[] {template1},
                    HostPlatform.IsWindows ? "copy" : "cp",
                    new[] {template1.ToString(), generated1.ToString()});

                //tundra backend has no .Includes() support yet.  for now generated headers have to be depended on globally.  we'll likely revisit this at some point(tm)
                SetupNativeProgram(toolChain, np => np.ExtraDependenciesForAllObjectFiles.Add(generated1));
            });
        }
    }
}
