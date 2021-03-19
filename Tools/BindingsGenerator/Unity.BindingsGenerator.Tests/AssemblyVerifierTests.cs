using System;
using System.Collections.Generic;
using System.Linq;
using System.Runtime.CompilerServices;
using System.Runtime.InteropServices;
using NUnit.Framework;
using Unity.BindingsGenerator.Core;
using UnityEngine.Bindings;
using Unity.BindingsGenerator.TestFramework;
using UnityEngine;
using Error = Unity.BindingsGenerator.Core.AssemblyVerifier.Error;

namespace Unity.BindingsGenerator.Tests
{
    [TestFixture]
    internal class AssemblyVerifierTests : BindingsGeneratorTestsBase
    {
        #pragma warning disable 169

        [Test]
        public void ErrorMessageGeneration()
        {
            var errors = new AssemblyVerifier.ErrorList();
            errors.Add(new AssemblyVerifier.Error("S1", AssemblyVerifier.Error.Type.KFreeFunctionMustBeStaticOrExplicitThis));
            errors.Add(new AssemblyVerifier.Error("S2", AssemblyVerifier.Error.Type.KFreeFunctionMustBeStaticOrExplicitThis));

            var str = errors.ToString();

            new AssertingStringMatcher(str)
                .Expect("S1:").Find("Free function must be static").Find("\n")
                .Expect("S2:").Find("Free function must be static").Find(".")
                .End();
        }

        [Test]
        public void ErrorMessageGenerationWithReplacement()
        {
            var errors = new AssemblyVerifier.ErrorList();
            errors.Add(new AssemblyVerifier.Error("S1", AssemblyVerifier.Error.Type.KIllegalParameterName, "ILLEGAL"));

            var str = errors.ToString();

            new AssertingStringMatcher(str)
                .Expect("S1: ILLEGAL is not a legal parameter name.")
                .End();
        }

        private class FreeFunctions : UnityEngine.Object
        {
            [MethodImpl(MethodImplOptions.InternalCall)]
            [NativeMethod(IsFreeFunction = true)]
            public static extern void StaticMethod();

            [MethodImpl(MethodImplOptions.InternalCall)]
            [NativeMethod(IsFreeFunction = true)]
            public extern void NonStaticMethod();
        }

        [Test]
        public void FreeFunctionsMustBeStatic()
        {
            var errors = AssemblyVerifier.Verify(TypeResolver.TypeDefinitionFor(typeof(FreeFunctions))).Errors;
            Assert.AreEqual(1, errors.Count);
            Assert.AreEqual(errors[0].ErrorNo, AssemblyVerifier.Error.Type.KFreeFunctionMustBeStaticOrExplicitThis);
            Assert.IsTrue(errors[0].SourceName.Contains("::NonStaticMethod"));
        }

        private class NonSensicalAttributes : UnityEngine.Object
        {
            [NativeMethod]
            public void NonNativeMethod()
            {
            }

            public int NonNativeProperty {[NativeMethod] get; [NativeMethod] set; }
        }

        [Test]
        public void SensicalAttributeCount()
        {
            var errors = AssemblyVerifier.Verify(TypeResolver.TypeDefinitionFor(typeof(NonSensicalAttributes))).Errors;
            Assert.AreEqual(3, errors.Count);
        }

        [TestCase(AssemblyVerifier.Error.Type.KNonSensicalAttribute,    "::NonNativeMethod")]
        [TestCase(AssemblyVerifier.Error.Type.KNonSensicalAttribute,    "::get_NonNativeProperty")]
        [TestCase(AssemblyVerifier.Error.Type.KNonSensicalAttribute,    "::set_NonNativeProperty")]
        public void SensicalAttributes(AssemblyVerifier.Error.Type type, string sourceName)
        {
            var errors = AssemblyVerifier.Verify(TypeResolver.TypeDefinitionFor(typeof(NonSensicalAttributes))).Errors;
            Assert.IsNotNull(
                errors.SingleOrDefault(e =>
                    e.ErrorNo == type &&
                    e.SourceName.Contains(sourceName)));
        }

        private class IllegalParameterNames : UnityEngine.Object
        {
            [MethodImpl(MethodImplOptions.InternalCall)]
            public static extern void NativeMethod(int a, int _unity_self, int qqq, int cj, int cj_marshalled, int qq_marshalled, int bb, int a_marshalled, int _unity_self_marshalled, int ret, int ret_marshalled);

            [MethodImpl(MethodImplOptions.InternalCall)]
            public extern void NativeMethodNonStatic(int _unity_self, int _unity_self_marshalled, int a);

