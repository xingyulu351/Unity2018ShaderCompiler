using System;
using System.Text;
using Bee.Core;
using Bee.Core.Tests;
using Bee.Tests;
using NiceIO;
using NUnit.Framework;
using Unity.BuildTools;

namespace Bee.StandaloneDriver.Tests
{
    [TestFixture]
    public class BeeStandaloneBackendTests : BackendTests
    {
        protected override BeeAPITestBackend<string> MakeTestBackend()
        {
            return new BeeAPITestBackend_StandaloneBee<string>();
        }

        [Test]
        public void DontAllowOutput_NoOutput_Passes()
        {
            PrepareTest(stringToOutput: null);

            var executeResult = TestBackend.Build(() => "dummy", s =>
            {
                AddAction(allowUnexpectedOutput: false, allowedOutputSubstrings: new string[0]);
            }, throwOnFailure: false);

            Assert.Zero(executeResult.ExitCode);
        }

        [Test]
        public void DontAllowOutput_Output_Fails()
        {
            PrepareTest(stringToOutput: "this_output_will_make_build_fail");

            var executeResult = TestBackend.Build(() => "dummy", s =>
            {
                AddAction(allowUnexpectedOutput: false, allowedOutputSubstrings: new string[0]);
            }, throwOnFailure: false);

            Assert.NotZero(executeResult.ExitCode);
        }

        [Test]
        public void DontAllowOutput_OutputMatchingRegex_Passes()
        {
            PrepareTest(stringToOutput: "this_output_will_not_make_build_fail");

            var executeResult = TestBackend.Build(() => "dummy", s =>
            {
                AddAction(allowUnexpectedOutput: false, allowedOutputSubstrings: new[] {"this"});
            }, throwOnFailure: false);

            Assert.Zero(executeResult.ExitCode);
        }

        [Test]
        public void DontAllowOutput_OutputNotMatchingRegex_Fails()
        {
            var veryUnexpectedOutputNotMatchingRegex = "very_unexpected_output_not_matching_regex";
            PrepareTest(stringToOutput: veryUnexpectedOutputNotMatchingRegex);

            var executeResult = TestBackend.Build(() => "dummy", s =>
            {
                AddAction(allowUnexpectedOutput: false, allowedOutputSubstrings: new[] {"^this.+$"});
            }, throwOnFailure: false);

            Assert.NotZero(executeResult.ExitCode);
            StringAssert.Contains(veryUnexpectedOutputNotMatchingRegex, executeResult.StdOut);
        }

        [Test]
        public void AllowOutput_OutputMatchingRegex_GetsSwallowed()
        {
            var thisOutputWillNotBePrinted = "this_output_will_not_be_printed";
            PrepareTest(stringToOutput: thisOutputWillNotBePrinted);

            var executeResult = TestBackend.Build(() => "dummy", s =>
            {
                AddAction(allowUnexpectedOutput: true, allowedOutputSubstrings: new[] {"^this.+$"});
            }, throwOnFailure: false);

            Assert.Zero(executeResult.ExitCode);
            StringAssert.DoesNotContain(thisOutputWillNotBePrinted, executeResult.StdOut);
        }

        [Test]
        public void SwallowedOutput_WhenActionFails_DoesGetPrinted()
        {
            var output = "this_output_will_be_printed_even_though_its_filtered";
            PrepareTest(stringToOutput: output, makeFail: true);

            var executeResult = TestBackend.Build(() => "dummy", s =>
            {
                AddAction(allowUnexpectedOutput: true, allowedOutputSubstrings: new[] {"^this.+$"});
            }, throwOnFailure: false);

            Assert.NotZero(executeResult.ExitCode);
            StringAssert.Contains(output, executeResult.StdOut);
        }

        private static string ReasonablyLongString
        {
            get
            {
                var sb = new StringBuilder();
                for (int i = 0; i != 1000; i++)
                    sb.Append(i.ToString());
                return sb.ToString();
            }
        }

        [Test]
        public void ReasonablyLongRegexCanMatch()
        {
            var output = ReasonablyLongString;
            PrepareTest(stringToOutput: output, makeFail: false);

            var executeResult = TestBackend.Build(() => "dummy", s =>
            {
                AddAction(allowUnexpectedOutput: false, allowedOutputSubstrings: new[] {ReasonablyLongString});
            }, throwOnFailure: false);

            Assert.Zero(executeResult.ExitCode);
        }

