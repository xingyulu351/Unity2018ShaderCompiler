using System;
using Bee.Core.Tests;
using Bee.NativeProgramAndToolchainTests.IntegrationTests;
using Bee.Toolchain.Emscripten;
using Bee.Toolchain.GNU;
using Bee.Toolchain.VisualStudio;
using Bee.Toolchain.Xcode;
using NUnit.Framework;
using Unity.BuildSystem.MacSDKSupport;
using Unity.BuildSystem.VisualStudio;

namespace Unity.BuildSystem.NativeProgramSupport.Tests.IntegrationTests
{
    public class Symbols : NativeProgramAndToolchainTestBase
    {
        [TestCaseSource(nameof(AllToolchains_FastestBackend))]
        public void GCCSymbolsVisibilityHidden_LinkingToAnotherUnitWithoutExplicitSymbolExport_FailLinkingMain(ToolChain testableToolchain, BeeAPITestBackend<ToolChain> testBackend)
        {
            if (testableToolchain is EmscriptenToolchain)
                Assert.Ignore("WebGL uses static lib for this config that doesn't hide symbols.");

            ManualFixtureSetup(testableToolchain, testBackend);

            TestRoot.Combine("lib1.cpp").WriteAllText(@"
#include ""stdio.h""

// The PS4 won't create a _stub file (needed to link against dynamic libraries) unless there is an export.
#ifdef SN_TARGET_ORBIS
__declspec(dllexport) void dummy() {}
#endif

void function1() {
    printf(""function1"");
}
");
            TestRoot.Combine("main.cpp").WriteAllText(@"
int main() {
    extern void function1();
    function1();
}
");
            var result = Build(toolchain =>
            {
                var lib1 = (DynamicLibrary)SetupNativeProgram("lib1", "lib1.cpp", toolchain.DynamicLibraryFormat, null);
                SetupNativeProgram("program", "main.cpp", toolchain.ExecutableFormat, ".", lib1);
            }, false);
            Assert.That(
                result.StdOut.ToLower(),
                Contains.Substring("undefined").And.Contain("function1").Or.Contain("unresolved").And.Contain("function1"),
                "Output didn't contain information about linker failure");
        }

        [TestCaseSource(nameof(AllToolchains_FastestBackend))]
        public void GCCSymbolsVisibilityHidden_LinkingToAnotherUnitProvidingExplicitSymbolExport_PassesLinkingMain(ToolChain testableToolchain, BeeAPITestBackend<ToolChain> testBackend)
        {
            if (testableToolchain is EmscriptenToolchain)
                Assert.Ignore("WebGL uses static lib for this config that doesn't hide symbols.");

            ManualFixtureSetup(testableToolchain, testBackend);

            TestRoot.Combine("lib1.cpp").WriteAllText(@"
#include ""stdio.h""

#include <stdio.h>
#if _MSC_VER || defined(SN_TARGET_ORBIS)
__declspec(dllexport)
#else
__attribute__ ((visibility (""default"")))
#endif
void function1() {
    printf(""function1"");
}
");
            TestRoot.Combine("main.cpp").WriteAllText(@"
int main() {
    extern void function1();
    function1();
}
");
            Build(toolchain =>
            {
                var lib1 = (DynamicLibrary)SetupNativeProgram("lib1", "lib1.cpp", toolchain.DynamicLibraryFormat, null);
                SetupNativeProgram("program", "main.cpp", toolchain.ExecutableFormat, ".", lib1);
            });
        }

        [TestCaseSource(nameof(AllToolchains_FastestBackend))]
        public void ExportSymbolsMap(ToolChain testableToolchain, BeeAPITestBackend<ToolChain> testBackend)
        {
            if (!(testableToolchain is MacToolchain) && !(testableToolchain is VisualStudioToolchain))
                Assert.Ignore("This test is only designed for mac and visualstudio right now");

            ManualFixtureSetup(testableToolchain, testBackend);

            TestRoot.Combine("lib1.cpp").WriteAllText(@"
#include ""stdio.h""

#include <stdio.h>

extern ""C"" { void function1() {printf(""function1""); }}
extern ""C"" { void function2() {printf(""function2""); }}

");
            TestRoot.Combine("main.cpp").WriteAllText(@"

extern ""C"" { void function1(); }
extern ""C"" { void function2(); }

int main() {
    function1();
    function2();
}
");
            TestRoot.Combine("my_exported_symbollist").WriteAllText("_function1");
            TestRoot.Combine("my_definition_file").WriteAllText("EXPORTS\nfunction1");

            var buildResult = Build(toolchain =>
            {
                var format = toolchain.DynamicLibraryFormat;
                var config = new NativeProgramConfiguration(CodeGen.Debug, format.Toolchain, false);
                var program = new NativeProgram("lib1")
                {
                    Sources = { "lib1.cpp" },
                };
                program.CompilerSettingsForMac().Add(c => c.WithVisibility(Visibility.Default));
                program.DynamicLinkerSettingsForMac().Add(l => l.WithExportedSymbolList("my_exported_symbollist"));
                program.DynamicLinkerSettingsForMsvc().Add(l => l.WithDefinitionFile("my_definition_file"));

                //Export symbol list requires all symbols you want to export to start off as visible. It can only "unvisible" symbols.
                //so let's configure symbol visibility to be visible for alls symbos:

                var lib1 = (DynamicLibrary)program.SetupSpecificConfiguration(config, format);
                SetupNativeProgram("program", "main.cpp", toolchain.ExecutableFormat, ".", lib1);
            }, throwOnFailure: false);

            Assert.IsTrue(buildResult.ExitCode != 0);

            if (testableToolchain is MacToolchain)
            {
                //Assert we get a linker error about function2,  and that we do not get a linker error about function1
                StringAssert.Contains("Undefined symbols", buildResult.StdOut);
                StringAssert.Contains("function2", buildResult.StdOut);
                StringAssert.DoesNotContain("function1", buildResult.StdOut);
            }

            if (testableToolchain is VisualStudioToolchain)
            {
                //Assert we get a linker error about function2,  and that we do not get a linker error about function1
                StringAssert.Contains("unresolved external symbol", buildResult.StdOut);
                StringAssert.Contains("function2", buildResult.StdOut);
                StringAssert.DoesNotContain("function1", buildResult.StdOut);
            }
        }
    }
}
