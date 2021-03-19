using System.IO;
using System.Linq;
using System.Xml.Linq;
using System.Xml.XPath;
using NUnit.Framework;
using UnderlyingModel;
using UnderlyingModel.Tests;
using XMLFormatter;

namespace XmlFormatter.Tests
{
    [TestFixture]
    public class MonoDocTests
    {
        private string m_OldCurDir;
        private IMemberGetter m_Project;
        private MonoDocGenerator m_Generator;

        [OneTimeSetUp]
        public void Init()
        {
            Directory.SetCurrentDirectory(TestContext.CurrentContext.TestDirectory);
            //arrange
            m_Project = TestUtils.CreateMemberGetterFromTestAssembly();
            m_Generator = new MonoDocGenerator(m_Project);

            m_OldCurDir = Directory.GetCurrentDirectory();
            Directory.SetCurrentDirectory("../..");
            Directory.CreateDirectory("ActualMonoDocs");
        }

        [OneTimeTearDown]
        public void Uninit()
        {
            //restore cur dir
            Directory.SetCurrentDirectory(m_OldCurDir);
        }

        [TestCase("SimpleClassForMonoDoc", "T:SimpleClassForMonoDoc", "This class has docs")]
        [TestCase("SimpleClassForMonoDoc.FieldA", "F:SimpleClassForMonoDoc.FieldA", "docs for FieldA")]
        [TestCase("SimpleClassForMonoDoc.FieldB", "F:SimpleClassForMonoDoc.FieldB", "docs for FieldB")]
        [TestCase("SimpleClassForMonoDoc.PropertyA", "P:SimpleClassForMonoDoc.PropertyA", "docs for PropertyA")]
        [TestCase("SimpleClassForMonoDoc.PropertyB", "P:SimpleClassForMonoDoc.PropertyB", "docs for PropertyB")]
        [TestCase("SimpleClassForMonoDoc.Foo", "M:SimpleClassForMonoDoc.Foo", "docs for Foo")]
        [TestCase("TestAssembly", "A:TestAssembly", "doc for TestAssembly")]
        public void VerifyMemberNameAndSummary(string itemName, string expMemberName, string expectedSummary)
        {
            var aItem = m_Project.GetMember(itemName);

            var actualXML = m_Generator.XmlContentList(aItem).First();

            VerifyMemberName(expMemberName, actualXML);
            var doc = XmlUtils.DocumentFromXElement(actualXML);

            var summaryValue = doc.XPathSelectElement("//member/summary/para").Value;
            Assert.AreEqual(expectedSummary, summaryValue);
        }

        [TestCase("UClass", "T:UnityEngine.UClass")]
        [TestCase("UClass.ctor", "M:UnityEngine.UClass.#ctor")]
        [TestCase("UClass.Foo", "M:UnityEngine.UClass.Foo")]
        [TestCase("UClass.Prop", "P:UnityEngine.UClass.Prop")]
        [TestCase("UClass._fieldA", "F:UnityEngine.UClass.fieldA")]
        [TestCase("UClass.MyDelegate", "T:UnityEngine.UClass.MyDelegate")]
        [TestCase("UEnum", "T:UEnum")]
        [TestCase("SimpleClassForMonoDoc.ctor", "M:SimpleClassForMonoDoc.#ctor")]
        [TestCase("UEnum.One", "F:UEnum.One")]
        [TestCase("SimpleClassForMonoDoc._fieldC", "F:SimpleClassForMonoDoc.fieldC")]
        [TestCase("SimpleStructForMonoDoc", "T:SimpleStructForMonoDoc")]
        [TestCase("SimpleClassForMonoDoc.Foo", "M:SimpleClassForMonoDoc.Foo")]
        [TestCase("SimpleClassForMonoDoc.BarInt", "M:SimpleClassForMonoDoc.BarInt(System.Int32)")]
        [TestCase("SimpleClassForMonoDoc.BarBool", "M:SimpleClassForMonoDoc.BarBool(System.Boolean)")]
        [TestCase("UI.UIDummyClass", "T:UnityEngine.UI.UIDummyClass")]
        [TestCase("UI.UIDummyClass.UIDummyEnum", "T:UnityEngine.UI.UIDummyClass.UIDummyEnum")]
        [TestCase("UI.UIDummyClass.UIDummyEnum.Val0", "F:UnityEngine.UI.UIDummyClass.UIDummyEnum.Val0")]
        [TestCase("TestAssembly", "A:TestAssembly")]
        public void VerifyMemberName(string itemName, string expMemberName)
        {
            var aItem = m_Project.GetMember(itemName);

            var actualXML = m_Generator.GetMemberNameXElement(aItem, 0, 0);
            VerifyMemberName(expMemberName, actualXML);
        }

