using Bee.Core.Tests;
using Bee.NativeProgramAndToolchainTests.IntegrationTests;
using NUnit.Framework;
using Unity.BuildSystem.CompilerTests.Attributes;
using Unity.BuildSystem.NativeProgramSupport;

namespace Unity.BuildSystem.CompilerTests
{
    public class NestedTemplatesTests : NativeProgramAndToolchainTestBase
    {
        private const string FeatureName = "'template<>' is supported";
        [TestCaseSource(nameof(RunnableToolchains_FastestBackend))]
        [CompilerFeatureName(FeatureName)]
        public void NestedTemplates_WithNoSpacingBetweenEndBrackets_Succeeds(ToolChain toolchain, BeeAPITestBackend<ToolChain> testBackend)
        {
            const string code = @"
#include <cstdio>
template<typename T>
class SomeClass
{
public:
    T member;
};
SomeClass<SomeClass<int>> c;

int main()
{
    c.member.member = 4;
    printf(""%d"", c.member.member);
}
";
            ManualFixtureSetup(toolchain, testBackend);
            MainCpp.WriteAllText(code);
            BuildAndCheckProgramOutput(
                toolChain => SetupNativeProgramAndRun(toolChain),
                expectedOutput: "4");
        }

        [TestCaseSource(nameof(RunnableToolchains_FastestBackend))]
        [CompilerFeatureName(FeatureName)]
        public void NestedTemplates_WithSpacingBetweenEndBrackets_Succeeds(ToolChain toolchain, BeeAPITestBackend<ToolChain> testBackend)
        {
            const string code = @"
#include <cstdio>
template<typename T>
class SomeClass
{
public:
    T member;
};
SomeClass<SomeClass<int> > c;

int main()
{
    c.member.member = 4;
    printf(""%d"", c.member.member);
}
";
            ManualFixtureSetup(toolchain, testBackend);
            MainCpp.WriteAllText(code);
            BuildAndCheckProgramOutput(
                toolChain => SetupNativeProgramAndRun(toolChain),
                expectedOutput: "4");
        }
    }
}
