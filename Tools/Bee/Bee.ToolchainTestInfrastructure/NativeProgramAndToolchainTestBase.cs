using System;
using Bee.Core;
using Bee.NativeProgramSupport.Core;
using JamSharp.Runtime;
using NiceIO;
using NUnit.Framework;
using Unity.BuildSystem.NativeProgramSupport;

/*
 * The "how do these native program tests work" story:
 *
 * the NativeProgram api is a bee api. It allows you to use c# to describe how you want a c++ program built.
 * Because NativeProgram was written for Unity when unity was transitioning from a jam backend to a tundra backend,
 * NativeProgram needs to be able to output graphs for both. We've tried to abstract the backend away behind IBackend interface.
 *
 * A lot of NativeProgram functionality can be tested through unit tests, and doesn't require a backend.  you just assert certain
 * things on the generated graph.  For integration tests that test if the generated graph actually works in the real world(tm)
 * you need to actually run it on a backend. Other bee api's (CSharpProgram / CSharpSupport) have the same challenge.
 *
 * All bee api's talk to Bee.Core.IBackend interface.  In order to make it possible to test those api's there is also a
 * "generic" test base class for when you want to test against real IBackend implementations. We have three backends that work
 * today. One is the standalone bee.exe.  Running against that is facilitated by BeeApiTestBackend_StandaloneBee.
 * Another one is BeeApiTestBackend_Jam. The fastest one is BeeApiTestBackend_Tundra, which directly writes out a tundra file.
 * w
 * Further complicating things for the NativeProgram tests, is that if you want to test if they work in the real world,
 * You also need to test it using real toolchains. So now for each test, you need to run it against both test backends,
 * as well as for each toolchain. Some tests we really want to run on all toolchains, some tests we really want to run on
 * all backends, and some tests we want to run on both. tests can specify in how many variations they want to be ran.
 *
 * How the test themselves work is also a bit tricky. For the bee and jam backends you write some test code that writes out some sourcefiles.
 * Then you want to provide some C# code that talks to the bee api under test. then you want to run the backend.  and then
 * you want to assert things like "was this error message printed".  "did it build my program".  "if I now run that program does
 * it print what I expected".  The "some c# code that talks to the bee api" is tricky, because that code is actually not
 * executed by the nunit process. the nunit test has to start the backend, which is a different executable. (jam binary or bee.exe).
 * it is that different process that will be executing the provided sharp.  Because it is very conveninient when authoring
 * the tests, to have the "buildcode" and the "testcode" right next to each other, we go through some lengths to make that possible.
 * the tundra testbackend is the only one that executes in the test process, which makes it the fastest backend, as well as the
 * most convenient one for debugging
 *
 * The test framework has to inform the backend "where the c# code that talks to the bee api lives".  For the bee and jam testbackends,
 * It does this by writing out the assembly name, typename, and methodname of the buildcode, that lives _in the actual test assembly_.  so you have
 * the test assembly loaded by nunit, which then starts bee.exe, which then does an Assembly.Load() on the same test assembly again.
 * This is the reason that you can only invoke static methods, and that the buildcode cannot be a lambda that captures all sorts of
 * arguments. We make one exception: instead of using a static method, you're also allowed to use a lambda that doesn't capture
 * anything. Technically this isn't a static method, but it's close enough, and under the hood we make it work.
 *
 * For the bee-standalone backend, the tests work slightly different than a "real" bee distirbution.  In a real bee distribution
 * we ILMerge BeeFrontend.exe and all its dependencies into one big Bee.exe.  The buildgraph for this is being setup by our
 * C# code. If we would want to test that Bee.exe directly, it means we have to run a jam command every time we want to test
 * a code change. That would break traditional NUnit workflow where you can just run tests from Rider. So what we do in these tests
 * is actually run against the non-ilmerged BeeFrontend.exe. At the time of this writing that feels like a good tradeoff between
 * "test-what-you-ship"  and  "make working on tests not be horrible"
 *
 * - Lucas 2018.
 */


namespace Bee.NativeProgramAndToolchainTests.IntegrationTests
{
    public class NativeProgramAndToolchainTestBase : ToolchainAndBackendTestBase
    {
        protected NPath MainCpp => TestRoot.Combine("main.cpp");

        protected void CppFile(string name, string text)
        {
            TestRoot.Combine($"{name}.cpp").WriteAllText(text);
        }

        private static NPath SetupNativeProgramInternal(ToolChain toolchain, Action<NativeProgram> configureCallback, bool lump, NativeProgramFormat format)
        {
            var np = new NativeProgram("program");
            np.Sources.Add(".");
            configureCallback?.Invoke(np);

            var config = new NativeProgramConfiguration(CodeGen.Debug, toolchain, lump);
            return np.SetupSpecificConfiguration(config,
                format ?? toolchain.ExecutableFormat,
                targetDirectory: new NPath(""),
                artifactsPath: $"artifacts/{np.Name}"
            );
        }

        protected static NPath SetupNativeProgram(ToolChain toolchain, Action<NativeProgram> configureCallback = null, bool lump = false, NativeProgramFormat format = null)
        {
            return SetupNativeProgramInternal(toolchain, configureCallback, lump, format);
        }

        protected static NPath SetupNativeProgram(string programName, string cppName, NativeProgramFormat format, NPath targetDirectory, params PrecompiledLibrary[] dependencies)
        {
            var config = new NativeProgramConfiguration(CodeGen.Debug, format.Toolchain, false);
            var program = new NativeProgram(programName)
            {
                Sources = { cppName },
                PrebuiltLibraries = { dependencies },
            };
            return program.SetupSpecificConfiguration(config, format, targetDirectory: targetDirectory);
        }

        protected static void SetupNativeProgramAndRun(ToolChain toolchain, Action<NativeProgram> configureCallback = null, bool lump = false, NativeProgramFormat format = null, bool ignoreProgramExitCode = false)
        {
            var target = SetupNativeProgramInternal(toolchain, configureCallback, lump, format);
            SetupRunProgram(toolchain, target, ignoreProgramExitCode);
        }

        protected static InvocationResult SetupRunProgram(ToolChain toolchain, NPath target, bool ignoreProgramExitCode = false)
        {
            return toolchain.ExecutableFormat.SetupInvocationOfProducedExecutable(target, ignoreProgramExitCode: ignoreProgramExitCode);
        }

        public void BuildAndCheckProgramOutput(Action<ToolChain> buildCode, string expectedOutput, string executableName = "program", bool throwOnFailure = true)
        {
            Build(buildCode, throwOnFailure);
            var outputFilepath = TestBackend.TestRoot.Combine($"{executableName}.out");
            Assert.AreEqual(expectedOutput, outputFilepath.ReadAllText().Trim());
        }

        protected Shell.ExecuteResult Build(Action<ToolChain> buildCode, bool throwOnFailure = true)
        {
            return TestBackend.Build(TestBackend.RunsOutOfProcess ? Toolchain.CreatingFunc : () => Toolchain, buildCode, throwOnFailure: throwOnFailure);
        }
    }
}
