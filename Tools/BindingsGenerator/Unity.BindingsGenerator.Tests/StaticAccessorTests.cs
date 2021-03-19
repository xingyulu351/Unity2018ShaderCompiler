using System;
using System.Collections.Generic;
using System.Runtime.CompilerServices;
using NUnit.Framework;
using UnityEngine.Bindings;
using Unity.BindingsGenerator.Core;
using Unity.BindingsGenerator.Core.Nodes;
using Unity.BindingsGenerator.TestFramework;

namespace Unity.BindingsGenerator.Tests
{
    [StaticAccessor("GetStaticAccessor()")]
    class StaticAccessorTestClass
    {
        [MethodImpl(MethodImplOptions.InternalCall)]
        public static extern void MethodWithMatchingNativeMethod();

        [MethodImpl(MethodImplOptions.InternalCall)]
        [NativeMethod(Name = "NativeMethodName")]
        public static extern void MethodWithDifferentNameFromNative();

        [NativeMethod(Name = "NativePropertyGetterName")]
        public static extern int Property
        {
            [MethodImpl(MethodImplOptions.InternalCall)]
            get;

            [MethodImpl(MethodImplOptions.InternalCall)]
            [NativeMethod]
            set;
        }

        [MethodImpl(MethodImplOptions.InternalCall)]
        public extern void InstanceMethod();
    }

    [StaticAccessor("GetStaticAccessor()", StaticAccessorType.Arrow)]
    class StaticAccessorTestClassThroughPointer
    {
        [MethodImpl(MethodImplOptions.InternalCall)]
        public static extern void MethodWithMatchingNativeMethod();
    }

    [StaticAccessor("ClassGetStaticAccessor()", StaticAccessorType.Arrow)]
    class StaticAccessorOnMethodTestClass
    {
        [MethodImpl(MethodImplOptions.InternalCall)]
        [StaticAccessor("MethodGetStaticAccessor()")]
        public static extern void MethodWithMatchingNativeMethod();
    }

    class StaticAccessorOnPropertyTestClass
    {
        [StaticAccessor("PropertyGetStaticAccessor()")]
        public static extern int Property
        {
            [MethodImpl(MethodImplOptions.InternalCall)]
            get;

            [MethodImpl(MethodImplOptions.InternalCall)]
            set;
        }
    }

    [TestFixture]
    public class StaticAccessorTests : BindingsGeneratorTestsBase
    {
        [Test]
        public void StaticAccessorAttributeIsIgnoredOnNonStaticMethods()
        {
            var methodNode = FunctionNodeForMethod<StaticAccessorTestClass>("InstanceMethod");

            var found = FindRecursive(new ExpressionStatementNode
            {
                Expression = new FunctionCallNode
                {
                    Qualifier = new InstanceMemberQualifier(),
                    FunctionName = "InstanceMethod"
                }
            }, methodNode);

            Assert.That(found, Is.Not.Null);
            var qualifier = (InstanceMemberQualifier)((FunctionCallNode)found.Expression).Qualifier;
            Assert.That(qualifier.Expression, Is.EqualTo(Naming.Self.Marshalled()));
            Assert.That(qualifier.IsReference, Is.False);
        }

        [TestCase("MethodWithMatchingNativeMethod", null)]
        [TestCase("MethodWithDifferentNameFromNative", "NativeMethodName")]
        public void TestStaticAccessorOnClassThroughNonPointer(string managedMethodName, string expectedNativeMethodName = null)
        {
            var methodNode = FunctionNodeForMethod<StaticAccessorTestClass>(managedMethodName);
            var expectedInvocation = new ExpressionStatementNode()
            {
                Expression = new FunctionCallNode
                {
                    Qualifier = new InstanceMemberQualifier
                    {
                        Expression = "GetStaticAccessor()",
                        IsReference = true
                    },
                    FunctionName = expectedNativeMethodName ?? managedMethodName,
                }
            };

            AssertContains(expectedInvocation , methodNode.Statements);
        }

