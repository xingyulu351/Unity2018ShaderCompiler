using System.Collections.Generic;
using NUnit.Framework;

/*
    [[ClassName]]               --> <a href="ClassName.html">ClassName</a>
    ::ref::propName         --> <a href="CurrentClass-propName.html">propName</a>
    [[ClassName.propName]]
    ::ref::MethodName       --> <a href="CurrentClass.MethodName.html">MethodName</a>
    [[ClassName.MethodName]] -> also used
    ClassName::ref::propName    --> <a href="ClassName-propName.html">ClassName.propName</a>
    ClassName::ref::MethodName  --> <a href="ClassName.MethodName.html">ClassName.MethodName</a>
    ClassName::pref::propName       --> <a href="ClassName-propName.html">propName</a>
    ClassName::pref::MethodName     --> <a href="ClassName.MethodName.html">MethodName</a>
*/

namespace ScriptRefBase.Tests
{
    [TestFixture]
    public class DescriptionLinkResolverTests
    {
        private LinkResolver m_LinkResolver;
        [OneTimeSetUp]
        public void Init()
        {
            m_LinkResolver = new LinkResolver();
        }

        [TestCase("left ::ref::propName right", "left propName right")]
        [TestCase("left ::ref::MethodName right", "left MethodName right")]
        [TestCase("", "")]
        [TestCase("blah blah blah", "blah blah blah")]
        [TestCase("left [[ClassName]] right", "left ClassName right")]
        [TestCase("left ClassName::pref::propName right", "left propName right")]
        [TestCase("left ClassName::ref::MethodName right", "left ClassName.MethodName right")]

        [TestCase("left {img EditorGUILayoutIntSlider.png} right", "left  right")]
        [TestCase("foo [[wiki:bar]] baz", "foo bar baz")]
        [TestCase("foo [[wiki:bar|baz]] boom", "foo baz boom")]
        [TestCase("foo [[wiki:bar]] baz", "foo bar baz")]
        [TestCase("foo [[wiki:bada boom]] baz", "foo bada boom baz")]
        public void DropStylingTest(string withStyle, string expectedNoStyleText)
        {
            var actualOutput = m_LinkResolver.ParseDescription(withStyle);

            Assert.AreEqual(expectedNoStyleText, actualOutput.PlainText);
        }

        [Test]
        public void RefPropWithClass()
        {
            const string input = "left ::ref::propName right";
            var expectedOutput =
                new MemDocElementList(new List<IMemDocElement>
                {
                    new MemDocElementPlainText("left "),
                    new MemDocElementLink("CurrentClass-propName", "propName"),
                    new MemDocElementPlainText(" right"),
                });
            ResolveAndVerifyWithClass(input, expectedOutput, "CurrentClass");
        }

        [Test]
        public void RefMethodWithClass()
        {
            const string input = "left ::ref::MethodName right";
            var expectedOutput =
                new MemDocElementList(new List<IMemDocElement>
                {
                    new MemDocElementPlainText("left "),
                    new MemDocElementLink("CurrentClass.MethodName", "MethodName"),
                    new MemDocElementPlainText(" right"),
                });
            ResolveAndVerifyWithClass(input, expectedOutput, "CurrentClass");
        }

        private void ResolveAndVerifyWithClass(string input, MemDocElementList expectedOutput, string className)
        {
            m_LinkResolver.CurClass = className;
            ResolveAndVerify(input, expectedOutput);
            m_LinkResolver.CurClass = "";
        }

        private void ResolveAndVerify(string input, MemDocElementList expectedOutput)
        {
            var actualOutput = m_LinkResolver.ParseDescription(input);

            if (expectedOutput.m_Elements.Count != actualOutput.m_Elements.Count)
                Assert.Fail("the two descriptions have different numbers of elements");
            for (int i = 0; i < expectedOutput.m_Elements.Count; i++)
                if (expectedOutput.m_Elements[i].ToString() != actualOutput.m_Elements[i].ToString())
                    Assert.Fail("Descriptions differ at index {0}", i);
        }

