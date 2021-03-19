using System;
using System.Linq;
using System.Runtime.CompilerServices;
using System.Runtime.InteropServices;
using NUnit.Framework;
using Unity.BindingsGenerator.Core;
using UnityEngine.Bindings;
using Unity.BindingsGenerator.Core.Nodes;
using Unity.BindingsGenerator.TestFramework;
using UnityEngine;

namespace Unity.BindingsGenerator.Tests
{
    [TestFixture]
    class StaticMethodTests : BindingsGeneratorTestsBase
    {
        class Empty : UnityEngine.Object
        {
            public Empty()
            {
            }

            public void AnotherMethod()
            {
            }
        }

        static class StaticClass
        {
            [MethodImpl(MethodImplOptions.InternalCall)]
            public static extern void Test();
        }

        class SimpleClass
        {
            [MethodImpl(MethodImplOptions.InternalCall)]
            public static extern void Test();
        }

        class MethodNameOverride
        {
            [MethodImpl(MethodImplOptions.InternalCall)]
            [NativeMethod(Name = "Bar")]
            public static extern void Test();
        }

        class FreeFunctionOverride
        {
            [MethodImpl(MethodImplOptions.InternalCall)]
            [NativeMethod(IsFreeFunction = true, Name = "SomeFreeFunction")]
            public static extern void Test();
        }

        class ParentOfNested
        {
            public class Nested
            {
                [MethodImpl(MethodImplOptions.InternalCall)]
                [NativeMethod(Name = "Bar")]
                public static extern void Test();
            }
        }

        abstract class AbstractClass
        {
            [MethodImpl(MethodImplOptions.InternalCall)]
            public static extern void Test();
        }

        [UnmanagedFunctionPointerAttribute(CallingConvention.Cdecl)]
        delegate void SomeDelegateFunctionPointer(int a);

        delegate void SomeDelegate(int a);

        interface IInterface
        {
        }

        class ParameterBinding
        {
            [MethodImpl(MethodImplOptions.InternalCall)]
            public static extern void TestBool(bool b);

            [MethodImpl(MethodImplOptions.InternalCall)]
            public static extern void TestChar(char c);

            [MethodImpl(MethodImplOptions.InternalCall)]
            public static extern void TestSignedIntegers(sbyte sb, short s, int i, long l);

            [MethodImpl(MethodImplOptions.InternalCall)]
            public static extern void TestUnsignedIntegers(byte b, ushort s, uint i, ulong l);

            [MethodImpl(MethodImplOptions.InternalCall)]
            public static extern void TestReals(float f, double d);

            public enum Test
            {
                value1,
                value2,
            };

            [MethodImpl(MethodImplOptions.InternalCall)]
            public static extern void TestEnum(Test e);

            [MethodImpl(MethodImplOptions.InternalCall)]
            public static extern void TestString(string s);

            [MethodImpl(MethodImplOptions.InternalCall)]
            public static extern void TestIntArray(int[] ints);

            [MethodImpl(MethodImplOptions.InternalCall)]
            public static extern void TestStringArray(string[] strings);

            [MethodImpl(MethodImplOptions.InternalCall)]
            public static extern void TestClassArgument(Empty classArgument, [Writable] Empty writableObjectArgument);

            [MethodImpl(MethodImplOptions.InternalCall)]
            public static extern void TestUnmarshalledClassArgument([Unmarshalled] Empty classArgument);

            [MethodImpl(MethodImplOptions.InternalCall)]
            public static extern void TestSystemArray(Array param);

            [MethodImpl(MethodImplOptions.InternalCall)]
            public static extern void TestIntPtr(IntPtr param);

            [MethodImpl(MethodImplOptions.InternalCall)]
            public static extern void TestUIntPtr(UIntPtr param);

            [MethodImpl(MethodImplOptions.InternalCall)]
            public static extern void TestDelegate(SomeDelegateFunctionPointer param, SomeDelegate param2);
        }

        class ParameterMarshalling
        {
            [MethodImpl(MethodImplOptions.InternalCall)]
            public static extern void TestBool(bool param);

            [MethodImpl(MethodImplOptions.InternalCall)]
            public static extern void TestInt(int param);

