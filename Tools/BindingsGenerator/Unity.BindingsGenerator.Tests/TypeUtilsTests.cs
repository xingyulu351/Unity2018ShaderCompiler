using System;
using System.Runtime.CompilerServices;
using Mono.Cecil;
using Mono.Cecil.Rocks;
using NUnit.Framework;
using Unity.BindingsGenerator.Core;
using UnityEngine.Bindings;
using Unity.BindingsGenerator.TestFramework;

#pragma warning disable 649
#pragma warning disable 169

namespace UnityEditor.Experimental.Build.AssetBundle
{
    struct SomeStruct
    {
        int a;
    }
}

namespace Unity.BindingsGenerator.Tests
{
    [TestFixture]
    internal class TypeUtilsTests : BindingsGeneratorTestsBase
    {
        // NOTE: This is just some basic tests. Most of the testing of TypeUtils is done through the other unit tests. If we need to
        // better test the low level functions we can add more tests here.

        class ClassSimple
        {
        }

        [NativeType("ScriptingClassToStruct")]
        [NativeAsStruct]
        class ClassToStruct
        {
        }

        class ClassUnityEngineObject : UnityEngine.Object
        {
        }

        [MarshalUnityObjectAs(typeof(UnityEngine.Object))]
        class MarshalUnityObjectAsObject : UnityEngine.Object
        {
        }

        class ClassWithIntPtrFieldNotInheritingFromObject : ClassSimple
        {
            internal IntPtr field;
        }

        class DeepClassHierarchyWithBaseTypeWithIntPtr : ClassWithIntPtrFieldNotInheritingFromObject
        {
        }

        class ClassWithInt
        {
            private int i;
        }

        class DerivedClassWithIntPtr : ClassWithInt
        {
            internal IntPtr intptr;
        }

        class ClassWithIntPtrField
        {
            internal IntPtr field;
        }

        class ClassWithStaticAndIntPtrField
        {
            // Due to the order partial classes are compiled, a class can end up
            // with a static field before the IntPtr field.
            public static string ID = "-1";
            internal IntPtr field;
        }

        struct StructWithInt
        {
            private int i;
        }

        struct StructWithString
        {
            private string i;
        }

        [NativeType(CodegenOptions = CodegenOptions.Custom)]
        struct SimpleStructWithUseCustom
        {
            public int a;
        }

        [NativeType(CodegenOptions = CodegenOptions.Custom)]
        struct NonPodStructWithUseCustom
        {
            public string a;
        }

        [NativeType(CodegenOptions = CodegenOptions.Force)]
        struct SimpleStructWithForceGenerate
        {
            public int a;
        }

        [NativeType(CodegenOptions = CodegenOptions.Force)]
        struct NonPodStructWithForceGenerate
        {
            public string a;
        }

        enum SimpleEnum
        {
            A = 1,
        }

        [Test(Description = "Test that we correctly classify native pod types")]
        [TestCase(true, typeof(int))]
        [TestCase(false, typeof(ClassUnityEngineObject))]
        [TestCase(false, typeof(string))]
        [TestCase(true, typeof(StructWithInt))]
        [TestCase(false, typeof(StructWithString))]
        [TestCase(true, typeof(SimpleEnum))]
        public void IsNativePODTypeWorks(bool expectIsNativePod, Type type)
        {
            Assert.AreEqual(expectIsNativePod, TypeUtils.IsNativePodType(TypeResolver.GetTypeReference(type), ScriptingBackend.Mono));
        }

        [Test(Description = "Test that we correctly classify structs")]
        [TestCase(false, typeof(int))]
        [TestCase(false, typeof(ClassUnityEngineObject))]
        [TestCase(false, typeof(string))]
        [TestCase(true, typeof(StructWithInt))]
        [TestCase(true, typeof(StructWithString))]
        [TestCase(false, typeof(SimpleEnum))]
        public void IsStructWorks(bool expectIsStruct, Type type)
        {
            Assert.AreEqual(expectIsStruct, TypeUtils.IsStruct(TypeResolver.GetTypeReference(type)));
        }

