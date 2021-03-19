using System;
using System.Collections.Generic;
using System.Linq;
using System.Runtime.CompilerServices;
using System.Runtime.InteropServices;
using NUnit.Framework;
using Unity.BindingsGenerator.TestFramework;
using Unity.BindingsGenerator.Core;
using UnityEngine.Bindings;
using Unity.BindingsGenerator.Core.Nodes;

namespace Unity.BindingsGenerator.Tests
{
    [TestFixture]
    internal class MarshallingStructTests : BindingsGeneratorTestsBase
    {
        #pragma warning disable 169
        #pragma warning disable 649

        public struct Simple
        {
            public int i;
            public float j;
        }

        public class SimpleUser
        {
            [MethodImpl(MethodImplOptions.InternalCall)]
            public static extern void Use(Simple param);
        }

        public class SimpleRefUser
        {
            [MethodImpl(MethodImplOptions.InternalCall)]
            public static extern void Use(ref Simple param);
        }

        public struct StructWithString
        {
            public string a;
        }

        public class StructWithStringUser
        {
            [MethodImpl(MethodImplOptions.InternalCall)]
            public static extern void Use(StructWithString param);
        }

        public class StructWithStringOutUser
        {
            [MethodImpl(MethodImplOptions.InternalCall)]
            public static extern void Use(out StructWithString param);
        }

        public class StructWithStringRefUser
        {
            [MethodImpl(MethodImplOptions.InternalCall)]
            public static extern void Use(ref StructWithString param);
        }

        public struct StructWithObject
        {
            public object a;
        }

        public class StructWithObjectUser
        {
            [MethodImpl(MethodImplOptions.InternalCall)]
            public static extern void Use1(StructWithObject param);

            [MethodImpl(MethodImplOptions.InternalCall)]
            public static extern void Use2(StructWithObject param);
        }

        public struct StructWithIntArray
        {
            public int[] a;
        }

        public class StructWithIntArrayUser
        {
            [MethodImpl(MethodImplOptions.InternalCall)]
            public static extern void Use(StructWithIntArray param);
        }

        public struct StructWithSimpleArray
        {
            public Simple[] a;
        }

        public class StructWithSimpleArrayUser
        {
            [MethodImpl(MethodImplOptions.InternalCall)]
            public static extern void Use(StructWithSimpleArray param);
        }

        public struct StructWithNonPodStructArray
        {
            public StructWithString[] a;
        }

        public class StructWithNonPodStructArrayUser
        {
            [MethodImpl(MethodImplOptions.InternalCall)]
            public static extern void Use(StructWithNonPodStructArray param);
        }

        public struct StructWithSimpleStruct
        {
            public Simple a;
        }

        public class StructWithSimpleStructUser
        {
            [MethodImpl(MethodImplOptions.InternalCall)]
            public static extern void Use(StructWithSimpleStruct param);
        }

        public struct StructWithNonPodStruct
        {
            private StructWithString a;
        }

        public class StructWithNonPodStructUser
        {
            [MethodImpl(MethodImplOptions.InternalCall)]
            public static extern void Use(StructWithNonPodStruct param);
        }

        [NativeType(CodegenOptions.Force)]
        public struct SimpleStructWithFieldNameOverride
        {
            [NativeName("Bar")]
            public int a;
        }

        public class SimpleStructWithFieldNameOverrideUser
        {
            [MethodImpl(MethodImplOptions.InternalCall)]
            public static extern void Use(SimpleStructWithFieldNameOverride param);
        }

        [NativeType(CodegenOptions = CodegenOptions.Force)]
        public struct SimpleStructWithForceGenerate
        {
            public int a;
        }

        public class SimpleStructWithForceGenerateUser
        {
            [MethodImpl(MethodImplOptions.InternalCall)]
            public static extern void Use(SimpleStructWithForceGenerate param);
        }

        [NativeType(CodegenOptions = CodegenOptions.Custom)]
        public struct NonPodStructWithUseCustom
        {
            public string a;
        }

        public class NonPodStructWithUseCustomUser
        {
            [MethodImpl(MethodImplOptions.InternalCall)]
            public static extern void Use(NonPodStructWithUseCustom param);
        }

        [NativeType(CodegenOptions = CodegenOptions.Custom)]
        public struct CustomStructUsingNonBlittableStruct
        {
            public StructWithString a;
        }

        public class CustomStructUsingNonBlittableStructUser
        {
            [MethodImpl(MethodImplOptions.InternalCall)]
            public static extern void Use(CustomStructUsingNonBlittableStruct param);
        }

        [NativeType(Header = "Foo.h")]
        public struct StructWithHeader
        {
            public string a;
        }

        public class StructWithHeaderUser
        {
            [MethodImpl(MethodImplOptions.InternalCall)]
            public static extern void Use(StructWithHeader param);
        }

        public class SimpleReturner
        {
            [MethodImpl(MethodImplOptions.InternalCall)]
            public static extern Simple Return();
        }

        public class StructWithStringReturner
        {
            [MethodImpl(MethodImplOptions.InternalCall)]
            public static extern StructWithString Return();
        }

        public struct StructWithExternMethod
        {
            [MethodImpl(MethodImplOptions.InternalCall)]
            public static extern void Test();
        }

        public unsafe struct StructWithFixedBuffer
        {
            fixed int SomeInts[32];
        }

        public class StructWithFixedBufferUser
        {
            [MethodImpl(MethodImplOptions.InternalCall)]
            public static extern void Test(StructWithFixedBuffer param);
        }

        public unsafe struct NonBlittableStructWithFixedBuffer
        {
            string a;
            fixed int SomeInts[32];
        }

