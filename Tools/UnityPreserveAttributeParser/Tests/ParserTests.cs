using System;
using System.Collections.Generic;
using System.IO;
using Mono.Cecil;
using NUnit.Framework;
using System.Linq;
using UnityEngineTestDLL.FolderOne;


namespace UnityPreserveAttributeParser.Tests
{
    [TestFixture]
    public class ParserTests
    {
        public class TestOutputWriter : IStrippingInformationOutputHandler
        {
            public HashSet<TypeDefinition> requiredTypes = new HashSet<TypeDefinition>();
            public HashSet<TypeDefinition> usedTypes = new HashSet<TypeDefinition>();
            public HashSet<MethodDefinition> requiredMethods = new HashSet<MethodDefinition>();
            public HashSet<MethodDefinition> usedMethods = new HashSet<MethodDefinition>();
            public HashSet<AssemblyDefinition> usedAssemblies = new HashSet<AssemblyDefinition>();
            public HashSet<string> usedModules = new HashSet<string>();

            public void ProcessAssembly(AssemblyDefinition assembly)
            {
                usedAssemblies.Add(assembly);
            }

            public void ProcessModule(string name)
            {
                usedModules.Add(name);
            }

            public void ProcessType(TypeDefinition type, PreserveState state)
            {
                if (state == PreserveState.Required)
                    requiredTypes.Add(type);
                if (state != PreserveState.Unused)
                    usedTypes.Add(type);
            }

            public void ProcessMethod(MethodDefinition method, PreserveState state)
            {
                if (state == PreserveState.Required)
                    requiredMethods.Add(method);
                if (state != PreserveState.Unused)
                    usedMethods.Add(method);
            }

            public void EndType()
            {
            }

            public void EndModule()
            {
            }

            public void EndAssembly()
            {
            }

            public void End()
            {
            }
        }

        public TestOutputWriter Run(Program.AssemblyProcessor processor, AssemblyDefinition assembly)
        {
            var handler = new TestOutputWriter();

            processor.Handlers.Add(handler);
            processor.Process(new[] {assembly});

            return handler;
        }

        public TestOutputWriter Run(AssemblyDefinition assemblyDefinition)
        {
            return Run(new Program.AssemblyProcessor(), assemblyDefinition);
        }

        public void TestType(string classNameRoot, bool classRequired, bool classIsUsed, bool reqMethodRequired, bool reqMethodUsed, bool usedMethodRequired, bool usedMethodUsed, bool unusedMethodRequired = false, bool unusedMethodUsed = false)
        {
            var assemblyDefinition = AssemblyDefinition.ReadAssembly(typeof(PublicClass).Assembly.Location);
            var output = Run(assemblyDefinition);
            var className = classNameRoot + "One";

            Assert.AreEqual(classRequired, output.requiredTypes.Any(t => t.Name == className));
            Assert.AreEqual(classIsUsed, output.usedTypes.Any(t => t.Name == className));

            Assert.AreEqual(reqMethodRequired, output.requiredMethods.Any(m => m.Name == "RequiredMethod" && m.DeclaringType.Name == className));
            Assert.AreEqual(reqMethodUsed, output.usedMethods.Any(m => m.Name == "RequiredMethod" && m.DeclaringType.Name == className));
            Assert.AreEqual(usedMethodRequired, output.requiredMethods.Any(m => m.Name == "UsedMethod" && m.DeclaringType.Name == className));
            Assert.AreEqual(usedMethodUsed, output.usedMethods.Any(m => m.Name == "UsedMethod" && m.DeclaringType.Name == className));
            Assert.AreEqual(unusedMethodRequired, output.requiredMethods.Any(m => m.Name == "UnusedMethod" && m.DeclaringType.Name == className));
            Assert.AreEqual(unusedMethodUsed, output.usedMethods.Any(m => m.Name == "UnusedMethod" && m.DeclaringType.Name == className));
        }

        public void TestType<T>(string expectedName, bool classRequired, bool classIsUsed, bool reqMethodRequired, bool reqMethodUsed, bool usedMethodRequired, bool usedMethodUsed)
        {
            var assemblyDefinition = AssemblyDefinition.ReadAssembly(typeof(PublicClass).Assembly.Location);
            var output = Run(assemblyDefinition);
            var className = typeof(T).Name;

            Assert.AreEqual(expectedName, Naming.CppNamefor(assemblyDefinition.MainModule.GetType(typeof(T).FullName)));
            Assert.AreEqual(classRequired, output.requiredTypes.Any(t => t.Name == className));
            Assert.AreEqual(classIsUsed, output.usedTypes.Any(t => t.Name == className));

            Assert.AreEqual(reqMethodRequired, output.requiredMethods.Any(m => m.Name == "RequiredMethod" && m.DeclaringType.Name == className));
            Assert.AreEqual(reqMethodUsed, output.usedMethods.Any(m => m.Name == "RequiredMethod" && m.DeclaringType.Name == className));
            Assert.AreEqual(usedMethodRequired, output.requiredMethods.Any(m => m.Name == "UsedMethod" && m.DeclaringType.Name == className));
            Assert.AreEqual(usedMethodUsed, output.usedMethods.Any(m => m.Name == "UsedMethod" && m.DeclaringType.Name == className));
            Assert.IsFalse(output.requiredMethods.Any(m => m.Name == "UnusedMethod" && m.DeclaringType.Name == className));
            Assert.IsFalse(output.usedMethods.Any(m => m.Name == "UnusedMethod" && m.DeclaringType.Name == className));
        }

