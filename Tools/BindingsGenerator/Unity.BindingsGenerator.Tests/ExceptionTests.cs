using System;
using System.Collections.Generic;
using System.Linq;
using System.Runtime.CompilerServices;
using Mono.Cecil;
using NUnit.Framework;
using Unity.BindingsGenerator.TestFramework;
using UnityEngine.Bindings;
using Unity.BindingsGenerator.Core;
using Unity.BindingsGenerator.Core.Nodes;

#pragma warning disable CS0649, CS0169

namespace Unity.BindingsGenerator.Tests
{
#pragma warning disable 169
#pragma warning disable 649
    [TestFixture]
    public class ExceptionTests : BindingsGeneratorTestsBase
    {
        class ClassThatCanThrow
        {
            [NativeThrows]
            [MethodImpl(MethodImplOptions.InternalCall)]
            public static extern void MethodThatCanThrow();

            [MethodImpl(MethodImplOptions.InternalCall)]
            public static extern void MethodThatCannotThrow();

            [MethodImpl(MethodImplOptions.InternalCall)]
            public static extern void MethodWithNullCheckThatCanThrow([NotNull] object o);

            [NativeThrows]
            public static extern int PropertyThatCanThrow
            {
                [MethodImpl(MethodImplOptions.InternalCall)]
                get;
                [MethodImpl(MethodImplOptions.InternalCall)]
                set;
            }
            public static extern int PropertyGetThatCanThrow
            {
                [NativeThrows]
                [MethodImpl(MethodImplOptions.InternalCall)]
                get;
                [MethodImpl(MethodImplOptions.InternalCall)]
                set;
            }
            public static extern int PropertySetThatCanThrow
            {
                [MethodImpl(MethodImplOptions.InternalCall)]
                get;
                [NativeThrows]
                [MethodImpl(MethodImplOptions.InternalCall)]
                set;
            }
        }

        [TestCase(typeof(ClassThatCanThrow), "MethodThatCanThrow", true, true)]
        [TestCase(typeof(ClassThatCanThrow), "MethodThatCannotThrow", false, false)]
        [TestCase(typeof(ClassThatCanThrow), "MethodWithNullCheckThatCanThrow", true, false)]
        [TestCase(typeof(ClassThatCanThrow), "get_PropertyThatCanThrow", true, true)]
        [TestCase(typeof(ClassThatCanThrow), "set_PropertyThatCanThrow", true, true)]
        [TestCase(typeof(ClassThatCanThrow), "get_PropertyGetThatCanThrow", true, true)]
        [TestCase(typeof(ClassThatCanThrow), "set_PropertyGetThatCanThrow", false, false)]
        [TestCase(typeof(ClassThatCanThrow), "get_PropertySetThatCanThrow", false, false)]
        [TestCase(typeof(ClassThatCanThrow), "set_PropertySetThatCanThrow", true, true)]
        public void TestGeneralExceptionSupport(Type testType, string testMethodName, bool needsExceptionSupport,
            bool hasThrow)
        {
            var functionNode = FunctionNodeForMethod(testType, testMethodName);
            Assert.IsNotNull(functionNode);
            var methodDefinition = MethodDefinitionFor(testType, testMethodName);
            var expectedExceptionVariable = new VariableDeclarationStatementNode
            {
                Name = Naming.ExceptionVariableName,
                Type = Naming.ScriptingExceptionType,
                Initializer = new StringExpressionNode { Str = "SCRIPTING_NULL" }
            };

            var labelExpectation = new LabelNode { Name = Naming.HandleExceptionLabel };
            var raiseExceptionCall = new ExpressionStatementNode
            {
                Expression = new FunctionCallNode
                {
                    Qualifier = FreeQualifier.Instance,
                    FunctionName = Naming.ScriptingRaiseExceptionFunction,
                    Arguments = { new FunctionArgument { Name = Naming.ExceptionVariableName } }
                }
            };

            if (needsExceptionSupport)
            {
                Assert.That(IsEqual(expectedExceptionVariable, functionNode.Statements[0]));
                Assert.That(IsEqual(new BlockNode { Statements = null }, functionNode.Statements[1]));
                Assert.That(IsEqual(labelExpectation, functionNode.Statements[2]));
                Assert.That(IsEqual(raiseExceptionCall, functionNode.Statements[3]));
            }
            else
            {
                Assert.IsFalse(FunctionContainsStatement(expectedExceptionVariable, functionNode));
                Assert.IsFalse(FunctionContainsStatement(new BlockNode { Statements = null }, functionNode));
                Assert.IsFalse(FunctionContainsStatement(labelExpectation, functionNode));
                Assert.IsFalse(FunctionContainsStatement(raiseExceptionCall, functionNode));
            }

            var staticFunctionCallNode = FindNativeFunctionCall(methodDefinition, functionNode);
            var lastArgument = staticFunctionCallNode.Arguments.LastOrDefault();

            if (hasThrow)
            {
                Assert.That(IsEqual(new FunctionArgument { Name = Naming.AddressOf(Naming.ExceptionVariableName) }, lastArgument));
            }
            else
            {
                if (lastArgument != null)
                    Assert.IsFalse(IsEqual(new FunctionArgument { Name = Naming.AddressOf(Naming.ExceptionVariableName) }, lastArgument));
            }
        }

