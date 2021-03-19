using System.Linq;
using NUnit.Framework;
using Unity.BindingsGenerator.Core;
using Unity.BindingsGenerator.TestFramework;
using UnityEngine.Bindings;

#pragma warning disable CS0649, CS0169

namespace Unity.BindingsGenerator.Tests
{
    class NativeHeaderTests : BindingsGeneratorTestsBase
    {
        [NativeHeader("BaseClassWithNativeHeader.h")]
        class BaseClassWithNativeHeader
        {
        }

#pragma warning disable 0649
        [NativeHeader("ClassWithNativeHeader.h")]
        class ClassWithNativeHeader : BaseClassWithNativeHeader
        {
#pragma warning disable 649
            [NativeHeader("FieldHeader.h")]
            internal BaseClassWithNativeHeader field;

            [NativeHeader("PropertyHeader.h")]
            internal ClassWithNativeHeader property {[NativeHeader("GetterHeader.h")] get; [NativeHeader("SetterHeader.h")] set; }

            [NativeHeader("MethodHeader.h")]
            [return : NativeHeader("ReturnHeader.h")]
            internal static ClassWithNativeHeader MethodReturn()
            {
                return default(ClassWithNativeHeader);
            }

            [NativeHeader("MethodHeader.h")]
            internal static void MethodParams(ClassWithNativeHeader param1, [NativeHeader("ParamHeader.h")] int param2)
            {
            }
        }

        [NativeHeader("ClassWithMultipleNativeHeader1.h")]
        [NativeHeader("ClassWithMultipleNativeHeader2.h")]
        class ClassWithMultipleNativeHeader
        {
        }

        [NativeHeader]
        class ClassWithNullNativeHeader
        {
        }

        [NativeHeader("ClassWithRecursiveInheritance.h")]
        class ClassWithRecursiveInheritance : ClassWithNativeHeader
        {
        }

        [Test]
        public void IncludesProviderForTypeDefinitionWorks()
        {
            var includes = IncludesProvider.IncludesFor(TypeResolver.TypeDefinitionFor<ClassWithNativeHeader>());

            CollectionAssert.AreEqual(
                new[]
                {
                    "BaseClassWithNativeHeader.h",
                    "ClassWithNativeHeader.h"
                }, includes);
        }

        [Test]
        public void IncludesProviderForFieldDefinitionWorks()
        {
            var includes = IncludesProvider.IncludesFor(
                TypeResolver.TypeDefinitionFor<ClassWithNativeHeader>().Fields.Single(f => f.Name == nameof(ClassWithNativeHeader.field)));

            CollectionAssert.AreEqual(
                new[]
                {
                    "BaseClassWithNativeHeader.h",
                    "FieldHeader.h"
                }, includes);
        }

        [Test]
        public void IncludesProviderForMethodDefinitionWorks()
        {
            var includes = IncludesProvider.IncludesFor(
                TypeResolver.TypeDefinitionFor<ClassWithNativeHeader>().Methods.Single(m => m.Name == nameof(ClassWithNativeHeader.MethodParams)));

            CollectionAssert.AreEqual(
                new[]
                {
                    "BaseClassWithNativeHeader.h",
                    "ClassWithNativeHeader.h",
                    "ParamHeader.h",
                    "MethodHeader.h"
                }, includes);
        }

        [Test]
        public void IncludesProviderForMethodDefinitionWithReturnTypeWorks()
        {
            var includes = IncludesProvider.IncludesFor(
                TypeResolver.TypeDefinitionFor<ClassWithNativeHeader>().Methods.Single(m => m.Name == nameof(ClassWithNativeHeader.MethodReturn)));

            CollectionAssert.AreEqual(
                new[]
                {
                    "BaseClassWithNativeHeader.h",
                    "ClassWithNativeHeader.h",
                    "ReturnHeader.h",
                    "MethodHeader.h"
                }, includes);
        }

        [Test]
        public void IncludesProviderForPropertyDefinitionWithReturnTypeWorks()
        {
            var includes = IncludesProvider.IncludesFor(
                TypeResolver.TypeDefinitionFor<ClassWithNativeHeader>().Properties.Single(p => p.Name == nameof(ClassWithNativeHeader.property)));

            CollectionAssert.AreEqual(
                new[]
                {
                    "BaseClassWithNativeHeader.h",
                    "ClassWithNativeHeader.h",
                    "GetterHeader.h",
                    "SetterHeader.h",
                    "PropertyHeader.h"
                }, includes);
        }

        [Test]
        public void IncludesProviderForClassWithMultipleHeadersWorks()
        {
            var includes = IncludesProvider.IncludesFor(
                TypeResolver.TypeDefinitionFor<ClassWithMultipleNativeHeader>());

            CollectionAssert.AreEqual(
                new[]
                {
                    "ClassWithMultipleNativeHeader1.h",
                    "ClassWithMultipleNativeHeader2.h"
                }, includes);
        }

        [Test]
        public void IncludesProviderForClassWithNullHeaderWorks()
        {
            var includes = IncludesProvider.IncludesFor(
                TypeResolver.TypeDefinitionFor<ClassWithNullNativeHeader>());

            CollectionAssert.AreEqual(new string[0], includes);
        }

        [Test]
        public void IncludesProviderForClassWithRecursiveInheranceWorks()
        {
            var includes = IncludesProvider.IncludesFor(
                TypeResolver.TypeDefinitionFor<ClassWithRecursiveInheritance>());

            CollectionAssert.AreEqual(
                new[]
                {
                    "BaseClassWithNativeHeader.h",
                    "ClassWithNativeHeader.h",
                    "ClassWithRecursiveInheritance.h"
                }, includes);
        }
    }
}