            [MethodImpl(MethodImplOptions.InternalCall)]
            public static extern void TestFloat(float param);

            public enum Test
            {
                value1,
                value2,
            };

            [MethodImpl(MethodImplOptions.InternalCall)]
            public static extern void TestEnum(Test param);

            [MethodImpl(MethodImplOptions.InternalCall)]
            public static extern void TestString(string param);

            [MethodImpl(MethodImplOptions.InternalCall)]
            public static extern void TestIntArray(int[] param);

            [MethodImpl(MethodImplOptions.InternalCall)]
            public static extern void TestStringArray(string[] param);

            [MethodImpl(MethodImplOptions.InternalCall)]
            public static extern void TestClassArgument(Empty param);

            [MethodImpl(MethodImplOptions.InternalCall)]
            public static extern void TestUnmarshalledClassArgument([Unmarshalled] Empty param);

            [MethodImpl(MethodImplOptions.InternalCall)]
            public static extern void TestWritableObjectArgument([Writable] Empty param);

            [MethodImpl(MethodImplOptions.InternalCall)]
            public static extern void TestArgumentCalledSelf(Empty self);

            [MethodImpl(MethodImplOptions.InternalCall)]
            public static extern void TestSystemArray(Array param);

            [MethodImpl(MethodImplOptions.InternalCall)]
            public static extern void TestIntPtr(IntPtr param);

            [MethodImpl(MethodImplOptions.InternalCall)]
            public static extern void TestUIntPtr(UIntPtr param);

            [MethodImpl(MethodImplOptions.InternalCall)]
            public static extern void TestUnmarshalledString([Unmarshalled] string param);

            [MethodImpl(MethodImplOptions.InternalCall)]
            public static extern void TestDelegate(SomeDelegate param);

            [MethodImpl(MethodImplOptions.InternalCall)]
            public static extern void TestDelegateFunctionPointer(SomeDelegateFunctionPointer param);

            [MethodImpl(MethodImplOptions.InternalCall)]
            public static extern void TestInterface(IInterface param);
        }

        class ReturnValueBinding
        {
            [MethodImpl(MethodImplOptions.InternalCall)]
            public static extern bool TestBool();

            [MethodImpl(MethodImplOptions.InternalCall)]
            public static extern int TestInt();

            [MethodImpl(MethodImplOptions.InternalCall)]
            public static extern float TestFloat();

            [MethodImpl(MethodImplOptions.InternalCall)]
            public static extern string TestString();

            [MethodImpl(MethodImplOptions.InternalCall)]
            public static extern int[] TestIntArray();

            [MethodImpl(MethodImplOptions.InternalCall)]
            public static extern string[] TestStringArray();

            [MethodImpl(MethodImplOptions.InternalCall)]
            public static extern Empty TestClassReturn();

            [MethodImpl(MethodImplOptions.InternalCall)]
            public static extern Array TestSystemArrayReturn();

            [MethodImpl(MethodImplOptions.InternalCall)]
            public static extern IntPtr TestIntPtrReturn();

            [MethodImpl(MethodImplOptions.InternalCall)]
            public static extern UIntPtr TestUIntPtrReturn();
        }

        class ReturnValueMarshalling
        {
            [MethodImpl(MethodImplOptions.InternalCall)]
            public static extern bool TestBool();

            [MethodImpl(MethodImplOptions.InternalCall)]
            public static extern int TestInt();

            [MethodImpl(MethodImplOptions.InternalCall)]
            public static extern float TestFloat();

            public enum Test
            {
                value1,
                value2,
            };

            [MethodImpl(MethodImplOptions.InternalCall)]
            public static extern Test TestEnum();

            [MethodImpl(MethodImplOptions.InternalCall)]
            public static extern string TestString();

            [MethodImpl(MethodImplOptions.InternalCall)]
            public static extern int[] TestIntArray();

            [MethodImpl(MethodImplOptions.InternalCall)]
            public static extern string[] TestStringArray();

            [MethodImpl(MethodImplOptions.InternalCall)]
            public static extern Empty TestClassReturn();

            [MethodImpl(MethodImplOptions.InternalCall)]
            public static extern Empty[] TestArrayOfClassTypeReturn();

