using NUnit.Framework;
using System.IO;

namespace InternalCallReplacer.Tests
{
    [TestFixture]
    public class IntegrationTests : IntegrationTestsBase
    {
        [Test]
        public void CanProcessStruct()
        {
            RunIntegrationTest("CanProcessStruct");
        }

        [Test]
        public void CanProcessGenericClasses()
        {
            RunIntegrationTest("CanProcessGenericClasses");
        }

        [Test]
        public void CanProcessSerializationCallbacks()
        {
            RunIntegrationTest("CanProcessSerializationCallbacks");
        }

        [Test]
        public void CanProcessSerializationCallbacksInStruct()
        {
            RunIntegrationTest("CanProcessSerializationCallbacksInStruct");
        }

        [Test]
        public void CanProcessGenericList()
        {
            RunIntegrationTest("CanProcessGenericList");
        }

        [Test]
        public void CanProcessColor32Struct()
        {
            RunIntegrationTest("CanProcessColor32Struct");
        }

        [Test]
        public void CanProcessMatrix4x4Struct()
        {
            RunIntegrationTest("CanProcessMatrix4x4Struct");
        }

        [Test]
        public void CanProcessPrivateOrInternalField()
        {
            RunIntegrationTest("CanProcessPrivateOrInternalField");
        }

        [Test]
        public void WontProcessGenericDictionary()
        {
            RunIntegrationTest("WontProcessGenericDictionary");
        }

        [Test]
        public void ArraysToObjectsSerializedProperly()
        {
            RunIntegrationTest("ArraysToObjectsSerializedProperly");
        }

        [Test]
        public void CanRunUNetWeaver()
        {
            RunIntegrationTest("CanRunUNetWeaver", true);
        }

        [Test]
        public void CanProcessListOfStructs()
        {
            RunIntegrationTest("CanProcessListOfStructs");
        }

        [Test]
        public void CanProcessFixedBuffer()
        {
            RunIntegrationTest("CanProcessFixedBuffer");
        }
    }
}
