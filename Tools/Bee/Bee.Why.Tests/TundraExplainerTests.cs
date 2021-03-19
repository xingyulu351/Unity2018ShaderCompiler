using System;
using System.Collections.Generic;
using System.Linq;
using Bee.Core;
using Bee.Core.Tests;
using Bee.TundraBackend.Tests;
using Newtonsoft.Json.Linq;
using NiceIO;
using NUnit.Framework;
using Unity.BuildSystem.IntegrationTests.Framework;
using Unity.BuildTools;

namespace Bee.Why.Tests
{
    class TundraExplainerTests
    {
        private BeeAPITestBackend<string> _testBackend;

        [SetUp]
        public void SetUp()
        {
            _testBackend = new BeeAPITestBackend_Tundra<string>(clearDigestCacheAfterBuild: false);
            _testBackend.Start(TestContext.CurrentContext.Test.MethodName);
        }

        private TundraExplainer Explainer => new TundraExplainer(new TundraBackend.TundraBackend(_testBackend.TestRoot, null));

        private IEnumerable<JObject> StructuredLogEvents => ((BeeAPITestBackend_Tundra<string>)_testBackend).StructuredLogEvents;

        [Test]
        [TestCase("sourcefile", false, true, TestName = "File with no extension, extension not listed, touch records change")]
        [TestCase("sourcefile.cs", false, true, TestName = "File with extension, extension not listed, touch records change")]
        [TestCase("sourcefile", true, true, TestName = "File with no extension, extension listed, touch records change")]
        [TestCase("sourcefile.cs", true, false, TestName = "File with extension, extension listed, touch does not record change")]
        public void TouchingAFile_RecordsThatItChangedInTheStructuredLog_IfItIsNotOnTheHashExtensionsList(string inputFileName, bool addExtension, bool changeExpected)
        {
            var testInputFile = _testBackend.TestRoot.Combine(inputFileName)
                .WriteAllText("test")
                .SetLastWriteTimeUtc(DateTime.UtcNow - TimeSpan.FromMinutes(5));

            NPath testOutputFile = testInputFile + "_dest";

            Action<string> buildCode = _ =>
            {
                if (addExtension)
                    ((TundraBackend.TundraBackend)Backend.Current).AddExtensionToBeScannedByHashInsteadOfTimeStamp("cs");
                Backend.Current.AddAction(
                    "testCmd",
                    new[] {testOutputFile},
                    new[] {testInputFile},
                    HostPlatform.IsWindows ? "copy" : "cp",
                    new[] {testInputFile.InQuotes(SlashMode.Native), testOutputFile.InQuotes(SlashMode.Native)},
                    allowUnwrittenOutputFiles: true);
            };

            _testBackend.Build(() => "dummy", buildCode);

            testInputFile.SetLastWriteTimeUtc(DateTime.UtcNow);

            _testBackend.Build(() => "dummy", buildCode);

            var changeEvent = StructuredLogEvents
                .FirstOrDefault(e => e["msg"].ToString() == "inputSignatureChanged" && e["annotation"].ToString() == "testCmd " + testOutputFile);

            if (changeExpected)
            {
                Assert.That(changeEvent, Is.Not.Null);
                var changes = (JArray)changeEvent["changes"];
                var change = (JObject)changes.First;
                Assert.That(change.Properties(), Has.Exactly(1).Matches((JProperty p) => p.Name == "key"));
                Assert.That(change["key"].ToString(), Is.EqualTo("InputFileTimestamp"));
                Assert.That(new NPath(change["path"].ToString()).ToString(), Is.EqualTo(testInputFile.ToString()));
            }
            else
            {
                Assert.That(changeEvent, Is.Null);
            }
        }

