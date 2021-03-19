using NUnit.Framework;
using UnderlyingModel.MemDoc;

namespace UnderlyingModel.Tests.MemDoc
{
    [TestFixture]
    public class ExampleTests
    {
        [Test]
        public void ExampleToString()
        {
            var example = new ExampleBlock(@"blah");
            const string kExpectedOut =
@"BEGIN EX
blah
END EX";
            Assert.AreEqual(kExpectedOut, example.ToString());
        }

        [Test]
        public void ExampleCsToString()
        {
            var example = new ExampleBlock("blah", CompLang.CSharp);

            const string kExpectedOut =
@"BEGIN EX
---C#---
blah
END EX";
            Assert.AreEqual(kExpectedOut, example.ToString());
        }

        [Test]
        public void DualExampleToString()
        {
            var dual = new ExampleBlock("one", "two");
            const string kExpectedOut =
@"BEGIN EX
one
---C#---
two
END EX";
            Assert.AreEqual(dual.ToString().WithUnixLineEndings(), kExpectedOut.WithUnixLineEndings());
        }

        [Test][Ignore("")]
        public void ExampleModifiedTextToString()
        {
            var example = new ExampleBlock(@"blah") { Text = @"blah2" };
            const string kExpectedOut =
@"BEGIN EX
blah2
END EX";
            Assert.AreEqual(kExpectedOut, example.ToString());
        }

        [Test]
        public void Example2LinesToString()
        {
            var example = new ExampleBlock(
@"blah
blah");
            const string kExpectedOut =
@"BEGIN EX
blah
blah
END EX";
            Assert.AreEqual(kExpectedOut, example.ToString());
        }
    }
}
