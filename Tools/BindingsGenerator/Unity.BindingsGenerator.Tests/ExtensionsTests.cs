using System;
using A;
using A.B;
using NUnit.Framework;
using Unity.BindingsGenerator.Core;
using Unity.BindingsGenerator.TestFramework;

namespace A
{
    public class InsideA
    {
        public class InsideInsideA {}
    }

    namespace B
    {
        public class InsideB
        {
            public class InsideInsideB {}
        }
    }
}


namespace Unity.BindingsGenerator.Tests
{
    [TestFixture]
    class ExtensionsTests : BindingsGeneratorTestsBase
    {
        [TestCase("A", typeof(InsideA))]
        [TestCase("A", typeof(InsideA.InsideInsideA))]
        [TestCase("A.B", typeof(InsideB))]
        [TestCase("A.B", typeof(InsideB.InsideInsideB))]
        public void SafeNamespaceNestedTypes(string expectedNamespace, Type type)
        {
            Assert.AreEqual(expectedNamespace, TypeResolver.TypeDefinitionFor(type).SafeNamespace());
        }

        [TestCase("InsideA", typeof(InsideA))]
        [TestCase("InsideB", typeof(InsideB))]
        [TestCase("InsideA.InsideInsideA", typeof(InsideA.InsideInsideA))]
        [TestCase("InsideB.InsideInsideB", typeof(InsideB.InsideInsideB))]
        public void SafeNameNestedTypes(string expectedNestedName, Type type)
        {
            Assert.AreEqual(expectedNestedName, TypeResolver.TypeDefinitionFor(type).NestedName());
        }

        [TestCase("InsideA/InsideInsideA", typeof(InsideA.InsideInsideA))]
        [TestCase("InsideB/InsideInsideB", typeof(InsideB.InsideInsideB))]
        public void SafeNameNestedTypesWithSeparator(string expectedNestedName, Type type)
        {
            Assert.AreEqual(expectedNestedName, TypeResolver.TypeDefinitionFor(type).NestedName("/"));
        }

        [TestCase("A.InsideA", typeof(InsideA))]
        [TestCase("A.B.InsideB", typeof(InsideB))]
        [TestCase("A.InsideA.InsideInsideA", typeof(InsideA.InsideInsideA))]
        [TestCase("A.B.InsideB.InsideInsideB", typeof(InsideB.InsideInsideB))]
        public void SafeNamespaceAndNameNestedTypes(string expectedNamespaceAndNestedName, Type type)
        {
            Assert.AreEqual(expectedNamespaceAndNestedName, TypeResolver.TypeDefinitionFor(type).NamespaceAndNestedName());
        }

        [TestCase("A.InsideA/InsideInsideA", typeof(InsideA.InsideInsideA))]
        [TestCase("A.B.InsideB/InsideInsideB", typeof(InsideB.InsideInsideB))]
        public void SafeNamespaceAndNameNestedTypesWithSeparator(string expectedNamespaceAndNestedName, Type type)
        {
            Assert.AreEqual(expectedNamespaceAndNestedName, TypeResolver.TypeDefinitionFor(type).NamespaceAndNestedName("/"));
        }
    }
}
