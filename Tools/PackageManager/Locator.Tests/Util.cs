using System;
using System.IO;

namespace Unity.PackageManager
{
    class Util
    {
        internal static DirectoryInfo GetTemporaryTestDirectory()
        {
            string tempPathForTest = Path.GetTempPath();
            string tempDirectoryName = Path.GetRandomFileName();
            DirectoryInfo dir = Directory.CreateDirectory(Path.Combine(tempPathForTest, tempDirectoryName));
            return dir;
        }

        internal static void DeleteTemporaryTestDirectory(DirectoryInfo tempDir)
        {
            tempDir.Delete(true);
        }
    }
}
