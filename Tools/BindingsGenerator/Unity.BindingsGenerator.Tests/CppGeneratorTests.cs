using System;
using System.Collections.Generic;
using System.Runtime.CompilerServices;
using NUnit.Framework;
using Unity.BindingsGenerator.Core;
using UnityEngine.Bindings;
using Unity.BindingsGenerator.Core.Nodes;
using Unity.BindingsGenerator.TestFramework;

namespace Unity.BindingsGenerator.Tests
{
    [TestFixture]
    public class CppGeneratorTests : BindingsGeneratorTestsBase
    {
        class EmptyClass
        {
        }

        static class StaticMethodClass
        {
            [MethodImpl(MethodImplOptions.InternalCall)]
            public static extern void StaticMethod();
        }

        static class StaticMethodWithFreeFunctionOverride
        {
            [MethodImpl(MethodImplOptions.InternalCall)]
            [NativeMethod(IsFreeFunction = true, Name = "SomeFreeFunction")]
            public static extern void StaticMethod();
        }

        class AutoPropInt : UnityEngine.Object
        {
            public extern int Prop
            {
                [MethodImpl(MethodImplOptions.InternalCall)]
                get;
                [MethodImpl(MethodImplOptions.InternalCall)]
                set;
            }
        }

        [Test]
        public void SimpleClassWorks()
        {
            var output = CppCodeFor<EmptyClass>();

            Assert.AreEqual("", output);
        }

        [Test]
        public void SimpleStaticMethodWorks()
        {
            var code = CppCodeFor(typeof(StaticMethodClass));

            var nativeFunctionName = "CppGeneratorTests_StaticMethodClass_CUSTOM_StaticMethod";

            new AssertingStringMatcher(code)
                .FindFunctionStart(Naming.VoidType, nativeFunctionName, Naming.DeclSpec, Naming.CallingConvention).ExpectEmptyParens()
                .ExpectBraceBegin()
                .ExpectEtwEntryStatement(nativeFunctionName)
                .ExpectStackCheckStatement("StaticMethod")
                .ExpectThreadSafeCheckStatement("StaticMethod")
                .ExpectFunctionCallStatement($"{Naming.RegisteredNameFor("Unity_BindingsGenerator_Tests_CppGeneratorTests_StaticMethodClass")}::StaticMethod")
                .ExpectBraceEnd()
                .End();
        }

        [Test]
        public void SimpleStaticMethodAsFreeFunctionWorks()
        {
            var code = CppCodeFor(typeof(StaticMethodWithFreeFunctionOverride));

            var nativeFunctionName = "CppGeneratorTests_StaticMethodWithFreeFunctionOverride_CUSTOM_StaticMethod";
            new AssertingStringMatcher(code)
                .FindFunctionStart(Naming.VoidType, nativeFunctionName, Naming.DeclSpec, Naming.CallingConvention).ExpectEmptyParens()
                .ExpectBraceBegin()
                .ExpectEtwEntryStatement(nativeFunctionName)
                .ExpectStackCheckStatement("StaticMethod")
                .ExpectThreadSafeCheckStatement("StaticMethod")
                .ExpectFunctionCallStatement("SomeFreeFunction")
                .ExpectBraceEnd()
                .End();
        }

        [Test]
        public void FileNodeGeneration()
        {
            var n = new FileNode();
            n.DefaultNamespace.Functions = new List<FunctionNode>
            {
                new FunctionNode {Name = "SomeMethodName", ReturnType = "SomeReturnType"},
                new FunctionNode {Name = "OtherMethodName", ReturnType = "OtherReturnType"},
            };

            n.AddInclude("Runtime/SomeFile.h");
            n.AddInclude("Runtime/SomeOtherFile.h");

            var code = new CppGenerator().Generate(n);

            new AssertingStringMatcher(code)
                .ExpectInclude("Runtime/SomeFile.h")
                .ExpectInclude("Runtime/SomeOtherFile.h")
                .FindEmptyFunction("SomeReturnType", "SomeMethodName")
                .FindEmptyFunction("OtherReturnType", "OtherMethodName")
                .End();
        }

