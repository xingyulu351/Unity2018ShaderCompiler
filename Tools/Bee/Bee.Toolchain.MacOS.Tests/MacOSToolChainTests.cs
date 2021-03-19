using Bee.Core;
using Bee.Toolchain.LLVM;
using Bee.Toolchain.Xcode;
using Bee.ToolchainTestInfrastructure;
using NiceIO;
using NUnit.Framework;
using Unity.BuildSystem.MacSDKSupport;
using Unity.BuildSystem.NativeProgramSupport;


namespace Bee.Toolchain.MacOS.Tests
{
    class MacOSToolChainTests : SingleToolchainTests
    {
        protected override ToolChain ProduceToolchainToTest() => new MacToolchain(MacSdk.Locator.UserDefaultOrDummy, new x64Architecture());

        private static NPath SetupMainProgram(ToolChain toolchain, params PrecompiledLibrary[] dependencies)
            => SetupNativeProgram("program", "main.cpp", toolchain.ExecutableFormat, BinaryFolder, dependencies);

        private static NPath AppBundleFolder => "build/program.app";
        private static NPath AppBuildFolder => AppBundleFolder.Combine("Contents");
        private static NPath BinaryFolder => AppBuildFolder.Combine("MacOS");


        [Test]
        public void Executable_WithNonSystemFramework_ProperlyLinks()
        {
            const string libName = "lib";

            SetupMainCallingOtherCpp(libName);

            BuildAndCheckProgramOutput(toolchain =>
            {
                SetupCppPrintingValue(libName, "foo");

                DynamicLibrary lib1 = SetupNativeProgram(libName, libName + ".cpp",
                    ((XcodeDynamicLibraryFormat)toolchain.DynamicLibraryFormat).WithLinkerSetting<XcodeDynamicLibraryFormat>(
                        l => l.WithInstallName($"@rpath/{libName}.framework/Versions/A/{libName}.dylib")), null);

                var frameworkRoot = TestRoot.Combine(AppBuildFolder, $"Frameworks/{lib1.Path.FileNameWithoutExtension}.framework");
                var A = frameworkRoot.Combine("Versions/A");
                var Current = frameworkRoot.Combine("Versions/Current");
                var frameworkLib = frameworkRoot.Combine(lib1.Path.FileNameWithoutExtension);
                var libOutput = A.Combine(lib1.Path.FileName);

                var setupScript = TestRoot.Combine("setup.sh");
                Backend.Current.AddWriteTextAction(setupScript, $@"
mkdir -p {libOutput.Parent}
cp {lib1.Path.ToString()} {libOutput.ToString()}
ln -s {libOutput.ToString()} {frameworkLib.ToString()}
ln -s {A.ToString()} {Current.ToString()}
");

                Backend.Current.AddAction(
                    actionName: "Run_setup",
                    targetFiles: new[] {frameworkRoot},
                    inputs: new[] {lib1.Path, setupScript},
                    executableStringFor: "sh",
                    commandLineArguments: new[] {setupScript.ToString()}
                );

                var testFw = new NonSystemFramework(frameworkRoot);

                var target = SetupMainProgram(toolchain, testFw);
                SetupRunProgram(toolchain, target);
            },
                "foo",
                $"{BinaryFolder.Combine("program")}"); // TODO: the test framework doesn't find the program if we have another target directory
        }

        [TestCase(true)]
        [TestCase(false)]
        public void ClangSettings_WithAddressSanitizer_SetsCorrectCompilerSettings(bool isEnabled)
        {
            const string kExpectEnabled = "ASAN_ENABLED";
            const string kExpectDisable = "ASAN_DISABLED";
            string expectedOuput = isEnabled ? kExpectEnabled : kExpectDisable;

            BuildAndCheckProgramOutput(toolchain =>
            {
                MainCpp.WriteAllText($@"
#include <stdio.h>
int main()
{{
#if defined(__has_feature)
#  if __has_feature(address_sanitizer)
        printf(""{kExpectEnabled}"");
#  else
        printf(""{kExpectDisable}"");
#  endif
#else
    printf(""{kExpectDisable}"");
#endif
}}");
                // Write an empty blacklist file so clang will not complain about it missing.
                TestRoot.Combine("clang-sanitizer-blacklist.txt").WriteAllText(string.Empty);

                ClangSanitizerMode mode = isEnabled ? ClangSanitizerMode.Address : ClangSanitizerMode.None;

                SetupNativeProgramAndRun(toolchain, program =>
                {
                    program.CompilerSettingsForClang().Add(settings =>
                        settings.WithSanitizer(mode));
                });
            },
                expectedOuput);
        }

        [TestCase(true)]
        [TestCase(false)]
        public void ClangSettings_WithThreadSanitizer_SetsCorrectCompilerSettings(bool isEnabled)
        {
            const string kExpectEnabled = "TSAN_ENABLED";
            const string kExpectDisable = "TSAN_DISABLED";
            string expectedOuput = isEnabled ? kExpectEnabled : kExpectDisable;

            BuildAndCheckProgramOutput(toolchain =>
            {
                MainCpp.WriteAllText($@"
#include <stdio.h>
int main()
{{
#if defined(__has_feature)
#  if __has_feature(thread_sanitizer)
        printf(""{kExpectEnabled}"");
#  else
        printf(""{kExpectDisable}"");
#  endif
#else
    printf(""{kExpectDisable}"");
#endif
}}");
                // Write an empty blacklist file so clang will not complain about it missing.
                TestRoot.Combine("clang-sanitizer-blacklist.txt").WriteAllText(string.Empty);

                ClangSanitizerMode mode = isEnabled ? ClangSanitizerMode.Thread : ClangSanitizerMode.None;

                SetupNativeProgramAndRun(toolchain, program =>
                {
                    program.CompilerSettingsForClang().Add(settings =>
                        settings.WithSanitizer(mode));
                });
            },
                expectedOuput);
        }

        private void SetupCppPrintingValue(string funcName, string value)
        {
            CppFile(funcName, $@"
#include <stdio.h>
extern ""C""
{{
    __attribute__ ((visibility (""default"")))
    int {funcName}()
    {{
        printf(""{value}"");
        return 0;
    }}
}}");
        }

        private void SetupMainCallingOtherCpp(string funcName)
        {
            MainCpp.WriteAllText($@"
extern ""C"" int {funcName} ();

int main()
{{
    return {funcName}();
}}");
        }
    }
}
