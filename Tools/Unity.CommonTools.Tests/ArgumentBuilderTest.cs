using NUnit.Framework;

namespace Unity.CommonTools.Test
{
    [TestFixture]
    public class ArgumentBuilderTest
    {
        private readonly ArgumentBuilder m_argumentBuilder = new ArgumentBuilder();

        [Test]
        public void QuoteArgument_WhenArgumentIsNull_ThenNullExceptionIsThrown()
        {
            TestDelegate act = () => m_argumentBuilder.QuoteArgument(null);

            Assert.That(act, Throws.ArgumentNullException);
        }

        [TestCase("")]
        [TestCase("foo")]
        [TestCase("--foo_bar-baz42")]
        [TestCase(@"z:\foo\bar\baz\")]
        public void QuoteArgument_WhenArgumentDoesNotNeedQuoting_ThenArgumentIsUnaltered(string input)
        {
            var result = m_argumentBuilder.QuoteArgument(input);

            Assert.That(result, Is.EqualTo(input));
        }

        [TestCase(@"foo bar", @"""foo bar""")]
        [TestCase(@"foo(bar)", @"""foo(bar)""")]
        [TestCase(@"foo%bar", @"""foo%bar""")]
        [TestCase(@"foo!bar", @"""foo!bar""")]
        [TestCase(@"foo^bar", @"""foo^bar""")]
        [TestCase(@"foo""bar", @"""foo\""bar""")]
        [TestCase(@"foo<bar", @"""foo<bar""")]
        [TestCase(@"foo>bar", @"""foo>bar""")]
        [TestCase(@"foo&bar", @"""foo&bar""")]
        [TestCase(@"foo|bar", @"""foo|bar""")]
        [TestCase(@"z:\foo\""baz""", @"""z:\foo\\\""baz\""""")]
        [TestCase(@"z:\foo bar\baz\", @"""z:\foo bar\baz\\""")]
        public void QuoteArgument_WhenArgumentNeedsQuoting_ThenArgumentIsQuoted(string input, string expectedResult)
        {
            var result = m_argumentBuilder.QuoteArgument(input);

            Assert.That(result, Is.EqualTo(expectedResult));
        }
    }
}
