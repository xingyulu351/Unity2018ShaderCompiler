using System;
using NUnit.Framework;
using System.Linq;
using Mono.Cecil;
using System.Collections.Generic;
using UnityEngineTestDLL.FolderOne;


namespace UnityPreserveAttributeParser
{
    [TestFixture]
    public class ProxyWrapperTests
    {
        public TypeDefinition GetTestType(string className)
        {
            var asm = AssemblyDefinition.ReadAssembly(typeof(PublicClass).Assembly.Location);
            return asm.MainModule.GetType(className);
        }

        [Test]
        public void TestProxyHeaderNamespace()
        {
            var type = GetTestType("UnityEngineTestDLL.Proxies.TestStruct");
            var header = new ManagedProxies.HeaderGenerator(type, "TestModule", new List<MethodReference>()).Generate();
            Assert.IsTrue(header.Contains("namespace Scripting { namespace UnityEngineTestDLL { namespace Proxies {"));
        }

        [Test]
        public void TestProxyHeaderStructFields()
        {
            var type = GetTestType("UnityEngineTestDLL.Proxies.TestStruct");
            var header = new ManagedProxies.HeaderGenerator(type, "TestModule", new List<MethodReference>()).Generate();
            Assert.IsTrue(header.Contains("float FloatField;"));
            Assert.IsTrue(header.Contains("inline float get_FloatField () const { return FloatField; }"));
            Assert.IsTrue(header.Contains("inline void set_FloatField (float const & _value) { FloatField = _value; };"));

            Assert.IsTrue(header.Contains("Scripting::UnityEngineTestDLL::Proxies::TestClassProxy ObjectField;"));
            Assert.IsTrue(header.Contains("ScriptingArrayOf< ::Scripting::UnityEngineTestDLL::Proxies::TestClassProxy> ArrayField;"));
        }

        [Test]
        public void TestProxyHeaderStructTraits()
        {
            var type = GetTestType("UnityEngineTestDLL.Proxies.TestStruct");
            var header = new ManagedProxies.HeaderGenerator(type, "TestModule", new List<MethodReference>()).Generate();
            Assert.IsTrue(header.Contains("static const bool IsScriptingObjectPtr = false;"));
            Assert.IsTrue(header.Contains("static const bool Blittable = false;"));
            Assert.IsTrue(header.Contains("static const bool ReferenceType = false;"));
            Assert.IsTrue(header.Contains("static const bool ValueType = true;"));
        }

        [Test]
        public void TestProxyHeaderStructNativeHeader()
        {
            var type = GetTestType("UnityEngineTestDLL.Proxies.TestStruct");
            var csw = new ManagedProxies.CommonScriptingClassesWriter("header.h", "source.cpp");
            csw.ProcessModule("TestModule");
            csw.ProcessAssembly(AssemblyDefinition.ReadAssembly(typeof(PublicClass).Assembly.Location));
            csw.ProcessType(type, PreserveState.Required);
            csw.EndAssembly();
            csw.EndModule();

            Assert.IsTrue(csw.Source.Contains("#include \"NativeHeader.h\""));
        }

        [Test]
        public void TestProxyHeaderStructNativeStructConstructor()
        {
            var type = GetTestType("UnityEngineTestDLL.Proxies.TestStruct");
            var header = new ManagedProxies.HeaderGenerator(type, "TestModule", new List<MethodReference>()).Generate();
            var source = new ManagedProxies.SourceGenerator(type, "TestModule", new List<MethodReference>()).Generate();
            Assert.IsTrue(header.Contains("TestStructProxy (const ::Test::NativeStruct& native)"));
            Assert.IsTrue(source.Contains("set_FloatField(native.NativeFloatField);"));
            Assert.IsTrue(header.Contains("static const bool Blittable = false;"));
            Assert.IsTrue(header.Contains("operator ::Test::NativeStruct () const"));
            Assert.IsTrue(source.Contains("native.NativeFloatField = get_FloatField();"));
        }

        [Test]
        public void TestProxyHeaderContainsForwardDeclarationsForNativeType()
        {
            var type = GetTestType("UnityEngineTestDLL.Proxies.TestStruct");

            var cscw = new ManagedProxies.CommonScriptingClassesWriter("", "");
            cscw.ProcessModule("TestModule");
            cscw.ProcessAssembly(type.Module.Assembly);
            cscw.ProcessType(type, PreserveState.Used);
            cscw.EndModule();
            Assert.IsTrue(cscw.Header.Contains("namespace Test { class NativeStruct;}"));
        }

