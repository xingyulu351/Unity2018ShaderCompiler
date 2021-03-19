using System.IO;
using NUnit.Framework;
using UnderlyingModel;
using UnderlyingModel.Tests;
using XMLFormatter;

namespace XmlFormatter.Tests
{
    [TestFixture]
    public partial class TestsWithDataProject
    {
        private string m_OldCurDir;
        private const bool m_WriteActualXmlToFile = false;

        [OneTimeSetUp]
        public void Init()
        {
            Directory.SetCurrentDirectory(TestContext.CurrentContext.TestDirectory);
            DirectoryUtil.DeleteAllFiles("Generated");

            m_OldCurDir = Directory.GetCurrentDirectory();
            Directory.SetCurrentDirectory("../..");
            Directory.CreateDirectory("ActualXML");
        }

        [OneTimeTearDown]
        public void Uninit()
        {
            //restore cur dir
            Directory.SetCurrentDirectory(m_OldCurDir);
        }

        //Below are bad tests by design:
        //disabled by markus d on 03/03/2017
        /*
        [TestCase("Animator._animatePhysics")] //property, boolean
        [TestCase("Color._red")] //property with a simple CONVERTEXAMPLE example
        [TestCase("DragAndDrop.AcceptDrag")] //method (static, void return, no params)
        [TestCase("Vector3._x")] //field
        [TestCase("Vector3._sqrMagnitude")] //property, float
        [TestCase("Vector3.ToString")] //method
        [TestCase("Vector3.Min")] //property with links to other properties in the same class
        [TestCase("Bounds.SetMinMax")] //variable name italicized as /blah/
        [TestCase("AndroidJNI.FromLongArray")] //className.Method used + <tt></tt>
        [TestCase("AndroidJavaClass.ctor")] //<tt> syntax
        [TestCase("AndroidJNI.SetShortField")]
        [TestCase("AndroidJavaObject.Dispose")]
        [TestCase("Vector3.op_Plus")]
        [TestCase("Matrix4x4.this")]
        [TestCase("ScriptableObject.OnEnable")]
        [TestCase("Array.Join")]
        [TestCase("Physics._maxAngularVelocity")]
        [TestCase("AndroidJavaObject.CallStatic")] //params
        public void VerifyAgainstExpectedXML(string memberName)
        {
            var project = TestUtils.CreateNewDataItemProjectFromLegacyDlls();

            //here we are testing the ability to resolve links, while in the final docs we also need the page we're pointing to to exist
            var xmlGenerator = new XMLGenerator(project, true, resolveLinksToUndoc: true);
            MemberItem member = xmlGenerator.MemberItemProject.GetMember(memberName);

            Assert.IsNotNull(member, "could not find member for {0}", memberName);

            var actualXML = xmlGenerator.XMLContent(member);

            var expectedPath = Path.Combine("ExpectedXML", memberName + ".xml");
            bool firstTime = !File.Exists(expectedPath);

            var actualPath = Path.Combine("ActualXML", memberName + ".xml");
            if (m_WriteActualXmlToFile)
                XmlUtils4.WriteToFileWithHeader(actualPath, actualXML);
            if (firstTime)
                File.Copy(actualPath, expectedPath);
            else
            {
                string expectedXML = File.ReadAllText(expectedPath);

                string readActualXML = m_WriteActualXmlToFile ? File.ReadAllText(actualPath) : XmlUtils4.XmlToStringWithHeader(actualXML);
                Assert.AreEqual(expectedXML.WithUnixLineEndings(), readActualXML.WithUnixLineEndings());
            }
        }

        [TestCase("NamespaceA.ClassReferencingMovedClassFunction", true)]
        [TestCase("NamespaceA.ClassMovedToNamespace", true)]
        [TestCase("NamespaceA.ClassWithFieldReferencingMovedClass._fieldA", true)]
        [TestCase("ClassWithFunctionReferencingMovedClass.FunctionWithMovedClassArgument", true)]
        [TestCase("GenericClassWithFunction_1.Foo", true)]
        [TestCase("InterfaceA", true)]
        [TestCase("NamespaceI.InterfaceB", true)]
        [TestCase("ClassImplementingInterfaces", true)]
        [TestCase("DocumentedClass", false)]
        [TestCase("MethodWithDefaultValue.Foo", true)]
        [TestCase("MethodWithDefaultValue.DocOnlyFoo", true)]
        public void VerifyExpectedOutputFromTestAssembly(string itemName, bool resolveLinksToUndoc)
        {
            //arrange
            var project = TestUtils.CreateMemberGetterFromTestAssembly();
            var xmlGenerator = new XMLGenerator(project, true, resolveLinksToUndoc);

            //act = get the member
            var aItem = project.GetMember(itemName);

            //assert = make sure this member generates expected XML
            var actualXML = GetActualXml(xmlGenerator, aItem, m_WriteActualXmlToFile);
            string expectedXML = GetExpectedXml(aItem);
            if (!m_WriteActualXmlToFile && !string.Equals(actualXML, expectedXML))
            {
                actualXML = GetActualXml(xmlGenerator, aItem, true);
            }

            Assert.AreEqual(expectedXML, actualXML);
        }

        private string GetExpectedXml(MemberItem aItem)
        {
            const string expectedXmlDir = "ExpectedXML";

            var expectedPath = Path.Combine(expectedXmlDir, aItem.HtmlName + ".xml");
            string expectedXML = File.ReadAllText(expectedPath);

            return expectedXML;
        }

        private string GetActualXml(XMLGenerator generator, MemberItem aItem, bool writeActualXmlToFile)
        {
            var actualXML = generator.XMLContent(aItem);
            if (writeActualXmlToFile)
            {
                const string actualXmlDir = "ActualXML";
                var actualPath = Path.Combine(actualXmlDir, aItem.HtmlName + ".xml");
                XMLGenerator.XmlToFileAtThisDirectory(aItem.HtmlName, actualXML, actualXmlDir);
                string xmlAsString = File.ReadAllText(actualPath);
                return xmlAsString;
            }
            string actualXmlSt = XmlUtils4.XmlToStringWithHeader(actualXML);
            return actualXmlSt;
        }*/
    }
}