            [MethodImpl(MethodImplOptions.InternalCall)]
            public static extern int NativeMethodReturning(int ret, int ret_marshalled, int a);

            public int NonNativeMethod(int a, int self, int qqq, int cj, int cj_marshalled, int qq_marshalled, int bb, int a_marshalled, int self_marshalled, int ret, int ret_marshalled)
            {
                return 0;
            }
        }

        [Test]
        public void LegalParameterNameCount()
        {
            var errors = AssemblyVerifier.Verify(TypeResolver.TypeDefinitionFor(typeof(IllegalParameterNames))).Errors;

            Assert.AreEqual(8, errors.Count);
        }

        [TestCase("ret_marshalled",         "NativeMethod")]
        [TestCase("_unity_self_marshalled", "NativeMethod")]
        [TestCase("a_marshalled",           "NativeMethod")]
        [TestCase("cj_marshalled",          "NativeMethod")]
        [TestCase("_unity_self_marshalled", "NativeMethodNonStatic")]
        [TestCase("_unity_self",            "NativeMethodNonStatic")]
        [TestCase("ret_marshalled",         "NativeMethodReturning")]
        [TestCase("ret",                    "NativeMethodReturning")]
        public void LegalParameterNames(string parameterName, string methodName)
        {
            var errors = AssemblyVerifier.Verify(TypeResolver.TypeDefinitionFor(typeof(IllegalParameterNames))).Errors;

            var errorStr = string.Format("'{0}' is not", parameterName);
            var uniqueMethodStr = string.Format("::{0}(", methodName);

            Assert.IsNotNull(
                errors.SingleOrDefault(
                    e =>
                        e.ErrorNo == AssemblyVerifier.Error.Type.KIllegalParameterName &&
                        e.ToString().Contains(errorStr) &&
                        e.SourceName.Contains(uniqueMethodStr)));
        }

        private class UnityEngineObjectInheritors
        {
            public class ClassWithNormalMethods
            {
                void NormalMethod()
                {
                }
            }

            public class NonMarshallableClassWithExternMethod
            {
                [MethodImpl(MethodImplOptions.InternalCall)]

                public extern void ExternMethod();
            }

            public class ClassWithStaticExternMethod
            {
                [MethodImpl(MethodImplOptions.InternalCall)]

                public extern static void ExternStaticMethod();
            }

            public class InheritingClassWithNormalMethods : UnityEngine.Object
            {
                void NormalMethod()
                {
                }
            }

            public class InheritingClassWithExternMethod : UnityEngine.Object
            {
                [MethodImpl(MethodImplOptions.InternalCall)]

                public extern void ExternMethod();
            }

            public class InheritingClassWithStaticExternMethod : UnityEngine.Object
            {
                [MethodImpl(MethodImplOptions.InternalCall)]

                public extern static void ExternStaticMethod();
            }

            public class IntPtrDummy
            {
                private IntPtr dummy;
            }

            public class SimpleClassWithFields
            {
                private int field;
            }

            public class ClassThatDoesNotDeclareFieldsButInheritsFromClassWithFields : SimpleClassWithFields
            {
            }

            public class ClassInheritingFromClassThatDoesNotDeclareFieldsButInheritsFromClassWithFields : ClassThatDoesNotDeclareFieldsButInheritsFromClassWithFields
            {
                private IntPtr intptrField;

                [MethodImpl(MethodImplOptions.InternalCall)]

                public extern void ExternMethod();
            }

            public class BaseTypeWithoutFields
            {
            }

            public class IntPtrClassDerivingFromCustomClass : BaseTypeWithoutFields
            {
                private IntPtr dummy;
            }

            public class ClassInheritingFromClassWithIntPtrAsFirstField : IntPtrClassDerivingFromCustomClass
            {
                [MethodImpl(MethodImplOptions.InternalCall)]

                public extern void ExternMethod();
            }

            public class InheritingDummy : UnityEngine.Object
            {
            }

            public class Dummy
            {
            }

            public class InheritingIntPtrDummy : InheritingDummy
            {
                private IntPtr dummy;

                [MethodImpl(MethodImplOptions.InternalCall)]

                public extern void DummyMethod();
            }

            public class ClassWithExternMethodAndArguments : UnityEngine.Object
            {
                [MethodImpl(MethodImplOptions.InternalCall)]

                public extern void ExternMethodWithNonInheritingParameter1(Dummy a);

                [MethodImpl(MethodImplOptions.InternalCall)]

                public extern void ExternMethodWithInheritingParameter(InheritingDummy a);

                [MethodImpl(MethodImplOptions.InternalCall)]

                public extern void ExternMethodWithIntPtrObjectParameter(IntPtrDummy a);

