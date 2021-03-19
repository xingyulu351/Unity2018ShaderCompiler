using System;
using System.Collections.Generic;
using System.Linq;
using System.Reflection;
using Mono.Cecil;
using NUnit.Framework;
using Unity.BindingsGenerator.Core;
using Unity.BindingsGenerator.Core.Nodes;

namespace Unity.BindingsGenerator.TestFramework
{
    public class BindingsGeneratorTestsBase
    {
        protected TreeGenerator GeneratedTreeFor<T>()
        {
            return GeneratedTreeFor(typeof(T));
        }

        protected TreeGenerator GeneratedTreeFor(Type type)
        {
            var typeDefinition = TypeResolver.TypeDefinitionFor(type);

            var generator = new TreeGenerator().Generate(new FilteringVisitor(typeDefinition));

            return generator;
        }

        protected TreeGenerator EnsureGenerated<T>(TreeGenerator.GenerateReason reason)
        {
            return EnsureGenerated(typeof(T), reason);
        }

        protected TreeGenerator EnsureGenerated(Type type, TreeGenerator.GenerateReason reason)
        {
            var typeDefinition = TypeResolver.TypeDefinitionFor(type);

            var generator = new TreeGenerator();
            generator.Initialize();
            generator.EnsureGenerated(typeDefinition, reason);

            return generator;
        }

        protected FileNode SourceFileNodeFor<T>()
        {
            return SourceFileNodeFor(typeof(T));
        }

        protected FileNode SourceFileNodeFor(Type type)
        {
            return GeneratedTreeFor(type).SourceFile;
        }

        protected FunctionNode FunctionNodeForMethod<T>(string managedMethodName)
        {
            return FunctionNodeForMethod(typeof(T), managedMethodName);
        }

        protected FunctionNode FunctionNodeForMethod<T>(string managedMethodName, out FileNode fileNode)
        {
            return FunctionNodeForMethod(typeof(T), managedMethodName, out fileNode);
        }

        protected FunctionNode FunctionNodeForMethod(Type type, string managedMethodName)
        {
            FileNode fileNode;
            return FunctionNodeForMethod(type, managedMethodName, out fileNode);
        }

        protected FunctionNode FunctionNodeForMethod(Type type, string managedMethodName, out FileNode fileNode)
        {
            fileNode = FileNodeForMethod(type, managedMethodName);
            return fileNode.DefaultNamespace.Functions.Single(f => true);
        }

        protected FileNode FileNodeForMethod<T>(string managedMethodName)
        {
            return FileNodeForMethod(typeof(T), managedMethodName);
        }

        protected FileNode FileNodeForMethod(Type type, string managedMethodName)
        {
            var generator = TreeGeneratorForMethod(type, managedMethodName);
            return generator.SourceFile;
        }

        public TreeGenerator TreeGeneratorForMethod(Type type, string managedMethodName)
        {
            var method = MethodDefinitionFor(type, managedMethodName);
            return new TreeGenerator().Generate(new FilteringVisitor(method));
        }

        protected MethodDefinition MethodDefinitionFor(Type type, string managedMethodName)
        {
            var typeDefinition = TypeResolver.TypeDefinitionFor(type);
            return typeDefinition.Methods.Single(m => m.Name == managedMethodName);
        }

        protected string CppCodeFor<T>()
        {
            return CppCodeFor(typeof(T));
        }

        protected string CppCodeFor(Type t)
        {
            var result = GeneratedTreeFor(t);

            var codeGenerator = new CppGenerator();
            var output = codeGenerator.Generate(result.SourceFile);

            return output;
        }

        protected string RegistrationCodeFor<T>()
        {
            return RegistrationCodeFor(typeof(T));
        }

        protected string RegistrationCodeFor(Type t)
        {
            var result = GeneratedTreeFor(t);

            return new RegistrationGenerator().Generate(result.SourceFile, "TestFileName.bindings.cs");
        }

        #region AssertEqual

        private bool IsEqual(string expected, string actual)
        {
            if (expected == null)
                return true;

            return expected.Equals(actual);
        }

        private void AssertEqual(string expected, string actual)
        {
            if (expected == null)
                return;

            Assert.AreEqual(expected, actual);
        }

