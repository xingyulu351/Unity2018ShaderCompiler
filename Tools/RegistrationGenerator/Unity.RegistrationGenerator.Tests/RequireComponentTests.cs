using Mono.Cecil;
using NUnit.Framework;
using System;
using System.Linq;

namespace Unity.RegistrationGenerator.Tests
{
    [TestFixture]
    public class RequireComponentTests
    {
        AssemblyDefinition m_Assembly;

        [SetUp]
        public void Init()
        {
            m_Assembly = TestHelpers.LoadExecutingAssembly();
        }

        [Test]
        public void Reflection_GettingAssemblyName_ReturnsTestAssemblyName()
        {
            Assert.AreEqual("Unity.RegistrationGenerator.Tests", m_Assembly.Name.Name);
        }

        [Test]
        public void Reflection_GettingCustomeAttributes_ReturnsRequiredComponentAttribute()
        {
            Assert.Contains("RequireComponent", m_Assembly.MainModule.Types.Select(a => a.Name).ToArray());
            Assert.Contains("RequireComponent",
                m_Assembly
                    .MainModule
                    .Types
                    .SelectMany(a => a.CustomAttributes)
                    .Select(a => a.AttributeType.Name).ToArray());
        }

        [Test]
        public void ReflectionForComponent2_ExtractingComponentRequirement_ReturnsDirectAttributes()
        {
            var r = RequireComponentProcessor.GetComponentRequirements(m_Assembly).Where(a => a.Item1.Name == "Component2");
            Assert.AreEqual(1, r.Count());
            var attrs = r.First().Item2;
            Assert.AreEqual(1, attrs.Count());
            Assert.AreEqual(GetTestType("Component1"), attrs.First().m_Type0);
            Assert.IsNull(attrs.First().m_Type1);
            Assert.IsNull(attrs.First().m_Type2);
        }

        [Test]
        public void ReflectionForComponent2multi_ExtractingComponentRequirement_ReturnsDirectAttributes()
        {
            var r = RequireComponentProcessor.GetComponentRequirements(m_Assembly).Where(a => a.Item1.Name == "Component2multi");
            Assert.AreEqual(1, r.Count());
            var attrsUnsorted = r.First().Item2;
            Assert.AreEqual(2, attrsUnsorted.Count());

            var attrs = attrsUnsorted.OrderBy(a => a.m_Type0.ToString());

            Assert.AreEqual(GetTestType("Component0"), attrs.First().m_Type0);
            Assert.IsNull(attrs.First().m_Type1);
            Assert.IsNull(attrs.First().m_Type2);

            Assert.AreEqual(GetTestType("Component1"), attrs.Last().m_Type0);
            Assert.IsNull(attrs.Last().m_Type1);
            Assert.IsNull(attrs.Last().m_Type2);
        }

        [Test]
        public void ReflectionForComponent3_ExtractingComponentRequirement_ReturnsDerivedAttributes()
        {
            var r = RequireComponentProcessor.GetComponentRequirements(m_Assembly).Where(a => a.Item1.Name == "Component3");
            Assert.AreEqual(1, r.Count());
            var attrs = r.First().Item2;
            Assert.AreEqual(1, attrs.Count());

            Assert.AreEqual(GetTestType("Component1"), attrs.First().m_Type0);
            Assert.IsNull(attrs.First().m_Type1);
            Assert.IsNull(attrs.First().m_Type2);
        }

        [Test]
        public void ReflectionForComponent4_ExtractingComponentRequirement_ReturnsDirectAndDerivedAttributes()
        {
            var r = RequireComponentProcessor.GetComponentRequirements(m_Assembly).Where(a => a.Item1.Name == "Component4");
            Assert.AreEqual(1, r.Count());
            var attrsUnsorted = r.First().Item2;
            Assert.AreEqual(2, attrsUnsorted.Count());

            var attrs = attrsUnsorted.OrderBy(a => a.m_Type0.ToString());

            Assert.AreEqual(GetTestType("Component0"), attrs.First().m_Type0);
            Assert.IsNull(attrs.First().m_Type1);
            Assert.IsNull(attrs.First().m_Type2);

            Assert.AreEqual(GetTestType("Component1"), attrs.Last().m_Type0);
            Assert.IsNull(attrs.Last().m_Type1);
            Assert.IsNull(attrs.Last().m_Type2);
        }

        [Test]
        public void ReflectionForAssembly_ExtractingComponentRequirement_ReturnsExpected()
        {
            var r = RequireComponentProcessor.GetComponentRequirements(m_Assembly);
            Assert.AreEqual(4, r.Count());
        }

        [Test]
        public void TestingAssembly_WhenProcessingRequireComponent_AddsExpectedEntries()
        {
            var p = new RequireComponentProcessor(null);
            var c = new MockCppCodeGeneration(m_Assembly);
            p.Process(c);

            Assert.Contains(GetTestType("Component0"), c.m_ClassDecls);
            Assert.Contains(GetTestType("Component1"), c.m_ClassDecls);
            Assert.Contains(GetTestType("Component2"), c.m_ClassDecls);
            Assert.Contains(GetTestType("Component3"), c.m_ClassDecls);
            Assert.Contains(GetTestType("Component4"), c.m_ClassDecls);
            Assert.Contains(GetTestType("Component2multi"), c.m_ClassDecls);
            Assert.False(c.m_ClassDecls.Contains(GetTestType("Component5")));

            Assert.IsEmpty(c.m_StructDecls);
            Assert.IsEmpty(c.m_Processors);
        }

        [Test]
        public void TestingAssembly_WhenProcessingRequireComponent_CodegenIncludesIt()
        {
            var codeGen = new CppCodeGeneration("TestModule", m_Assembly);
            codeGen.AddProcessor(new RequireComponentProcessor(null));
            var result = codeGen.Generate();
            Assert.True(result.Contains("g_TestModuleModuleSpec.requireComponentSpec = g_RequireComponentSpecTestModule"));
        }

        private TypeDefinition GetTestType(string name)
        {
            return m_Assembly.MainModule.Types.Single(t => t.Name == name);
        }
    }
}
