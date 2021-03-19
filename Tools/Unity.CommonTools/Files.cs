using System;
using System.Diagnostics;
using System.IO;
using System.Linq;
using System.Text;

namespace Unity.CommonTools
{
    using FileSelector = Func<FileInfo, bool>;

    public static class Files
    {
        public static StreamReader OpenReadOnly(string fileName)
        {
            return new StreamReader(
                new FileStream(fileName, FileMode.Open, FileAccess.Read, FileShare.ReadWrite), Encoding.UTF8);
        }

        public static void Copy(string source, string target)
        {
            Copy(source, target, false);
        }

        public static void Copy(string source, string target, bool includeHiddenFiles)
        {
            if (Directory.Exists(source))
            {
                CopyVisibleDirectoryRecursive(source, target, includeHiddenFiles);
            }
            else
            {
                File.Copy(source, target);
            }
        }

        public static void CopyDeep(string source, string target)
        {
            CopyDeep(source, target, false);
        }

        public static void CopyDeep(string source, string target, bool includeHiddenFiles)
        {
            if (Directory.Exists(source))
            {
                CopyVisibleDirectoryRecursive(source, target, includeHiddenFiles);
            }
            else
            {
                new FileInfo(target).Directory.Create();
                File.Copy(source, target);
            }
        }

        static public void CopyVisibleDirectoryRecursive(string source, string target)
        {
            CopyVisibleDirectoryRecursive(source, target, false);
        }

        static private void CopyVisibleDirectoryRecursive(string source, string target, bool includeHiddenFiles)
        {
            var targetFullPath = Path.GetFullPath(target);
            if (!Directory.Exists(targetFullPath))
                Directory.CreateDirectory(targetFullPath);

            CopyFiles(source, targetFullPath, includeHiddenFiles);
            CopySubDirectories(source, targetFullPath, includeHiddenFiles);
        }

        static void CopySubDirectories(string source, string target, bool includeHiddenFiles)
        {
            foreach (var di in Directory.GetDirectories(source))
            {
                var fname = Path.GetFileName(di);
                if (fname.StartsWith("."))
                    continue;

                CopyVisibleDirectoryRecursive(Path.Combine(source, fname), Path.Combine(target, fname), includeHiddenFiles);
            }
        }

        static void CopyFiles(string source, string target, bool includeHiddenFiles)
        {
            foreach (var fi in Directory.GetFiles(source))
            {
                var fname = Path.GetFileName(fi);
                if (!includeHiddenFiles && fname.StartsWith("."))
                    continue;

                var targetPath = Path.Combine(target, fname);
                if (File.Exists(targetPath))
                {
                    FileAttributes attributes = File.GetAttributes(targetPath);
                    attributes &= ~(FileAttributes.Hidden | FileAttributes.ReadOnly);
                    File.SetAttributes(targetPath, attributes);
                }

                File.Copy(fi, targetPath, true);
            }
        }

        public static void CopyProjectFolder(string projectSource)
        {
            CopyVisibleDirectoryRecursive(Path.Combine(projectSource, "Assets"), "Assets", false);

            string[] potentialLibaryFiles =
            {
                "EditorBuildSettings.asset", "ProjectSettings.asset", "TagManager.asset", "TimeManager.asset", "InputManager.asset"
            };

            if (!Directory.Exists("ProjectSettings"))
                Directory.CreateDirectory("ProjectSettings");

            foreach (var file in potentialLibaryFiles)
            {
                var sourceFile = Path.Combine(Path.Combine(projectSource, "ProjectSettings"), file);
                if (File.Exists(sourceFile))
                {
                    Console.WriteLine("Copying " + Path.Combine("ProjectSettings", file));
                    File.Copy(sourceFile, Path.Combine("ProjectSettings", file), true);
                }
            }
        }