        private bool IsEqualList(object expected, object actual)
        {
            var type = expected.GetType();
            var countPropInfo = type.GetProperty("Count");

            var count = (int)countPropInfo.GetValue(expected, null);
            if (count != (int)countPropInfo.GetValue(actual, null))
                return false;

            var indexerName = ((DefaultMemberAttribute)type.GetCustomAttributes(typeof(DefaultMemberAttribute), true)[0]).MemberName;
            var indexerPropInfo = type.GetProperty(indexerName);
            for (int i = 0; i < count; ++i)
            {
                var index = new Object[] {i};
                if (!IsEqual(indexerPropInfo.GetValue(expected, index), indexerPropInfo.GetValue(actual, index)))
                    return false;
            }

            return true;
        }

        private void AssertEqualList(object expected, object actual)
        {
            var type = expected.GetType();
            var countPropInfo = type.GetProperty("Count");

            int count = (int)countPropInfo.GetValue(expected, null);
            Assert.AreEqual(count, countPropInfo.GetValue(actual, null));

            String indexerName =
                ((DefaultMemberAttribute)type.GetCustomAttributes(typeof(DefaultMemberAttribute), true)[0]).MemberName;
            var indexerPropInfo = type.GetProperty(indexerName);
            for (int i = 0; i < count; ++i)
            {
                var index = new Object[] {i};
                AssertEqual(indexerPropInfo.GetValue(expected, index), indexerPropInfo.GetValue(actual, index));
            }
        }

        protected bool IsEqual(object expected, object actual)
        {
            if (expected == null)
                return true;

            if (actual == null)
                return false;

            var type = expected.GetType();
            if (type != actual.GetType())
                return false;

            if (type == typeof(bool) || type.IsEnum)
                return expected.Equals(actual);
            else if (type == typeof(string))
                return IsEqual((string)expected, (string)actual);
            else if (type.IsGenericType && type.GetGenericTypeDefinition() == typeof(List<>))
                return IsEqualList(expected, actual);
            else if (type.IsClass)
            {
                var propInfos = type.GetProperties();
                foreach (var propInfo in propInfos)
                {
                    if (!IsEqual(propInfo.GetValue(expected, null), propInfo.GetValue(actual, null)))
                        return false;
                }

                if (type.GetFields().Any(f => !f.IsStatic))
                    throw new NotImplementedException("Don't know how to compare fields yet. Please test if you add.");

                return true;
            }
            else
                throw new NotImplementedException("Unexpected type. Don't know how to compare them");
        }

        protected void AssertEqual(object expected, object actual)
        {
            if (expected == null)
                return;

            Assert.That(actual, Is.Not.Null);

            var type = expected.GetType();
            Assert.AreEqual(type, actual.GetType());

            if (type == typeof(Boolean) || type.IsEnum || type == typeof(Int32))
                Assert.AreEqual(expected, actual);
            else if (type == typeof(string))
                AssertEqual((string)expected, (string)actual);
            else if (type.IsGenericType && type.GetGenericTypeDefinition() == typeof(List<>))
                AssertEqualList(expected, actual);
            else if (type.IsClass)
            {
                var propInfos = type.GetProperties();
                foreach (var propInfo in propInfos)
                {
                    AssertEqual(propInfo.GetValue(expected, null), propInfo.GetValue(actual, null));
                }

                if (type.GetFields().Length > 0)
                    throw new NotImplementedException("Don't know how to compare fields yet. Please test if you add.");
            }
            else
                throw new NotImplementedException("Unexpected type '" + type.Name + "'. Don't know how to compare them");
        }

        protected void AssertContains<T>(object expectation, List<T> list, string message = null)
        {
            foreach (var element in list)
            {
                if (IsEqual(expectation, element))
                    return;
            }

            Assert.Fail(message ?? $"List {list} does not contain {expectation}");
        }

        protected void AssertNotContains<T>(object expectation, List<T> list)
        {
            foreach (var element in list)
            {
                if (IsEqual(expectation, element))
                    Assert.Fail(string.Format("List {0} unexpectedly contains {1}", list.ToString(), expectation.ToString()));
                ;
            }
        }

        protected bool FindSequenceRecursive<T>(List<T> expectation, List<IStatementNode> statements) where T : IStatementNode
        {
            for (int i = 0; i < statements.Count - expectation.Count + 1; i++)
            {
                bool allEqual = true;
                for (int j = 0; j < expectation.Count; j++)
                {
                    if (!IsEqual(expectation[j], statements[i + j]))
                    {
                        allEqual = false;
                        break;
                    }
                }
                if (allEqual)
                    return true;
            }
            //recurse into blocks
            foreach (var blockNode in statements.OfType<BlockNode>())
            {
                if (FindSequenceRecursive(expectation, blockNode.Statements))
                    return true;
            }
            return false;
        }