        [Test]
        [TestCase("sourcefile", false, true)]
        [TestCase("sourcefile.cs", false, true)]
        [TestCase("sourcefile", true, true)]
        [TestCase("sourcefile.cs", true, true)]
        public void ChangingAFilesContentAndTimestamp_RecordsThatItChangedInTheStructuredLog(
            string inputFileName,
            bool addExtension,
            bool changeExpected)
        {
            var testInputFile = _testBackend.TestRoot.Combine(inputFileName)
                .WriteAllText("test")
                .SetLastWriteTimeUtc(DateTime.UtcNow - TimeSpan.FromMinutes(5));

            NPath testOutputFile = testInputFile + "_dest";

            Action<string> buildCode = _ =>
            {
                if (addExtension)
                    ((TundraBackend.TundraBackend)Backend.Current).AddExtensionToBeScannedByHashInsteadOfTimeStamp("cs");
                Backend.Current.AddAction(
                    "testCmd",
                    new[] {testOutputFile},
                    new[] {testInputFile},
                    HostPlatform.IsWindows ? "copy" : "cp",
                    new[] {testInputFile.InQuotes(SlashMode.Native), testOutputFile.InQuotes(SlashMode.Native)},
                    allowUnwrittenOutputFiles: true);
            };

            _testBackend.Build(() => "dummy", buildCode);

            testInputFile.WriteAllText("test2").SetLastWriteTimeUtc(DateTime.UtcNow);

            _testBackend.Build(() => "dummy", buildCode);

            var changeEvents = StructuredLogEvents
                .Where(e => e["msg"].ToString() == "inputSignatureChanged")
                .Where(e => e["annotation"].ToString() == "testCmd " + testOutputFile);

            Assert.That(changeEvents.Count(), Is.EqualTo(changeExpected ? 1 : 0));
        }

        [Test]
        public void CanExplainThatNodeGotRebuiltBecause_InputFileTimestampChanged()
        {
            NPath testFile2 = _testBackend.TestRoot.Combine("main.txt");
            testFile2.WriteAllText("test").SetLastWriteTimeUtc(DateTime.UtcNow - TimeSpan.FromMinutes(5));

            Action<string> buildTask = _ => {
                ((TundraBackend.TundraBackend)Backend.Current).AddExtensionToBeScannedByHashInsteadOfTimeStamp(".cs");
                Backend.Current.AddAction(
                    "DummyAction",
                    new[] {new NPath("test.out")},
                    new[] {new NPath("main.txt")},
                    "echo hello",
                    new string[] {},
                    allowUnwrittenOutputFiles: true);
            };

            _testBackend.Build(() => "dummy", buildTask);

            testFile2.WriteAllText("test2").SetLastWriteTimeUtc(DateTime.UtcNow);

            _testBackend.Build(() => "dummy", buildTask);

            var explanation = Explainer.Explain("test.out");

            Assert.AreEqual(1, explanation.RebuildReasons.Length);

            var reason2 = (InputTimeStampChangedReason)explanation.RebuildReasons[0];
            Assert.AreEqual("main.txt", reason2.File.ToString());
        }

        [Test]
        public void CanExplainThatNodeGotRebuiltBecause_InputFileDigestChanged()
        {
            NPath testFile = _testBackend.TestRoot.Combine("main.cs");
            testFile.WriteAllText("class A { static void Main() {} }").SetLastWriteTimeUtc(DateTime.UtcNow - TimeSpan.FromMinutes(5));

            Action<string> buildTask = _ => {
                ((TundraBackend.TundraBackend)Backend.Current).AddExtensionToBeScannedByHashInsteadOfTimeStamp(".cs");
                Backend.Current.AddAction(
                    "DummyAction",
                    new[] {new NPath("test.out")},
                    new[] {new NPath("main.cs")},
                    "echo hello",
                    new string[] {},
                    allowUnwrittenOutputFiles: true);
            };

            _testBackend.Build(() => "dummy", buildTask);

            testFile.WriteAllText("class B { static void Main() { throw new System.Exception(); } }").SetLastWriteTimeUtc(DateTime.UtcNow);

            _testBackend.Build(() => "dummy", buildTask);

            var explanation = Explainer.Explain("test.out");

            Assert.AreEqual(1, explanation.RebuildReasons.Length);

            var reason1 = (InputHashChangedReason)explanation.RebuildReasons[0];
            Assert.AreEqual("main.cs", reason1.File.ToString());
        }

