using System;
using System.Linq;
using System.Runtime.CompilerServices;
using Mono.Cecil;
using NUnit.Framework;
using Unity.BindingsGenerator.Core;
using UnityEngine.Bindings;
using Unity.BindingsGenerator.Core.Nodes;
using Unity.BindingsGenerator.TestFramework;

namespace Unity.BindingsGenerator.Tests
{
    [TestFixture]
    internal class EnumTests : BindingsGeneratorTestsBase
    {
        private enum SimpleEnum
        {
            Value1,
            Value2,
            Value3,
        }

        [Test]
        public void SimpleEnumWorks()
        {
            var expectedStruct = new StructDefinitionNode
            {
                Name = Naming.IntermediateTypeName(nameof(SimpleEnum)),
                ForceUniqueViaPreprocessor = true,
                IsTemplate = false,
                Members = { new FieldDeclarationStatementNode("value", "SInt32") }
            };
            var result = EnsureGenerated<SimpleEnum>(TreeGenerator.GenerateReason.IsArgumentType);
            Assert.AreEqual(1, result.HeaderFile.DefaultNamespace.Structs.Count);

            var actualStruct = result.HeaderFile.DefaultNamespace.Structs[0];

            AssertEqual(expectedStruct, actualStruct);
            AssertMarshalInfoExists(Naming.IntermediateTypeName(nameof(SimpleEnum)), result.SourceFile.Namespaces.First(n => n.Name == Naming.MarshallingNamespace), false, true);
        }

        private enum EnumWithValueOverride
        {
            Value1 = 10,
            Value2 = 20,
            Value3 = 30,
        }

        [Test]
        public void SimpleEnumValueOverrideWorks()
        {
            var result = EnsureGenerated<EnumWithValueOverride>(TreeGenerator.GenerateReason.IsArgumentType);
            Assert.AreEqual(1, result.HeaderFile.DefaultNamespace.Structs.Count);
        }

        [NativeType(Header = "Foo.h")]
        private enum EnumWithHeader
        {
            Value1,
        }

        [Test]
        public void EnumWithHeaderWorks()
        {
            var result = EnsureGenerated<EnumWithHeader>(TreeGenerator.GenerateReason.IsArgumentType);
            Assert.AreEqual(1, result.HeaderFile.IncludeFiles.Count);
            Assert.AreEqual("Foo.h", result.HeaderFile.IncludeFiles[0]);
        }

        [Test]
        public void EnsureEnumIsNotGeneratedMultipleTimes()
        {
            var typeDefinition = TypeResolver.TypeDefinitionFor(typeof(EnumWithHeader));

            var result = new TreeGenerator();
            result.Initialize();
            result.EnsureGenerated(typeDefinition, TreeGenerator.GenerateReason.IsArgumentType);
            result.EnsureGenerated(typeDefinition, TreeGenerator.GenerateReason.IsArgumentType);

            Assert.AreEqual(1, result.HeaderFile.DefaultNamespace.Structs.Count);
        }

        [NativeHeader("UnityConnectScriptingClasses.h")]
        private static class SimpleEnumUser
        {
            private static extern SimpleEnum simpleEnum
            {
                [MethodImpl(MethodImplOptions.InternalCall)]
                [NativeMethod("GetPlayerSessionState")]
                get;
            }
        }

        [Test]
        public void PropertyReturnGeneratesEnum()
        {
            var result = GeneratedTreeFor(typeof(SimpleEnumUser));
            Assert.AreEqual(1, result.HeaderFile.DefaultNamespace.Structs.Count);

            var e = result.HeaderFile.DefaultNamespace.Structs[0];
            e.Name = Naming.IntermediateTypeName(nameof(SimpleEnum));
        }

        private enum ByteEnum : byte
        {
            Value1,
        }

        [Test]
        public void ByteEnumGeneratesProperStructField()
        {
            var expectedStruct = new StructDefinitionNode
            {
                Name = Naming.IntermediateTypeName(nameof(ByteEnum)),
                ForceUniqueViaPreprocessor = true,
                IsTemplate = false,
                Members = { new FieldDeclarationStatementNode("value", "UInt8")}
            };
            var result = EnsureGenerated<ByteEnum>(TreeGenerator.GenerateReason.IsArgumentType);
            var actualStruct = result.HeaderFile.DefaultNamespace.Structs.First(s =>
                s.Name == Naming.IntermediateTypeName(nameof(ByteEnum)));

            AssertEqual(expectedStruct, actualStruct);
        }

        [Test]
        [TestCase(typeof(UnityEngine.MonoBehaviour))]
        [TestCase(typeof(UnityEngine.ScriptableObject))]
        public void EnsureMarshalInfoNotGeneratedForMonoBehaviourTypes(Type type)
        {
            var typeDefinition = TypeResolver.TypeDefinitionFor(type);

            var result = new TreeGenerator();
            result.Initialize();
            result.EnsureGenerated(typeDefinition, TreeGenerator.GenerateReason.IsArgumentType);

            Assert.AreEqual(0, result.MarshallingNamespace.Structs.Count);
        }
    }
}