            [MethodImpl(MethodImplOptions.InternalCall)]
            public static extern Array TestSystemArrayReturn();

            [MethodImpl(MethodImplOptions.InternalCall)]
            public static extern IntPtr TestIntPtrReturn();

            [MethodImpl(MethodImplOptions.InternalCall)]
            public static extern UIntPtr TestUIntPtrReturn();

            [MethodImpl(MethodImplOptions.InternalCall)]
            public static extern IInterface TestInterfaceReturn();
        }

        struct StaticMethodStruct
        {
            [MethodImpl(MethodImplOptions.InternalCall)]
            public static extern void TestInt(int param);

            [MethodImpl(MethodImplOptions.InternalCall)]
            public static extern int TestIntReturn();
        }

        class ThreadSafe
        {
            [MethodImpl(MethodImplOptions.InternalCall)]
            [NativeMethod(IsThreadSafe = true)]
            public static extern void Test();
        }

        [Test(Description = "Test that no matter what kind of override we use the binding function name remains the same")]
        [TestCase(typeof(SimpleClass), nameof(SimpleClass.Test))]
        [TestCase(typeof(StaticClass), nameof(StaticClass.Test))]
        [TestCase(typeof(MethodNameOverride), nameof(MethodNameOverride.Test))]
        [TestCase(typeof(FreeFunctionOverride), nameof(FreeFunctionOverride.Test))]
        public void BindingFunctionNameWorks(Type type, string methodName)
        {
            Assert.IsNotNull(FunctionNodeForMethod(type, methodName));
        }

        [Test(Description = "Test that we call the right native static function name based on C# name + overrides")]
        public void NameOverrideWorks()
        {
            var functionNode = FunctionNodeForMethod(
                typeof(MethodNameOverride), nameof(MethodNameOverride.Test));
            var expectation = new ExpressionStatementNode(
                new FunctionCallNode(
                    "Bar",
                    new StaticMemberQualifier(typeof(MethodNameOverride).RegisteredName())));

            CollectionAssert.Contains(functionNode.Statements, expectation);
        }

        [Test(Description = "Test that we call the right free function based on name override")]
        public void FreeFunctionOverrideWorks()
        {
            var functionNode = FunctionNodeForMethod(
                typeof(FreeFunctionOverride), nameof(FreeFunctionOverride.Test));
            var expectation =
                new ExpressionStatementNode(new FunctionCallNode("SomeFreeFunction", FreeQualifier.Instance));

            CollectionAssert.Contains(functionNode.Statements, expectation);
        }

        [Test(Description = "Test that the names and types of the parameters for the binding function are as we expect")]
        [TestCase(typeof(StaticClass), nameof(StaticClass.Test), new string[] {}, new string[] {})]
        [TestCase(typeof(ParameterBinding), nameof(ParameterBinding.TestBool), new[] {"ScriptingBool"}, new[] {"b"})]
        [TestCase(typeof(ParameterBinding), nameof(ParameterBinding.TestChar), new[] {"SInt16"}, new[] {"c"})]
        [TestCase(typeof(ParameterBinding), nameof(ParameterBinding.TestSignedIntegers), new[] {"SInt8", "SInt16", "SInt32", "SInt64"}, new[] {"sb", "s", "i", "l"})]
        [TestCase(typeof(ParameterBinding), nameof(ParameterBinding.TestUnsignedIntegers), new[] {"UInt8", "UInt16", "UInt32", "UInt64"}, new[] {"b", "s", "i", "l"})]
        [TestCase(typeof(ParameterBinding), nameof(ParameterBinding.TestReals), new[] {"float", "double"}, new[] {"f", "d"})]
        [TestCase(typeof(ParameterBinding), nameof(ParameterBinding.TestString), new[] {"ICallType_String_Argument"}, new[] {"s"})]
        [TestCase(typeof(ParameterBinding), nameof(ParameterBinding.TestIntArray), new[] {"ICallType_Array_Argument"}, new[] {"ints"})]
        [TestCase(typeof(ParameterBinding), nameof(ParameterBinding.TestStringArray), new[] {"ICallType_Array_Argument"}, new[] {"strings"})]
        [TestCase(typeof(ParameterBinding), nameof(ParameterBinding.TestSystemArray), new[] {"ICallType_Array_Argument"}, new[] {"param"})]
        [TestCase(typeof(ParameterBinding), nameof(ParameterBinding.TestIntPtr), new[] {"ICallType_IntPtr_Argument"}, new[] {"param"})]
        [TestCase(typeof(ParameterBinding), nameof(ParameterBinding.TestUIntPtr), new[] {"ICallType_IntPtr_Argument"}, new[] {"param"})]
        [TestCase(typeof(ParameterBinding), nameof(ParameterBinding.TestDelegate), new[] {"void*", "ICallType_Object_Argument"}, new[] {"param", "param2"})]
        [TestCase(typeof(ParameterBinding), nameof(ParameterBinding.TestDelegate), new[] {"void*", "ICallType_Object_Argument"}, new[] {"param", "param2"})]
        [TestCase(typeof(StaticMethodStruct), nameof(StaticMethodStruct.TestInt), new[] {"SInt32"}, new[] {"param"})]
        public void ParametersBindingWorks(Type type, string methodName, string[] argumentTypes, string[] argumentNames)
        {
            var functionNode = FunctionNodeForMethod(type, methodName);
            var parameters = argumentTypes.Select((t, i) => new FunctionParameter(argumentNames[i], t));

            CollectionAssert.AreEqual(parameters, functionNode.Parameters);
        }

