using Mono.Cecil;
using NUnit.Framework;

namespace Unity.RegistrationGenerator.Tests
{
    [TestFixture]
    class ClassRegistrationTests
    {
        CppCodeGeneration m_CodeGeneration;

        [SetUp]
        public void Init()
        {
            var assembly = TestHelpers.LoadExecutingAssembly();
            m_CodeGeneration = new CppCodeGeneration("TestModule", assembly);
        }

        [Test]
        public void ClassesAreRegistered()
        {
            var p = new ClassRegistrationProcessor(null);

            m_CodeGeneration.AddProcessor(p);

            var result = m_CodeGeneration.Generate();

            var strings = new[]
            {
                "template <> void RegisterUnityClass<Component>(const char* module);",
                "template <> void RegisterUnityClass<MonoBehaviour>(const char* module);",
                "template <> void RegisterUnityClass<MyNamespace::InnerNamespace::NativeClassObjectDerived>(const char* module);",
                "RegisterUnityClass<Component>(\"TestModule\");",
                "RegisterUnityClass<MonoBehaviour>(\"TestModule\");",
                "RegisterUnityClass<MyNamespace::InnerNamespace::NativeClassObjectDerived>(\"TestModule\");",
            };

            foreach (var s in strings)
                Assert.True(result.Contains(s), s + " not Found in " + result);
        }

        [Test]
        public void ClassesDerivedFromScriptableObjectsAreNotRegistered()
        {
            var p = new ClassRegistrationProcessor(null);

            m_CodeGeneration.AddProcessor(p);

            var result = m_CodeGeneration.Generate();

            var strings = new[]
            {
                "template <> void RegisterUnityClass<MonoBehaviour>(const char* module);",
                "RegisterUnityClass<MonoBehaviour>(\"TestModule\");",
            };

            foreach (var s in strings)
                Assert.True(result.Contains(s), s + " not Found in " + result);

            Assert.False(result.Contains("MonoBehaviourDerivedTestClass"),  "MonoBehaviourDerivedTestClass found in " + result);
        }
    }
}