        public class NonBlittableStructWithFixedBufferUser
        {
            [MethodImpl(MethodImplOptions.InternalCall)]
            public static extern void Test(NonBlittableStructWithFixedBuffer param);
        }

        public class IntPtrType
        {
            public IntPtr intPtr;
        }

        public class IntPtrTypeUser
        {
            [MethodImpl(MethodImplOptions.InternalCall)]
            public static extern void Use(IntPtrType param);
        }

        public struct StructWithStaticField
        {
            public string a;
            public static readonly int i;
            public const int j = 1;
        }

        public class StructWithStaticFieldUser
        {
            [MethodImpl(MethodImplOptions.InternalCall)]
            public static extern void Use(StructWithStaticField param);
        }

        private void TestStructBinding(FileNode actual, FunctionNode functionNode, string functionName, string expectedType, string expectedTypeWithNamespace, bool Pod = false, bool isReference = false, string[] expectedOtherStructs = null)
        {
            var bindingsExpectedType = Naming.BindingsTypePrefix + "Unity_BindingsGenerator_Tests_MarshallingStructTests_" + expectedType;
            string parameterType = Pod ? bindingsExpectedType : Naming.IntermediateTypeName(expectedType);

            var parameterExpectation = new List<FunctionParameter>
            {
                new FunctionParameter {Type = isReference ? Naming.Reference(parameterType) : Naming.ConstReference(parameterType)}
            };
            AssertEqual(parameterExpectation, functionNode.Parameters);

            if (!Pod)
            {
                var declarationExpectation = new VariableDeclarationStatementNode
                {
                    Name = "param".Marshalled(),
                    Type = isReference ? Naming.Reference(bindingsExpectedType) : bindingsExpectedType,
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
                            new FunctionArgument {Name = Naming.AddressOf("param".Marshalled())},
                            new FunctionArgument {Name = Naming.AddressOf("param")},
                        }
                    }
                };
                Assert.IsTrue(FunctionContainsStatement(marshalExpectation, functionNode));
                string[] expectedStructs = { expectedTypeWithNamespace };
                if (expectedOtherStructs != null)
                    expectedStructs = expectedStructs.Concat(expectedOtherStructs).ToArray();

                Assert.IsTrue(FileContainsTypeVerifications(actual, expectedStructs));
            }

            var argumentName = Pod ? "param" : "param".Marshalled();