        [Test]
        public void CanExplainThatNodeGotRebuiltBecause_ImplicitDependencyChanged()
        {
            NPath testFile = _testBackend.TestRoot.Combine("main.cpp");
            testFile.WriteAllText("#include \"main.h\"").SetLastWriteTimeUtc(DateTime.UtcNow - TimeSpan.FromMinutes(5));

            NPath testHeader = _testBackend.TestRoot.Combine("main.h");
            testHeader.WriteAllText("#pragma once").SetLastWriteTimeUtc(DateTime.UtcNow - TimeSpan.FromMinutes(5));

            Action<string> buildTask = _ => {
                ((TundraBackend.TundraBackend)Backend.Current).AddExtensionToBeScannedByHashInsteadOfTimeStamp(".cpp", ".h");
                Backend.Current.AddAction(
                    "DummyAction",
                    new[] {new NPath("test.out")},
                    new[] {new NPath("main.cpp")},
                    "echo hello",
                    new string[] {},
                    allowUnwrittenOutputFiles: true);
                ((TundraBackend.TundraBackend)Backend.Current).ScanCppFileForHeaders(
                    new NPath("test.out"),
                    new NPath("main.cpp"),
                    new[] {_testBackend.TestRoot});
            };

            _testBackend.Build(() => "dummy", buildTask);

            testHeader.WriteAllText("#pragma once\nint foo();\n").SetLastWriteTimeUtc(DateTime.UtcNow);

            _testBackend.Build(() => "dummy", buildTask);

            var explanation = Explainer.Explain("test.out");

            Assert.AreEqual(1, explanation.RebuildReasons.Length);

            var reason1 = (InputHashChangedReason)explanation.RebuildReasons[0];
            Assert.AreEqual("main.h", reason1.File.ToString());
        }

        [Test]
        public void CanExplainThatNodeGotRebuiltBecause_DoublyImplicitDependencyChanged()
        {
            NPath testFile = _testBackend.TestRoot.Combine("main.cpp");
            testFile.WriteAllText("#include \"main.h\"").SetLastWriteTimeUtc(DateTime.UtcNow - TimeSpan.FromMinutes(5));

            NPath testHeader = _testBackend.TestRoot.Combine("main.h");
            testHeader.WriteAllText("#include \"main2.h\"").SetLastWriteTimeUtc(DateTime.UtcNow - TimeSpan.FromMinutes(5));

            NPath testHeader2 = _testBackend.TestRoot.Combine("main2.h");
            testHeader2.WriteAllText("#pragma once").SetLastWriteTimeUtc(DateTime.UtcNow - TimeSpan.FromMinutes(5));

            Action<string> buildTask = _ => {
                ((TundraBackend.TundraBackend)Backend.Current).AddExtensionToBeScannedByHashInsteadOfTimeStamp(".cpp", ".h");
                Backend.Current.AddAction(
                    "DummyAction",
                    new[] {new NPath("test.out")},
                    new[] {new NPath("main.cpp")},
                    "echo hello",
                    new string[] {},
                    allowUnwrittenOutputFiles: true);
                ((TundraBackend.TundraBackend)Backend.Current).ScanCppFileForHeaders(
                    new NPath("test.out"),
                    new NPath("main.cpp"),
                    new[] {_testBackend.TestRoot});
            };

            _testBackend.Build(() => "dummy", buildTask);

            testHeader2.WriteAllText("#pragma once\nint foo();\n").SetLastWriteTimeUtc(DateTime.UtcNow);

            _testBackend.Build(() => "dummy", buildTask);

            var explanation = Explainer.Explain("test.out");

            Assert.AreEqual(1, explanation.RebuildReasons.Length);

            var reason1 = (InputHashChangedReason)explanation.RebuildReasons[0];
            Assert.AreEqual("main2.h", reason1.File.ToString());
        }

        [Test]
        public void CanExplainThatNodeGotRebuiltBecause_DoublyImplicitDependencyWasAdded()
        {
            NPath testFile = _testBackend.TestRoot.Combine("main.cpp");
            testFile.WriteAllText("#include \"main.h\"").SetLastWriteTimeUtc(DateTime.UtcNow - TimeSpan.FromMinutes(5));

            NPath testHeader = _testBackend.TestRoot.Combine("main.h");
            testHeader.WriteAllText("#pragma once").SetLastWriteTimeUtc(DateTime.UtcNow - TimeSpan.FromMinutes(5));

            Action<string> buildTask = _ => {
                ((TundraBackend.TundraBackend)Backend.Current).AddExtensionToBeScannedByHashInsteadOfTimeStamp(".cpp", ".h");
                Backend.Current.AddAction(
                    "DummyAction",
                    new[] {new NPath("test.out")},
                    new[] {new NPath("main.cpp")},
                    "echo hello",
                    new string[] {},
                    allowUnwrittenOutputFiles: true);
                ((TundraBackend.TundraBackend)Backend.Current).ScanCppFileForHeaders(
                    new NPath("test.out"),
                    new NPath("main.cpp"),
                    new[] {_testBackend.TestRoot});
            };

            _testBackend.Build(() => "dummy", buildTask);

            NPath testHeader2 = _testBackend.TestRoot.Combine("main2.h");
            testHeader2.WriteAllText("#pragma once").SetLastWriteTimeUtc(DateTime.UtcNow);
            testHeader.WriteAllText("#include \"main2.h\"").SetLastWriteTimeUtc(DateTime.UtcNow);

            _testBackend.Build(() => "dummy", buildTask);

            var explanation = Explainer.Explain("test.out");

            Assert.AreEqual(1, explanation.RebuildReasons.Length);

            var reason1 = (InputListChangedReason)explanation.RebuildReasons[0];
            Assert.That(reason1.NewList, Has.Member(new NPath("main2.h")));
        }

