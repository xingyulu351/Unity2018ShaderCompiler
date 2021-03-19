using Bee.Core.Tests;
using Bee.NativeProgramAndToolchainTests.IntegrationTests;
using NUnit.Framework;
using Unity.BuildSystem.CompilerTests.Attributes;
using Unity.BuildSystem.NativeProgramSupport;

namespace Unity.BuildSystem.CompilerTests
{
    public class NullPtrTests : NativeProgramAndToolchainTestBase
    {
        [TestCaseSource(nameof(RunnableToolchains_FastestBackend))]
        [CompilerFeatureName("'nullptr' is supported")]
        [CompilerFeatureDescription("Simple checks to see if toolchain has built in support for nullptr")]
        public void NullPtr(ToolChain toolchain, BeeAPITestBackend<ToolChain> testBackend)
        {
            const string code = @"
#include <cstdio>
int main()
{
    int* intPtr = nullptr;
    printf(""%x"", intPtr);
}
";
            ManualFixtureSetup(toolchain, testBackend);
            MainCpp.WriteAllText(code);
            BuildAndCheckProgramOutput(
                toolChain => SetupNativeProgramAndRun(toolChain),
                expectedOutput: "0");
        }
    }
}
