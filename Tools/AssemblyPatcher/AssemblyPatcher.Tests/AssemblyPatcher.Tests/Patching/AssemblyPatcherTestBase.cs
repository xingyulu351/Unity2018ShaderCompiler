using System;
using System.CodeDom.Compiler;
using System.Collections.Generic;
using System.Diagnostics;
using System.IO;
using System.Linq;
using System.Reflection;
using System.Text;
using System.Text.RegularExpressions;
using Microsoft.Win32;
using Mono.Cecil;
using NUnit.Framework;

using AssemblyPatcher.Configuration;
using AssemblyPatcher.Extensions;
using AssemblyPatcher.Tests.Interfaces;

namespace AssemblyPatcher.Tests
{
    public class AssemblyPatcherTestBase
    {
        protected static readonly string TestBaseDir = Path.GetTempFileName();
        protected string TestAssemblyKeySnkPath;

        [OneTimeSetUp]
        public void FixtureSetup()
        {
            if (!Directory.Exists(TestBaseDir))
            {
                File.Delete(TestBaseDir);
                Directory.CreateDirectory(TestBaseDir);
            }

            CopyAssemblyToTestFolder(Assembly.GetExecutingAssembly().Location);
            CopyAssemblyToTestFolder(typeof(ITestRunner).Assembly.Location);

            // the crypto tests depend on this and it's a bit of a mess to get it into place,
            // so just jam it in
            TestAssemblyKeySnkPath =
                Path.GetFullPath(Path.Combine(TestBaseDir, Path.GetFileName("TestAssemblyKey.snk")));
            if (!File.Exists(TestAssemblyKeySnkPath))
            {
                File.Copy(FindTestFile("TestAssemblyKey.snk"), TestAssemblyKeySnkPath);
            }
        }

        internal static string FindTestFile(string origPath)
        {
            var sourcePath = origPath;
            if (File.Exists(sourcePath))
                return sourcePath;

            // try the test directory (e.g. .../bin/Debug)
            sourcePath = Path.Combine(TestContext.CurrentContext.TestDirectory, origPath);
            if (File.Exists(sourcePath))
                return sourcePath;

            // try the project root
            sourcePath = Path.Combine(TestContext.CurrentContext.TestDirectory, @"..\..", origPath);
            if (File.Exists(sourcePath))
                return sourcePath;

            throw new FileNotFoundException(String.Format("Test source {0} not found, tried '{1}' and '{2}'",
                origPath,
                Path.Combine(Environment.CurrentDirectory, origPath),
                sourcePath));
        }

        protected void AssertPatcherFailure(string testScenario, string expectedOutput)
        {
            var testSpec = CompileTestAssemblyAndExtractConfigFor(testScenario, TestBaseDir);
            PatchAssembly(testSpec, expectedOutput);
        }

        protected string Test(string testScenario, params string[] extraFiles)
        {
            CopyExtraFilesToTestDir(extraFiles);

            var testSpec = CompileTestAssemblyAndExtractConfigFor(testScenario, TestBaseDir);
            AssertUnpatchedAssembly(testSpec);

            var patchedAssembly = PatchAndVerify(testSpec);
            AssertPatchedAssembly(testSpec, patchedAssembly);

            return patchedAssembly;
        }

        protected void Test(string testScenario, Action<AssemblyDefinition> extraValidation, params string[] extraFiles)
        {
            var patchedAssemblyPath = Test(testScenario, extraFiles);
            ExecuteExtraValidations(patchedAssemblyPath, extraValidation);
        }

        private void ExecuteExtraValidations(string patchedAssemblyPath, Action<AssemblyDefinition> extraValidation)
        {
            var patchedAssembly = AssemblyDefinition.ReadAssembly(patchedAssemblyPath);
            extraValidation(patchedAssembly);
        }

        protected string PatchAndVerify(AssemblyPatcherTestSpec testSpec)
        {
            var patchedAssemblyPath = PatchAssembly(testSpec);
            PEVerify(patchedAssemblyPath);

            return patchedAssemblyPath;
        }

        protected void AssertPatchedAssembly(AssemblyPatcherTestSpec testSpec, string patchedAssemblyPath)
        {
            Assert.That(
                RunIsolated(patchedAssemblyPath),
                Is.EqualTo(testSpec.PatchedExpectation));
        }