        public static void CopyFromDiskImage(string diskImagePath, string mountPath, string copyFromMount, string copyToPath)
        {
            if (Workspace.IsWindows())
            {
                throw new InvalidOperationException("Disk image manipulation not available on Windows.");
            }
            string result;
            RunAndWait("hdiutil", "eject \"" + mountPath + "\" -quiet", out result);
            RunAndWait("hdid", "\"" + diskImagePath + "\" -quiet", out result);
            RunAndWait("cp", "-R \"" + Path.Combine(mountPath, copyFromMount) + "\" \"" + copyToPath + "\"", out result);
            RunAndWait("hdiutil", "eject \"" + mountPath + "\" -quiet", out result);
        }

        public static int RunAndWait(string fileName, string arguments, out string result)
        {
            return RunAndWait(fileName, arguments, false, out result);
        }

        public static int RunAndWait(string fileName, string arguments, bool removeMonoEnvironmentVariables, out string result)
        {
            var output = new StringBuilder();
            var process = new Process();

            process.StartInfo.FileName = fileName;
            process.StartInfo.Arguments = arguments;
            process.StartInfo.CreateNoWindow = true;
            process.StartInfo.UseShellExecute = false;
            process.StartInfo.RedirectStandardOutput = true;
            process.StartInfo.StandardOutputEncoding = Encoding.UTF8;

            if (removeMonoEnvironmentVariables)
                RemoveMonoEnvironmentVariables(process.StartInfo);

            process.OutputDataReceived += (sender, args) => output.AppendLine(args.Data);
            process.Start();
            process.BeginOutputReadLine();
            process.WaitForExit();

            result = output.ToString();
            return process.ExitCode;
        }

        public static void RemoveMonoEnvironmentVariables(ProcessStartInfo startInfo)
        {
            startInfo.EnvironmentVariables.Remove("MONO_PATH");
            startInfo.EnvironmentVariables.Remove("MONO_GAC_PREFIX");
            startInfo.EnvironmentVariables.Remove("MONO_CONFIG");
            startInfo.EnvironmentVariables.Remove("MONO_CFG_DIR");
            startInfo.EnvironmentVariables.Remove("DYLD_FALLBACK_LIBRARY_PATH");
        }

        public static string GetTempFile()
        {
            return Path.Combine(GetTempPath(), Path.GetRandomFileName());
        }

        public static string GetTempPath()
        {
            var t = Environment.GetEnvironmentVariable("UNITY_INTEGRATIONTESTS_TEMP");
            if (t != null && t.Length > 0)
                return t;
            return Path.GetTempPath();
        }

        public static string CreateTempFolder()
        {
            var foldername = Path.GetRandomFileName();
            return Path.Combine(GetTempPath(), foldername);
        }

        public static string CreateTempFolderWithSpacesInPath()
        {
            var foldername = string.Format("{0} With Spaces", Path.GetRandomFileName());
            return Path.Combine(GetTempPath(), foldername);
        }

        public static void CopyDirectoryWithSymlinks(DirectoryInfo source, DirectoryInfo target)
        {
            if (Workspace.IsWindows())
            {
                CopyDirectory(source, target);
            }
            else
            {
                Directory.Delete(target.FullName, true);

                var startInfo = new ProcessStartInfo("cp", "-R '" + source.FullName + "' '" + target.FullName + "'");
                using (var cp = new Process())
                {
                    cp.StartInfo = startInfo;
                    cp.Start();
                    cp.WaitForExit();

                    if (cp.ExitCode != 0)
                        throw new ArgumentException("Failed copy");
                }
            }

            if (!target.Exists)
                throw new ArgumentException("Failed copy");

            Console.WriteLine("cp " + source.FullName + " -> " + target.FullName);
        }

        public static void CopyDirectory(DirectoryInfo source,
            DirectoryInfo target, FileSelector fileSelector)
        {
            if (Directory.Exists(target.FullName) == false)
                Directory.CreateDirectory(target.FullName);

            foreach (var fi in source.GetFiles().Where(fileSelector))
                fi.CopyTo(Path.Combine(target.ToString(), fi.Name), true);

            foreach (var sourceSubDir in source.GetDirectories())
                CopyDirectory(sourceSubDir,
                    target.CreateSubdirectory(sourceSubDir.Name),
                    fileSelector);
        }

