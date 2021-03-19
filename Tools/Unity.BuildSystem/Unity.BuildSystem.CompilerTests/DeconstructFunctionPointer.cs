using Bee.Core.Tests;
using Bee.NativeProgramAndToolchainTests.IntegrationTests;
using NUnit.Framework;
using Unity.BuildSystem.CompilerTests.Attributes;
using Unity.BuildSystem.NativeProgramSupport;
using Unity.BuildTools;

namespace Unity.BuildSystem.CompilerTests
{
    public class DeconstructionFunctionPointerTests : NativeProgramAndToolchainTestBase
    {
        private const string DeconstructFunctionPointerFeatureName = "'DeconstructFunctionType<>' is supported";

        [TestCaseSource(nameof(RunnableToolchains_FastestBackend))]
        [CompilerFeatureName(DeconstructFunctionPointerFeatureName)]
        public void PrependArgument_FunctionTypeWithOneArgument_PrependsArgument(ToolChain toolchain, BeeAPITestBackend<ToolChain> testBackend)
        {
            var code = $@"
#include <cstdio>
#include ""Runtime/Utilities/DeconstructFunctionType.h""

void argumentFunctionWithTwoArguments(float f, int i)
{{
    printf(""f:%d i:%d"", (int)(f * 100), i);
}}

void functionWithFunctionPointer(DeconstructFunctionType<void (*)(int)>::PrependArgument<float>::ResultType func)
{{
    func(3.14f, 5);
}};

int main()
{{
    functionWithFunctionPointer(argumentFunctionWithTwoArguments);
}}
";
            ManualFixtureSetup(toolchain, testBackend);
            MainCpp.WriteAllText(code);
            BuildAndCheckProgramOutput(toolChain => SetupNativeProgramAndRun(toolChain, np => np.IncludeDirectories.Add(Paths.UnityRoot)), "f:314 i:5");
        }

        [TestCaseSource(nameof(RunnableToolchains_FastestBackend))]
        [CompilerFeatureName(DeconstructFunctionPointerFeatureName)]
        public void PrependArgument_FunctionTypeWithoutArgument_AddsPrependedArgument(ToolChain toolchain, BeeAPITestBackend<ToolChain> testBackend)
        {
            var code = $@"
#include <cstdio>
#include ""Runtime/Utilities/DeconstructFunctionType.h""

void argumentFunctionWithOneArgument(float f)
{{
    printf(""f:%d"", (int)(f * 100));
}}

void functionWithFunctionPointer(DeconstructFunctionType<void (*)()>::PrependArgument<float>::ResultType func)
{{
    func(3.14f);
}};

int main()
{{
    functionWithFunctionPointer(argumentFunctionWithOneArgument);
}}
";
            ManualFixtureSetup(toolchain, testBackend);
            MainCpp.WriteAllText(code);
            BuildAndCheckProgramOutput(toolChain => SetupNativeProgramAndRun(toolChain, np => np.IncludeDirectories.Add(Paths.UnityRoot)), "f:314");
        }
    }
}
