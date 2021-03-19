using System;
using System.Collections.Generic;
using System.IO;
using System.Linq;
using System.Runtime.InteropServices;
using NiceIO;
using NUnit.Framework;
using Unity.BuildTools;

namespace Bee.Core.Tests
{
    [TestFixture]
    public abstract class BackendTests
    {
        protected BeeAPITestBackend<string> TestBackend;

        [SetUp]
        public void SetUp()
        {
            TestBackend = MakeTestBackend();
            TestBackend.Start();
        }

        [Test]
        public void BuildCodeSeesCorrectEnvironmentVariables()
        {
            //let's make sure the Ž in Zylis doesn't get butchered. Lithuanians have names too you know.
            Environment.SetEnvironmentVariable("TESTVAR", "Žilys");

            TestBackend.Build(() => "dummy", s =>
            {
                Backend.Current.AddWriteTextAction("output", Environment.GetEnvironmentVariable("TESTVAR"));
            });

            var outputFile = TestBackend.TestRoot.Combine("output");

            Assert.IsTrue(outputFile.FileExists());
            Assert.AreEqual("Žilys", outputFile.ReadAllText());
        }

        [Test]
        public void EnvironmentVariablesSpecifiedInActionWork()
        {
            if (HostPlatform.IsWindows)
                TestBackend.TestRoot.Combine("test.cmd").WriteAllText("echo value is %MYVAR%");
            else
                TestBackend.TestRoot.Combine("test.sh").WriteAllText("echo value is $MYVAR");
            var result = TestBackend.Build(() => "dummy", s =>
            {
                var environmentVariables = new Dictionary<string, string> {{"MYVAR", "42"}};

                Backend.Current.AddAction(
                    actionName: "runscript",
                    targetFiles: new NPath[] {"dummy"},
                    inputs: Array.Empty<NPath>(),
                    executableStringFor: HostPlatform.IsWindows ? "test.cmd" : "sh test.sh",
                    commandLineArguments: Array.Empty<string>(),
                    environmentVariables: environmentVariables
                );
            }, throwOnFailure: false);

            StringAssert.Contains("value is 42", result.StdOut);
        }

        [Test]
        public void BuildCodeConsoleWriteLineIsVisible()
        {
            var result = TestBackend.Build(() => "dummy", dummy =>
            {
                Console.WriteLine("Hello");
            }, false);
            StringAssert.Contains("Hello", result.StdOut);
        }

        [Test]
        public void ThrowingExceptionInBuildCodeFailsBuild()
        {
            var result = TestBackend.Build(() => "dummy", dummy => throw new Exception("Hello"), throwOnFailure: false);

            //maybe we should change tundra to ouptut exceptions in stdout,  instead of stderr
            StringAssert.Contains("Hello", result.StdOut + result.StdErr);
        }

        static string CommandToCopyAFile(string from, string to)
        {
            if (HostPlatform.IsWindows)
            {
                //use this exotic syntax to update the destination timestamp, which is what we need for this test. cp does this automatically
                //we have to mention 'to' twice in the second command to make it work for files outside the current
                //directory
                return $"copy {from} {to} && copy /b {to} +,, {to}";
            }

            return $"cp {from} {to}";
        }

        [Test]
        public void OneOfInputsIsNewerThanOneOfTargetsDoesNotCauseRebuild()
        {
            var input1 = TestBackend.TestRoot.Combine("input1").WriteAllText("content1");
            var input2 = TestBackend.TestRoot.Combine("input2").WriteAllText("content2");

            File.SetLastWriteTimeUtc(input1.ToString(SlashMode.Native), new DateTime(2017, 1, 1));
            File.SetLastWriteTimeUtc(input2.ToString(SlashMode.Native), new DateTime(2016, 1, 1));

            Action<string> buildCode = s =>
            {
                Backend.Current.AddAction(actionName: "CopyTwoFilesInOneGo",
                    targetFiles: new NPath[] {"output1", "output2"},
                    inputs: new NPath[] {"input1", "input2"},
                    executableStringFor: $"{CommandToCopyAFile("input1","output1")} && {CommandToCopyAFile("input2","output2")}",
                    commandLineArguments: Array.Empty<string>(),
                    supportResponseFile: false
                );
            };

            var result1 = TestBackend.Build(() => "dummy", buildCode);
            StringAssert.Contains("CopyTwoFilesInOneGo", result1.StdOut);

            Assert.AreEqual("content1", TestBackend.TestRoot.Combine("output1").ReadAllText());
            Assert.AreEqual("content2", TestBackend.TestRoot.Combine("output2").ReadAllText());

            var result2 = TestBackend.Build(() => "dummy", buildCode);
            StringAssert.DoesNotContain("CopyTwoFilesInOneGo", result2.StdOut);
        }

