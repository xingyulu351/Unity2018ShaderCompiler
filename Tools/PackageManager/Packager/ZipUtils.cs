using System;
using System.IO;
using ICSharpCode.SharpZipLib.Core;
using ICSharpCode.SharpZipLib.Zip;
using System.Text;
using System.Collections.Generic;
#if !WINDOWS
using Mono.Unix;
#endif

namespace Unity.Packager
{
    public class ZipUtils
    {
        static readonly ZipEntryFactory factory = new ZipEntryFactory();

        /// <summary>
        /// Compresses the folder.
        /// </summary>
        /// <returns>The hashes of all files in the zip, suitable for an md5 file.</returns>
        /// <param name="sourcePath">Source path.</param>
        /// <param name="baseSourcePath">Base source path.</param>
        /// <param name="zipPath">Zip path.</param>
        public static string CompressFolder(string sourcePath, string baseSourcePath, string zipPath)
        {
            FileStream fileOut = File.Create(zipPath);
            ZipOutputStream zipStream = new ZipOutputStream(fileOut);
            zipStream.SetLevel(5);
            int folderOffset = baseSourcePath.Length + (baseSourcePath.EndsWith("\\") ? 0 : 1);
            CompressFolder(sourcePath, zipStream, folderOffset);
            zipStream.Close();
            return Utils.CreateMD5(zipPath);
        }

        static void CompressFolder(string path, ZipOutputStream zipStream, int folderOffset)
        {
            string[] files = Directory.GetFiles(path);
            foreach (string filename in files)
            {
                string entryName = filename.Substring(folderOffset);  // Makes the name in zip based on the folder
                entryName = ZipEntry.CleanName(entryName);  // Removes drive from name and fixes slash direction
                var newEntry = CreateZipEntry(filename, entryName);
                zipStream.PutNextEntry(newEntry);

                // Zip the file in buffered chunks
                // the "using" will close the stream even if an exception occurs
                byte[] buffer = new byte[4096];
                using (FileStream streamReader = File.OpenRead(filename))
                {
                    Utils.Copy(streamReader, zipStream, buffer);
                }
                zipStream.CloseEntry();
            }

            string[] folders = Directory.GetDirectories(path);
            foreach (string folder in folders)
            {
                CompressFolder(folder, zipStream, folderOffset);
            }
        }

        public static byte[] Compress(string[] filenames, byte[][] files)
        {
            MemoryStream outputStream = new MemoryStream();
            ZipOutputStream zipStream = new ZipOutputStream(outputStream);
            zipStream.SetLevel(5);

            for (int i = 0; i < filenames.Length; i++)
            {
                string entryName = filenames[i];
                byte[] file = files[i];
                entryName = ZipEntry.CleanName(entryName);  // Removes drive from name and fixes slash direction
                ZipEntry newEntry = new ZipEntry(entryName);
                newEntry.DateTime = DateTime.Now;

                // To permit the zip to be unpacked by built-in extractor in WinXP and Server2003, WinZip 8, Java, and other older code,
                // you need to do one of the following: Specify UseZip64.Off, or set the Size.
                // If the file may be bigger than 4GB, or you do not need WinXP built-in compatibility, you do not need either,
                // but the zip will be in Zip64 format which not all utilities can understand.
                newEntry.Size = file.Length;
                zipStream.PutNextEntry(newEntry);

                Utils.Copy(new MemoryStream(file), zipStream, new byte[4096]);
                zipStream.CloseEntry();
            }
            zipStream.IsStreamOwner = false;
            zipStream.Close();

            return outputStream.GetBuffer();
        }

        public static string ExtractZipFile(string inputFile)
        {
            string outputPath = Utils.GetTempDirectory().FullName;
            ZipFile zf = null;
            try
            {
                FileStream fs = File.OpenRead(inputFile);
                zf = new ZipFile(fs);
                foreach (ZipEntry zipEntry in zf)
                    ExtractZipEntry(zf, zipEntry, outputPath);
            }
            catch (Exception ex)
            {
                Console.WriteLine(ex);
            }
            finally
            {
                if (zf != null)
                {
                    zf.IsStreamOwner = true; // Makes close also shut the underlying stream
                    zf.Close();  // Ensure we release resources
                }
            }
            return outputPath;
        }

        public static string ExtractFile(string zipfile, string file, string outputPath)
        {
            ZipFile zf = null;
            try
            {
                zf = new ZipFile(zipfile);
                var entryName = ZipEntry.CleanName(file);
                var entry = zf.GetEntry(file);
                if (ExtractZipEntry(zf, entry, outputPath))
                    return Path.Combine(outputPath, entryName);
            }
            catch (Exception ex)
            {
                Console.WriteLine(ex);
            }
            finally
            {
                if (zf != null)
                {
                    zf.IsStreamOwner = true; // Makes close also shut the underlying stream
                    zf.Close();  // Ensure we release resources
                }
            }
            return null;
        }

