using NUnit.Framework;

namespace AssemblyPatcher.Tests
{
    [TestFixture]
    public class ConditionalPatchRuleClashTestCase : AssemblyPatcherTestBase
    {
        [Test]
        public void Test()
        {
            Test(@"TestScenarios\ConditionalPatchRuleClash.cs");
        }
    }
}
