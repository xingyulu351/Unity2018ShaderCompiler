using Bee.Core.Tests;
using Bee.NativeProgramAndToolchainTests.IntegrationTests;
using NUnit.Framework;
using Unity.BuildSystem.IntegrationTests.Framework;

namespace Unity.BuildSystem.NativeProgramSupport.Tests.IntegrationTests
{
    public class LumpDependencies : NativeProgramAndToolchainTestBase
    {
        [TestCaseSource(nameof(SingleToolchain_AllBackends))]
        public void ChangingCppFileOfLump(ToolChain toolchain, BeeAPITestBackend<ToolChain> testBackend)
        {
            ManualFixtureSetup(toolchain, testBackend);

            var subdir = TestRoot.Combine("subdir").EnsureDirectoryExists();
            subdir.Combine("main.cpp").WriteAllText(@"
extern void func();

int main() {
    func();
}
");
            var file2 = subdir.Combine("file2.cpp");
            file2.WriteAllText(@"
void func() {
    #pragma message(""Test message: 2"")
}");
            using (new TakeAtLeastEnoughTimeForFileSystemResolutionToIncrement())
            {
                var result1 = Build(toolChain => SetupNativeProgram(toolChain, lump: true));
                Assert.That(result1.StdOut, Does.Contain("Test message: 2"));
            }

            file2.WriteAllText(@"
void func() {
    #pragma message(""Test message: 3"")
}");
            var result2 = Build(toolChain => SetupNativeProgram(toolChain, lump: true));
            Assert.That(result2.StdOut, Does.Contain("Test message: 3"));
        }

        [TestCaseSource(nameof(SingleToolchain_AllBackends))]
        public void ChangeHeaderIncludedByFileInLump(ToolChain toolchain, BeeAPITestBackend<ToolChain> testBackend)
        {
            ManualFixtureSetup(toolchain, testBackend);

            MainCpp.WriteAllText(@"
#include ""myheader.h""
extern void func();

int main() {
    func();
    #pragma message(""Test message: "" MYVALUE)
}
");
            TestRoot.Combine("file2.cpp").WriteAllText(@"
void func() {
    #pragma message(""Test message: 2"")
}");

            var header = TestRoot.Combine("myheader.h").WriteAllText("#define MYVALUE \"14\"");

            using (new TakeAtLeastEnoughTimeForFileSystemResolutionToIncrement())
            {
                var result1 = Build(toolChain => SetupNativeProgram(toolChain, lump: true));
                Assert.That(result1.StdOut, Does.Contain("Test message: 2").And.Contain("Test message: 14"));
            }

            header.WriteAllText("#define MYVALUE \"28\"");
            var result2 = Build(toolChain => SetupNativeProgram(toolChain, lump: true));
            Assert.That(result2.StdOut, Does.Contain("Test message: 2").And.Contain("Test message: 28"));
        }
    }
}