                public void MethodWithNonInheritingParameter(Dummy a)
                {
                }

                public void MethodWithInheritingParameter(InheritingDummy a)
                {
                }

                public void MethodWithIntPtrObjectParameter(IntPtrDummy a)
                {
                }
            }

            public class IntPtrObjectWithExternMethodAndArguments
            {
                private IntPtr dummy;

                [MethodImpl(MethodImplOptions.InternalCall)]

                public extern void ExternMethodWithNonInheritingParameter2(Dummy a);

                [MethodImpl(MethodImplOptions.InternalCall)]

                public extern void ExternMethodWithInheritingParameter(InheritingDummy a);

                [MethodImpl(MethodImplOptions.InternalCall)]

                public extern void ExternMethodWithIntPtrObjectParameter(IntPtrDummy a);

                public void MethodWithNonInheritingParameter(Dummy a)
                {
                }

                public void MethodWithInheritingParameter(InheritingDummy a)
                {
                }

                public void MethodWithIntPtrObjectParameter(IntPtrDummy a)
                {
                }
            }
        }

        [Test]
        public void VerifyUnityEngineObjectInheritance()
        {
            var errors = AssemblyVerifier.Verify(TypeResolver.TypeDefinitionFor(typeof(UnityEngineObjectInheritors))).Errors;


            Assert.IsTrue(errors.All(e => e.ErrorNo == AssemblyVerifier.Error.Type.KClassMustBeMarshallable));

            string[] typesWithIssues = { "NonMarshallableClassWithExternMethod", "ClassInheritingFromClassThatDoesNotDeclareFieldsButInheritsFromClassWithFields" };
            foreach (var typeWithIssue in typesWithIssues)
            {
                Assert.NotNull(errors.SingleOrDefault(e => e.SourceName.Contains(typeWithIssue)), string.Format("Type should have been flaged as invalid: {0}", typeWithIssue));
            }

            Assert.AreEqual(2, errors.Count, string.Join(Environment.NewLine, errors));
        }

        public struct BlittableStructWithExternMethods
        {
            private int i;

            [MethodImpl(MethodImplOptions.InternalCall)]
            public extern void ExternMethod();

            public void NonExternMethod() {}

            [MethodImpl(MethodImplOptions.InternalCall)]
            public static extern void StaticExternMethod();
        }

        [Test]
        public void VerifyBlittableStructMethods()
        {
            var errors = AssemblyVerifier.Verify(TypeResolver.TypeDefinitionFor(typeof(BlittableStructWithExternMethods))).Errors;
            Assert.AreEqual(0, errors.Count);
        }

        public struct NonBlittableStructWithExternMethods
        {
            private string s;

            [MethodImpl(MethodImplOptions.InternalCall)]
            public extern void ExternMethod();

            public void NonExternMethod() {}

            [MethodImpl(MethodImplOptions.InternalCall)]
            public static extern void StaticExternMethod();
        }

        public class TypeReplacementTestClass : UnityEngine.Object
        {
            [MethodImpl(MethodImplOptions.InternalCall)]
            public extern void ExternMethodWithReplacedType(Type t);

            [MethodImpl(MethodImplOptions.InternalCall)]
            public extern void ExternMethodWithUnreplacedType(Activator a);
        }

        [Test]
        public void VerifyTypeReplacements()
        {
            var errors = AssemblyVerifier.Verify(TypeResolver.TypeDefinitionFor(typeof(TypeReplacementTestClass))).Errors;
            Assert.AreEqual(0, errors.Count);
        }

        class SomeClass : UnityEngine.Object
        {
        }

        struct SomeStruct
        {
        }

        enum SomeEnum
        {
            A = 1,
        }

        class SensicalNotNullTestClass
        {
            [MethodImpl(MethodImplOptions.InternalCall)]
            public static extern void StringTest(string a, [NotNull] string b, [NotNull] ref string c);
            [MethodImpl(MethodImplOptions.InternalCall)]
            public static extern void ClassTest(SomeClass a, [NotNull] SomeClass b, [NotNull] ref SomeClass c);
            [MethodImpl(MethodImplOptions.InternalCall)]
            public static extern void ArrayTest(int[] a, [NotNull] SomeStruct[] b, [NotNull] SomeClass[] c, [NotNull] int[] d);
        }

        [Test]
        public void VerifySensicalCanBeNull()
        {
            var errors = AssemblyVerifier.Verify(TypeResolver.TypeDefinitionFor(typeof(SensicalNotNullTestClass))).Errors;
            Assert.That(errors, Is.Empty);
        }

