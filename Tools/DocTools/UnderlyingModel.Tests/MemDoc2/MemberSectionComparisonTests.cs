using System.Collections.Generic;
using NUnit.Framework;
using UnderlyingModel.MemDoc;
using UnderlyingModel.MemDoc.Comparers;

namespace UnderlyingModel.Tests.MemDoc2
{
    public class MemberSectionComparisonTests
    {
        /* These tests validate MemberSectionComparer
         */
        [Test]
        public void DifferentParameterSameDoc()
        {
            var param1 = new ParameterWithDoc("x") {Doc = "hello"};
            var param2 = new ParameterWithDoc("y") {Doc = "hello"};
            var section1 = new MemberSubSection
            {
                Parameters = new List<ParameterWithDoc> {param1}
            };

            var section2 = new MemberSubSection
            {
                Parameters = new List<ParameterWithDoc> {param2}
            };
            AssertSectionsNotEqual(section1, section2);
        }

        [Test]
        public void SameParameterDifferentDoc()
        {
            var param1 = new ParameterWithDoc("x") { Doc = "hello" };
            var param2 = new ParameterWithDoc("x") { Doc = "bye" };
            var section1 = new MemberSubSection
            {
                Parameters = new List<ParameterWithDoc> { param1 }
            };

            var section2 = new MemberSubSection
            {
                Parameters = new List<ParameterWithDoc> { param2 }
            };
            AssertSectionsNotEqual(section1, section2);
        }

        [Test]
        public void DifferentNumParams()
        {
            var param1 = new ParameterWithDoc("x") { Doc = "hello" };
            var param2 = new ParameterWithDoc("y") { Doc = "bye" };
            var section1 = new MemberSubSection
            {
                Parameters = new List<ParameterWithDoc> { param1 }
            };

            var section2 = new MemberSubSection
            {
                Parameters = new List<ParameterWithDoc> { param1, param2 }
            };
            AssertSectionsNotEqual(section1, section2);
        }

        [Test]
        public void DifferentReturnDoc()
        {
            var section1 = new MemberSubSection
            {
                ReturnDoc = new ReturnWithDoc { Doc = "hello" }
            };

            var section2 = new MemberSubSection
            {
                ReturnDoc = new ReturnWithDoc { Doc = "bye" }
            };
            AssertSectionsNotEqual(section1, section2);
        }

        [Test]
        public void ReturnDocOneIsEmpty()
        {
            var section1 = new MemberSubSection();

            var section2 = new MemberSubSection
            {
                ReturnDoc = new ReturnWithDoc { Doc = "bye" }
            };
            AssertSectionsNotEqual(section1, section2);
        }

        [Test]
        public void ReturnDocOtherIsEmpty()
        {
            var section1 = new MemberSubSection
            {
                ReturnDoc = new ReturnWithDoc { Doc = "hello" }
            };
            var section2 = new MemberSubSection();

            AssertSectionsNotEqual(section1, section2);
        }

        [Test]
        public void EmptySections()
        {
            var section1 = new MemberSubSection();
            var section2 = new MemberSubSection();

            AssertSectionsEqual(section1, section2);
        }

        [Test]
        public void TextBlocksDiffNumber10()
        {
            var section1 = new MemberSubSection
            {
                TextBlocks = new List<TextBlock> { new DescriptionBlock("hello")}
            };
            var section2 = new MemberSubSection();
            AssertSectionsNotEqual(section1, section2);
        }

        [Test]
        public void TextBlocksDiffNumber12()
        {
            var section1 = new MemberSubSection
            {
                TextBlocks = new List<TextBlock> { new DescriptionBlock("hello") }
            };
            var section2 = new MemberSubSection
            {
                TextBlocks = new List<TextBlock> { new DescriptionBlock("hello"), new DescriptionBlock("goodbye")}
            };
            AssertSectionsNotEqual(section1, section2);
        }

        [Test]
        public void TextBlocksDiffText()
        {
            var section1 = new MemberSubSection
            {
                TextBlocks = new List<TextBlock> { new DescriptionBlock("hello") }
            };
            var section2 = new MemberSubSection
            {
                TextBlocks = new List<TextBlock> { new DescriptionBlock("goodbye") }
            };

            AssertSectionsNotEqual(section1, section2);
        }

        [Test]
        public void TextBlocksDiffTypes()
        {
            var section1 = new MemberSubSection
            {
                TextBlocks = new List<TextBlock> { new DescriptionBlock("hello") }
            };
            var section2 = new MemberSubSection
            {
                TextBlocks = new List<TextBlock> { new ExampleBlock("blah")}
            };
            AssertSectionsNotEqual(section1, section2);
        }

        private void AssertSectionsNotEqual(MemberSubSection section1, MemberSubSection section2)
        {
            var comparer = new MemberSectionComparer();
            Assert.AreNotEqual(0, comparer.Compare(section1, section2), "expecting sections {0} to be different, but they're not", section1);
        }

        private void AssertSectionsEqual(MemberSubSection section1, MemberSubSection section2)
        {
            var comparer = new MemberSectionComparer();
            Assert.IsTrue(comparer.Compare(section1, section2).Success, "expecting {0}, but got {1}", section1, section2);
        }
    }
}
