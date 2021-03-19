using Bee.Core.Tests;
using Bee.NativeProgramAndToolchainTests.IntegrationTests;
using NiceIO;
using NUnit.Framework;
using Unity.BuildSystem.IntegrationTests.Framework;

namespace Unity.BuildSystem.NativeProgramSupport.Tests.IntegrationTests
{
    public class HeaderScanningTests : NativeProgramAndToolchainTestBase
    {
        [TestCaseSource(nameof(SingleToolchain_AllBackends))]
        public void ChangingCppCausesRebuild(ToolChain toolchain, BeeAPITestBackend<ToolChain> testBackend)
        {
            ManualFixtureSetup(toolchain, testBackend);
            MainCpp.WriteAllText(@"int main() { int i = 0; }");

            using (new TakeAtLeastEnoughTimeForFileSystemResolutionToIncrement())
                Build(toolChain => SetupNativeProgram(toolChain));

            MainCpp.WriteAllText(@"
#pragma message(""Message from reloaded cpp"")
int main() { int i = 1; }");
            var result = Build(toolChain => SetupNativeProgram(toolChain));
            Assert.That(result.StdOut, Does.Contain("Message from reloaded cpp"));
        }

        [TestCaseSource(nameof(SingleToolchain_AllBackends))]
        public void HeaderInSameDirectoryAsCppGetsScannedForIncludes_NoLump_Relative(ToolChain toolchain, BeeAPITestBackend<ToolChain> testBackend)
        {
            ManualFixtureSetup(toolchain, testBackend);
            BuildChangeHeaderBuildAgainAndAssertOutputChanges(SetupHeaderScanningTest(fromLump: false, includeRelative: true));
        }

        [TestCaseSource(nameof(SingleToolchain_AllBackends))]
        public void HeaderInSameDirectoryAsCppGetsScannedForIncludes_NoLump_NoRelative(ToolChain toolchain, BeeAPITestBackend<ToolChain> testBackend)
        {
            ManualFixtureSetup(toolchain, testBackend);
            BuildChangeHeaderBuildAgainAndAssertOutputChanges(SetupHeaderScanningTest(fromLump: false, includeRelative: false));
        }

        [TestCaseSource(nameof(SingleToolchain_AllBackends))]
        public void HeaderInSameDirectoryAsCppGetsScannedForIncludes_Lump_Relative(ToolChain toolchain, BeeAPITestBackend<ToolChain> testBackend)
        {
            ManualFixtureSetup(toolchain, testBackend);
            BuildChangeHeaderBuildAgainAndAssertOutputChanges(SetupHeaderScanningTest(fromLump: true, includeRelative: true));
        }

        [TestCaseSource(nameof(SingleToolchain_AllBackends))]
        public void HeaderInSameDirectoryAsCppGetsScannedForIncludes_Lump_NoRelative(ToolChain toolchain, BeeAPITestBackend<ToolChain> testBackend)
        {
            ManualFixtureSetup(toolchain, testBackend);
            BuildChangeHeaderBuildAgainAndAssertOutputChanges(SetupHeaderScanningTest(fromLump: true, includeRelative: false));
        }

        private void BuildChangeHeaderBuildAgainAndAssertOutputChanges(NPath subheader)
        {
            using (new TakeAtLeastEnoughTimeForFileSystemResolutionToIncrement())
            {
                var result1 = Build(toolChain => SetupNativeProgram(toolChain));
                Assert.That(result1.StdOut, Does.Contain("Test message: 42"));
            }

            subheader.WriteAllText("#define VALUE \"100\"");
            var result2 = Build(toolChain => SetupNativeProgram(toolChain));
            Assert.That(result2.StdOut, Does.Contain("Test message: 100"));
        }

        private NPath SetupHeaderScanningTest(bool fromLump, bool includeRelative)
        {
            //make sure to have some upper case characters in headers and paths, as that's
            //a traditional troublecase for jam's headerscanner
            var subdir = TestRoot.Combine("SubDir");
            TestRoot.EnsureDirectoryExists("artifacts/program/SubDir");

            var subheader = subdir.Combine("SubHeader.h").WriteAllText("#define VALUE \"42\"");

            var includeStr = includeRelative ? "SubHeader.h" : "SubDir/SubHeader.h";
            subdir.Combine("Header.h").WriteAllText($"#include \"{includeStr}\"");

            subdir.Combine("Main.cpp").WriteAllText(@"
#include ""Header.h""
int main()
{
    #pragma message(""Test message: "" VALUE)
}");


            if (fromLump)
            {
                //add a cpp file next to main.cpp, so that they get lumped, and we can
                //excersise the headerscanning code for lumps, which is different from regular files
                //as they do not exist yet when jam does headerscanning.
                subdir.Combine("File2.cpp").WriteAllText("void func() {}");
            }


            return subheader;
        }
    }
}