        public static string[] ExtractFiles(string zipfile, string[] filenames)
        {
            List<string> files = new List<string>();
            string outputPath = Utils.GetTempDirectory().FullName;
            ZipFile zf = null;
            try
            {
                zf = new ZipFile(zipfile);
                foreach (var file in filenames)
                {
                    var entryName = ZipEntry.CleanName(file);
                    var entry = zf.GetEntry(file);
                    if (ExtractZipEntry(zf, entry, outputPath))
                        files.Add(Path.Combine(outputPath, entryName));
                }
            }
            catch (Exception ex)
            {
                Console.WriteLine(ex);
            }
            finally
            {
                if (zf != null)
                {
                    zf.IsStreamOwner = true; // Makes close also shut the underlying stream
                    zf.Close();  // Ensure we release resources
                }
            }
            return files.ToArray();
        }

        public static string UpdateZip(string zipfile,  string[] files, string[] names)
        {
            ZipFile zf = null;

            try
            {
                zf = new ZipFile(zipfile);

                zf.BeginUpdate(new CustomStorage(zf, zipfile));

                for (int i = 0; i < files.Length; i++)
                {
                    var entry = zf.GetEntry(names[i]);
                    if (entry != null)
                        zf.Delete(entry);
                    ZipEntry newEntry = CreateZipEntry(files[i], names[i]);
                    zf.Add(files[i], newEntry);
                }
                zf.CommitUpdate();
            }
            catch (Exception ex)
            {
                Console.WriteLine(ex);
            }
            finally
            {
                if (zf != null)
                {
                    zf.IsStreamOwner = true; // Makes close also shut the underlying stream
                    zf.Close();  // Ensure we release resources
                }
            }
            return Utils.CreateMD5(zipfile);
        }

        public static string UpdateZip(string zipfile, string newZipFile, string oldName, string newName, string[] filenames, List<string> names)
        {
            ZipFile zf = null;
            if (newZipFile != null && !Path.IsPathRooted(newZipFile))
                newZipFile = Path.Combine(Path.GetDirectoryName(zipfile), newZipFile);

            try
            {
                //MemoryStream outputStream = new MemoryStream ();
                FileStream outputStream = new FileStream(newZipFile ?? zipfile, FileMode.Create);
                ZipOutputStream zipStream = new ZipOutputStream(outputStream);
                zipStream.SetLevel(5);

                zf = new ZipFile(zipfile);
                foreach (ZipEntry zipEntry in zf)
                {
                    if (zipEntry.IsDirectory)
                        continue;

                    string entryName = zipEntry.Name.Replace(oldName, newName);
                    if (names.Contains(entryName))
                        continue;

                    var newEntry = RenameZipEntry(zipEntry, entryName);

                    zipStream.PutNextEntry(newEntry);

                    Utils.Copy(zf.GetInputStream(zipEntry), zipStream, new byte[4096]);
                    zipStream.CloseEntry();
                }

                for (int i = 0; i < filenames.Length; i++)
                {
                    var entryName = names[i];
                    var filename = filenames[i];
                    entryName = ZipEntry.CleanName(entryName);  // Removes drive from name and fixes slash direction
                    var newEntry = CreateZipEntry(filename, entryName);

                    zipStream.PutNextEntry(newEntry);
                    byte[] buffer = new byte[4096];
                    using (FileStream streamReader = File.OpenRead(filename))
                    {
                        Utils.Copy(streamReader, zipStream, buffer);
                    }
                    zipStream.CloseEntry();
                }

                zipStream.IsStreamOwner = true;
                zipStream.Close();
            }
            catch (Exception ex)
            {
                Console.WriteLine(ex);
                return null;
            }
            finally
            {
                if (zf != null)
                {
                    zf.IsStreamOwner = true; // Makes close also shut the underlying stream
                    zf.Close();  // Ensure we release resources
                }
            }
            return Utils.CreateMD5(newZipFile ?? zipfile);
        }

