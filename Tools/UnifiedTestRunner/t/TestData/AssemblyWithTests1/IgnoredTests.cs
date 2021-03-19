using NUnit.Framework;

namespace AssemblyWithTests1
{
    [TestFixture]
    public class IgnoredTests
    {
        [Test]
        [Ignore("Ignored")]
        public void IgnoredTest1()
        {
            Assert.IsTrue(true);
        }
    }
}