        [Test]
        public void Blank()
        {
            const string input = "";
            var expectedOutput = new MemDocElementList(new List<IMemDocElement> {new MemDocElementPlainText("")});
            ResolveAndVerify(input, expectedOutput);
        }

        [Test]
        public void SimpleCase()
        {
            const string input = "blah blah blah";
            var expectedOutput =
                new MemDocElementList(new List<IMemDocElement> {new MemDocElementPlainText("blah blah blah")});
            ResolveAndVerify(input, expectedOutput);
        }

        [Test]
        public void ClassNameTypical()
        {
            const string input = "left [[ClassName]] right";
            var expectedOutput =
                new MemDocElementList(new List<IMemDocElement>
                {
                    new MemDocElementPlainText("left "),
                    new MemDocElementLink("ClassName", "ClassName"),
                    new MemDocElementPlainText(" right")
                });
            ResolveAndVerify(input, expectedOutput);
        }

        [Test]
        public void ClassNameOnlyLeft()
        {
            const string input = "left [[ClassName]]";
            var expectedOutput =
                new MemDocElementList(new List<IMemDocElement>
                {
                    new MemDocElementPlainText("left "),
                    new MemDocElementLink("ClassName", "ClassName"),
                });
            ResolveAndVerify(input, expectedOutput);
        }

        [Test]
        public void ClassNameOnlyRight()
        {
            const string input = "[[ClassName]] right";
            var expectedOutput =
                new MemDocElementList(new List<IMemDocElement>
                {
                    new MemDocElementLink("ClassName", "ClassName"),
                    new MemDocElementPlainText(" right"),
                });
            ResolveAndVerify(input, expectedOutput);
        }

        [Test]
        public void ClassNameOnly()
        {
            const string input = "[[ClassName]]";
            var expectedOutput =
                new MemDocElementList(new List<IMemDocElement>
                {
                    new MemDocElementLink("ClassName", "ClassName"),
                });
            ResolveAndVerify(input, expectedOutput);
        }

        [Test]
        public void ClassName2Instances()
        {
            const string input = "foo [[ClassName1]] bar [[ClassName2]] baz";
            var expectedOutput =
                new MemDocElementList(new List<IMemDocElement>
                {
                    new MemDocElementPlainText("foo "),
                    new MemDocElementLink("ClassName1", "ClassName1"),
                    new MemDocElementPlainText(" bar "),
                    new MemDocElementLink("ClassName2", "ClassName2"),
                    new MemDocElementPlainText(" baz"),
                });
            ResolveAndVerify(input, expectedOutput);
        }

        //first match the right pattern, then the left
        [Test]
        public void RefFollowedByClass()
        {
            const string input = "left ClassName::ref::propName middle [[ClassName]] right";
            var expectedOutput =
                new MemDocElementList(new List<IMemDocElement>
                {
                    new MemDocElementPlainText("left "),
                    new MemDocElementLink("ClassName-propName", "ClassName.propName"),
                    new MemDocElementPlainText(" middle "),
                    new MemDocElementLink("ClassName", "ClassName"),
                    new MemDocElementPlainText(" right"),
                });
            ResolveAndVerify(input, expectedOutput);
        }

        [Test]
        public void RefAlternativeSyntax()
        {
            const string input = "checked using [[GameObject.activeSelf]]. bull";
            var expectedOutput =
                new MemDocElementList(new List<IMemDocElement>
                {
                    new MemDocElementPlainText("checked using "),
                    new MemDocElementLink("GameObject-activeSelf", "GameObject.activeSelf"),
                    new MemDocElementPlainText(". bull"),
                });
            ResolveAndVerify(input, expectedOutput);
        }

        [Test]
        public void ClassPrefProp()
        {
            const string input = "left ClassName::pref::propName right";
            var expectedOutput =
                new MemDocElementList(new List<IMemDocElement>
                {
                    new MemDocElementPlainText("left "),
                    new MemDocElementLink("ClassName-propName", "propName"),
                    new MemDocElementPlainText(" right"),
                });
            ResolveAndVerify(input, expectedOutput);
        }