            var callExpectation = new ExpressionStatementNode()
            {
                Expression = new FunctionCallNode
                {
                    Qualifier = new StaticMemberQualifier(),
                    FunctionName = functionName,
                    Arguments = new List<FunctionArgument> {new FunctionArgument {Name = argumentName}}
                }
            };
            Assert.IsTrue(FunctionContainsStatement(callExpectation, functionNode));
        }

        private void TestStruct(TreeGenerator tree, StructDefinitionNode actualStruct, string expectedStructType, string expectedStructTypeWithNamespace, string expectedMemberType, string fieldName = "a", string[] expectedOtherStructs = null, bool checkTypeVerifications = true)
        {
            var intermediateStructName = Naming.IntermediateTypeName(expectedStructType);
            var expectedBindingsStructType = Naming.BindingsTypePrefix + "Unity_BindingsGenerator_Tests_MarshallingStructTests_" + expectedStructType;
            var expectedStruct = new StructDefinitionNode
            {
                Name = intermediateStructName,
                ForceUniqueViaPreprocessor = true,
                Members = new List<FieldDeclarationStatementNode>
                {
                    new FieldDeclarationStatementNode
                    {
                        Name = "a",
                        Type = expectedMemberType
                    }
                }
            };
            AssertEqual(expectedStruct, actualStruct);

            var actualMarshallingNamespace = MarshalingNamespaceFromTree(tree);
            var expectedMarshalFunction = new FunctionNode
            {
                UseDefineSentinel = true,
                Name = "Marshal",
                TemplateSpecialization = null,
                Parameters = new List<FunctionParameter>
                {
                    new FunctionParameter
                    {
                        Name = "marshalled",
                        Type = expectedBindingsStructType.Pointer()
                    },
                    new FunctionParameter
                    {
                        Name = "unmarshalled",
                        Type = intermediateStructName.ConstPointer()
                    }
                },
                Statements = new List<IStatementNode>
                {
                    new ExpressionStatementNode
                    {
                        Expression = new FunctionCallNode
                        {
                            Qualifier = new StaticMemberQualifier { ClassName = "Marshalling" },
                            FunctionName = "Marshal",
                            Arguments = new List<FunctionArgument>
                            {
                                new FunctionArgument {Name = string.Format("&(marshalled->{0})", fieldName)},
                                new FunctionArgument {Name = string.Format("&(unmarshalled->{0})", "a")}
                            }
                        }
                    }
                },
                IsInline = true
            };
            AssertContains(expectedMarshalFunction, actualMarshallingNamespace.Functions);

            var expectedUnmarshalFunction = new FunctionNode
            {
                UseDefineSentinel = true,
                Name = "Unmarshal",
                TemplateSpecialization = null,
                Parameters = new List<FunctionParameter>
                {
                    new FunctionParameter
                    {
                        Name = "unmarshalled",
                        Type = intermediateStructName.Pointer()
                    },
                    new FunctionParameter
                    {
                        Name = "marshalled",
                        Type = expectedBindingsStructType.ConstPointer()
                    }
                },
                Statements = new List<IStatementNode>
                {
                    new ExpressionStatementNode
                    {
                        Expression = new FunctionCallNode
                        {
                            Qualifier = new StaticMemberQualifier { ClassName = "Marshalling" },
                            FunctionName = "Unmarshal",
                            Arguments = new List<FunctionArgument>
                            {
                                new FunctionArgument {Name = string.Format("&(unmarshalled->{0})", "a")},
                                new FunctionArgument {Name = string.Format("&(marshalled->{0})", fieldName)}
                            }
                        }
                    }
                },
                IsInline = true
            };
            AssertContains(expectedUnmarshalFunction, actualMarshallingNamespace.Functions);

            string[] expectedTypes = { expectedStructTypeWithNamespace };
            if (expectedOtherStructs != null)
                expectedTypes = expectedTypes.Concat(expectedOtherStructs).ToArray();

            Assert.AreEqual(expectedTypes.Length, actualMarshallingNamespace.Structs.Count);
            AssertMarshalInfoExists(intermediateStructName, actualMarshallingNamespace, true, false);

            if (checkTypeVerifications)
                Assert.IsTrue(FileContainsTypeVerifications(tree.SourceFile, expectedTypes));
        }

        private void TestReturn(FileNode actual, FunctionNode functionNode, string functionName, string expectedType, string expectedTypeWithNamespaces, bool pod = false)
        {
            var bindingsExpectedType = Naming.BindingsTypePrefix + "Unity_BindingsGenerator_Tests_MarshallingStructTests_" + expectedType;
            var parameterType = pod ? bindingsExpectedType : Naming.IntermediateTypeName(expectedType);
            var requiresMarshalling = !pod;

            if (requiresMarshalling)
            {
                var parameterExpectation = new List<FunctionParameter>
                {
                    new FunctionParameter {Type = Naming.Reference(parameterType), IsOutReturn = true}
                };
                AssertEqual(parameterExpectation, functionNode.Parameters);

                var declarationExpectation = new VariableDeclarationStatementNode
                {
                    Name = Naming.ReturnVar.Marshalled(),
                    Type = bindingsExpectedType,
                    Initializer = new FunctionCallNode
                    {
                        Qualifier = new StaticMemberQualifier(),
                        FunctionName = "Return"
                    }
                };
                Assert.IsTrue(FunctionContainsStatement(declarationExpectation, functionNode));

                var marshalExpectation = new ExpressionStatementNode
                {
                    Expression = new FunctionCallNode
                    {
                        Qualifier = new StaticMemberQualifier { ClassName = "Marshalling" },
                        FunctionName = "Unmarshal",
                        Arguments =
                        {
                            new FunctionArgument {Name = Naming.AddressOf(Naming.ReturnVar)},
                            new FunctionArgument {Name = Naming.AddressOf(Naming.ReturnVar.Marshalled())},
                        }
                    }
                };
                Assert.IsTrue(FunctionContainsStatement(marshalExpectation, functionNode));
                Assert.IsTrue(FileContainsTypeVerifications(actual, expectedTypeWithNamespaces));
            }
            else
            {
                var assignmentExpectation = new ExpressionStatementNode
                {
                    Expression = new AssignNode
                    {
                        Lhs = new StringExpressionNode { Str = Naming.ReturnVar },
                        Rhs = new FunctionCallNode
                        {
                            Qualifier = new StaticMemberQualifier(),
                            FunctionName = "Return"
                        }
                    }
                };
                Assert.IsTrue(FunctionContainsStatement(assignmentExpectation, functionNode));
            }
        }

        [Test]
        public void SimpleStructWorks()
        {
            Assert.AreEqual(true, TypeUtils.IsNativePodType(TypeResolver.GetTypeReference<Simple>(), ScriptingBackend.Mono));

            var actualTree = GeneratedTreeFor<SimpleUser>();
            var actual = actualTree.SourceFile;
            var actualFunction = actual.FindFunction(Naming.ByRefInjectedMethod("Use"));
            TestStructBinding(actual, actualFunction, "Use", typeof(Simple).Name, typeof(Simple).FullnameWithUnderscores(), Pod: true);
            Assert.AreEqual(0, actual.DefaultNamespace.Structs.Count);

            AssertMarshalInfoExists(
                Naming.IntermediateTypeName(nameof(Simple)),
                MarshalingNamespaceFromTree(actualTree),
                false,
                true);
        }

        [Test]
        public void SimpleStructRefWorks()
        {
            var actualTree = GeneratedTreeFor<SimpleRefUser>();
            var actual = actualTree.SourceFile;
            var actualFunction = actual.FindFunction("Use");
            TestStructBinding(actual, actualFunction, "Use", typeof(Simple).Name, typeof(Simple).FullnameWithUnderscores(), Pod: true, isReference: true);
        }

        [Test]
        [TestCase(typeof(StructWithStringRefUser), TypeUsage.Parameter)]
        [TestCase(typeof(StructWithStringOutUser), TypeUsage.OutParameter)]
        public void StructWithStringRefWorks(Type type, TypeUsage usage)
        {
            var actualTree = GeneratedTreeFor(type);
            var actual = actualTree.SourceFile;
            var actualFunction = actual.FindFunction("Use");

            var expectedParameter = new FunctionParameter(name: "param", type: Naming.Reference(Naming.IntermediateTypeName(nameof(StructWithString))))
            {
                Usage = usage
            };
            var expectedUnmarshalFunctionCall = new ExpressionStatementNode(
                new FunctionCallNode
                {
                    FunctionName = Naming.UnmarshalFunctionName,
                    Arguments =
                    {
                        new FunctionArgument(),
                        new FunctionArgument()
                    }
                });
            AssertEqual(expectedParameter, actualFunction.Parameters[0]);
            FunctionContainsStatement(expectedUnmarshalFunctionCall, actualFunction);
        }

        [Test]
        [TestCase(typeof(StructWithString), typeof(StructWithStringUser))]
        [TestCase(typeof(StructWithStaticField), typeof(StructWithStaticFieldUser))]
        public void NonBlittableStructWorks(Type structType, Type userType)
        {
            Assert.AreEqual(false, TypeUtils.IsNativePodType(TypeResolver.GetTypeReference(structType), ScriptingBackend.Mono));

            var actualTree = GeneratedTreeFor(userType);
            var actualFunction = actualTree.SourceFile.FindFunction(Naming.ByRefInjectedMethod("Use"));

            TestStructBinding(actualTree.SourceFile, actualFunction, "Use", structType.Name, structType.FullnameWithUnderscores());

            var actualStruct = actualTree.HeaderFile.DefaultNamespace.Structs[0];
            TestStruct(actualTree, actualStruct, structType.Name, structType.FullnameWithUnderscores(), "Marshalling::StringStructField");
        }

        [Test]
        public void NonblittableStructGeneratesNamespacesInOrder()
        {
            var actualTree = GeneratedTreeFor<StructWithStringUser>();
            var actualNamespaces = actualTree.SourceFile.Namespaces;
            Assert.AreEqual(3, actualNamespaces.Count);
            Assert.AreEqual(Naming.RegistrationChecksNamespace, actualNamespaces[0].Name);
            Assert.AreEqual(Naming.MarshallingNamespace, actualNamespaces[1].Name);
            Assert.AreEqual(string.Empty, actualNamespaces[2].Name);
        }

        [Test(Description = "Test that a struct with a UnityEngine.Object reference works")]
        public void StructWithObjectWorks()
        {
            Assert.AreEqual(false, TypeUtils.IsNativePodType(TypeResolver.GetTypeReference<StructWithObject>(), ScriptingBackend.Mono));

            var actualTree = GeneratedTreeFor<StructWithObjectUser>();
            var actualFunction = actualTree.SourceFile.FindFunction(Naming.ByRefInjectedMethod("Use1"));

            TestStructBinding(actualTree.SourceFile, actualFunction, "Use1", typeof(StructWithObject).Name, typeof(StructWithObject).FullnameWithUnderscores());

            var actualStruct = actualTree.HeaderFile.DefaultNamespace.Structs[0];
            TestStruct(actualTree, actualStruct, typeof(StructWithObject).Name, typeof(StructWithObject).FullnameWithUnderscores(), "Marshalling::ManagedObjectStructField");
        }

        [Test(Description = "Test that a struct with a int array works")]
        public void StructWithIntArrayWorks()
        {
            Assert.AreEqual(false, TypeUtils.IsNativePodType(TypeResolver.GetTypeReference<StructWithIntArray>(), ScriptingBackend.Mono));

            var actualTree = GeneratedTreeFor<StructWithIntArrayUser>();
            var actualFunction = actualTree.SourceFile.FindFunction(Naming.ByRefInjectedMethod("Use"));

            TestStructBinding(actualTree.SourceFile, actualFunction, "Use", typeof(StructWithIntArray).Name, typeof(StructWithIntArray).FullnameWithUnderscores());

            var actualStruct = actualTree.HeaderFile.DefaultNamespace.Structs[0];
            TestStruct(actualTree, actualStruct, typeof(StructWithIntArray).Name, typeof(StructWithIntArray).FullnameWithUnderscores(), "Marshalling::ArrayStructField<SInt32>");
        }

        [Test(Description = "Test that a struct with a pod struct array works")]
        public void StructWithSimpleArrayWorks()
        {
            Assert.AreEqual(false, TypeUtils.IsNativePodType(TypeResolver.GetTypeReference<StructWithSimpleArray>(), ScriptingBackend.Mono));

            var actualTree = GeneratedTreeFor<StructWithSimpleArrayUser>();
            var actualFunction = actualTree.SourceFile.FindFunction(Naming.ByRefInjectedMethod("Use"));

            TestStructBinding(actualTree.SourceFile, actualFunction, "Use", nameof(StructWithSimpleArray), typeof(StructWithSimpleArray).FullnameWithUnderscores(), expectedOtherStructs: new string[] { typeof(Simple).FullnameWithUnderscores() });

            var actualStruct = actualTree.HeaderFile.DefaultNamespace.Structs.First(s => s.Name.Contains(nameof(StructWithSimpleArray)));
            TestStruct(actualTree, actualStruct, typeof(StructWithSimpleArray).Name, typeof(StructWithSimpleArray).FullnameWithUnderscores(), $"Marshalling::ArrayStructField<{Naming.IntermediateTypeName(nameof(Simple))}>", expectedOtherStructs: new string[] { typeof(Simple).FullnameWithUnderscores() });
        }

        [Test(Description = "Test that a struct with a non pod struct array works")]
        public void StructWithNonPodArrayWorks()
        {
            Assert.AreEqual(false, TypeUtils.IsNativePodType(TypeResolver.GetTypeReference<StructWithNonPodStructArray>(), ScriptingBackend.Mono));

            var actualTree = GeneratedTreeFor<StructWithNonPodStructArrayUser>();
            var actualFunction = actualTree.SourceFile.FindFunction(Naming.ByRefInjectedMethod("Use"));

            TestStructBinding(actualTree.SourceFile, actualFunction, "Use", typeof(StructWithNonPodStructArray).Name, typeof(StructWithNonPodStructArray).FullnameWithUnderscores(), expectedOtherStructs: new string[] { typeof(StructWithString).FullnameWithUnderscores() });

            var actualStruct1 = actualTree.HeaderFile.DefaultNamespace.Structs[0];
            TestStruct(actualTree, actualStruct1, typeof(StructWithString).Name, typeof(StructWithString).FullnameWithUnderscores(), "Marshalling::StringStructField", expectedOtherStructs: new string[] { typeof(StructWithNonPodStructArray).FullnameWithUnderscores() });

            var actualStruct2 = actualTree.HeaderFile.DefaultNamespace.Structs[1];
            TestStruct(actualTree, actualStruct2, typeof(StructWithNonPodStructArray).Name, typeof(StructWithNonPodStructArray).FullnameWithUnderscores(), "Marshalling::ArrayStructField<StructWithString__>", expectedOtherStructs: new string[] { typeof(StructWithString).FullnameWithUnderscores() });
        }

        [Test(Description = "Test that a struct with a pod struct array works")]
        public void StructWithSimpleStructWorks()
        {
            Assert.AreEqual(true, TypeUtils.IsNativePodType(TypeResolver.GetTypeReference<StructWithSimpleStruct>(), ScriptingBackend.Mono));

            var actualTree = GeneratedTreeFor<StructWithSimpleStructUser>();
            var actualFunction = actualTree.SourceFile.FindFunction(Naming.ByRefInjectedMethod("Use"));

            TestStructBinding(actualTree.SourceFile, actualFunction, "Use", typeof(StructWithSimpleStruct).Name, typeof(StructWithSimpleStruct).FullnameWithUnderscores(), Pod: true);
            Assert.AreEqual(2, actualTree.HeaderFile.DefaultNamespace.Structs.Count);
        }

        [Test(Description = "Test that a struct with a non pod struct")]
        public void StructWithNonPodWorks()
        {
            Assert.AreEqual(false, TypeUtils.IsNativePodType(TypeResolver.GetTypeReference<StructWithNonPodStruct>(), ScriptingBackend.Mono));

            var actualTree = GeneratedTreeFor<StructWithNonPodStructUser>();
            var actualFunction = actualTree.SourceFile.FindFunction(Naming.ByRefInjectedMethod("Use"));

            TestStructBinding(actualTree.SourceFile, actualFunction, "Use", typeof(StructWithNonPodStruct).Name, typeof(StructWithNonPodStruct).FullnameWithUnderscores(), expectedOtherStructs: new string[] { typeof(StructWithString).FullnameWithUnderscores() });

            var actualStruct1 = actualTree.HeaderFile.DefaultNamespace.Structs[0];
            TestStruct(actualTree, actualStruct1, typeof(StructWithString).Name, typeof(StructWithString).FullnameWithUnderscores(), "Marshalling::StringStructField", expectedOtherStructs: new string[] { typeof(StructWithNonPodStruct).FullnameWithUnderscores() });

            var actualStruct2 = actualTree.HeaderFile.DefaultNamespace.Structs[1];
            TestStruct(actualTree, actualStruct2, typeof(StructWithNonPodStruct).Name, typeof(StructWithNonPodStruct).FullnameWithUnderscores(), Naming.IntermediateTypeName("StructWithString"), expectedOtherStructs: new string[] { typeof(StructWithString).FullnameWithUnderscores() });
        }

        [Test(Description = "Test that we emit a struct when a function takes a non pod struct as a parameter")]
        public void StructEmittingWorks()
        {
            var actualTree = GeneratedTreeFor<StructWithStringUser>();
            Assert.AreEqual(1, actualTree.HeaderFile.DefaultNamespace.Structs.Count);

            var expectation = new StructDefinitionNode
            {
                Name = Naming.IntermediateTypeName("StructWithString"),
                ForceUniqueViaPreprocessor = true,
                TypeDefs = null,
                Members = null
            };
            AssertContains(expectation, actualTree.HeaderFile.DefaultNamespace.Structs);
        }

        [Test(Description = "Test that we only emit one struct even if we have multiple functions using the struct")]
        public void MultipleUsersWorks()
        {
            var actualTree = GeneratedTreeFor<StructWithObjectUser>();
            Assert.AreEqual(1, actualTree.HeaderFile.DefaultNamespace.Structs.Count);

            var expectation = new StructDefinitionNode
            {
                Name = Naming.IntermediateTypeName("StructWithObject"),
                ForceUniqueViaPreprocessor = true,
                TypeDefs = null,
                Members = null
            };
            AssertContains(expectation, actualTree.HeaderFile.DefaultNamespace.Structs);
        }

        //

        [Test(Description = "Test that name override for struct fields work")]
        public void NameOverrideForStructfieldsWork()
        {
            var actualTree = GeneratedTreeFor<SimpleStructWithFieldNameOverrideUser>();
            var actualStruct = actualTree.HeaderFile.DefaultNamespace.Structs[0];
            TestStruct(actualTree, actualStruct, typeof(SimpleStructWithFieldNameOverride).Name, typeof(SimpleStructWithFieldNameOverride).FullnameWithUnderscores(), "SInt32", "Bar");
        }

        [Test(Description = "Test that fixed fields generate correct intermediate structs")]
        public void FixedFieldStructGeneratesCorrectIntermediateTypes()
        {
            var actualTree = GeneratedTreeFor<StructWithFixedBufferUser>();
            var actualStruct = actualTree.HeaderFile.DefaultNamespace.Structs.First(s => s.Name == Naming.IntermediateTypeName(nameof(StructWithFixedBuffer)));
            var expectedStruct = new StructDefinitionNode
            {
                Name = Naming.IntermediateTypeName(nameof(StructWithFixedBuffer)),
                ForceUniqueViaPreprocessor = true,
                Members = new List<FieldDeclarationStatementNode>
                {
                    new FieldDeclarationStatementNode
                    {
                        Name = "SomeInts",
                        Type = "SInt32",
                        ElementCount = 32
                    }
                }
            };
            AssertEqual(expectedStruct, actualStruct);
            Assert.IsFalse(actualTree.HeaderFile.DefaultNamespace.Structs.Any(s => s.Name.Contains("__FixedBuffer__")));
        }

        [Test(Description = "Test that non-blittable structs with fixed buffers throw")]
        public void NonBlittableFixedFieldStructThrows()
        {
            Assert.Throws<NotImplementedException>(() => GeneratedTreeFor<NonBlittableStructWithFixedBufferUser>());
        }

        [Test(Description = "Test that we generate intermediate even if it shouldn't be needed")]
        public void ForceGenerateWork()
        {
            var actualTree = GeneratedTreeFor<SimpleStructWithForceGenerateUser>();
            var actualFunction = actualTree.SourceFile.FindFunction(Naming.ByRefInjectedMethod("Use"));

            TestStructBinding(actualTree.SourceFile, actualFunction, "Use", typeof(SimpleStructWithForceGenerate).Name, typeof(SimpleStructWithForceGenerate).FullnameWithUnderscores());

            var actualStruct = actualTree.HeaderFile.DefaultNamespace.Structs[0];
            TestStruct(actualTree, actualStruct, typeof(SimpleStructWithForceGenerate).Name, typeof(SimpleStructWithForceGenerate).FullnameWithUnderscores(), "SInt32");
        }

        [Test(Description = "Test that we don't generate intermediate struct when UseCustom is set, but that we do generate MarshalInfo<T>")]
        public void UseCustomNonPodStructWork()
        {
            var actualTree = GeneratedTreeFor<NonPodStructWithUseCustomUser>();
            var actual = actualTree.SourceFile;
            var actualFunction = actual.FindFunction(Naming.ByRefInjectedMethod("Use"));

            TestStructBinding(actual, actualFunction, "Use", typeof(NonPodStructWithUseCustom).Name, typeof(NonPodStructWithUseCustom).FullnameWithUnderscores());
            Assert.AreEqual(0, actual.DefaultNamespace.Structs.Count);
            AssertMarshalInfoExists(
                Naming.IntermediateTypeName(typeof(NonPodStructWithUseCustom).Name),
                MarshalingNamespaceFromTree(actualTree),
                true,
                false);
        }

        [Test(Description = "Test that we don't generate intermediate struct when UseCustom is set, but that we do generate MarshalInfo<T>")]
        public void CustomStructUsingNonBlittableStructUser_GeneratesMarshallingForFieldType()
        {
            var actualTree = GeneratedTreeFor<CustomStructUsingNonBlittableStructUser>();

            AssertMarshalInfoExists(
                Naming.IntermediateTypeName(typeof(StructWithString).Name),
                MarshalingNamespaceFromTree(actualTree),
                true,
                false);
        }

        [Test(Description = "Test that we Header attribute property works")]
        public void HeaderPropertyWork()
        {
            var actual = GeneratedTreeFor<StructWithHeaderUser>().HeaderFile;
            Assert.AreEqual("Foo.h", actual.IncludeFiles[0]);
        }

        [Test(Description = "Test that returning a POD struct works")]
        public void ReturnSimpleWorks()
        {
            var actual = GeneratedTreeFor<SimpleReturner>().SourceFile;
            var actualFunction = actual.FindFunction(Naming.ByRefInjectedMethod("Return"));
            TestReturn(actual, actualFunction, "Return", typeof(Simple).Name, typeof(Simple).FullnameWithUnderscores(), true);
        }

        [Test(Description = "Test that returning a non pod struct works")]
        public void ReturnNonPodWorks()
        {
            var actual = GeneratedTreeFor<StructWithStringReturner>().SourceFile;
            var actualFunction = actual.FindFunction(Naming.ByRefInjectedMethod("Return"));
            TestReturn(actual, actualFunction, "Return", typeof(StructWithString).Name, typeof(StructWithString).FullnameWithUnderscores(), false);
        }

        [Test(Description = "Test that we can have an extern static method on a struct")]
        public void StructWithExternMethodWorks()
        {
            var actual = GeneratedTreeFor<StructWithExternMethod>().SourceFile;
            var actualFunction = actual.FindFunction("Test");

            var expectation = new ExpressionStatementNode
            {
                Expression = new FunctionCallNode
                {
                    Qualifier = new StaticMemberQualifier { ClassName = Naming.RegisteredNameFor("Unity_BindingsGenerator_Tests_MarshallingStructTests_StructWithExternMethod")},
                    FunctionName = "Test"
                }
            };

            Assert.IsTrue(FunctionContainsStatement(expectation, actualFunction));
        }

        [Test(Description = "Test that we don't generate intermediate struct for reference types, but that we do generate MarshalInfo<T>")]
        public void UseIntPtrTypeGeneratesMarshalInfoButNoIntermediateType()
        {
            var actualTree = GeneratedTreeFor<IntPtrTypeUser>();
            var actual = actualTree.SourceFile;

            Assert.AreEqual(0, actual.DefaultNamespace.Structs.Count);
            AssertMarshalInfoExists(
                typeof(IntPtrType).RegisteredName(),
                MarshalingNamespaceFromTree(actualTree),
                false,
                false);
        }

        public struct BlittableStructWithExternMethod
        {
            int a;

            [MethodImpl(MethodImplOptions.InternalCall)]
            public extern void Test();
        }

        [Test(Description = "Test that we can have an extern non-static method on a blittable struct")]
        public void BlittableStructWithExternMethodWorks()
        {
            FileNode fileNode;
            var functionNode = FunctionNodeForMethod(typeof(BlittableStructWithExternMethod), "Test", out fileNode);

            Assert.AreEqual(functionNode.Parameters[0].Type, Naming.Pointer(typeof(BlittableStructWithExternMethod).RegisteredName()));

            var expectation = new ExpressionStatementNode
            {
                Expression =
                    new FunctionCallNode
                {
                    Qualifier = new InstanceMemberQualifier { Expression = Naming.Self },
                    FunctionName = "Test",
                }
            };

            AssertContains(expectation, functionNode.Statements);
            Assert.IsTrue(FileContainsTypeVerifications(fileNode, typeof(BlittableStructWithExternMethod).FullnameWithUnderscores()));
        }

        [NativeType(CodegenOptions.Custom, "FooName")]
        public struct StructWithIntermediateNameOverload
        {
        }

        public class StructWithIntermediateNameOverloadUser
        {
            [MethodImpl(MethodImplOptions.InternalCall)]
            public static extern void Use(StructWithIntermediateNameOverload param);
        }

        [Test(Description = "Test that name overload on intermediate struct works")]
        public void StructWithIntermediateNameOverloadWorks()
        {
            var actualFunction = FileNodeForMethod<StructWithIntermediateNameOverloadUser>("Use").FindFunction("Use_Injected");

            var parameterExpectation = new FunctionParameter
            {
                Name = "param",
                Type = Naming.ConstReference("FooName")
            };

            AssertContains(parameterExpectation, actualFunction.Parameters);
        }

        public struct StructWithIgnoreField
        {
            string a;
            [UnityEngine.Bindings.Ignore]
            int b;
            float c;
        }

        public class StructWithIgnoreFieldUser
        {
            [MethodImpl(MethodImplOptions.InternalCall)]
            public static extern void Use(StructWithIgnoreField param);
        }

        [Test(Description = "Test that [Ignore] on a struct field disables marshalling statements for that field")]
        public void StructFieldWithIgnoreAttributeDoesntHaveMarshallingInstructions()
        {
            var actualTree = TreeGeneratorForMethod(typeof(StructWithIgnoreFieldUser), "Use");

            var actualStructNode = actualTree.HeaderFile.DefaultNamespace.Structs.Single();
            var actualMarshallingNamespace =
                actualTree.SourceFile.Namespaces.First(n => n.Name == Naming.MarshallingNamespace);
            var actualMarshalFunction =
                actualMarshallingNamespace.Functions.First(n => n.Name == Naming.MarshalFunctionName);
            var actualUnmarshalFunction =
                actualMarshallingNamespace.Functions.First(n => n.Name == Naming.UnmarshalFunctionName);

            Assert.AreEqual(3, actualStructNode.Members.Count);

            Assert.AreEqual(2, actualMarshalFunction.Statements.Count);
            Assert.AreEqual(2, actualUnmarshalFunction.Statements.Count);

            Assert.IsFalse(actualMarshalFunction.Statements.Any(s => ((s as ExpressionStatementNode).Expression as FunctionCallNode).Arguments[0].Name.Contains("->b")));
            Assert.IsFalse(actualUnmarshalFunction.Statements.Any(s => ((s as ExpressionStatementNode).Expression as FunctionCallNode).Arguments[0].Name.Contains("->b")));
        }

        public struct StructNotUsedInExtern
        {
            string a;

            [MethodImpl(MethodImplOptions.InternalCall)]
            public static extern void Use(int param);
        }

        [Test(Description = "Struct which has only externs but is not used as an argument should not generate marshalling")]
        public void UnusedStructWithExternDoesNotHaveMarshallingStruct()
        {
            var fileNode = FileNodeForMethod<StructNotUsedInExtern>("Use");
            Assert.IsFalse(fileNode.Namespaces.Any(n => n.Structs.Count > 0));
        }

        public struct RecursiveStruct
        {
            public RecursiveStruct[] field;
        }

        public class RecursiveStructUser
        {
            [MethodImpl(MethodImplOptions.InternalCall)]
            public static extern void Use(RecursiveStruct param);
        }

        [Test(Description = "Test that structs with recursive field definitions work")]
        public void RecursiveStructGenerationWorks()
        {
            var actualTree = GeneratedTreeFor<RecursiveStructUser>();
            var actualStructNode = actualTree.HeaderFile.DefaultNamespace.Structs.Single();

            Assert.AreEqual("Marshalling::ArrayStructField<RecursiveStruct__>", actualStructNode.Members.Single().Type);
        }

        [Test(Description = "Test that struct codegen order dependency is correct")]
        public void StructCodegenOrderDependencyIsCorrect()
        {
            var actualTree = GeneratedTreeFor<StructWithNonPodStructArrayUser>();

            Assert.AreEqual("StructWithString__", actualTree.HeaderFile.DefaultNamespace.Structs[0].Name);
            Assert.AreEqual("StructWithNonPodStructArray__", actualTree.HeaderFile.DefaultNamespace.Structs[1].Name);
        }

        [NativeAsStruct]
        [StructLayout(LayoutKind.Sequential)]
        public class ClassToStruct
        {
            string a;
        }

        public class ClassToStructUser
        {
            [MethodImpl(MethodImplOptions.InternalCall)]
            public static extern void Use(ClassToStruct param);
            [MethodImpl(MethodImplOptions.InternalCall)]
            public static extern ClassToStruct Return();
            [MethodImpl(MethodImplOptions.InternalCall)]
            public static extern void UseArray(ClassToStruct[] param);
        }

        [Test(Description = "Test that classes with NativeAsStruct have MarshalInfo generated properly")]
        public void ClassToStructGeneratesMarshalInfo()
        {
            var actualTree = GeneratedTreeFor<ClassToStructUser>();
            TestStruct(actualTree,
                actualTree.HeaderFile.DefaultNamespace.Structs[0],
                nameof(ClassToStruct),
                typeof(ClassToStruct).FullnameWithUnderscores(),
                "Marshalling::StringStructField");
        }

        [Test(Description = "Test that classes with NativeAsStruct have MarshalInfo generated properly")]
        public void ClassToStructReturnUnmarshalledToManagedObjectForStruct()
        {
            var actualFunction = FunctionNodeForMethod<ClassToStructUser>("Return");
            var returnDeclaration = new VariableDeclarationStatementNode("ret_unmarshalled",
                $"Marshalling::ManagedObjectForStruct<{Naming.IntermediateTypeName(nameof(ClassToStruct))}>");
            Assert.IsTrue(FunctionContainsStatement(returnDeclaration, actualFunction));
        }

        [NativeType(CodegenOptions = CodegenOptions.Custom, IntermediateScriptingStructName = "Namespace::MyNativeStruct")]
        [NativeAsStruct]
        [StructLayout(LayoutKind.Sequential)]
        public class ClassToStructCustom
        {
            string a;
        }

        public struct AsStructField
        {
            private ClassToStructCustom a;
        }

        public class AsStructFieldUser
        {
            [MethodImpl(MethodImplOptions.InternalCall)]
            public static extern void Use(AsStructField param);
        }

        [Test(Description = "Test that structs with NativeAsStruct fields are marshaled properly")]
        public void AsStructFieldMarshaledWithManagedObjectAsStructMarshaller()
        {
            var actualTree = GeneratedTreeFor<AsStructFieldUser>();
            TestStruct(actualTree,
                actualTree.HeaderFile.DefaultNamespace.Structs.First(s => s.Name == Naming.IntermediateTypeName(nameof(AsStructField))),
                nameof(AsStructField),
                typeof(AsStructField).FullnameWithUnderscores(),
                $"Marshalling::ManagedObjectForStruct<Namespace::MyNativeStruct>",
                expectedOtherStructs: new[] {typeof(ClassToStructCustom).FullnameWithUnderscores()},
                checkTypeVerifications: false);
        }

        public struct BlittableStructWithUnion
        {
            [UnityEngine.Bindings.Ignore(DoesNotContributeToSize = true)]
            short unioned;
            byte a;
            byte b;
        }
        public class BlittableStructWithUnionUser
        {
            [MethodImpl(MethodImplOptions.InternalCall)]
            public static extern void Use(BlittableStructWithUnion param);
        }

        [Test(Description = "Test that structs with NativeAsStruct fields are marshaled properly")]
        public void BlittableStructWithUnionDoesNotGenerateForIgnoredField()
        {
            var actualTree = GeneratedTreeFor<BlittableStructWithUnionUser>();
            var structDefinitionNode = actualTree.HeaderFile.DefaultNamespace.Structs.First(s =>
                s.Name == Naming.IntermediateTypeName(nameof(BlittableStructWithUnion)));

            AssertNotContains(new FieldDeclarationStatementNode(name: "unioned"), structDefinitionNode.Members);
        }

        public struct NonBlittableStructWithUnion
        {
            [UnityEngine.Bindings.Ignore(DoesNotContributeToSize = true)]
            short unioned;

            byte a;

            byte b;
            string c;
        }
        public class NonBlittableStructWithUnionUser
        {
            [MethodImpl(MethodImplOptions.InternalCall)]
            public static extern void Use(NonBlittableStructWithUnion param);
        }

        [Test(Description = "Test that structs with NativeAsStruct fields are marshaled properly")]
        public void NonBlittableStructWithUnionDoesNotGenerateForIgnoredField()
        {
            var actualTree = GeneratedTreeFor<NonBlittableStructWithUnionUser>();
            var structDefinitionNode = actualTree.HeaderFile.DefaultNamespace.Structs.First(s =>
                s.Name == Naming.IntermediateTypeName(nameof(NonBlittableStructWithUnion)));

            AssertNotContains(new FieldDeclarationStatementNode(name: "unioned"), structDefinitionNode.Members);
        }

        public struct NonBlittableStructWithInstanceMethod
        {
            public string a;
            [MethodImpl(MethodImplOptions.InternalCall)]
            public extern void Test();
        }

        [Test]
        public void NonBlittableStructWithInstanceMethodGeneratesCorrectCode()
        {
            var functionDefinition = FunctionNodeForMethod<NonBlittableStructWithInstanceMethod>("Test");

            Assert.That(
                FindSequenceRecursive(new List<IStatementNode>
                {
                    new ExpressionStatementNode
                    {
                        Expression = new FunctionCallNode(
                            "Test",
                            new InstanceMemberQualifier("_unity_self_marshalled"))
                    }
                }, functionDefinition.Statements),
                Is.True);

            Assert.That(
                FindSequenceRecursive(new List<IStatementNode>
                {
                    new VariableDeclarationStatementNode("_unity_self_marshalled",
                        string.Format("Marshalling::OutMarshaller<{0}, {1}>", Naming.IntermediateTypeName(nameof(NonBlittableStructWithInstanceMethod)), typeof(NonBlittableStructWithInstanceMethod).RegisteredName()))
                }, functionDefinition.Statements),
                Is.True);
        }
    }
}
