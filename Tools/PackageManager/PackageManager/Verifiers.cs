#pragma warning disable 618

using System;
using System.IO;
using System.Security.Cryptography;
using System.Text;
using Unity.PackageManager.Ivy;
using ICSharpCode.SharpZipLib.Zip;

namespace Unity.PackageManager
{
    public class Verifier : Task
    {
        public virtual bool Verify()
        {
            return true;
        }

        protected override bool TaskRunning()
        {
            if (!base.TaskRunning())
                return false;

            return Verify();
        }
    }

    public class IvyVerifier : Verifier
    {
        string xml;

        public IvyVerifier(Uri path)
        {
            if (File.Exists(path.LocalPath))
                xml = File.ReadAllText(path.LocalPath, Encoding.UTF8);
        }

        public IvyVerifier(byte[] bytes)
        {
            xml = Encoding.UTF8.GetString(bytes);
        }

        public IvyVerifier(string xml)
        {
            this.xml = xml;
        }

        public override bool Verify()
        {
            if (xml == null)
                return false;
            try
            {
                Result = IvyParser.Parse<object>(xml);
                if (IvyParser.HasErrors)
                    return false;
            }
            catch (Exception ex)
            {
                Console.WriteLine(ex);
                return false;
            }
            return true;
        }
    }

    public class BinaryVerifier : Verifier
    {
        private readonly Uri localPath;
        private readonly IvyArtifact artifact;

        public BinaryVerifier(Uri localPath, IvyArtifact artifact)
        {
            this.localPath = localPath;
            this.artifact = artifact;
            ProgressMessage = "Verifying";
        }

        public override bool Verify()
        {
            string path = Path.Combine(localPath.LocalPath, artifact.Filename);
            if (!File.Exists(path))
            {
                Console.WriteLine("Verification failure: local path '{0}' does not exist", path);
                return false;
            }

            string hashFile = Path.Combine(localPath.LocalPath, artifact.MD5Filename);
            if (!File.Exists(hashFile))
            {
                Console.WriteLine("Verification failure: local checksum file '{0}' does not exist", hashFile);
                return false;
            }

            string[] hashContent = File.ReadAllText(hashFile).Split(new[] {' '}, StringSplitOptions.RemoveEmptyEntries);
            if (hashContent.Length != 2)
            {
                Console.WriteLine("Verification failure: checksum file has {0} tokens, expected 2", hashContent.Length);
                return false;
            }
            string reportedHash = hashContent[0];

            MD5 md5 = MD5.Create();
            byte[] hash = null;
            using (FileStream fs = File.OpenRead(path))
                hash = md5.ComputeHash(fs);

            string strHash = "";
            foreach (byte b in hash)
                strHash += b.ToString("x2");

            if (!reportedHash.Equals(strHash))
            {
                Console.WriteLine("Verification failure: remote checksum {0} does not match local checksum {1}", reportedHash, strHash);
                return false;
            }
            return true;
        }
    }

    public class ZipVerifier : Verifier
    {
        readonly string file;
        readonly string localPath;

        public ZipVerifier(string zipFile, string extractedPath)
        {
            file = zipFile;
            localPath = extractedPath;
            Name = "Unzip Verifier Task";
            ProgressMessage = "Verifying";
        }

        public override bool Verify()
        {
            ZipFile zf = null;
            try
            {
                FileStream fs = File.OpenRead(file);
                zf = new ZipFile(fs);
                int processed = 0;
                string basePath = null;
                foreach (ZipEntry zipEntry in zf)
                {
                    if (zipEntry.IsDirectory)
                        continue;           // Ignore directories

                    var entryName = zipEntry.Name;
                    if (entryName.EndsWith("ivy.xml"))
                    {
                        entryName = entryName.Replace("ivy.xml", "ivy-waiting-for-unzip-to-end");
                        basePath = Path.Combine(localPath, Path.GetDirectoryName(entryName));
                    }

                    string fullPath = Path.Combine(localPath, entryName);

                    // Check existence
                    if (!File.Exists(fullPath))
                    {
                        Console.WriteLine("ZipVerifier: file doesn't exist: {0}", fullPath);
                        return false;
                    }
#if !WINDOWS
                    if (Environment.OSVersion.Platform == PlatformID.Unix || Environment.OSVersion.Platform == PlatformID.MacOSX)
                    {
                        if (zipEntry.ExternalFileAttributes != 0)
                        {
                            // Check permissions
                            Mono.Unix.Native.Stat stat;
                            if (0 != Mono.Unix.Native.Syscall.stat(fullPath, out stat))
                            {
                                Console.WriteLine("ZipVerifier: couldn't stat {0}", fullPath);
                                return false;
                            }
                            if (zipEntry.ExternalFileAttributes != (zipEntry.ExternalFileAttributes & (int)stat.st_mode))
                            {
                                Console.WriteLine("ZipVerifier: permissions don't match: {0} vs {1}", zipEntry.ExternalFileAttributes, stat.st_mode);
                                return false;
                            }
                        }
                    }
#endif
                    UpdateProgress((float)++processed / zf.Size);
                }

                var ivy = Path.Combine(basePath, "ivy-waiting-for-unzip-to-end");
                if (!File.Exists(ivy))
                {
                    Console.WriteLine("ZipVerifier: Missing package description for {0}", basePath);
                    return false;
                }

                File.Move(ivy, Path.Combine(basePath, "ivy.xml"));
            }
            catch
            {
                throw;
            }
            finally
            {
                if (zf != null)
                {
                    //zf.IsStreamOwner = true; // Makes close also shut the underlying stream
                    zf.Close();  // Ensure we release resources
                }
            }

            return true;
        }
    }
}
