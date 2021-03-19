using System.Collections.Generic;
using NUnit.Framework;

namespace ScriptRefBase.Tests
{
    [TestFixture]
    public class StyleElementTests
    {
        private LinkResolver m_LinkResolver;
        [OneTimeSetUp]
        public void Init()
        {
            m_LinkResolver = new LinkResolver();
        }

        [TestCase("left __hello__ right", "left hello right")]
        [TestCase("left ''hello'' right", "left hello right")]
        [TestCase("left __''hello''__ right", "left hello right")]
        [TestCase("left / x / right", "left / x / right")]
        [TestCase("left /x/ right", "left x right")]
        [TestCase("left /x/, right", "left x, right")]
        [TestCase("left /x/. right", "left x. right")]
        [TestCase(@"left /x/?", "left x?")]
        [TestCase("left /x.y/ right", "left x.y right")]
        [TestCase(@"left /z/ and /w/ right", "left z and w right")]
        [TestCase("/x/ left /y/ right", "x left y right")]
        [TestCase(@"left https://developer.apple.com/index.html right", "left https://developer.apple.com/index.html right")]
        [TestCase(@"left /Users/joe/myPath.txt right", @"left /Users/joe/myPath.txt right")]
        public void DropStylingTest(string withStyle, string expectedNoStyleText)
        {
            var actualOutput = m_LinkResolver.ParseDescription(withStyle);

            Assert.AreEqual(expectedNoStyleText, actualOutput.PlainText);
        }

        [Test]
        public void BoldFaceTest()
        {
            const string input = "left __hello__ right";
            var expectedOutput =
                new MemDocElementList(new List<IMemDocElement>
                {
                    new MemDocElementPlainText("left "),
                    new MemDocElementStyle("hello", StyleType.Bold),
                    new MemDocElementPlainText(" right"),
                });
            ResolveAndVerify(input, expectedOutput);
        }

        [Test]
        public void ItalicTest()
        {
            const string input = @"left ''hello'' right";
            var expectedOutput =
                new MemDocElementList(new List<IMemDocElement>
                {
                    new MemDocElementPlainText("left "),
                    new MemDocElementStyle("hello", StyleType.Italic),
                    new MemDocElementPlainText(" right"),
                });
            ResolveAndVerify(input, expectedOutput);
        }

        [Test]
        public void BoldItalicTest()
        {
            const string input = @"left __''hello''__ right";
            var expectedOutput =
                new MemDocElementList(new List<IMemDocElement>
                {
                    new MemDocElementPlainText("left "),
                    new MemDocElementStyle("hello", StyleType.BoldItalic),
                    new MemDocElementPlainText(" right"),
                });
            ResolveAndVerify(input, expectedOutput);
        }

        [Test]
        public void OldVarTest()
        {
            const string input = @"left / x / right";
            var expectedOutput =
                new MemDocElementList(new List<IMemDocElement>
                {
                    new MemDocElementPlainText("left / x / right")
                });
            ResolveAndVerify(input, expectedOutput);
        }

        [Test]
        public void VarNoSpacersTest() //var without spaces
        {
            const string input = @"left /x/ right";
            var expectedOutput =
                new MemDocElementList(new List<IMemDocElement>
                {
                    new MemDocElementPlainText("left "),
                    new MemDocElementStyle("x", StyleType.Var),
                    new MemDocElementPlainText(" right")
                });
            ResolveAndVerify(input, expectedOutput);
        }

        [Test]
        public void VarTestComma() //var with comma
        {
            const string input = @"left /x/, right";
            var expectedOutput =
                new MemDocElementList(new List<IMemDocElement>
                {
                    new MemDocElementPlainText("left "),
                    new MemDocElementStyle("x", StyleType.Var),
                    new MemDocElementPlainText(", right")
                });
            ResolveAndVerify(input, expectedOutput);
        }

        [Test]
        public void VarTestPeriod() //var with period
        {
            const string input = @"left /x/. right";
            var expectedOutput =
                new MemDocElementList(new List<IMemDocElement>
                {
                    new MemDocElementPlainText("left "),
                    new MemDocElementStyle("x", StyleType.Var),
                    new MemDocElementPlainText(". right")
                });
            ResolveAndVerify(input, expectedOutput);
        }

        [Test]
        public void VarTestQuestionMark() //var with period
        {
            const string input = @"left /x/?";
            var expectedOutput =
                new MemDocElementList(new List<IMemDocElement>
                {
                    new MemDocElementPlainText("left "),
                    new MemDocElementStyle("x", StyleType.Var),
                    new MemDocElementPlainText("?")
                });
            ResolveAndVerify(input, expectedOutput);
        }

        [Test]
        public void VarTestPeriodInTheMiddle() //var with comma
        {
            const string input = @"left /x.y/ right";
            var expectedOutput =
                new MemDocElementList(new List<IMemDocElement>
                {
                    new MemDocElementPlainText("left "),
                    new MemDocElementStyle("x.y", StyleType.Var),
                    new MemDocElementPlainText(" right")
                });
            ResolveAndVerify(input, expectedOutput);
        }

        [Test]
        public void TwoVars()
        {
            const string input = @"left /z/ and /w/ right";
            var expectedOutput =
                new MemDocElementList(new List<IMemDocElement>
                {
                    new MemDocElementPlainText("left "),
                    new MemDocElementStyle("z", StyleType.Var),
                    new MemDocElementPlainText(" and "),
                    new MemDocElementStyle("w", StyleType.Var),
                    new MemDocElementPlainText(" right")
                });
            ResolveAndVerify(input, expectedOutput);
        }

        [Test]
        public void TwoVarsOneInFront()
        {
            const string input = @"/x/ left /y/ right";
            var expectedOutput =
                new MemDocElementList(new List<IMemDocElement>
                {
                    new MemDocElementStyle("x", StyleType.Var),
                    new MemDocElementPlainText(" left "),
                    new MemDocElementStyle("y", StyleType.Var),
                    new MemDocElementPlainText(" right")
                });
            ResolveAndVerify(input, expectedOutput);
        }

        [Test]
        public void UrlTest()
        {
            const string input = @"left https://developer.apple.com/library/ios/qa/qa1561/_index.html right";
            var expectedOutput =
                new MemDocElementList(new List<IMemDocElement>
                {
                    new MemDocElementPlainText("left https://developer.apple.com/library/ios/qa/qa1561/_index.html right")
                });
            ResolveAndVerify(input, expectedOutput);
        }

        [Test]
        public void WeirdLocalUrlTest()
        {
            const string input = @"left /Users/joe/myPath.txt right";
            var expectedOutput =
                new MemDocElementList(new List<IMemDocElement>
                {
                    new MemDocElementPlainText("left /Users/joe/myPath.txt right")
                });
            ResolveAndVerify(input, expectedOutput);
        }

        private void ResolveAndVerify(string input, MemDocElementList expectedOutput)
        {
            var actualOutput = m_LinkResolver.ParseDescription(input);

            var expectedCount = expectedOutput.m_Elements.Count;
            var actualCount = actualOutput.m_Elements.Count;
            if (expectedCount != actualCount)
                Assert.Fail("the two descriptions have different numbers of elements, expected {0}, actual {1}", expectedCount, actualCount);
            for (int i = 0; i < expectedCount; i++)
            {
                var exp = expectedOutput.m_Elements[i].ToString();
                var act = actualOutput.m_Elements[i].ToString();
                if (exp != act)
                    Assert.Fail("Descriptions differ at index {0}; expected='{1}', actual='{2}'", i, exp, act);
            }
        }
    }
}