        protected bool HasValidSignature(string assemblyPath)
        {
            return Sn("-v " + "\"" + assemblyPath + "\"", Console.WriteLine) == 0;
        }

        protected static int Sn(string args, Action<string> action)
        {
            var netFxToolsKey = Registry.LocalMachine.OpenSubKey(@"SOFTWARE\Microsoft\Microsoft SDKs\Windows\v7.0A\WinSDK-NetFx40Tools-x86", false);
            var netFxToolsPath = (string)netFxToolsKey.GetValue("InstallationFolder");

            var snProcess = Process.Start(new ProcessStartInfo
            {
                FileName = netFxToolsPath + "Sn.exe ",
                Arguments = args,
                RedirectStandardOutput = true,
                UseShellExecute = false,
                WindowStyle = ProcessWindowStyle.Hidden,
                CreateNoWindow = true
            });

            var snOutput = snProcess.StandardOutput.ReadToEnd();

            snProcess.WaitForExit(5000);

            if (snProcess.ExitCode != 0 && action != null)
            {
                action(snOutput);
            }

            return snProcess.ExitCode;
        }

        protected static void PEVerify(string patchedAssemblyPath)
        {
            var netFxToolsKey = Registry.LocalMachine.OpenSubKey(@"SOFTWARE\Microsoft\Microsoft SDKs\Windows\v7.0A\WinSDK-NetFx40Tools-x86", false);
            var netFxToolsPath = (string)netFxToolsKey.GetValue("InstallationFolder");

            var peverifyProcess = Process.Start(new ProcessStartInfo
            {
                FileName = netFxToolsPath + "PEVerify.exe ",
                Arguments = "\"" + patchedAssemblyPath + "\"" + " /VERBOSE /HRESULT",
                RedirectStandardOutput = true,
                UseShellExecute = false,
                WindowStyle = ProcessWindowStyle.Hidden,
                CreateNoWindow = true
            });

            var peverifyOutput = peverifyProcess.StandardOutput.ReadToEnd();

            peverifyProcess.WaitForExit(5000);

            if (peverifyProcess.ExitCode != 0)
            {
                Assert.Fail("PEVerify failed with code " + peverifyProcess.ExitCode + " : " + peverifyOutput);
            }
        }

        protected void AssertUnpatchedAssembly(AssemblyPatcherTestSpec testSpec)
        {
            var actual = RunIsolated(testSpec.AssemblyPath);
            Assert.That(actual, Is.EqualTo(testSpec.UnpatchedExpectation));
        }

        private string RunIsolated(string assemblyPath)
        {
            var appDomainSetup = new AppDomainSetup
            {
                ApplicationBase = Directory.GetCurrentDirectory()
            };

            using (var app = AppDomain.CreateDomain("AssemblyPatcher - " + assemblyPath, null, appDomainSetup).AsDisposable())
            {
                AppDomain appdomain = app;

                var testRunner = (ITestRunner)appdomain.CreateInstanceFromAndUnwrap(assemblyPath, "Runner");
                return testRunner.Run();
            }
        }

        protected AssemblyPatcherTestSpec CompileTestAssemblyAndExtractConfigFor(string testSourcePath, string targetDirectory, params string[] referencedAssemblyNames)
        {
            testSourcePath = FindTestFile(testSourcePath);

            var codeDomProvider = CodeDomProvider.CreateProvider("csharp");
            var compilerParameters = new CompilerParameters();
            compilerParameters.ReferencedAssemblies.AddRange(referencedAssemblyNames);
            compilerParameters.ReferencedAssemblies.Add(typeof(ITestRunner).Assembly.Location);
            compilerParameters.OutputAssembly = Path.Combine(targetDirectory, Path.GetFileNameWithoutExtension(testSourcePath) + "-test.dll");

            ConfigureCSharpCompiler(compilerParameters);

            var result = codeDomProvider.CompileAssemblyFromFile(compilerParameters, InjectRunner(testSourcePath, targetDirectory));
            if (result.Errors.HasErrors)
            {
                throw new ArgumentException(testSourcePath + " has errors: " + result.Errors.OfType<CompilerError>().Aggregate("", (acc, curr) => acc + "\r\n" + curr.ToString()));
            }

            var testContent = File.ReadLines(testSourcePath).ToList();
            return new AssemblyPatcherTestSpec(
                result.PathToAssembly,
                ConfigurantionFor(testContent),
                UnpatchedExpectationFor(testContent),
                PatchedExpectationFor(testContent));
        }