        [Test(Description = "Test that we correctly translate unmarshalled type names for primitive types")]
        [TestCase("ScriptingBool", typeof(bool), false)]
        [TestCase("SInt16", typeof(char), false)]
        [TestCase("SInt8", typeof(sbyte), false)]
        [TestCase("SInt16", typeof(short), false)]
        [TestCase("SInt32", typeof(int), false)]
        [TestCase("SInt64", typeof(long), false)]
        [TestCase("UInt8", typeof(byte), false)]
        [TestCase("UInt16", typeof(ushort), false)]
        [TestCase("UInt32", typeof(uint), false)]
        [TestCase("UInt64", typeof(ulong), false)]
        [TestCase("float", typeof(float), false)]
        [TestCase("double", typeof(double), false)]
        public void ResolvePrimitiveUnmarshalledTypesWorks(string expectedUnmarshalledTypeName, Type type, bool expectExposesManagedTypeName)
        {
            var typeRef = TypeResolver.GetTypeReference(type);
            var actualUnmarshalledTypeName = TypeUtils.ResolveUnmarshalledTypeName(typeRef, TypeUsage.ReturnType, ScriptingBackend.Mono);
            Assert.AreEqual(expectedUnmarshalledTypeName, actualUnmarshalledTypeName);
            Assert.AreEqual(expectExposesManagedTypeName, actualUnmarshalledTypeName.ExposesManagedTypeNameFor(typeRef));
        }

        [Test(Description = "Test that we correctly translate unmarshalled type names for array types")]
        [TestCase("ICallType_Array_Return", typeof(int[]), false)]
        [TestCase("ICallType_Array_Return", typeof(string[]), false)]
        [TestCase("ICallType_Array_Return", typeof(StructWithInt[]), false)]
        [TestCase("ICallType_Array_Return", typeof(StructWithString[]), false)]
        public void ResolveArrayUnmarshalledTypeWorks(string expectedUnmarshalledTypeName, Type type, bool expectExposesManagedTypeName)
        {
            var typeRef = TypeResolver.GetTypeReference(type);
            var actualUnmarshalledTypeName = TypeUtils.ResolveUnmarshalledTypeName(typeRef, TypeUsage.ReturnType, ScriptingBackend.Mono);
            Assert.AreEqual(expectedUnmarshalledTypeName, actualUnmarshalledTypeName);
            Assert.AreEqual(expectExposesManagedTypeName, actualUnmarshalledTypeName.ExposesManagedTypeNameFor(typeRef));
        }

        [Test(Description = "Test that we correctly translate unmarshalled type names for structs")]
        [TestCase("ICallType_Generated_Unity_BindingsGenerator_Tests_TypeUtilsTests_StructWithInt", typeof(StructWithInt), true)]
        [TestCase("ICallType_Generated_UnityEditor_Experimental_Build_AssetBundle_SomeStruct", typeof(UnityEditor.Experimental.Build.AssetBundle.SomeStruct), true)]
        [TestCase("StructWithString__", typeof(StructWithString), false)]
        public void ResolveStructUnmarshalledTypeWorks(string expectedUnmarshalledTypeName, Type type, bool expectExposesManagedTypeName)
        {
            var typeRef = TypeResolver.GetTypeReference(type);
            var actualUnmarshalledTypeName = TypeUtils.ResolveUnmarshalledTypeName(typeRef, TypeUsage.ReturnType, ScriptingBackend.Mono);
            Assert.AreEqual(expectedUnmarshalledTypeName, actualUnmarshalledTypeName);
            Assert.AreEqual(expectExposesManagedTypeName, actualUnmarshalledTypeName.ExposesManagedTypeNameFor(typeRef));
        }

