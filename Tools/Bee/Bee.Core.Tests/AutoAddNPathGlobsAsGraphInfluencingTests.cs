using System;
using System.IO;
using Moq;
using NiceIO;
using NiceIO.Tests;
using NUnit.Framework;

namespace Bee.Core.Tests
{
    public class AutoAddNPathGlobsAsGraphInfluencingTests : TestWithTempDir
    {
        private Mock<IBackend> _backend;
        private NPath _subdir;

        [SetUp]
        public override void Setup()
        {
            base.Setup();
            _subdir = _tempPath.Combine("mydir/subdir").EnsureDirectoryExists();
            _backend = new Mock<IBackend>();
            Backend.Current = _backend.Object;
        }

        [Test]
        public void SimpleGlobGetsLogged()
        {
            using (new AutoAddNPathGlobsAsGraphInfluencing())
                _subdir.Files();

            _backend.Verify(b => b.RegisterGlobInfluencingGraph(_subdir));
        }

        [Test]
        public void RecursiveLogGetsAllLayersRegistered()
        {
            using (new AutoAddNPathGlobsAsGraphInfluencing())
                _tempPath.Files(true);

            _backend.Verify(b => b.RegisterGlobInfluencingGraph(_subdir.Parent.Parent));
            _backend.Verify(b => b.RegisterGlobInfluencingGraph(_subdir.Parent));
            _backend.Verify(b => b.RegisterGlobInfluencingGraph(_subdir));
        }

        [Test]
        public void NonExistingDirectory_DoesNotCrashInCallback()
        {
            using (new AutoAddNPathGlobsAsGraphInfluencing())
                Assert.Throws<DirectoryNotFoundException>(() => _tempPath.Combine("not_existing").Files(true));
        }
    }
}
