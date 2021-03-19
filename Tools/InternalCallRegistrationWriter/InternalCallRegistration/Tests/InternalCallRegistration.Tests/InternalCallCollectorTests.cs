using InternalCallRegistration.Tests.Fixtures.UnityFaux;
using NUnit.Framework;
using Unity.InternalCallRegistrationWriter.Collector;

namespace Unity.InternalCallRegistration.Tests
{
    [TestFixture]
    public class InternalCallCollectorTests
    {
        private static InternalCallCollector CreateCollectorForTestFixtureAssembly()
        {
            return new InternalCallCollector(UnityFauxFixtureLocation.Location());
        }

        [Test]
        public void CanGetAssemblyDefinitionForTestFixture()
        {
            Assert.IsNotNull(CreateCollectorForTestFixtureAssembly().AssemblyDefinition);
        }

        [Test]
        public void CanFindInternalCallsInTestFixture()
        {
            var internalCallCollector = CreateCollectorForTestFixtureAssembly();

            Assert.IsNotNull(internalCallCollector.AllCollectedCalls);
            Assert.IsNotEmpty(internalCallCollector.AllCollectedCalls);
        }

        [Test]
        public void CanFindInternalCallsInSingleType()
        {
            var collector = CreateCollectorForTestFixtureAssembly();
            var icallsForSimpleUnityBindingsType = collector.CollectedCallsFor("UnityEngine.SimpleUnityBindingsType");
            Assert.IsNotNull(icallsForSimpleUnityBindingsType);
            Assert.IsNotEmpty(icallsForSimpleUnityBindingsType);
        }
    }
}