        public static void CopyDirectory(DirectoryInfo source,
            DirectoryInfo target)
        {
            CopyDirectory(source, target, file => true);
        }

        public static string ZipPath(string path)
        {
            string targetName = Paths.GetFileOrFolderName(path), zipFile = targetName + ".zip";
            var zipPath = Path.Combine(Path.GetDirectoryName(path), zipFile);

            if (File.Exists(zipPath))
            {
                File.Delete(zipPath);
            }

            ProcessStartInfo startInfo;
            if (Workspace.IsWindows())
            {
                var sevenZipPath = Workspace.ResolvePath("External", "PlatformDependent", "Win", "builds", "libs", "7zip", "7za.exe");
                Console.WriteLine("Grabbing 7zip from " + sevenZipPath);
                startInfo = new ProcessStartInfo(sevenZipPath, "a " + zipFile + " " + targetName);
            }
            else
            {
                startInfo = new ProcessStartInfo("zip", "-r " + zipFile + " " + targetName);
            }

            startInfo.WorkingDirectory = Path.GetDirectoryName(path);

            using (var zipProcess = new Process())
            {
                zipProcess.StartInfo = startInfo;
                zipProcess.Start();
                zipProcess.WaitForExit();
            }

            return zipPath;
        }

        public static string GetBundleVersion(string bundlePath)
        {
            var plistFile = Paths.Combine(bundlePath, "Contents", "Info.plist");

            if (!File.Exists(plistFile))
            {
                Console.WriteLine("GetBundleVersion did not find Info.plist for the bundle. Expected at: " + plistFile);
                return null;
            }

            var version = File.ReadAllText(plistFile);

            version = version.Substring(version.IndexOf("<key>CFBundleVersion</key>") + "<key>CFBundleVersion</key>".Length);
            version = version.Substring(version.IndexOf("<string>") + "<string>".Length);
            version = version.Substring(0, version.IndexOf("</string>"));

            return version;
        }

        public static void SetPlistValue(string plistFile, string keyString, string versionString)
        {
            if (!File.Exists(plistFile))
            {
                throw new FileNotFoundException("SetPlistVersion did not find the specified plist file: " + plistFile);
            }

            var version = File.ReadAllText(plistFile);

            var valueStartIndex = version.IndexOf("<key>" + keyString + "</key>") + ("<key>" + keyString + "</key>").Length;
            var valueEndIndex = version.IndexOf("</string>", valueStartIndex) + "</string>".Length;

            File.WriteAllText(
                plistFile,
                string.Format(
                    "{0}\n<string>{1}</string>\n{2}",
                    version.Substring(0, valueStartIndex),
                    versionString,
                    version.Substring(valueEndIndex)
                )
            );
        }

        public static void WriteAllTextCreatingRequiredDirectories(string fileName, string text)
        {
            EnsureParentDirectory(fileName);
            File.WriteAllText(fileName, text);
        }

        public static void WriteAllBytesCreatingRequiredDirectories(string fileName, byte[] bytes)
        {
            EnsureParentDirectory(fileName);
            File.WriteAllBytes(fileName, bytes);
        }

        static void EnsureParentDirectory(string fileName)
        {
            var directoryName = Path.GetDirectoryName(fileName);
            Directory.CreateDirectory(directoryName);
        }

        public static void DeleteFilesAndDirectoriesUnder(string path)
        {
            foreach (var subDir in Directory.GetDirectories(path))
            {
                DeleteFilesAndDirectoriesUnder(subDir);
                Directory.Delete(subDir, false);
            }
            foreach (var file in Directory.GetFiles(path))
            {
                File.SetAttributes(file, File.GetAttributes(file) & ~FileAttributes.ReadOnly);
                File.Delete(file);
            }
        }

        public static string ReadAllTextReadOnly(string logFilePath)
        {
            using (var sw = OpenReadOnly(logFilePath))
            {
                return sw.ReadToEnd();
            }
        }
    }
}
