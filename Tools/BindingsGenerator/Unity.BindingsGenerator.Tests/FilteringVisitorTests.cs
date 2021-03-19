using System.Collections.Generic;
using System.Linq;
using System.Runtime.CompilerServices;
using Mono.Cecil;
using NUnit.Framework;
using Unity.BindingsGenerator.Core;
using Unity.BindingsGenerator.TestFramework;

namespace Unity.BindingsGenerator.Tests
{
    public static class StaticClass
    {
        [MethodImpl(MethodImplOptions.InternalCall)]
        extern static void StaticMethod();
    }

    public abstract class TestClass
    {
        public delegate int TestDelegate();

        public abstract void TestAbstractMethod();
    }

    public interface ITestInterface
    {
        void InterfaceMethod();
    }

    [TestFixture]
    public class FilteringVisitorTests
    {
        [OneTimeSetUp]
        public void Setup()
        {
            _collector = TypeCollectorVisitor.Collect(typeof(TestClass).Assembly.Location);
        }

        [Test]
        public void EnsureDelegatesAreNotProcessed()
        {
            Assert.IsFalse(_collector.Types.Any(t => t.IsDelegate()));
        }

        [Test]
        public void EnsureAbstractMethodsAreNotProcessed()
        {
            Assert.IsFalse(_collector.Methods.Any(m => m.IsAbstract));
        }

        [Test]
        public void EnsureInterfacesAreNotProcessed()
        {
            Assert.IsFalse(_collector.Types.Any(m => m.IsInterface));
        }

        [Test]
        public void EnsureStaticClassesAreNotIgnored()
        {
            Assert.IsTrue(_collector.Types.Any(t => t.Name == "StaticClass"));
            Assert.IsTrue(_collector.Methods.Any(m => m.Name == "StaticMethod"));
        }

        private TypeCollectorVisitor _collector;
    }

    class TypeCollectorVisitor : Visitor
    {
        public static TypeCollectorVisitor Collect(string assemblyPath)
        {
            var assembly = TypeResolver.LoadAssembly(assemblyPath);

            var collector = new TypeCollectorVisitor();
            var visitor = new FilteringVisitor(assembly);

            visitor.Accept(collector);

            return collector;
        }

        public override void Visit(TypeDefinition typeDefinition)
        {
            base.Visit(typeDefinition);
            Types.Add(typeDefinition);
        }

        public override void Visit(MethodDefinition methodDefinition)
        {
            base.Visit(methodDefinition);
            Methods.Add(methodDefinition);
        }

        public IList<MethodDefinition> Methods = new List<MethodDefinition>();
        public IList<TypeDefinition> Types = new List<TypeDefinition>();
    }
}
