using NUnit.Framework;

namespace UnityBindingsParser.Tests
{
    [TestFixture]
    public class SimpleParserTests
    {
        [Test]
        public void SimpleCommentsNoSpaces()
        {
            const string simpleComment = "//blah";
            Assert.IsTrue(ParserUtil.SimpleCommentMarker.IsMatch(simpleComment));
            Assert.IsFalse(ParserUtil.DocumentationMarker.IsMatch(simpleComment));
        }

        [Test]
        public void ProperCommentNoSpaces()
        {
            const string properComment = "///blah";
            Assert.IsFalse(ParserUtil.SimpleCommentMarker.IsMatch(properComment));
            Assert.IsTrue(ParserUtil.DocumentationMarker.IsMatch(properComment));
        }

        [Test]
        public void SimpleCommentsLeadingSpace()
        {
            const string simpleComment = " //blah";
            Assert.IsTrue(ParserUtil.SimpleCommentMarker.IsMatch(simpleComment));
            Assert.IsFalse(ParserUtil.DocumentationMarker.IsMatch(simpleComment));
        }

        [Test]
        public void ProperCommentsLeadingSpace()
        {
            const string properComment = " ///blah";
            Assert.IsFalse(ParserUtil.SimpleCommentMarker.IsMatch(properComment));
            Assert.IsTrue(ParserUtil.DocumentationMarker.IsMatch(properComment));
        }

        [Test]
        public void SimpleCommentsSpaceAfterSlash()
        {
            const string simpleComment = "// blah";
            Assert.IsTrue(ParserUtil.SimpleCommentMarker.IsMatch(simpleComment));
            Assert.IsFalse(ParserUtil.DocumentationMarker.IsMatch(simpleComment));
        }

        [Test]
        public void ProperCommentsSpaceAfterSlash()
        {
            const string properComment = "/// blah";
            Assert.IsFalse(ParserUtil.SimpleCommentMarker.IsMatch(properComment));
            Assert.IsTrue(ParserUtil.DocumentationMarker.IsMatch(properComment));
        }

        [Test]
        public void SimpleCommentsSpaceBeforeAndAfterSlash()
        {
            const string simpleComment = " // blah";
            Assert.IsTrue(ParserUtil.SimpleCommentMarker.IsMatch(simpleComment));
            Assert.IsFalse(ParserUtil.DocumentationMarker.IsMatch(simpleComment));
        }

        [Test]
        public void ProperCommentsSpaceBeforeAndAfterSlash()
        {
            const string properComment = " /// blah";
            Assert.IsFalse(ParserUtil.SimpleCommentMarker.IsMatch(properComment));
            Assert.IsTrue(ParserUtil.DocumentationMarker.IsMatch(properComment));
        }

        [Test]
        public void ProperCommentsNoText()
        {
            const string properComment = " ///";
            Assert.IsFalse(ParserUtil.SimpleCommentMarker.IsMatch(properComment));
            Assert.IsTrue(ParserUtil.DocumentationMarker.IsMatch(properComment));
        }

        [Test]
        public void SimpleAttribute()
        {
            const string attString = "[Attrib]";
            Assert.IsTrue(ParserUtil.AttributeMarker.IsMatch(attString));
        }
    }
}