        class NonsensicalNotNullTestClass
        {
            [MethodImpl(MethodImplOptions.InternalCall)]
            public static extern void StructTest([NotNull] SomeStruct a);
            [MethodImpl(MethodImplOptions.InternalCall)]
            public static extern void StructRefTest([NotNull] ref SomeStruct a);
            [MethodImpl(MethodImplOptions.InternalCall)]
            public static extern void EnumTest([NotNull] SomeEnum a);
            [MethodImpl(MethodImplOptions.InternalCall)]
            public static extern void PrimitiveWithAttrTest([NotNull] bool a, [NotNull] int b, [NotNull] float c, [NotNull] double d);
            [MethodImpl(MethodImplOptions.InternalCall)]
            public static extern void PrimitiveRefWithAttrTest([NotNull] ref bool a, [NotNull] ref int b, [NotNull] ref float c, [NotNull] ref double d);

            public static void NonExtern([NotNull] string a) {}
        }

        [Test]
        public void VerifyNonsensicalNotNull()
        {
            var errors = AssemblyVerifier.Verify(TypeResolver.TypeDefinitionFor(typeof(NonsensicalNotNullTestClass))).Errors;
            Assert.AreEqual(12, errors.Count);
        }

        public class DefaultParameterTestClass : UnityEngine.Object
        {
            public enum SomeEnum
            {
                A, B
            }

            public struct SomeStruct
            {
                private int a;
            }

            public class SomeClass : UnityEngine.Object
            {
                private int a;
            }

            [MethodImpl(MethodImplOptions.InternalCall)]
            public extern void AllowedDefaultParameters(int a = 1, string b = "", string c = null, SomeEnum d = SomeEnum.A, char e = 'A', bool f = false, float g = 1, double h = 2, SomeClass i = null);

            [MethodImpl(MethodImplOptions.InternalCall)]
            public extern void DisallowedDefaultParameters(SomeStruct ok, SomeStruct a = new SomeStruct(), SomeStruct b = default(SomeStruct));
        }

        [Test]
        [NUnit.Framework.Ignore("Default parameters expansion is now disabled in the bindings generator")]
        public void VerifyDefaultParameters()
        {
            var errors = AssemblyVerifier.Verify(TypeResolver.TypeDefinitionFor(typeof(DefaultParameterTestClass))).Errors;
            Assert.IsTrue(errors.All(e => e.ErrorNo == AssemblyVerifier.Error.Type.KDefaultParameterNotSupported && e.SourceName.Contains("DefaultParameterTestClass::DisallowedDefaultParameters")));
            Assert.AreEqual(2, errors.Count);
        }

        class StubReturnTypeTestClass : UnityEngine.Object
        {
            [MethodImpl(MethodImplOptions.InternalCall)]
            [NativeConditional("CONDITION")]
            public extern SomeStruct FailingMethodReturningSomeStruct();

            [MethodImpl(MethodImplOptions.InternalCall)]
            [NativeConditional("CONDITION", "SomeStruct()")]
            public extern SomeStruct MethodReturningSomeStructWithExplicitStub();

            [MethodImpl(MethodImplOptions.InternalCall)]
            [NativeConditional("CONDITION")]
            public extern IntPtr MethodReturningIntPtr();

            [MethodImpl(MethodImplOptions.InternalCall)]
            [NativeConditional("CONDITION")]
            public extern int MethodReturningInt();

            [MethodImpl(MethodImplOptions.InternalCall)]
            [NativeConditional("CONDITION")]
            public extern void MethodReturningVoid();

            [MethodImpl(MethodImplOptions.InternalCall)]
            [NativeConditional("CONDITION")]
            public extern SomeEnum MethodReturningEnum();
        }

        [Test]
        public void VerifyStubReturnTypeErrors()
        {
            var errors = AssemblyVerifier.Verify(TypeResolver.TypeDefinitionFor(typeof(StubReturnTypeTestClass))).Errors;
            Assert.IsTrue(errors.All(e => e.ErrorNo == AssemblyVerifier.Error.Type.KStubHasIncompatibleReturnType && e.SourceName.Contains("::FailingMethod")));
            Assert.AreEqual(1, errors.Count);
        }

        class OverloadedMethods
        {
            [MethodImpl(MethodImplOptions.InternalCall)]
            public static extern void SomeMethod();

            [MethodImpl(MethodImplOptions.InternalCall)]
            public static extern void SomeOverloadedMethod(int a);

            [MethodImpl(MethodImplOptions.InternalCall)]
            public static extern void SomeOverloadedMethod(float b);
        }

        [Test]
        public void VerifyMethodOverloadIsError()
        {
            var errors = AssemblyVerifier.Verify(TypeResolver.TypeDefinitionFor(typeof(OverloadedMethods))).Errors;
            Assert.IsTrue(errors.All(e => e.ErrorNo == AssemblyVerifier.Error.Type.KMethodOverloadNotSupported));
            Assert.AreEqual(1, errors.Count);
        }