        [Test(Description = "Test that the names and types of the parameters for the binding function are as we expect")]
        [TestCase(typeof(ParameterBinding), nameof(ParameterBinding.TestClassArgument), new[] {"ICallType_ReadOnlyUnityEngineObject_Argument", "ICallType_Object_Argument"}, new[] {"classArgument", "writableObjectArgument"}, new[] {TypeUsage.Parameter, TypeUsage.WritableParameter})]
        [TestCase(typeof(ParameterBinding), nameof(ParameterBinding.TestUnmarshalledClassArgument), new[] {"ICallType_Object_Argument"}, new[] {"classArgument"}, new[] {TypeUsage.UnmarshalledParameter})]
        public void ParametersBindingWorks(Type type, string methodName, string[] argumentTypes, string[] argumentNames, TypeUsage[] argumentUsages)
        {
            var functionNode = FunctionNodeForMethod(type, methodName);
            var parameters = argumentTypes.Select((t, i) => new FunctionParameter(argumentNames[i], t)
            {
                Usage = argumentUsages[i]
            });

            CollectionAssert.AreEqual(parameters, functionNode.Parameters);
        }

        [Test(Description = "Test that the names and types of the parameters for the binding function are as we expect")]
        [TestCase(typeof(ParameterBinding), nameof(ParameterBinding.TestEnum), new[] { "ICallType_Generated_Unity_BindingsGenerator_Tests_StaticMethodTests_ParameterBinding_Test" }, new[] {"e"}, typeof(ParameterBinding.Test))]
        public void ParametersBindingWorks(Type type, string methodName, string[] argumentTypes, string[] argumentNames, Type typeExpectingRegistration)
        {
            FileNode fileNode;
            var functionNode = FunctionNodeForMethod(type, methodName, out fileNode);
            var parameters = argumentTypes.Select((t, i) => new FunctionParameter(argumentNames[i], t));

            CollectionAssert.AreEqual(parameters, functionNode.Parameters);

            var registrationNamespace = fileNode.Namespaces.Single(n => n.Name == Naming.RegistrationChecksNamespace);
            var expectation = new TypeDefNode
            {
                SourceType = typeExpectingRegistration.RegisteredName(),
                DestType = Naming.VerifyBindingNameFor(typeExpectingRegistration.FullnameWithUnderscores())
            };

            CollectionAssert.Contains(registrationNamespace.TypeDefs, expectation);
        }

