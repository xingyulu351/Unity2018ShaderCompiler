using System;
using System.IO;
using System.Linq;
using System.Collections.Generic;
using Bee.Core;
using Bee.Core.Tests;
using Bee.TestHelpers;
using JamSharp.Runtime;
using Newtonsoft.Json.Linq;
using NiceIO;
using NUnit.Framework;
using Unity.BuildTools;

namespace Bee.TundraBackend.Tests
{
    public class BeeAPITestBackend_Tundra<T> : BeeAPITestBackend<T>
    {
        public override string Name => "Tundra";
        public override bool RunsOutOfProcess => false;

        // We often invoke multiple builds in single tests, where we build something, change an input, then build again.
        // If the builds are fast, this can cause Tundra not to realise that things have changed, because the timestamps
        // have not advanced. Sometimes it's necessary to use TakeAtLeastEnoughTimeForFileSystemResolutionToIncrement,
        // but often it's enough to just clear the digest cache, so that Tundra is forced to re-hash things and discover
        // that inputs changed. So by default we do that after all builds.
        //
        // HOWEVER, there's also a few cases where we need to _not_ do that because we're investigating behaviour that
        // relies on the digest cache existing (e.g. many of the Bee.Why tests), so it's configurable.
        public bool ClearDigestCacheAfterBuild { get; }

        public BeeAPITestBackend_Tundra() : this(true)
        {
        }

        public BeeAPITestBackend_Tundra(bool clearDigestCacheAfterBuild)
        {
            ClearDigestCacheAfterBuild = clearDigestCacheAfterBuild;
        }

        public override Shell.ExecuteResult Build(Func<T> firstCallThis, Action<T> thenCallThis, bool throwOnFailure = true, string[] requestedTargets = null)
        {
            var tundraBackend = new TundraBackend(TestRoot, null, false);
            Backend.Current = tundraBackend;

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
                    Console.WriteLine(e.ToString());
                    buildCodeFailed = true;
                }
            }

            Shell.ExecuteResult executeResult;
            if (!buildCodeFailed)
            {
                var generatedJson = TestRoot.Combine("artifacts/test.dag.json.generated");
                tundraBackend.Write(generatedJson);

                executeResult = InvokeTundra(TestRoot, generatedJson, requestedTargets);
                executeResult.StdOut = stringWriter + executeResult.StdOut;
            }
            else
            {
                executeResult = new Shell.ExecuteResult()
                {
                    ExitCode = 1,
                    StdOut = stringWriter.ToString(),
                    StdErr = string.Empty
                };
            }

            TestContext.Out.WriteLine(executeResult.StdOut);
            TestContext.Out.WriteLine(executeResult.StdErr);
            if (executeResult.ExitCode != 0 && throwOnFailure)
                throw new AssertionException($"Expected a succesful execution, but exitcode was {executeResult.ExitCode}. Output was: {executeResult.StdOut + executeResult.StdErr}");

            if (ClearDigestCacheAfterBuild && (throwOnFailure || TestRoot.Combine(tundraBackend.DigestCacheFile).FileExists()))
                TestRoot.Combine(tundraBackend.DigestCacheFile).Delete();

            return executeResult;
        }

        public static Shell.ExecuteResult InvokeTundra(NPath testRoot, NPath generatedJson, string[] requestedTargets = null)
        {
            var dagJson = testRoot.Combine("artifacts/test.dag.json");
            var dag = testRoot.Combine("artifacts/test.dag");
            dag.DeleteIfExists();

            string frontendCommandLine;
            if (HostPlatform.IsWindows)
                frontendCommandLine = $"copy {generatedJson.InQuotes(SlashMode.Native)} {dagJson.InQuotes(SlashMode.Native)} >NUL";
            else
                frontendCommandLine = $"cp {generatedJson.InQuotes(SlashMode.Native)} {dagJson.InQuotes(SlashMode.Native)}";

            var executeResult = TundraInvoker.Invoke(TundraPath, dag, testRoot, frontendCommandLine, Shell.StdMode.Capture, true, requestedTargets);


            return executeResult;
        }

        internal static NPath TundraPath
        {
            get
            {
                string suffix = "";
                if (HostPlatform.IsOSX)
                    suffix = "darwin/tundra2";
                else if (HostPlatform.IsWindows)
                    suffix = "MSWin32/tundra2.exe";
                else if (HostPlatform.IsLinux)
                    suffix = "linux/tundra2";
                else
                    throw new ArgumentException();
                var tundraPath = Paths.UnityRoot.Combine($"External/tundra/builds/{suffix}");
                if (tundraPath.FileExists())
                    return tundraPath;
                return Paths.UnityRoot.Combine($"External/tundra/{suffix}");
            }
        }

        public IEnumerable<JObject> StructuredLogEvents => new NPath($"{TestRoot}/tundra.log.json").ReadAllLines().Select(JObject.Parse);
    }

    [TestFixture]
    class BeeAPITestBackend_Tundra_Tests : BackendTests
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
            return new BeeAPITestBackend_Tundra<string>();
        }
    }
}
