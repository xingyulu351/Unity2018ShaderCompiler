using System;
using System.IO;
using System.Security.Cryptography;
using Unity.DataContract;
using Unity.PackageManager.Ivy;
using System.Collections.Generic;

namespace Unity.Packager
{
    public static class Utils
    {
        public static DirectoryInfo GetTempDirectory()
        {
            while (true)
            {
                string temp = GetTempFilename();
                temp = BuildPath(Path.GetDirectoryName(temp), "unity-" + Environment.UserName, Path.GetFileNameWithoutExtension(temp));
                if (Directory.Exists(temp))
                    continue;
                return Directory.CreateDirectory(temp);
            }
        }

        public static string GetTempFilename()
        {
            string tmpDir = Path.GetTempPath();
            return GetTempFilename(tmpDir);
        }

        public static string GetTempFilename(DirectoryInfo tmpDir)
        {
            return GetTempFilename(tmpDir.FullName);
        }

        public static string GetTempFilename(string tmpDir)
        {
            string filename = null;
            Random rnd = new Random();
            do
            {
                int num = rnd.Next();
                num++;
                filename = Path.Combine(tmpDir, "tmp" + num.ToString("x") + ".tmp");
            }
            while (File.Exists(filename));
            return filename;
        }

        public static string BuildPath(params string[] parts)
        {
            string path = "";
            for (int i = parts.Length - 1; i >= 0; i--)
            {
                if (!String.IsNullOrEmpty(parts[i]))
                    path = Path.Combine(parts[i], path);
            }
            return path;
        }

        public static DirectoryInfo CreatePackageContentsTree(string basePath, string org, string name, string version)
        {
            string temp = BuildPath(basePath, org, name, version);
            DirectoryInfo di = Directory.CreateDirectory(temp);
            return di;
        }

        public static void CopyFilesRecurse(string from, DirectoryInfo to)
        {
            CopyFilesRecurse(new DirectoryInfo(from), to);
        }

        public static void CopyFilesRecurse(DirectoryInfo from, DirectoryInfo to)
        {
            foreach (FileInfo entry in from.GetFiles())
            {
                entry.CopyTo(Path.Combine(to.FullName, entry.Name));
            }

            foreach (DirectoryInfo entry in from.GetDirectories())
            {
                DirectoryInfo subdir = to.CreateSubdirectory(entry.Name);
                CopyFilesRecurse(entry, subdir);
            }
        }

        /// <summary>
        /// Zips the contents of a package and creates an ivy file pointing to the zip file, for the repo index. Also creates an md5 hash file.
        /// </summary>
        /// <returns>Ivy file path</returns>
        /// <param name="module"></param>
        /// <param name="packageContentsPath"></param>
        /// <param name="basePath">The path where packageContentsPath was created in, so that the zip file will contain paths relative to that</param>
        /// <param name="outputPath">Where to put the zip, xml and md5 files</param>
        public static string CreatePackage(IvyModule module, DirectoryInfo packageContentsPath, DirectoryInfo basePath, DirectoryInfo outputPath, string outputFilename)
        {
            string zipPath = Path.Combine(outputPath.FullName, outputFilename);

            // compress package
            string hash = ZipUtils.CompressFolder(packageContentsPath.FullName, basePath.FullName, zipPath);

            var notes = module.GetArtifact(ArtifactType.ReleaseNotes);
            module.Artifacts.Clear();

            // update module with zip artifact
            module.Artifacts.Add(new IvyArtifact() {
                Name = Path.GetFileNameWithoutExtension(outputFilename),
                Extension = "zip",
                Type = ArtifactType.Package
            });

            if (notes != null)
            {
                string outputNotes = Utils.BuildPath(outputPath.FullName, Path.GetFileNameWithoutExtension(outputFilename) + ".notes");
                if (File.Exists(outputNotes))
                    File.Delete(outputNotes);
                File.Copy(Utils.BuildPath(packageContentsPath.FullName, notes.Filename), outputNotes, true);
                module.Artifacts.Add(new IvyArtifact() {
                    Name = Path.GetFileNameWithoutExtension(outputFilename),
                    Extension = "notes",
                    Type = ArtifactType.ReleaseNotes
                });
            }

            string filePathNoExt = Path.Combine(outputPath.FullName, Path.GetFileNameWithoutExtension(outputFilename));

            File.WriteAllText(filePathNoExt + ".md5", hash);

            // create ivy file for zip
            return module.WriteIvyFile(outputPath.FullName, Path.GetFileNameWithoutExtension(outputFilename) + ".xml");
        }

        public static void CreateMD5(string inputPath, string outputPath)
        {
            string strHash = CreateMD5(inputPath);
            File.WriteAllText(outputPath, strHash);
        }

        public static string CreateMD5(string inputPath)
        {
            // create md5 hash
            MD5 md5 = MD5.Create();
            byte[] hash = null;
            using (FileStream fs = File.OpenRead(inputPath))
                hash = md5.ComputeHash(fs);

            string strHash = "";
            foreach (byte b in hash)
                strHash += b.ToString("x2");

            strHash += " " + Path.GetFileName(inputPath);
            return strHash;
        }

        public static string CreateMD5(byte[] bytes, string filename)
        {
            // create md5 hash
            MD5 md5 = MD5.Create();
            byte[] hash = md5.ComputeHash(bytes);

            string strHash = "";
            foreach (byte b in hash)
                strHash += b.ToString("x2");

            strHash += " " + Path.GetFileName(filename);
            return strHash;
        }

        /// <summary>
        /// Copy the contents of one <see cref="Stream"/> to another. Returns an md5 hash
        /// </summary>
        /// <param name="source">The stream to source data from.</param>
        /// <param name="destination">The stream to write data to.</param>
        /// <param name="buffer">The buffer to use during copying.</param>
        public static byte[] Copy(Stream source, Stream destination, byte[] buffer)
        {
            if (source == null)
                throw new ArgumentNullException("source");

            if (destination == null)
                throw new ArgumentNullException("destination");

            if (buffer == null)
                throw new ArgumentNullException("buffer");

            // Ensure a reasonable size of buffer is used without being prohibitive.
            if (buffer.Length < 128)
                throw new ArgumentException("Buffer is too small", "buffer");

            bool copying = true;

            MD5 md5 = MD5.Create();
            int offset = 0;
            while (copying)
            {
                int bytesRead = source.Read(buffer, 0, buffer.Length);
                if (bytesRead > 0)
                {
                    destination.Write(buffer, 0, bytesRead);
                    offset += md5.TransformBlock(buffer, 0, buffer.Length, null, 0);
                }
                else
                {
                    destination.Flush();
                    copying = false;
                    md5.TransformFinalBlock(buffer, 0, 0);
                }
            }

            return md5.Hash;
        }

        // We don't want to expose local base paths in certain instances
        public static void ClearIvyFileInformation(IEnumerable<IvyModule> modules)
        {
            foreach (var module in modules)
            {
                module.BasePath = null;
                module.IvyFile = null;
            }
        }

        public static Uri ValidateDir(string path)
        {
            return new Uri("file://" + Path.GetFullPath(path));
        }
    }
}