        [Test(Description = "Test that we call the marshal functions and convert the type correctly before passing it to the native function")]
        [TestCase(typeof(ParameterMarshalling), nameof(ParameterMarshalling.TestBool), "param")]
        [TestCase(typeof(ParameterMarshalling), nameof(ParameterMarshalling.TestInt), "param")]
        [TestCase(typeof(ParameterMarshalling), nameof(ParameterMarshalling.TestFloat), "param")]
        [TestCase(typeof(ParameterMarshalling), nameof(ParameterMarshalling.TestEnum), "param")]
        [TestCase(typeof(StaticMethodStruct), nameof(StaticMethodStruct.TestInt), "param")]
        public void ParameterMarshallingWorksForParamsThatDoNotRequireMarshalling(Type type, string methodName, string paramName)
        {
            var functionNode = FunctionNodeForMethod(type, methodName);
            var callExpectation = new ExpressionStatementNode(
                new FunctionCallNode(methodName, new StaticMemberQualifier(type.RegisteredName()), paramName));

            CollectionAssert.Contains(functionNode.Statements, callExpectation);
        }

        [Test(Description = "Test that we call the marshal functions and convert the type correctly before passing it to the native function")]
        [TestCase(typeof(ParameterMarshalling), nameof(ParameterMarshalling.TestString), "Marshalling::StringMarshaller", "param")]
        [TestCase(typeof(ParameterMarshalling), nameof(ParameterMarshalling.TestIntArray), "Marshalling::ArrayMarshaller<SInt32>", "param")]
        [TestCase(typeof(ParameterMarshalling), nameof(ParameterMarshalling.TestStringArray), "Marshalling::ArrayMarshaller<Marshalling::StringArrayElement>", "param")]
        [TestCase(typeof(ParameterMarshalling), nameof(ParameterMarshalling.TestClassArgument), "Marshalling::ReadOnlyUnityObjectMarshaller<ICallType_Generated_Unity_BindingsGenerator_Tests_StaticMethodTests_Empty>", "param")]
        [TestCase(typeof(ParameterMarshalling), nameof(ParameterMarshalling.TestUnmarshalledClassArgument), "Marshalling::ManagedObjectMarshaller", "param")]
        [TestCase(typeof(ParameterMarshalling), nameof(ParameterMarshalling.TestWritableObjectArgument), "Marshalling::UnityObjectMarshaller<ICallType_Generated_Unity_BindingsGenerator_Tests_StaticMethodTests_Empty>", "param")]
        [TestCase(typeof(ParameterMarshalling), nameof(ParameterMarshalling.TestArgumentCalledSelf), "Marshalling::ReadOnlyUnityObjectMarshaller<ICallType_Generated_Unity_BindingsGenerator_Tests_StaticMethodTests_Empty>", "self")]
        [TestCase(typeof(ParameterMarshalling), nameof(ParameterMarshalling.TestSystemArray), "Marshalling::SystemArrayMarshaller", "param")]
        [TestCase(typeof(ParameterMarshalling), nameof(ParameterMarshalling.TestIntPtr), "Marshalling::IntPtrMarshaller", "param")]
        [TestCase(typeof(ParameterMarshalling), nameof(ParameterMarshalling.TestUIntPtr), "Marshalling::IntPtrMarshaller", "param")]
        [TestCase(typeof(ParameterMarshalling), nameof(ParameterMarshalling.TestUnmarshalledString), "ICallType_String_Argument", "param")]
        [TestCase(typeof(ParameterMarshalling), nameof(ParameterMarshalling.TestDelegate), "Marshalling::ManagedObjectMarshaller", "param")]
        [TestCase(typeof(ParameterMarshalling), nameof(ParameterMarshalling.TestDelegateFunctionPointer), "Marshalling::UntypedPtrMarshaller", "param")]
        [TestCase(typeof(ParameterMarshalling), nameof(ParameterMarshalling.TestInterface), "Marshalling::ManagedObjectMarshaller", "param")]
        public void ParameterMarshallingWorksForParamsThatRequireMarshalling(Type type, string methodName, string expectedType, string paramName)
        {
            var functionNode = FunctionNodeForMethod(type, methodName);
            var declarationExpectation = new VariableDeclarationStatementNode(paramName.Marshalled(), expectedType);
            CollectionAssert.Contains(functionNode.Statements, declarationExpectation);

            var marshalExpectation = new ExpressionStatementNode(
                new FunctionCallNode("Marshal", new StaticMemberQualifier("Marshalling"),
                    Naming.AddressOf(paramName.Marshalled()), Naming.AddressOf(paramName)));
            CollectionAssert.Contains(functionNode.Statements, marshalExpectation);

            var callExpectation = new ExpressionStatementNode(
                new FunctionCallNode(methodName, new StaticMemberQualifier(type.RegisteredName()), paramName.Marshalled()));
            CollectionAssert.Contains(functionNode.Statements, callExpectation);
        }