        [Test]
        public void DefaultNamespaceNodeGeneration()
        {
            var n = new FileNode();
            n.DefaultNamespace.Functions.Add(new FunctionNode {Name = "SomeMethodName", ReturnType = "SomeReturnType"});

            var code = new CppGenerator().Generate(n);
            Assert.False(code.Contains("namespace"));

            new AssertingStringMatcher(code)
                .FindEmptyFunction("SomeReturnType", "SomeMethodName")
                .End();
        }

        [Test]
        public void NamedNamespaceNodeGeneration()
        {
            var n = new FileNode
            {
                Namespaces =
                {
                    new NamespaceNode
                    {
                        Name = "Marshalled",
                        Functions = new List<FunctionNode>
                        {
                            new FunctionNode { Name = "SomeMethodName", ReturnType = "SomeReturnType" },
                        }
                    },
                    new NamespaceNode
                    {
                        Comment = "A Comment",
                        Name = "After",
                        TypeDefs = new List<TypeDefNode>
                        {
                            new TypeDefNode { SourceType = "SourceType", DestType = "DestType"}
                        }
                    }
                }
            };

            var code = new CppGenerator().Generate(n);

            new AssertingStringMatcher(code)
                .FindNamespaceStart("Marshalled")
                .FindEmptyFunction("SomeReturnType", "SomeMethodName")
                .ExpectBraceEnd()
                .FindComment("A Comment")
                .FindNamespaceStart("After")
                .FindTypedef("SourceType", "DestType")
                .ExpectBraceEnd()
                .End();
        }

        [Test]
        public void NamespaceNodeMemberGeneration()
        {
            var functionNode = new FunctionNode { Name = "SomeMethodName", ReturnType = "SomeReturnType" };
            var comment = "comment";
            var structDefinitionNode = new StructDefinitionNode { Name = "SomeStruct" };
            var typeDefNode = new TypeDefNode { SourceType = "source", DestType = "dest" };
            var n =
                new NamespaceNode
            {
                Comment = comment,
                Name = "Marshalled",
                Functions = { functionNode },
                Structs = {structDefinitionNode},
                TypeDefs = {typeDefNode}
            };

            var code = new CppGenerator().Generate(n);

            new AssertingStringMatcher(code)
                .FindComment(comment)
                .FindNamespaceStart("Marshalled")
                .OptionalWhitespace()
                .ExpectGeneratedOutput(typeDefNode, 0).OptionalWhitespace()
                .ExpectGeneratedOutput(structDefinitionNode, 0).OptionalWhitespace()
                .ExpectGeneratedOutput(functionNode, 0).OptionalWhitespace()
                .ExpectBraceEnd()
                .End();
        }

        [Test]
        public void StructNodeGenerationWithFields()
        {
            var s = new StructDefinitionNode
            {
                Name = "SomeName",

                Members = new List<FieldDeclarationStatementNode>
                {
                    new FieldDeclarationStatementNode { Type = "T1", Name = "N1" },
                    new FieldDeclarationStatementNode { Type = "T2", Name = "N2" },
                    new FieldDeclarationStatementNode { Type = "T3", Name = "N3" },
                },
            };

            var code = new CppGenerator().Generate(s);

            new AssertingStringMatcher(code)
                .OptionalWhitespace()
                .Expect("struct SomeName").OptionalWhitespace().ExpectBraceBegin()
                .ExpectVariableDeclarationStatement("T1", "N1")
                .ExpectVariableDeclarationStatement("T2", "N2")
                .ExpectVariableDeclarationStatement("T3", "N3")
                .ExpectBraceEnd().ExpectSemiColon()
                .End();
        }

        [Test]
        public void StructNodeGenerationWithForceUnique()
        {
            var s = new StructDefinitionNode
            {
                Name = "SomeName",
                ForceUniqueViaPreprocessor = true
            };

            var code = new CppGenerator().Generate(s);

            new AssertingStringMatcher(code)
                .OptionalWhitespace()
                .Expect("#ifndef SomeName_DEFINED\n#define SomeName_DEFINED\n")
                .Expect("struct SomeName").OptionalWhitespace().ExpectBraceBegin().OptionalWhitespace()
                .ExpectBraceEnd().ExpectSemiColon()
                .OptionalWhitespace()
                .Expect("#endif /*SomeName_DEFINED*/\n")
                .End();
        }

