#pragma warning disable 618

using System;
using System.IO;
using Unity.PackageManager.Ivy;
using ICSharpCode.SharpZipLib.Zip;

namespace Unity.PackageManager
{
    internal class UnzipperTask : Task
    {
        private readonly Uri basePath;
        private readonly IvyArtifact artifact;

        public UnzipperTask(Uri basePath, IvyArtifact artifact)
        {
            Name = "Unzipper Task";
            ProgressMessage = "Installing";
            this.basePath = basePath;
            this.artifact = artifact;
        }

        protected override bool TaskRunning()
        {
            ExtractZipFile(Path.Combine(basePath.LocalPath, artifact.Filename), Settings.installLocation);
            return base.TaskRunning();
        }

        void ExtractZipFile(string archive, string outFolder)
        {
            ZipFile zf = null;
            EstimatedDuration = 1L;
            DateTime startTime = DateTime.Now;
            int processed = 0;
            long totalBytes = 0L;

            try
            {
                FileStream fs = File.OpenRead(archive);
                zf = new ZipFile(fs);

                foreach (ZipEntry zipEntry in zf)
                {
                    if (zipEntry.IsDirectory)
                        continue;           // Ignore directories

                    String entryFileName = zipEntry.Name;
                    // to remove the folder from the entry:- entryFileName = Path.GetFileName(entryFileName);
                    // Optionally match entrynames against a selection list here to skip as desired.
                    // The unpacked length is available in the zipEntry.Size property.

                    Stream zipStream = zf.GetInputStream(zipEntry);

                    if (entryFileName.EndsWith("ivy.xml"))
                    {
                        // we're going to rename the ivy.xml temporarily so the package is not picked up by accident while it's
                        // not fully unzipped (could happen on an unexpected restart. The last task of the installer is to
                        // set the ivy.xml file correctly, after all the unzipping is done.
                        entryFileName = entryFileName.Replace("ivy.xml", "ivy-waiting-for-unzip-to-end");
                    }
                    String fullZipToPath = Path.Combine(outFolder, entryFileName);
                    string directoryName = Path.GetDirectoryName(fullZipToPath);
                    if (directoryName.Length > 0)
                        Directory.CreateDirectory(directoryName);
#if !WINDOWS
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
                    // Unzip file in buffered chunks. This is just as fast as unpacking to a buffer the full size
                    // of the file, but does not waste memory.
                    // The "using" will close the stream even if an exception occurs.
                    FileInfo targetFile = new FileInfo(fullZipToPath);
                    using (FileStream streamWriter = targetFile.OpenWrite())
                    {
                        Utils.Copy(zipStream, streamWriter, 4096, targetFile.Length,
                            (totalRead, timeToFinish) =>
                            {
                                EstimatedDuration = timeToFinish;
                                UpdateProgress((float)(totalBytes + totalRead) / targetFile.Length);
                                return !CancelRequested;
                            },
                            100);

                        //Utils.Copy (zipStream, streamWriter, 4096);// 4K is optimum
                    }
                    targetFile.LastWriteTime = zipEntry.DateTime;
                    processed++;
                    totalBytes += zipEntry.Size;
                    double elapsedMillisecondsPerFile = (DateTime.Now - startTime).TotalMilliseconds / processed;
                    EstimatedDuration = Math.Max(1L, (long)((fs.Length - totalBytes) * elapsedMillisecondsPerFile));
                    UpdateProgress((float)processed / zf.Size);
                }
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
        }
    }
}
