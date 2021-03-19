using System;
using System.Runtime.CompilerServices;
using NUnit.Framework;
using Unity.BindingsGenerator.Core;
using UnityEngine.Bindings;
using Unity.BindingsGenerator.Core.Nodes;
using Unity.BindingsGenerator.TestFramework;

namespace Unity.BindingsGenerator.Tests
{
    [TestFixture]
    class StubTests : BindingsGeneratorTestsBase
    {
        class ClassWithMethodStubs
        {
            [MethodImpl(MethodImplOptions.InternalCall)]
            extern void NoStubTest();

            [MethodImpl(MethodImplOptions.InternalCall)]
            [NativeConditional("SOME_CONDITION1")]
            extern void VoidReturnTest();

            [MethodImpl(MethodImplOptions.InternalCall)]
            [NativeConditional("SOME_CONDITION2")]
            extern int IntReturnTest();

            public extern int Prop
            {
                [NativeConditional("SOME_CONDITION3")]
                [MethodImpl(MethodImplOptions.InternalCall)]
                get;

                [NativeConditional("SOME_CONDITION4")]
                [MethodImpl(MethodImplOptions.InternalCall)]
                set;
            }

            [NativeConditional("SOME_CONDITION5")]
            public extern int Prop2
            {
                [MethodImpl(MethodImplOptions.InternalCall)]
                get;

                [MethodImpl(MethodImplOptions.InternalCall)]
                set;
            }
        }

        class SomeClass
        {
        }

        enum SomeEnum
        {
            SomeValue = 1,
            SomeOtherValue = 2,
        }

        class ClassWithMethodStubsReturnValueTest
        {
            [MethodImpl(MethodImplOptions.InternalCall)]
            [NativeConditional("CONDITION")]
            extern Byte      ReturnTest01();

            [MethodImpl(MethodImplOptions.InternalCall)]
            [NativeConditional("CONDITION")]
            extern Char      ReturnTest02();

            [MethodImpl(MethodImplOptions.InternalCall)]
            [NativeConditional("CONDITION")]
            extern UInt16    ReturnTest03();

            [MethodImpl(MethodImplOptions.InternalCall)]
            [NativeConditional("CONDITION")]
            extern UInt32    ReturnTest04();

            [MethodImpl(MethodImplOptions.InternalCall)]
            [NativeConditional("CONDITION")]
            extern UInt64    ReturnTest05();

            [MethodImpl(MethodImplOptions.InternalCall)]
            [NativeConditional("CONDITION")]
            extern Int16     ReturnTest06();

            [MethodImpl(MethodImplOptions.InternalCall)]
            [NativeConditional("CONDITION")]
            extern Int32     ReturnTest07();

            [MethodImpl(MethodImplOptions.InternalCall)]
            [NativeConditional("CONDITION")]
            extern Int64     ReturnTest08();

            [MethodImpl(MethodImplOptions.InternalCall)]
            [NativeConditional("CONDITION")]
            extern float     ReturnTest09();

            [MethodImpl(MethodImplOptions.InternalCall)]
            [NativeConditional("CONDITION")]
            extern double    ReturnTest10();

            [MethodImpl(MethodImplOptions.InternalCall)]
            [NativeConditional("CONDITION")]
            extern string    ReturnTest11();

            [MethodImpl(MethodImplOptions.InternalCall)]
            [NativeConditional("CONDITION")]
            extern SomeClass ReturnTest12();

            [MethodImpl(MethodImplOptions.InternalCall)]
            [NativeConditional("CONDITION")]
            extern bool      ReturnTest13();

            [MethodImpl(MethodImplOptions.InternalCall)]
            [NativeConditional("CONDITION")]
            extern SomeEnum  ReturnTest14();

            [MethodImpl(MethodImplOptions.InternalCall)]
            [NativeConditional("CONDITION")]
            extern void      ReturnTest15();

            [MethodImpl(MethodImplOptions.InternalCall)]
            [NativeConditional("CONDITION", "SomeOtherValue")]
            extern SomeEnum  ReturnTestCustom();
        }