        [Test]
        public void StructNodeGenerationWithForceUniqueAndTemplateSpecializationEmitsCorrectMacroName()
        {
            var s = new StructDefinitionNode
            {
                Name = "SomeName",
                TemplateSpecialization = {"Spec1", "Spec2"},
                ForceUniqueViaPreprocessor = true
            };
            var expectedMacroName = "SomeName_Spec1_Spec2_DEFINED";

            var code = new CppGenerator().Generate(s);

            new AssertingStringMatcher(code)
                .OptionalWhitespace()
                .Expect($"#ifndef {expectedMacroName}\n#define {expectedMacroName}\n")
                .Find($"#endif /*{expectedMacroName}*/\n")
                .End();
        }

        [Test]
        public void StructNodeGenerationWithForceUniqueAndTemplateArgumentsAndSpecializationEmitsCorrectMacroName()
        {
            var s = new StructDefinitionNode
            {
                Name = "SomeName",
                IsTemplate = true,
                TemplateArguments = {"T1", "N::T2"},
                TemplateSpecialization = { "Spec1", "N::Spec2" },
                ForceUniqueViaPreprocessor = true
            };
            var expectedMacroName = "SomeName_T1_N_T2_Spec1_N_Spec2_DEFINED";

            var code = new CppGenerator().Generate(s);

            new AssertingStringMatcher(code)
                .OptionalWhitespace()
                .Expect($"#ifndef {expectedMacroName}\n#define {expectedMacroName}\n")
                .Find($"#endif /*{expectedMacroName}*/\n")
                .End();
        }

        [Test]
        public void FieldDeclarationStaticConst()
        {
            var code = new CppGenerator().Generate(new FieldDeclarationStatementNode
            {
                Name = "Foo",
                Type = "Bar",
                Initializer = new StringExpressionNode { Str = "Baz" },
                IsConst = true,
                IsStatic = true
            });

            new AssertingStringMatcher(code)
                .OptionalWhitespace()
                .Expect("static const Bar Foo = Baz")
                .ExpectSemiColon().OptionalWhitespace()
                .End();
        }

        [Test]
        public void FieldDeclarationElementCount()
        {
            var code = new CppGenerator().Generate(new FieldDeclarationStatementNode
            {
                Name = "Foo",
                Type = "Bar",
                ElementCount = 4
            });

            new AssertingStringMatcher(code)
                .OptionalWhitespace()
                .Expect("Bar Foo[4]")
                .ExpectSemiColon().OptionalWhitespace()
                .End();
        }

        [Test]
        public void TemplateNoParametersStructNodeGeneration()
        {
            var s = new StructDefinitionNode
            {
                Name = "SomeName",
                IsTemplate = true,
            };

            var code = new CppGenerator().Generate(s);

            new AssertingStringMatcher(code)
                .OptionalWhitespace()
                .Expect("template<>").OptionalWhitespace()
                .Expect("struct SomeName").OptionalWhitespace().ExpectBraceBegin()
                .ExpectBraceEnd().ExpectSemiColon()
                .End();
        }

        [Test]
        public void TemplateWithArgumentsStructNodeGeneration()
        {
            var s = new StructDefinitionNode
            {
                Name = "SomeName",
                IsTemplate = true,
                TemplateArguments = {"typename a", "typename b"}
            };

            var code = new CppGenerator().Generate(s);

            new AssertingStringMatcher(code)
                .OptionalWhitespace()
                .Expect("template<typename a").ExpectComma().OptionalWhitespace()
                .Expect("typename b>").OptionalWhitespace()
                .Expect("struct SomeName").OptionalWhitespace().ExpectBraceBegin()
                .ExpectBraceEnd().ExpectSemiColon()
                .End();
        }

        [Test]
        public void TemplateWithParametersStructNodeGeneration()
        {
            var s = new StructDefinitionNode
            {
                Name = "SomeName",
                TemplateSpecialization = {"A", "B"},
                TemplateArguments = { "typename a", "typename b" }
            };

            var code = new CppGenerator().Generate(s);

            new AssertingStringMatcher(code)
                .OptionalWhitespace()
                .Expect("struct SomeName").OptionalWhitespace()
                .Expect("<").OptionalWhitespace()
                .Expect("A").ExpectComma().OptionalWhitespace()
                .Expect("B").OptionalWhitespace()
                .Expect(">").OptionalWhitespace().ExpectBraceBegin()
                .ExpectBraceEnd().ExpectSemiColon()
                .End();
        }