        public void TestMethod<T>(string methodName, string expectedName, bool isUsed, bool isRequired)
        {
            var assemblyDefinition = AssemblyDefinition.ReadAssembly(typeof(PublicClass).Assembly.Location);
            var output = Run(assemblyDefinition);
            var className = typeof(T).FullName.Replace("+", "/");
            var typeDef = assemblyDefinition.MainModule.GetType(className);

            Assert.AreEqual(expectedName, Naming.CppNamefor(typeDef.Methods.Single(m => m.Name == methodName)));

            Assert.AreEqual(isRequired, output.requiredMethods.Any(m => m.Name == methodName && m.DeclaringType.FullName == className));
            Assert.AreEqual(isUsed, output.usedMethods.Any(m => m.Name == methodName && m.DeclaringType.FullName == className));
        }

        [Test]
        public void TestRequiredClass()
        {
            TestType("RequiredClass", true, true, true, true, false, true);
        }

        [Test]
        public void TestUsedClass()
        {
            TestType("UsedClass", false, true, true, true, false, true);
        }

        [Test]
        public void TestUnusedClass()
        {
            TestType("UnusedClass", false, false, true, true, false, true);
        }

        [Test]
        public void TestUnusedClassWithNoUsedMethods()
        {
            TestType("UnusedClassWithNoUsedMethods", false, false, false, false, false, false);
        }

        [Test]
        public void TestRequiredStruct()
        {
            TestType("RequiredStruct", true, true, true, true, false, true);
        }

        [Test]
        public void TestUsedStruct()
        {
            TestType("UsedStruct", false, true, true, true, false, true);
        }

        [Test]
        public void TestUnusedStruct()
        {
            TestType("UnusedStruct", false, false, true, true, false, true);
        }

        [Test]
        public void TestUnusedStructWithNoUsedMethods()
        {
            TestType("UnusedStructWithNoUsedMethods", false, false, false, false, false, false);
        }

        [Test]
        public void TestRequiredInterface()
        {
            TestType("RequiredInterface", true, true, true, true, false, true, false, false);
        }

        [Test]
        public void TestUsedInterface()
        {
            TestType("UsedInterface", false, true, true, true, false, true, false, false);
        }

        [Test]
        public void TestUnusedInterface()
        {
            TestType("UnusedInterface", false, false, true, true, false, true, false, false);
        }

        [Test]
        public void TestRequiredEnum()
        {
            TestType("RequiredEnum", true, true, false, false, false, false);
        }

        [Test]
        public void TestUsedEnum()
        {
            TestType("UsedEnum", false, true, false, false, false, false);
        }

        [Test]
        public void TestUnusedEnum()
        {
            TestType("UnusedEnum", false, false, false, false, false, false);
        }

        [Test]
        public void TestAttributesAreNotInherited()
        {
            TestType("SuperRequiredClass", false, false, false, false, false, false);
        }

        [Test]
        public void TestCustomAttributeNameWorksForUsedAttribute()
        {
            TestType<WithCustomNameOne>("customName", false, true, false, false, false, false);
        }

        [Test]
        public void TestCustomAttributeNameAsFieldWorksForUsedAttribute()
        {
            TestType<WithCustomNameAsFieldOne>("customName", false, true, false, false, false, false);
        }

        [Test]
        public void TestCustomAttributeNameWorksForRequiredAttribute()
        {
            TestType<WithCustomNameRequiredOne>("customNameRequired", true, true, false, false, false, false);
        }

        [Test]
        public void TestCustomAttributeNameAsFieldWorksForRequiredAttribute()
        {
            TestType<WithCustomNameRequiredAsFieldOne>("customNameRequired", true, true, false, false, false, false);
        }

        [Test]
        public void TestNamingWorksForMethodsWithCustomName()
        {
            var assemblyDefinition = AssemblyDefinition.ReadAssembly(typeof(PublicClass).Assembly.Location);
            var typeDef = assemblyDefinition.MainModule.GetType(typeof(WithMethodWithCustomNamesOne).FullName);

            Assert.AreEqual("customNameRequired", Naming.CppNamefor(typeDef.Methods.Single(m => m.Name == "RequiredMethod")));
            Assert.AreEqual("customName", Naming.CppNamefor(typeDef.Methods.Single(m => m.Name == "UsedMethod")));
            Assert.AreEqual("unusedMethod", Naming.CppNamefor(typeDef.Methods.Single(m => m.Name == "UnusedMethod")));
        }