        protected virtual void ConfigureCSharpCompiler(CompilerParameters compilerParameters)
        {
        }

        private static Stream ConfigurantionFor(IEnumerable<string> testContent)
        {
            var configLines = testContent.Where(l => ConfigLineRegex.IsMatch(l)).Select(line => ConfigLineRegex.Match(line).Groups["config_line"].Value.Trim());
            if (!configLines.Any())
                return new MemoryStream();

            var configText = configLines.Aggregate((curr, acc) => curr + "\r\n" + acc);
            return new MemoryStream(Encoding.ASCII.GetBytes(configText));
        }

        private string PatchedExpectationFor(IEnumerable<string> testContent)
        {
            return ExpectationFor(testContent, PatchedExpectationRegex);
        }

        private string UnpatchedExpectationFor(IEnumerable<string> testContent)
        {
            return ExpectationFor(testContent, UnpatchedExpectationRegex);
        }

        private string ExpectationFor(IEnumerable<string> testContent, Regex expectationRegex)
        {
            var found = testContent.Where(l => expectationRegex.IsMatch(l)).ToList();
            if (found.Count != 1)
            {
                throw new ArgumentException(String.Format("Only one line can start with '{0}', found {1}.", expectationRegex, found.Count));
            }

            return expectationRegex.Match(found.Single().Trim()).Groups["expectation"].Value;
        }

        private static string InjectRunner(string testSourcePath, string targetFolder)
        {
            testSourcePath = FindTestFile(testSourcePath);
            var testOriginalContent = File.ReadAllText(testSourcePath);
            var testClassName = Path.GetFileNameWithoutExtension(testSourcePath);

            var runnerClass = String.Format(@"

                    public class Runner : System.MarshalByRefObject, AssemblyPatcher.Tests.Interfaces.ITestRunner
                    {{
                        public string Run()
                        {{
                            return new {0}().Run();
                        }}
                    }}", testClassName);

            var targetFile = Path.Combine(targetFolder, testClassName + ".cs");
            File.WriteAllText(targetFile, testOriginalContent + runnerClass);

            return targetFile;
        }

        protected string PatchSignedAssembly(AssemblyPatcherTestSpec testSpec, string keyFilePath)
        {
            return PatchAssembly(testSpec, keyFilePath: keyFilePath);
        }

        protected string PatchAssembly(AssemblyPatcherTestSpec testSpec, string expectedOutput = null, string keyFilePath = null)
        {
            var outputAssembly = Path.Combine(
                Path.GetDirectoryName(testSpec.AssemblyPath), Path.GetFileNameWithoutExtension(testSpec.AssemblyPath) + ".patched.dll");

            var patcher = new AssemblyPatcher(new Parser().Parse(testSpec.Config), TestBaseDir, TargetPlatform.General);
            Configure(patcher);

            Console.WriteLine("Source : {0}", testSpec.AssemblyPath);
            Console.WriteLine("Patched: {0}", outputAssembly);

            try
            {
                patcher.Patch(testSpec.AssemblyPath, outputAssembly, keyFilePath);
            }
            catch (PatchingException pe)
            {
                if (expectedOutput == null) throw;

                StringAssert.Contains(expectedOutput, pe.Message);
            }

            return outputAssembly;
        }

        internal virtual void Configure(AssemblyPatcher patcher)
        {
        }

        protected static void CopyExtraFilesToTestDir(params string[] extraFiles)
        {
            foreach (var filePath in extraFiles)
            {
                File.Copy(FindTestFile(filePath), Path.Combine(Path.Combine(TestBaseDir), Path.GetFileName(filePath)), true);
            }
        }

        private static void CopyAssemblyToTestFolder(string testsAssemblyPath)
        {
            File.Copy(testsAssemblyPath, Path.Combine(TestBaseDir, Path.GetFileName(testsAssemblyPath)), true);
        }

        private static Regex ConfigLineRegex = new Regex(@"^\s*//\s*~\s*(?<config_line>.*)\s*");
        private static Regex UnpatchedExpectationRegex = new Regex(@"^\s*//\s*:<\s*(?<expectation>.*)\s*");
        private static Regex PatchedExpectationRegex = new Regex(@"^\s*//\s*:>\s*(?<expectation>.*)\s*");
    }
}
