using NUnit.Framework;
using UnderlyingModel.MemDoc;

namespace UnderlyingModel.Tests.MemDoc
{
    [TestFixture]
    public class MemFormatComparisonTests
    {
        [Test]
        public void MemberWith2MeaningfulBlocks()
        {
            const string kInputString = @"<signature>
sig1
sig2
</signature>
Summary for first sigs
<signature>
sig3
sig4
</signature>
Summary for second sigs
";
            var derivedModel = new MemDocModel();
            derivedModel.ParseFromString(kInputString);
            Assert.AreEqual(2, derivedModel.SubSections.Count);
            Assert.AreEqual("Summary for first sigs", derivedModel.SubSections[0].Summary);
            Assert.AreEqual("Summary for second sigs", derivedModel.SubSections[1].Summary);
        }

        //class
        [TestCase(@"<suggestedtype>
class
</suggestedtype>
<txttag>
CSNONE
</txttag>
Arrays ...", "Arrays ...", AssemblyType.Class)]

        //function / method
        [TestCase(@"<suggestedtype>
method
</suggestedtype>
<txttag>
CSNONE
</txttag>
Arrays ...", "Arrays ...", AssemblyType.Method)]

        //property
        [TestCase(@"<suggestedtype>
property
</suggestedtype>
<txttag>
CSNONE
</txttag>
Arrays ...", "Arrays ...", AssemblyType.Property)]

        //property
        [TestCase(@"<suggestedtype>
constructor
</suggestedtype>
<txttag>
CSNONE
</txttag>
Arrays ...", "Arrays ...", AssemblyType.Constructor)]

        //invalid suggestion
        [TestCase(@"
<suggestedtype>
bullshit
</suggestedtype>
<txttag>
CSNONE
</txttag>
Arrays ...", "Arrays ...", AssemblyType.Unknown)]
        //no suggestion
        [TestCase(@"
<txttag>
CSNONE
</txttag>
Arrays ...", "Arrays ...", AssemblyType.Unknown)]

        [Timeout(1000)]
        public void VerifyCsNoneOutput(string input, string expectedSummary, AssemblyType expectedType)
        {
            var derivedModel = new MemDocModel();
            derivedModel.ParseFromString(input);
            Assert.IsTrue(derivedModel.IsDocOnly());
            Assert.AreEqual(expectedType, derivedModel.AssemblyKind);
            Assert.AreEqual(expectedSummary, derivedModel.SubSections[0].Summary);
        }
    }
}