        private FunctionCallNode FindNativeFunctionCall(MethodDefinition methodDefinition, FunctionNode functionNode)
        {
            var expectedStaticFunctionCallNode = new FunctionCallNode
            {
                Qualifier = new StaticMemberQualifier(),
                FunctionName = TypeUtils.NativeNameFor(methodDefinition),
                Arguments = null
            };

            if (methodDefinition.ReturnType.MetadataType == MetadataType.Void)
            {
                var expectedCall = new ExpressionStatementNode
                {
                    Expression = expectedStaticFunctionCallNode
                };

                var found = FindRecursive(expectedCall, functionNode);
                Assert.That(found, Is.Not.Null);

                return ((FunctionCallNode)found.Expression);
            }
            else
            {
                var expectedVariableDeclaration = new VariableDeclarationStatementNode
                {
                    Initializer = expectedStaticFunctionCallNode
                };

                var foundVariableDeclaration = FindRecursive(expectedVariableDeclaration, functionNode);
                if (foundVariableDeclaration != null)
                    return (FunctionCallNode)foundVariableDeclaration.Initializer;

                var expectedReturnStatement = new ReturnStatementNode
                {
                    Expression = expectedStaticFunctionCallNode
                };

                var foundReturnStatement = FindRecursive(expectedReturnStatement, functionNode);
                if (foundReturnStatement != null)
                    return (FunctionCallNode)foundReturnStatement.Expression;
            }
            Assert.Fail("Could not find native function call");
            return null;
        }

        struct BlittableStruct
        {
            int a;
        }

        struct NonblittableStruct
        {
            string a;
        }

        class ReturnTest
        {
            [NativeThrows]
            [MethodImpl(MethodImplOptions.InternalCall)]
            public static extern void VoidMethod();

            [NativeThrows]
            [MethodImpl(MethodImplOptions.InternalCall)]
            public static extern string UnmarshallingReturnMethod();

            [NativeThrows]
            [MethodImpl(MethodImplOptions.InternalCall)]
            public static extern int NonUnmarshallingReturnMethod();

            [NativeThrows]
            [MethodImpl(MethodImplOptions.InternalCall)]
            public static extern BlittableStruct BlittableStructReturnMethod();

            [NativeThrows]
            [MethodImpl(MethodImplOptions.InternalCall)]
            public static extern NonblittableStruct NonblittableStructReturnMethod();
        }

