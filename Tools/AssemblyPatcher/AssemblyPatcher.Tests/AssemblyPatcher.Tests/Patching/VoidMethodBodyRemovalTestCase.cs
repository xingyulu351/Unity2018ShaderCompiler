using NUnit.Framework;

namespace AssemblyPatcher.Tests
{
    [TestFixture]
    public class VoidMethodBodyRemovalTestCase : AssemblyPatcherTestBase
    {
        [Test]
        public void Test()
        {
            Test("TestScenarios/VoidMethodBodyRemoval.cs");
        }
    }
}