        [Test(Description = "Test that we correctly translate unmarshalled parameter type names")]
        [TestCase("const ICallType_Generated_Unity_BindingsGenerator_Tests_TypeUtilsTests_StructWithInt&", typeof(StructWithInt), true)]
        [TestCase("const StructWithString__&", typeof(StructWithString), false)]
        [TestCase("Marshalling::ManagedObjectForStruct<ClassToStruct__>", typeof(ClassToStruct), false)]
        public void ResolveUnmarshalledParameterTypeWorks(string expectedUnmarshalledTypeName, Type type, bool expectExposesManagedTypeName)
        {
            var typeRef = TypeResolver.GetTypeReference(type);
            var actualUnmarshalledTypeName = TypeUtils.ResolveUnmarshalledTypeName(typeRef, TypeUsage.Parameter, ScriptingBackend.Mono);
            Assert.AreEqual(expectedUnmarshalledTypeName, actualUnmarshalledTypeName);
            Assert.AreEqual(expectExposesManagedTypeName, actualUnmarshalledTypeName.ExposesManagedTypeNameFor(typeRef));
        }

        [Test(Description = "Test that we correctly translate unmarshalled names for scripting objects")]
        [TestCase("Marshalling::UnityObjectReturnValue", typeof(ClassUnityEngineObject), TypeUsage.ReturnType, false)]
        [TestCase("Marshalling::UnityObjectReturnValue", typeof(UnityEngine.Object), TypeUsage.ReturnType, false)]
        [TestCase("ICallType_Object_Argument", typeof(ClassUnityEngineObject), TypeUsage.UnmarshalledParameter, false)]
        [TestCase("Marshalling::UnityObjectStructField<ICallType_Generated_UnityEngine_Object>", typeof(MarshalUnityObjectAsObject), TypeUsage.StructField, false)]
        public void ResolveObjectUnmarshalledTypeWorks(string expectedUnmarshalledTypeName, Type type, TypeUsage typeUsage, bool expectExposesManagedTypeName)
        {
            var typeRef = TypeResolver.GetTypeReference(type);
            var actualUnmarshalledTypeName = TypeUtils.ResolveUnmarshalledTypeName(typeRef, typeUsage, ScriptingBackend.Mono);
            Assert.AreEqual(expectedUnmarshalledTypeName, actualUnmarshalledTypeName);
            Assert.AreEqual(expectExposesManagedTypeName, actualUnmarshalledTypeName.ExposesManagedTypeNameFor(typeRef));
        }

        [TestCase("Marshalling::ArrayStructField<ICallType_Generated_Unity_BindingsGenerator_Tests_TypeUtilsTests_SimpleEnum,SimpleEnum__>", typeof(SimpleEnum[]), TypeUsage.StructField, true)]
        [TestCase("Marshalling::ArrayStructField<Marshalling::ArrayArrayElement<ICallType_Generated_Unity_BindingsGenerator_Tests_TypeUtilsTests_SimpleEnum,SimpleEnum__> >", typeof(SimpleEnum[][]), TypeUsage.StructField, true)]
        public void ResolveUnmarshalledTypeWorks(string expectedUnmarshalledTypeName, Type type, TypeUsage typeUsage, bool expectExposesManagedTypeName)
        {
            var typeRef = TypeResolver.GetTypeReference(type);
            var actualUnmarshalledTypeName = TypeUtils.ResolveUnmarshalledTypeName(typeRef, typeUsage, ScriptingBackend.Mono);
            Assert.AreEqual(expectedUnmarshalledTypeName, actualUnmarshalledTypeName);
            Assert.AreEqual(expectExposesManagedTypeName, actualUnmarshalledTypeName.ExposesManagedTypeNameFor(typeRef));
        }

