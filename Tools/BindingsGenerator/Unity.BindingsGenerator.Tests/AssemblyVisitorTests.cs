using System.Collections.Generic;
using System.Linq;
using Mono.Cecil;
using NUnit.Framework;
using Unity.BindingsGenerator.Core;
using Unity.BindingsGenerator.TestFramework;

namespace Unity.BindingsGenerator.Tests
{
    [TestFixture]
    class AssemblyVisitorTests
    {
        private const string _assemblyPath = @"Unity.BindingsGenerator.AssemblyVisitorTestAssembly.dll";

        private AssemblyDefinition _assembly;
        private TreeGenerator _generator;
        private readonly HashSet<string> _fullMethodNamesInAssembly = new HashSet<string>();
        private readonly HashSet<string> _fullTypeNamesInAssembly = new HashSet<string>();

        [SetUp]
        public void Setup()
        {
            _assembly = TypeResolver.LoadAssembly(_assemblyPath);
            _generator = new TreeGenerator().Generate(new FilteringVisitor(_assembly));

            BuildFullMethodAndTypeNameSet();
        }

        class FullMethodNameVisitor : Visitor
        {
            private HashSet<string> _fullMethodNamesInAssembly;
            private HashSet<string> _fullTypeNamesInAssembly;

            public FullMethodNameVisitor(HashSet<string> fullMethodNamesInAssembly, HashSet<string> fullTypeNamesInAssembly)
            {
                _fullMethodNamesInAssembly = fullMethodNamesInAssembly;
                _fullTypeNamesInAssembly = fullTypeNamesInAssembly;
            }

            public override void Visit(TypeDefinition typeDefinition)
            {
                string fullName = typeDefinition.FullName;
                string fullNameWithoutNamespace = fullName.Replace(typeDefinition.Namespace + ".", "");
                _fullTypeNamesInAssembly.Add(fullNameWithoutNamespace);
            }

            public override void Visit(MethodDefinition methodDefinition)
            {
                string fullName = methodDefinition.DeclaringType.FullName + "::" + methodDefinition.Name;
                string fullNameWithoutNamespace = fullName.Replace(methodDefinition.DeclaringType.Namespace + ".", "");
                _fullMethodNamesInAssembly.Add(fullNameWithoutNamespace);
            }
        }

        private void BuildFullMethodAndTypeNameSet()
        {
            new NonFilteringVisitor(_assembly).Accept(new FullMethodNameVisitor(_fullMethodNamesInAssembly, _fullTypeNamesInAssembly));
        }

        [Test]
        public void CanReadAssembly()
        {
            Assert.NotNull(_assembly);
        }

        [Test]
        public void CanGenerate()
        {
            Assert.NotNull(_generator);
        }

        private bool HasInclude(string include)
        {
            return _generator.HeaderFile.IncludeFiles.Contains(include);
        }

        private bool HasMethod(string klassName, string methodName)
        {
            return _generator.SourceFile.FindMemberFunction(klassName, "CUSTOM_" + methodName) != null;
        }

        private bool HasGetterMethod(string klassName, string methodName)
        {
            return _generator.SourceFile.FindMemberFunction(klassName, "Get_Custom_Prop" + methodName) != null;
        }

        private bool HasSetterMethod(string klassName, string methodName)
        {
            return _generator.SourceFile.FindMemberFunction(klassName, "Set_Custom_Prop" + methodName) != null;
        }

        private bool HasStruct(string name)
        {
            return _generator.HeaderFile.FindStruct(name) != null;
        }

        private bool AssemblyHasMethod(string name)
        {
            return _fullMethodNamesInAssembly.Contains(name);
        }

        private bool AssemblyHasType(string name)
        {
            return _fullTypeNamesInAssembly.Contains(name);
        }

        [Test]
        public void ClassWithNoNativeTypeAttributeIsGenerated()
        {
            Assert.IsTrue(HasInclude("ClassWithNoNativeTypeAttribute"));
            Assert.IsTrue(HasMethod("ClassWithNoNativeTypeAttribute", "MemberMethod"));
            Assert.IsTrue(HasMethod("ClassWithNoNativeTypeAttribute", "StaticMethodWithClassArgument"));
        }

        [Test]
        public void EmptyNativeTypeNotGenerated()
        {
            Assert.IsFalse(HasInclude("EmptyNativeTypeClass"));
            Assert.IsTrue(AssemblyHasType("EmptyNativeTypeClass"));
        }

        [Test]
        public void NativeTypeWithExternMemberMethodGenerated()
        {
            Assert.IsTrue(HasInclude("NativeTypeClassWithMemberMethod"));
            Assert.IsTrue(HasMethod("NativeTypeClassWithMemberMethod", "ExternMemberMethod"));
        }

        [Test]
        public void NativeTypeWithExternStaticMethodGenerated()
        {
            Assert.IsTrue(HasInclude("NativeTypeClassWithStaticMethod"));
            Assert.IsTrue(HasMethod("NativeTypeClassWithStaticMethod", "ExternStaticMethod"));
        }

        [Test]
        public void EmptyNativeStructNotGenerated()
        {
            Assert.IsFalse(HasInclude("EmptyNativeTypeStruct"));
            Assert.IsTrue(AssemblyHasType("EmptyNativeTypeStruct"));
        }

        [Test]
        public void NativeStructWithExternStaticMethodGenerated()
        {
            Assert.IsTrue(HasInclude("NativeTypeStructWithStaticMethod"));
            Assert.IsTrue(HasMethod("NativeTypeStructWithStaticMethod", "ExternStaticMethod"));
        }

