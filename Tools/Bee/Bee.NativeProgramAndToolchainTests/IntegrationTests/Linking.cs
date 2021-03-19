using System.Linq;
using Bee.Core.Tests;
using Bee.NativeProgramAndToolchainTests.IntegrationTests;
using Bee.Toolchain.GNU;
using Bee.Toolchain.Linux;
using NiceIO;
using NUnit.Framework;
using Unity.BuildSystem.NativeProgramSupport;

namespace Bee.NativeProgramSupport.Tests.IntegrationTests
{
    public class Linking : NativeProgramAndToolchainTestBase
    {
        private void CppPrintingValue(string name, string value)
        {
            CppFile(name, @"
                #include <stdio.h>
                #if defined(_MSC_VER) || defined(__ORBIS__)
                __declspec(dllexport)
                #else
                __attribute__ ((visibility (""default"")))
                #endif
                int " + name + @"()
                {
                    printf(""" + value + @""");
                    return 0;
                }"
            );
        }

        private void CppCallingOtherCpp(string name, params string[] cpps)
        {
            CppFile(name, @"
                #if defined(_MSC_VER) || defined(__ORBIS__)
                __declspec(dllexport)
                #else
                __attribute__ ((visibility (""default"")))
                #endif
                int " + name + @"()
                {
                    int res = 0;
                    " + string.Join("\n", cpps.Select(cpp => $"extern int {cpp}(); res |= {cpp}();")) + @"
                    return res;
                }"
            );
        }

        private static StaticLibrary SetupStaticLibrary(string name, ToolChain toolchain)
            => SetupNativeProgram(name, name + ".cpp", toolchain.StaticLibraryFormat, null);

        private static DynamicLibrary SetupDynamicLibrary(string name, ToolChain toolchain, params PrecompiledLibrary[] dependencies)
            => SetupNativeProgram(name, name + ".cpp", toolchain.DynamicLibraryFormat, null, dependencies);

        private static NPath SetupMainProgram(ToolChain toolchain, params PrecompiledLibrary[] dependencies)
            => SetupNativeProgram("program", "main.cpp", toolchain.ExecutableFormat, ".", dependencies);

        [TestCaseSource(nameof(AllToolchains_FastestBackend))]
        public void StaticLibrary_Works(ToolChain toolchain, BeeAPITestBackend<ToolChain> testBackend)
        {
            ManualFixtureSetup(toolchain, testBackend);

            CppPrintingValue("lib", "success");
            Build(t => SetupStaticLibrary("lib", t));
        }

        [TestCaseSource(nameof(AllToolchains_FastestBackend))]
        public void DynamicLibrary_Works(ToolChain testableToolchain, BeeAPITestBackend<ToolChain> testBackend)
        {
            ManualFixtureSetup(testableToolchain, testBackend);

            CppPrintingValue("lib", "success");
            Build(toolchain => SetupDynamicLibrary("lib", toolchain));
        }

        [TestCaseSource(nameof(AllToolchains_FastestBackend))]
        public void DynamicLibrary_WithStaticLibraries_Works(ToolChain testableToolchain, BeeAPITestBackend<ToolChain> testBackend)
        {
            ManualFixtureSetup(testableToolchain, testBackend);

            CppPrintingValue("lib1", "suc");
            CppPrintingValue("lib2", "cess");
            CppCallingOtherCpp("lib3", "lib1", "lib2");
            Build(toolchain =>
            {
                var lib1 = SetupStaticLibrary("lib1", toolchain);
                var lib2 = SetupStaticLibrary("lib2", toolchain);
                SetupDynamicLibrary("lib3", toolchain, lib1, lib2);
            });
        }

        [TestCaseSource(nameof(AllToolchains_FastestBackend))]
        public void DynamicLibrary_WithDynamicLibraries_Works(ToolChain testableToolchain, BeeAPITestBackend<ToolChain> testBackend)
        {
            ManualFixtureSetup(testableToolchain, testBackend);

            CppPrintingValue("lib1", "suc");
            CppPrintingValue("lib2", "cess");
            CppCallingOtherCpp("lib3", "lib1", "lib2");
            Build(toolchain =>
            {
                var lib1 = SetupDynamicLibrary("lib1", toolchain);
                var lib2 = SetupDynamicLibrary("lib2", toolchain);
                SetupDynamicLibrary("lib3", toolchain, lib1, lib2);
            });
        }