        [Test(Description = "Test that we correctly translate unmarshalled names for scripting objects")]
        [TestCase("ICallType_Generated_Unity_BindingsGenerator_Tests_TypeUtilsTests_SimpleEnum", typeof(SimpleEnum), false)]
        [TestCase("ICallType_Generated_Unity_BindingsGenerator_Tests_TypeUtilsTests_SimpleEnum*", typeof(SimpleEnum), true)]
        public void ResolveEnumUnmarshalledTypeWorks(string expectedUnmarshalledTypeName, Type type, bool isByReference)
        {
            var typeRef = TypeResolver.GetTypeReference(type);
            if (isByReference)
                typeRef = typeRef.MakeByReferenceType();

            var actualUnmarshalledTypeName = TypeUtils.ResolveUnmarshalledTypeName(typeRef, TypeUsage.Parameter, ScriptingBackend.Mono);
            Assert.AreEqual(expectedUnmarshalledTypeName, actualUnmarshalledTypeName);
            Assert.IsTrue(actualUnmarshalledTypeName.ExposesManagedTypeNameFor(typeRef));
        }

        [Test(Description = "Test that we correctly translate marshalled names for primitive types")]
        [TestCase("SInt16", typeof(char), TypeUsage.Parameter, false)]
        [TestCase("SInt8",  typeof(sbyte), TypeUsage.Parameter, false)]
        [TestCase("SInt16", typeof(short), TypeUsage.Parameter, false)]
        [TestCase("SInt32", typeof(int), TypeUsage.Parameter, false)]
        [TestCase("SInt64", typeof(long), TypeUsage.Parameter, false)]
        [TestCase("UInt8",  typeof(byte), TypeUsage.Parameter, false)]
        [TestCase("UInt16", typeof(ushort), TypeUsage.Parameter, false)]
        [TestCase("UInt32", typeof(uint), TypeUsage.Parameter, false)]
        [TestCase("UInt64", typeof(ulong), TypeUsage.Parameter, false)]
        [TestCase("Marshalling::ArrayMarshaller<SInt32>", typeof(int[]), TypeUsage.Parameter, false)]

        [TestCase("Marshalling::StringMarshaller", typeof(string), TypeUsage.Parameter, false)]
        [TestCase("Marshalling::ArrayMarshaller<Marshalling::StringArrayElement>", typeof(string[]), TypeUsage.Parameter, false)]

        [TestCase("ICallType_Generated_Unity_BindingsGenerator_Tests_TypeUtilsTests_SimpleEnum", typeof(SimpleEnum), TypeUsage.Parameter, true)]
        [TestCase("Marshalling::ArrayMarshaller<ICallType_Generated_Unity_BindingsGenerator_Tests_TypeUtilsTests_SimpleEnum,SimpleEnum__>", typeof(SimpleEnum[]), TypeUsage.Parameter, true)]
        [TestCase("Marshalling::ArrayMarshaller<Marshalling::ArrayArrayElement<ICallType_Generated_Unity_BindingsGenerator_Tests_TypeUtilsTests_SimpleEnum,SimpleEnum__> >", typeof(SimpleEnum[][]), TypeUsage.Parameter, true)]
        [TestCase("Marshalling::ArrayUnmarshaller<ICallType_Generated_Unity_BindingsGenerator_Tests_TypeUtilsTests_SimpleEnum,SimpleEnum__>", typeof(SimpleEnum[]), TypeUsage.ReturnType, true)]
        [TestCase("Marshalling::ArrayUnmarshaller<ScriptingBool,bool>", typeof(bool[]), TypeUsage.ReturnType, false)]
        [TestCase("Marshalling::ArrayUnmarshaller<SInt16,Marshalling::ScriptingCharForMarshalInfo>", typeof(char[]), TypeUsage.ReturnType, false)]
        [TestCase("Marshalling::ArrayOutMarshaller<ICallType_Generated_Unity_BindingsGenerator_Tests_TypeUtilsTests_SimpleEnum,SimpleEnum__>", typeof(SimpleEnum[]), TypeUsage.OutParameter, true)]

