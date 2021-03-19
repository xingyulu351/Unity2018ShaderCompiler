using System;
using System.Collections.Generic;
using System.IO;
using System.Linq;
using System.Text;
using NUnit.Framework;
using Unity.CommonTools;
using System.Text.RegularExpressions;

namespace BindingsToCsAndCpp.Tests
{
    [TestFixture]
    public class UnityEngineTests
    {
        private string _activePlatform;

        private static string TestsDirectory
        {
            get
            {
                return Path.GetFullPath(Path.Combine(Workspace.BasePath, @"Tools\UnityBindingsParser\BindingsToCsAndCpp\Tests\UnityEngineTestFixtures"));
            }
        }

        private string OutputFilesDirectory
        {
            get { return Paths.UnifyDirectorySeparator(Path.Combine(Workspace.BasePath, @"artifacts\Tests\UnityEngineTests\ExportGenerated\" + _activePlatform)); }
        }

        private string ExpectedFilesDirectory
        {
            get { return Paths.UnifyDirectorySeparator(Path.Combine(TestsDirectory, @"Expected\" + _activePlatform)); }
        }

        private void ResetTests()
        {
            var generatedFiles = new string[0];
            if (Directory.Exists(OutputFilesDirectory))
                generatedFiles = Directory.GetFiles(OutputFilesDirectory, "*.cpp");
            if (!Directory.Exists(ExpectedFilesDirectory))
                Directory.CreateDirectory(ExpectedFilesDirectory);
            foreach (var file in generatedFiles)
            {
                var dst = Path.Combine(ExpectedFilesDirectory, Path.GetFileName(file));
                File.Copy(file, dst, true);
            }
        }

        private void TestPlatform(string platformName)
        {
            _activePlatform = platformName;

            var inputDirectory = Paths.UnifyDirectorySeparator(Path.Combine(TestsDirectory, "Export"));
            var inputFiles = Directory.GetFiles(inputDirectory, Constants.SourceFileWildcard);

            var args = new List<string> {"-nocs", "-platform=" + _activePlatform, "-output=" + OutputFilesDirectory};
            args.AddRange(inputFiles);

            if (Directory.Exists(OutputFilesDirectory))
                Directory.Delete(OutputFilesDirectory, true);

            Program.Main(args.ToArray());

            var expectedFiles = new string[0];

            Assert.IsTrue(Directory.Exists(ExpectedFilesDirectory));

            expectedFiles = Directory.GetFiles(ExpectedFilesDirectory, "*.cpp").Select(m => Path.GetFileName(m)).ToArray();

            var allExceptions = new StringBuilder();
            foreach (var file in expectedFiles)
            {
                var generated = Path.Combine(OutputFilesDirectory, file);
                var expected = Path.Combine(ExpectedFilesDirectory, file);

                try
                {
                    if (!File.Exists(generated)) throw new Exception(generated + " not found.");

                    var generatedSrc = File.ReadAllText(generated);
                    var expectedSrc = File.ReadAllText(expected);

                    var generatedSrcTrimmed = Regex.Replace(generatedSrc, @"[ \t]+", String.Empty);
                    var expectedSrcTrimmed = Regex.Replace(expectedSrc, @"[ \t]+", String.Empty);

                    var generatedSrcLines = Regex.Split(generatedSrcTrimmed, @"\r?\n|\r");
                    var expectedSrcLines = Regex.Split(expectedSrcTrimmed, @"\r?\n|\r");

                    var expectedFilename = Path.GetFileName(expected);

                    Assert.AreEqual(expectedSrcLines.Length, generatedSrcLines.Length,  "Line count does not match for " + expectedFilename);

                    for (int i = 0; i < Math.Min(generatedSrcLines.Length, expectedSrcLines.Length); ++i)
                        Assert.AreEqual(expectedSrcLines[i], generatedSrcLines[i], "Mismatch at "  + expectedFilename + ":" + (i + 1) + " (Whitespace is ignored)");
                }
                catch (Exception ex)
                {
                    allExceptions.AppendLine(ex.Message);
                }
            }

            // Uncomment this if you want to reset all the tests, this simply copies _generated_ source to _expected_ directory.
            // On first run it will fail, because we do check before resetting, but on second time it will succeed
            // That's done on purpose, so I can which tests will be reseted
            //ResetTests();

            var allExceptionsMessage = allExceptions.ToString();
            if (allExceptionsMessage.Length > 0) throw new Exception(allExceptionsMessage);
        }

        [Test]
        public void Editor()
        {
            TestPlatform("Editor");
        }

        [Test]
        public void Metro()
        {
            TestPlatform("Metro");
        }
    }
}