        [Test]
        public void CanExplainThatNodeGotBuiltBecause_NotPreviouslyBuilt()
        {
            _testBackend.TestRoot.Combine("inputfile").WriteAllText("inputfilecontent");
            _testBackend.Build(() => "dummy", s => SetupSimpleAction("main.exe", "inputfile", HostPlatformCopyCommand), requestedTargets: new[] { "main.exe" });

            var explanation = Explainer.Explain("main.exe");
            Assert.IsInstanceOf<NotBuiltBeforeReason>(explanation.RebuildReasons.Single());
        }

        [Test]
        public void CanExplainThatNodeGotRebuiltBecause_ActionChanged()
        {
            _testBackend.TestRoot.Combine("inputfile").WriteAllText("inputfilecontent");
            _testBackend.Build(() => "dummy", s => SetupSimpleAction("main.exe", "inputfile", HostPlatformCopyCommand));
            _testBackend.Build(
                () => "dummy",
                s => SetupSimpleAction("main.exe", "inputfile", "echo", allowUnwrittenOutputFiles: true));
            var explanation = Explainer.Explain("main.exe");
            var rebuildReason = (ActionChangedReason)explanation.RebuildReasons.Single();
            StringAssert.Contains("echo", rebuildReason.NewAction);
        }

        [Test]
        public void CanExplainThatNodeGotRebuiltBecause_OutputWasMissing()
        {
            Action<string> buildCode = s =>
                Backend.Current.AddAction("CopyFile", new NPath[] {"outputfile"}, new NPath[] {"inputfile"}, HostPlatformCopyCommand, new[] {"inputfile", "outputfile"});

            _testBackend.TestRoot.Combine("inputfile").WriteAllText("inputfilecontent");
            _testBackend.Build(() => "dummy", buildCode);
            _testBackend.TestRoot.Combine("outputfile").Delete();
            _testBackend.Build(() => "dummy", buildCode);

            var explanation = Explainer.Explain("CopyFile");
            var rebuildReason = (OutputsMissingReason)explanation.RebuildReasons.Single();
            Assert.That(rebuildReason.OutputsMissing, Is.EquivalentTo(new NPath[] {"outputfile"}));
        }

        [Test]
        [Platform("Win")]
        public void CanExplainThatNodeGotRebuiltBecause_PreviousAttemptFailed()
        {
            Action<string> buildCode = s => Backend.Current.AddAction("DeliberateFail", new NPath[] {"out"}, new NPath[] {}, "cmd", new[] {"/C EXIT 1"});
            _testBackend.Build(() => "dummy", buildCode, throwOnFailure: false);
            _testBackend.Build(() => "dummy", buildCode, throwOnFailure: false);

            var explanation = Explainer.Explain("DeliberateFail");
            Assert.That(explanation.RebuildReasons.Single(), Is.InstanceOf<PreviousBuildFailedReason>());
        }

        [Test]
        public void CanExplainThatNodeGotRebuiltBecause_InputsChanged()
        {
            Action<NPath> buildCode = inputToUse =>
            {
                Backend.Current.AddAction(
                    "simpleaction",
                    new NPath[] {"main.exe"},
                    new[] {inputToUse},
                    HostPlatformCopyCommand,
                    new[] {"inputfile", "main.exe"},
                    allowUnwrittenOutputFiles: true);
            };
            _testBackend.TestRoot.Combine("inputfile").WriteAllText("inputfilecontent");
            _testBackend.Build(() => "dummy", s => buildCode("inputfile"));

            //change the declared input, but not the commandline to test that we don't crash in this rare situation
            _testBackend.Build(() => "dummy", s => buildCode("otherInput"));

            var explanation = Explainer.Explain("main.exe");
            var reason = (InputListChangedReason)explanation.RebuildReasons[0];
            Assert.That(reason.OldList, Is.EquivalentTo(new[] { new NPath("inputfile") }));
            Assert.That(reason.NewList, Is.EquivalentTo(new[] { new NPath("otherInput") }));
        }

