using NUnit.Framework;

namespace Unity.BuildSystem.Tests
{
    [TestFixture]
    public class SubIncludeRunnerTests
    {
        [Test]
        public void RunsOnlyOnce()
        {
            var run = new SubIncludeRunner();
            run.RunOnce(A);
            Assert.AreEqual(6, counter);
            run.RunOnce(A);
            Assert.AreEqual(6, counter);
        }

        static int counter = 5;
        static void A()
        {
            counter++;
        }
    }
}