        [Test]
        public void WriteTextFile()
        {
            TestBackend.Build(() => "dummy", s => Backend.Current.AddWriteTextAction("outputfile", "hello", "WriteMyFile"));

            Assert.AreEqual("hello", TestBackend.TestRoot.Combine("outputfile").ReadAllText());
        }

        [Test]
        public void FailedWriteTextFileGetsProperlyReported()
        {
            TestBackend.TestRoot.Combine("directory_here").CreateDirectory();

            Action<string> buildCode = s =>
            {
                Backend.Current.AddWriteTextAction("directory_here", "hello1", "WriteMyFile");
            };

            var result = TestBackend.Build(() => "dummy", buildCode, throwOnFailure: false);
            Assert.NotZero(result.ExitCode);
            StringAssert.Contains("directory_here", result.StdOut);
        }

        [Test]
        public void WriteTextFileUpdatesWhenContentsChange()
        {
            TestBackend.Build(() => "dummy", s =>
            {
                Backend.Current.AddWriteTextAction("outputfile", "hello", "WriteMyFile");
            });

            Assert.AreEqual("hello", TestBackend.TestRoot.Combine("outputfile").ReadAllText());

            TestBackend.Build(() => "dummy", s =>
            {
                Backend.Current.AddWriteTextAction("outputfile", "NEWCONTENT", "WriteMyFile");
            });

            Assert.AreEqual("NEWCONTENT", TestBackend.TestRoot.Combine("outputfile").ReadAllText());
        }

        [Test]
        public void WriteTextFileAtPathOccupiedByADirectory()
        {
            TestBackend.TestRoot.Combine("mydirectory").EnsureDirectoryExists();

            var result = TestBackend.Build(() => "dummy", s => { Backend.Current.AddWriteTextAction("mydirectory", "hello"); }, throwOnFailure: false);

            Assert.NotZero(result.ExitCode);

            var validErrors = new[]
            {
                "Error opening for writing the file: mydirectory, error: Is a directory",
                "Error opening for writing the file: mydirectory, error: Permission denied",
                "could not create file"
            };
            Assert.IsTrue(validErrors.Any(e => result.StdOut.Contains(e)), "Expected a reasonable error message about file writing not succesful, but instead got: " + result.StdOut);
        }

        [Test]
        public void CanRequestSpecificAliasTarget()
        {
            var result = TestBackend.Build(() => "dummy", s =>
            {
                Backend.Current.AddWriteTextAction("output1", "hello");
                Backend.Current.AddWriteTextAction("output2", "hello");
                Backend.Current.AddAliasDependency("myalias", "output2");
            }, requestedTargets: new[] {"myalias"});

            StringAssert.DoesNotContain("output1", result.StdOut);
            StringAssert.Contains("output2", result.StdOut);
        }

        [Test]
        public void NotRequestedTargetDoesNotGetBuilt()
        {
            var result = TestBackend.Build(() => "dummy", s =>
            {
                Backend.Current.AddWriteTextAction("output/one", "hello");
                Backend.Current.AddWriteTextAction("output2", "hello");
            }, requestedTargets: new[] {"output2"});

            StringAssert.DoesNotContain("output/one", result.StdOut);
        }

        [Test]
        public void RequestingNonExistingTargetFailsBuild()
        {
            var expectedError = "does_not_exist";

            try
            {
                //there's several okay behaviours here.  it's okay if the backend makes the .Build call throw.
                var result = TestBackend.Build(() => "dummy", s => {}, requestedTargets: new[] {"does_not_exist"},
                    throwOnFailure: false);

                //it's also okay, if it finishes, but then makes sure that tundra itself complains when asking to build a non existing node.
                StringAssert.Contains(expectedError, result.StdOut + result.StdErr);
                Assert.NotZero(result.ExitCode);
            }
            catch (Exception e)
            {
                StringAssert.Contains(expectedError, e.Message);
            }
        }