        class ExternIndexedProperty : UnityEngine.Object
        {
            public extern char this[int index]
            {
                [MethodImpl(MethodImplOptions.InternalCall)]
                get;
                [MethodImpl(MethodImplOptions.InternalCall)]
                set;
            }
        }

        [Test]
        public void VerifyIndexedPropertyIsError()
        {
            var errors = AssemblyVerifier.Verify(TypeResolver.TypeDefinitionFor(typeof(ExternIndexedProperty))).Errors;
            Assert.IsTrue(errors.All(e => e.ErrorNo == AssemblyVerifier.Error.Type.KIndexedPropertyNotSupported));
            Assert.AreEqual(1, errors.Count);
        }

        class WritableAttributeTest
        {
            [MethodImpl(MethodImplOptions.InternalCall)]
            public static extern void SomeMethod([Writable] int fail, [Writable] UnityEngineObjectInheritors.Dummy fail2, [Writable] UnityEngineObjectInheritors.InheritingDummy success);
        }

        [Test]
        public void VerifyWritableAttributeOnNonUnityEngineObjectIsError()
        {
            var errors = AssemblyVerifier.Verify(TypeResolver.TypeDefinitionFor(typeof(WritableAttributeTest))).Errors;
            Assert.IsTrue(errors.All(e => e.ErrorNo == AssemblyVerifier.Error.Type.KNonSensicalWritableAttribute));
            Assert.AreEqual(2, errors.Count);
            Assert.AreEqual(1, errors.Count(e => e.MsgExtenders[0] == "fail"));
            Assert.AreEqual(1, errors.Count(e => e.MsgExtenders[0] == "fail2"));
        }

        class ListReturner
        {
            [MethodImpl(MethodImplOptions.InternalCall)]
            public static extern List<int> SomeMethod();
        }

        [Test]
        public void VerifyListReturnIsError()
        {
            var errors = AssemblyVerifier.Verify(TypeResolver.TypeDefinitionFor(typeof(ListReturner))).Errors;
            Assert.IsTrue(errors.All(e => e.ErrorNo == AssemblyVerifier.Error.Type.KUnsupportedListReturn));
            Assert.AreEqual(1, errors.Count);
        }

        class StaticAccessorAppliedToInstanceMethod : UnityEngine.Object
        {
            [MethodImpl(MethodImplOptions.InternalCall)]
            [StaticAccessor("DoesntMatter")]
            public extern void InstanceMethod();

            [MethodImpl(MethodImplOptions.InternalCall)]
            [StaticAccessor("DoesntMatter")]
            public static extern void StaticMethod(); // Used to ensure no "false positives" are reported.
        }

        [Test]
        public void VerifyStaticAccessorAppliedToInstanceMethodIsError()
        {
            var errors = AssemblyVerifier.Verify(TypeResolver.TypeDefinitionFor(typeof(StaticAccessorAppliedToInstanceMethod))).Errors;
            Assert.IsTrue(errors.All(e => e.ErrorNo == AssemblyVerifier.Error.Type.KStaticAccessorOnInstanceMethod));
            Assert.AreEqual(1, errors.Count);
        }

        class StaticAccessorAppliedToNonExternMethod : UnityEngine.Object
        {
            [StaticAccessor("DoesntMatter")]
            public void NonExternMethod() {}
        }

        [Test]
        public void VerifyStaticAccessorAppliedToNonExternMethodIsError()
        {
            var errors = AssemblyVerifier.Verify(TypeResolver.TypeDefinitionFor(typeof(StaticAccessorAppliedToNonExternMethod))).Errors;
            Assert.IsTrue(errors.All(e => e.ErrorNo == AssemblyVerifier.Error.Type.KStaticAccessorOnNonExternMethod));
            Assert.AreEqual(1, errors.Count);
        }

