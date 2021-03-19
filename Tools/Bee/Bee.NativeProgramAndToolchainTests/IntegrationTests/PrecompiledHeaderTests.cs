using System;
using System.Linq;
using Bee.Core.Tests;
using Bee.NativeProgramAndToolchainTests.IntegrationTests;
using JamSharp.Runtime;
using NiceIO;
using NUnit.Framework;
using Unity.BuildSystem.IntegrationTests.Framework;

namespace Unity.BuildSystem.NativeProgramSupport.Tests.IntegrationTests
{
    public class PrecompiledHeaderTests : NativeProgramAndToolchainTestBase
    {
        [TestCaseSource(nameof(AllToolchains_AllBackends))]
        public void SimpleProgram(ToolChain toolchain, BeeAPITestBackend<ToolChain> testBackend)
        {
            ManualFixtureSetup(toolchain, testBackend);
            TestRoot.Combine("MyPrefix.h").WriteAllText(@"
#define GETVALUE ""5""");

            MainCpp.WriteAllText(@"
int main() {
    #pragma message(""Test message: "" GETVALUE)
}");
            var result = Build(toolChain => SetupNativeProgram(toolChain, np => np.DefaultPCH = "MyPrefix.h"));
            Assert.That(result.StdOut, Does.Contain("Test message: 5"));
        }

        int GuardCounter;
        string WithIncludeGuard(string hdr)
        {
            var def = $"PCH_{GuardCounter++}";
            return @"
#ifndef " + def + @"
#define " + def + @"
" + hdr + @"
#endif";
        }

        [TestCaseSource(nameof(AllToolchains_AllBackends))]
        public void DifferentDefinesMakeDifferentPchs(ToolChain toolchain, BeeAPITestBackend<ToolChain> testBackend)
        {
            ManualFixtureSetup(toolchain, testBackend);
            TestRoot.Combine("MyPrefix.h").WriteAllText(WithIncludeGuard(@"
#define STRINGISE2(X) #X
#define STRINGISE(X) STRINGISE2(X)
#define LOCALDEFINE MYDEFINE"));
            TestRoot.Combine("file1.cpp").WriteAllText(@"
void getvalue1() {
    #pragma message(""Test message: "" STRINGISE(LOCALDEFINE))
}");
            TestRoot.Combine("file2.cpp").WriteAllText(@"
void getvalue2() {
    #pragma message(""Test message: "" STRINGISE(LOCALDEFINE))
}");

            MainCpp.WriteAllText(@"
extern void getvalue1();
extern void getvalue2();
int main() {
    getvalue1();
    getvalue2();
}");
            var result = Build(toolChain => SetupNativeProgram(toolChain, np =>
            {
                np.PerFileDefines.Add(new[] {"MYDEFINE=5"}, "file1.cpp");
                np.PerFileDefines.Add(new[] {"MYDEFINE=10"}, "file2.cpp");
                np.PerFileDefines.Add(new[] {"MYDEFINE=0"}, "main.cpp");
                np.DefaultPCH = "MyPrefix.h";
            }));
            Assert.That(result.StdOut, Does.Contain("Test message: 5").And.Contain("Test message: 10"));
        }

        [TestCaseSource(nameof(AllToolchains_AllBackends))]
        public void TouchingSubIncludeRebuildsPch(ToolChain toolchain, BeeAPITestBackend<ToolChain> testBackend)
        {
            ManualFixtureSetup(toolchain, testBackend);
            var header = TestRoot.Combine("mydependency.h");
            header.WriteAllText(@"#define MYDEFINE ""10""");

            TestRoot.Combine("MyPrefix.h").WriteAllText(WithIncludeGuard(@"
#include ""mydependency.h""
inline void getvalue() {
    #pragma message(""Test message: "" MYDEFINE)
}"));

            MainCpp.WriteAllText(@"
int main() {
    getvalue();
}");
            Action<ToolChain> buildCode = toolChain => SetupNativeProgram(toolChain, np => np.DefaultPCH = "MyPrefix.h");
            using (new TakeAtLeastEnoughTimeForFileSystemResolutionToIncrement())
            {
                var result1 = Build(buildCode);
                Assert.That(result1.StdOut, Does.Contain("Test message: 10"));
            }

            header.WriteAllText(@"#define MYDEFINE ""5""");
            var result2 = Build(buildCode);
            Assert.That(result2.StdOut, Does.Contain("Test message: 5"));
        }

        [TestCaseSource(nameof(AllToolchains_AllBackends))]
        public void PerFilePchsAreUsed(ToolChain toolchain, BeeAPITestBackend<ToolChain> testBackend)
        {
            ManualFixtureSetup(toolchain, testBackend);
            TestRoot.Combine("MyPrefix1.h").WriteAllText(WithIncludeGuard(@"
inline void getvalue1() {
    #pragma message(""Test message: 1"")
}"));
            TestRoot.Combine("MyPrefix2.h").WriteAllText(WithIncludeGuard(@"
inline void getvalue2() {
    #pragma message(""Test message: 2"")
}"));
            TestRoot.Combine("MyPrefix3.h").WriteAllText(WithIncludeGuard("#error"));

            TestRoot.Combine("file1.cpp").WriteAllText("void file1() { getvalue1(); }");
            TestRoot.Combine("file2.cpp").WriteAllText("void file2() { getvalue2(); }");
            TestRoot.Combine("file3.cpp").WriteAllText(@"
void file3() {
    #pragma message(""Test message: 3"")
}");
            MainCpp.WriteAllText(@"
extern void file1();
extern void file2();
extern void file3();

int main() {
    file1();
    file2();
    file3();
}
");
            var result = Build(toolChain => SetupNativeProgram(toolChain,
                np =>
                {
                    np.DefaultPCH = "MyPrefix3.h";
                    np.PerFilePchs.Add("MyPrefix1.h", "file1.cpp");
                    np.PerFilePchs.Add("MyPrefix2.h", "file2.cpp");
                    np.PerFilePchs.Add((NPath)null, "file3.cpp", "main.cpp");
                }));
            Assert.That(result.StdOut, Does.Contain("Test message: 1")
                .And.Contain("Test message: 2")
                .And.Contain("Test message: 3"));
        }

        static bool DoesOutputContainAnyOf(Shell.ExecuteResult executeResult, params string[] needles)
        {
            var combinedOutput = executeResult.StdOut + executeResult.StdErr;
            return needles.Any(needle => combinedOutput.Contains(needle));
        }

        [TestCaseSource(nameof(AllToolchains_AllBackends))]
        public void MultiPCHDefinesWorkCorrectly(ToolChain toolchain, BeeAPITestBackend<ToolChain> testBackend)
        {
            ManualFixtureSetup(toolchain, testBackend);

            TestRoot.Combine("MyPrefix.h")
                .WriteAllText(
@"
#if DO_ERROR
#error
#endif");
            TestRoot.Combine("file1.cpp")
                .WriteAllText(
@"
int howdy() {return 5;}");
            TestRoot.Combine("file2.cpp")
                .WriteAllText(
@"
#include <stdio.h>
void nothappening() {printf(""this could never happen\n"");}");
            TestRoot.Combine("main.cpp")
                .WriteAllText(
@"
int main() {
return 0;
}");
            var executeResult = Build(
                toolChain => SetupNativeProgram(
                    toolChain,
                    np =>
                    {
                        np.DefaultPCH = "MyPrefix.h";
                        np.PerFileDefines.Add(new[] {"DO_ERROR=0"}, "file1.cpp", "main.cpp");
                        np.PerFileDefines.Add(new[] {"DO_ERROR=1"}, "file2.cpp");
                    }), false);
            Assert.That(DoesOutputContainAnyOf(executeResult, "#error"));
        }
    }
}