        [Test]
        public void FunctionNodeGeneration()
        {
            var n = new FunctionNode { DeclSpec = "DECL", CallingConvention = "CALLING", Name = "SomeMethodName", ReturnType = "SomeReturnType"};
            var code = new CppGenerator().Generate(n);

            new AssertingStringMatcher(code)
                .FindEmptyFunction("SomeReturnType", "SomeMethodName", "DECL", "CALLING")
                .End();
        }

        [Test]
        public void FriendFunctionNodeGeneration()
        {
            var n = new FunctionNode { DeclSpec = "DECL", CallingConvention = "CALLING", Name = "SomeMethodName", ReturnType = "SomeReturnType", IsFriend = true};
            var code = new CppGenerator().Generate(n);

            new AssertingStringMatcher(code)
                .FindEmptyFunction("SomeReturnType", "SomeMethodName", "DECL", "CALLING", true)
                .End();
        }

        [Test]
        public void FunctionNodeGenerationWithArgument()
        {
            var n = new FunctionNode { Name = "MethodName", Parameters = new List<FunctionParameter> {new FunctionParameter { Type = "ParameterType", Name = "ParameterName"}}};
            var code = new CppGenerator().Generate(n);

            new AssertingStringMatcher(code)
                .Find("MethodName").OptionalWhitespace()
                .ExpectParensBegin()
                .ExpectVariableDeclaration("ParameterType", "ParameterName")
                .ExpectParensEnd();
        }

        [Test]
        public void FunctionNodeGenerationWithStatements()
        {
            var n = new FunctionNode { Name = "MethodName", Statements = new List<IStatementNode> { new StringStatementNode() { Str = "Statement1" }, new StringStatementNode() { Str = "Statement2" }, new StringStatementNode() { Str = "Statement3" } } };
            var code = new CppGenerator().Generate(n);

            new AssertingStringMatcher(code)
                .Find("MethodName").OptionalWhitespace().ExpectEmptyParens()
                .ExpectBraceBegin()
                .ExpectStatement("Statement1")
                .ExpectStatement("Statement2")
                .ExpectStatement("Statement3")
                .ExpectBraceEnd().End();
        }

        [Test]
        public void FunctionNodeGenerationWithTemplateSpecialization()
        {
            var n = new FunctionNode { Name = "SomeMethodName", ReturnType = "SomeReturnType", TemplateSpecialization = { "Foo" } };
            var code = new CppGenerator().Generate(n);

            new AssertingStringMatcher(code)
                .Find("template <>").ExpectWhitespace().Expect("SomeReturnType").ExpectWhitespace().Expect("SomeMethodName").Expect("<").Expect("Foo").Expect(">").OptionalWhitespace().ExpectEmptyParens()
                .ExpectBraceBegin()
                .ExpectBraceEnd()
                .End();
        }

        [Test]
        public void FunctionNodeGenerationWithTemplateSpecializationAndUseDefineSentinel()
        {
            var n = new FunctionNode { Name = "SomeMethodName", ReturnType = "SomeReturnType", TemplateSpecialization = { "Foo", "Bar" }, UseDefineSentinel = true};
            var code = new CppGenerator().Generate(n);

            new AssertingStringMatcher(code)
                .Find("#ifndef SOMEMETHODNAME_FOO_BAR\n#define SOMEMETHODNAME_FOO_BAR").OptionalWhitespace()
                .Expect("template <>").ExpectWhitespace().Expect("SomeReturnType").ExpectWhitespace().Expect("SomeMethodName").Expect("<Foo, Bar>").OptionalWhitespace().ExpectEmptyParens()
                .ExpectBraceBegin()
                .ExpectBraceEnd()
                .Expect("#endif /*SOMEMETHODNAME_FOO_BAR*/").OptionalWhitespace()
                .End();
        }

        [Test]
        public void ExpressionStatementNodeGeneration()
        {
            var n = new ExpressionStatementNode { Expression = new StringExpressionNode {Str = "SomeExpression"}};
            var code = new CppGenerator().Generate((IStatementNode)n); // cast: we want to through fanout method

            new AssertingStringMatcher(code)
                .OptionalWhitespace().ExpectStatement("SomeExpression").End();
        }

