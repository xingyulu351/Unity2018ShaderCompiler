using System;
using System.IO;
using ICSharpCode.SharpZipLib.Core;
using ICSharpCode.SharpZipLib.Zip;
using System.Text;

namespace BindingsToCsAndCpp.TestGenerator
{
    public class ZipUtils
    {
        /// <summary>
        /// Compresses the folder.
        /// </summary>
        /// <returns>The hashes of all files in the zip, suitable for an md5 file.</returns>
        /// <param name="sourcePath">Source path.</param>
        /// <param name="baseSourcePath">Base source path.</param>
        /// <param name="zipName">Zip name.</param>
        /// <param name="targetPath">Target path.</param>
        public static string CompressFolder(string sourcePath, string baseSourcePath, string zipPath)
        {
            FileStream fileOut = File.Create(zipPath);
            ZipOutputStream zipStream = new ZipOutputStream(fileOut);
            zipStream.SetLevel(3);
            int folderOffset = baseSourcePath.Length + (baseSourcePath.EndsWith("\\") ? 0 : 1);
            CompressFolder(sourcePath, zipStream, folderOffset);
            zipStream.Close();
            return zipPath;
        }

        static void CompressFolder(string path, ZipOutputStream zipStream, int folderOffset)
        {
            string[] files = Directory.GetFiles(path);
            foreach (string filename in files)
            {
                FileInfo fi = new FileInfo(filename);

                string entryName = filename.Substring(folderOffset);  // Makes the name in zip based on the folder
                entryName = ZipEntry.CleanName(entryName);  // Removes drive from name and fixes slash direction
                ZipEntry newEntry = new ZipEntry(entryName);

                newEntry.DateTime = fi.LastWriteTime; // Note the zip format stores 2 second granularity

                // Specifying the AESKeySize triggers AES encryption. Allowable values are 0 (off), 128 or 256.
                // A password on the ZipOutputStream is required if using AES.
                //   newEntry.AESKeySize = 256;

                // To permit the zip to be unpacked by built-in extractor in WinXP and Server2003, WinZip 8, Java, and other older code,
                // you need to do one of the following: Specify UseZip64.Off, or set the Size.
                // If the file may be bigger than 4GB, or you do not need WinXP built-in compatibility, you do not need either,
                // but the zip will be in Zip64 format which not all utilities can understand.
                //   zipStream.UseZip64 = UseZip64.Off;
                newEntry.Size = fi.Length;

                zipStream.PutNextEntry(newEntry);

                // Zip the file in buffered chunks
                // the "using" will close the stream even if an exception occurs
                byte[] buffer = new byte[4096];
                using (FileStream streamReader = File.OpenRead(filename))
                {
                    Copy(streamReader, zipStream, buffer);
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
            zipStream.SetLevel(3);

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

                Copy(new MemoryStream(file), zipStream, new byte[4096]);
                zipStream.CloseEntry();
            }
            //zipStream.IsStreamOwner = false;
            zipStream.Close();

            return outputStream.GetBuffer();
        }

        public static string ExtractZipFile(string inputFile, string outputPath)
        {
            ZipFile zf = null;
            FileStream fs = File.OpenRead(inputFile);
            try
            {
                zf = new ZipFile(fs);
                foreach (ZipEntry zipEntry in zf)
                {
                    if (zipEntry.IsDirectory)
                        continue;           // Ignore directories

                    String entryFileName = zipEntry.Name;
                    // to remove the folder from the entry:- entryFileName = Path.GetFileName(entryFileName);
                    // Optionally match entrynames against a selection list here to skip as desired.
                    // The unpacked length is available in the zipEntry.Size property.

                    byte[] buffer = new byte[4096];     // 4K is optimum
                    Stream zipStream = zf.GetInputStream(zipEntry);

                    // Manipulate the output filename here as desired.
                    String fullZipToPath = Path.Combine(outputPath, entryFileName);
                    string directoryName = Path.GetDirectoryName(fullZipToPath);
                    if (directoryName.Length > 0)
                        Directory.CreateDirectory(directoryName);
                    FileInfo targetFile = new FileInfo(fullZipToPath);
                    using (FileStream streamWriter = targetFile.OpenWrite())
                    {
                        Copy(zipStream, streamWriter, buffer);
                    }
                    targetFile.LastWriteTime = zipEntry.DateTime;
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
                    //zf.IsStreamOwner = true; // Makes close also shut the underlying stream
                    fs.Close();
                    zf.Close();  // Ensure we release resources
                }
            }
            return outputPath;
        }

        public static DirectoryInfo GetTempDirectory()
        {
            while (true)
            {
                string temp = Path.GetTempFileName();
                try { File.Delete(temp); } catch {}
                temp = BuildPath(Path.GetDirectoryName(temp), "unity", Path.GetFileNameWithoutExtension(temp));
                if (Directory.Exists(temp))
                    continue;
                return Directory.CreateDirectory(temp);
            }
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

        /// <summary>
        /// Copy the contents of one <see cref="Stream"/> to another. Returns an md5 hash
        /// </summary>
        /// <param name="source">The stream to source data from.</param>
        /// <param name="destination">The stream to write data to.</param>
        /// <param name="buffer">The buffer to use during copying.</param>
        public static void Copy(Stream source, Stream destination, byte[] buffer)
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

            int offset = 0;
            while (copying)
            {
                int bytesRead = source.Read(buffer, 0, buffer.Length);
                if (bytesRead > 0)
                {
                    destination.Write(buffer, 0, bytesRead);
                }
                else
                {
                    destination.Flush();
                    copying = false;
                }
            }
        }
    }
}
