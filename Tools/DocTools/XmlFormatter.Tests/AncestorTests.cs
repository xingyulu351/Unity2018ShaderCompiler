using NUnit.Framework;
using UnderlyingModel;
using UnderlyingModel.Tests;
using XMLFormatter;

namespace XmlFormatter.Tests
{
    public partial class TestsWithDataProject
    {
        [Test]
        public void VerifyAncestorsArray()
        {
            var project = TestUtils.CreateNewDataItemProjectFromLegacyDlls();
            var xmlGenerator = new XMLGenerator(project, true, resolveLinksToUndoc: true);

            var ancestors = xmlGenerator.GetAncestors("Array");
            Assert.AreEqual(0, ancestors.Count);
        }

        [Test]
        public void VerifyAncestorsAndroidJavaClass()
        {
            var project = TestUtils.CreateNewDataItemProjectFromLegacyDlls();
            var xmlGenerator = new XMLGenerator(project, true, resolveLinksToUndoc: true);

            var ancestors = xmlGenerator.GetAncestors("AndroidJavaClass");
            Assert.AreEqual(1, ancestors.Count);
            Assert.AreEqual("AndroidJavaObject", ancestors[0].ItemName);
        }

        [Test]
        public void VerifyAncestorsDeep()
        {
            var project = TestUtils.CreateNewDataItemProjectFromLegacyDlls();
            var xmlGenerator = new XMLGenerator(project, true);

            var ancestors = xmlGenerator.GetAncestors("GUIText");
            Assert.AreEqual(4, ancestors.Count);
            Assert.AreEqual("GUIElement", ancestors[0].ItemName);
            Assert.AreEqual("Behaviour", ancestors[1].ItemName);
            Assert.AreEqual("Component", ancestors[2].ItemName);
            Assert.AreEqual("Object", ancestors[3].ItemName);
        }

        [Test]
        public void VerifyAncestorsUnityEvent()
        {
            var project = TestUtils.CreateNewDataItemProjectFromLegacyDlls();
            var xmlGenerator = new XMLGenerator(project, true);

            var ancestors = xmlGenerator.GetAncestors("Events.UnityEvent_1");
            Assert.AreEqual(1, ancestors.Count);
            Assert.AreEqual("Events.UnityEventBase", ancestors[0].ItemName);
        }
    }
}