        protected bool FunctionContainsStatement(IStatementNode expectation, FunctionNode function)
        {
            return FindRecursive(expectation, function.Statements) != null;
        }

        protected T FindRecursive<T>(T expectation, FunctionNode function) where T : IStatementNode
        {
            return FindRecursive(expectation, function.Statements);
        }

        private T FindRecursive<T>(T expectation, List<IStatementNode> statements) where T : IStatementNode
        {
            foreach (var statement in statements)
            {
                if (IsEqual(expectation, statement))
                    return (T)statement;

                var block = statement as BlockNode;
                if (block == null)
                    continue;

                var foundInBlock = FindRecursive(expectation, block.Statements);
                if (foundInBlock != null)
                    return foundInBlock;
            }
            return default(T);
        }

        protected bool FileContainsTypeVerifications(FileNode actual, params string[] expectedTypes)
        {
            var namespaceNode = actual.Namespaces.FirstOrDefault(n => n.Name == Naming.RegistrationChecksNamespace);
            if (namespaceNode == null)
                return false;

            var typeDefs = new List<TypeDefNode>(namespaceNode.TypeDefs);
            foreach (var expectedType in expectedTypes)
            {
                var foundTypeDef = typeDefs.FirstOrDefault(td => IsEqual(new TypeDefNode
                {
                    SourceType = Naming.RegisteredNameFor(expectedType),
                    DestType = Naming.VerifyBindingNameFor(expectedType)
                }, td));

                if (foundTypeDef == null)
                    return false;

                typeDefs.Remove(foundTypeDef);
            }
            return typeDefs.Count == 0;
        }

        #endregion

        protected static NamespaceNode MarshalingNamespaceFromTree(TreeGenerator actualTree)
        {
            return actualTree.SourceFile.Namespaces.Find(n => n.Name == Naming.MarshallingNamespace);
        }

        protected void AssertMarshalInfoExists(string marshalInfoTypeName, NamespaceNode actualMarshallingNamespace, bool requiresMarshaling, bool expectSizeCheck)
        {
            StructDefinitionNode expectedMarshalInfoStruct = new StructDefinitionNode
            {
                ForceUniqueViaPreprocessor = true,
                Name = Naming.MarshalInfo,
                IsTemplate = true,
                TemplateSpecialization = {marshalInfoTypeName},
                Members = new List<FieldDeclarationStatementNode>
                {
                    new FieldDeclarationStatementNode
                    {
                        Name = Naming.RequiresMarshalingField,
                        Type = "bool",
                        IsStatic = true,
                        IsConst = true,
                        Initializer = new StringExpressionNode {Str = requiresMarshaling ? "true" : "false"}
                    }
                },
                FunctionDefinitions =
                {
                    new FunctionNode
                    {
                        Name = Naming.ScriptingClassFunction,
                        IsStatic = true,
                        IsInline = true,
                        ReturnType = "ScriptingClassPtr",
                        Statements = null
                    }
                }
            };
            var actualMarshalInfoStruct = actualMarshallingNamespace.Structs.FirstOrDefault(s => IsEqual(expectedMarshalInfoStruct, s));
            Assert.NotNull(actualMarshalInfoStruct, $"Could not find MarshalInfo definition for \"{marshalInfoTypeName}\"");

            Assert.AreEqual(expectSizeCheck,
                actualMarshalInfoStruct.FunctionDefinitions[0].Statements.Any(s => s is StringStatementNode && ((StringStatementNode)s).Str.StartsWith("ASSERT_SAME_SIZE")),
                "ASSERT_SAME_SIZE check did not meet expectation");
        }

        protected void AssertMarshalInfoNotExists(string marshalInfoTypeName, TreeGenerator tree)
        {
            AssertMarshalInfoNotExists(marshalInfoTypeName, MarshalingNamespaceFromTree(tree));
        }

        protected void AssertMarshalInfoNotExists(string marshalInfoTypeName, NamespaceNode actualMarshallingNamespace)
        {
            StructDefinitionNode expectedMarshalInfoStruct = new StructDefinitionNode
            {
                ForceUniqueViaPreprocessor = true,
                Name = Naming.MarshalInfo,
                IsTemplate = true,
                TemplateSpecialization = {marshalInfoTypeName},
                Members = null,
                FunctionDefinitions = null
            };
            AssertNotContains(expectedMarshalInfoStruct, actualMarshallingNamespace.Structs);
        }
    }
}