        [Test]
        public void TestProxyHeaderContainsForwardDeclarationsForNativeTypesWithCustomForwardDeclaration()
        {
            var type = GetTestType("UnityEngineTestDLL.Proxies.TestClass");

            var cscw = new ManagedProxies.CommonScriptingClassesWriter("", "");
            cscw.ProcessModule("TestModule");
            cscw.ProcessAssembly(type.Module.Assembly);
            cscw.ProcessType(type, PreserveState.Used);
            cscw.EndModule();
            Assert.IsTrue(cscw.Header.Contains("struct NativeClassCustomForwarderDeclaration;"));
        }

        [Test]
        public void TestProxyHeaderClassFields()
        {
            var type = GetTestType("UnityEngineTestDLL.Proxies.TestClass");
            var header = new ManagedProxies.HeaderGenerator(type, "TestModule", new List<MethodReference>()).Generate();

            Assert.IsTrue(header.Contains("return ScriptingTraits<float>::GetterMethod(*this, s_AccessInfo_FloatField.fieldPtr.GetBackendPtr()->getterMethodPtr);"));
            Assert.IsTrue(header.Contains("ScriptingTraits<float>::SetterMethod(*this, _value, s_AccessInfo_FloatField.fieldPtr.GetBackendPtr()->setterMethodPtr);"));
        }

        [Test]
        public void TestProxyHeaderClassDeclaration()
        {
            var type = GetTestType("UnityEngineTestDLL.Proxies.TestClass");
            var header = new ManagedProxies.HeaderGenerator(type, "TestModule", new List<MethodReference>()).Generate();
            Assert.IsTrue(header.Contains("class TestClassProxy : public  ::Scripting::UnityEngineTestDLL::Proxies::BaseClassProxy"));
        }

        [Test]
        public void TestProxyHeaderNestedClassDeclaration()
        {
            var type = GetTestType("UnityEngineTestDLL.Proxies.BaseClass/NestedClass");
            var header = new ManagedProxies.HeaderGenerator(type, "TestModule", new List<MethodReference>()).Generate();
            Assert.IsTrue(header.Contains("class NestedClassProxy"));
            Assert.IsTrue(header.Contains("::Scripting::UnityEngineTestDLL::Proxies::BaseClass::NestedClassProxy"));
            Assert.IsTrue(header.Contains("namespace Scripting { namespace UnityEngineTestDLL { namespace Proxies { namespace BaseClass {"));
        }

        [Test]
        public void TestProxyHeaderClassTraits()
        {
            var type = GetTestType("UnityEngineTestDLL.Proxies.TestClass");
            var header = new ManagedProxies.HeaderGenerator(type, "TestModule", new List<MethodReference>()).Generate();
            Assert.IsTrue(header.Contains("static const bool IsScriptingObjectPtr = true;"));
            Assert.IsTrue(header.Contains("static const bool Blittable = false;"));
            Assert.IsTrue(header.Contains("static const bool ReferenceType = true;"));
            Assert.IsTrue(header.Contains("static const bool ValueType = false;"));
        }

        [Test]
        public void TestProxySourceClassProxySetup()
        {
            var type = GetTestType("UnityEngineTestDLL.Proxies.TestClass");
            var source = new ManagedProxies.SourceGenerator(type, "TestModule", new List<MethodReference>()).Generate();

            Assert.IsTrue(source.Contains("TestClassProxy::s_ScriptingClass = OptionalType(\"TestDLL.dll\", \"UnityEngineTestDLL.Proxies\", \"TestClass\");"));
            Assert.IsTrue(source.Contains("TestClassProxy::s_AccessInfo_FloatField.fieldPtr = scripting_class_get_field_from_name (TestClassProxy::s_ScriptingClass, \"FloatField\");"));
            Assert.IsTrue(source.Contains("TestClassProxy::s_AccessInfo_FloatField.offset = scripting_field_get_offset (TestClassProxy::s_AccessInfo_FloatField.fieldPtr);"));
            Assert.IsTrue(source.Contains("TestClassProxy::s_ScriptingClass = SCRIPTING_NULL;"));
            Assert.IsTrue(source.Contains("TestClassProxy::s_AccessInfo_FloatField.fieldPtr = SCRIPTING_NULL;"));
            Assert.IsTrue(source.Contains("TestClassProxy::s_AccessInfo_FloatField.offset = -1;"));
        }

        [Test]
        public void TestProxyHeaderIncludesBaseType()
        {
            var type = GetTestType("UnityEngineTestDLL.Proxies.TestClass");

            var cscw = new ManagedProxies.CommonScriptingClassesWriter("", "");
            cscw.ProcessModule("TestModule");
            cscw.ProcessAssembly(type.Module.Assembly);
            cscw.ProcessType(type, PreserveState.Used);
            cscw.EndModule();
            Console.WriteLine(cscw.Header);
            Assert.IsTrue(cscw.Header.Contains("class BaseClassProxy : public ScriptingObjectPtr"));
        }
    }
}
