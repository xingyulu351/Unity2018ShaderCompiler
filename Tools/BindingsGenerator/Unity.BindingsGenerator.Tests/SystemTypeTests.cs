using System;
using System.Runtime.CompilerServices;
using NUnit.Framework;
using Unity.BindingsGenerator.Core;
using Unity.BindingsGenerator.Core.Nodes;
using Unity.BindingsGenerator.TestFramework;

namespace Unity.BindingsGenerator.Tests
{
    [TestFixture]
    class SystemTypeTests : BindingsGeneratorTestsBase
    {
        class TestSystemTypeClass
        {
            [MethodImpl(MethodImplOptions.InternalCall)]
            public static extern void Test(System.Type type);

            [MethodImpl(MethodImplOptions.InternalCall)]
            public static extern System.Type ReturnTest();
        };

        [TestCase(typeof(TestSystemTypeClass), "Test", "ICallType_SystemTypeObject_Argument", "type")]
        public void ParameterTypeIsCorrect(Type klass, string methodName, string parameterType, string parameterName)
        {
            var functionNode = FunctionNodeForMethod(klass, methodName);

            var parameterExpectation = new FunctionParameter() {Name = parameterName, Type = parameterType};

            AssertContains(parameterExpectation, functionNode.Parameters);
        }

        [TestCase(typeof(TestSystemTypeClass), "Test", "Marshalling::SystemTypeMarshaller", "type")]
        public void MarshallingStatementsAreCorrect(Type klass, string methodName, string parameterType, string parameterName)
        {
            var functionNode = FunctionNodeForMethod(klass, methodName);

            var variableDeclarationStatement = new VariableDeclarationStatementNode
            {
                Name = parameterName.Marshalled(),
                Type = parameterType,
            };

            var marshallingStatement = new ExpressionStatementNode
            {
                Expression = new FunctionCallNode
                {
                    Qualifier = new StaticMemberQualifier { ClassName = "Marshalling" },
                    FunctionName = "Marshal",
                    Arguments =
                    {
                        new FunctionArgument {Name = Naming.AddressOf(parameterName.Marshalled())},
                        new FunctionArgument {Name = Naming.AddressOf(parameterName)},
                    }
                }
            };

            Assert.IsTrue(FunctionContainsStatement(variableDeclarationStatement, functionNode));
            Assert.IsTrue(FunctionContainsStatement(marshallingStatement, functionNode));
        }

        [Test(Description = "Test that return type for the binding function is correct")]
        public void ReturnValueBindingWorks()
        {
            var functionNode = FunctionNodeForMethod<TestSystemTypeClass>("ReturnTest");
            AssertEqual(new FunctionNode { ReturnType = "ICallType_SystemTypeObject_Return", Parameters = null, Statements = null }, functionNode);
        }

        [Test(Description = "Test that we marshal return values correctly")]
        public void ReturnValueMarshallingWork()
        {
            var functionNode = FunctionNodeForMethod<TestSystemTypeClass>("ReturnTest");

            var tempExpectation = new VariableDeclarationStatementNode()
            {
                Name = Naming.ReturnVar.Unmarshalled(),
                Type = functionNode.ReturnType
            };
            Assert.IsTrue(FunctionContainsStatement(tempExpectation, functionNode));

            var unmarshalledVariableExpectation = new VariableDeclarationStatementNode()
            {
                Name = Naming.ReturnVar,
                Type = "Marshalling::SystemTypeUnmarshaller"
            };
            Assert.IsTrue(FunctionContainsStatement(unmarshalledVariableExpectation, functionNode));

            var callExpectation = new ExpressionStatementNode
            {
                Expression = new FunctionCallNode
                {
                    Qualifier = new StaticMemberQualifier { ClassName = "Marshalling" },
                    FunctionName = "Unmarshal",
                    Arguments =
                    {
                        new FunctionArgument {Name = Naming.AddressOf(Naming.ReturnVar).Unmarshalled()},
                        new FunctionArgument {Name = Naming.AddressOf(Naming.ReturnVar)},
                    }
                }
            };
            Assert.IsTrue(FunctionContainsStatement(callExpectation, functionNode));

            var returnExpectation = new ReturnStatementNode
            {
                Expression = new StringExpressionNode {Str = Naming.ReturnVar.Unmarshalled()}
            };
            Assert.IsTrue(FunctionContainsStatement(returnExpectation, functionNode));
        }
    }
}
