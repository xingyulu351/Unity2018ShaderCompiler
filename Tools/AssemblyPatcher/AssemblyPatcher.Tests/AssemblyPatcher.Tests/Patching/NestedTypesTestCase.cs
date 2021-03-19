using NUnit.Framework;

namespace AssemblyPatcher.Tests
{
    [TestFixture]
    public class NestedTypesTestCase : AssemblyPatcherTestBase
    {
        [Test]
        public void TestNestedTypes()
        {
            Test(@"TestScenarios\NestedTypes.cs");
        }
    }
}