        [Test]
        public void CanExplainThatNodeGotRebuiltBecause_ResponseFileChanged()
        {
            _testBackend.TestRoot.Combine("inputfile").WriteAllText("inputfilecontent");

            Action<string> buildCode = rsp =>
            {
                //a bit hacky, as there's no tool we can rely on that actually accepts a response file,  we fake it by doing a full echo command
                //in the command string, and then having data in the commandline arguments that are going to be sent to the rsp, that the echo just
                //turns out to ignore.
                Backend.Current.AddAction("simpleaction", new NPath[] { "main.exe" }, new NPath[] { "inputfile" },
                    "echo inputfile > main.exe ", new[] { rsp }, supportResponseFile: true);
            };
            _testBackend.Build(() => "dummy", s => buildCode("old content"));

            using (new TakeAtLeastEnoughTimeForFileSystemResolutionToIncrement()) {}

            _testBackend.Build(() => "dummy", s => buildCode("new content"));
            var explanation = Explainer.Explain("main.exe");
            var rebuildReason = (ResponseFileChangedReason)explanation.RebuildReasons.Single();

            StringAssert.Contains("old content", rebuildReason.OldContent);
            StringAssert.Contains("new content", rebuildReason.NewContent);
        }

        [Test]
        public void ExplanationOfNodeIncludesDependencyChain()
        {
            var inputFile = _testBackend.TestRoot.Combine("inputfile.1");
            _testBackend.Build(() => "dummy", s =>
            {
                Backend.Current.AddWriteTextAction(inputFile, "inputfilecontent");
                SetupSimpleAction("inputfile.2", inputFile, HostPlatformCopyCommand);
                SetupSimpleAction("inputfile.3", "inputfile.2", HostPlatformCopyCommand);
                SetupSimpleAction("inputfile.4", "inputfile.3", HostPlatformCopyCommand);
                SetupSimpleAction("inputfile.5", "inputfile.4", HostPlatformCopyCommand);
            }, true, new[] { "inputfile.5" });

            var explanation = Explainer.Explain("inputfile.1");

            Assert.That(explanation.DependencyChain, Is.Not.Null);
            Assert.That(explanation.DependencyChain.Nodes.Select(n => n.Item1.Annotation).ToArray(), Is.EqualTo(new[]
            {
                "simpleaction inputfile.5",
                "simpleaction inputfile.4",
                "simpleaction inputfile.3",
                "simpleaction inputfile.2",
                "WriteText " + inputFile
            }));
        }

        [Test]
        public void CanGetNodeByOutputPath()
        {
            var inputFile = _testBackend.TestRoot.Combine("inputfile.1");
            _testBackend.Build(() => "dummy", s =>
            {
                Backend.Current.AddWriteTextAction(inputFile, "inputfilecontent");
                SetupSimpleAction("inputfile.2", inputFile, HostPlatformCopyCommand);
                SetupSimpleAction("inputfile.3", "inputfile.2", HostPlatformCopyCommand);
                SetupSimpleAction("inputfile.4", "inputfile.3", HostPlatformCopyCommand);
                SetupSimpleAction("inputfile.5", "inputfile.4", HostPlatformCopyCommand);
            }, true, new[] { "inputfile.5" });

            var node = Explainer.FindNodeByOutput("inputfile.3");
            Assert.That(node.Annotation, Is.EqualTo($"simpleaction inputfile.3"));
        }

        [Test]
        public void CanGetNodeBySecondaryOutputPath()
        {
            _testBackend.Build(() => "dummy", s =>
            {
                Backend.Current.AddAction(
                    "Action",
                    new NPath[] {"output1", "output2", "output3", "output4"},
                    new NPath[] {},
                    "echo hello",
                    new string[] {},
                    allowUnwrittenOutputFiles: true);
            }, true, new[] { "output1" });

            var node = Explainer.FindNodeByOutput("output2");
            Assert.That(node.Annotation, Is.EqualTo($"Action output1"));
            Assert.That(Explainer.FindNodeByOutput("output3").OriginalDagIndex, Is.EqualTo(node.OriginalDagIndex));
            Assert.That(Explainer.FindNodeByOutput("output4").OriginalDagIndex, Is.EqualTo(node.OriginalDagIndex));
        }