        [TestCase("FunctionWithNoParameters", "SimpleClassForMonoDoc.Foo",
@"<?xml version=""1.0"" encoding=""utf-8"" standalone=""yes""?>
<members>
  <member name=""M:SimpleClassForMonoDoc.Foo"">
    <summary>
      <para>docs for Foo</para>
    </summary>
  </member>
</members>
")]
        [TestCase("TwoSectionsOneDocEach", "SimpleClassForMonoDoc.Baz",
@"<?xml version=""1.0"" encoding=""utf-8"" standalone=""yes""?>
<members>
  <member name=""M:SimpleClassForMonoDoc.Baz(System.Int32)"">
    <summary>
      <para>docs for Baz(int).</para>
    </summary>
    <param name=""a"">a doc.</param>
  </member>
  <member name=""M:SimpleClassForMonoDoc.Baz(System.Single)"">
    <summary>
      <para>docs for Baz(float).</para>
    </summary>
    <param name=""b"">b doc.</param>
  </member>
</members>
")]
        [TestCase("TwoSectionsDocsTogether", "SimpleClassForMonoDoc.Qux",
@"<?xml version=""1.0"" encoding=""utf-8"" standalone=""yes""?>
<members>
  <member name=""M:SimpleClassForMonoDoc.Qux(System.Int32)"">
    <summary>
      <para>docs for Qux.</para>
    </summary>
    <param name=""a"">a doc.</param>
    <param name=""b"">b doc.</param>
  </member>
  <member name=""M:SimpleClassForMonoDoc.Qux(System.Single)"">
    <summary>
      <para>docs for Qux.</para>
    </summary>
    <param name=""a"">a doc.</param>
    <param name=""b"">b doc.</param>
  </member>
</members>
")]
        [TestCase("ParamAndReturnValues", "SimpleClassForMonoDoc.FooBarBaz",
@"<?xml version=""1.0"" encoding=""utf-8"" standalone=""yes""?>
<members>
  <member name=""M:SimpleClassForMonoDoc.FooBarBaz(System.Int32,System.Single,System.Boolean)"">
    <summary>
      <para>docs for FooBarBaz</para>
    </summary>
    <param name=""foo"">Foo docs.</param>
    <param name=""bar"">Bar docs.</param>
    <param name=""baz"">Baz docs.</param>
    <returns>
      <para>Return doc.</para>
    </returns>
  </member>
</members>
")]

        [TestCase("OptionalParameters", "SimpleClassForMonoDoc.OptionalParameters",
@"<?xml version=""1.0"" encoding=""utf-8"" standalone=""yes""?>
<members>
  <member name=""M:SimpleClassForMonoDoc.OptionalParameters()"">
    <summary>
      <para>Docs for OptionalParameters</para>
    </summary>
    <param name=""a"">a doc.</param>
    <param name=""b"">b doc.</param>
    <param name=""c"">c doc.</param>
  </member>
  <member name=""M:SimpleClassForMonoDoc.OptionalParameters(System.Int32)"">
    <summary>
      <para>Docs for OptionalParameters</para>
    </summary>
    <param name=""a"">a doc.</param>
    <param name=""b"">b doc.</param>
    <param name=""c"">c doc.</param>
  </member>
  <member name=""M:SimpleClassForMonoDoc.OptionalParameters(System.Int32,System.Int32)"">
    <summary>
      <para>Docs for OptionalParameters</para>
    </summary>
    <param name=""a"">a doc.</param>
    <param name=""b"">b doc.</param>
    <param name=""c"">c doc.</param>
  </member>
  <member name=""M:SimpleClassForMonoDoc.OptionalParameters(System.Int32,System.Int32,System.Int32)"">
    <summary>
      <para>Docs for OptionalParameters</para>
    </summary>
    <param name=""a"">a doc.</param>
    <param name=""b"">b doc.</param>
    <param name=""c"">c doc.</param>
  </member>
</members>
")]


        [TestCase("DefaultValues", "SimpleClassForMonoDoc.DefaultValues",
@"<?xml version=""1.0"" encoding=""utf-8"" standalone=""yes""?>
<members>
  <member name=""M:SimpleClassForMonoDoc.DefaultValues()"">
    <summary>
      <para>Docs for DefaultValues</para>
    </summary>
    <param name=""a"">a doc.</param>
    <param name=""b"">b doc.</param>
  </member>
  <member name=""M:SimpleClassForMonoDoc.DefaultValues(System.Int32)"">
    <summary>
      <para>Docs for DefaultValues</para>
    </summary>
    <param name=""a"">a doc.</param>
    <param name=""b"">b doc.</param>
  </member>
  <member name=""M:SimpleClassForMonoDoc.DefaultValues(System.Int32,System.Int32)"">
    <summary>
      <para>Docs for DefaultValues</para>
    </summary>
    <param name=""a"">a doc.</param>
    <param name=""b"">b doc.</param>
  </member>
</members>
")]


        [TestCase("StripMarkup", "SimpleClassForMonoDoc.DocsWithMarkup",
@"<?xml version=""1.0"" encoding=""utf-8"" standalone=""yes""?>
<members>
  <member name=""M:SimpleClassForMonoDoc.DocsWithMarkup"">
    <summary>
      <para>MyClass MyPropertyOrMethod MyClass.MyPropertyOrMethod MyClass.MyPropertyOrMethod MyClass.MyPropertyOrMethod bold bold2 italic italic2 variable variable2 monospaced monospaced2
 See Also:Something (Read Only)</para>
    </summary>
    <param name=""bold"">bold</param>
    <returns>
      <para>italic</para>
    </returns>
  </member>
</members>
")]
        [TestCase("GenericParameter", "SimpleClassForMonoDoc.GenericParameter",
@"<?xml version=""1.0"" encoding=""utf-8"" standalone=""yes""?>
<members>
  <member name=""M:SimpleClassForMonoDoc.GenericParameter(System.Int32,System.Collections.Generic.IDictionary`2&lt;System.Int32,System.Int32&gt;)"">
    <summary>
      <para>doc for GenericParameter</para>
    </summary>
    <param name=""a""></param>
    <param name=""dic""></param>
  </member>
</members>
")]

        [TestCase("MultiDimArray", "SimpleClassForMonoDoc.MultiDimArray",
@"<?xml version=""1.0"" encoding=""utf-8"" standalone=""yes""?>
<members>
  <member name=""M:SimpleClassForMonoDoc.MultiDimArray(System.Int32,System.Int32[,,,])"">
    <summary>
      <para>doc for MultiDimArray</para>
    </summary>
    <param name=""a""></param>
    <param name=""b""></param>
  </member>
</members>
")]

        [TestCase("TestAssembly", "TestAssembly",
@"<?xml version=""1.0"" encoding=""utf-8"" standalone=""yes""?>
<members>
  <member name=""A:TestAssembly"">
    <summary>
      <para>doc for TestAssembly</para>
    </summary>
  </member>
</members>
")]
        public void VerifyFullXml(string testName, string itemName, string expectedXml)
        {
            var aItem = m_Project.GetMember(itemName);

            var actualXmlList = m_Generator.XmlContentList(aItem);
            var wrapListInMembers = new XElement("members", actualXmlList);

            var st = XmlUtils4.XmlToStringWithHeader(wrapListInMembers);
            Assert.AreEqual(expectedXml.WithUnixLineEndings(), st.WithUnixLineEndings());
        }

        private static void VerifyMemberName(string expMemberName, XElement actualXML)
        {
            var doc = XmlUtils.DocumentFromXElement(actualXML);
            var topMember = doc.XPathSelectElement("//member");
            var nameValue = topMember.Attribute("name").Value;
            Assert.AreEqual(expMemberName, nameValue);
        }
    }
}
