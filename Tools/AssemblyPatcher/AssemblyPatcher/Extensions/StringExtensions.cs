using System.IO;

namespace AssemblyPatcher.Extensions
{
    static class StringExtensions
    {
        public static void EnsureDirectoryExists(this string path)
        {
            var targetDir = Path.GetDirectoryName(path);
            if (!Directory.Exists(targetDir))
            {
                Directory.CreateDirectory(targetDir);
            }
        }
    }
}
