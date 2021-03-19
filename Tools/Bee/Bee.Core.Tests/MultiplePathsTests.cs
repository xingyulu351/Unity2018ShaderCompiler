using System;
using NiceIO;
using NUnit.Framework;

namespace Bee.Core.Tests
{
    [TestFixture]
    public class MultiplePathsTests
    {
        class DummyPaths : MultiplePaths
        {
            public DummyPaths(NPath[] paths) : base(paths)
            {
            }
        }

        [Test]
        public void SomeFilesMayBeNull()
        {
            new DummyPaths(new NPath[] {"somedir1/myfile", null, "somedir1/myfile2"});
        }

        [Test]
        public void ZeroPathsThrows()
        {
            Assert.Throws<ArgumentException>(() => new DummyPaths(new NPath[] {}));
        }

        [Test]
        public void OnlyNullPathsThrow()
        {
            Assert.Throws<ArgumentException>(() => new DummyPaths(new NPath[] { null, null}));
        }

        [Test]
        public void GiveMultiplePathsNewParent()
        {
            var paths = new DummyPaths(new NPath[] {"somedir1/myfile", null, "somedir1/myfile2"});

            var withNewParent = paths.WithNewParentDirectory("newparent");

            Assert.AreEqual("newparent/myfile", withNewParent.Paths[0].ToString());
            Assert.AreEqual("newparent/myfile2", withNewParent.Paths[2].ToString());
        }

        [Test]
        public void CreateWithFilesThatHaveDifferentParents()
        {
            Assert.Throws<ArgumentException>(() => new DummyPaths(new NPath[] {"somedir1/myfile", "somedir2/myfile2"}));
        }
    }
}
