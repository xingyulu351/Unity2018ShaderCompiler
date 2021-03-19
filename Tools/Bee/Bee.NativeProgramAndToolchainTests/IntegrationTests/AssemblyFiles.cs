using System.Linq;
using Bee.Core.Tests;
using Bee.NativeProgramAndToolchainTests.IntegrationTests;
using NUnit.Framework;

namespace Unity.BuildSystem.NativeProgramSupport.Tests.IntegrationTests
{
    public class CppAndAssemblyFiles : NativeProgramAndToolchainTestBase
    {
        [TestCaseSource(nameof(RunnableToolchains_FastestBackend))]
        [Ignore("Test depends on NASM in the Unity repository; re-enable once we have migrated it to Stevedore")]
        public void TestNasm(ToolChain toolchain, BeeAPITestBackend<ToolChain> testBackend)
        {
            ManualFixtureSetup(toolchain, testBackend);

            var compiler = toolchain.ObjectFileProducers.FirstOrDefault(l => l.SupportedExtensions.Contains(".asm"));
            if (compiler == null)
                Assert.Ignore("No .asm compiler available");
            if (!(toolchain.Architecture is x64Architecture))
                Assert.Ignore("This test is only written for use with x64 architectures");

            MainCpp.WriteAllText(@"
#include <stdio.h>

#define PLATFORM_REQUIRES_PREFIX __ORBIS__

#if PLATFORM_REQUIRES_PREFIX
#    define getvalue _getvalue
#endif

extern ""C"" {
    int getvalue();
}

int main() {
    printf(""%d"", getvalue());
}
");
            TestRoot.Combine("assembly.asm").WriteAllText($@"bits 64
section .text
global getvalue

getvalue:
    mov rax, 24
    ret");
            BuildAndCheckProgramOutput(toolChain => SetupNativeProgramAndRun(toolChain), "24");
        }

        [TestCaseSource(nameof(RunnableToolchains_FastestBackend))]
        [Ignore("Test depends on YASM in the Unity repository; re-enable once we have migrated it to Stevedore")]
        public void TestYasm(ToolChain toolchain, BeeAPITestBackend<ToolChain> testBackend)
        {
            ManualFixtureSetup(toolchain, testBackend);

            var compiler = toolchain.ObjectFileProducers.FirstOrDefault(l => l.SupportedExtensions.Contains(".yasm"));
            if (compiler == null)
                Assert.Ignore("No .yasm compiler available");
            if (!(toolchain.Architecture is x64Architecture))
                Assert.Ignore("This test is only written for use with x64 architectures");

            MainCpp.WriteAllText(@"
#include <stdio.h>

extern ""C"" {
    int getvalue();
}

int main() {
    printf(""%d"", getvalue());
}
");
            TestRoot.Combine("assembly.yasm").WriteAllText($@"bits 64
section .text

%ifdef UNITY_OSX
    global _getvalue
    _getvalue:
%else
    global getvalue
    getvalue:
%endif
    mov rax, 24
    ret");
            BuildAndCheckProgramOutput(toolChain => SetupNativeProgramAndRun(toolChain), "24");
        }
    }
}
