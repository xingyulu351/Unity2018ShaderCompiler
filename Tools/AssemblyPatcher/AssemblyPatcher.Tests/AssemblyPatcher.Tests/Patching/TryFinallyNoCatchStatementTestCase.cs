using NUnit.Framework;

namespace AssemblyPatcher.Tests
{
    [TestFixture]
    public class TryFinallyNoCatchStatement : AssemblyPatcherTestBase
    {
        [Test]
        public void Test()
        {
            Test("TestScenarios/TryFinallyNoCatchStatement.cs");
        }
    }
}