        [Test]
        public void UnwrittenOutputFailsBuild()
        {
            var result = TestBackend.Build(() => "dummy", s =>
            {
                Backend.Current.AddAction(actionName: "dummy",
                    targetFiles: new NPath[] {"file1"},
                    inputs: new NPath[0],
                    executableStringFor: "echo dummy",
                    commandLineArguments: new string[0]
                );
            }, requestedTargets: new[] {"file1"}, throwOnFailure: false);

            Assert.NotZero(result.ExitCode);
            Assert.That(result.StdOut.Contains(@"Failed because this command failed to write the following output files:"));
        }

        [Test]
        public void PartiallyWrittenOutputFailsBuildButDoesNotDeleteOutputs()
        {
            var result = TestBackend.Build(() => "dummy", s =>
            {
                Backend.Current.AddAction(actionName: "dummy",
                    targetFiles: new NPath[] {"file1", "file2"},
                    inputs: new NPath[0],
                    executableStringFor: $"echo dummy > file1",
                    commandLineArguments: new string[0]
                );
            }, requestedTargets: new[] {"file1"}, throwOnFailure: false);

            Assert.NotZero(result.ExitCode);
            Assert.That(result.StdOut.Contains(@"Failed because this command failed to write the following output files:"));
            Assert.That(TestBackend.TestRoot.Combine("file1").ReadAllText(), Does.Contain("dummy"));
        }

        [Test]
        public void FullyWrittenOutputPassesBuild()
        {
            var result = TestBackend.Build(() => "dummy", s =>
            {
                var target = "file1";
                var cmd = $"echo dummy > {target}";

                Backend.Current.AddAction(actionName: "dummy",
                    targetFiles: new NPath[] {target},
                    inputs: new NPath[0],
                    executableStringFor: cmd,
                    commandLineArguments: new string[0]
                );
            }, requestedTargets: new[] {"file1"}, throwOnFailure: false);
            Assert.Zero(result.ExitCode);
        }

        [Test]
        public void UnwrittenOutputWithExceptionPassesBuild()
        {
            var result = TestBackend.Build(() => "dummy", s =>
            {
                Backend.Current.AddAction(actionName: "dummy",
                    targetFiles: new NPath[] {"file1", "file2"},
                    inputs: new NPath[0],
                    executableStringFor: "echo dummy",
                    commandLineArguments: new string[0],
                    allowUnwrittenOutputFiles: true
                );
            }, requestedTargets: new[] {"file1"}, throwOnFailure: false);

            Assert.Zero(result.ExitCode);
        }

        [Test]
        public void ManuallyChangedWrittenFileDoesNotGetOverWritten()
        {
            //We don't strongly care about this behaviour, you could argue both ways on what correct behaviour should be,
            //but adding this test so that we at least notice if we were to change behaviour so it can be a concious choice.
            Action<string> buildCode = s => Backend.Current.AddWriteTextAction("outputfile", "hello", "WriteMyFile");
            TestBackend.Build(() => "dummy", buildCode);
            var outputFile = TestBackend.TestRoot.Combine("outputfile");
            Assert.AreEqual("hello", outputFile.ReadAllText());
            outputFile.WriteAllText("OVERWRITTEN");
            TestBackend.Build(() => "dummy", buildCode);
            Assert.AreEqual("OVERWRITTEN", outputFile.ReadAllText());
        }

        [Test]
        public void AddDependencyDoesNotCauseRebuildWhenChanged()
        {
            Action<string> buildcode = s =>
            {
                Backend.Current.AddAction(
                    "ActionA",
                    new NPath[] {"output"},
                    new NPath[0],
                    $"echo {s} > output",
                    new string[0],
                    allowUnwrittenOutputFiles: true);
                Backend.Current.AddAction(
                    "ActionB",
                    new NPath[] {"output2"},
                    new NPath[0],
                    "echo hello > output2",
                    new string[0],
                    allowUnwrittenOutputFiles: true);
                Backend.Current.AddDependency("output2", "output");
            };

            TestBackend.Build(() => "1", buildcode, requestedTargets: new[] {"output2"});

            var result = TestBackend.Build(() => "2", buildcode, requestedTargets: new[] { "output2" });

            Assert.That(result.StdOut, Does.Contain("ActionA"));
            Assert.That(result.StdOut, Does.Not.Contain("ActionB"));
        }

