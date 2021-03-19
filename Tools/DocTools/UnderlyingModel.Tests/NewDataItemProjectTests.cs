using System.Collections.Generic;
using System.IO;
using System.Linq;
using NUnit.Framework;

namespace UnderlyingModel.Tests
{
    [TestFixture]
    public class NewDataItemProjectTests
    {
        [SetUp]
        public void Initialize()
        {
            Directory.SetCurrentDirectory(TestContext.CurrentContext.TestDirectory);
        }

        [Test]
        public void EmptyClassExists()
        {
            var project = TestUtils.CreateMemberGetterFromTestAssembly();
            var aItem = project.GetMember("EmptyClass");
            ExistsWithAsmAndDoc(aItem);
        }

        [Test]
        public void DocOnlyClass()
        {
            var project = TestUtils.CreateMemberGetterFromTestAssembly();
            var aItem = project.GetMember("DocOnlyClass");
            Assert.IsNotNull(aItem, "aItem should not be null");
            Assert.IsTrue(aItem.AnyHaveDoc, "Item '{0}' should have a doc", aItem.ItemName);
            Assert.IsTrue(aItem.IsDocOnly);
            Assert.AreEqual(1, aItem.Signatures.Count);
        }

        [Test]
        public void MovedClassDetected()
        {
            var project = TestUtils.CreateMemberGetterFromTestAssembly();
            var aOldItem = project.GetMember("ClassMovedToNamespace");
            Assert.IsTrue(aOldItem.IsObsolete);
            Assert.IsFalse(aOldItem.ObsoleteInfo.IsError);
            var aItem = project.GetMember("NamespaceA.ClassMovedToNamespace");
            ExistsWithAsmAndDoc(aItem);
            Assert.Greater(aItem.ChildMembers.Count, 0, "this class should have children");
        }

        [Test]
        public void NonObsoleteClass_Attributes_Preserved()
        {
            var project = TestUtils.CreateMemberGetterFromTestAssembly();
            var aNotObsoleteClass = project.GetMember("ClassANotObsolete");
            Assert.IsFalse(aNotObsoleteClass.IsObsolete);
            var methodA = project.GetMember("ClassANotObsolete.MethodA");
            var methodB = project.GetMember("ClassANotObsolete.MethodB");
            Assert.IsTrue(methodA.IsObsolete);
            Assert.IsFalse(methodB.IsObsolete);
        }

        [Test]
        public void ObsoleteClass_Attributes_InheritedByMethods()
        {
            var project = TestUtils.CreateMemberGetterFromTestAssembly();
            var aObsoleteClass = project.GetMember("ClassAObsolete");
            Assert.IsTrue(aObsoleteClass.IsObsolete);
            var methodA = project.GetMember("ClassAObsolete.MethodA");
            var methodB = project.GetMember("ClassAObsolete.MethodB");
            Assert.IsTrue(methodA.IsObsolete);
            Assert.IsTrue(methodB.IsObsolete);
        }

        [Test]
        public void ObsoleteClass_Attributes_InheritedByNestedEnum()
        {
            var project = TestUtils.CreateMemberGetterFromTestAssembly();
            var aObsoleteClass = project.GetMember("ClassAObsolete");
            Assert.IsTrue(aObsoleteClass.IsObsolete);
            var enumA = project.GetMember("ClassAObsolete.EnumA");
            Assert.IsTrue(enumA.IsObsolete);
        }

        [Test]
        public void ObsoleteClass_Attributes_InheritedByNestedEnumValues()
        {
            var project = TestUtils.CreateMemberGetterFromTestAssembly();
            var aObsoleteClass = project.GetMember("ClassAObsolete");
            Assert.IsTrue(aObsoleteClass.IsObsolete);
            var enumA = project.GetMember("ClassAObsolete.EnumA.ValueA");
            Assert.IsTrue(enumA.IsObsolete);
        }

        [Test]
        public void NonObsoleteEnum_Attributes_Preserved()
        {
            var project = TestUtils.CreateMemberGetterFromTestAssembly();
            var aNotObsoleteEnum = project.GetMember("EnumANotObsolete");
            Assert.IsFalse(aNotObsoleteEnum.IsObsolete);
            var valueA = project.GetMember("EnumANotObsolete.ValueA");
            var valueB = project.GetMember("EnumANotObsolete.ValueB");
            Assert.IsTrue(valueA.IsObsolete);
            Assert.IsFalse(valueB.IsObsolete);
        }

        [Test]
        public void ObsoleteEnum_Attributes_Inherited()
        {
            var project = TestUtils.CreateMemberGetterFromTestAssembly();
            var aNotObsoleteEnum = project.GetMember("EnumAObsolete");
            Assert.IsTrue(aNotObsoleteEnum.IsObsolete);
            var valueA = project.GetMember("EnumAObsolete.ValueA");
            var valueB = project.GetMember("EnumAObsolete.ValueB");
            Assert.IsTrue(valueA.IsObsolete);
            Assert.IsTrue(valueA.ObsoleteInfo.IsError);
            Assert.AreEqual("I am also obsolete", valueA.ObsoleteInfo.ObsoleteText);
            Assert.IsTrue(valueB.IsObsolete);
        }

        [Test]
        public void FunctionWithMovedClassArgumentDetected()
        {
            var project = TestUtils.CreateMemberGetterFromTestAssembly();
            var aItem = project.GetMember("ClassWithFunctionReferencingMovedClass.FunctionWithMovedClassArgument");
            ExistsWithAsmAndDoc(aItem);
            Assert.AreEqual(1, aItem.FirstSignatureAsm.ParamList.Count);
        }

        [Test]
        public void GenericClassWithFunction()
        {
            var project = TestUtils.CreateMemberGetterFromTestAssembly();

            var classItem = project.GetMember("GenericClassWithFunction_1");
            ExistsWithAsmAndDoc(classItem);

            Assert.Greater(classItem.ChildMembers.Count, 0, "this class should have children");

            var methodItem = project.GetMember("GenericClassWithFunction_1.Foo");
            ExistsWithAsmAndDoc(methodItem);
        }

        [Test]
        public void InterfaceInheritance()
        {
            var project = TestUtils.CreateMemberGetterFromTestAssembly();
            var aItem = project.GetMember("InterfaceA");
            ExistsWithAsmAndDoc(aItem);
            var iiNames = aItem.ImplementedInterfaces.Select(m => m.ItemName);
            Assert.IsTrue(iiNames.Contains("InterfaceAA"));
        }

        [Test]
        public void ClassImplementsInterfaces()
        {
            var project = TestUtils.CreateMemberGetterFromTestAssembly();
            var aItem = project.GetMember("ClassImplementingInterfaces");
            ExistsWithAsmAndDoc(aItem);
            var iiNames = aItem.ImplementedInterfaces.Select(m => m.ItemName).ToList();
            CollectionAssert.AreEqual(
                new[] {"InterfaceA", "InterfaceAA", "InterfaceC", "NamespaceI.InterfaceB" }, iiNames.OrderBy(m => m));
        }

        private static void ExistsWithAsmAndDoc(MemberItem aItem)
        {
            Assert.IsNotNull(aItem, "aItem should not be null");
            Assert.IsTrue(aItem.AnyHaveAsm, "Item '{0}' should have an assembly entry", aItem.ItemName);
            Assert.IsTrue(aItem.AnyHaveDoc, "Item '{0}' should have a doc", aItem.ItemName);
        }
    }
}
