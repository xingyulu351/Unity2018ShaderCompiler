using System.Linq;
using System.Reflection;
using Mono.Cecil;
using NUnit.Framework;
using Unity.BindingsGenerator.Core;
using Unity.BindingsGenerator.TestFramework;

namespace Unity.BindingsGenerator.Tests
{
    [TestFixture]
    class ExternTests
    {
        private const string _assemblyName = @"Unity.BindingsGenerator.TestAssembly.dll";
        private AssemblyDefinition _assembly;
        private TypeDefinition _klass;

        [SetUp]
        public void Setup()
        {
            _assembly = TypeResolver.LoadAssembly(_assemblyName);
            _klass = _assembly.MainModule.Types.FirstOrDefault(t => t.Name == "ClassWithExterns");
        }

        [Test]
        public void CanOpenTestAssemblyWithCecil()
        {
            Assert.IsTrue(_assembly != null);
            Assert.IsTrue(_klass != null);
        }

        [Test]
        public void CanOpenTestAssemblyViaSystem()
        {
            var ass = Assembly.LoadFrom(TypeResolver.GetAssemblyPathNextToExecutingAssembly(_assemblyName));
            Assert.IsNotNull(ass);
        }

        [Test]
        public void TestIsInternalCallWorks()
        {
            Assert.IsFalse(TypeUtils.IsMethodInternalCall(_klass.Methods.First(md => md.Name == "NormalFunction")));
            Assert.IsTrue(TypeUtils.IsMethodInternalCall(_klass.Methods.First(md => md.Name == "ExternNoAttributes")));
            Assert.IsFalse(TypeUtils.IsMethodInternalCall(_klass.Methods.First(md => md.Name == "ExternWithDllImport")));
            Assert.IsTrue(TypeUtils.IsMethodInternalCall(_klass.Methods.First(md => md.Name == "ExternWithInternalCall")));
        }
    }
}
