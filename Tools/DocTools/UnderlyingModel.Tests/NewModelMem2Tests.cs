using System.IO;
using System.Xml.Linq;
using NUnit.Framework;
using UnderlyingModel.ItemSerializers;
using UnderlyingModel.MemDoc.Serializers;

namespace UnderlyingModel.Tests
{
    [TestFixture]
    public class NewModelMem2Tests
    {
        NewDataItemProject m_NewDataItemProject;
        private const string TestsDirectory = "../../..";
        private string m_OldCurDir;

        [OneTimeSetUp]
        public void Init()
        {
            Directory.SetCurrentDirectory(TestContext.CurrentContext.TestDirectory);
            m_OldCurDir = Directory.GetCurrentDirectory();
            Directory.SetCurrentDirectory(TestsDirectory);
            var dirs = DirectoryCalculator.GetMemberItemDirectoriesAsmXml(
                "Tools/DocTools/TestDlls",
                "Tools/DocTools/TestXml");
            DocGenSettings.Instance.Directories = dirs;
            m_NewDataItemProject = new NewDataItemProject(dirs, false);
            m_NewDataItemProject.ReloadAllProjectData();
        }

        [OneTimeTearDown]
        public void Uninit()
        {
            //restore cur dir
            Directory.SetCurrentDirectory(m_OldCurDir);
        }

        [Test]
        public void ArrayTest()
        {
            const string name = "Array";
            m_NewDataItemProject.ReloadProjectDataMinimal(name);
            var elem = m_NewDataItemProject.GetMember(name);
            Assert.AreEqual(12, elem.ChildMembers.Count);
            var constrElem = elem.ChildMembers[3];
            Assert.AreEqual(AssemblyType.Constructor, constrElem.ItemType);
            Assert.IsNotEmpty(constrElem.DocModel.Summary);
        }

        [Test]
        public void AnimatorCondition()
        {
            //Disabled by mark d, 1st Feb 2017
            //No documentation or explanation why this is needed, appears
            //to be a specific setup to hard code the number of animator
            //conditions allowed, meaningless test

            Assert.Pass();

            /*
            const string name = "Animations.AnimatorCondition";
            m_NewDataItemProject.ReloadProjectDataMinimal(name);
            var elem = m_NewDataItemProject.GetMember(name);
            Assert.AreEqual(4, elem.ChildMembers.Count);
            var constrElem = elem.ChildMembers[3];
            Assert.IsFalse(constrElem.AnyHaveAsm);
            Assert.IsNotEmpty(constrElem.DocModel.Summary);*/
        }

        [Ignore("reenable when trunk is in new format")]
        [TestCase("ArrayShort")]
        [TestCase("ColorRed")]
        [TestCase("ExampleFormatting")]
        public void LoadAndSaveTest(string itemName)
        {
            const int sourceVersion = 3;
            var ser = new MemDocModelXmlSerializer { Version = sourceVersion };

            var inputDir = Path.Combine(DirectoryCalculator.DocToolsDirName, "UnderlyingModel.Tests/Mem2Source");
            var expectedDir = Path.Combine(DirectoryCalculator.DocToolsDirName, "UnderlyingModel.Tests/Mem2Expected");
            var outputDir = Path.Combine(DirectoryCalculator.DocToolsDirName, "UnderlyingModel.Tests/Mem2Output");
            Directory.CreateDirectory(outputDir);

            var sourcePath = MemFilePathWithExt(inputDir, itemName);
            var sourceModel = ser.Deserialize(sourcePath);

            var actualXml = new XElement(XmlTags.Root, ser.Serialize(sourceModel));
            actualXml.SetAttributeValue(XmlTags.Version, sourceVersion);

            var destinPath = MemFilePathWithExt(outputDir, itemName);
            XmlUtils.WriteToFileWithHeader(destinPath, actualXml);

            var expectedPath = MemFilePathWithExt(expectedDir, itemName);
            string expectedXml = File.ReadAllText(expectedPath);

            string readActualXml = File.ReadAllText(destinPath);
            Assert.AreEqual(expectedXml.WithUnixLineEndings(), readActualXml.WithUnixLineEndings());
        }