        [TestCase("ICallType_Generated_Unity_BindingsGenerator_Tests_TypeUtilsTests_StructWithInt", typeof(StructWithInt), TypeUsage.Parameter, true)]
        [TestCase("ICallType_Generated_Unity_BindingsGenerator_Tests_TypeUtilsTests_StructWithString", typeof(StructWithString), TypeUsage.Parameter, true)]

        [TestCase("Marshalling::ReadOnlyUnityObjectMarshaller<ICallType_Generated_Unity_BindingsGenerator_Tests_TypeUtilsTests_ClassUnityEngineObject>", typeof(ClassUnityEngineObject), TypeUsage.Parameter, true)]
        [TestCase("Marshalling::ReadOnlyUnityObjectMarshaller<ICallType_Generated_UnityEngine_Object>", typeof(MarshalUnityObjectAsObject), TypeUsage.Parameter, false)]
        [TestCase("Marshalling::ReadOnlyUnityObjectMarshaller<ICallType_Generated_UnityEngine_Object>", typeof(UnityEngine.Object), TypeUsage.Parameter, true)]
        [TestCase("Marshalling::ManagedObjectMarshaller", typeof(UnityEngine.Object), TypeUsage.UnmarshalledParameter, false)]

        [TestCase("Marshalling::ManagedObjectMarshaller", typeof(ClassSimple), TypeUsage.Parameter, false)]
        [TestCase("ICallType_Generated_Unity_BindingsGenerator_Tests_TypeUtilsTests_ClassToStruct", typeof(ClassToStruct), TypeUsage.Parameter, true)]
        [TestCase("Marshalling::ManagedObjectMarshaller", typeof(ClassSimple), TypeUsage.UnmarshalledParameter, false)]
        [TestCase("Marshalling::ManagedObjectMarshaller", typeof(ClassToStruct), TypeUsage.UnmarshalledParameter, false)]
        public void ResolveMarshalledTypeNameWorks(string expectedUnmarshalledTypeName, Type type, TypeUsage typeUsage, bool expectExposesManagedTypeName)
        {
            var typeRef = TypeResolver.GetTypeReference(type);
            var actualMarshalledTypeName = TypeUtils.ResolveMarshalledTypeName(typeRef, typeUsage, ScriptingBackend.Mono);
            Assert.AreEqual(expectedUnmarshalledTypeName, actualMarshalledTypeName);
            Assert.AreEqual(expectExposesManagedTypeName, actualMarshalledTypeName.ExposesManagedTypeNameFor(typeRef));
        }

        [Test(Description = "Test that struct type name marshalling works")]
        [TestCase("ICallType_Generated_Unity_BindingsGenerator_Tests_TypeUtilsTests_StructWithInt", typeof(StructWithInt), TypeUsage.Parameter, true)]
        [TestCase("Marshalling::OutMarshaller<StructWithString__, ICallType_Generated_Unity_BindingsGenerator_Tests_TypeUtilsTests_StructWithString>", typeof(StructWithString), TypeUsage.Parameter, true)]
        [TestCase("Marshalling::OutMarshaller<Marshalling::ManagedObjectForStruct<ClassToStruct__>, ICallType_Generated_Unity_BindingsGenerator_Tests_TypeUtilsTests_ClassToStruct>", typeof(ClassToStruct), TypeUsage.OutParameter, true)]
        public void ResolveByReferenceMarshalledTypeWorks(string expectedUnmarshalledTypeName, Type type, TypeUsage typeUsage, bool expectExposesManagedTypeName)
        {
            var typeRef = TypeResolver.GetTypeReference(type).MakeByReferenceType();

            var actualMarshalledTypeName = TypeUtils.ResolveMarshalledTypeName(typeRef, typeUsage, ScriptingBackend.Mono);
            Assert.AreEqual(expectedUnmarshalledTypeName, actualMarshalledTypeName);
            Assert.AreEqual(expectExposesManagedTypeName, actualMarshalledTypeName.ExposesManagedTypeNameFor(typeRef));
        }