        [Test]
        public void ReturnStatementNodeGeneration()
        {
            var n = new ReturnStatementNode { Expression = new StringExpressionNode { Str = "SomeExpression" } };
            var code = new CppGenerator().Generate((IStatementNode)n); // cast: we want to through fanout method

            new AssertingStringMatcher(code)
                .OptionalWhitespace().ExpectReturn().ExpectStatement("SomeExpression").End();
        }

        [Test]
        public void VoidReturnStatementNodeGeneration()
        {
            var n = new ReturnStatementNode { Expression = null };
            var code = new CppGenerator().Generate((IStatementNode)n); // cast: we want to through fanout method

            new AssertingStringMatcher(code)
                .OptionalWhitespace().Expect("return;").OptionalWhitespace().End();
        }

        [Test]
        public void StringStatementNodeGeneration()
        {
            var n = new StringStatementNode { Str = "SomeStatement" };
            var code = new CppGenerator().Generate((IStatementNode)n); // cast: we want to through fanout method

            new AssertingStringMatcher(code)
                .OptionalWhitespace().ExpectStatement("SomeStatement").End();
        }

        [Test]
        public void VariableDeclarationStatementNodeGeneration()
        {
            var n = new VariableDeclarationStatementNode { Type = "SomeType", Name = "SomeName" };
            var code = new CppGenerator().Generate((IStatementNode)n); // cast: we want to through fanout method

            new AssertingStringMatcher(code)
                .OptionalWhitespace().Expect("SomeType").ExpectWhitespace().ExpectIdentifier("SomeName").ExpectSemiColon().End();
        }

        [Test]
        public void VariableDeclarationStatementNodeWithInitializerGeneration()
        {
            var n = new VariableDeclarationStatementNode { Type = "SomeType", Name = "SomeName", Initializer = new StringExpressionNode {Str = "SomeExpression"}};
            var code = new CppGenerator().Generate((IStatementNode)n); // cast: we want to through fanout method

            new AssertingStringMatcher(code)
                .OptionalWhitespace().Expect("SomeType").ExpectWhitespace().ExpectIdentifier("SomeName")
                .ExpectEqualSign()
                .Expect("SomeExpression")
                .ExpectSemiColon().End();
        }

        [Test]
        public void IfStatementNodeGeneration()
        {
            var n = new IfStatementNode
            {
                Condition = new StringExpressionNode {Str = "SomeCondition"},
                Statements =
                    new List<IStatementNode>
                {
                    new StringStatementNode() {Str = "Statement1"},
                    new StringStatementNode() {Str = "Statement2"},
                }
            };
            var code = new CppGenerator().Generate(n);

            new AssertingStringMatcher(code)
                .OptionalWhitespace().Expect("if").OptionalWhitespace().ExpectParensBegin().Expect("SomeCondition").OptionalWhitespace().ExpectParensEnd()
                .ExpectBraceBegin()
                .ExpectStatement("Statement1")
                .ExpectStatement("Statement2")
                .ExpectBraceEnd().End();
        }

        [Test]
        public void AssignNodeGeneration()
        {
            var n = new AssignNode { Lhs = new StringExpressionNode { Str = "SomeName" }, Rhs = new StringExpressionNode {Str = "SomeExpression"}};
            var code = new CppGenerator().Generate((IExpressionNode)n); // cast: we want to through fanout method

            new AssertingStringMatcher(code)
                .ExpectIdentifier("SomeName")
                .ExpectEqualSign()
                .Expect("SomeExpression").End();
        }

        [Test]
        public void StaticFunctionCallNodeGeneration()
        {
            var n = new FunctionCallNode
            {
                Qualifier = new StaticMemberQualifier { ClassName = "SomeClassName" },
                FunctionName = "SomeFunctionName"
            };
            var code = new CppGenerator().Generate((IExpressionNode)n); // cast: we want to through fanout method

            new AssertingStringMatcher(code)
                .OptionalWhitespace().ExpectFunctionCall("SomeClassName::SomeFunctionName").End();
        }

        [Test]
        public void FreeFunctionCallNodeGeneration()
        {
            var n = new FunctionCallNode { Qualifier = FreeQualifier.Instance, FunctionName = "SomeFunctionName" };
            var code = new CppGenerator().Generate((IExpressionNode)n); // cast: we want to through fanout method

            new AssertingStringMatcher(code)
                .OptionalWhitespace().ExpectFunctionCall("SomeFunctionName").End();
        }

