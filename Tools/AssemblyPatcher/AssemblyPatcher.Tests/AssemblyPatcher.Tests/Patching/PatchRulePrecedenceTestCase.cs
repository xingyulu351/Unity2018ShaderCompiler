using NUnit.Framework;

namespace AssemblyPatcher.Tests
{
    [TestFixture]
    public class PatchRulePrecedenceTestCase : AssemblyPatcherTestBase
    {
        [Test]
        public void Test()
        {
            Test(@"TestScenarios\PatchRulePrecedence.cs");
        }

        [Test]
        public void TestPrecedenceWithConditional()
        {
            Test(@"TestScenarios\ConditionalAndPatchRulePrecedence.cs");
        }
    }
}