        [Test(Description = "Test that return type for the binding function is correct")]
        [TestCase(typeof(ReturnValueBinding), nameof(ReturnValueBinding.TestBool), "ScriptingBool")]
        [TestCase(typeof(ReturnValueBinding), nameof(ReturnValueBinding.TestInt), "SInt32")]
        [TestCase(typeof(ReturnValueBinding), nameof(ReturnValueBinding.TestFloat), "float")]
        [TestCase(typeof(ReturnValueBinding), nameof(ReturnValueBinding.TestString), "ICallType_String_Return")]
        [TestCase(typeof(ReturnValueBinding), nameof(ReturnValueBinding.TestIntArray), "ICallType_Array_Return")]
        [TestCase(typeof(ReturnValueBinding), nameof(ReturnValueBinding.TestStringArray), "ICallType_Array_Return")]
        [TestCase(typeof(ReturnValueBinding), nameof(ReturnValueBinding.TestClassReturn), "Marshalling::UnityObjectReturnValue")]
        [TestCase(typeof(ReturnValueBinding), nameof(ReturnValueBinding.TestSystemArrayReturn), "ICallType_Array_Return")]
        [TestCase(typeof(StaticMethodStruct), nameof(StaticMethodStruct.TestIntReturn), "SInt32")]
        [TestCase(typeof(ReturnValueBinding), nameof(ReturnValueBinding.TestIntPtrReturn), "ICallType_IntPtr_Argument")]
        [TestCase(typeof(ReturnValueBinding), nameof(ReturnValueBinding.TestUIntPtrReturn), "ICallType_IntPtr_Argument")]
        public void ReturnValueBindingWorks(Type type, string methodName, string returnType)
        {
            var functionNode = FunctionNodeForMethod(type, methodName);

            Assert.AreEqual(returnType, functionNode.ReturnType);
        }

        [Test(Description = "Test that we marshal return values correctly")]
        [TestCase(typeof(ReturnValueMarshalling), nameof(ReturnValueMarshalling.TestInt))]
        [TestCase(typeof(ReturnValueMarshalling), nameof(ReturnValueMarshalling.TestFloat))]
        [TestCase(typeof(ReturnValueMarshalling), nameof(ReturnValueMarshalling.TestEnum))]
        [TestCase(typeof(StaticMethodStruct),     nameof(StaticMethodStruct.TestIntReturn))]
        public void ReturnValueMarshallingWorkForTypesThatRequireMarshalling(Type type, string methodName)
        {
            var functionNode = FunctionNodeForMethod(type, methodName);
            var returnExpectation = new ReturnStatementNode(
                new FunctionCallNode(methodName, new StaticMemberQualifier(type.RegisteredName())));

            CollectionAssert.Contains(functionNode.Statements, returnExpectation);
        }