        [Test]
        public void SettingUpImplicitDependencyOnInputsInFolder_ExecutesImplicitDependencyDuringBuild()
        {
            const string implicitDir = "dir";
            const string implicitTarget = "dir.madeImplictly";

            TestBackend.Build(() => "dummy", s =>
            {
                Backend.Current.AddWriteTextAction(implicitTarget, "Made as an implicit dependency");
                Backend.Current.AddImplicitDependenciesToAllActionsWithInputsThatLiveIn(implicitDir, implicitTarget);

                Backend.Current.AddAction(
                    "MakeThing",
                    new NPath[] {"output"},
                    new[] {new NPath(implicitDir).Combine("somefile")},
                    "echo hello",
                    new string[0],
                    allowUnwrittenOutputFiles: true);
            }, requestedTargets: new[] { "output" });

            Assert.That(TestBackend.TestRoot.Combine(implicitTarget).FileExists());
        }

        [Test]
        public void SettingUpImplicitDependencyOnScannerDirectories_ExecutesImplicitDependencyDuringBuild()
        {
            const string implicitDir = "dir";
            const string implicitTarget = "dir.madeImplictly";

            const string output = "output";
            TestBackend.Build(() => "dummy", s =>
            {
                Backend.Current.AddWriteTextAction(implicitTarget, "Made as an implicit dependency");
                Backend.Current.AddImplicitDependenciesToAllActionsWithInputsThatLiveIn(implicitDir, implicitTarget);

                NPath inputFile = "somefile";
                Backend.Current.AddAction(
                    "MakeThing",
                    new NPath[] {output},
                    new[] {inputFile},
                    "echo hello",
                    new string[0],
                    allowUnwrittenOutputFiles:
                    true);

                Backend.Current.ScanCppFileForHeaders(output, inputFile, new[] { new NPath(implicitDir).Combine("some/subfolder")});
            }, requestedTargets: new[] { output });

            Assert.That(TestBackend.TestRoot.Combine(implicitTarget).FileExists());
        }

        [Test]
        public void ImplicitDependencyOnInputsInFolder_DoesNotForceRebuilds()
        {
            const string implicitDir = "dir";
            const string implicitTarget = "dir.madeImplictly";


            Action<string> buildcode = s =>
            {
                Backend.Current.AddWriteTextAction(implicitTarget, s);
                Backend.Current.AddImplicitDependenciesToAllActionsWithInputsThatLiveIn(implicitDir, implicitTarget);

                Backend.Current.AddAction("MakeThing", new NPath[] {"output"},
                    new[] {new NPath(implicitDir).Combine("somefile")}, "echo hello > output", new string[0]);
            };

            var implicitTargetFile = implicitTarget.ToNPath().MakeAbsolute(TestBackend.TestRoot);

            var result = TestBackend.Build(() => "Made as an implicit dependency", buildcode, requestedTargets: new[] { "output" });

            Assert.That(implicitTargetFile.ReadAllText(), Is.EqualTo("Made as an implicit dependency"));
            Assert.That(result.StdOut, Does.Contain("MakeThing"));

            result = TestBackend.Build(() => "Made differently as an implicit dependency", buildcode, requestedTargets: new[] { "output" });

            Assert.That(implicitTargetFile.ReadAllText(), Is.EqualTo("Made differently as an implicit dependency"));
            Assert.That(result.StdOut, Does.Not.Contain("MakeThing"));
        }

        [Test]
        [Platform("Win")]
        public void CopyToolDoesNotProduceUselessOutput()
        {
            const string srcFile = "srcfile";
            const string dstFile = "dstfile";

            srcFile.ToNPath().MakeAbsolute(TestBackend.TestRoot).CreateFile();

            var result = TestBackend.Build(() => null, s =>
            {
                CopyTool.Instance().Setup(dstFile, srcFile);
            });

            Assert.That(result.StdOut, Does.Contain("CopyTool"));
            Assert.That(result.StdOut, Does.Not.Contain("1 file(s) copied."));
        }

        [Test]
        [Platform("Win")]
        public void CopyToolProducesFailureMessageOnFailure()
        {
            const string srcFile = "srcfile";
            const string dstFile = "dstfile";

            var result = TestBackend.Build(() => null, s =>
            {
                CopyTool.Instance().Setup(dstFile, srcFile);
            }, throwOnFailure: false);

            Assert.That(result.StdOut, Does.Contain("CopyTool"));
            Assert.That(result.StdOut, Does.Contain("The system cannot find the file specified."));
        }

        protected abstract BeeAPITestBackend<string> MakeTestBackend();
    }
}
