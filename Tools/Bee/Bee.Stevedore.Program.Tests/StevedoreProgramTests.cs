using System.Diagnostics;
using System.IO;
using Bee.TestHelpers;
using NiceIO;
using NUnit.Framework;

namespace Bee.Stevedore.Program.Tests
{
    public class StevedoreProgramTests : StevedoreTestsBase
    {
        [Test]
        public void ShowConfig_DefaultConfig()
        {
            var config = new StevedoreConfig();
            var output = new StringWriter();
            StevedoreProgram.ShowConfig(config, output);

            Assert.AreEqual(@"Config files:

Configuration:
  cache-folder =
  timeout = 10000
  repo.public.url = https://stevedore.unity3d.com/r/public/
  repo.testing.url = http://stevedore.hq.unity3d.com/r/testing/
  repo.unity-internal.url = http://stevedore.hq.unity3d.com/r/unity-internal/
".Replace("\r", ""), output.ToString().Replace(" \n", "\n"));
        }

        [TestCase("cache-folder = dummy")]
        [TestCase("timeout = 42")]
        [TestCase("proxy = http://test")]
        [TestCase("proxy.http = http://etc")]
        [TestCase("repo.xyzzy.http-header = Foo: Bar")]
        public void ShowConfig_Roundtrip(string configText)
        {
            ReadConfig(out var config, configText);

            var output = new StringWriter();
            StevedoreProgram.ShowConfig(config, output);

            var actualOutput = output.ToString();
            StringAssert.Contains($"\n  {configText}\n", actualOutput);
        }

        [Test]
        public void GetArtifact_CacheFolderNotSet()
        {
            var emptyConfig = new StevedoreConfig();
            var e = Assert.Throws<HumaneException>(
                () => StevedoreProgram.GetArtifact(emptyConfig, "...", DummyArtifactId, "...")
            );
            Assert.AreEqual("Cannot download artifacts. 'cache-folder' could not be inferred from environment, nor was it configured in Stevedore.conf", e.Message);
        }

        [Test]
        [Platform(Include = "Win")]
        public void GetArtifact_CacheFolderTooLong()
        {
            var tooLongPath = @"C:\nul:\invalid\and\excessive\path\".PadRight(108, 'x');
            var config = new StevedoreConfig { CacheFolder = tooLongPath };
            var e = Assert.Throws<HumaneException>(
                () => StevedoreProgram.GetArtifact(config, "...", DummyArtifactId, "...")
            );
            Assert.AreEqual("Cannot download artifacts. 'cache-folder' exceeds 107 characters, which is too long for Windows: " + tooLongPath, e.Message);
        }

        [Test]
        public void GetArtifact_UnknownRepo()
        {
            using (var tmp = new TempDirectory())
            {
                var config = new StevedoreConfig();
                config.CacheFolder = tmp;

                var e = Assert.Throws<UnavailableRepositoryException>(
                    () => StevedoreProgram.GetArtifact(config, "nosuchrepo", DummyArtifactId, "/nosuchdir")
                );
                Assert.AreEqual("Cannot download 'dummy'. No repository named 'nosuchrepo' has been configured", e.Message);
            }
        }

        [Test]
        [Platform(Include = "Win")]
        public void GetArtifact_UnpackPathTooLong()
        {
            var tooLongPath = @"C:\nul:\invalid\and\excessive\path\".PadRight(242, 'x');
            var config = new StevedoreConfig { CacheFolder = @"C:\blah" };
            var e = Assert.Throws<HumaneException>(
                () => StevedoreProgram.GetArtifact(config, "...", DummyArtifactId, tooLongPath)
            );
            Assert.AreEqual("Cannot download artifacts. The unpack path exceeds 241 characters, which is too long for Windows: " + tooLongPath, e.Message);
        }

        [Test]
        public void NewArtifact_UnknownExtension_Fails()
        {
            var e = Assert.Throws<HumaneException>(
                () => StevedoreProgram.NewArtifact(new StevedoreConfig(), null, "blah.ext", new ArtifactName("test-artifact"), "1.0")
            );
            Assert.AreEqual("'.ext' is not a supported artifact extension; use .7z or .zip", e.Message);
        }

        [Test]
        public void NewArtifact_ByDefault_JustHashes()
        {
            var output = new StringWriter();
            using (var tmp = new TempDirectory())
            {
                var source = tmp.Combine("test.7z").WriteAllText("hello world");

                StevedoreProgram.NewArtifact(null, output, source, new ArtifactName("test-artifact"), "1.0");

                var actualOutput = output.ToString();
                Assert.AreEqual("test-artifact/1.0_b94d27b9934d3e08a52e52d7da7dabfac484efe37a5380ee9088f7ace2efcde9.7z\n", actualOutput);
            }
        }

        [Test]
        [Platform(Include = "Win")]
        public void RecursivelyDeleteDirectoryWithLongPaths_HandlesLongPaths()
        {
            var tmp = new TempDirectory(pathBudget: 182);

            var a = tmp.CreateDirectory(new string('a', 90));
            var b = tmp.CreateDirectory(new string('b', 90));
            var c = tmp.CreateDirectory(new string('c', 90));
            c.Move(b);
            b.Move(a);

            // Normal delete fails due to MAX_PATH limitations.
            Assert.Throws<DirectoryNotFoundException>(() => tmp.Delete());

            // But this works.
            StevedoreProgram.RecursivelyDeleteDirectoryWithLongPaths(tmp);

            Assert.IsFalse(tmp.Exists());
        }

        [TestCase("delete-this", "target", true)] // top-level symlink to directory
        [TestCase("delete-this/link", "../target", false)] // nested symlink to directory
        [TestCase("delete-this/link", "../target/file", false)] // nested symlink to file
        [Platform(Exclude = "Win")]
        public void RecursivelyDeleteDirectoryWithLongPaths_OnlyFollowsTopLevelSymlink(string symlink, string target, bool shouldDelete)
        {
            using (var tmp = new TempDirectory())
            {
                // Create text file:  $tmp/target/file
                var targetDir = tmp.CreateDirectory("target");
                var targetFile = targetDir.Combine("file").WriteAllText("hello");

                // Create the symlink
                var symlinkPath = tmp.Combine(symlink);
                symlinkPath.EnsureParentDirectoryExists();
                Process.Start("ln", $"-s \"{target}\" {symlinkPath.InQuotes(SlashMode.Native)}").WaitForExit();
                Assert.That(symlinkPath.Exists());

                // Verify that deletion deletes the symlinks (but not the symlink targets, unless shouldDelete is true).
                Assert.That(targetFile.Exists());
                StevedoreProgram.RecursivelyDeleteDirectoryWithLongPaths(tmp.Combine("delete-this"));
                var targetFileWasDeleted = !targetFile.Exists();
                Assert.That(targetFileWasDeleted, Is.EqualTo(shouldDelete));
                Assert.That(!symlinkPath.Exists());
            }
        }
    }
}