        [Test]
        public void TestVoidReturnHandling()
        {
            var functionNode = FunctionNodeForMethod<ReturnTest>("VoidMethod");
            var foundSequence = FindSequenceRecursive(new List<IStatementNode>
            {
                new ExpressionStatementNode {Expression = new FunctionCallNode { Qualifier = new StaticMemberQualifier(), Arguments = null}},
                new IfStatementNode
                {
                    Condition = new StringExpressionNode {Str = Naming.ExceptionVariableName + " != " + Naming.ScriptingNull},
                    Statements = {new StringStatementNode {Str = "goto " + Naming.HandleExceptionLabel}}
                },
                new ReturnStatementNode()
            },
                functionNode.Statements);

            Assert.That(foundSequence, Is.True);
        }

        [Test]
        public void TestUnmarshallingReturnHandling()
        {
            var functionNode = FunctionNodeForMethod<ReturnTest>("UnmarshallingReturnMethod");
            var foundSequence = FindSequenceRecursive(new List<IStatementNode>
            {
                new VariableDeclarationStatementNode {Name = Naming.ReturnVar},
                new IfStatementNode
                {
                    Condition = new StringExpressionNode {Str = Naming.ExceptionVariableName + " != " + Naming.ScriptingNull},
                    Statements = {new StringStatementNode {Str = "goto " + Naming.HandleExceptionLabel}}
                },
            },
                functionNode.Statements);

            Assert.That(foundSequence, Is.True);
        }

        [Test]
        public void TestNonUnmarshallingReturnHandling()
        {
            var functionNode = FunctionNodeForMethod<ReturnTest>("NonUnmarshallingReturnMethod");

            var foundSequence = FindSequenceRecursive(new List<IStatementNode>
            {
                new VariableDeclarationStatementNode {Name = Naming.ReturnVar},
                new IfStatementNode
                {
                    Condition = new StringExpressionNode {Str = Naming.ExceptionVariableName + " != " + Naming.ScriptingNull},
                    Statements = {new StringStatementNode {Str = "goto " + Naming.HandleExceptionLabel}}
                },
                new ReturnStatementNode {Expression = null}
            },
                functionNode.Statements);

            Assert.That(foundSequence, Is.True);
        }

        [Test]
        public void TestReturnBlittableStruct()
        {
            var functionNode = FunctionNodeForMethod<ReturnTest>("BlittableStructReturnMethod");

            var foundSequence = FindSequenceRecursive(new List<IStatementNode>
            {
                new ExpressionStatementNode { Expression = new AssignNode { Lhs = new StringExpressionNode { Str = Naming.ReturnVar } } },
                new IfStatementNode
                {
                    Condition = new StringExpressionNode {Str = Naming.ExceptionVariableName + " != " + Naming.ScriptingNull},
                    Statements = {new StringStatementNode {Str = "goto " + Naming.HandleExceptionLabel}}
                },
                new ReturnStatementNode {Expression = null}
            },
                functionNode.Statements);

            Assert.That(foundSequence, Is.True);
        }

        [Test]
        public void TestReturnNonblittableStruct()
        {
            var functionNode = FunctionNodeForMethod<ReturnTest>("NonblittableStructReturnMethod");

            var foundSequence = FindSequenceRecursive(new List<IStatementNode>
            {
                new VariableDeclarationStatementNode {Name = Naming.ReturnVar.Marshalled()},
                new IfStatementNode
                {
                    Condition = new StringExpressionNode {Str = Naming.ExceptionVariableName + " != " + Naming.ScriptingNull},
                    Statements = {new StringStatementNode {Str = "goto " + Naming.HandleExceptionLabel}}
                },
                new ExpressionStatementNode {Expression = new FunctionCallNode { Qualifier = new StaticMemberQualifier(), Arguments = null}},
                new ReturnStatementNode {Expression = null}
            },
                functionNode.Statements);

            Assert.That(foundSequence, Is.True);
        }
    }
}
