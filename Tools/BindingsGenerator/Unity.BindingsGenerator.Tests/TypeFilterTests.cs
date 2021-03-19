using System;
using System.IO;
using System.Linq;
using Mono.Cecil;
using NUnit.Framework;
using Unity.BindingsGenerator.TestFramework;
using Unity.CommonTools;
using TypeFilter = Unity.BindingsGenerator.Core.TypeFilter;

namespace Unity.BindingsGenerator.Tests
{
    [TestFixture]
    class TypeFilterTests
    {
        private const string AssemblyName = @"Unity.BindingsGenerator.TestAssembly.dll";
        private static readonly string BindingsFileName = Paths.Combine(Workspace.BasePath, "Tools", "BindingsGenerator", "Unity.BindingsGenerator.TestAssembly", "ClassWithExterns.bindings.cs");

        private AssemblyDefinition _assembly;

        private TypeDefinition _classWithExterns;
        private TypeDefinition _nestedClassWithExterns;
        private TypeDefinition _classWithoutExterns;

        private MethodDefinition _normalMethodInCs;
        private MethodDefinition _normalMethodInBindings;
        private MethodDefinition _externMethodInNestedBindingsClass;

        private PropertyDefinition _normalPropertyInCs;
        private PropertyDefinition _normalPropertyInBindings;

        [SetUp]
        public void Setup()
        {
            _assembly = TypeResolver.LoadAssembly(AssemblyName);

            _classWithExterns = _assembly.MainModule.Types.Single(t => t.Name == "ClassWithExterns");
            _classWithoutExterns = _assembly.MainModule.Types.Single(t => t.Name == "ClassWithoutExterns");
            _nestedClassWithExterns = _assembly.MainModule.Types.Single(t => t.Name == "ParentOfNested").NestedTypes.First();

            _normalMethodInCs = _classWithoutExterns.Methods.Single(m => m.Name == "NormalFunction");
            _normalMethodInBindings = _classWithExterns.Methods.Single(m => m.Name == "NormalFunction");
            _externMethodInNestedBindingsClass = _nestedClassWithExterns.Methods.Single(m => m.Name == "ExternMethod");

            _normalPropertyInCs = _classWithoutExterns.Properties.Single(m => m.Name == "NormalProperty");
            _normalPropertyInBindings = _classWithExterns.Properties.Single(m => m.Name == "NormalProperty");
        }

        [Test]
        public void TestFilterAcceptsMethodsDeclaredInBindingsFiles()
        {
            var filter = new TypeFilter(BindingsFileName, new string[0]);
            Assert.IsTrue(filter.IsDeclaredInTheRightInputFile(_normalMethodInBindings));
        }

        [Test]
        public void TestFilterAcceptsPropertiesDeclaredInBindingsFiles()
        {
            var filter = new TypeFilter(BindingsFileName, new string[0]);
            Assert.IsTrue(filter.IsDeclaredInTheRightInputFile(_normalPropertyInBindings));
        }

        [Test]
        public void TestFilterRejectsMethodsDeclaredInCsFiles()
        {
            var filter = new TypeFilter(BindingsFileName, new string[0]);
            Assert.IsFalse(filter.IsDeclaredInTheRightInputFile(_normalMethodInCs));
        }

        [Test]
        public void TestFilterRejectsPropertiesDeclaredInCsFiles()
        {
            var filter = new TypeFilter(BindingsFileName, new string[0]);
            Assert.IsFalse(filter.IsDeclaredInTheRightInputFile(_normalPropertyInCs));
        }

        [Test]
        public void TestFilterAcceptsNestedClasses()
        {
            var filter = new TypeFilter(BindingsFileName, new string[0]);
            Assert.IsTrue(filter.IsDeclaredInTheRightInputFile(_externMethodInNestedBindingsClass));
        }
    }
}
