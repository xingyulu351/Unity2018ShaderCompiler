using NUnit.Framework;

namespace AssemblyPatcher.Tests
{
    [TestFixture]
    public class FieldsTestCase : AssemblyPatcherTestBase
    {
        [Test]
        public void Test()
        {
            Test("TestScenarios/Fields.cs", "AssemblyPatcher.Tests.ExternalAssembly.dll");
        }
    }
}
