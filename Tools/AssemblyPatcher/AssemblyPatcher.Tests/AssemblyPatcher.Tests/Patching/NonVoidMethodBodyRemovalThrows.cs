using NUnit.Framework;

namespace AssemblyPatcher.Tests
{
    [TestFixture]
    public class NonVoidMethodBodyRemovalThrowsTestCase : AssemblyPatcherTestBase
    {
        [Test]
        public void Test()
        {
            AssertPatcherFailure(
                "TestScenarios/NonVoidMethodBodyRemovalThrows.cs",
                "Patch rule 'System.String NonVoidMethodBodyRemovalThrows::Method() -> {}' cannot be applied to non void return type method 'System.String NonVoidMethodBodyRemovalThrows::Method()'");
        }
    }
}
