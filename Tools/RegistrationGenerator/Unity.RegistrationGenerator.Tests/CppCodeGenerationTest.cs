using Mono.Cecil;
using NUnit.Framework;
using System.Linq;
using System.Text.RegularExpressions;

namespace Unity.RegistrationGenerator.Tests
{
    [TestFixture]
    class CppCodeGenerationTest
    {
        CppCodeGeneration m_CodeGeneration;
        AssemblyDefinition m_Assembly;

        [SetUp]
        public void Init()
        {
            m_Assembly = TestHelpers.LoadExecutingAssembly();
            m_CodeGeneration = new CppCodeGeneration("TestModule", m_Assembly);
        }

        [Test]
        public void CodeGeneration_NoProcessors_CanGenerate()
        {
            m_CodeGeneration.Generate();
        }

        [Test]
        public void CodeGeneration_OneProcessor_RegistersData()
        {
            var p = new MockCodegenProcessor();
            p.m_Includes.Add("Foo.h");
            p.m_Includes.Add("Bar.h");

            p.m_ClassDecls.Add(m_Assembly.MainModule.Types.Single(m => m.Name == "Component1"));
            p.m_ClassDecls.Add(m_Assembly.MainModule.Types.Single(m => m.Name == "Component2"));

            p.m_StructDecls.Add(m_Assembly.MainModule.Types.Single(m => m.Name == "Component2"));
            p.m_StructDecls.Add(m_Assembly.MainModule.Types.Single(m => m.Name == "Component3"));

            m_CodeGeneration.AddProcessor(p);
            m_CodeGeneration.Generate();

            Assert.Contains("Foo.h", m_CodeGeneration.Includes.ToArray());
            Assert.Contains("Bar.h", m_CodeGeneration.Includes.ToArray());
            Assert.Contains(GetTestType("Component1"), m_CodeGeneration.ClassDeclarations.ToArray());
            Assert.Contains(GetTestType("Component2"), m_CodeGeneration.ClassDeclarations.ToArray());
            Assert.Contains(GetTestType("Component2"), m_CodeGeneration.StructDeclarations.ToArray());
            Assert.Contains(GetTestType("Component3"), m_CodeGeneration.StructDeclarations.ToArray());
        }

        [Test]
        public void CodeGeneration_AddRequireComponentProcessor_CodegensContainsExpected()
        {
            var p = new MockCodegenProcessor();
            p.m_Includes.Add("Foo.h");
            p.m_Includes.Add("Bar.h");

            p.m_ClassDecls.Add(m_Assembly.MainModule.Types.Single(m => m.Name == "TestClass1"));
            p.m_ClassDecls.Add(m_Assembly.MainModule.Types.Single(m => m.Name == "TestClass2"));
            p.m_ClassDecls.Add(m_Assembly.MainModule.Types.Single(m => m.Name == "TestClass3"));

            p.m_StructDecls.Add(m_Assembly.MainModule.Types.Single(m => m.Name == "TestStruct1"));
            p.m_StructDecls.Add(m_Assembly.MainModule.Types.Single(m => m.Name == "TestStruct2"));
            p.m_StructDecls.Add(m_Assembly.MainModule.Types.Single(m => m.Name == "TestStruct3"));

            m_CodeGeneration.AddProcessor(p);
            m_CodeGeneration.Generate();

            var result = m_CodeGeneration.Generate();

            var strings = new[]
            {
                "#include \"Foo.h\"",
                "namespace MyNamespace { namespace InnerNamespace { class NativeClass2; } };",
                "namespace MyNamespace { namespace InnerNamespace { struct NativeStruct2; } };",
                p.m_MockData,
                "g_TestModuleModuleSpec.mockCodegenSpec = g_MockCodegenSpecTestModule",
            };

            foreach (var s in strings)
            {
                var m = Regex.Match(result, s, RegexOptions.Multiline);
                Assert.True(m.Success, s + " not Found in " + result);
            }
        }

        [Test]
        public void CodeGeneration_EmptyArrayIsNull()
        {
            var p = new MockCodegenProcessor();
            p.m_MockData = null;
            m_CodeGeneration.AddProcessor(p);
            var result = m_CodeGeneration.Generate();
            Assert.IsTrue(result.Contains("g_MockCodegenSpecTestModule = NULL;"));
        }

        private TypeDefinition GetTestType(string name)
        {
            return m_Assembly.MainModule.Types.Single(t => t.Name == name);
        }
    }
}
