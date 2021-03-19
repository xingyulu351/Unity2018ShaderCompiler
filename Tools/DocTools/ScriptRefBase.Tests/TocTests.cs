using System.Collections.Generic;
using System.Linq;
using NUnit.Framework;

namespace ScriptRefBase.Tests
{
    [TestFixture]
    public class TocTests
    {
        [Test]
        public void PsVitaNsReturnsExpectedNestedness()
        {
            var nsList = new[] { "UnityEngine.PSVita" };
            var nsSetExpected = new Dictionary<string, int>
            {
                {"UnityEngine.PSVita", 2},
                {"UnityEngine", 1},
            };
            var nsSetActual = TocPopulator.AssignNestednessLevelToNamespaces(nsList.ToList());
            Assert.AreEqual(nsSetExpected, nsSetActual);
        }

        [Test]
        public void PsVitansReturnsExpectedToc()
        {
            var nsList = new[] { "UnityEngine.PSVita" };

            var nsTreeExpected = new TocRepresentation();
            nsTreeExpected.AddBaseElement("UnityEngine", "UnityEngine", TocEntryType.Namespace);
            nsTreeExpected.AddChild("UnityEngine", "UnityEngine.PSVita", "UnityEngine.PSVita", TocEntryType.Namespace);
            var nsTreeActual = TocPopulator.GetTocRepresentationFromNsList(nsList.ToList());

            TocRepresentationCompare(nsTreeExpected, nsTreeActual);
        }

        private void TocRepresentationCompare(TocRepresentation expected, TocRepresentation actual)
        {
            Assert.AreEqual(expected.Elements.Count, actual.Elements.Count, "number of elements doesn't match");
            for (int i = 0; i < expected.Elements.Count; i++)
            {
                Assert.AreEqual(expected.Elements[i].ItemName, actual.Elements[i].ItemName, "ItemName at index " + i + " doesn't match");
                Assert.AreEqual(expected.Elements[i].DisplayName, actual.Elements[i].DisplayName, "DisplayName at index " + i + " doesn't match");
                Assert.AreEqual(expected.Elements[i].EntryType, actual.Elements[i].EntryType, "EntryType at index " + i + " doesn't match");
                Assert.AreEqual(expected.Elements[i].Children.Count, actual.Elements[i].Children.Count, "Children count at index " + i + " doesn't match");
                if (expected.Elements[i].Parent == null)
                    Assert.AreEqual(expected.Elements[i].Parent, actual.Elements[i].Parent, "Parent (null) at index " + i + " doesn't match");
                else
                    Assert.AreEqual(expected.Elements[i].Parent.ItemName, actual.Elements[i].Parent.ItemName, "Parent at index " + i + " doesn't match");
            }
        }
    }
}