        [TestCase(typeof(ClassWithMethodStubs), "NoStubTest", null, null)]
        [TestCase(typeof(ClassWithMethodStubs), "VoidReturnTest", "SOME_CONDITION1", null)]
        [TestCase(typeof(ClassWithMethodStubs), "IntReturnTest", "SOME_CONDITION2", "0")]
        [TestCase(typeof(ClassWithMethodStubs), "get_Prop", "SOME_CONDITION3", "0")]
        [TestCase(typeof(ClassWithMethodStubs), "set_Prop", "SOME_CONDITION4", null)]
        [TestCase(typeof(ClassWithMethodStubs), "get_Prop2", "SOME_CONDITION5", "0")]
        [TestCase(typeof(ClassWithMethodStubs), "set_Prop2", "SOME_CONDITION5", null)]
        [TestCase(typeof(ClassWithMethodStubsReturnValueTest), "ReturnTest01", "CONDITION", "0")]
        [TestCase(typeof(ClassWithMethodStubsReturnValueTest), "ReturnTest02", "CONDITION", "0")]
        [TestCase(typeof(ClassWithMethodStubsReturnValueTest), "ReturnTest03", "CONDITION", "0")]
        [TestCase(typeof(ClassWithMethodStubsReturnValueTest), "ReturnTest04", "CONDITION", "0")]
        [TestCase(typeof(ClassWithMethodStubsReturnValueTest), "ReturnTest05", "CONDITION", "0")]
        [TestCase(typeof(ClassWithMethodStubsReturnValueTest), "ReturnTest06", "CONDITION", "0")]
        [TestCase(typeof(ClassWithMethodStubsReturnValueTest), "ReturnTest07", "CONDITION", "0")]
        [TestCase(typeof(ClassWithMethodStubsReturnValueTest), "ReturnTest08", "CONDITION", "0")]
        [TestCase(typeof(ClassWithMethodStubsReturnValueTest), "ReturnTest09", "CONDITION", "0.0f")]
        [TestCase(typeof(ClassWithMethodStubsReturnValueTest), "ReturnTest10", "CONDITION", "0.0")]
        [TestCase(typeof(ClassWithMethodStubsReturnValueTest), "ReturnTest11", "CONDITION", "scripting_string_new(\"\")")]
        [TestCase(typeof(ClassWithMethodStubsReturnValueTest), "ReturnTest12", "CONDITION", "SCRIPTING_NULL")]
        [TestCase(typeof(ClassWithMethodStubsReturnValueTest), "ReturnTest13", "CONDITION", "false")]
        [TestCase(typeof(ClassWithMethodStubsReturnValueTest), "ReturnTest14", "CONDITION", "(ICallType_Generated_Unity_BindingsGenerator_Tests_StubTests_SomeEnum)0")]
        [TestCase(typeof(ClassWithMethodStubsReturnValueTest), "ReturnTest15", "CONDITION", null)]
        [TestCase(typeof(ClassWithMethodStubsReturnValueTest), "ReturnTestCustom", "CONDITION", "SomeOtherValue")]
        public void StubEmittedCorrectly(Type klass, string methodName, string expectedCondition, string expectedReturnValue)
        {
            var functionNode = FunctionNodeForMethod(klass, methodName);

            if (expectedCondition != null)
            {
                Assert.That(functionNode.Statements.Count, Is.EqualTo(1));
                Assert.That(functionNode.Statements[0] as StubStatementNode, Is.Not.Null);

                var stub = functionNode.Statements[0] as StubStatementNode;

                Assert.That(stub.Condition.Str, Is.EqualTo(expectedCondition));

                if (expectedReturnValue != null)
                {
                    Assert.NotNull(stub.StatementsForStub);
                    var returnStatement = stub.StatementsForStub[0] as ReturnStatementNode;
                    Assert.NotNull(returnStatement);
                    Assert.That((returnStatement.Expression as StringExpressionNode)?.Str, Is.EqualTo(expectedReturnValue));
                }
                else
                    Assert.That(stub.StatementsForStub, Is.Empty);
            }
            else
            {
                Assert.That(functionNode.Statements.Count, Is.Not.EqualTo(1));
                Assert.That(functionNode.Statements[0] as StubStatementNode, Is.Null);
            }
        }

