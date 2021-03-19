using Bee.Core.Tests;
using Bee.NativeProgramAndToolchainTests.IntegrationTests;
using NUnit.Framework;
using Unity.BuildSystem.CompilerTests.Attributes;
using Unity.BuildSystem.NativeProgramSupport;
using Unity.BuildTools;

namespace Unity.BuildSystem.CompilerTests
{
    // TODO: These tests look to be some kind of continuation of tests from Runtime/Core/MoveForwardTest.cpp
    // TODO: Can non-failing of these tests be moved back to the natives? Particularly there's a full copy of
    // TODO: Move_CanMove test in there.
    public class MoveTests : NativeProgramAndToolchainTestBase
    {
        const string testSourceCommon = @"
#include <cstdio>
#include ""Runtime/Core/move_forward.h""
class MoveAndCopy
{
public:
    MoveAndCopy(const MoveAndCopy& other) { m_CopyCount = other.m_CopyCount + 1; m_MoveCount = other.m_MoveCount; }
    MoveAndCopy& operator=(const MoveAndCopy& other) { m_CopyCount = other.m_CopyCount + 1; m_MoveCount = other.m_MoveCount; return *this; }

    MoveAndCopy(MoveAndCopy&& other) { m_MoveCount = other.m_MoveCount + 1; m_CopyCount = other.m_CopyCount; }
    MoveAndCopy& operator=(MoveAndCopy&& other) {m_MoveCount = other.m_MoveCount + 1; m_CopyCount = other.m_CopyCount; return *this; }

    MoveAndCopy(size_t copyCount, size_t moveCount) : m_CopyCount(copyCount), m_MoveCount(moveCount) {}
    size_t CopyCount() const { return m_CopyCount; }
    size_t MoveCount() const { return m_MoveCount; }
private:
    size_t m_CopyCount;
    size_t m_MoveCount;
};
";

        [TestCaseSource(nameof(RunnableToolchains_FastestBackend))]
        [CompilerFeatureName("test core::move moves")]
        [CompilerFeatureDescription("test case where core::move can move movable object")]
        public void Move_CanMove(ToolChain toolchain, BeeAPITestBackend<ToolChain> testBackend)
        {
            var code = $@"
{testSourceCommon}

int main()
{{
    MoveAndCopy movedFrom(0, 0);
    MoveAndCopy movedTo(core::move(movedFrom));
    printf(""%d"", static_cast<int>(movedTo.MoveCount()));
}}
";
            ManualFixtureSetup(toolchain, testBackend);
            MainCpp.WriteAllText(code);
            BuildAndCheckProgramOutput(
                toolChain => SetupNativeProgramAndRun(
                    toolChain,
                    np => np.IncludeDirectories.Add($"{Paths.UnityRoot}")),
                expectedOutput: "1");
        }

        [TestCaseSource(nameof(AllToolchains_FastestBackend))]
        [CompilerFeatureName("test core::move - move const instance fails")]
        [CompilerFeatureDescription("test core::move should cause a compile error if you try to move a const instance")]
        public void Move_ConstFails(ToolChain toolchain, BeeAPITestBackend<ToolChain> testBackend)
        {
            var code = $@"
{testSourceCommon}

int main()
{{
    const MoveAndCopy movedFrom(0, 0);
    MoveAndCopy movedTo(core::move(movedFrom));
    return static_cast<int>(movedTo.MoveCount());
}}
";
            ManualFixtureSetup(toolchain, testBackend);
            MainCpp.WriteAllText(code);
            var result = Build(
                toolChain => SetupNativeProgram(
                    toolChain,
                    np => np.IncludeDirectories.Add($"{Paths.UnityRoot}")),
                throwOnFailure: false);
            Assert.That(
                result.StdOut.Contains("not possible to move, because type is const"),
                "No compiler error message about moving const type");
        }

        [TestCaseSource(nameof(RunnableToolchains_FastestBackend))]
        [CompilerFeatureName("test core::move - move const fails")]
        [CompilerFeatureDescription("test core::move should cause a compile error if you try to move a const object")]
        public void MoveOrCopy_CanCopyConst(ToolChain toolchain, BeeAPITestBackend<ToolChain> testBackend)
        {
            var code = $@"
{testSourceCommon}

int main()
{{
    const MoveAndCopy movedFrom(0, 0);
    MoveAndCopy movedTo(core::move_or_copy(movedFrom));
    printf(""%d"", static_cast<int>(movedTo.CopyCount()));
}}
";
            ManualFixtureSetup(toolchain, testBackend);
            MainCpp.WriteAllText(code);
            BuildAndCheckProgramOutput(
                toolChain => SetupNativeProgramAndRun(
                    toolChain,
                    np => np.IncludeDirectories.Add($"{Paths.UnityRoot}")),
                expectedOutput: "1");
        }
    }
}
