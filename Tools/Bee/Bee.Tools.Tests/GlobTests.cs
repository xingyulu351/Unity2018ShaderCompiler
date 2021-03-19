using System.Collections.Generic;
using System.IO;
using System.Linq;
using Bee.TestHelpers;
using JamSharp.Runtime;
using NiceIO;
using NUnit.Framework;
using Unity.BuildTools;

namespace Bee.Tools.Tests
{
    [TestFixture]
    public class GlobTests
    {
        NPath _tempDir;

        static readonly List<string> Files = new List<string>
        {
            "a.txt",
            "a.txd",
            "b.txt",
            "a.txttxt",
            "a.txt.txt",
            ".a.txt",
            "aaa.txt",
            "abcdef.txt",
            ".txt",
            "a.t",
            "Ab.txt",
            "a",
            "ABC.txt",
            "a.cpp",
            "a.cppx",
            "a.cpp.cpp",
        };

        static readonly string[] Folders =
        {
            "dir/",
            "dir/dir/",
            "dir/subdir/",
            "dir/subdir/subdir/",
            "dar/",
            "dur/",
            ".dir/",
            ".dir/.subdir/",
        };

        static readonly string[] FilesInFolders =
        {
            "dir/a.txt",
            "dir/aaa.txt",
            "dir/subdir/a.txt",
            "dir/subdir/a.cpp",
            "dir/subdir/subdir/a.txt",
            ".dir/a.txt",
            ".dir/.subdir/abcdef.txt",
        };


        [OneTimeSetUp]
        public void Setup()
        {
            _tempDir = new TempDirectory("GlobTests");
            foreach (var d in Folders)
                Directory.CreateDirectory($"{_tempDir}/{d}");
            foreach (var f in Files.Concat(FilesInFolders))
                File.WriteAllText($"{_tempDir}/{f}", "yes");

            string cmdLine = string.Format(HostPlatform.IsWindows ? "cmd /C mklink {1} {0}" : "ln -s {0} {1}",
                _tempDir.Combine("dir/subdir/a.txt").InQuotes(),
                _tempDir.Combine("a-symlink.txt").InQuotes());
            Shell.Execute(cmdLine).ThrowOnFailure();
            Files.Add("a-symlink.txt");
        }

        [OneTimeTearDown]
        public void TearDown()
        {
            _tempDir.Delete();
        }

        IEnumerable<string> Search(string pattern)
        {
            return Glob.Search(pattern).Select(p => p.Substring(_tempDir.ToString().Length + 1));
        }

        [Test]
        public void SearchExact()
        {
            var res = Search($"{_tempDir}/a.txt");
            CollectionAssert.AreEquivalent(new[] {"a.txt"}, res);
        }

        [Test]
        public void SearchExactNonExisting()
        {
            var res = Search($"{_tempDir}/z.txt");
            CollectionAssert.AreEquivalent(new string[] {}, res);
        }

        [Test]
        public void SearchExactDifferentCase()
        {
            var res = Search($"{_tempDir}/A.txt");
            if (!HostPlatform.IsLinux)
            {
                // a.txt exists; we search for A.txt, should find it, with A casing
                CollectionAssert.AreEquivalent(new[] {"A.txt"}, res);
            }
            else
            {
                CollectionAssert.AreEquivalent(new string[] {}, res);
            }
        }

        [Test]
        public void SearchExactDir()
        {
            var res = Search($"{_tempDir}/dir/");
            CollectionAssert.AreEquivalent(new[] {"dir/"}, res);
        }

        [Test]
        public void SearchStar()
        {
            var res = Search($"{_tempDir}/*");
            CollectionAssert.AreEquivalent(Files, res);
        }

        [Test]
        public void SearchStarDir()
        {
            var res = Search($"{_tempDir}/*/");
            CollectionAssert.AreEquivalent(new[] {".dir/", "dir/", "dar/", "dur/"}, res);
        }

        [Test]
        public void SearchStarDirStar()
        {
            var res = Search($"{_tempDir}/*/*");
            CollectionAssert.AreEquivalent(new[] {".dir/a.txt", "dir/a.txt", "dir/aaa.txt"}, res);
        }