        static bool ExtractZipEntry(ZipFile zf, ZipEntry zipEntry, string outputPath)
        {
            try
            {
                if (zipEntry.IsDirectory)
                    return false;   // Ignore directories

                String entryFileName = zipEntry.Name;
                // to remove the folder from the entry:- entryFileName = Path.GetFileName(entryFileName);
                // Optionally match entrynames against a selection list here to skip as desired.
                // The unpacked length is available in the zipEntry.Size property.

                // Manipulate the output filename here as desired.
                String fullZipToPath = Path.Combine(outputPath, entryFileName);
                string directoryName = Path.GetDirectoryName(fullZipToPath);
                if (directoryName.Length > 0)
                    Directory.CreateDirectory(directoryName);
                #if !WINDOWS
                // Unzip file in buffered chunks. This is just as fast as unpacking to a buffer the full size
                // of the file, but does not waste memory.
                // The "using" will close the stream even if an exception occurs.
                if (Environment.OSVersion.Platform == PlatformID.Unix || Environment.OSVersion.Platform == PlatformID.MacOSX)
                {
                    if (zipEntry.ExternalFileAttributes > 0)
                    {
                        int fd = Mono.Unix.Native.Syscall.open(fullZipToPath,
                            Mono.Unix.Native.OpenFlags.O_CREAT | Mono.Unix.Native.OpenFlags.O_TRUNC,
                            (Mono.Unix.Native.FilePermissions)zipEntry.ExternalFileAttributes);
                        Mono.Unix.Native.Syscall.close(fd);
                    }
                }
                #endif

                FileInfo targetFile = new FileInfo(fullZipToPath);
                byte[] buffer = new byte[4096]; // 4K is optimum
                Stream zipStream = zf.GetInputStream(zipEntry);
                using (FileStream streamWriter = targetFile.OpenWrite())
                {
                    Utils.Copy(zipStream, streamWriter, buffer);
                }
                targetFile.LastWriteTime = zipEntry.DateTime;
            }
            catch (Exception ex)
            {
                Console.WriteLine(ex);
                return false;
            }
            return true;
        }

        class CustomStorage : DiskArchiveStorage
        {
            readonly string sourceZipFile;
            Stream tmpStream;

            public CustomStorage(ZipFile zipFile, string newZipFile) : base(zipFile)
            {
                sourceZipFile = zipFile.Name;
            }

            public override Stream GetTemporaryOutput()
            {
                if (tmpStream != null)
                    return tmpStream;

                string tempName = Utils.GetTempFilename();
                #if !WINDOWS
                if (Environment.OSVersion.Platform == PlatformID.Unix || Environment.OSVersion.Platform == PlatformID.MacOSX)
                {
                    Mono.Unix.Native.Stat stat;
                    Mono.Unix.Native.Syscall.stat(sourceZipFile, out stat);
                    int fd = Mono.Unix.Native.Syscall.open(tempName,
                        Mono.Unix.Native.OpenFlags.O_CREAT | Mono.Unix.Native.OpenFlags.O_TRUNC,
                        stat.st_mode);
                    Mono.Unix.Native.Syscall.close(fd);
                }
                #endif
                tmpStream = File.Open(tempName, FileMode.OpenOrCreate, FileAccess.Write, FileShare.None);
                return tmpStream;
            }

            public override Stream MakeTemporaryCopy(Stream stream)
            {
                stream.Close();
                if (tmpStream != null)
                    tmpStream.Close();

                string tempName = Utils.GetTempFilename();
                File.Copy(sourceZipFile, tempName, true);

                tmpStream = File.Open(tempName, FileMode.OpenOrCreate, FileAccess.Write, FileShare.None);
                return tmpStream;
            }
        }

        static ZipEntry CopyZipEntry(ZipEntry original, string filename)
        {
            var newEntry = factory.MakeFileEntry(filename, original.Name, true);
            #if !WINDOWS
            Mono.Unix.Native.Stat stat;
            Mono.Unix.Native.Syscall.stat(filename, out stat);
            newEntry.ExternalFileAttributes = (int)stat.st_mode;
            #endif
            return newEntry;
        }

        static ZipEntry RenameZipEntry(ZipEntry original, string newName)
        {
            ZipEntry newEntry = factory.MakeFileEntry(newName, newName, false);
            newEntry.DateTime = original.DateTime;
            newEntry.ExternalFileAttributes = original.ExternalFileAttributes;
            newEntry.ExtraData = original.ExtraData;
            newEntry.Size = original.Size;
            //ZipEntry newEntry = (ZipEntry) original.Clone ();
            //newEntry.Name = newName;
            return newEntry;
        }

        static ZipEntry CreateZipEntry(string filename, string name)
        {
            var newEntry = factory.MakeFileEntry(filename, name, true);
            #if !WINDOWS
            Mono.Unix.Native.Stat stat;
            Mono.Unix.Native.Syscall.stat(filename, out stat);
            newEntry.ExternalFileAttributes = (int)stat.st_mode;
            #endif
            return newEntry;
        }
    }
}
