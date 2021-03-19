using NUnit.Framework;

namespace AssemblyPatcher.Tests
{
    [TestFixture]
    public class CatchStatementTypeReferencesTestCase : AssemblyPatcherTestBase
    {
        [Test]
        public void Test()
        {
            Test("TestScenarios/CatchStatementTypeReferences.cs");
        }
    }
}