        [Test]
        public void NativeBlittableStructWithExternInstanceMethodGenerated()
        {
            Assert.IsTrue(HasInclude("NativeTypeBlittableStructWithInstanceMethod"));
            Assert.IsTrue(HasMethod("NativeTypeBlittableStructWithInstanceMethod", "ExternInstanceMethod"));
        }

        [Test]
        public void NativeTypeUsedAsArgumentGenerated()
        {
            Assert.IsTrue(HasInclude("NativeTypeUsedAsArgumentType"));
            Assert.IsTrue(HasMethod("NativeTypeUser", "ArgumentUser"));
        }

        [Test]
        public void NativeTypeUsedAsReturnValueGenerated()
        {
            Assert.IsTrue(HasInclude("NativeTypeUsedAsReturnType"));
            Assert.IsTrue(HasMethod("NativeTypeUser", "ReturnValueUser"));
        }

        [Test]
        public void NativeTypeUsedAsGetterValueGenerated()
        {
            Assert.IsTrue(HasInclude("NativeTypeUsedAsGetterType"));
            Assert.IsTrue(HasGetterMethod("NativeTypeUser", "GetterOnlyPropUser"));
        }

        [Test]
        public void NativeTypeUsedAsSetterValueGenerated()
        {
            Assert.IsTrue(HasInclude("NativeTypeUsedAsSetterType"));
            Assert.IsTrue(HasSetterMethod("NativeTypeUser", "SetterOnlyPropUser"));
        }

        [Test]
        public void NativeTypeUsedAsPropertyValueGenerated()
        {
            Assert.IsTrue(HasInclude("NativeTypeUsedAsPropertyType"));
            Assert.IsTrue(HasGetterMethod("NativeTypeUser", "PropertyUser"));
            Assert.IsTrue(HasSetterMethod("NativeTypeUser", "PropertyUser"));
        }

        [Test]
        public void NativeEnumUsedAsArgumentGenerated()
        {
            Assert.IsTrue(HasInclude("NativeEnumUsedAsArgument"));
            Assert.IsTrue(HasStruct(Naming.IntermediateTypeName("NativeEnumUsedAsArgument")));
            Assert.IsTrue(HasMethod("OtherTypeUser", "EnumUser"));
        }

        [Test]
        public void NativeNonGeneratingEnumUsedAsArgumentNotGenerated()
        {
            Assert.IsTrue(AssemblyHasType("NativeNonGeneratingEnumUsedAsArgument"));
            Assert.IsTrue(HasInclude("NativeNonGeneratingEnumUsedAsArgument")); // This is just policy. No good reason for it.
            Assert.IsTrue(HasStruct(Naming.IntermediateTypeName("NativeNonGeneratingEnumUsedAsArgument")));
            Assert.IsTrue(HasMethod("OtherTypeUser", "EnumUser"));
        }

        [Test]
        public void PodNativeStructUsedAsArgumentNotGenerated()
        {
            Assert.IsTrue(AssemblyHasType("PodNativeStructUsedAsArgument"));
            Assert.IsTrue(HasMethod("OtherTypeUser", Naming.ByRefInjectedMethod("PodStructUser")));

            Assert.IsTrue(HasInclude("PodNativeStructUsedAsArgument"));
            Assert.IsTrue(HasStruct(Naming.IntermediateTypeName("PodNativeStructUsedAsArgument")));
        }

        [Test]
        public void NonPodNativeStructUsedAsArgumentGenerated()
        {
            Assert.IsTrue(HasInclude("NonPodNativeStructUsedAsArgument"));
            Assert.IsTrue(HasMethod("OtherTypeUser", Naming.ByRefInjectedMethod("NonPodStructUser")));
            Assert.IsTrue(HasStruct(Naming.IntermediateTypeName("NonPodNativeStructUsedAsArgument")));
        }

        [Test]
        public void UnusedNativeTypeDoesNotGenerate()
        {
            Assert.IsTrue(AssemblyHasType("UnusedNativeType"));
            Assert.IsFalse(HasInclude("UnusedNativeType"));
        }

        [Test]
        public void UnusedNativeEnumDoesNotGenerate()
        {
            Assert.IsTrue(AssemblyHasType("UnusedNativeEnum"));
            Assert.IsFalse(HasInclude("UnusedNativeEnum"));
        }

        [Test]
        public void UnusedNativeStructDoesNotGenerate()
        {
            Assert.IsTrue(AssemblyHasType("UnusedNativeStruct"));
            Assert.IsFalse(HasInclude("UnusedNativeStruct"));
            Assert.IsFalse(HasStruct(Naming.IntermediateTypeName("UnusedNativeEnum")));
        }

        [Test]
        public void ClassUsedOnlyAsReturnTypeEmitsInclude()
        {
            Assert.IsTrue(HasInclude("ClassUsedOnlyAsReturn"));
        }

        [Test]
        public void MethodInNestedClassGeneratesMethod()
        {
            Assert.IsTrue(HasMethod("NestedWithStaticMethod", "ExternStaticMethod"));
        }

        [Test]
        public void CheckNothingUnexpectedInGeneratedOutput()
        {
            // This integration test verifies that nothing unexpected shows up in generated output
            Assert.AreEqual(17, _generator.HeaderFile.IncludeFiles.Count, string.Join("\n", _generator.HeaderFile.IncludeFiles));
            Assert.AreEqual(4, _generator.HeaderFile.DefaultNamespace.Structs.Count);
            Assert.AreEqual(17, _generator.SourceFile.DefaultNamespace.Functions.Count, _generator.SourceFile.DefaultNamespace.Functions.Aggregate("", (acc, curr) => acc + "\n" + curr.Name));
        }
    }
}
