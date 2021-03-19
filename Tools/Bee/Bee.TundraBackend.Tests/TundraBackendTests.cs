using System;
using System.Linq;
using Bee.TestHelpers;
using Newtonsoft.Json.Linq;
using NiceIO;
using NUnit.Framework;

namespace Bee.TundraBackend.Tests
{
    [TestFixture]
    public class TundraBackendTests
    {
        private TundraBackend TestBackend;
        private NPath TestRoot;

        [SetUp]
        public void Setup()
        {
            TestRoot = new TempDirectory();
            TestBackend = new TundraBackend(TestRoot, null, true);
        }

        [Test]
        public void AddingDuplicateTargetThrows()
        {
            TestBackend.AddAction("myaction1", new NPath[] {"file1", "file2"}, new NPath[] {"input1"}, "mycommand1", new string[0]);
            Assert.Throws<ArgumentException>(() =>
                TestBackend.AddAction("myaction2", new NPath[] {"file2"}, new NPath[] {"input2"}, "mycommand2", new string[] {"myarg1"})
            );
        }

        [Test]
        public void AddingDuplicatActionsThatDiffersOnlyByTargetListThrows()
        {
            TestBackend.AddAction("myaction", new NPath[] { "file1", "file2"}, new NPath[0], "mycommand", new string[0]);
            Assert.Throws<ArgumentException>(() =>
                TestBackend.AddAction("myaction", new NPath[] {"file2"}, new NPath[0], "mycommand", new string[0])
            );
        }

        [Test]
        public void AddingDuplicatActionsThatDiffersOnlyByActionNameThrows()
        {
            TestBackend.AddAction("myaction1", new NPath[] { "file"}, new NPath[0], "mycommand", new string[0]);
            Assert.Throws<ArgumentException>(() =>
                TestBackend.AddAction("myaction2", new NPath[] {"file"}, new NPath[0], "mycommand", new string[0])
            );
        }

        [Test]
        public void AddingDuplicatActionsThatDiffersOnlyByInputThrows()
        {
            TestBackend.AddAction("myaction", new NPath[] {"file"}, new NPath[] {"input1"}, "mycommand", new string[0]);
            Assert.Throws<ArgumentException>(() =>
                TestBackend.AddAction("myaction", new NPath[] {"file"}, new NPath[] {"input2"}, "mycommand", new string[0])
            );
        }

        [Test]
        public void AddingDuplicatActionsThatDiffersOnlyByCommandThrows()
        {
            TestBackend.AddAction("myaction", new NPath[] {"file"}, new NPath[0], "mycommand1", new string[0]);
            Assert.Throws<ArgumentException>(() =>
                TestBackend.AddAction("myaction", new NPath[] {"file"}, new NPath[0], "mycommand2", new string[0])
            );
        }

        [Test]
        public void AddingDuplicatActionsThatDiffersOnlyByCommandArgThrows()
        {
            TestBackend.AddAction("myaction", new NPath[] {"file"}, new NPath[0], "mycommand", new string[] {"myarg1"});
            Assert.Throws<ArgumentException>(() =>
                TestBackend.AddAction("myaction", new NPath[] {"file"}, new NPath[0], "mycommand", new string[] {"myarg2"})
            );
        }

        [Test]
        public void AddSameTargetTwiceDoesntThrow()
        {
            TestBackend.AddAction("myaction", new NPath[] { "file1", "file2"}, new NPath[0], "mycommand", new string[0]);
            TestBackend.AddAction("myaction", new NPath[] { "file1", "file2"}, new NPath[0], "mycommand", new string[0]);
        }

        [Test]
        public void AddingRehularActionThatConflictWithTextActionThrows()
        {
            TestBackend.AddWriteTextAction(TestRoot.Combine("file1"), "contents2");
            Assert.Throws<ArgumentException>(() =>
                TestBackend.AddAction("myaction", new[] {TestRoot.Combine("file1")}, new NPath[0], "mycommand", new string[0])
            );
        }

        [Test]
        public void AddingTextActionThatConflictWithRegularActionThrows()
        {
            TestBackend.AddAction("myaction", new[] {TestRoot.Combine("file1")}, new NPath[0], "mycommand", new string[0]);
            Assert.Throws<ArgumentException>(() =>
                TestBackend.AddWriteTextAction(TestRoot.Combine("file1"), "contents2")
            );
        }