        [Test]
        public void MemberFunctionCallNodeGeneration()
        {
            var n = new FunctionCallNode
            {
                Qualifier = new InstanceMemberQualifier { Expression = "SomeInstanceName" } ,
                FunctionName = "SomeFunctionName"
            };
            var code = new CppGenerator().Generate((IExpressionNode)n); // cast: we want to through fanout method

            new AssertingStringMatcher(code)
                .OptionalWhitespace().ExpectFunctionCall("SomeInstanceName->SomeFunctionName").End();
        }

        public void MemberFunctionCallOnStaticAccessorReferenceGeneration()
        {
            var n = new FunctionCallNode
            {
                Qualifier = new InstanceMemberQualifier { Expression = "GetMyInstance()", IsReference = true },
                FunctionName = "SomeFunctionName"
            };
            var code = new CppGenerator().Generate((IExpressionNode)n); // cast: we want to through fanout method

            var expectedInvocation = string.Format("GetMyInstance(){0}SomeFunctionName", ".");
            new AssertingStringMatcher(code)
                .OptionalWhitespace().ExpectFunctionCall(expectedInvocation).End();
        }

        public void MemberFunctionCallOnStaticAccessorPointerGeneration()
        {
            var n = new FunctionCallNode
            {
                Qualifier = new InstanceMemberQualifier { Expression = "GetMyInstance()", IsReference = false},
                FunctionName = "SomeFunctionName"
            };
            var code = new CppGenerator().Generate((IExpressionNode)n); // cast: we want to through fanout method

            var expectedInvocation = string.Format("GetMyInstance(){0}SomeFunctionName", "->");
            new AssertingStringMatcher(code)
                .OptionalWhitespace().ExpectFunctionCall(expectedInvocation).End();
        }

        [Test]
        public void FreeFunctionCallNodeWithArgumentsGeneration()
        {
            var n = new FunctionCallNode
            {
                Qualifier = FreeQualifier.Instance,
                FunctionName = "SomeFunctionName", Arguments = new List<FunctionArgument> { new FunctionArgument {Name = "SomeArgumentName"}}
            };
            var code = new CppGenerator().Generate((IExpressionNode)n); // cast: we want to through fanout method

            new AssertingStringMatcher(code)
                .OptionalWhitespace().ExpectIdentifier("SomeFunctionName").ExpectParensBegin()
                .ExpectIdentifier("SomeArgumentName")
                .ExpectParensEnd().End();
        }

        [Test]
        public void StringExpressionNodeGeneration()
        {
            var n = new StringExpressionNode { Str = "SomeExpression" };
            var code = new CppGenerator().Generate((IExpressionNode)n); // cast: we want to through fanout method

            new AssertingStringMatcher(code)
                .OptionalWhitespace().Expect("SomeExpression").OptionalWhitespace().End();
        }

        [Test]
        public void FunctionParameterGeneration()
        {
            var n = new List<FunctionParameter>
            {
                new FunctionParameter {Type = "TypeA", Name = "NameA"},
                new FunctionParameter {Type = "TypeB", Name = "NameB"},
                new FunctionParameter {Type = "TypeC", Name = "NameC"},
            };
            var code = new CppGenerator().Generate(n);

            new AssertingStringMatcher(code)
                .OptionalWhitespace()
                .ExpectVariableDeclaration("TypeA", "NameA").ExpectComma()
                .ExpectVariableDeclaration("TypeB", "NameB").ExpectComma()
                .ExpectVariableDeclaration("TypeC", "NameC")
                .End();
        }

        [Test]
        public void FunctionArgumentGeneration()
        {
            var n = new List<FunctionArgument>
            {
                new FunctionArgument {Name = "NameA"},
                new FunctionArgument {Name = "NameB"},
                new FunctionArgument {Name = "NameC"},
            };
            var code = new CppGenerator().Generate(n);

            new AssertingStringMatcher(code)
                .OptionalWhitespace()
                .ExpectIdentifier("NameA").ExpectComma()
                .ExpectIdentifier("NameB").ExpectComma()
                .ExpectIdentifier("NameC")
                .OptionalWhitespace().End();
        }

