using System;
using NUnit.Framework;

namespace AssemblyPatcher.Tests
{
    [TestFixture]
    public class OutParameterTestCase : AssemblyPatcherTestBase
    {
        [Test]
        public void Test()
        {
            Assert.Throws<MissingMemberException>(() => Test("TestScenarios/OutParameter.cs"));
        }
    }
}
