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
    internal class ParameterTests : BindingsGeneratorTestsBase
    {
        #pragma warning disable 169

        class SomeClass : UnityEngine.Object
        {
        }

        struct SomeStruct
        {
            int a;
        }

        enum SomeEnum
        {
            A = 1,
        }

        class CanBeNullTestClass
        {
            [MethodImpl(MethodImplOptions.InternalCall)]
            public extern void MemberMethodTest();

            [MethodImpl(MethodImplOptions.InternalCall)]
            public static extern void PrimitiveTest(int a, bool  b, SomeEnum c, SomeStruct d);

            [MethodImpl(MethodImplOptions.InternalCall)]
            public static extern void PrimitiveReferenceTest(ref int a, ref bool b, ref SomeEnum c);

            [MethodImpl(MethodImplOptions.InternalCall)]
            public static extern SomeStruct StructReturnTest();

            [MethodImpl(MethodImplOptions.InternalCall)]
            public static extern void ReferenceTypeTest(string a, int[] b, SomeClass c);

            [MethodImpl(MethodImplOptions.InternalCall)]
            public static extern void ReferenceTypeWithAttributeTest([NotNull] string a, [NotNull] int[] b, [NotNull] SomeClass c);
            [MethodImpl(MethodImplOptions.InternalCall)]
            public static extern void StringWithUnmarshalled([NotNull][Unmarshalled] string a);
        }

        private IStatementNode ExpectedNullCheckStatementFor(string parameterName, bool isUnmarshalled)
        {
            var ifStatement = new IfStatementNode();
            var typeUsage = isUnmarshalled ? TypeUsage.UnmarshalledParameter : TypeUsage.Parameter;
            ifStatement.Condition = new StringExpressionNode { Str = TypeUtils.NullCheckFor(parameterName, typeUsage, ScriptingObjectMarshallingType.ScriptingObjectPtr) };
            ifStatement.Statements = new List<IStatementNode>
            {
                new ExpressionStatementNode
                {
                    Expression = new AssignNode
                    {
                        Lhs = new StringExpressionNode { Str = Naming.ExceptionVariableName },
                        Rhs = new FunctionCallNode
                        {
                            Qualifier = FreeQualifier.Instance,
                            FunctionName = "Scripting::CreateArgumentNullException",
                            Arguments = { new FunctionArgument { Name = String.Format("\"{0}\"", parameterName) } }
                        }
                    }
                },
                new StringStatementNode { Str = "goto " + Naming.HandleExceptionLabel }
            }; // we only look for if condition, don't care about statements
            return ifStatement;
        }

        private void AssertFunctionContainsNullCheckFor(FunctionNode functionNode, string parameterName, bool isUnmarshalled = false)
        {
            var ifStatement = ExpectedNullCheckStatementFor(parameterName, isUnmarshalled);
            Assert.IsTrue(FunctionContainsStatement(ifStatement, functionNode));
        }

        private void AssertFunctionNotContainsNullCheckFor(FunctionNode functionNode, string parameterName, bool isUnmarshalled)
        {
            var ifStatement = ExpectedNullCheckStatementFor(parameterName, isUnmarshalled);
            Assert.IsFalse(FunctionContainsStatement(ifStatement, functionNode));
        }

        private void AssertFunctionNotContainsNullCheckFor(FunctionNode functionNode, string parameterName)
        {
            AssertFunctionNotContainsNullCheckFor(functionNode, parameterName, false);
        }

        private void AssertFunctionContainsObjectNullCheckFor(FunctionNode functionNode, string parameterName)
        {
            var ifStatement = new IfStatementNode();
            ifStatement.Condition = new StringExpressionNode { Str = TypeUtils.NullCheckFor(parameterName, TypeUsage.Parameter, ScriptingObjectMarshallingType.ScriptingObjectPtr) };
            ifStatement.Statements = new List<IStatementNode>
            {
                new ExpressionStatementNode
                {
                    Expression = new AssignNode
                    {
                        Lhs = new StringExpressionNode { Str = Naming.ExceptionVariableName },
                        Rhs = new FunctionCallNode
                        {
                            Qualifier = FreeQualifier.Instance,
                            FunctionName = "Marshalling::CreateNullExceptionForUnityEngineObject",
                            Arguments = { new FunctionArgument { Name = parameterName } }
                        }
                    }
                },
                new StringStatementNode { Str = "goto " + Naming.HandleExceptionLabel }
            }; // we only look for if condition, don't care about statements
            Assert.IsTrue(FunctionContainsStatement(ifStatement, functionNode));
        }

        [Test]
        public void TestStructReturn()
        {
            var functionNode = FunctionNodeForMethod<CanBeNullTestClass>("StructReturnTest");
            AssertFunctionNotContainsNullCheckFor(functionNode, Naming.ReturnVar);
        }

        [Test]
        public void TestNoNullChecksForPrimitives()
        {
            FileNode fileNode;
            var functionNode = FunctionNodeForMethod<CanBeNullTestClass>("PrimitiveTest", out fileNode);
            AssertFunctionNotContainsNullCheckFor(functionNode, "a");
            AssertFunctionNotContainsNullCheckFor(functionNode, "b");
            AssertFunctionNotContainsNullCheckFor(functionNode, "c");
            AssertFunctionNotContainsNullCheckFor(functionNode, "d");
            Assert.IsTrue(FileContainsTypeVerifications(fileNode, typeof(SomeEnum).FullnameWithUnderscores(), typeof(SomeStruct).FullnameWithUnderscores()));
        }

        [Test]
        public void TestNoNullChecksForPrimitiveReferences()
        {
            FileNode fileNode;
            var functionNode = FunctionNodeForMethod<CanBeNullTestClass>("PrimitiveReferenceTest", out fileNode);
            AssertFunctionNotContainsNullCheckFor(functionNode, "a");
            AssertFunctionNotContainsNullCheckFor(functionNode, "b");
            AssertFunctionNotContainsNullCheckFor(functionNode, "c");
        }

        [Test]
        public void TestNullChecksForMemberMethod()
        {
            var functionNode = FunctionNodeForMethod<CanBeNullTestClass>("MemberMethodTest");
            AssertFunctionContainsNullCheckFor(functionNode, Naming.Self);
        }

        [Test]
        public void TestNoNullChecksForReferenceTypes()
        {
            var functionNode = FunctionNodeForMethod<CanBeNullTestClass>("ReferenceTypeTest");
            AssertFunctionNotContainsNullCheckFor(functionNode, "a");
            AssertFunctionNotContainsNullCheckFor(functionNode, "b");
            AssertFunctionNotContainsNullCheckFor(functionNode, "c");
        }

        [Test]
        public void TestNullChecksForReferenceTypesWithAttribute()
        {
            var functionNode = FunctionNodeForMethod<CanBeNullTestClass>("ReferenceTypeWithAttributeTest");
            AssertFunctionContainsNullCheckFor(functionNode, "a");
            AssertFunctionContainsNullCheckFor(functionNode, "b");
            AssertFunctionContainsNullCheckFor(functionNode, "c");
        }

        [Test]
        public void TestNullChecksForStringWithUnmarshalled()
        {
            var functionNode = SourceFileNodeFor<CanBeNullTestClass>().FindFunction("StringWithUnmarshalled");
            AssertFunctionContainsNullCheckFor(functionNode, "a", true);
        }

        private IStatementNode ExpectedMarshallingStatementFor(string parameterName)
        {
            return TreeGenerator.MarshallingStatementFor(parameterName);
        }

        private IStatementNode ExpectedVariableDeclarationStatementFor(string parameterName)
        {
            // Deducing type and initializer are hard, only care that the variable with name is declared
            return new VariableDeclarationStatementNode
            {
                Initializer = null,
                Name = parameterName.Marshalled(),
                Type = null
            };
        }

        private void AssertFunctionContainsMarshallingFor(FunctionNode functionNode, string parameterName)
        {
            var declarationStatement = ExpectedVariableDeclarationStatementFor(parameterName);
            Assert.IsTrue(FunctionContainsStatement(declarationStatement, functionNode));
            var marshallingStatement = ExpectedMarshallingStatementFor(parameterName);
            Assert.IsTrue(FunctionContainsStatement(marshallingStatement, functionNode));
        }

        private void AssertFunctionNotContainsMarshallingFor(FunctionNode functionNode, string parameterName)
        {
            var declarationStatement = ExpectedVariableDeclarationStatementFor(parameterName);
            Assert.IsFalse(FunctionContainsStatement(declarationStatement, functionNode));
            var marshallingStatement = ExpectedMarshallingStatementFor(parameterName);
            Assert.IsFalse(FunctionContainsStatement(marshallingStatement, functionNode));
        }

        class UnityEngineObjectTestClass : UnityEngine.Object
        {
            [MethodImpl(MethodImplOptions.InternalCall)]
            public static extern void StaticMethod(string a);
            [MethodImpl(MethodImplOptions.InternalCall)]
            public extern void NonStaticMethod(string b);
            [MethodImpl(MethodImplOptions.InternalCall)]
            [NativeWritableSelf]
            public extern void WritableSelf();
        }

        [Test]
        public void StaticMethodsDoesntMarshalSelf()
        {
            var functionNode = FunctionNodeForMethod<UnityEngineObjectTestClass>(nameof(UnityEngineObjectTestClass.StaticMethod));
            AssertFunctionContainsMarshallingFor(functionNode, "a");
            AssertFunctionNotContainsMarshallingFor(functionNode, Naming.Self);
        }

        [Test]
        public void NonStaticMethodsDoesMarshalSelf()
        {
            var functionNode = FunctionNodeForMethod<UnityEngineObjectTestClass>(nameof(UnityEngineObjectTestClass.NonStaticMethod));
            AssertFunctionContainsMarshallingFor(functionNode, "b");
            AssertFunctionContainsMarshallingFor(functionNode, Naming.Self);
            AssertFunctionContainsObjectNullCheckFor(functionNode, functionNode.Parameters[0].Name);
        }

        [Test]
        [TestCase(typeof(UnityEngineObjectTestClass), nameof(UnityEngineObjectTestClass.WritableSelf))]
        public void WritableSelfCreatesWritableSelfParameter(Type type, string methodName)
        {
            var functionNode = FunctionNodeForMethod(type, methodName);
            Assert.AreEqual("ICallType_Object_Argument", functionNode.Parameters[0].Type);
            Assert.AreEqual(TypeUsage.WritableParameter, functionNode.Parameters[0].Usage);
            AssertFunctionContainsObjectNullCheckFor(functionNode, functionNode.Parameters[0].Name);
        }

        struct BlittableStructWithExtern
        {
            int a;

            [MethodImpl(MethodImplOptions.InternalCall)]
            public extern void NonStaticMethod();
        }

        [Test]
        public void NonStaticExternMethodOnBlittableStructDoesntMarshalSelf()
        {
            var functionNode = FunctionNodeForMethod<BlittableStructWithExtern>("NonStaticMethod");
            AssertFunctionNotContainsMarshallingFor(functionNode, Naming.Self);
        }

        [Test]
        public void NonStaticExternMethodOnBlittableStructDoesntNullCheckSelf()
        {
            var functionNode = FunctionNodeForMethod<BlittableStructWithExtern>("NonStaticMethod");
            AssertFunctionNotContainsNullCheckFor(functionNode, Naming.Self);
        }
    }
}
