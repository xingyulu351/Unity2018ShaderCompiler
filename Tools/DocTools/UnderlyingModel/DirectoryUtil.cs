using System;
using System.IO;
using System.Linq;


namespace UnderlyingModel
{
    static public class DirectoryUtil
    {
        public const string MemExtensionMask = "*.mem";
        public const string MemExtension = "mem";
        public const string Mem2ExtensionMask = "*.mem.xml";
        public const string Mem2Extension = "mem.xml";

        public static bool TryGetAllMemFiles(out string[] memFiles, string extDirectory)
        {
            memFiles = null;

            try
            {
                memFiles = Directory.GetFiles(extDirectory, MemExtensionMask).ToArray();
            }
            catch (DirectoryNotFoundException d)
            {
                Console.WriteLine(d);
                return false;
            }

            return memFiles.Length > 0;
        }

        public static void CreateDirectoryIfNeeded(string outputFolder)
        {
            try
            {
                if (!Directory.Exists(outputFolder))
                    Directory.CreateDirectory(outputFolder);
            }
            catch (DirectoryNotFoundException)
            {
                Console.Error.WriteLine("could not create directory {0}", outputFolder);
            }
        }

        public static void DeleteAllFiles(string dir)
        {
            if (!Directory.Exists(dir))
                return;
            foreach (var f in Directory.GetFiles(dir))
                File.Delete(f);
        }

        public static void CopyDirectory(string sourcePath, string destPath)
        {
            CreateDirectoryIfNeeded(destPath);

            try
            {
                foreach (var file in Directory.GetFiles(sourcePath))
                {
                    var dest = Path.Combine(destPath, Path.GetFileName(file));
                    File.Copy(file, dest);
                }

                foreach (var folder in Directory.GetDirectories(sourcePath))
                {
                    var dest = Path.Combine(destPath, Path.GetFileName(folder));
                    CopyDirectory(folder, dest);
                }
            }
            catch (DirectoryNotFoundException dir)
            {
                Console.WriteLine(dir.Message);
                throw dir;
            }
        }

        public static void CopyDirectoryFromScratch(string fullPathOrig, string fullPathDestin)
        {
            DeleteAllFiles(fullPathDestin);
            if (Directory.Exists(fullPathDestin))
                Directory.Delete(fullPathDestin, true);
            CreateDirectoryIfNeeded(fullPathDestin);
            CopyDirectory(fullPathOrig, fullPathDestin);
        }
    }
}
