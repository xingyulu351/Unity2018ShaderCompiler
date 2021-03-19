using System;
using Moq;
using NiceIO;
using NiceIO.Tests;
using NUnit.Framework;

namespace Bee.Core.Tests
{
    public class NoGlobsInDynamicallyUnpackedDirectoriesVerifierTests : TestWithTempDir
    {
        private Mock<IBackend> _backend;
        private NPath _subdir;

        [SetUp]
        public override void Setup()
        {
            base.Setup();
            _backend = new Mock<IBackend>();
            Backend.Current = _backend.Object;
            _subdir = _tempPath.Combine("mydir/subdir").EnsureDirectoryExists();
        }

        [Test]
        public void GlobbingAnAlreadyRegisteredDirectoryWithImplicitInputsThrows()
        {
            _backend.Setup(b => b.AllDirectoriesProducingImplicitInputs).Returns(new[] {_tempPath });
            using (new NoUnstableAccessInDynamicallyUnpackedDirectoriesVerifier())
                Assert.Throws<InvalidOperationException>(() => _tempPath.Files());
        }

        [Test]
        public void GlobbingAnAlreadyRegisteredDirectoryByAbsolutePathThrows()
        {
            _backend.Setup(b => b.AllDirectoriesProducingImplicitInputs).Returns(new[] {_tempPath.RelativeTo(NPath.CurrentDirectory)});
            using (new NoUnstableAccessInDynamicallyUnpackedDirectoriesVerifier())
                Assert.Throws<InvalidOperationException>(() => _tempPath.MakeAbsolute().Files());
        }

        [Test]
        public void GlobbingASubdirectoryOfAnAlreadyRegisteredDirectoryWithImplicitInputsThrows()
        {
            _backend.Setup(b => b.AllDirectoriesProducingImplicitInputs).Returns(new[] { _tempPath.Combine("mydir") });
            using (new NoUnstableAccessInDynamicallyUnpackedDirectoriesVerifier())
                Assert.Throws<InvalidOperationException>(() => _subdir.Files());
        }

        [Test]
        public void RecursiveGlobOfParentDirectoryOfOffLimitsDirectory()
        {
            _backend.Setup(b => b.AllDirectoriesProducingImplicitInputs).Returns(new[] {_subdir});

            using (new NoUnstableAccessInDynamicallyUnpackedDirectoriesVerifier())
                Assert.Throws<InvalidOperationException>(() => _subdir.Parent.Files(true));
        }

        [Test]
        public void RegisteringImplicitDirectory_AfterItHasBeenGlobbed_Throws()
        {
            Assert.Throws<InvalidOperationException>(() =>
            {
                using (new NoUnstableAccessInDynamicallyUnpackedDirectoriesVerifier())
                {
                    _subdir.Files();
                    _backend.Setup(b => b.AllDirectoriesProducingImplicitInputs)
                        .Returns(new[] {_tempPath.Combine("mydir")});
                }
            });
        }

        [Test]
        public void StattingAFileInAnImplicitDirectoryThrows()
        {
            _backend.Setup(b => b.AllDirectoriesProducingImplicitInputs).Returns(new[] { _tempPath });
            using (new NoUnstableAccessInDynamicallyUnpackedDirectoriesVerifier())
                Assert.Throws<InvalidOperationException>(() => _tempPath.Combine("somefile").FileExists());
        }

        [Test]
        public void StattingAnImplicitDirectoryThrows()
        {
            _backend.Setup(b => b.AllDirectoriesProducingImplicitInputs).Returns(new[] { _tempPath });
            using (new NoUnstableAccessInDynamicallyUnpackedDirectoriesVerifier())
                Assert.Throws<InvalidOperationException>(() => _tempPath.DirectoryExists());
        }

        [Test]
        public void ReadingContentsOfAFileInAnImplicitDirectoryThrows()
        {
            _backend.Setup(b => b.AllDirectoriesProducingImplicitInputs).Returns(new[] { _tempPath });
            using (new NoUnstableAccessInDynamicallyUnpackedDirectoriesVerifier())
                Assert.Throws<InvalidOperationException>(() => _tempPath.Combine("somefile").ReadAllText());
        }
    }
}