        [Test(Description = "Test that we know when a type needs a marshalling struct")]
        [TestCase(false, typeof(int))]
        [TestCase(false, typeof(StructWithInt))]
        [TestCase(true, typeof(StructWithString))]
        [TestCase(false, typeof(SimpleEnum))]
        public void TestRequireMarshallingStructWorks(bool expectedRequiresMarshallingStruct, Type type)
        {
            Assert.AreEqual(expectedRequiresMarshallingStruct, TypeUtils.RequireMarshalling(TypeResolver.GetTypeReference(type), ScriptingBackend.Mono));
        }

        [Test(Description = "Test that the attribute overrides work for generating marshalling struct")]
        [TestCase(true, typeof(SimpleStructWithUseCustom))]
        [TestCase(true, typeof(SimpleStructWithForceGenerate))]
        [TestCase(true, typeof(NonPodStructWithUseCustom))]
        [TestCase(true, typeof(NonPodStructWithForceGenerate))]
        public void TestRequireMarshallingStructOverridesWork(bool expectedRequiresMarshallingStruct, Type type)
        {
            Assert.AreEqual(expectedRequiresMarshallingStruct, TypeUtils.RequireMarshalling(TypeResolver.GetTypeReference(type), ScriptingBackend.Mono));
        }

        [Test(Description = "Test that the GetScriptingObjectMarshallingType works properly")]
        [TestCase(ScriptingObjectMarshallingType.Invalid, typeof(int))]
        [TestCase(ScriptingObjectMarshallingType.Invalid, typeof(SomeEnum))]
        [TestCase(ScriptingObjectMarshallingType.Invalid, typeof(string))]
        [TestCase(ScriptingObjectMarshallingType.Invalid, typeof(SomeStruct))]
        [TestCase(ScriptingObjectMarshallingType.Invalid, typeof(object[]))]
        [TestCase(ScriptingObjectMarshallingType.Invalid, typeof(SomeClass[]))]
        [TestCase(ScriptingObjectMarshallingType.Invalid, typeof(ClassUnityEngineObject[]))]
        [TestCase(ScriptingObjectMarshallingType.Invalid, typeof(ClassWithIntPtrField[]))]
        [TestCase(ScriptingObjectMarshallingType.ScriptingObjectPtr, typeof(DerivedClassWithIntPtr))]
        [TestCase(ScriptingObjectMarshallingType.ScriptingObjectPtr, typeof(object))]
        [TestCase(ScriptingObjectMarshallingType.ScriptingObjectPtr, typeof(SomeClass))]
        [TestCase(ScriptingObjectMarshallingType.UnityEngineObject, typeof(ClassUnityEngineObject))]
        [TestCase(ScriptingObjectMarshallingType.IntPtrObject, typeof(ClassWithIntPtrField))]
        [TestCase(ScriptingObjectMarshallingType.IntPtrObject, typeof(ClassWithStaticAndIntPtrField))]
        [TestCase(ScriptingObjectMarshallingType.IntPtrObject, typeof(DeepClassHierarchyWithBaseTypeWithIntPtr))]
        public void TestGetScriptingObjectTypeWorks(ScriptingObjectMarshallingType expectMarshallingType, Type type)
        {
            Assert.AreEqual(expectMarshallingType, TypeUtils.GetScriptingObjectMarshallingType(TypeResolver.GetTypeReference(type)));
        }

        class SomeClass
        {
        }

        struct SomeStruct
        {
        }

        struct SomeNonBlittableStruct
        {
            private string a;
        }

        enum SomeEnum
        {
            A = 1,
        }

