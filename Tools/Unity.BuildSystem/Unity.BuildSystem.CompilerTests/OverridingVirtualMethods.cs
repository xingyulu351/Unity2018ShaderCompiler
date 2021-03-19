using Bee.Core.Tests;
using Bee.NativeProgramAndToolchainTests.IntegrationTests;
using NUnit.Framework;
using Unity.BuildSystem.CompilerTests.Attributes;
using Unity.BuildSystem.NativeProgramSupport;

namespace Unity.BuildSystem.CompilerTests
{
    public class OverridingVirtualMethodsTests : NativeProgramAndToolchainTestBase
    {
        private const string OverrideFeatureName = "'override' is supported";

        private const string OverrideFeatureDescription =
            "Checks the existance of 'override' as well that it works as it should (both proper and improper usage)";
        private const string OverridingVirtualMethodsBaseClass = @"
class Base
{
    virtual void VirtualMethod();
    void NonVirtualMethod();
};
";

        [TestCaseSource(nameof(AllToolchains_FastestBackend))]
        [CompilerFeatureName(OverrideFeatureName)]
        [CompilerFeatureDescription(OverrideFeatureDescription)]
        public void OverridingVirtualMethods_Succeeds(ToolChain toolchain, BeeAPITestBackend<ToolChain> testBackend)
        {
            var code = $@"
{OverridingVirtualMethodsBaseClass}

class Derived : Base
{{
    void VirtualMethod() override;
    void NonVirtualMethod();
}};

int main() {{ }}
";
            ManualFixtureSetup(toolchain, testBackend);
            MainCpp.WriteAllText(code);
            Build(toolChain => SetupNativeProgram(toolChain));
        }

        [TestCaseSource(nameof(AllToolchains_FastestBackend))]
        [CompilerFeatureName(OverrideFeatureName)]
        [CompilerFeatureDescription(OverrideFeatureDescription)]
        [CompilerTestFailureByDefault]
        public void OverridingNonVirtualMethods_Fails(ToolChain toolchain, BeeAPITestBackend<ToolChain> testBackend)
        {
            var code = $@"
{OverridingVirtualMethodsBaseClass}

class Derived : Base
{{
    void VirtualMethod();
    void NonVirtualMethod() override;
}};
";
            ManualFixtureSetup(toolchain, testBackend);
            MainCpp.WriteAllText(code);
            var result = Build(
                toolChain => SetupNativeProgram(toolChain),
                throwOnFailure: false);
            Assert.That(result.StdOut, Does.Contain("error").And.Contain("override").And.Contain("NonVirtualMethod"),
                "No error message about overriding non virtual method");
        }
    }
}