        [TestCaseSource(nameof(AllToolchains_FastestBackend))]
        public void DynamicLibrary_WithStaticLibrary_ThatHasDynamicLibraryDependency_Works(ToolChain testableToolchain, BeeAPITestBackend<ToolChain> testBackend)
        {
            ManualFixtureSetup(testableToolchain, testBackend);

            CppPrintingValue("lib1", "suc");
            CppPrintingValue("lib2", "cess");
            CppCallingOtherCpp("lib3", "lib1", "lib2");
            Build(toolchain =>
            {
                var lib1 = SetupDynamicLibrary("lib1", toolchain);
                var lib2 = SetupStaticLibrary("lib2", toolchain);
                SetupDynamicLibrary("lib3", toolchain, lib1, lib2);
            });
        }

        [TestCaseSource(nameof(AllToolchains_FastestBackend))]
        public void Executable_Works(ToolChain testableToolchain, BeeAPITestBackend<ToolChain> testBackend)
        {
            ManualFixtureSetup(testableToolchain, testBackend);

            CppPrintingValue("main", "success");
            Build(toolchain => SetupMainProgram(toolchain));
        }

        [TestCaseSource(nameof(AllToolchains_FastestBackend))]
        public void Executable_WithStaticLibraries_Works(ToolChain testableToolchain, BeeAPITestBackend<ToolChain> testBackend)
        {
            ManualFixtureSetup(testableToolchain, testBackend);

            CppPrintingValue("lib1", "suc");
            CppPrintingValue("lib2", "cess");
            CppCallingOtherCpp("main", "lib1", "lib2");
            Build(toolchain =>
            {
                var lib1 = SetupStaticLibrary("lib1", toolchain);
                var lib2 = SetupStaticLibrary("lib2", toolchain);
                SetupMainProgram(toolchain, lib1, lib2);
            });
        }

        [TestCaseSource(nameof(AllToolchains_FastestBackend))]
        public void Executable_WithDynamicLibraries_Works(ToolChain testableToolchain, BeeAPITestBackend<ToolChain> testBackend)
        {
            ManualFixtureSetup(testableToolchain, testBackend);

            CppPrintingValue("lib1", "suc");
            CppPrintingValue("lib2", "cess");
            CppCallingOtherCpp("main", "lib1", "lib2");
            Build(toolchain =>
            {
                var lib1 = SetupDynamicLibrary("lib1", toolchain);
                var lib2 = SetupDynamicLibrary("lib2", toolchain);
                SetupMainProgram(toolchain, lib1, lib2);
            });
        }

        [TestCaseSource(nameof(AllToolchains_FastestBackend))]
        public void Executable_WithDynamicLibrary_UsingRPathLink_Works(ToolChain testableToolchain, BeeAPITestBackend<ToolChain> testBackend)
        {
            ManualFixtureSetup(testableToolchain, testBackend);
            if (!(testableToolchain.DynamicLibraryFormat is NativeProgramFormat<LdDynamicLinker>))
                Assert.Ignore("Only running on Linux toolchains which use LdDynamicLinker");

            CppPrintingValue("libtest", "success");
            CppCallingOtherCpp("libmiddle", "libtest");
            CppCallingOtherCpp("main", "libmiddle");
            Build(toolchain =>
            {
                DynamicLibrary libtest = SetupNativeProgram(
                    "libtest",
                    "libtest.cpp",
                    toolchain.DynamicLibraryFormat,
                    new NPath("./tmp"),
                    new PrecompiledLibrary[] {});

                var libmiddle = SetupDynamicLibrary("libmiddle", toolchain, libtest);

                SetupNativeProgram(
                    toolchain,
                    (NativeProgram program) =>
                    {
                        program.Sources.Add("main.cpp");
                        program.PrebuiltLibraries.Add(libmiddle);
                        program.DynamicLinkerSettings().Add(l => l is LdDynamicLinker ld ? ld.WithDynamicLibraryLinkPath("./tmp") : l);
                    },
                    format: toolchain.ExecutableFormat);
            });
        }