        [Test]
        public void TestStaticAccessorOnClassThroughPointer()
        {
            var methodName = "MethodWithMatchingNativeMethod";
            var methodNode = FunctionNodeForMethod<StaticAccessorTestClassThroughPointer>(methodName);
            var expectedInvocation = new ExpressionStatementNode
            {
                Expression = new FunctionCallNode
                {
                    Qualifier = new InstanceMemberQualifier
                    {
                        Expression = "GetStaticAccessor()",
                        IsReference = false
                    },
                    FunctionName = methodName,
                }
            };

            AssertContains(expectedInvocation, methodNode.Statements);
        }

        [Test]
        public void TestStaticAccessorOnMethod()
        {
            var methodName = "MethodWithMatchingNativeMethod";
            var methodNode = FunctionNodeForMethod<StaticAccessorOnMethodTestClass>(methodName);
            var expectedInvocation = new ExpressionStatementNode
            {
                Expression = new FunctionCallNode
                {
                    Qualifier = new InstanceMemberQualifier
                    {
                        Expression = "MethodGetStaticAccessor()",
                        IsReference = true
                    },
                    FunctionName = methodName
                }
            };

            AssertContains(expectedInvocation, methodNode.Statements);
        }

        [TestCase(typeof(StaticAccessorTestClass), "get_Property", "GetStaticAccessor()", "GetNativePropertyGetterName")]
        [TestCase(typeof(StaticAccessorOnPropertyTestClass), "get_Property", "PropertyGetStaticAccessor()", "GetProperty")]
        public void TestStaticAccessorOnPropertyGetter(Type type, string managedMethodName, string expression, string nativeFunctionName)
        {
            var methodNode = FunctionNodeForMethod(type, managedMethodName);
            var expectedInvocation = new ReturnStatementNode
            {
                Expression = new FunctionCallNode
                {
                    Qualifier = new InstanceMemberQualifier
                    {
                        Expression = expression,
                        IsReference = true
                    },
                    FunctionName = nativeFunctionName
                }
            };

            AssertContains(expectedInvocation, methodNode.Statements);
        }

        [TestCase(typeof(StaticAccessorTestClass), "set_Property", "GetStaticAccessor()", "SetNativePropertyGetterName")]
        [TestCase(typeof(StaticAccessorOnPropertyTestClass), "set_Property", "PropertyGetStaticAccessor()", "SetProperty")]
        public void TestStaticAccessorOnPropertySetter(Type type, string managedMethodName, string expression, string nativeFunctionName)
        {
            var methodNode = FunctionNodeForMethod(type, managedMethodName);
            var expectedInvocation = new ExpressionStatementNode
            {
                Expression = new FunctionCallNode
                {
                    Qualifier = new InstanceMemberQualifier
                    {
                        Expression = expression,
                        IsReference = true
                    },
                    FunctionName = nativeFunctionName,
                    Arguments = new List<FunctionArgument>
                    {
                        new FunctionArgument { Name = Naming.PropertyValue }
                    }
                }
            };

            AssertContains(expectedInvocation, methodNode.Statements);
        }

        [StaticAccessor("NativeNamespace", StaticAccessorType.DoubleColon)]
        class DoubleColonTests
        {
            [MethodImpl(MethodImplOptions.InternalCall)]
            public static extern void SomeMethod();

            [MethodImpl(MethodImplOptions.InternalCall)]
            [StaticAccessor("NativeNamespace2", StaticAccessorType.DoubleColon)]
            public static extern void SomeMethodWithOverload();
        }

        [TestCase(typeof(DoubleColonTests), nameof(DoubleColonTests.SomeMethod), "NativeNamespace")]
        [TestCase(typeof(DoubleColonTests), nameof(DoubleColonTests.SomeMethodWithOverload), "NativeNamespace2")]
        public void DoubleColonAccessorMapsToCorrectNamespaceWithStaticQualifier(Type type, string methodName, string expectedNamespaceName)
        {
            var methodNode = FunctionNodeForMethod(type, methodName);
            var expectedInvocation = new ExpressionStatementNode
            {
                Expression = new FunctionCallNode
                {
                    Qualifier = new StaticMemberQualifier
                    {
                        ClassName = expectedNamespaceName,
                    },
                }
            };

            AssertContains(expectedInvocation, methodNode.Statements);
        }
    }
}