        class CouldBeNullShouldBeTrue
        {
            [MethodImpl(MethodImplOptions.InternalCall)]
            public static extern void ClassTest(SomeClass a);
            [MethodImpl(MethodImplOptions.InternalCall)]
            public static extern void ArrayTest(int[] a, SomeClass[] b);
            [MethodImpl(MethodImplOptions.InternalCall)]
            public static extern void StringTest(string a, ref string b);
            [MethodImpl(MethodImplOptions.InternalCall)]
            public static extern void SystemTypeTest(System.Type a);
        }

        class CouldBeNullShouldBeFalse
        {
            [MethodImpl(MethodImplOptions.InternalCall)]
            public static extern void StructTest(SomeStruct a);
            [MethodImpl(MethodImplOptions.InternalCall)]
            public static extern void StructRefTest(ref SomeStruct b);
            [MethodImpl(MethodImplOptions.InternalCall)]
            public static extern void StructTest(SomeNonBlittableStruct a);
            [MethodImpl(MethodImplOptions.InternalCall)]
            public static extern void EnumTest(SomeEnum a);
            [MethodImpl(MethodImplOptions.InternalCall)]
            public static extern void SimpleTypeTest(bool a, int b, float c, double d);
            [MethodImpl(MethodImplOptions.InternalCall)]
            public static extern void SimpleTypeRefTest(ref bool a, ref int b, ref float c, ref double d);
            [MethodImpl(MethodImplOptions.InternalCall)]
            public static extern void EnumRefTest(ref SomeEnum b);
        }

        [Test]
        public void TestCouldBeNullTrueWorks()
        {
            var t = TypeResolver.TypeDefinitionFor(typeof(CouldBeNullShouldBeTrue));

            foreach (var methodDefinition in t.Methods)
            {
                foreach (var parameterDefinition in methodDefinition.Parameters)
                {
                    Assert.AreEqual(true, TypeUtils.CouldBeNull(parameterDefinition.ParameterType));
                }
            }
        }

        [Test]
        public void TestCouldBeNullFalseWorks()
        {
            var t = TypeResolver.TypeDefinitionFor(typeof(CouldBeNullShouldBeFalse));

            foreach (var methodDefinition in t.Methods)
            {
                foreach (var parameterDefinition in methodDefinition.Parameters)
                {
                    Assert.AreEqual(false, TypeUtils.CouldBeNull(parameterDefinition.ParameterType));
                }
            }
        }

        public unsafe struct StructWithFixedBuffer
        {
            fixed int SomeInts[32];
        }

        [Test]
        public void FixedBufferStructIsBlittable()
        {
            Assert.IsTrue(TypeUtils.IsNativePodType(TypeResolver.TypeDefinitionFor(typeof(StructWithFixedBuffer)), ScriptingBackend.Mono));
            Assert.AreEqual(128, System.Runtime.InteropServices.Marshal.SizeOf(typeof(StructWithFixedBuffer)));
        }

        [Test]
        [TestCase(typeof(bool), true, ScriptingBackend.Mono)]
        [TestCase(typeof(bool[]), false, ScriptingBackend.Mono)]
        [TestCase(typeof(bool), false, ScriptingBackend.DotNet)]
        [TestCase(typeof(bool[]), false, ScriptingBackend.DotNet)]
        [TestCase(typeof(int), true, ScriptingBackend.Mono)]
        [TestCase(typeof(int[]), false, ScriptingBackend.Mono)]
        [TestCase(typeof(string), false, ScriptingBackend.Mono)]
        [TestCase(typeof(char), true, ScriptingBackend.Mono)]
        [TestCase(typeof(char), false, ScriptingBackend.DotNet)]
        public void IsNativePodTypeWorks(Type type, bool expectedNativePod, ScriptingBackend scriptingBackend)
        {
            Assert.AreEqual(expectedNativePod, TypeUtils.IsNativePodType(TypeResolver.GetTypeReference(type), scriptingBackend));
        }
    }
}
