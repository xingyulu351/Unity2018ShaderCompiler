using System;
using System.Collections.Generic;
using System.IO;
using System.Text.RegularExpressions;
using NUnit.Framework;
using Common;
using UnityEngine;
using System.Diagnostics;
using System.Linq;
using ICSharpCode.NRefactory.CSharp.Refactoring;
using UnityEngine.Networking;

namespace InternalCallReplacer.Tests
{
    public class IntegrationTestsBase
    {
        private static string currentTestName = "";
        protected static void RunIntegrationTest(string testname)
        {
            RunIntegrationTest(testname, false);
        }

        protected static void RunIntegrationTest(string testname, bool runUNetWeaver)
        {
            currentTestName = testname;
            RunIntegrationTest("Input.cs",
                "ExpectedOutput.cs",
                "ExpectedOutput.il", runUNetWeaver);
            currentTestName = "";
        }

        private static string ReadTestFile(string testname, string inputCs)
        {
            var path = FixturePathFor(testname) + "/" + inputCs;
            if (!File.Exists(path))
                return null;

            return File.ReadAllText(path);
        }

        private static void WriteTestFile(string testname, string inputCs, string sourceCode)
        {
            var path = FixturePathFor(testname) + "/" + inputCs;
            File.WriteAllText(path, sourceCode.Replace("\t", "    "));
        }

        private static string RemoveCommentsFrom(string fileContents)
        {
            return Regex.Replace(fileContents, @"/\*.+?\*/", string.Empty, RegexOptions.Singleline);
        }

        protected static string FixturePathFor(string testname)
        {
            return Path.Combine(CalculateProjectDirectory(), testname);
        }

        private static string CalculateProjectDirectory()
        {
            var exeDirectory = Path.GetDirectoryName(new Uri(System.Reflection.Assembly.GetExecutingAssembly().CodeBase).LocalPath);
            return Path.GetFullPath(Path.Combine(exeDirectory, "../../Tests/Fixtures"));
        }

        private static string FixSource(string text)
        {
            text = text.Replace("\r\n", "\n");
            text = text.Replace("    ", "\t");
            if (text.Length > 1 && text[text.Length - 1] == '\n')
            {
                text = text.Substring(0, text.Length - 1);
            }

            return text;
        }

        private static string UnityEngineDLLPath
        {
            get { return typeof(IUnitySerializable).Assembly.Location; }
        }

        private static string UnityEngineNetworkingDLLPath
        {
            get { return typeof(NetworkBehaviour).Assembly.Location; }
        }

        private static string CompareSources(string expected, string input)
        {
            if (expected == null) return "";
            try
            {
                Assert.AreEqual(FixSource(expected), FixSource(input));
            }
            catch (Exception ex)
            {
                return ex.Message + "\n";
            }
            return "";
        }

        private static string RunPEVerify(string assemblyPath)
        {
            string peverifyPath = @"C:\Program Files (x86)\Microsoft SDKs\Windows\v8.1A\bin\NETFX 4.5.1 Tools\peverify.exe";

            // PEVerify requires dependencies to be in the same folder
            string dstUnityEnginePath = Path.Combine(Path.GetDirectoryName(assemblyPath), "UnityEngine.dll");
            if (!File.Exists(dstUnityEnginePath))
                File.Copy(UnityEngineDLLPath, dstUnityEnginePath);
            string dstUnityEngineNetworkingPath = Path.Combine(Path.GetDirectoryName(assemblyPath), "UnityEngine.Networking.dll");
            if (!File.Exists(dstUnityEngineNetworkingPath))
                File.Copy(UnityEngineNetworkingDLLPath, dstUnityEngineNetworkingPath);

            var errorsToIgnore = new[]
            {
                "/IGNORE=0x8013185D", // "[found address of Byte] Expected numeric type on the stack." - this happens because we convert reference to first member of byte array to an unmanaged pointer. This operation is valid, but not verifyable.
            };

            var p = new Process();
            p.StartInfo.FileName = peverifyPath;
            p.StartInfo.Arguments = string.Format("/NOLOGO /HRESULT {0} {1}", errorsToIgnore.Aggregate((x, y) => x + " " + y), assemblyPath);
            p.StartInfo.UseShellExecute = false;
            p.StartInfo.RedirectStandardOutput = true;
            p.Start();

            p.WaitForExit();

            if (p.ExitCode != 0)
            {
                string log = p.StandardOutput.ReadToEnd();
                return log;
            }
            return "";
        }

        private static void RunIntegrationTest(string inputCSFileName, string expectedCSFileName, string expectedILFileName, bool runUNetWeaver)
        {
            Console.WriteLine("Project directory is " + CalculateProjectDirectory());
            string targetFolder = Path.Combine(Path.GetTempPath(), "SerializationWeaver");
            if (Directory.Exists(targetFolder)) Directory.Delete(targetFolder, true);

            AssemblyTools.SetTempPath(targetFolder);

            var inputCS = ReadTestFile(currentTestName, inputCSFileName);
            if (inputCS == null) throw new FileNotFoundException("Input.cs file not found for test " + currentTestName);

            var expectedCS = ReadTestFile(currentTestName, expectedCSFileName);
            var expectedIL = ReadTestFile(currentTestName, expectedILFileName);


            string inputAssembly = AssemblyTools.CompileAssembly(inputCS, new string[] { UnityEngineDLLPath, UnityEngineNetworkingDLLPath }, Compiler.Mono, "Assembly-CSharp");
            string outputAssembly = Path.Combine(targetFolder, "Out\\Assembly-CSharp.dll");

            List<string> args = new List<string>();
            args.Add(string.Format("-assembly={0}", inputAssembly));
            args.Add("-pdb");
            args.Add("-verbose");
            args.Add(string.Format("-unity-engine={0}", UnityEngineDLLPath));
            if (runUNetWeaver)
                args.Add(string.Format("-unity-networking={0}", UnityEngineNetworkingDLLPath));
            args.Add(string.Format("{0}", Path.Combine(targetFolder, "Out")));

            int result = usw.Program.Main(args.ToArray());

            Assert.AreEqual(result, 0, "SerializationWeaver failure.");

            var moduleDefinition = AssemblyTools.ReadAssembly(outputAssembly, Path.GetDirectoryName(UnityEngineDLLPath), Path.GetDirectoryName(UnityEngineNetworkingDLLPath));
            var outputCS = AssemblyTools.ConvertAssemblyToCSharp(moduleDefinition);
            outputCS = "using UnityEngine;\r\n" +
                "using UnityEngine.Serialization;\r\n" +
                outputCS;

            var outputIL = AssemblyTools.ConvertAssemblyToIL(moduleDefinition);

            // Used for reseting all test output, so don't need to do it by hand.
            // So after setting resetTests to true, run all tests once, so all tests will be overwritten (they will still fail)
            // Then you must set it back to false, run all tests again, they should be all green.
            bool resetTests = false;

            if (resetTests)
            {
                WriteTestFile(currentTestName, expectedCSFileName, outputCS);
                WriteTestFile(currentTestName, expectedILFileName, outputIL);
            }

            string peverifyLog = RunPEVerify(outputAssembly);
            if (!string.IsNullOrEmpty(peverifyLog)) Assert.Fail(peverifyLog);

            string exceptionMessage = "";
            exceptionMessage += CompareSources(expectedCS, outputCS);
            exceptionMessage += CompareSources(expectedIL, outputIL);

            Assert.AreEqual("", exceptionMessage);
        }
    }
}