        [Test(Description = "Test that we marshal return values correctly")]
        [TestCase(typeof(ReturnValueMarshalling), nameof(ReturnValueMarshalling.TestString), "Marshalling::StringUnmarshaller")]
        [TestCase(typeof(ReturnValueMarshalling), nameof(ReturnValueMarshalling.TestIntArray), "Marshalling::ArrayUnmarshaller<SInt32>")]
        [TestCase(typeof(ReturnValueMarshalling), nameof(ReturnValueMarshalling.TestStringArray), "Marshalling::ArrayUnmarshaller<Marshalling::StringArrayElement>")]
        [TestCase(typeof(ReturnValueMarshalling), nameof(ReturnValueMarshalling.TestSystemArrayReturn), "Marshalling::SystemArrayUnmarshaller")]
        [TestCase(typeof(ReturnValueMarshalling), nameof(ReturnValueMarshalling.TestIntPtrReturn), "Marshalling::IntPtrUnmarshaller")]
        [TestCase(typeof(ReturnValueMarshalling), nameof(ReturnValueMarshalling.TestUIntPtrReturn), "Marshalling::IntPtrUnmarshaller")]
        [TestCase(typeof(ReturnValueMarshalling), nameof(ReturnValueMarshalling.TestInterfaceReturn), "Marshalling::ManagedObjectUnmarshaller")]
        public void ReturnValueMarshallingWorkForTypesThatDoNotRequireMarshalling(Type type, string methodName, string expectedType)
        {
            var functionNode = FunctionNodeForMethod(type, methodName);

            var tempExpectation = new VariableDeclarationStatementNode(Naming.ReturnVar.Unmarshalled(), functionNode.ReturnType);
            CollectionAssert.Contains(functionNode.Statements, tempExpectation);

            var unmarshalledVariableExpectation = new VariableDeclarationStatementNode(
                Naming.ReturnVar,
                expectedType,
                new FunctionCallNode(methodName, new StaticMemberQualifier(type.RegisteredName())));
            CollectionAssert.Contains(functionNode.Statements, unmarshalledVariableExpectation);

            var callExpectation = new ExpressionStatementNode(
                new FunctionCallNode(
                    "Unmarshal",
                    new StaticMemberQualifier("Marshalling"),
                    Naming.AddressOf(Naming.ReturnVar).Unmarshalled(),
                    Naming.AddressOf(Naming.ReturnVar)));
            CollectionAssert.Contains(functionNode.Statements, callExpectation);

            var returnExpectation = new ReturnStatementNode(new StringExpressionNode(Naming.ReturnVar.Unmarshalled()));
            CollectionAssert.Contains(functionNode.Statements, returnExpectation);
        }

        [Test(Description = "Test that we marshal return values correctly")]
        [TestCase(typeof(ReturnValueMarshalling), nameof(ReturnValueMarshalling.TestClassReturn), "Marshalling::UnityObjectUnmarshaller<ICallType_Generated_Unity_BindingsGenerator_Tests_StaticMethodTests_Empty>", typeof(Empty))]
        [TestCase(typeof(ReturnValueMarshalling), nameof(ReturnValueMarshalling.TestArrayOfClassTypeReturn), "Marshalling::ArrayUnmarshaller<Marshalling::UnityObjectArrayElement<ICallType_Generated_Unity_BindingsGenerator_Tests_StaticMethodTests_Empty> >", typeof(Empty))]
        public void ReturnValueMarshallingWork(Type type, string methodName, string expectedType, Type typeExpectingRegistration)
        {
            FileNode fileNode;
            var functionNode = FunctionNodeForMethod(type, methodName, out fileNode);
            var tempExpectation = new VariableDeclarationStatementNode(Naming.ReturnVar.Unmarshalled(), functionNode.ReturnType);
            CollectionAssert.Contains(functionNode.Statements, tempExpectation);

            var unmarshalledVariableExpectation = new VariableDeclarationStatementNode(
                Naming.ReturnVar, expectedType,
                new FunctionCallNode(methodName, new StaticMemberQualifier(type.RegisteredName())));
            CollectionAssert.Contains(functionNode.Statements, unmarshalledVariableExpectation);

            var callExpectation = new ExpressionStatementNode(
                new FunctionCallNode(
                    "Unmarshal",
                    new StaticMemberQualifier("Marshalling"),
                    Naming.AddressOf(Naming.ReturnVar).Unmarshalled(),
                    Naming.AddressOf(Naming.ReturnVar)));
            CollectionAssert.Contains(functionNode.Statements, callExpectation);

            var returnExpectation = new ReturnStatementNode(new StringExpressionNode(Naming.ReturnVar.Unmarshalled()));
            CollectionAssert.Contains(functionNode.Statements, returnExpectation);

            var registrationNamespace = fileNode.Namespaces.Single(n => n.Name == Naming.RegistrationChecksNamespace);
            var expectation = new TypeDefNode
            {
                SourceType = typeExpectingRegistration.RegisteredName(),
                DestType = Naming.VerifyBindingNameFor(typeExpectingRegistration.FullnameWithUnderscores())
            };

            CollectionAssert.Contains(registrationNamespace.TypeDefs, expectation);
        }