        [Test]
        public void StubStatementGeneration()
        {
            var n = new StubStatementNode
            {
                Condition = new StringExpressionNode {Str = "SomeCondition"},
                Statements =
                    new List<IStatementNode>
                {
                    new StringStatementNode() {Str = "Statement1"},
                    new StringStatementNode() {Str = "Statement2"},
                },
                StatementsForStub = { new ReturnStatementNode {Expression = new StringExpressionNode {Str = "19"}}}
            };
            var code = new CppGenerator().Generate(n);

            new AssertingStringMatcher(code)
                .OptionalWhitespace().Expect("#if").ExpectWhitespace().Expect("SomeCondition").OptionalWhitespace()
                .ExpectStatement("Statement1")
                .ExpectStatement("Statement2")
                .Expect("#else").ExpectWhitespace()
                .ExpectStatement("return 19")
                .Expect("#endif").OptionalWhitespace().End();
        }

        [Test]
        public void StubStatementGenerationWithoutReturn()
        {
            var n = new StubStatementNode
            {
                Condition = new StringExpressionNode {Str = "SomeCondition"},
                Statements =
                    new List<IStatementNode>
                {
                    new StringStatementNode() {Str = "Statement1"},
                    new StringStatementNode() {Str = "Statement2"},
                },
                StatementsForStub = null
            };
            var code = new CppGenerator().Generate(n);

            new AssertingStringMatcher(code)
                .OptionalWhitespace().Expect("#if").ExpectWhitespace().Expect("SomeCondition").OptionalWhitespace()
                .ExpectStatement("Statement1")
                .ExpectStatement("Statement2")
                .Expect("#endif").OptionalWhitespace().End();
        }

        [Test]
        public void FieldReferenceNodeGeneration()
        {
            var n = new FieldReferenceNode
            {
                Qualifier = new InstanceMemberQualifier
                {
                    Expression = "GetObject()",
                    IsReference = false
                },
                Name = "foo"
            };
            var expectedCode = "GetObject()->foo";
            var code = new CppGenerator().Generate(n);

            Assert.AreEqual(expectedCode, code);
        }

        [Test]
        public void StaticMemberQualifierGeneration()
        {
            var n = new StaticMemberQualifier()
            {
                ClassName = "Foo"
            };
            var expectedCode = "Foo::";
            var code = new CppGenerator().Generate(n);

            Assert.AreEqual(expectedCode, code);
        }

        [Test]
        [TestCase("Foo()", false, "Foo()->")]
        [TestCase("Foo", true, "Foo.")]
        public void InstanceMemberQualifierGeneration(string expression, bool isReference, string expectedCode)
        {
            var n = new InstanceMemberQualifier()
            {
                Expression = expression,
                IsReference = isReference
            };
            var code = new CppGenerator().Generate(n);

            Assert.AreEqual(expectedCode, code);
        }

        [Test]
        public void Indent()
        {
            var indent = new CppGenerator.Indent();

            Assert.AreEqual("", indent.ToString());
            Assert.AreEqual("", (string)indent);
            Assert.IsTrue(indent == "");

            indent++;
            Assert.IsTrue(indent == "\t");

            indent++;
            Assert.IsTrue(indent == "\t\t");

            indent--;
            Assert.IsTrue(indent == "\t");

            indent--;
            Assert.IsTrue(indent == "");

            indent--;
            Assert.IsTrue(indent == "");

            indent++;
            Assert.IsTrue(indent == "\t");

            Assert.AreEqual("\t", String.Format("{0}", indent));
        }

        [Test]
        public void BlockNodeGeneration()
        {
            var n = new BlockNode
            {
                Statements =
                    new List<IStatementNode>
                {
                    new StringStatementNode {Str = "Statement1"},
                    new StringStatementNode {Str = "Statement2"},
                }
            };
            var code = new CppGenerator().Generate(n);

            new AssertingStringMatcher(code)
                .OptionalWhitespace()
                .ExpectBraceBegin()
                .ExpectStatement("Statement1")
                .ExpectStatement("Statement2")
                .ExpectBraceEnd().End();
        }

        [Test]
        public void LabelNodeGeneration()
        {
            var n = new LabelNode { Name = "SomeLabel" };
            var code = new CppGenerator().Generate((IStatementNode)n); // cast: we want to through fanout method

            new AssertingStringMatcher(code)
                .Expect("SomeLabel").ExpectColon().OptionalWhitespace().End();
        }
    }
}