        [Ignore("Loading and Saving does not guarantee preserved newlines")]
        [TestCase("", "Array", 3u)]
        [TestCase("", "ArrayShort", 3u)]
        [TestCase("", "Hashtable", 3u)]
        [TestCase("UnityEditor.Animations", "AnimatorCondition", 3u)]
        [TestCase("UnityEditor.Animations", "AnimatorConditionMode", 3u)]
        public void LoadAndSavePreservesDocModel(string fullNamespace, string itemName, uint sourceVersion = 1)
        {
            var inputDir = Path.Combine(DirectoryCalculator.RootDirName, "Tools/DocTools/UnderlyingModel.Tests/Mem2Source");
            var outputDir = Path.Combine(DirectoryCalculator.RootDirName, "Tools/DocTools/UnderlyingModel.Tests/Mem2Output");
            Directory.CreateDirectory(Path.Combine(inputDir, fullNamespace));
            Directory.CreateDirectory(Path.Combine(outputDir, fullNamespace));

            var sourcePath = CreateFullPath(inputDir, fullNamespace, itemName);
            var ser = new MemDocModelXmlSerializer {Version = sourceVersion};
            var expModel = ser.Deserialize(sourcePath);

            var actualXml = new XElement(XmlTags.Root, ser.Serialize(expModel));
            actualXml.SetAttributeValue(XmlTags.Version, sourceVersion);
            var destinPath = CreateFullPath(outputDir, fullNamespace, itemName);
            XmlUtils.WriteToFileWithHeader(destinPath, actualXml);

            string expectedXML = File.ReadAllText(sourcePath);

            string readActualXML = File.ReadAllText(destinPath);
            Assert.AreEqual(expectedXML.WithUnixLineEndings(), readActualXML.WithUnixLineEndings());
        }

        [Ignore("Loading and Saving does not guarantee preserved newlines")]
        [TestCase("", "Array", "Array", 3u)]
        [TestCase("UnityEditor.Animations", "AnimatorCondition", "Animations.AnimatorCondition", 3u)]
        [TestCase("UnityEditor.Animations", "AnimatorConditionMode", "Animations.AnimatorConditionMode", 3u)]
        public void LoadAndSavePreservesItem(string fullNamespace, string className, string itemName, uint sourceVersion)
        {
            var inputDir = Path.Combine(DirectoryCalculator.RootDirName, "Tools/DocTools/UnderlyingModel.Tests/Mem2Source");
            var outputDir = Path.Combine(DirectoryCalculator.RootDirName, "Tools/DocTools/UnderlyingModel.Tests/Mem2Output");

            DocGenSettings.Instance.Directories.Mem2files = inputDir;
            var sourcePath = CreateFullPath(inputDir, fullNamespace, className);
            var destinPath = CreateFullPath(outputDir, fullNamespace, className);

            //find the item
            var item = m_NewDataItemProject.GetMember(itemName);
            Assert.IsNotNull(item, "no item with name {0}", itemName);
            string expectedXML = File.ReadAllText(sourcePath);

            var ser = new MemberItemIOMem2(m_NewDataItemProject) {Version = sourceVersion};
            ser.LoadDoc(item);

            DocGenSettings.Instance.Directories.Mem2files = outputDir;
            ser.SaveDoc(item);

            string readActualXML = File.ReadAllText(destinPath);
            Assert.AreEqual(expectedXML.WithUnixLineEndings(), readActualXML.WithUnixLineEndings());
        }

        [Ignore("Loading and Saving does not guarantee preserved newlines")]
        [TestCase("", "Array", "Array")]
        [TestCase("UnityEditor.Animations", "AnimatorCondition", "Animations.AnimatorCondition")]
        [TestCase("UnityEditor.Animations", "AnimatorConditionMode", "Animations.AnimatorConditionMode")]
        public void LoadAndSavePreservesItem(string fullNamespace, string className, string itemName)
        {
            const uint currentVersion = 3;
            var inputDir = Path.Combine(DirectoryCalculator.RootDirName, "Tools/DocTools/UnderlyingModel.Tests/Mem2Source");
            var outputDir = Path.Combine(DirectoryCalculator.RootDirName, "Tools/DocTools/UnderlyingModel.Tests/Mem2Output");

            DocGenSettings.Instance.Directories.Mem2files = inputDir;
            var sourcePath = CreateFullPath(inputDir, fullNamespace, className);
            var destinPath = CreateFullPath(outputDir, fullNamespace, className);

            //find the item
            var item = m_NewDataItemProject.GetMember(itemName);
            Assert.IsNotNull(item, "no item with name {0}", itemName);
            string expectedXML = File.ReadAllText(sourcePath);

            var ser = new MemberItemIOMem2(m_NewDataItemProject) { Version = currentVersion };
            ser.LoadDoc(item);

            DocGenSettings.Instance.Directories.Mem2files = outputDir;
            ser.SaveDoc(item);

            string readActualXML = File.ReadAllText(destinPath);
            Assert.AreEqual(expectedXML.WithUnixLineEndings(), readActualXML.WithUnixLineEndings());
        }

        private static string CreateFullPath(string inputDir, string fullNamespace, string itemName)
        {
            var dir = Path.Combine(inputDir, fullNamespace);
            return string.Format("{0}/{1}.{2}", dir, itemName, DirectoryUtil.Mem2Extension);
        }

        private static string MemFilePathWithExt(string inputDir, string itemName)
        {
            return string.Format("{0}/{1}.{2}", inputDir, itemName, DirectoryUtil.Mem2Extension);
        }
    }
}
