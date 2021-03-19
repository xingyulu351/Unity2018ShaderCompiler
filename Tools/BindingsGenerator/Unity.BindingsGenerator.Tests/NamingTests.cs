using System.Linq;
using Mono.Cecil;
using NUnit.Framework;
using Unity.BindingsGenerator.Core;
using Unity.BindingsGenerator.TestFramework;
using UnityEngine.Bindings;

#pragma warning disable CS0649, CS0169

namespace Unity.BindingsGenerator.Tests
{
    [TestFixture]
    public class NamingTests
    {
        AssemblyDefinition _assembly;

        [SetUp]
        public void Setup()
        {
            _assembly = TypeResolver.LoadAssembly(typeof(ClassOriginalName).Assembly.Location);
        }

        [Test]
        public void TestAddressOf()
        {
            Assert.AreEqual("&foo", Naming.AddressOf("foo"));
        }

        [Test]
        public void TestMarshalled()
        {
            Assert.AreEqual("foo_marshalled", "foo".Marshalled());
        }

        [Test]
        public void TestReference()
        {
            Assert.AreEqual("foo&", Naming.Reference("foo"));
        }

        [Test]
        public void TestConstReference()
        {
            Assert.AreEqual("const foo&", Naming.ConstReference("foo"));
        }

        [Test]
        public void TestMarshallingStruct()
        {
            Assert.AreEqual("foo__", Naming.IntermediateTypeName("foo"));
        }

        [Test]
        public void TestPointer()
        {
            Assert.AreEqual("foo*", "foo".Pointer());
        }

        [Test]
        public void TestByRefInjectedMethod()
        {
            Assert.AreEqual("SomeMethod_Injected", Naming.ByRefInjectedMethod("SomeMethod"));
        }

        [Test]
        public void TestWithTemplateArgument()
        {
            Assert.AreEqual("A<B>", "A".WithTemplateArgument("B"));
        }

        [Test]
        public void TestWithDoubleTemplateArgument()
        {
            Assert.AreEqual("A<B<C> >", "A".WithTemplateArgument("B".WithTemplateArgument("C")));
        }

        [Test]
        public void CppNameForReturnsProperString()
        {
            Assert.AreEqual("", Naming.CppNameFor(new TypeDefinition("", "", TypeAttributes.Class)));
            Assert.AreEqual("hello", Naming.CppNameFor(new TypeDefinition("", "HELLO", TypeAttributes.Class)));
            Assert.AreEqual("aoEnabled", Naming.CppNameFor(new TypeDefinition("", "AOEnabled", TypeAttributes.Class)));
        }

        [Test]
        public void TestNativeNamespaceRemoval()
        {
            Assert.AreEqual("ClassName", "ABC::ClassName".WithoutNativeNamespaces());
            Assert.AreEqual("Q", "ABC::DEF::Q".WithoutNativeNamespaces());
        }

        enum EnumOriginalName
        {
            [NativeName("CustomFieldName")]
            FieldOriginalName
        }

#pragma warning disable 0649
        class ClassOriginalName
        {
#pragma warning disable 649
            [NativeName("CustomFieldName")]
            public int FieldOriginalName;

            [NativeName("CustomMethodName")]
            public void MethodOriginalName() {}

            [NativeName("CustomPropertyName")]
            public int PropertyOriginalName { get; set; }


            public int Property2OriginalName
            {
                [NativeName("CustomGetterName")]
                get;
                [NativeName("CustomSetterName")]
                set;
            }
        }

        [Test]
        public void NativeNameWorks()
        {
            var klass = _assembly.MainModule.GetType("Unity.BindingsGenerator.Tests.NamingTests/ClassOriginalName");
            var enumz = _assembly.MainModule.GetType("Unity.BindingsGenerator.Tests.NamingTests/EnumOriginalName");

            Assert.AreEqual("CustomFieldName", TypeUtils.NativeNameFor(klass.Fields.Single(f => f.Name == nameof(ClassOriginalName.FieldOriginalName))));
            Assert.AreEqual("CustomFieldName", TypeUtils.NativeNameFor(enumz.Fields.Single(f => f.Name == nameof(EnumOriginalName.FieldOriginalName))));
            Assert.AreEqual("CustomMethodName", TypeUtils.NativeNameFor(klass.Methods.Single(m => m.Name == nameof(ClassOriginalName.MethodOriginalName))));
            Assert.AreEqual("CustomPropertyName", TypeUtils.NativeNameFor(klass.Properties.Single(p => p.Name == nameof(ClassOriginalName.PropertyOriginalName))));
            Assert.AreEqual("GetCustomPropertyName", TypeUtils.NativeNameFor(klass.Properties.Single(p => p.Name == nameof(ClassOriginalName.PropertyOriginalName)).GetMethod));
            Assert.AreEqual("SetCustomPropertyName", TypeUtils.NativeNameFor(klass.Properties.Single(p => p.Name == nameof(ClassOriginalName.PropertyOriginalName)).SetMethod));
            Assert.AreEqual("CustomGetterName", TypeUtils.NativeNameFor(klass.Properties.Single(p => p.Name == nameof(ClassOriginalName.Property2OriginalName)).GetMethod));
            Assert.AreEqual("CustomSetterName", TypeUtils.NativeNameFor(klass.Properties.Single(p => p.Name == nameof(ClassOriginalName.Property2OriginalName)).SetMethod));
        }

        struct StructWithAutoProp
        {
            [NativeName("AutoPropertyCustomName")]
            public int AutoProperty1 { get; set; }
        }

        [Test]
        public void NativeNameWorksOnAutoProperty()
        {
            var klass = _assembly.MainModule.GetType("Unity.BindingsGenerator.Tests.NamingTests/StructWithAutoProp");

            Assert.AreEqual("AutoPropertyCustomName", TypeUtils.NativeNameFor(klass.Fields[0]));
        }
    }
}