        [TestCaseSource(nameof(AllToolchains_FastestBackend))]
        public void Executable_WithStaticLibraryAndDynamicLibrary_Works(ToolChain testableToolchain, BeeAPITestBackend<ToolChain> testBackend)
        {
            ManualFixtureSetup(testableToolchain, testBackend);

            CppPrintingValue("lib1", "suc");
            CppPrintingValue("lib2", "cess");
            CppCallingOtherCpp("main", "lib1", "lib2");
            Build(toolchain =>
            {
                var lib1 = SetupDynamicLibrary("lib1", toolchain);
                var lib2 = SetupStaticLibrary("lib2", toolchain);
                SetupMainProgram(toolchain, lib2, lib1);
            });
        }

        [TestCaseSource(nameof(AllToolchains_FastestBackend))]
        public void Executable_WithStaticLibrary_ThatHasMissingStaticLibraryDependency_Fails(ToolChain testableToolchain, BeeAPITestBackend<ToolChain> testBackend)
        {
            ManualFixtureSetup(testableToolchain, testBackend);

            // This is used to catch situations where static libraries bundle their references
            // which is not the expected behavior when linking a static library
            CppPrintingValue("missing_function_lib", "should not link properly");
            CppCallingOtherCpp("static_lib", "missing_function_lib");
            CppCallingOtherCpp("main", "static_lib");
            var result = Build(toolchain =>
            {
                StaticLibrary lib1 = SetupStaticLibrary("missing_function_lib", toolchain);
                StaticLibrary lib2 = SetupNativeProgram("static_lib", "static_lib.cpp", toolchain.StaticLibraryFormat, null, lib1);
                SetupMainProgram(toolchain, lib2);
            }, false);
            Assert.That(result.StdOut.ToLower(), Contains.Substring("undefined").Or.Contain("unresolved"));
            Assert.That(result.StdOut, Contains.Substring("missing_function_lib"));
            Assert.AreNotEqual(0, result.ExitCode);
        }

        [TestCaseSource(nameof(AllToolchains_FastestBackend))]
        public void Executable_WithBundleStaticLibrary_Works(ToolChain toolchain, BeeAPITestBackend<ToolChain> testBackend)
        {
            ManualFixtureSetup(toolchain, testBackend);
            if (!(toolchain.StaticLibraryFormat is GNUStaticLibraryFormat))
                Assert.Ignore("Only running on toolchains which use GNUStaticLibraryFormat");

            if (toolchain.Sdk is WSLSdk)
                Assert.Ignore("GNUStaticLibraryFormat is unsupported when running WSL");

            CppPrintingValue("lib1", "success");
            CppCallingOtherCpp("main", "lib1");
            Build(t =>
            {
                StaticLibrary lib2 = SetupNativeProgram("lib1", "lib1.cpp", new GNUBundledStaticLibraryFormat(t), null);
                SetupMainProgram(toolchain, lib2);
            });
        }

        [TestCaseSource(nameof(AllToolchains_FastestBackend))]
        public void Executable_WithBundleStaticLibrary_ThatHasStaticLibraryDependency_Works(ToolChain toolchain, BeeAPITestBackend<ToolChain> testBackend)
        {
            ManualFixtureSetup(toolchain, testBackend);
            if (!(toolchain.StaticLibraryFormat is GNUStaticLibraryFormat))
                Assert.Ignore("Only running on toolchains which use GNUStaticLibraryFormat");

            if (toolchain.Sdk is WSLSdk)
                Assert.Ignore("GNUStaticLibraryFormat is unsupported when running WSL");

            CppPrintingValue("lib1", "success");
            CppCallingOtherCpp("lib2", "lib1");
            CppCallingOtherCpp("main", "lib2");
            Build(t =>
            {
                StaticLibrary lib1 = SetupStaticLibrary("lib1", toolchain);
                StaticLibrary lib2 = SetupNativeProgram("lib2", "lib2.cpp", new GNUBundledStaticLibraryFormat(t), null, lib1);
                SetupMainProgram(t, lib2);
            });
        }

