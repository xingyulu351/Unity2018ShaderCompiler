using Bee.Core.Tests;
using Bee.NativeProgramAndToolchainTests.IntegrationTests;
using NUnit.Framework;
using Unity.BuildSystem.CompilerTests.Attributes;
using Unity.BuildSystem.NativeProgramSupport;

namespace Unity.BuildSystem.CompilerTests
{
    public class CounterTests : NativeProgramAndToolchainTestBase
    {
        private const string FeatureName = "'__COUNTER__' is supported";

        [TestCaseSource(nameof(AllToolchains_FastestBackend))]
        [CompilerFeatureName(FeatureName)]
        public void CounterGeneratesUniqueValuesOnEachUsage(ToolChain toolchain,
            BeeAPITestBackend<ToolChain> testBackend)
        {
            const string code = @"
#if __COUNTER__ == __COUNTER__
#error __COUNTER__ does not have unique values
#endif
int main() { }
";
            ManualFixtureSetup(toolchain, testBackend);
            MainCpp.WriteAllText(code);
            Build(toolChain => SetupNativeProgram(toolChain));
        }

        [TestCaseSource(nameof(AllToolchains_FastestBackend))]
        [CompilerFeatureName(FeatureName)]
        public void CounterValueCanBeAssignedToInteger(ToolChain toolchain,
            BeeAPITestBackend<ToolChain> testBackend)
        {
            const string code = @"
const int a = __COUNTER__;
int main() {
}
";
            ManualFixtureSetup(toolchain, testBackend);
            MainCpp.WriteAllText(code);
            Build(toolChain => SetupNativeProgram(toolChain));
        }
    }
}
