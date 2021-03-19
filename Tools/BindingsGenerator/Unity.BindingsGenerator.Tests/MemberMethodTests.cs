using System;
using System.Collections.Generic;
using System.Linq;
using System.Runtime.CompilerServices;
using NUnit.Framework;
using Unity.BindingsGenerator.Core;
using UnityEngine.Bindings;
using Unity.BindingsGenerator.Core.Nodes;
using Unity.BindingsGenerator.TestFramework;

namespace Unity.BindingsGenerator.Tests
{
    [TestFixture]
    internal class MemberMethodTests : BindingsGeneratorTestsBase
    {
        #pragma warning disable 649
        private class Simple : UnityEngine.Object
        {
            [MethodImpl(MethodImplOptions.InternalCall)]
            public extern void Test();

            [MethodImpl(MethodImplOptions.InternalCall)]
            public extern void TestArguments(int a, bool b, string c, int[] d);

            [MethodImpl(MethodImplOptions.InternalCall)]
            public extern int TestReturnValue();
        }

        private class MethodNameOverride : UnityEngine.Object
        {
            [MethodImpl(MethodImplOptions.InternalCall)]
            [NativeMethod(Name = "Bar")]
            public extern void TestNameOverride();
        }

        private class IntPtrObject
        {
            internal IntPtr m_IntPtr;

            [MethodImpl(MethodImplOptions.InternalCall)]
            public extern void Test();
        }

        // NOTE: We test marshalling, threadsafe, etc for the static methods so we just test additional stuff here

        [Test(Description = "Test that name override works for member functions")]
        [TestCase(typeof(MethodNameOverride), "TestNameOverride", "Bar")]
        public void NameOverrideWorks(Type type, string functionName, string nativeName)
        {
            var functionNode = FunctionNodeForMethod(type, functionName);

            var callExpectation = new ExpressionStatementNode
            {
                Expression = new FunctionCallNode
                {
                    Qualifier = new InstanceMemberQualifier { Expression = Naming.Self.Marshalled() },
                    FunctionName = nativeName,
                }
            };

            Assert.True(FunctionContainsStatement(callExpectation, functionNode));
        }

        [Test(Description = "Test that we call the member function with the right arguments")]
        [TestCase(typeof(Simple), "Test", new string[] {})]
        [TestCase(typeof(Simple), "TestArguments", new string[] {"a", "b", "c_marshalled", "d_marshalled"})]
        public void MemberFunctionCallWorks(Type type, string functionName, string[] argumentNames)
        {
            var functionNode = FunctionNodeForMethod(type, functionName);
            var arguments = argumentNames.Select(an => new FunctionArgument {Name = an}).ToList();

            var callExpectation = new ExpressionStatementNode
            {
                Expression = new FunctionCallNode
                {
                    Qualifier = new InstanceMemberQualifier { Expression = Naming.Self.Marshalled() },
                    FunctionName = functionName,
                    Arguments = arguments,
                }
            };

            Assert.IsTrue(FunctionContainsStatement(callExpectation, functionNode));
        }

        [Test(Description = "Test that we correctly insert the self parameter and the type is correct including name overrides")]
        [TestCase(typeof(Simple), "Test", "Simple")]
        public void SelfParameterWorks(Type type, string functionName, string nativeTypeName)
        {
            var functionNode = FunctionNodeForMethod(type, functionName);

            var expectation = new FunctionParameter
            {
                Name = Naming.Self,
                Type = "ICallType_ReadOnlyUnityEngineObject_Argument",
                IsSelf = true
            };

            AssertEqual(expectation, functionNode.Parameters[0]);
        }

        [Test(Description = "Test that we call the member function with the right arguments")]
        public void MemberFunctionReturnValueWorks()
        {
            var functionNode = FunctionNodeForMethod<Simple>("TestReturnValue");

            var returnExpectation = new ReturnStatementNode
            {
                Expression = new FunctionCallNode
                {
                    Qualifier = new InstanceMemberQualifier(),
                    FunctionName = "TestReturnValue",
                    Arguments = null
                }
            };
            Assert.IsTrue(FunctionContainsStatement(returnExpectation, functionNode));
        }

        [Test(Description = "Test that we marshal self correctly for intptr object")]
        public void IntPtrObjectWorks()
        {
            var functionNode = FunctionNodeForMethod<IntPtrObject>("Test");

            var expectation = new List<FunctionParameter>
            {
                new FunctionParameter {Type = "ICallType_Object_Argument", Name = Naming.Self, IsSelf = true}
            };
            AssertEqual(expectation, functionNode.Parameters);
        }
    }
}
