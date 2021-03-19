using NUnit.Framework;

namespace AssemblyPatcher.Tests
{
    [TestFixture]
    public class BaseClassRetargetingTestCase : AssemblyPatcherTestBase
    {
        [Test]
        public void TestSameAssembly()
        {
            Test(@"TestScenarios\BaseTypeRetargeting.cs");
        }
    }
}
