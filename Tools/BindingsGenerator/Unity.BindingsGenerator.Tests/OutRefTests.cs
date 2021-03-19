using System;
using System.Runtime.CompilerServices;
using NUnit.Framework;
using Unity.BindingsGenerator.Core;
using Unity.BindingsGenerator.TestFramework;
using Unity.BindingsGenerator.Core.Nodes;

namespace Unity.BindingsGenerator.Tests
{
    internal class OutRefTests : BindingsGeneratorTestsBase
    {
        private class PrimitiveOutTests
        {
            [MethodImpl(MethodImplOptions.InternalCall)]
            public extern void FloatOutTest(out float param);
            [MethodImpl(MethodImplOptions.InternalCall)]
            public extern void FloatRefTest(ref float param);
            [MethodImpl(MethodImplOptions.InternalCall)]
            public extern void StringOutTest(out string param);
            [MethodImpl(MethodImplOptions.InternalCall)]
            public extern void StringRefTest(ref string param);
        }

        [TestCase(typeof(PrimitiveOutTests), nameof(PrimitiveOutTests.FloatOutTest), "float*", "param", TypeUsage.OutParameter, "Marshalling::PointerMarshaller<float>")]
        [TestCase(typeof(PrimitiveOutTests), nameof(PrimitiveOutTests.FloatRefTest), "float*", "param", TypeUsage.Parameter, "Marshalling::PointerMarshaller<float>")]
        [TestCase(typeof(PrimitiveOutTests), nameof(PrimitiveOutTests.StringOutTest), "ICallType_String_Argument_Out", "param", TypeUsage.OutParameter, "Marshalling::StringOutMarshaller")]
        [TestCase(typeof(PrimitiveOutTests), nameof(PrimitiveOutTests.StringRefTest), "ICallType_String_Argument_Ref", "param", TypeUsage.Parameter, "Marshalling::StringOutMarshaller")]
        public void PrimitiveOutParameterTest(Type klass, string methodName, string parameterType, string parameterName, TypeUsage typeUsage, string marshalledType)
        {
            var functionNode = FunctionNodeForMethod(klass, methodName);
            var parameterExpectation = new FunctionParameter {Name = parameterName, Type = parameterType, Usage = typeUsage};
            AssertContains(parameterExpectation, functionNode.Parameters);

            var declarationExpectation = new VariableDeclarationStatementNode
            {
                Name = parameterName.Marshalled(),
                Type = marshalledType,
            };
            Assert.IsTrue(FunctionContainsStatement(declarationExpectation, functionNode));

            var marshalExpectation = new ExpressionStatementNode
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
            Assert.IsTrue(FunctionContainsStatement(marshalExpectation, functionNode));
        }
    }
}