        struct SimpleStruct
        {
        }
        class ClassWithCustomStructConditional
        {
            [MethodImpl(MethodImplOptions.InternalCall)]
            [NativeConditional("CONDITION", "SimpleStruct()")]
            public extern SimpleStruct Test();
        }

        [Test]
        public void StructStubEmittedCorrectly()
        {
            var expectedAssignStatement = new ExpressionStatementNode(
                new AssignNode(
                    new StringExpressionNode(Naming.ReturnVar),
                    new StringExpressionNode("SimpleStruct()")));

            var functionNode = FunctionNodeForMethod(typeof(ClassWithCustomStructConditional), nameof(ClassWithCustomStructConditional.Test));

            Assert.That(functionNode.Statements.Count, Is.EqualTo(1));
            Assert.That(functionNode.Statements[0] as StubStatementNode, Is.Not.Null);

            var stub = functionNode.Statements[0] as StubStatementNode;

            Assert.That(stub.Condition.Str, Is.EqualTo("CONDITION"));

            Assert.NotNull(stub.StatementsForStub);
            var assignStatementActual = stub.StatementsForStub[0] as ExpressionStatementNode;
            Assert.NotNull(assignStatementActual);
            AssertEqual(expectedAssignStatement, assignStatementActual);
        }

        [NativeConditional("CONDITION")]
        class ClassWithStub
        {
            [MethodImpl(MethodImplOptions.InternalCall)]
            extern int NoStubMethod();
        }

        [NativeConditional("CONDITION", false)]
        class ClassWithStubEnabledFalse
        {
            [MethodImpl(MethodImplOptions.InternalCall)]
            extern int NoStubMethod();

            [MethodImpl(MethodImplOptions.InternalCall)]
            [NativeConditional(true)]
            extern int StubMethodWithEnabledTrue();
        }

        [NativeConditional("CONDITION", true)]
        class ClassWithStubEnabledTrue
        {
            [MethodImpl(MethodImplOptions.InternalCall)]
            extern int NoStubMethod();

            [MethodImpl(MethodImplOptions.InternalCall)]
            [NativeConditional(false)]
            extern int StubMethodWithEnabledFalse();
        }

        [NativeConditional(null)]
        class ClassWithStubButNoCondition
        {
            [MethodImpl(MethodImplOptions.InternalCall)]
            extern int NoStubMethod();

            [MethodImpl(MethodImplOptions.InternalCall)]
            [NativeConditional(null)]
            extern int NoStubConditionMethod();
        }

        [NativeConditional("CONDITION")]
        struct StructWithStub
        {
            [MethodImpl(MethodImplOptions.InternalCall)]
            static extern int NoStubMethod();

            [MethodImpl(MethodImplOptions.InternalCall)]
            [NativeConditional(false)]
            static extern int StubMethodWithEnabledFalse();
        }

        [TestCase(typeof(ClassWithStub), "NoStubMethod", true)]
        [TestCase(typeof(ClassWithStubEnabledFalse), "NoStubMethod", false)]
        [TestCase(typeof(ClassWithStubEnabledFalse), "StubMethodWithEnabledTrue", true)]
        [TestCase(typeof(ClassWithStubEnabledTrue), "NoStubMethod", true)]
        [TestCase(typeof(ClassWithStubEnabledTrue), "StubMethodWithEnabledFalse", false)]
        [TestCase(typeof(ClassWithStubButNoCondition), "NoStubMethod", false)]
        [TestCase(typeof(ClassWithStubButNoCondition), "NoStubConditionMethod", false)]
        [TestCase(typeof(StructWithStub), "NoStubMethod", true)]
        [TestCase(typeof(StructWithStub), "StubMethodWithEnabledFalse", false)]
        public void StubEnabledCorrectly(Type klass, string methodName, bool expectedStub)
        {
            var functionNode = FunctionNodeForMethod(klass, methodName);

            if (expectedStub)
            {
                Assert.That(functionNode.Statements.Count, Is.EqualTo(1));
                Assert.That(functionNode.Statements[0] as StubStatementNode, Is.Not.Null);
            }
            else
            {
                Assert.That(functionNode.Statements.Count, Is.Not.EqualTo(1));
                Assert.That(functionNode.Statements[0] as StubStatementNode, Is.Null);
            }
        }
    }
}
