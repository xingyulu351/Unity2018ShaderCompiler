using System;
using NUnit.Framework;
using System.Linq;
using Mono.Cecil;
using UnityEngineTestDLL.FolderOne;


namespace UnityPreserveAttributeParser
{
    [TestFixture]
    public class MethodWrapperTests
    {
        public MethodDefinition GetTestMethod(string className, string methodName)
        {
            var asm = AssemblyDefinition.ReadAssembly(typeof(PublicClass).Assembly.Location);
            var type = asm.MainModule.GetType(className);
            return type.Methods.First((arg) => arg.Name == methodName).Resolve();
        }

        [Test]
        public void TestMethodDeclaration()
        {
            var method = GetTestMethod("UnityEngineTestDLL.FolderOne.TestClass", "InstanceMethodNoArgs");
            var result = MethodCallWrapperGenerator.MethodDefinitionToCppDeclaration(method, "MyModule", MethodCallWrapperGenerator.GenerationOptions.AddThisAsParameter);
            Assert.AreEqual(result, "void InstanceMethodNoArgs(ScriptingObjectPtr _this, ScriptingExceptionPtr *outException=NULL)");
        }

        [Test]
        public void TestMethodDeclarationWithParameters()
        {
            var method = GetTestMethod("UnityEngineTestDLL.FolderOne.TestClass", "InstanceMethodArgs");
            var result = MethodCallWrapperGenerator.MethodDefinitionToCppDeclaration(method, "MyModule", MethodCallWrapperGenerator.GenerationOptions.AddThisAsParameter);
            Assert.AreEqual(result, "void InstanceMethodArgs(ScriptingObjectPtr _this, int arg1, float arg2, core::string arg3, ScriptingObjectPtr arg4, ScriptingExceptionPtr *outException=NULL)");
        }

        [Test]
        public void TestMethodDeclarationWithProxyClassParameter()
        {
            var method = GetTestMethod("UnityEngineTestDLL.FolderOne.TestClass", "InstanceMethodProxyStructArg");
            var result = MethodCallWrapperGenerator.MethodDefinitionToCppDeclaration(method, "MyModule", MethodCallWrapperGenerator.GenerationOptions.ForProxy);
            Assert.AreEqual(result, "void InstanceMethodProxyStructArg( ::Scripting::UnityEngineTestDLL::Proxies::TestStructProxy* arg, ScriptingExceptionPtr *outException=NULL)", result);
        }

        [Test]
        public void TestMethodDeclarationWithReturnType()
        {
            var method = GetTestMethod("UnityEngineTestDLL.FolderOne.TestClass", "InstanceMethodReturnsInt");
            var result = MethodCallWrapperGenerator.MethodDefinitionToCppDeclaration(method, "MyModule", MethodCallWrapperGenerator.GenerationOptions.AddThisAsParameter);
            Assert.AreEqual(result, "int InstanceMethodReturnsInt(ScriptingObjectPtr _this, ScriptingExceptionPtr *outException=NULL)");
        }

        [Test]
        public void TestMethodDeclarationWithArrayReturnType()
        {
            var method = GetTestMethod("UnityEngineTestDLL.FolderOne.TestClass", "InstanceMethodReturnsArray");
            var result = MethodCallWrapperGenerator.MethodDefinitionToCppDeclaration(method, "MyModule", MethodCallWrapperGenerator.GenerationOptions.AddThisAsParameter);
            Assert.AreEqual(result, "ScriptingArrayPtr InstanceMethodReturnsArray(ScriptingObjectPtr _this, ScriptingExceptionPtr *outException=NULL)");
        }

        [Test]
        public void TestMethodDeclarationStatic()
        {
            var method = GetTestMethod("UnityEngineTestDLL.FolderOne.TestClass", "StaticMethodNoArgs");
            var result = MethodCallWrapperGenerator.MethodDefinitionToCppDeclaration(method, "MyModule", MethodCallWrapperGenerator.GenerationOptions.AddThisAsParameter);
            Assert.AreEqual(result, "void StaticMethodNoArgs(ScriptingExceptionPtr *outException=NULL)");
        }

        [Test]
        public void TestMethodDeclarationStaticWithStructArg()
        {
            var method = GetTestMethod("UnityEngineTestDLL.FolderOne.TestClass", "StaticMethodStructArg");
            var result = MethodCallWrapperGenerator.MethodDefinitionToCppDeclaration(method, "MyModule", MethodCallWrapperGenerator.GenerationOptions.AddThisAsParameter);
            Assert.AreEqual(result, "void StaticMethodStructArg(const void* arg, ScriptingExceptionPtr *outException=NULL)");
        }

        [Test]
        public void TestMethodDeclarationStaticWithOutArg()
        {
            var method = GetTestMethod("UnityEngineTestDLL.FolderOne.TestClass", "StaticMethodOutArg");
            var result = MethodCallWrapperGenerator.MethodDefinitionToCppDeclaration(method, "MyModule", MethodCallWrapperGenerator.GenerationOptions.AddThisAsParameter);
            Assert.AreEqual(result, "void StaticMethodOutArg(int* arg, ScriptingExceptionPtr *outException=NULL)");
        }

        [Test]
        public void TestMethodDeclarationForImplementation()
        {
            var method = GetTestMethod("UnityEngineTestDLL.FolderOne.TestClass", "InstanceMethodNoArgs");
            var result = MethodCallWrapperGenerator.MethodDefinitionToCppDeclaration(method, "MyModule", MethodCallWrapperGenerator.GenerationOptions.AddThisAsParameter | MethodCallWrapperGenerator.GenerationOptions.Implementation | MethodCallWrapperGenerator.GenerationOptions.InCommonScriptingClasses);
            Assert.AreEqual(result, "void MyModuleScriptingClasses::InstanceMethodNoArgs(ScriptingObjectPtr _this, ScriptingExceptionPtr *outException)");
        }