        [Test]
        public void FileOnceBuiltThatsNoLongerPartOfGraphGetsDeleted()
        {
            TestBackend.Build(() => "dummy", s => Backend.Current.AddWriteTextAction("myfile", "mycontent"));
            Assert.IsTrue(TestBackend.TestRoot.Combine("myfile").FileExists());
            TestBackend.Build(() => "dummy", s => {});
            Assert.IsFalse(TestBackend.TestRoot.Combine("myfile").FileExists());
        }

        [Test]
        public void ReasonablyLongRegexCorrectlyNotMatchesStringThatIsTooShort()
        {
            //A regex parser that would have cap a regular expression at a very short length would fail this test,
            //as it would fail to notice that the last 4 characters of the regex are actually not present in what's being
            //printed.
            var veryVeryVeryLongString = ReasonablyLongString;
            var output = veryVeryVeryLongString.Substring(0, veryVeryVeryLongString.Length - 4);
            PrepareTest(stringToOutput: output, makeFail: false);

            var executeResult = TestBackend.Build(() => "dummy", s =>
            {
                AddAction(allowUnexpectedOutput: false, allowedOutputSubstrings: new[] {ReasonablyLongString});
            }, throwOnFailure: false);

            Assert.NotZero(executeResult.ExitCode);
        }

        [Test]
        public void ExceptionInBuildCodeHasLineNumbers()
        {
            var beeBackend = (BeeAPITestBackend_StandaloneBee<string>)TestBackend;

            beeBackend.TestRoot.Combine("Build.bee.cs").WriteAllText(@"class A
{
    static void Main()
    {
        throw new System.Exception(""This better have linenumbers!"");
    }
}");

            var executeResult = beeBackend.InvokeStandaloneBee(throwOnFailure: false);
            Assert.NotZero(executeResult.ExitCode);

            //the exception was thrown from line 5, let's assert this info is in the output
            var expectation = HostPlatform.IsWindows ? "Build.bee.cs:line 5" : "Build.bee.cs:5";
            StringAssert.Contains(expectation, executeResult.StdOut);
        }

        [Test]
        public void ExplainDisplaysDependencyChain()
        {
            TestBackend.TestRoot.Combine("main.cs").WriteAllText("class A { static void Main() {} }");
            TestBackend.Build(() => "dummy", _ =>
            {
                CopyTool.Instance().Setup("main2.cs", "main.cs");
                CopyTool.Instance().Setup("main3.cs", "main2.cs");
                CopyTool.Instance().Setup("main4.cs", "main3.cs");
                CopyTool.Instance().Setup("main5.cs", "main4.cs");
            }, true, new[] {"main5.cs"});

            var executeResult = ((BeeAPITestBackend_StandaloneBee<string>)TestBackend).InvokeStandaloneBee(true, new[] {"why", "main2.cs"});

            Assert.That(executeResult.StdOut, new MultilineStringMatchesSpecConstraint(
                "",
                "This is why CopyTool main2.cs was built:",
                "",
                " CopyTool main5.cs, requested as a final build target, takes as an input the output of:",
                " |- CopyTool main4.cs, which takes as an input the output of:",
                "    |- CopyTool main3.cs, which takes as an input the output of:",
                "       |- CopyTool main2.cs, which was built because:",
                "          |",
                "          | The node hadn't been built before."
            ));
        }

        private static void AddAction(bool allowUnexpectedOutput, string[] allowedOutputSubstrings)
        {
            Backend.Current.AddAction(
                actionName: "RunScript",
                targetFiles: new NPath[] {"targetfile"},
                inputs: new NPath[] { ScriptFilename},
                executableStringFor: HostPlatform.IsWindows ? "" : "sh",
                commandLineArguments: new[] {ScriptFilename},
                allowUnexpectedOutput: allowUnexpectedOutput,
                allowedOutputSubstrings: allowedOutputSubstrings
            );
        }

        private static string ScriptFilename => HostPlatform.IsWindows ? "test.bat" : "test.sh";

        private void PrepareTest(string stringToOutput, bool makeFail = false)
        {
            var sb = new StringBuilder();
            if (HostPlatform.IsWindows)
                sb.AppendLine("@ECHO OFF");
            if (stringToOutput != null)
                sb.AppendLine($"echo {stringToOutput}");
            sb.AppendLine(makeFail ? "exit 1;" : "echo dummy > targetfile");
            TestBackend.TestRoot.Combine(ScriptFilename).WriteAllText(sb.ToString());
        }
    }
}