        [Test]
        public void CanConstructDependencyChainForNodeFoundBySecondaryOutputPath()
        {
            _testBackend.Build(() => "dummy", s =>
            {
                Backend.Current.AddAction(
                    "Action1",
                    new NPath[] {"primary", "secondary"},
                    new NPath[] {},
                    "echo hello",
                    new string[] {},
                    allowUnwrittenOutputFiles: true);
                Backend.Current.AddAction(
                    "Action2",
                    new NPath[] {"output2"},
                    new NPath[] {"primary"},
                    "echo hello",
                    new string[] {},
                    allowUnwrittenOutputFiles: true);
                Backend.Current.AddAction(
                    "Action3",
                    new NPath[] {"output3"},
                    new NPath[] {"output2"},
                    "echo hello",
                    new string[] {},
                    allowUnwrittenOutputFiles: true);
                Backend.Current.AddAction(
                    "Action4",
                    new NPath[] {"output4"},
                    new NPath[] {"output3"},
                    "echo hello",
                    new string[] {},
                    allowUnwrittenOutputFiles: true);
                Backend.Current.AddAction(
                    "Action5",
                    new NPath[] {"output5"},
                    new NPath[] {"output4"},
                    "echo hello",
                    new string[] {},
                    allowUnwrittenOutputFiles: true);
            }, true, new[] { "output5" });

            var explanation = Explainer.Explain("secondary");
            Assert.That(explanation.DependencyChain, Is.Not.Null);
            Assert.That(explanation.DependencyChain.Nodes, Has.Count.EqualTo(5));
            Assert.That(explanation.DependencyChain.Nodes.Select(n => n.Item1.Annotation).ToArray(),
                Is.EqualTo(new[] {"Action5 output5", "Action4 output4", "Action3 output3", "Action2 output2", "Action1 primary"}));
        }

        [Test]
        public void CanExplainThatNodeGotRebuiltBecause_DependencyChanged()
        {
            NPath testFile = _testBackend.TestRoot.Combine("main.txt");
            testFile.WriteAllText("class A { static void Main() {} }").SetLastWriteTimeUtc(DateTime.UtcNow - TimeSpan.FromMinutes(5));

            Action<string> buildTask = _ => {
                SetupSimpleAction(new NPath("main.cs"), testFile, HostPlatformCopyCommand);
                SetupSimpleAction(new NPath("test.out"), new NPath("main.cs"), HostPlatformCopyCommand);
            };

            _testBackend.Build(() => "dummy", buildTask);

            using (new TakeAtLeastEnoughTimeForFileSystemResolutionToIncrement()) {}

            testFile.WriteAllText("class B { static void Main() { throw new System.Exception(); } }").SetLastWriteTimeUtc(DateTime.UtcNow);

            _testBackend.Build(() => "dummy", buildTask);

            var explanation = Explainer.Explain("test.out");

            Assert.AreEqual(1, explanation.RebuildReasons.Length);

            var reason1 = (DependencyChanged)explanation.RebuildReasons[0];
            Assert.That(reason1.RawChange, Is.TypeOf<InputTimeStampChangedReason>());
            Assert.AreEqual("main.cs", reason1.RawChange.File.ToString());

            var dependencyReason = (InputTimeStampChangedReason)reason1.DependencyInfo.RebuildReasons.Single();
            Assert.That(dependencyReason.File, Is.EqualTo(testFile));
        }

        private static void SetupSimpleAction(
            NPath target,
            NPath input,
            string command,
            NPath[] args = null,
            bool allowUnwrittenOutputFiles = false)
        {
            args = args ?? new[] { input, target };
            Backend.Current.AddAction(
                "simpleaction",
                new[] {target},
                new[] {input},
                command,
                args.Select(p => p.ToString(SlashMode.Native)).ToArray(),
                allowUnwrittenOutputFiles: allowUnwrittenOutputFiles);
        }

        private static string HostPlatformCopyCommand => HostPlatform.IsWindows ? "copy" : "cp";
    }
}
