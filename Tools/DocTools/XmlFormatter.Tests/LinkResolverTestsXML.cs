using System.Xml.Linq;
using NUnit.Framework;
using ScriptRefBase;
using UnderlyingModel;
using XMLFormatter;

/*
  [[ClassName]]                 --> <a href="ClassName.html">ClassName</a>
  [[ClassName.propName]]
  [[ClassName.MethodName]] -> also used
    ::ref::propName         --> <a href="CurrentClass-propName.html">propName</a>
    ::ref::MethodName       --> <a href="CurrentClass.MethodName.html">MethodName</a>
    ClassName::ref::propName    --> <a href="ClassName-propName.html">ClassName.propName</a>
    ClassName::ref::MethodName  --> <a href="ClassName.MethodName.html">ClassName.MethodName</a>
    ClassName::pref::propName       --> <a href="ClassName-propName.html">propName</a>
    ClassName::pref::MethodName     --> <a href="ClassName.MethodName.html">MethodName</a>
    <tt> something </tt>            --> true type
    @@blah@@  --> monotype
    ''blah''  --> italics
     __blah__ --> bold
     __''blah''__ --> bold + italic
     / var /    --> variable name (spaces required to not confuse with URLs)
*/

namespace XmlFormatter.Tests
{
    [TestFixture]
    public class LinkResolverTestsXML
    {
        private LinkResolver m_LinkResolver;

        [OneTimeSetUp]
        public void Init()
        {
            m_LinkResolver = new LinkResolver();
        }

        private void VerifyResolveLinks(string input, string expectedOutput, string curClass)
        {
            m_LinkResolver.CurClass = curClass;
            var ir = m_LinkResolver.ParseDescription(input);
            XElement actualOutputXML = MemDocOutputterXML.GetXML("boom", ir);
            string actualOutput = actualOutputXML.ToString();
            Assert.AreEqual(expectedOutput.WithUnixLineEndings(), actualOutput.WithUnixLineEndings());
        }

        [TestCase(@"blah ::ref::Find, blah",
            @"<![CDATA[blah ]]><link ref=""Object.Find"">Find</link><![CDATA[, blah]]>", "Object")]
        [TestCase(@"via ::ref::color property.",
            @"<![CDATA[via ]]><link ref=""Material-color"">color</link><![CDATA[ property.]]>", "Material")]
        public void VerifyResolveLinksWithClass(string input, string expectedOutput, string curClass)
        {
            var descriptionOutput = "<boom>" + expectedOutput + "</boom>";
            VerifyResolveLinks(input, descriptionOutput, curClass);
        }

