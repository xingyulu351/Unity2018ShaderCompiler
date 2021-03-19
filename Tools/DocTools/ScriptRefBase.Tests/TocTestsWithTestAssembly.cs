using System;
using System.IO;
using System.Linq;
using NUnit.Framework;
using UnderlyingModel;
using UnderlyingModel.Tests;

namespace ScriptRefBase.Tests
{
    [TestFixture]
    public class TocTestsWithTestAssembly
    {
        private IMemberGetter m_MemberGetter;
        private TocRepresentation m_TocRepresentation;

        [OneTimeSetUp]
        public void Init()
        {
            Directory.SetCurrentDirectory(TestContext.CurrentContext.TestDirectory);
            m_MemberGetter = TestUtils.CreateMemberGetterFromTestAssembly();
            m_TocRepresentation = TocPopulator.MakeTableOfContents(m_MemberGetter, "");
        }

        [OneTimeTearDown]
        public void Uninit()
        {
        }

        [Test]
        public void ClassWithoutNamespace()
        {
            VerifyDisplayNamesHierarchy("EmptyClass", new[] { "" });
        }

        [Test]
        public void ClassInsideNamespace()
        {
            VerifyDisplayNamesHierarchy("PlayerSettings", new[] { "UnityEditor" });
        }

        [Test]
        public void InnerEnumInsideInnerClassInsideNamespaceAllDocumented()
        {
            VerifyDisplayNamesHierarchy("PSVitaAppCategory", new[] { "PSVita", "PlayerSettings", "UnityEditor" });
        }

        [Test]
        public void InnerEnumInsideInnerClassInsideNamespaceFirstParentUndocumented()
        {
            VerifyDisplayNamesHierarchy("PS4RemotePlayKeyAssignment", new[] { "PS4", "PlayerSettings", "UnityEditor" });
        }

        [Test]
        public void InnerEnumInsideInnerClassInsideNestedNamespace()
        {
            VerifyDisplayNamesHierarchy("AspectMode", new[] {"AspectRatioFitter", "UnityEngine.UI", "UnityEngine"});
        }

        private void VerifyDisplayNamesHierarchy(string leafEntryName, string[] tocEntryDisplayNames)
        {
            TocEntry leafEntry = m_TocRepresentation.Elements.First(m => m.DisplayName == leafEntryName);
            var curEntry = leafEntry;
            var level = 0;
            while (curEntry.Parent != null)
            {
                curEntry = curEntry.Parent;
                var curLevelExpectedDisplayName = tocEntryDisplayNames[level];
                Assert.AreEqual(curLevelExpectedDisplayName, curEntry.DisplayName, "mismatch at ancestor number {0}", level + 1);
                level++;
            }
            Assert.AreEqual(level, tocEntryDisplayNames.Length, "name hierarchy depth differs");
        }
    }
}