        [TestCaseSource(nameof(RunnableToolchains_FastestBackend))]
        public void AndRunning_Executable_Works(ToolChain testableToolchain, BeeAPITestBackend<ToolChain> testBackend)
        {
            ManualFixtureSetup(testableToolchain, testBackend);

            CppPrintingValue("main", "success");
            BuildAndCheckProgramOutput(toolchain =>
            {
                var target = SetupMainProgram(toolchain);
                SetupRunProgram(toolchain, target);
            }, "success");
        }

        [TestCaseSource(nameof(RunnableToolchains_FastestBackend))]
        public void AndRunning_Executable_WithStaticLibraries_Works(ToolChain testableToolchain, BeeAPITestBackend<ToolChain> testBackend)
        {
            ManualFixtureSetup(testableToolchain, testBackend);
            CppPrintingValue("lib1", "suc");
            CppPrintingValue("lib2", "cess");
            CppCallingOtherCpp("main", "lib1", "lib2");
            BuildAndCheckProgramOutput(toolchain =>
            {
                var lib1 = SetupStaticLibrary("lib1", toolchain);
                var lib2 = SetupStaticLibrary("lib2", toolchain);
                var target = SetupMainProgram(toolchain, lib1, lib2);
                SetupRunProgram(toolchain, target);
            }, "success");
        }

        /* TODO: Figure out how to deploy dynamic libraries
        [NativeProgramTest]
        public void AndRunning_Executable_WithDynamicLibraries_Works()
        {
            CppPrintingValue("lib1", "suc");
            CppPrintingValue("lib2", "cess");
            CppCallingOtherCpp("main", "lib1", "lib2");
            BuildAndCheckProgramOutput(toolchain =>
                {
                    var lib1 = SetupDynamicLibrary("lib1", toolchain);
                    var lib2 = SetupDynamicLibrary("lib2", toolchain);
                    var target = SetupMainProgram(toolchain, lib1, lib2);
                    SetupRunProgram(toolchain, target);
                }, "success");
        }

        [NativeProgramTest]
        public void AndRunning_Executable_WithStaticLibrary_ThatHasADynamicLibraryDependency_Works()
        {
            CppPrintingValue("lib1", "suc");
            CppPrintingValue("lib2", "cess");
            CppCallingOtherCpp("main", "lib1", "lib2");
            BuildAndCheckProgramOutput(toolchain =>
                {
                    var lib1 = SetupDynamicLibrary("lib1", toolchain);
                    var lib2 = SetupStaticLibrary("lib2", toolchain);
                    var target = SetupMainProgram(toolchain, lib2, lib1);
                    SetupRunProgram(toolchain, target);
                }, "success");
        }
        */

        [TestCaseSource(nameof(AllToolchains_FastestBackend))]
        public void Executable_WithTwoStaticLibraries_WithDifferentObjectDependencies_OnStaticLibrary_Works(ToolChain testableToolchain, BeeAPITestBackend<ToolChain> testBackend)
        {
            ManualFixtureSetup(testableToolchain, testBackend);

            CppPrintingValue("lib1_1", "suc");
            CppPrintingValue("lib1_2", "cess");
            CppCallingOtherCpp("lib2", "lib1_1");
            CppCallingOtherCpp("lib3", "lib1_2");
            CppCallingOtherCpp("main", "lib2", "lib3");
            Build(toolchain =>
            {
                var config = new NativeProgramConfiguration(CodeGen.Debug, toolchain, false);

                var lib1Program = new NativeProgram("lib1");
                lib1Program.Sources.Add("lib1_1.cpp");
                lib1Program.Sources.Add("lib1_2.cpp");
                StaticLibrary lib1 = lib1Program.SetupSpecificConfiguration(config, toolchain.StaticLibraryFormat);

                var lib2 = SetupStaticLibrary("lib2", toolchain);
                var lib3 = SetupStaticLibrary("lib3", toolchain);
                SetupMainProgram(toolchain, lib2, lib3, lib1);
            });
        }

        [TestCaseSource(nameof(AllToolchains_FastestBackend))]
        public void Executable_WithUnresolvedExternal_Fails(ToolChain testableToolchain, BeeAPITestBackend<ToolChain> testBackend)
        {
            ManualFixtureSetup(testableToolchain, testBackend);
            CppCallingOtherCpp("main", "missing_function");
            var result = Build(toolchain => SetupNativeProgram(toolchain, format: toolchain.ExecutableFormat), false);
            Assert.That(result.StdOut.ToLower(), Contains.Substring("undefined").Or.Contain("unresolved"));
            Assert.That(result.StdOut, Contains.Substring("missing_function"));
            Assert.AreNotEqual(0, result.ExitCode);
        }