        [Test]
        public void TestMethodImplementation()
        {
            var method = GetTestMethod("UnityEngineTestDLL.FolderOne.TestClass", "InstanceMethodArgs");
            var result = MethodCallWrapperGenerator.MethodDefinitionToCppWrapper(method, "MyModule", PreserveState.Required);
            Assert.AreEqual(result,
                "void MyModuleScriptingClasses::InstanceMethodArgs(ScriptingObjectPtr _this, int arg1, float arg2, core::string arg3, ScriptingObjectPtr arg4, ScriptingExceptionPtr *outException)\n" +
                "{\n" +
                "\tScriptingMethodPtr method = GetMyModuleScriptingClasses().instanceMethodArgs;\n" +
                "\tScriptingInvocation invocation(_this, method);\n" +
                "\tinvocation.AddInt(arg1);\n" +
                "\tinvocation.AddFloat(arg2);\n" +
                "\tinvocation.AddString(arg3);\n" +
                "\tinvocation.AddObject(arg4);\n" +
                "\tif (outException != NULL)\n" +
                "\t{\n" +
                "\t\tinvocation.logException = false;\n" +
                "\t\tinvocation.Invoke<void>(outException);\n" +
                "\t}\n" +
                "\telse\n" +
                "\t{\n" +
                "\t\tinvocation.Invoke<void>();\n" +
                "\t}\n" +
                "}\n"
            );
        }

        [Test]
        public void TestMethodImplementationWithReturnType()
        {
            var method = GetTestMethod("UnityEngineTestDLL.FolderOne.TestClass", "InstanceMethodReturnsInt");
            var result = MethodCallWrapperGenerator.MethodDefinitionToCppWrapper(method, "MyModule", PreserveState.Required);
            Assert.AreEqual(result,
                "int MyModuleScriptingClasses::InstanceMethodReturnsInt(ScriptingObjectPtr _this, ScriptingExceptionPtr *outException)\n" +
                "{\n" +
                "\tScriptingMethodPtr method = GetMyModuleScriptingClasses().instanceMethodReturnsInt;\n" +
                "\tint retval;\n" +
                "\tScriptingInvocation invocation(_this, method);\n" +
                "\tif (outException != NULL)\n" +
                "\t{\n" +
                "\t\tinvocation.logException = false;\n" +
                "\t\tretval = invocation.Invoke<int>(outException);\n" +
                "\t}\n" +
                "\telse\n" +
                "\t{\n" +
                "\t\tretval = invocation.Invoke<int>();\n" +
                "\t}\n" +
                "\treturn retval;\n" +
                "}\n"
            );
        }

        [Test]
        public void TestMethodImplementationStaticWithOutArg()
        {
            var method = GetTestMethod("UnityEngineTestDLL.FolderOne.TestClass", "StaticMethodOutArg");
            var result = MethodCallWrapperGenerator.MethodDefinitionToCppWrapper(method, "MyModule", PreserveState.Required);
            Assert.AreEqual(result,
                "void MyModuleScriptingClasses::StaticMethodOutArg(int* arg, ScriptingExceptionPtr *outException)\n" +
                "{\n" +
                "\tScriptingMethodPtr method = GetMyModuleScriptingClasses().staticMethodOutArg;\n" +
                "\tScriptingInvocation invocation(method);\n" +
                "\tinvocation.AddInt(*arg);\n" +
                "\tif (outException != NULL)\n" +
                "\t{\n" +
                "\t\tinvocation.logException = false;\n" +
                "\t\tinvocation.Invoke<void>(outException);\n" +
                "\t}\n" +
                "\telse\n" +
                "\t{\n" +
                "\t\tinvocation.Invoke<void>();\n" +
                "\t}\n" +
                "\t*arg = invocation.Arguments().GetIntAt(0);\n" +
                "}\n"
            );
        }

        [Test]
        public void TestMethodImplementationOptional()
        {
            var method = GetTestMethod("UnityEngineTestDLL.FolderOne.TestClass", "InstanceMethodArgs");
            var result = MethodCallWrapperGenerator.MethodDefinitionToCppWrapper(method, "MyModule", PreserveState.Used);
            Assert.AreEqual(result,
                "void MyModuleScriptingClasses::InstanceMethodArgs(ScriptingObjectPtr _this, int arg1, float arg2, core::string arg3, ScriptingObjectPtr arg4, ScriptingExceptionPtr *outException)\n" +
                "{\n" +
                "\tScriptingMethodPtr method = GetMyModuleScriptingClasses().instanceMethodArgs;\n" +
                "\tif (method.IsNull())\n" +
                "\t\treturn;\n" +
                "\tScriptingInvocation invocation(_this, method);\n" +
                "\tinvocation.AddInt(arg1);\n\tinvocation.AddFloat(arg2);\n" +
                "\tinvocation.AddString(arg3);\n" +
                "\tinvocation.AddObject(arg4);\n" +
                "\tif (outException != NULL)\n" +
                "\t{\n" +
                "\t\tinvocation.logException = false;\n" +
                "\t\tinvocation.Invoke<void>(outException);\n" +
                "\t}\n" +
                "\telse\n" +
                "\t{\n" +
                "\t\tinvocation.Invoke<void>();\n" +
                "\t}\n" +
                "}\n"
            );
        }
    }
}
