using System;
using System.Collections.Generic;
using System.IO;
using System.Linq;
using System.Xml;
using MemSyntax;
using NUnit.Framework;

namespace APIDocumentationGenerator.Tests
{
    [TestFixture]
    public class SearchTests
    {
        private string m_OldCurDir;
        [OneTimeSetUp]
        public void Init()
        {
            m_OldCurDir = Directory.GetCurrentDirectory();
            try
            {
                Directory.SetCurrentDirectory("../Tests/XMLforSearch");
            }
            catch (Exception)
            {
                Console.WriteLine("unable to set current directory");
            }
        }

        [OneTimeTearDown]
        public void Uninit()
        {
            //restore cur dir
            Directory.SetCurrentDirectory(m_OldCurDir);
        }

        [TestCase("ADError._code", "aderror code error")]
        [TestCase("ADErrorCode", "aderrorcode error enumeration identify advertisement")]
        [TestCase("Vector3._x", "vector3 x component vector")]
        [TestCase("Vector3.Min", "vector3 min vector made smallest components vectors")]
        [TestCase("Bounds.SetMinMax", "bounds setminmax sets min max value box separately using faster than assigning")]
        [TestCase("Vector3.Project", "project projects projection blahblah onnormal blahblah2 vector vector3 onto another")]
        [TestCase("Vector3-magnitude", "vector vectors vector3 using magnitude magnitudes length read square squared root need compare computing some them faster")]
        [TestCase("AssetDatabase.LoadAssetAtPath", "assetdatabase loadassetatpath")]
        public void SearchWordsFromXML(string xmlFile, string expectedList)
        {
            var fileName = xmlFile + ".xml";
            if (!File.Exists(fileName))
            {
                fileName = Path.Combine(TestContext.CurrentContext.TestDirectory, @"..\Tests\XMLforSearch", xmlFile) + ".xml";
            }
            if (!File.Exists(fileName))
            {
                throw new FileNotFoundException("Couldn't find input test xml for " + xmlFile);
            }
            var xmlDoc = new XmlDocument();
            var reader = new XmlTextReader(fileName);
            try
            {
                xmlDoc.Load(reader);
            }
            catch (Exception)
            {
                Assert.Fail("could not load xml from {0}", reader.BaseURI);
            }
            var search = new SearchWordsXmlDocument(xmlDoc);
            var actualWords = search.GetSearchWordsList().ToList();
            var expectedWords = expectedList.Split(' ').ToList();
            actualWords.Sort();
            expectedWords.Sort();
            Assert.AreEqual(expectedWords, actualWords);
        }

        [TestCase("Backslash '\\'", "Backslash '\\\\'")] //one isolated backslash
        [TestCase("foo'\\'bar'\\'baz", "foo'\\\\'bar'\\\\'baz")] //two isolated backslashes
        [TestCase("foo'\\\\'bar'\\\\'baz", "foo'\\\\'bar'\\\\'baz")] //two double backslashes
        [TestCase("trailing\\", "trailing\\\\")] //trailing single backslash
        [TestCase("trailing\\\\", "trailing\\\\")] //trailing double backslash
        [TestCase("trailing\\\"", "trailing\\\"")] //trailing escaped quote
        [TestCase("\\\"leading", "\\\"leading")] //leading escaped quote
        [TestCase("\\\\leading", "\\\\leading")] //leading double backslash
        [TestCase("\\leading", "\\\\leading")] //leading single backslash
        [TestCase("\"leading", "\"leading")] //leading single quote
        public void BackslashEscaping(string source, string expected)
        {
            var actual = SearchIndexJsonOutput.ReplaceNonEscapeBackSlashes(source);
            Assert.AreEqual(expected, actual);
        }
    }
}