        [Test(Description = "Test that stack check works")]
        public void CallDeclarationWorks()
        {
            var functionNode = FunctionNodeForMethod(typeof(SimpleClass), nameof(SimpleClass.Test));

            Assert.AreEqual(Naming.DeclSpec, functionNode.DeclSpec);
        }

        [Test(Description = "Test that stack check works")]
        public void CallingConventionWorks()
        {
            var functionNode = FunctionNodeForMethod(typeof(SimpleClass), nameof(SimpleClass.Test));

            Assert.AreEqual(Naming.CallingConvention, functionNode.CallingConvention);
        }

        [Test(Description = "Test that etw entry macro is emitted")]
        public void EtwEntryWorks()
        {
            var expectation = new StringStatementNode($"{Naming.EtwEntryMacro}(StaticMethodTests_SimpleClass_CUSTOM_Test)");
            var functionNode = FunctionNodeForMethod(typeof(SimpleClass), nameof(SimpleClass.Test));

            CollectionAssert.Contains(functionNode.Statements, expectation);
        }

        [Test(Description = "Test that stack check works")]
        public void StackCheckWorks()
        {
            var expectation = new StringStatementNode($"{Naming.StackCheckMacro}(Test)");
            var functionNode = FunctionNodeForMethod(typeof(SimpleClass), nameof(SimpleClass.Test));

            CollectionAssert.Contains(functionNode.Statements, expectation);
        }

        [Test(Description = "Test that thread safe check works")]
        public void ThreadSafeCheckWorks()
        {
            var expectation = new StringStatementNode($"{Naming.ThreadSafeMacro}(Test)");

            var functionNodeSimpleClass = FunctionNodeForMethod(typeof(SimpleClass), nameof(SimpleClass.Test));
            var functionNodeThreadSafe = FunctionNodeForMethod(typeof(ThreadSafe), nameof(ThreadSafe.Test));

            CollectionAssert.Contains(functionNodeSimpleClass.Statements, expectation);
            CollectionAssert.DoesNotContain(functionNodeThreadSafe.Statements, expectation);
        }

        class ClassWithStaticMethod
        {
            [MethodImpl(MethodImplOptions.InternalCall)]
            public static extern void StaticMethod();
        }

        [Test(Description = "Test that static methods go through BIND_MANAGED_TYPE_NAME macro")]
        public void StaticMethodInvokedOnGeneratedClassName()
        {
            var functionNode = FunctionNodeForMethod(typeof(ClassWithStaticMethod), nameof(ClassWithStaticMethod.StaticMethod));

            var callExpectation = new ExpressionStatementNode(
                new FunctionCallNode("StaticMethod",
                    new StaticMemberQualifier(
                        TypeUtils.RegisteredNameFor(TypeResolver.GetTypeReference<ClassWithStaticMethod>()))));

            CollectionAssert.Contains(functionNode.Statements, callExpectation);
        }

        [Test]
        public void UnmarshalledUnityEngineObjectDoesNotGenerateMarshalInfo()
        {
            var tree = TreeGeneratorForMethod(typeof(ParameterMarshalling),
                nameof(ParameterMarshalling.TestUnmarshalledClassArgument));

            AssertMarshalInfoNotExists(typeof(Empty).RegisteredName(), tree);
        }

        [Test]
        public void ExternInNestedClassGeneratesFunction()
        {
            var tree = GeneratedTreeFor<ParentOfNested>();
            var actualFunction = tree.SourceFile.DefaultNamespace.Functions.FirstOrDefault(
                f => f.Name == "StaticMethodTests_ParentOfNested_Nested_CUSTOM_Test");
            Assert.NotNull(actualFunction);
        }

        [Test]
        public void ExternInAbstractClassGeneratesFunction()
        {
            var tree = GeneratedTreeFor<AbstractClass>();
            var actualFunction = tree.SourceFile.DefaultNamespace.Functions.FirstOrDefault(
                f => f.Name == "StaticMethodTests_AbstractClass_CUSTOM_Test");
            Assert.NotNull(actualFunction);
        }
    }
}
