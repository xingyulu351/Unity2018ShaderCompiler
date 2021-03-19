using System;
using System.IO;
using System.Linq;
using System.Text;
using Bee.Core;
using Bee.Core.Tests;
using Bee.TestHelpers;
using Bee.TundraBackend.Tests;
using JamSharp.JamState;
using JamSharp.Runtime;
using NiceIO;
using NUnit.Framework;
using Unity.BuildSystem.Common;
using Unity.BuildSystem.JamStateToTundraConverter;
using JamTarget = JamSharp.JamState.JamTarget;

namespace Unity.BuildSystem.JamStateToBeeConverter.Tests
{
    public class BeeApiTestBackend_JamConvertedToTundra<T> : BeeAPITestBackend<T>
    {
        public override string Name { get; } = "JamConvertedToTundra";
        public override bool RunsOutOfProcess => false;

        public override Shell.ExecuteResult Build(Func<T> firstCallThis, Action<T> thenCallThis, bool throwOnFailure = true, string[] requestedTargets = null)
        {
            var jamBackend = new JamBackend();
            Backend.Current = jamBackend;

            var jamStateBuilder = new JamStateBuilder();
            RuntimeManager.ParallelJamStateBuilder = jamStateBuilder;

            var stringWriter = new StringWriter();
            bool buildCodeFailed = false;
            using (new TempChangeDirectory(TestRoot))
            using (new TempConsoleRedirect(stringWriter))
            {
                try
                {
                    thenCallThis(firstCallThis());
                }
                catch (Exception e)
                {
                    if (throwOnFailure)
                        throw;
                    Console.WriteLine(e.ToString());
                    buildCodeFailed = true;
                }
            }

            if (buildCodeFailed)
                return new Shell.ExecuteResult()
                {
                    ExitCode = 1,
                    StdOut = stringWriter.ToString(),
                    StdErr = string.Empty
                };

            var generatedJson = TestRoot.Combine("artifacts/test.dag.json.generated");

            new Converter().Convert(jamStateBuilder.Build(), requestedTargets, generatedJson, jamBackend);

            TestRoot.Combine("artifacts/tundra").EnsureDirectoryExists();

            var executeResult = BeeAPITestBackend_Tundra<T>.InvokeTundra(TestRoot, generatedJson, requestedTargets ?? new[] {"convertedjamtargets"});

            executeResult.StdOut = stringWriter + executeResult.StdOut;

            TestContext.Out.WriteLine("Tundra output:\n" + executeResult.StdOut);
            return executeResult;
        }
    }

    [TestFixture]
    class BeeAPITestBackend_JamConvertedToTundra_Tests : BackendTests
    {
        [Test]
        public void CanRunTest()
        {
            var testBackend = MakeTestBackend();
            testBackend.Start();
            var result = testBackend.Build(() => "dummy", dummy =>
            {
                Backend.Current.AddWriteTextAction("myfile", "mycontents");
            });
            TestContext.Out.WriteLine("Output: " + result.StdOut);
            Assert.AreEqual("mycontents", testBackend.TestRoot.Combine("myfile").ReadAllText());
        }

        protected override BeeAPITestBackend<string> MakeTestBackend()
        {
            return new BeeApiTestBackend_JamConvertedToTundra<string>();
        }
    }
}
