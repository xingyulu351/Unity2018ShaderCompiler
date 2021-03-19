using NUnit.Framework;

namespace Unity.BindingsGenerator.TestFramework
{
    [TestFixture]
    public class AssertingStringMatcherTests
    {
        [Test]
        public void ExpectWorks()
        {
            new AssertingStringMatcher("A").Expect("A");
            new AssertingStringMatcher("AB").Expect("A").Expect("B");
        }

        [Test]
        public void ExpectFails()
        {
            Assert.Catch(delegate
            {
                new AssertingStringMatcher("A").Expect("Q");
            });

            Assert.Catch(delegate
            {
                new AssertingStringMatcher("").Expect("Q");
            });

            Assert.Catch(delegate
            {
                new AssertingStringMatcher("A").Expect("A").Expect("Q");
            });
        }

        [Test]
        public void FindWorks()
        {
            new AssertingStringMatcher("BBBA").Find("A").End();
            new AssertingStringMatcher("BABC").Find("A").Find("C").End();
        }

        [Test]
        public void FindFails()
        {
            Assert.Catch(delegate
            {
                new AssertingStringMatcher("BBBA").Find("C");
            });
            Assert.Catch(delegate
            {
                new AssertingStringMatcher("BABC").Find("C").Find("Q");
            });
            Assert.Catch(delegate
            {
                new AssertingStringMatcher("").Find("C");
            });
        }

        [Test]
        public void ExpectWhitespaceWorks()
        {
            new AssertingStringMatcher("  \t\n\r \r\t\nA").ExpectWhitespace().Expect("A").End();
        }

        [Test]
        public void ExpectWhitespaceFails()
        {
            Assert.Catch(delegate
            {
                new AssertingStringMatcher("A").ExpectWhitespace();
            });
            Assert.Catch(delegate
            {
                new AssertingStringMatcher("A").Expect("A").ExpectWhitespace();
            });
        }

        [Test]
        public void OptionalWhitespaceWorks()
        {
            new AssertingStringMatcher("  \t\n\r \r\t\nA").OptionalWhitespace().Expect("A").End();
            new AssertingStringMatcher("A").OptionalWhitespace().Expect("A").End();
            new AssertingStringMatcher("A").Expect("A").OptionalWhitespace().End();
            new AssertingStringMatcher("").OptionalWhitespace().End();
        }

        [Test]
        public void EndWorks()
        {
            new AssertingStringMatcher("").End();
            new AssertingStringMatcher("A").Expect("A").End();
        }

        [Test]
        public void EndFails()
        {
            Assert.Catch(delegate
            {
                new AssertingStringMatcher("A").End();
            });
        }
    }
}