        [Test]
        public void AddingDuplicateTextActionsThrows()
        {
            TestBackend.AddWriteTextAction(TestRoot.Combine("file1"), "contents1");
            Assert.Throws<ArgumentException>(() =>
                TestBackend.AddWriteTextAction(TestRoot.Combine("file1"), "contents2")
            );
        }

        [Test]
        public void AddingSameTextActionTwiceDoesntThrow()
        {
            TestBackend.AddWriteTextAction(TestRoot.Combine("file1"), "contents1");
            TestBackend.AddWriteTextAction(TestRoot.Combine("file1"), "contents1");
        }

        [Test]
        public void AddingSameTextActionTwiceWithDifferentContentsThrows()
        {
            TestBackend.AddWriteTextAction(TestRoot.Combine("file1"), "contents1");
            var exception = Assert.Throws<ArgumentException>(() => TestBackend.AddWriteTextAction(TestRoot.Combine("file1"), "contents2"));
            StringAssert.Contains("different file contents", exception.Message);
        }

        [Test]
        public void RegisteringGeneratedFileAsInputAutomaticallySetsUpDependency()
        {
            TestBackend.AddAction("makefile", new NPath[] {"generatedfile"}, Array.Empty<NPath>(), "dummy",
                Array.Empty<string>());
            TestBackend.AddAction("use_generated_file_as_input", new NPath[] {"dummytarget"},
                new NPath[] {"generatedfile"}, "dummy", Array.Empty<string>());

            var dummyTargetIndex = TestBackend.ActionIndexFor("dummytarget");
            var generatedfileIndex = TestBackend.ActionIndexFor("generatedfile");

            var actionObject = (JObject)TestBackend.ActionsArray[dummyTargetIndex];
            var dependencies = (JArray)actionObject["Deps"];
            //we should have one dependency
            Assert.AreEqual(1, dependencies.Count);

            //on action 0 that we just registered as the first thing
            Assert.AreEqual(generatedfileIndex, (int)dependencies[0]);
        }

        [Test]
        public void CanAddIdenticalImplicitDependencies()
        {
            TestBackend.AddWriteTextAction("bar", "hello");
            TestBackend.AddImplicitDependenciesToAllActionsWithInputsThatLiveIn("foo", "bar");
            TestBackend.AddImplicitDependenciesToAllActionsWithInputsThatLiveIn("foo", "bar");
        }

        [Test]
        public void CannotAddNonIdenticalImplicitDependenciesForSameDirectory()
        {
            TestBackend.AddWriteTextAction("bar1", "hello");
            TestBackend.AddWriteTextAction("bar2", "hello");
            TestBackend.AddImplicitDependenciesToAllActionsWithInputsThatLiveIn("foo", "bar1");
            Assert.Throws<ArgumentException>(
                () => TestBackend.AddImplicitDependenciesToAllActionsWithInputsThatLiveIn("foo", "bar2")
            );
        }

        [Test]
        public void InputThatBelongsToStevedoreArtifactCausesDependencyOnStevedore()
        {
            NPath unpackLocation = "downloadpackages/somepackage";
            var fileInArtifact = unpackLocation.Combine("somesubdir/myinputfile");
            var downloadTargetNode = unpackLocation.Combine(".Stevedore");

            TestBackend.AddAction(
                actionName: "download",
                targetFiles: new[] {downloadTargetNode},
                inputs: new NPath[0],
                executableStringFor: $"echo dummy > {fileInArtifact.InQuotes(SlashMode.Native)}",
                commandLineArguments: Array.Empty<string>()
            );

            TestBackend.AddImplicitDependenciesToAllActionsWithInputsThatLiveIn(unpackLocation, downloadTargetNode);

            TestBackend.AddAction(actionName: "dummy",
                targetFiles: new NPath[] {"targetfile"},
                inputs: new NPath[] {fileInArtifact},
                executableStringFor: "echo",
                commandLineArguments: new[] {"hello"}
            );

            Assert.AreEqual(ActionObjectFor(downloadTargetNode)["DebugActionIndex"], ActionObjectFor("targetfile")["Deps"].AsJEnumerable().Single());
        }

        private JObject ActionObjectFor(NPath targetFile)
        {
            return (JObject)TestBackend.ActionsArray.Single(a => Contains(targetFile, a));
        }

        private static bool Contains(NPath targetFile, JToken a)
        {
            var jToken = a["Annotation"];
            var value = jToken.Value<string>();
            var s = targetFile.ToString();
            var contains = value.Contains(s);
            return contains;
        }
    }
}