        [Test]
        public void VerifyExternPropertyInStructIsNotErrorInEditorButErrorInPlayer()
        {
            // Extern properties in structs will not compile in .NET (that is the whole point of this verification step)
            // So we build using Mono
            var assemblyPath = CommonTools.CSharpCompiler.CompileAssemblyFromSource(@"
                using System.Runtime.CompilerServices;
                struct BlittableStructWithExternProperty
                {
                    int a;

                    private extern int property { [MethodImpl(MethodImplOptions.InternalCall)]get; [MethodImpl(MethodImplOptions.InternalCall)]set; }

                }
            ");

            var assembly = TypeResolver.LoadAssembly(assemblyPath);
            var typeDefinition = assembly.MainModule.GetType("BlittableStructWithExternProperty");

            GlobalContext.SetDefine("UNITY_EDITOR");
            var errors = AssemblyVerifier.Verify(typeDefinition).Errors;
            Assert.AreEqual(0, errors.Count);

            GlobalContext.UnsetDefine("UNITY_EDITOR");
            errors = AssemblyVerifier.Verify(typeDefinition).Errors;
            Assert.AreEqual(1, errors.Count);
            Assert.AreEqual(AssemblyVerifier.Error.Type.KExternPropertyInStructInPlayer, errors[0].ErrorNo);
        }

        public class FieldBoundPropertyWithThrowsOnProp
        {
            [NativeThrows]
            [NativeProperty(TargetType = TargetType.Field)]
            public static extern int WithThrows
            {
                [MethodImpl(MethodImplOptions.InternalCall)]
                get;
                [MethodImpl(MethodImplOptions.InternalCall)]
                set;
            }
        }

        public class FieldBoundPropertyWithThrowsOnGet
        {
            [NativeProperty(TargetType = TargetType.Field)]
            public static extern int WithThrows
            {
                [MethodImpl(MethodImplOptions.InternalCall)]
                [NativeThrows]
                get;
                [MethodImpl(MethodImplOptions.InternalCall)]
                set;
            }
        }

        public class FieldBoundPropertyWithThrowsOnSet
        {
            [NativeProperty(TargetType = TargetType.Field)]
            public static extern int WithThrows
            {
                [MethodImpl(MethodImplOptions.InternalCall)]
                get;
                [MethodImpl(MethodImplOptions.InternalCall)]
                [NativeThrows]
                set;
            }
        }

        [Test]
        [TestCase(typeof(FieldBoundPropertyWithThrowsOnProp))]
        [TestCase(typeof(FieldBoundPropertyWithThrowsOnGet))]
        [TestCase(typeof(FieldBoundPropertyWithThrowsOnSet))]
        public void VerifyNativeThrowsOnFieldBoundPropertyIsError(Type type)
        {
            var errors = AssemblyVerifier.Verify(TypeResolver.TypeDefinitionFor(type)).Errors;
            Assert.AreEqual(1, errors.Count);
            Assert.IsTrue(errors.All(e => e.ErrorNo == AssemblyVerifier.Error.Type.KThrowsOnFieldTargetingProperty));
        }

        public class MarshallableParameter : UnityEngine.Object
        {
            [MethodImpl(MethodImplOptions.InternalCall)]
            public extern void ArrayOfNonPodParameter(string[] array);
            [MethodImpl(MethodImplOptions.InternalCall)]
            public extern void NullablePrimitive(int? nullableInt);

            public delegate void Dg();
            [MethodImpl(MethodImplOptions.InternalCall)]
            public extern void DelegateParameter(Dg dg);
        }

        [Test]
        public void VerifyArrayParameter()
        {
            var errors = AssemblyVerifier.Verify(MethodDefinitionFor(typeof(MarshallableParameter), nameof(MarshallableParameter.ArrayOfNonPodParameter))).Errors;
            Assert.AreEqual(0, errors.Count);
        }

        [Test]
        public void VerifyNullablePrimitiveParameter()
        {
            var errors = AssemblyVerifier.Verify(MethodDefinitionFor(typeof(MarshallableParameter), nameof(MarshallableParameter.NullablePrimitive))).Errors;
            Assert.AreEqual(0, errors.Count);
        }

        [Test]
        public void VerifyDelegateParameter()
        {
            var errors = AssemblyVerifier.Verify(MethodDefinitionFor(typeof(MarshallableParameter), nameof(MarshallableParameter.DelegateParameter))).Errors;
            Assert.AreEqual(0, errors.Count);
        }

        [NativeType("MyNativeStruct")]
        [NativeAsStruct]
        public class ClassToStruct
        {
        }

        [Test]
        public void VerifyNativeAsStructOnNonSequentialIsError()
        {
            var errors = AssemblyVerifier.Verify(TypeResolver.TypeDefinitionFor(typeof(ClassToStruct))).Errors;
            Assert.AreEqual(1, errors.Count);
            Assert.IsTrue(errors.All(e => e.ErrorNo == AssemblyVerifier.Error.Type.KNativeAsStructRequiresExplicitLayout));
        }

        public class FieldBoundProperties
        {
            [NativeProperty(TargetType = TargetType.Field)]
            private static extern int getOnlyPropWithNativeProperty
            {
                [MethodImpl(MethodImplOptions.InternalCall)]
                get;
            }
            [NativeProperty(TargetType = TargetType.Field)]
            private static extern int setOnlyPropWithNativeProperty
            {
                [MethodImpl(MethodImplOptions.InternalCall)]
                set;
            }
        }

        [Test]
        public void VerifyFieldBoundPropertyIsNotError()
        {
            var errors = AssemblyVerifier.Verify(TypeResolver.TypeDefinitionFor(typeof(FieldBoundProperties))).Errors;
            Assert.AreEqual(0, errors.Count);
        }

        [NativeType]
        abstract class AbstractClass
        {
            [MethodImpl(MethodImplOptions.InternalCall)]
            public static extern void Method();
        }
        [Test]
        public void AbstractClassesAreOk()
        {
            var errors = AssemblyVerifier.Verify(TypeResolver.TypeDefinitionFor(typeof(AbstractClass))).Errors;

            Assert.AreEqual(0, errors.Count);
        }

        public static class OutEnumArray
        {
            [MethodImpl(MethodImplOptions.InternalCall)]
            extern static void OutEnumArrayMethod([Out] SomeEnum[] param);
        }

        public static class OutEnumArrayNonExtern
        {
            static void OutEnumArrayMethod([Out] SomeEnum[] param)
            {
            }
        }

        [Test]
        public void VerifyOutEnumArrayIsErrorOnDotNet()
        {
            var errors = AssemblyVerifier.Verify(TypeResolver.TypeDefinitionFor(typeof(OutEnumArray)), ScriptingBackend.DotNet).Errors;
            Assert.AreEqual(1, errors.Count);
            Assert.IsTrue(errors.All(e => e.ErrorNo == AssemblyVerifier.Error.Type.KOutEnumNotSupportedOnDotNet));
        }

        [Test]
        public void VerifyOutEnumArrayInNonExternIsNotErrorOnDotNet()
        {
            var errors = AssemblyVerifier.Verify(TypeResolver.TypeDefinitionFor(typeof(OutEnumArrayNonExtern)), ScriptingBackend.DotNet).Errors;
            Assert.AreEqual(0, errors.Count);
        }

        public void CheckError(List<Error> errors, Error.Type type, string sourceName, string memberName)
        {
            var i = errors.FindIndex(e => e.ErrorNo == type && e.SourceName == sourceName && e.MsgExtenders.Contains(memberName));
            Assert.IsTrue(i != -1, "Did not find expected error " + type + ":" + sourceName);
            errors.RemoveAt(i);
        }

        [Test]
        public void VerifyAssemblyDependencyValidator()
        {
            var t = TypeResolver.TypeDefinitionFor(typeof(Unity.BindingsGenerator.TestDependentAssembly.MyClass));
            var errors = AssemblyVerifier.Verify(t.Module.Assembly).Errors;

            CheckError(errors, Error.Type.KCannotUseTypeInternalToOtherAssembly,
                "System.Void Unity.BindingsGenerator.TestDependentAssembly.MyClass::.ctor()", "Unity.BindingsGenerator.TestDependencyAssembly.InternalClass");
            CheckError(errors, Error.Type.KCannotUseTypeInternalToOtherAssemblyNotAllowedByAttribute,
                "System.Void Unity.BindingsGenerator.TestDependentAssembly.MyClass::.ctor()", "Unity.BindingsGenerator.TestDependencyAssembly.InternalClassWithAttributeSpecifyingWrongModule");
            CheckError(errors, Error.Type.KCannotUseTypeInternalToOtherAssembly,
                "System.Void Unity.BindingsGenerator.TestDependentAssembly.MyClass::.ctor()", "System.Void Unity.BindingsGenerator.TestDependencyAssembly.PublicClass::InternalMethod()");
            CheckError(errors, Error.Type.KCannotUseTypeInternalToOtherAssemblyNotAllowedByAttribute,
                "System.Void Unity.BindingsGenerator.TestDependentAssembly.MyClass::.ctor()", "System.Void Unity.BindingsGenerator.TestDependencyAssembly.PublicClass::InternalMethodWithAttributeSpecifyingWrongModule()");
            CheckError(errors, Error.Type.KCannotUseTypeInternalToOtherAssembly,
                "System.Void Unity.BindingsGenerator.TestDependentAssembly.MyClass::.ctor()", "System.Int32 Unity.BindingsGenerator.TestDependencyAssembly.PublicClass::InternalField");
            CheckError(errors, Error.Type.KCannotUseTypeInternalToOtherAssemblyNotAllowedByAttribute,
                "System.Void Unity.BindingsGenerator.TestDependentAssembly.MyClass::.ctor()", "System.Int32 Unity.BindingsGenerator.TestDependencyAssembly.PublicClass::InternalFieldWithAttributeSpecifyingWrongModule");
            CheckError(errors, Error.Type.KCannotUseTypeInternalToOtherAssembly,
                "System.Void Unity.BindingsGenerator.TestDependentAssembly.MyClass::.ctor()", "System.Void Unity.BindingsGenerator.TestDependencyAssembly.InternalClassWithAttribute::InternalMethod()");
            CheckError(errors, Error.Type.KCannotUseTypeInternalToOtherAssembly,
                "System.Void Unity.BindingsGenerator.TestDependentAssembly.MyClass::.ctor()", "System.Int32 Unity.BindingsGenerator.TestDependencyAssembly.InternalClassWithAttribute::InternalField");
            CheckError(errors, Error.Type.KCannotUseTypeInternalToOtherAssembly,
                "System.Void Unity.BindingsGenerator.TestDependentAssembly.MyClass::.ctor()", "System.Void Unity.BindingsGenerator.TestDependencyAssembly.PublicClass::set_PublicPropertyWithInternalSetter(System.Int32)");
            CheckError(errors, Error.Type.KCannotUseTypeInternalToOtherAssembly,
                "System.Void Unity.BindingsGenerator.TestDependentAssembly.MyClass::.ctor()", "System.Void Unity.BindingsGenerator.TestDependencyAssembly.PublicClass::set_PublicPropertyWithInternalSetterWithAttributeOnProperty(System.Int32)");
            CheckError(errors, Error.Type.KCannotUseTypeInternalToOtherAssembly,
                "System.Void Unity.BindingsGenerator.TestDependentAssembly.MyClass::.ctor()", "System.Void Unity.BindingsGenerator.TestDependencyAssembly.PublicClass::set_InternalProperty(System.Int32)");
            CheckError(errors, Error.Type.KCannotUseTypeInternalToOtherAssembly,
                "System.Void Unity.BindingsGenerator.TestDependentAssembly.MyClass::.ctor()", "Unity.BindingsGenerator.TestDependencyAssembly.InternalEnum");
            CheckError(errors, Error.Type.KCannotUseTypeInternalToOtherAssembly,
                "System.Void Unity.BindingsGenerator.TestDependentAssembly.MyClass::.ctor()", "Unity.BindingsGenerator.TestDependencyAssembly.InternalInterface");
            CheckError(errors, Error.Type.KCannotUseTypeInternalToOtherAssembly,
                "System.Void Unity.BindingsGenerator.TestDependentAssembly.MyClass::.ctor()", "Unity.BindingsGenerator.TestDependencyAssembly.InternalClass/PublicNestedClass");
            CheckError(errors, Error.Type.KCannotUseTypeInternalToOtherAssembly,
                "System.Void Unity.BindingsGenerator.TestDependentAssembly.MyClass::.ctor()", "Unity.BindingsGenerator.TestDependencyAssembly.InternalClass/InternalNestedClass");
            CheckError(errors, Error.Type.KCannotUseTypeInternalToOtherAssembly,
                "System.Void Unity.BindingsGenerator.TestDependentAssembly.MyClass::.ctor()", "Unity.BindingsGenerator.TestDependencyAssembly.InternalClass/InternalNestedClassWithAttribute");
            CheckError(errors, Error.Type.KCannotUseTypeInternalToOtherAssembly,
                "System.Void Unity.BindingsGenerator.TestDependentAssembly.MyClass::.ctor()", "Unity.BindingsGenerator.TestDependencyAssembly.PublicClass/InternalNestedClass");
            CheckError(errors, Error.Type.KCannotUseTypeInternalToOtherAssembly,
                "System.Void Unity.BindingsGenerator.TestDependentAssembly.MyClass::.ctor()", "Unity.BindingsGenerator.TestDependencyAssembly.InternalClassWithAttribute/InternalNestedClass");

            CheckError(errors, Error.Type.KCannotUseTypeInternalToOtherAssembly,
                "Unity.BindingsGenerator.TestDependentAssembly.MyClass", "Unity.BindingsGenerator.TestDependencyAssembly.InternalClass2");
            CheckError(errors, Error.Type.KCannotUseTypeInternalToOtherAssembly,
                "System.Void Unity.BindingsGenerator.TestDependentAssembly.MyClass::GenericMethod()", "Unity.BindingsGenerator.TestDependencyAssembly.InternalClass3");
            CheckError(errors, Error.Type.KCannotUseTypeInternalToOtherAssembly,
                "System.Void Unity.BindingsGenerator.TestDependentAssembly.MyClass::.ctor()", "Unity.BindingsGenerator.TestDependencyAssembly.InternalClass4");

            Assert.IsEmpty(errors);
        }
    }
}
