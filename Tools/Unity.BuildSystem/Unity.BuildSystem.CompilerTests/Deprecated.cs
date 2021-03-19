using System.Text.RegularExpressions;
using Bee.Core.Tests;
using Bee.NativeProgramAndToolchainTests.IntegrationTests;
using NUnit.Framework;
using Unity.BuildSystem.CompilerTests.Attributes;
using Unity.BuildSystem.NativeProgramSupport;
using Unity.BuildTools;

namespace Unity.BuildSystem.CompilerTests
{
    public class DeprecatedTests : NativeProgramAndToolchainTestBase
    {
        [TestCaseSource(nameof(AllToolchains_FastestBackend))]
        [CompilerFeatureName("'DEPRECATED()' prints warnings when used")]
        [CompilerFeatureDescription("Uses DEPRECATED() macro to see that it does in fact print deprecation warnings when used")]
        public void WarningOnUsingDeprecatedFunction(ToolChain toolchain, BeeAPITestBackend<ToolChain> testBackend)
        {
            var code = $@"
#include ""Runtime/Utilities/Annotations.h""
DEPRECATED(""Don't use this"") void DeprecatedFunction() {{}}
int main()
{{
    DeprecatedFunction();
}}
";
            ManualFixtureSetup(toolchain, testBackend);
            MainCpp.WriteAllText(code);
            var result = Build(toolChain => SetupNativeProgram(toolChain, np => np.IncludeDirectories.Add(Paths.UnityRoot)));
            Assert.That(
                Regex.Matches(result.StdOut, "warning.*DeprecatedFunction").Count > 0,
                "No warning message about deprecated function");
            Assert.That(
                result.StdOut.Contains("Don't use this"),
                "Warning message missing 'deprecated reason' info");
        }
    }
}