        [TestCaseSource(nameof(AllToolchains_FastestBackend))]
        public void Executable_WithMissingStaticLibrary_Fails(ToolChain testableToolchain, BeeAPITestBackend<ToolChain> testBackend)
        {
            ManualFixtureSetup(testableToolchain, testBackend);

            MainCpp.WriteAllText(@"int main() { return 0; }");
            var result = Build(toolchain => SetupNativeProgram(toolchain, np =>
                np.PrebuiltLibraries.Add(new StaticLibrary($"/_missing_static_library_.{toolchain.StaticLibraryFormat.Extension}")),
                format: toolchain.ExecutableFormat), false);
            Assert.That(result.StdOut, Contains.Substring("_missing_static_library_"));
            Assert.AreNotEqual(0, result.ExitCode);
        }

        [TestCaseSource(nameof(AllToolchains_FastestBackend))]
        public void DynamicLibrary_WithMissingStaticLibrary_Fails(ToolChain testableToolchain, BeeAPITestBackend<ToolChain> testBackend)
        {
            ManualFixtureSetup(testableToolchain, testBackend);
            MainCpp.WriteAllText(@"int test_function() { return 0; }");
            var result = Build(toolchain => SetupNativeProgram(toolchain, np =>
                np.PrebuiltLibraries.Add(new StaticLibrary($"/_missing_static_library_.{toolchain.StaticLibraryFormat.Extension}")),
                format: toolchain.DynamicLibraryFormat), false);
            Assert.That(result.StdOut, Contains.Substring("_missing_static_library_"));
            Assert.AreNotEqual(0, result.ExitCode);
        }

        [TestCaseSource(nameof(AllToolchains_FastestBackend))]
        public void Executable_WithMissingDynamicLibrary_Fails(ToolChain testableToolchain, BeeAPITestBackend<ToolChain> testBackend)
        {
            ManualFixtureSetup(testableToolchain, testBackend);

            MainCpp.WriteAllText(@"int main() { return 0; }");
            var result = Build(toolchain => SetupNativeProgram(toolchain, np =>
                np.PrebuiltLibraries.Add(new DynamicLibrary($"/_missing_dynamic_library_.{toolchain.DynamicLibraryFormat.Extension}")),
                format: toolchain.ExecutableFormat), false);
            Assert.That(result.StdOut, Contains.Substring("_missing_dynamic_library_"));
            Assert.AreNotEqual(0, result.ExitCode);
        }

        [TestCaseSource(nameof(AllToolchains_FastestBackend))]
        public void DynamicLibrary_WithMissingDynamicLibrary_Fails(ToolChain testableToolchain, BeeAPITestBackend<ToolChain> testBackend)
        {
            ManualFixtureSetup(testableToolchain, testBackend);

            MainCpp.WriteAllText(@"int test_function() { return 0; }");
            var result = Build(toolchain => SetupNativeProgram(toolchain, np =>
                np.PrebuiltLibraries.Add(new DynamicLibrary($"/_missing_dynamic_library_.{toolchain.DynamicLibraryFormat.Extension}")),
                format: toolchain.DynamicLibraryFormat), false);
            Assert.That(result.StdOut, Contains.Substring("_missing_dynamic_library_"));
            Assert.AreNotEqual(0, result.ExitCode);
        }

        [TestCaseSource(nameof(AllToolchains_FastestBackend))]
        public void Executable_WithMissingSystemLibrary_Fails(ToolChain testableToolchain, BeeAPITestBackend<ToolChain> testBackend)
        {
            ManualFixtureSetup(testableToolchain, testBackend);

            MainCpp.WriteAllText(@"int main() { return 0; }");
            var result = Build(toolchain => SetupNativeProgram(toolchain, np =>
                np.PrebuiltLibraries.Add(new SystemLibrary("_missing_system_library_")),
                format: toolchain.ExecutableFormat), false);
            Assert.That(result.StdOut, Contains.Substring("_missing_system_library_"));
            Assert.AreNotEqual(0, result.ExitCode);
        }