        [Test]
        public void TestNamingWorksForMethodsWithCustomNameByField()
        {
            var assemblyDefinition = AssemblyDefinition.ReadAssembly(typeof(PublicClass).Assembly.Location);
            var typeDef = assemblyDefinition.MainModule.GetType(typeof(WithMethodWithCustomNamesAsFieldOne).FullName);

            Assert.AreEqual("customNameRequired", Naming.CppNamefor(typeDef.Methods.Single(m => m.Name == "RequiredMethod")));
            Assert.AreEqual("customName", Naming.CppNamefor(typeDef.Methods.Single(m => m.Name == "UsedMethod")));
            Assert.AreEqual("unusedMethod", Naming.CppNamefor(typeDef.Methods.Single(m => m.Name == "UnusedMethod")));
        }

        [Test]
        public void TestNamingWorksForNestedTypes()
        {
            var assemblyDefinition = AssemblyDefinition.ReadAssembly(typeof(PublicClass).Assembly.Location);

            var typeDef1 = assemblyDefinition.MainModule.GetType(typeof(ParentClassOne.RequiredNestedOne).FullName.Replace("+", "/"));
            var typeDef2 = assemblyDefinition.MainModule.GetType(typeof(ParentClassOne.RequiredNested2One).FullName.Replace("+", "/"));

            Assert.AreEqual("parentClassOneRequiredNestedOne", Naming.CppNamefor(typeDef1));
            Assert.AreEqual("customName", Naming.CppNamefor(typeDef2));
        }

        [Test]
        public void TestRequiredAttributeWithOptionalParameterSetToTrue()
        {
            TestType<RequiredClassMarkedOptional1One>("requiredClassMarkedOptional1One", false, true, false, false, false, false);
            TestType<RequiredClassMarkedOptional2One>("requiredClassMarkedOptional2One", false, true, false, false, false, false);
            TestType<RequiredClassMarkedOptional3One>("customName", false, true, false, false, false, false);
            TestType<RequiredClassMarkedOptional4One>("customName", false, true, false, false, false, false);
            TestType<RequiredClassMarkedOptional5One>("customName", false, true, false, false, false, false);
        }

        [Test]
        public void TestRequiredAttributeWithOptionalParameterForNestedType()
        {
            TestMethod<TheParentClassOne.RequiredNestedOne>("RequiredMethod1", "requiredMethod1", true, true);
            TestMethod<TheParentClassOne.RequiredNestedOne>("RequiredMethod2", "requiredMethod2", true, true);
            TestMethod<TheParentClassOne.RequiredNestedOne>("RequiredMethod3", "requiredMethod3", true, true);
            TestMethod<TheParentClassOne.RequiredNestedOne>("RequiredMethod4", "customName", true, true);
            TestMethod<TheParentClassOne.RequiredNestedOne>("RequiredMethod5", "customName", true, true);
            TestMethod<TheParentClassOne.RequiredNestedOne>("RequiredMethod6", "customName", true, true);

            TestMethod<TheParentClassOne.RequiredNestedOne>("UsedMethod1", "usedMethod1", true, false);
            TestMethod<TheParentClassOne.RequiredNestedOne>("UsedMethod2", "usedMethod2", true, false);
            TestMethod<TheParentClassOne.RequiredNestedOne>("UsedMethod3", "customName", true, false);
            TestMethod<TheParentClassOne.RequiredNestedOne>("UsedMethod4", "customName", true, false);
            TestMethod<TheParentClassOne.RequiredNestedOne>("UsedMethod5", "customName", true, false);
        }

        [Test]
        public void TestInheritance()
        {
            var assembly = typeof(UnityEngineTestDLL.Inheritance.TheParentClassOne).Assembly;
            var assemblyDefinition = AssemblyDefinition.ReadAssembly(assembly.Location);
            var processor = new Program.AssemblyProcessor();
            var handler = new ManagedProxies.CommonScriptingClassesWriter(null, null);

            var buildDir = System.Reflection.Assembly.GetAssembly(this.GetType()).Location;
            buildDir = Path.GetDirectoryName(buildDir);
            var testDir = Path.Combine(buildDir, "../../Tests/TestDLL");
            if (Directory.Exists(testDir))
                Directory.SetCurrentDirectory(testDir);
            processor.Handlers.Add(handler);
            processor.Process(new[] {assemblyDefinition});

            Assert.AreEqual(4, handler.SortedProcessedTypes.Count);
            Assert.AreEqual("UnityEngineTestDLL.Inheritance.TheBaseClassOne", handler.SortedProcessedTypes[0].FullName);
            Assert.AreEqual("UnityEngineTestDLL.Inheritance.TheOtherBaseClassOne", handler.SortedProcessedTypes[1].FullName);
            Assert.AreEqual("UnityEngineTestDLL.Inheritance.TheOtherParentClassOne", handler.SortedProcessedTypes[2].FullName);
            Assert.AreEqual("UnityEngineTestDLL.Inheritance.TheParentClassOne", handler.SortedProcessedTypes[3].FullName);
        }
    }
}
