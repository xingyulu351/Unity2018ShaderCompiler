using System;
using System.Diagnostics;
using System.Linq;
using Bee.Stevedore.Program;
using Bee.TestHelpers;
using Bee.Tests;
using JamSharp.Runtime;
using NiceIO;
using NUnit.Framework;
using Unity.BuildTools;

namespace Bee.Stevedore.Unity.Tests
{
    // These tests try to build HelloWorld using various beefiles. Beefiles
    // are picked based on test name and loaded from assembly resources; be
    // sure to jam BuildSystemProjectFiles if beefiles are added or removed.
    public class BeeStevedoreIntegrationTests
    {
        private BeeAPITestBackend_StandaloneBee<string> m_Backend;
        private Shell.ExecuteResult m_Results;

        [SetUp]
        public void SetUp()
        {
            m_Backend = null;
            m_Results = null;
        }

        [TearDown]
        public void TearDown()
        {
            var root = m_Backend?.TestRoot;
            if (root != null && root.Exists())
                StevedoreProgram.RecursivelyDeleteDirectoryWithLongPaths(root);
        }

        void RunBeeInVerboseMode(int pathBudget = TempDirectory.DefaultPathBudget)
        {
            m_Backend = new BeeAPITestBackend_StandaloneBee<string>();
            m_Backend.Start(null, pathBudget);
            m_Backend.TestRoot.Combine("src/HelloWorld.cpp").WriteAllText(@"
                #include <stdio.h>

                int main()
                {
                    puts(""Hello, Steve!"");
                    return 0;
                }
                ");

            var beeFileResource = $"{TestContext.CurrentContext.Test.MethodName}_beefile.cs";
            m_Backend.TestRoot.Combine("Build.bee.cs")
                .WriteAllText(ResourceHelper.ReadTextFromResource(typeof(BeeStevedoreIntegrationTests).Assembly, beeFileResource));

            m_Results = m_Backend.InvokeStandaloneBee(true, new[] {"-v"});
        }

        void AssertUnpackedArtifacts(params string[] artifacts)
        {
            foreach (var name in artifacts)
            {
                StringAssert.Contains($"Download and unpack artifacts/Stevedore/{name}/.StevedoreVersion", m_Results.StdOut);
            }
            // Ensure we unpacked only the expected artifacts, and no more.
            CollectionAssert.AreEquivalent(artifacts, m_Backend.TestRoot.Combine("artifacts/Stevedore").Directories().Select(p => p.FileName));
        }

        void AssertOutputContains(params string[] texts)
        {
            foreach (var s in texts)
                StringAssert.Contains(s, m_Results.StdOut);
        }

        void AssertEnvironmentPathNotReferenced(string environmentVariableName, bool required = false)
        {
            var path = Environment.GetEnvironmentVariable(environmentVariableName);
            if (string.IsNullOrEmpty(path))
            {
                Assert.IsFalse(required, $"{environmentVariableName} was not set in environment, but is required for test");
                return;
            }
            StringAssert.DoesNotContain(path, m_Results.StdOut);
        }

        void AssertDidNotUseProgramFiles()
        {
            AssertEnvironmentPathNotReferenced("ProgramFiles", required: true);
            AssertEnvironmentPathNotReferenced("ProgramFiles(x86)");
            AssertEnvironmentPathNotReferenced("ProgramData");
        }

        void AssertFileExists(NPath path)
        {
            Assert.IsTrue(m_Backend.TestRoot.FileExists(path), $"Expected {path} to be an existing file");
        }

        [Test]
        public void HelloWorldEmscripten()
        {
            // This test requires excessively long paths...
            var longestPath = @"artifacts\Stevedore\winpython2-x64\WinPython-64bit-2.7.13.1Zero/python-2.7.13.amd64/Lib/site-packages/pip/_vendor/requests/packages/urllib3/packages/ssl_match_hostname/_implementation.pyc";
            RunBeeInVerboseMode(pathBudget: longestPath.Length);

            if (HostPlatform.IsWindows)
            {
                AssertUnpackedArtifacts(
                    "7za-win-x64",
                    "emscripten",
                    "emscripten-llvm-win-x64",
                    "node-win-x64",
                    "roslyn-csc-win64",
                    "winpython2-x64"
                );

                AssertOutputContains(
                    @"""artifacts\Stevedore\winpython2-x64\WinPython-64bit-2.7.13.1Zero\python-2.7.13.amd64\python.exe"" ""artifacts\Stevedore\emscripten\emscripten-1.38.3\emcc.py"""
                );

                AssertDidNotUseProgramFiles();
            }
            else if (HostPlatform.IsOSX)
            {
                AssertUnpackedArtifacts(
                    "7za-mac-x64",
                    "emscripten",
                    "emscripten-llvm-mac-x64",
                    "node-mac-x64",
                    "roslyn-csc-mac"
                );

                AssertOutputContains(
                    @"""/usr/bin/python"" ""artifacts/Stevedore/emscripten/emscripten-1.38.3/emcc.py"""
                );
            }
            else if (HostPlatform.IsLinux)
            {
                AssertUnpackedArtifacts(
                    "7za-linux-x64",
                    "emscripten",
                    "emscripten-llvm-linux-x64",
                    "node-linux-x64",
                    "roslyn-csc-linux"
                );

                AssertOutputContains(
                    @"""/usr/bin/python2"" ""artifacts/Stevedore/emscripten/emscripten-1.38.3/emcc.py"""
                );
            }
            else throw new InvalidOperationException("impossible!");

            AssertFileExists("build/HelloWorld.js");
            AssertEnvironmentPathNotReferenced("EMSDK");
        }

        [Test]
        public void HelloWorldNativePinned()
        {
            if (HostPlatform.IsLinux)
                throw new IgnoreException("Presently not supported on Linux");

            try
            {
                RunBeeInVerboseMode();
            }
            finally
            {
                if (HostPlatform.IsWindows)
                {
                    // To avoid sharing violations during test teardown, attempt to
                    // clean up these lingering processes left behind by Visual Studio.
                    // Note that mspdbsrv.exe will gladly take on responsibilities for
                    // builds using unrelated Visual Studio installations; thus any
                    // other VS ongoing builds may break when we kill it.
                    KillWindowsProcesses("vctip.exe");
                    KillWindowsProcesses("mspdbsrv.exe");
                }
            }

            if (HostPlatform.IsWindows)
            {
                AssertUnpackedArtifacts(
                    "7za-win-x64",
                    "roslyn-csc-win64",
                    "vs2017-toolchain",
                    "win10sdk"
                );

                AssertOutputContains(
                    @"PATH=artifacts\Stevedore\win10sdk\bin\x64;artifacts\Stevedore\win10sdk\bin\x86",
                    @"artifacts/Stevedore/vs2017-toolchain/bin/Hostx64/x64/link"
                );

                AssertDidNotUseProgramFiles();

                AssertFileExists("build/HelloWorld.exe");
            }
            else if (HostPlatform.IsOSX)
            {
                AssertUnpackedArtifacts(
                    "7za-mac-x64",
                    "mac-toolchain-10_12",
                    "roslyn-csc-mac"
                );

                AssertOutputContains(
                    "artifacts/Stevedore/mac-toolchain-10_12/usr/bin/clang++"
                );

                AssertFileExists("build/HelloWorld");
            }
        }

        static void KillWindowsProcesses(string imageName)
        {
            // Use taskkill.exe to kill processes; the pure .NET System.Diagnostics way have so far been fruitless.
            while (true)
            {
                var proc = Process.Start("taskkill.exe", $"/F /IM \"{imageName}\"");
                proc.WaitForExit();

                // taskkill.exe kills only the first matching process, but we
                // need to kill all of them, as there's no straightforward way
                // to distinguish based on full path. Thus we loop until we get
                // an error (presumably "no such process found").
                if (proc.ExitCode != 0) break;
            }
        }
    }
}