        [TestCaseSource(nameof(AllToolchains_FastestBackend))]
        public void DynamicLibrary_WithMissingSystemLibrary_Fails(ToolChain testableToolchain, BeeAPITestBackend<ToolChain> testBackend)
        {
            ManualFixtureSetup(testableToolchain, testBackend);
            MainCpp.WriteAllText(@"int test_function() { return 0; }");
            var result = Build(toolchain => SetupNativeProgram(toolchain, np =>
                np.PrebuiltLibraries.Add(new SystemLibrary("_missing_system_library_")),
                format: toolchain.DynamicLibraryFormat), false);
            Assert.That(result.StdOut, Contains.Substring("_missing_system_library_"));
            Assert.AreNotEqual(0, result.ExitCode);
        }

        class ToolchainSpecificLibrary : PrecompiledLibrary
        {
            public override bool ToolchainSpecific => true;
            public override bool System => true;
            public ToolchainSpecificLibrary(string name) => _Name = name;
        }

        [TestCaseSource(nameof(AllToolchains_FastestBackend))]
        public void Executable_IgnoresToolchainSpecificLibrary(ToolChain testableToolchain, BeeAPITestBackend<ToolChain> testBackend)
        {
            ManualFixtureSetup(testableToolchain, testBackend);
            MainCpp.WriteAllText(@"int main() { return 0; }");
            Build(toolchain => SetupNativeProgram(toolchain, np =>
                np.PrebuiltLibraries.Add(new ToolchainSpecificLibrary("_missing_toolchain_specific_library_")),
                format: toolchain.ExecutableFormat));
        }

        [TestCaseSource(nameof(AllToolchains_FastestBackend))]
        public void DynamicLibrary_IgnoresToolchainSpecificLibrary(ToolChain testableToolchain, BeeAPITestBackend<ToolChain> testBackend)
        {
            ManualFixtureSetup(testableToolchain, testBackend);
            MainCpp.WriteAllText(@"
                #if defined(_MSC_VER) || defined(__ORBIS__)
                __declspec(dllexport)
                #else
                __attribute__ ((visibility (""default"")))
                #endif
                int test_function() { return 0; }");
            Build(toolchain => SetupNativeProgram(toolchain, np =>
                np.PrebuiltLibraries.Add(new ToolchainSpecificLibrary("_missing_toolchain_specific_library_")),
                format: toolchain.DynamicLibraryFormat));
        }

        [TestCaseSource(nameof(RunnableToolchains_FastestBackend))]
        public void ConsumeDependencyAsBagOfObjectFiles(ToolChain toolChain, BeeAPITestBackend<ToolChain> testBackend)
        {
            ManualFixtureSetup(toolChain, testBackend);

            //let's add a static constructor in a library. If it would be compiled as a normal static library, most linkers
            //will not invoke the static constructor if nothing is being referenced in the library.  If the library is consumed
            //as a "bag of objects", then it should get called.
            TestRoot.Combine("lib.cpp").WriteAllText(@"
                struct S {
                    S() {
                        extern const char* printThis;
                        printThis = ""static-constructor"";
                    }
                } s;");
            MainCpp.WriteAllText(@"
                #include ""stdio.h""

                const char* printThis;
                int main() {
                    if (printThis)
                        printf(""%s"", printThis);
                    return 0;
                }"
            );

            BuildAndCheckProgramOutput(t =>
            {
                var lib = new NativeProgram("lib1");
                lib.Sources.Add("lib.cpp");

                var program = new NativeProgram("program")
                {
                    Sources = {"main.cpp"},
                    StaticLibraries = {new FromSourceLibraryDependency()
                                       {
                                           Library = lib,

                                           //using LinkAllObjectFiles mode to ensure that the objectfile holding the static constructor doesnt get dropped
                                           FromSourceLibraryMode = FromSourceLibraryMode.BagOfObjects
                                       }}
                };

                NativeProgramFormat format = t.ExecutableFormat;
                var config = new NativeProgramConfiguration(CodeGen.Debug, format.Toolchain, false);
                var target = program.SetupSpecificConfiguration(config, format, targetDirectory: ".");

                SetupRunProgram(t, target);
            }, "static-constructor");
        }
    }
}
