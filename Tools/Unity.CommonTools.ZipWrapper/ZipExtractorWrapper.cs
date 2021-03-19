using System.IO;
using System.Linq;
using Ionic.Zip;

namespace Unity.CommonTools.ZipWrapper
{
    public class ZipExtractorWrapper : IZipExtractor
    {
        public bool IsZipFile(string fileName)
        {
            return ZipFile.IsZipFile(fileName);
        }

        public void ExtractAll(string zipPath, string targetPath)
        {
            ExtractAll(zipPath, targetPath, false);
        }

        /// <remarks>
        /// This is a workarround. zipFile.ExtractAll(...) and zipEntry.Extract(filepath) does not
        /// work on mac since it internally replaces the output path slashes (/) with
        /// backslashes (\) resulting in an illegal output path.
        ///
        /// This methods acts as a workarround to that and handles all the filepath handling
        /// itself and uses the Extract to Stream feature instead.
        /// </remarks>
        public void ExtractAll(string zipPath, string targetPath, bool overwriteExistingFiles)
        {
            using (var zipFile = new ZipFile(zipPath))
            {
                foreach (var zipEntry in zipFile.Entries)
                {
                    ExtractEntry(zipEntry, targetPath, overwriteExistingFiles);
                }
            }
        }

        public void ExtractSingle(string zipPath, string targetPath, string fileName, bool overwriteExistingFiles)
        {
            using (var zipFile = new ZipFile(zipPath))
            {
                var zipEntry = zipFile.Entries.First(entry => entry.FileName == fileName);
                ExtractEntry(zipEntry, targetPath, overwriteExistingFiles);
            }
        }

        private void ExtractEntry(ZipEntry zipEntry, string targetPath, bool overwriteExistingFiles)
        {
            var entryFilePath = DetermineEntryFilePath(zipEntry);
            var targetFileName = Path.Combine(targetPath, entryFilePath);
            if (zipEntry.IsDirectory)
            {
                EnsureDirectoryExists(targetFileName);
            }
            else
            {
                if (overwriteExistingFiles == false && File.Exists(targetFileName))
                {
                    throw new ZipException(string.Format("The file {0} already exists.", targetFileName));
                }

                var entityDirectoryPath = Path.GetDirectoryName(targetFileName);
                EnsureDirectoryExists(entityDirectoryPath);

                using (var outputStream = (Stream) new FileStream(targetFileName, FileMode.CreateNew))
                {
                    zipEntry.Extract(outputStream);
                }
            }
        }

        private static void EnsureDirectoryExists(string targetFileName)
        {
            if (!Directory.Exists(targetFileName))
                Directory.CreateDirectory(targetFileName);
        }

        private string DetermineEntryFilePath(ZipEntry entry)
        {
            string filePath = entry.FileName;

            if (filePath.IndexOf(':') == 1)
            {
                filePath = filePath.Substring(2);
            }

            if (filePath.StartsWith("/"))
            {
                filePath = filePath.Substring(1);
            }

            return filePath.Replace('/', Path.DirectorySeparatorChar);
        }
    }

    public interface IZipExtractor
    {
        bool IsZipFile(string fileName);
        void ExtractAll(string zipPath, string targetPath);
        void ExtractAll(string zipPath, string targetPath, bool overwriteExistingFiles);
    }
}
