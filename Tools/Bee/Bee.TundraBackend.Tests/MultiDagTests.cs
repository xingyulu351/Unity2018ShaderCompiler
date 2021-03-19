using System;
using JamSharp.Runtime;
using NiceIO;
using NiceIO.Tests;
using NUnit.Framework;
using NUnit.Framework.Internal;
using Unity.BuildTools;

namespace Bee.TundraBackend.Tests
{
    [TestFixture]
    public class MultiDagTests : TestWithTempDir
    {
        [Test]
        [Platform(Exclude = "Win", Reason = "'touch' not available on Windows (case 1067983)")]
        public void UsingTwoDagsOnSingleBuildState_SmallerDag_DoesNotDelete_BigDagsFiles()
        {
            var tundraArtifacts = _tempPath.Combine("artifacts");

            //we create a graph with two outputs, and build it.
            InvokeTundraOn(tundraArtifacts, "test.dag.testjson", new NPath[] {"output1", "output2"});
            Assert.IsTrue(_tempPath.Combine("output2").FileExists());

            //now we create another graph, that uses the same buildstate, that has fewer nodes.
            //it should not delete output2, as we only delete files from the buildstate if we've seen it exist in the graph we're currently buiding.
            //this is a different graph, so the deletion should not happen
            InvokeTundraOn(tundraArtifacts, "othertest.dag.testjson", new NPath[] {"output1"});
            Assert.IsTrue(_tempPath.Combine("output2").FileExists());
        }

        [Test]
        [Platform(Exclude = "Win", Reason = "'touch' not available on Windows (case 1067983)")]
        public void OutputNoLongerUsedByDagGetsRemoved()
        {
            var tundraArtifacts = _tempPath.Combine("artifacts");

            //we create a graph with two outputs, and build it.
            InvokeTundraOn(tundraArtifacts, "test.dag.testjson", new NPath[] {"output1", "output2"});
            Assert.IsTrue(_tempPath.Combine("output2").FileExists());

            //we create a graph at the same location, with one fewer output.  this should cause a delete
            InvokeTundraOn(tundraArtifacts, "test.dag.testjson", new NPath[] {"output1"});
            Assert.IsFalse(_tempPath.Combine("output2").FileExists());
        }

        private Shell.ExecuteResult InvokeTundraOn(NPath tundraArtifacts, string testDagTestjson, NPath[] outputs)
        {
            var jsonPath = tundraArtifacts.Combine(testDagTestjson);
            WriteGraphWithOutputs(tundraArtifacts, jsonPath, outputs);

            var dagJson = jsonPath.ChangeExtension("json");
            NPath dag = dagJson.Parent.Combine(dagJson.FileNameWithoutExtension).DeleteIfExists();

            return TundraInvoker.Invoke(BeeAPITestBackend_Tundra<int>.TundraPath, dag, _tempPath, FrontendCommandLineFor(jsonPath, dagJson), Shell.StdMode.Capture, true, null).ThrowOnFailure();
        }

        private static string FrontendCommandLineFor(NPath jsonPath, NPath dagJson)
        {
            return HostPlatform.IsWindows
                ? $"copy {jsonPath.InQuotes(SlashMode.Native)} {dagJson.InQuotes(SlashMode.Native)} >NUL"
                : $"cp {jsonPath.InQuotes(SlashMode.Native)} {dagJson.InQuotes(SlashMode.Native)}";
        }

        private static void WriteGraphWithOutputs(NPath tundraArtifacts, NPath jsonPath, NPath[] outputs)
        {
            var tundraBackend = new TundraBackend(tundraArtifacts);

            foreach (var output in outputs)
            {
                tundraBackend.AddAction(actionName: "CreateOutput",
                    targetFiles: new NPath[] {output},
                    inputs: new NPath[] {},
                    executableStringFor: "touch",
                    commandLineArguments: new[] {output.InQuotes(SlashMode.Native)});
            }

            tundraBackend.Write(jsonPath);
        }
    }
}