        [Test]
        public void SearchStarFilename()
        {
            var res = Search($"{_tempDir}/*.txt");
            CollectionAssert.AreEquivalent(new[] {".a.txt", ".txt", "a.txt", "a.txt.txt", "aaa.txt", "a-symlink.txt", "Ab.txt", "ABC.txt", "abcdef.txt", "b.txt"}, res);
        }

        [Test]
        public void SearchStarFilename2()
        {
            var res = Search($"{_tempDir}/*.cpp");
            CollectionAssert.AreEquivalent(new[] {"a.cpp", "a.cpp.cpp"}, res);
        }

        [Test]
        public void SearchStarExtension()
        {
            var res = Search($"{_tempDir}/a.*");
            CollectionAssert.AreEquivalent(new[] {"a.cpp", "a.cpp.cpp", "a.cppx", "a.t", "a.txd", "a.txt", "a.txt.txt", "a.txttxt"}, res);
        }

        [Test]
        public void SearchStarFilenameAndExtension()
        {
            var res = Search($"{_tempDir}/*.*");
            Assert.AreEqual(16, res.Count()); // will find files directly in the folder, except for "a"
        }

        [Test]
        public void SearchQuestion()
        {
            var res = Search($"{_tempDir}/?.txt");
            CollectionAssert.AreEquivalent(new[] {"a.txt", "b.txt"}, res);
        }

        [Test]
        public void SearchStarStar()
        {
            var res = Search($"{_tempDir}/**");
            CollectionAssert.AreEquivalent(Files.Concat(FilesInFolders), res);
        }

        [Test]
        public void SearchStarStarDir()
        {
            var res = Search($"{_tempDir}/**/");
            CollectionAssert.AreEquivalent(Folders, res);
        }

        [Test]
        public void SearchQuestionDir()
        {
            var res = Search($"{_tempDir}/???/");
            CollectionAssert.AreEquivalent(new[] {"dir/", "dar/", "dur/"}, res);
        }

        [Test]
        public void SearchStarStarComponentThenFilename()
        {
            var res = Search($"{_tempDir}/**/a.txt");
            CollectionAssert.AreEquivalent(new[] {"a.txt", ".dir/a.txt", "dir/a.txt", "dir/subdir/a.txt", "dir/subdir/subdir/a.txt"}, res);
        }

        [Test]
        public void SearchStarStarStartOfFilename()
        {
            var res = Search($"{_tempDir}/**.cpp");
            CollectionAssert.AreEquivalent(new[] {"a.cpp", "a.cpp.cpp", "dir/subdir/a.cpp"}, res);
        }

        [Test]
        public void SearchRecursiveByExtension()
        {
            var res = Search($"{_tempDir}/**/*.cpp");
            // note: .cppx file is returned too, because of pecularity of Directory.GetFiles vs longer-than-3 extensions on windows
            var exp =
                HostPlatform.IsWindows ?
                new[] {"a.cpp", "a.cpp.cpp", "dir/subdir/a.cpp", "a.cppx"} :
            new[] {"a.cpp", "a.cpp.cpp", "dir/subdir/a.cpp"};
            CollectionAssert.AreEquivalent(exp, res);
        }

        [Test]
        public void SearchByExtensionsMatchesNPath()
        {
            var resGlob = Search($"{_tempDir}/**/*.cpp").Concat(Search($"{_tempDir}/**/*.txt"));
            var resNpath = _tempDir.Files(new[] {"cpp", "txt"}, true).Select(p => p.RelativeTo(_tempDir).ToString());
            // files found via globbing will also return "cppx" etc when searching for "cpp" extensions, due to how
            // Directory.GetFiles works on Windows
            if (HostPlatform.IsWindows)
                resNpath = resNpath.Append("a.cppx").Append("a.txttxt");
            CollectionAssert.AreEquivalent(resNpath, resGlob);
        }

        [Test]
        public void SearchIncludesSymlinks()
        {
            Assert.That(Search($"{_tempDir}/*.txt"), Does.Contain("a-symlink.txt"));
        }
    }
}
