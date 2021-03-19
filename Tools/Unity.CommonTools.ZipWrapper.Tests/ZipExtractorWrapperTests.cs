using System;
using System.IO;
using System.Linq;
using NUnit.Framework;

namespace Unity.CommonTools.ZipWrapper.Tests
{
    public class ZipExtractorWrapperTests
    {
        private string m_TemporaryDirectory;
        private ZipExtractorWrapper m_ZipExtractor;

        [SetUp]
        public void Setup()
        {
            m_TemporaryDirectory = Paths.Combine(Files.GetTempPath(), string.Format("ZipExtractorWrapperTests-{0}", Guid.NewGuid()));
            Directory.CreateDirectory(m_TemporaryDirectory);

            m_ZipExtractor = new ZipExtractorWrapper();
        }

        [TearDown]
        public void TearDown()
        {
            if (Directory.Exists(m_TemporaryDirectory))
            {
                Files.DeleteFilesAndDirectoriesUnder(m_TemporaryDirectory);
                Directory.Delete(m_TemporaryDirectory);
            }
        }

        [Test]
        public void IsZipFile_WhenValidZipFileIsGiven_ThenTrueIsReturned()
        {
            var filepath = SetupTestFile("ZipExtractorWrapper.zip");

            var isZipFile = m_ZipExtractor.IsZipFile(filepath);

            Assert.That(isZipFile, Is.True);
        }

        [Test]
        public void IsZipFile_WhenInvalidZipFileIsGiven_ThenFalseIsReturned()
        {
            var filepath = Path.Combine(m_TemporaryDirectory, "invalidZipFile.zip");
            File.WriteAllBytes(filepath, new byte[] { 0, 1, 2, 3 });

            var isZipFile = m_ZipExtractor.IsZipFile(filepath);

            Assert.That(isZipFile, Is.False);
        }

        [Test]
        public void IsZipFile_WhenFileDoesNotExists_ThenFalseIsReturned()
        {
            var filepath = Path.Combine(m_TemporaryDirectory, "non-existing-filename.zip");

            var isZipFile = m_ZipExtractor.IsZipFile(filepath);

            Assert.That(isZipFile, Is.False);
        }

        [Test]
        public void ExtractAll_WhenExtractingZipFile_ThenZipFileIsExtracted()
        {
            var filepath = SetupTestFile("ZipExtractorWrapper.zip");

            m_ZipExtractor.ExtractAll(filepath, m_TemporaryDirectory);

            var expectedEmptyFilepath = Path.Combine(m_TemporaryDirectory, "empty-file.txt");
            Assert.That(File.Exists(expectedEmptyFilepath), Is.True);
            Assert.That(File.ReadAllText(expectedEmptyFilepath), Is.Empty);
            var expectedSlashEncodedEntryFilepath = Path.Combine(m_TemporaryDirectory, "slash-encoded-entry.txt");
            Assert.That(File.Exists(expectedSlashEncodedEntryFilepath), Is.True);
            Assert.That(File.ReadAllText(expectedSlashEncodedEntryFilepath), Is.EqualTo("slash-encoded-entry content"));
            var expectedDriveEncodedEntryFilepath = Path.Combine(m_TemporaryDirectory, "drive-encoded-entry.txt");
            Assert.That(File.Exists(expectedDriveEncodedEntryFilepath), Is.True);
            Assert.That(File.ReadAllText(expectedDriveEncodedEntryFilepath), Is.EqualTo("drive-encoded-entry content"));
            var expectedFileInFolderFilepath = Path.Combine(m_TemporaryDirectory, Path.Combine("folder", "file-in-folder.txt"));
            Assert.That(File.Exists(expectedFileInFolderFilepath), Is.True);
            Assert.That(File.ReadAllText(expectedFileInFolderFilepath), Is.EqualTo("file-in-folder content"));
            var expectedEmptyFolderPath = Path.Combine(m_TemporaryDirectory, "empty-folder");
            Assert.That(Directory.Exists(expectedEmptyFolderPath), Is.True);
        }

        [Test]
        public void ExtractSingle_WhenExtractingSingleFile_ThenOnlySingleFileIsExtracted()
        {
            var filepath = SetupTestFile("ZipExtractorWrapper.zip");

            m_ZipExtractor.ExtractSingle(filepath, m_TemporaryDirectory, "folder/file-in-folder.txt", overwriteExistingFiles: false);

            var expectedFileInFolderFilepath = Path.Combine(m_TemporaryDirectory, Path.Combine("folder", "file-in-folder.txt"));
            Assert.That(File.Exists(expectedFileInFolderFilepath), Is.True);
            Assert.That(File.ReadAllText(expectedFileInFolderFilepath), Is.EqualTo("file-in-folder content"));
            Assert.That(Directory.GetDirectories(m_TemporaryDirectory).Single(), Does.EndWith("folder"));
            Assert.That(Directory.GetFiles(Path.Combine(m_TemporaryDirectory, "folder")).Single(), Does.EndWith("file-in-folder.txt"));
        }

        private string SetupTestFile(string filename)
        {
            var assemblyResourceName = string.Format("Unity.CommonTools.ZipWrapper.Tests.ZipExtractorWrapperTestFiles.{0}", filename);
            var buildArtifactsFile = Path.Combine(m_TemporaryDirectory, filename);

            var assembly = GetType().Assembly;
            using (var sourceZipStream = assembly.GetManifestResourceStream(assemblyResourceName))
            {
                Assert.That(sourceZipStream, Is.Not.Null, () => string.Format("Could not find embedded assembly resource `{0}`\n Embedded resources in assembly `{1}` are:\n\t {2}",
                    assemblyResourceName, assembly.GetName().Name, string.Join("\n\t", assembly.GetManifestResourceNames())));

                using (var outputStream = File.Create(buildArtifactsFile))
                {
                    sourceZipStream.CopyTo(outputStream);
                }
            }

            return buildArtifactsFile;
        }
    }
}
