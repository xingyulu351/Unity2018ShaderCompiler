using System;
using System.Collections.Generic;
using Moq;
using NUnit.Framework;
using Unity.SerializationWeaver.Serialization.Tests.Fixtures;
using UnityEngine;
using UnityEngine.Serialization;

namespace Unity.SerializationWeaver.Serialization.Tests
{
    [TestFixture]
    public class TestSerialization : SerializationWeaverTestBase
    {
        [Test]
        public void TestWithVolatileField()
        {
            const int fieldValue = 33;
            RunSerializeTest<WithVolatileField>(
                () => new Dictionary<string, object>
                {
                    { "Field", fieldValue }
                },
                (mock, o) =>
                    mock.Setup(_ => _.WriteInt32(fieldValue)));
        }

        [Test]
        public void TestWithSingleField()
        {
            const int fieldValue = 33;

            RunSerializeTest<WithSingleField>(
                () => new Dictionary<string, object>
                {
                    { "Field", fieldValue }
                },
                (mock, o) =>
                    mock.Setup(_ => _.WriteInt32(fieldValue)));
        }

        [Test]
        public void TestWithGenericBaseClass()
        {
            const float fieldValue = 33.0f;

            RunSerializeTest<WithGenericBaseClass>(
                () => new Dictionary<string, object>
                {
                    { "Field", fieldValue },
                    { "GenericField", fieldValue }
                },
                (mock, o) =>
                {
                    mock.Setup(_ => _.WriteSingle(fieldValue));
                    mock.Setup(_ => _.WriteSingle(fieldValue));
                });
        }

        [Test]
        public void TestWithStructContainingBools()
        {
            RunSerializeTest<Struct>(
                () => new Dictionary<string, object>
                {
                    { "intField1", 123 },
                    { "boolField1", true },
                    { "intField2", 456 },
                    { "boolField2", false },
                    { "intField3", 789 }
                },
                (mock, o) =>
                {
                    mock.Setup(_ => _.WriteInt32(123));
                    mock.Setup(_ => _.WriteBoolean(true));
                    mock.Setup(_ => _.Align());
                    mock.Setup(_ => _.WriteInt32(456));
                    mock.Setup(_ => _.WriteBoolean(false));
                    mock.Setup(_ => _.Align());
                    mock.Setup(_ => _.WriteInt32(789));
                }
            );
        }

        private static void RunSerializeTest<T>(Func<Dictionary<string, object>> getInitialFieldValues, Action<Mock<ISerializedStateWriter>, object> setupMock)
        {
            RunTest<T, ISerializedStateWriter, SerializedStateWriter>(
                _ => Unity.Serialization.Weaver.SerializationWeaver.Weave(_, UnityEngineDLLAssembly()),
                _ => _.GetMethod("Unity_Serialize"),
                getInitialFieldValues,
                setupMock);
        }
    }
}
