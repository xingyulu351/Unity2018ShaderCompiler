using System;
using System.Linq;
using Bee.Core.Tests;
using Bee.NativeProgramAndToolchainTests.IntegrationTests;
using NUnit.Framework;

namespace Unity.BuildSystem.NativeProgramSupport.Tests.IntegrationTests
{
    public class Exceptions : NativeProgramAndToolchainTestBase
    {
        private static bool NeedsRTTIForExceptions(NativeProgramConfiguration programConfig) =>
            programConfig.ToolChain.EnablingExceptionsRequiresRTTI;

        private static System.Collections.IEnumerable TestCaseSource_ToolchainThatCanBuild_And_RequireRTTIForExceptions()
        {
            return AllToolchains_FastestBackend().Cast<TestCaseData>().Where(
                x => ((ToolChain)x.Arguments[0]).EnablingExceptionsRequiresRTTI);
        }

        [TestCaseSource(nameof(TestCaseSource_ToolchainThatCanBuild_And_RequireRTTIForExceptions))]
        public void WithExceptions_AndRTTINotEnabled_ForToolchainsThatRequireRTTIForExceptions(ToolChain toolchain, BeeAPITestBackend<ToolChain> testBackend)
        {
            ManualFixtureSetup(toolchain, testBackend);
            MainCpp.WriteAllText(@"int main () {}"); // This is only about compiler settings, no need to have code with exceptions.
            Assert.That(() =>
                Build(toolChain => SetupNativeProgram(
                    toolChain,
                    np => np._compilerCustomizations.Add<CLikeCompilerSettings>(c => c.WithExceptions(true))),
                    throwOnFailure: true),
                Throws.Exception);
        }

        [TestCaseSource(nameof(RunnableToolchains_FastestBackend))]
        public void ExceptionThrown(ToolChain toolchain, BeeAPITestBackend<ToolChain> testBackend)
        {
            ManualFixtureSetup(toolchain, testBackend);

            MainCpp.WriteAllText(@"
#include <stdio.h>
#include <vector>

int main () {
    int ret;
    std::vector<int> vec;
    try
    {
        // Some compilers (notably some clang versions) will allow you to compile with exceptions enabled + disabled RTTI,
        // even it if breaks exception handling in the way demonstrated here. This does typically not happen with trivial types like int!
        // Throwing std::exception directly did not hit this error reliably (optimized out?), which is why we use this slower to compile solution.
        ret = vec.at(3);
    }
    catch (std::exception const& e)
    {
        printf(""ok"");
        return 0;
    }
    catch (...)
    {
        printf(""It looks like you need to enable RTTI for this toolchain when using exceptions."");
        return 1;
    }
    printf(""Exception not handled."");
    return ret; // Use ret so code is not optimized out.
}");

            BuildAndCheckProgramOutput(toolChain => SetupNativeProgramAndRun(
                toolChain,
                np =>
                {
                    np._compilerCustomizations.Add(c => c.WithExceptions(true));
                    np._compilerCustomizations.Add(NeedsRTTIForExceptions, c => c.WithRTTI(true));
                }), "ok");
        }

        [TestCaseSource(nameof(AllToolchains_FastestBackend))]
        public void WithExceptions_Supported(ToolChain toolchain, BeeAPITestBackend<ToolChain> testBackend)
        {
            ManualFixtureSetup(toolchain, testBackend);

            MainCpp.WriteAllText(@"
int main () {
    try
    {
        throw 1;
    }
    catch (int e)
    {
        // all is well
    }
}");

            Assert.That(() =>
                Build(toolChain => SetupNativeProgram(
                    toolChain,
                    np =>
                    {
                        np._compilerCustomizations.Add<CLikeCompilerSettings>(c => c.WithExceptions(true));
                        np._compilerCustomizations.Add(NeedsRTTIForExceptions, c => c.WithRTTI(true));
                    }),
                    throwOnFailure: true),
                Throws.Nothing);

            var result = Build(
                toolChain => SetupNativeProgram(
                    toolChain,
                    np => np._compilerCustomizations.Add<CLikeCompilerSettings>(c => c.WithExceptions(false))),
                throwOnFailure: false);
            Assert.That(result.StdOut, Does.Contain("cannot use 'throw' with exceptions disabled")
                .Or.Contain("exception handling disabled, use -fexceptions to enable")
                .Or.Contain("warning C4530: C++ exception")
                .Or.Contain("error  #540: support for exception handling is disabled; use --exceptions to enable"));
        }
    }
}
