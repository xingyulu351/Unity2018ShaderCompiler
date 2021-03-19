using System;
using NUnit.Framework;

namespace AssemblyPatcher.Tests
{
    [TestFixture]
    public class RemovingMethodCallsAtTryCatchFinallyBoundaries : AssemblyPatcherTestBase
    {
        [Test]
        public void Test()
        {
            try
            {
                Test("TestScenarios/RemovingMethodCallsAtTryCatchFinallyBoundaries.cs");
                Assert.Fail();
            }
            catch (MissingMemberException mme)
            {
                Assert.IsTrue(mme.Message.Contains("System.Void A::Bar()"));
            }
        }
    }
}