        [Test]
        public void ClassPrefMethod()
        {
            const string input = "left ClassName::pref::MethodName right";
            var expectedOutput =
                new MemDocElementList(new List<IMemDocElement>
                {
                    new MemDocElementPlainText("left "),
                    new MemDocElementLink("ClassName.MethodName", "MethodName"),
                    new MemDocElementPlainText(" right"),
                });
            ResolveAndVerify(input, expectedOutput);
        }

        [Test]
        public void ClassRefProp()
        {
            const string input = "left ClassName::ref::propName right";
            var expectedOutput =
                new MemDocElementList(new List<IMemDocElement>
                {
                    new MemDocElementPlainText("left "),
                    new MemDocElementLink("ClassName-propName", "ClassName.propName"),
                    new MemDocElementPlainText(" right"),
                });
            ResolveAndVerify(input, expectedOutput);
        }

        [Test]
        public void ClassRefMethod()
        {
            const string input = "left ClassName::ref::MethodName right";
            var expectedOutput =
                new MemDocElementList(new List<IMemDocElement>
                {
                    new MemDocElementPlainText("left "),
                    new MemDocElementLink("ClassName.MethodName", "ClassName.MethodName"),
                    new MemDocElementPlainText(" right"),
                });
            ResolveAndVerify(input, expectedOutput);
        }

        [Test]
        public void ImageRegular()
        {
            const string input = "left {img EditorGUILayoutIntSlider.png} right";
            var expectedOutput =
                new MemDocElementList(new List<IMemDocElement>
                {
                    new MemDocElementPlainText("left "),
                    new MemDocElementImage("EditorGUILayoutIntSlider.png"),
                    new MemDocElementPlainText(" right"),
                });
            ResolveAndVerify(input, expectedOutput);
        }

        [Test]
        public void WikiLink()
        {
            const string input = "foo [[wiki:bar]] baz";
            var expectedOutput =
                new MemDocElementList(new List<IMemDocElement>
                {
                    new MemDocElementPlainText("foo "),
                    new MemDocElementLink("bar", "bar", LinkType.Wiki),
                    new MemDocElementPlainText(" baz"),
                });
            ResolveAndVerify(input, expectedOutput);
        }

        [Test]
        public void WikiManLink()
        {
            const string input = "foo [[wiki:man:bar]] baz";
            var expectedOutput =
                new MemDocElementList(new List<IMemDocElement>
                {
                    new MemDocElementPlainText("foo "),
                    new MemDocElementLink("bar", "bar", LinkType.Wiki),
                    new MemDocElementPlainText(" baz"),
                });
            ResolveAndVerify(input, expectedOutput);
        }

        [Test]
        public void WikiCompLink()
        {
            const string input = "foo [[wiki:comp:bar]] baz";
            var expectedOutput =
                new MemDocElementList(new List<IMemDocElement>
                {
                    new MemDocElementPlainText("foo "),
                    new MemDocElementLink("bar", "bar", LinkType.Wiki),
                    new MemDocElementPlainText(" baz"),
                });
            ResolveAndVerify(input, expectedOutput);
        }

        [Test]
        public void WikiLinkWithText()
        {
            const string input = "foo [[wiki:bar|baz]] boom";
            var expectedOutput =
                new MemDocElementList(new List<IMemDocElement>
                {
                    new MemDocElementPlainText("foo "),
                    new MemDocElementLink("bar", "baz", LinkType.Wiki),
                    new MemDocElementPlainText(" boom"),
                });
            ResolveAndVerify(input, expectedOutput);
        }

        [Test]
        public void WikiLinkPlainWithSpace()
        {
            const string input = "foo [[wiki:bada boom]] baz";
            var expectedOutput =
                new MemDocElementList(new List<IMemDocElement>
                {
                    new MemDocElementPlainText("foo "),
                    new MemDocElementLink("badaboom", "bada boom", LinkType.Wiki),
                    new MemDocElementPlainText(" baz"),
                });
            ResolveAndVerify(input, expectedOutput);
        }
    }
}