        [TestCase("", "<![CDATA[]]>")]
        [TestCase("blah blah blah", "<![CDATA[blah blah blah]]>")]
        [TestCase("left [[ClassName]] right", "<![CDATA[left ]]><link ref=\"ClassName\">ClassName</link><![CDATA[ right]]>")]
        [TestCase("left [[ClassName]]", "<![CDATA[left ]]><link ref=\"ClassName\">ClassName</link>")]
        [TestCase("[[ClassName]] right", @"
  <link ref=""ClassName"">ClassName</link><![CDATA[ right]]>")]
        [TestCase("[[ClassName]]", @"
  <link ref=""ClassName"">ClassName</link>
")]
        [TestCase(@"[[ClassName]], [[ClassName2]]", @"
  <link ref=""ClassName"">ClassName</link><![CDATA[, ]]><link ref=""ClassName2"">ClassName2</link>")]
        [TestCase(@"GUILayout::ref::Width, GUILayout::ref::Height", @"
  <link ref=""GUILayout.Width"">GUILayout.Width</link><![CDATA[, ]]><link ref=""GUILayout.Height"">GUILayout.Height</link>")]

        [TestCase("foo [[ClassName1]] bar [[ClassName2]] baz",
            @"<![CDATA[foo ]]><link ref=""ClassName1"">ClassName1</link><![CDATA[ bar ]]><link ref=""ClassName2"">ClassName2</link><![CDATA[ baz]]>")]
        [TestCase("foo [[blah|text]] bar",
            @"<![CDATA[foo ]]><link ref=""blah"">text</link><![CDATA[ bar]]>")]         //link with text
        [TestCase("foo [[blah|more text]] bar",
            @"<![CDATA[foo ]]><link ref=""blah"">more text</link><![CDATA[ bar]]>")]         //link with text that has space in it
        [TestCase("foo ''bar'' baz",
            @"<![CDATA[foo ]]><i>bar</i><![CDATA[ baz]]>")]         //italic
        [TestCase("foo __bar__ baz",
            @"<![CDATA[foo ]]><b>bar</b><![CDATA[ baz]]>")]         //boldface
        [TestCase("foo __http://__ bar",
            @"<![CDATA[foo ]]><b>http://</b><![CDATA[ bar]]>")]         //boldface with punctuation
        [TestCase("foo /bar/ baz",
            @"<![CDATA[foo ]]><varname>bar</varname><![CDATA[ baz]]>")]         //varname
        [TestCase("foo __''bar:''__ baz",
            @"<![CDATA[foo ]]><bi>bar:</bi><![CDATA[ baz]]>")]         //bold, italic, with punctuation
        [TestCase("foo __''bar''__ baz",
            @"<![CDATA[foo ]]><bi>bar</bi><![CDATA[ baz]]>")]         //bold, italic
        [TestCase("foo @@bar@@ baz",
            @"<![CDATA[foo ]]><monotype>bar</monotype><![CDATA[ baz]]>")]         //monotype
        [TestCase("foo @@bar(url)@@ baz",
            @"<![CDATA[foo ]]><monotype>bar(url)</monotype><![CDATA[ baz]]>")]         //monotype with non-alpha characters
        [TestCase("foo <tt>bar</tt> baz",
            @"<![CDATA[foo ]]><tt>bar</tt><![CDATA[ baz]]>")]         //true type
        [TestCase("foo <tt>java.lang.String</tt> baz",
            @"<![CDATA[foo ]]><tt>java.lang.String</tt><![CDATA[ baz]]>")]         //true type with punctuation

        [TestCase("foo [[wiki:bar]] baz",
            @"<![CDATA[foo ]]><link ref=""../Manual/bar"">bar</link><![CDATA[ baz]]>")]     //regular wiki link, no text
        [TestCase("foo [[wiki:bada boom]] baz",
            @"<![CDATA[foo ]]><link ref=""../Manual/badaboom"">bada boom</link><![CDATA[ baz]]>")]     //regular wiki link, no text, spaces in name
        [TestCase("foo [[wiki:bar|baz]] boom",
            @"<![CDATA[foo ]]><link ref=""../Manual/bar"">baz</link><![CDATA[ boom]]>")]     //regular wiki link (manual), with text
        [TestCase("[[wiki:class-Animation|Animation component's]]",
@"
  <link ref=""../Manual/class-Animation"">Animation component's</link>
")] //wiki link to a class
        [TestCase("[[wiki:comp-Animation|Animation component's]]",
@"
  <link ref=""../Manual/comp-Animation"">Animation component's</link>
")] //wiki link to a comp-

        [TestCase("[[wiki:comp:GUI Scripting Guide|GUI tutorial]]",
@"
  <link ref=""../Manual/GUIScriptingGuide"">GUI tutorial</link>
")] //wiki link (force component)
        [TestCase("[[wiki:man:GUI Scripting Guide|GUI tutorial]]",
@"
  <link ref=""../Manual/GUIScriptingGuide"">GUI tutorial</link>
")] //wiki link (force manual)

        [TestCase(@"SA: <a href=""http://download.oracle.com/"">Java</a>",
            @"<![CDATA[See Also: ]]><a href=""http://download.oracle.com/"">Java</a>")]
        [TestCase(@"blah blah (RO)", @"<![CDATA[blah blah (Read Only)]]>")]
        [TestCase(@"blah blah\\other line", @"<![CDATA[blah blah]]><br /><![CDATA[other line]]>")]
        [TestCase(@"blah blah<br>other line", @"<![CDATA[blah blah]]><br /><![CDATA[other line]]>")]
        [TestCase(@"blah blah

other line", @"<![CDATA[blah blah]]><br /><br /><![CDATA[other line]]>")]
        [TestCase("blah blah\r\n\r\nother line", @"<![CDATA[blah blah]]><br /><br /><![CDATA[other line]]>")] //windows line endings
        [TestCase(@"using [[GameObject.activeSelf]]", @"<![CDATA[using ]]><link ref=""GameObject-activeSelf"">GameObject.activeSelf</link>")]
        [TestCase(@"SA: [[SocialPlatforms.GameCenter.GameCenterPlatform]].",
            @"<![CDATA[See Also: ]]><link ref=""SocialPlatforms.GameCenter.GameCenterPlatform"">SocialPlatforms.GameCenter.GameCenterPlatform</link><![CDATA[.]]>")]
        public void VerifyResolveLinksDescription(string input, string expectedOutput)
        {
            var descriptionOutput = "<boom>" + expectedOutput + "</boom>";
            VerifyResolveLinks(input, descriptionOutput, "");
        }
    }
}
