using NUnit.Framework;

namespace AssemblyPatcher.Tests
{
    [TestFixture]
    public class TargetReplacementMethodInSameAssemblyAsSource : AssemblyPatcherTestBase
    {
        [Test]
        public void Test()
        {
            Test("TestScenarios/TargetReplacementMethodInSameAssemblyAsSource.cs");
        }
    }
}
