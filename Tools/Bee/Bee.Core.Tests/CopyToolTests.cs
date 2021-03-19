using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using NiceIO;
using NUnit.Framework;

namespace Bee.Core.Tests
{
    class CopyToolTests
    {
        private readonly CopyTool _instance = new CopyTool();

        [SetUp]
        public void InstallDummyBackend()
        {
            Backend.Current = new DummyBackend();
        }

        [TearDown]
        public void CleanupDummyBackend()
        {
            Backend.Current = null;
        }

        [Test]
        public void CopyingToTheSameTargetPath_FromTheSameSourcePath_Succeeds()
        {
            _instance.Setup("targetPath", "sourcePath1");
            Assert.That(() => _instance.Setup("targetPath", "sourcePath1"), Throws.Nothing);
        }

        [Test]
        public void CopyingToTheSameTargetPath_FromDifferentSourcePaths_ThrowsArgumentException()
        {
            _instance.Setup("targetPath", "sourcePath1");
            Assert.That(() => _instance.Setup("targetPath", "sourcePath2"),
                Throws.ArgumentException.With.Message.StartsWith(
                    "You're trying to copy to targetPath more than once."));
        }

        [Test]
        public void CopyingToTheSameTargetPath_FromTransitivelyTheSameSourcePath_Succeeds()
        {
            _instance.Setup("intermediatePath", "sourcePath");
            _instance.Setup("targetPath", "sourcePath");
            Assert.That(() => _instance.Setup("targetPath", "intermediatePath"), Throws.Nothing);
        }

        [Test]
        public void CopyingToTheSameTargetPath_FromTwoTransitivelySameSourcePaths_Succeeds()
        {
            _instance.Setup("intermediatePath1", "sourcePath");
            _instance.Setup("intermediatePath2", "sourcePath");

            _instance.Setup("targetPath", "intermediatePath1");
            Assert.That(() => _instance.Setup("targetPath", "intermediatePath2"), Throws.Nothing);
        }
    }
}
