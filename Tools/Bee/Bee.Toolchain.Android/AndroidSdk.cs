using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Text.RegularExpressions;
using Bee.Core;
using NiceIO;
using Unity.BuildTools;

namespace Bee.Toolchain.Android
{
    public static class AndroidSdk
    {
        public static NPath JdkBinPath
        {
            get
            {
                if (HostPlatform.IsWindows)
                    return "PlatformDependent/AndroidPlayer/External/OpenJDK/builds/builds/Windows/bin";
                else if (HostPlatform.IsOSX)
                    return "PlatformDependent/AndroidPlayer/External/OpenJDK/builds/builds/MacOS/bin";
                else if (HostPlatform.IsLinux)
                    return "PlatformDependent/AndroidPlayer/External/OpenJDK/builds/builds/Linux/bin";
                else
                    return "";
            }
        }

        public static NPath GetJavaCommand(string exe)
        {
            NPath cmd = exe;
            var javaPath = JdkBinPath;
            if (javaPath != "")
                cmd = $"{javaPath}/{exe}";
            return HostPlatform.IsWindows ? cmd.InQuotes(SlashMode.Backward) : cmd.InQuotes();
        }

        public static NPath JavacPath
        {
            get
            {
                return GetJavaCommand("javac");
            }
        }

        public static NPath JarPath
        {
            get
            {
                return GetJavaCommand("jar");
            }
        }

        public static NPath JavaPath
        {
            get
            {
                return GetJavaCommand("java");
            }
        }

        public static NPath Path
        {
            get
            {
                var embededPath = new NPath("External/Android/NonRedistributable/sdk/builds");
                if (embededPath.DirectoryExists())
                    return embededPath;
                var path = Environment.GetEnvironmentVariable("ANDROID_SDK_ROOT");
                return path != null ? new NPath(path) : null;
            }
        }

        public static NPath RootPath
        {
            get
            {
                var path = Path;
                if (path == null)
                {
                    Errors.Exit("Android build requires ANDROID_SDK_ROOT environment variable to be set or built-in SDK to be present");
                }
                if (!path.DirectoryExists())
                {
                    Errors.Exit($"Android build; Android SDK folder does not exist: '{path}'");
                }
                return path;
            }
        }

        private static string Separator => HostPlatform.IsWindows ? ";" : ":";

        public static Version GetToolsVersion(NPath folder)
        {
            folder = folder.MakeAbsolute();
            if (!folder.DirectoryExists())
                return new Version(0, 0);
            var propsFile = folder.Combine("source.properties");
            if (!propsFile.FileExists())
                return new Version(0, 0);
            return GetToolsVersionFromFileContents(propsFile.ReadAllText());
        }

        public static Version GetToolsVersionFromFileContents(string sourcePropsContents)
        {
            foreach (var row in sourcePropsContents.Split('\n'))
            {
                var match = Regex.Match(row, "^\\s*Pkg\\.Revision\\s*=\\s*([0-9.]+)");
                if (match.Success)
                {
                    var version = match.Groups[1].Value;
                    if (version.IndexOf('.') == -1)
                        version += ".0";
                    if (Version.TryParse(version, out var result))
                        return result;
                }
            }
            return new Version(0, 0);
        }

        private static HashSet<NPath> CompileJavaFoldersUsed = new HashSet<NPath>();

        private static NPath[] CompileJava(
            NPath destFolder,
            NPath[] sourceFiles,
            NPath[] sourceFolders,
            NPath[] classPath,
            NPath bootClassPath,
            string targetVersion,
            bool @unchecked
        )
        {
            // javac produces unknown amount of files (each nested class gets it's own .class file),
            // so we demand that each compilation be done into unique destination folder, and
            // clear anything in it together with compilation, to prevent stale .class files there.
            if (CompileJavaFoldersUsed.Contains(destFolder))
                Errors.Exit($"Java files were already compiled into destination folder {destFolder}, we don't support compiling several times into same destination");
            CompileJavaFoldersUsed.Add(destFolder);

            // resulting .class files are produced with the same folder hierarchy under destFolder, as they are under
            // the sourceFolder's.
            var dstFiles = sourceFiles.Select(p =>
                destFolder.Combine(p.RelativeTo(sourceFolders.First(p.IsChildOf)).ChangeExtension("class"))).ToArray();

            var args = sourceFiles.InQuotes()
                .Append("-d", destFolder.InQuotes())
                .Append("-sourcepath", string.Join(Separator, sourceFolders.InQuotes()))
                .Append("-target", targetVersion, "-source", targetVersion)
                .Append("-encoding", "ascii", "-g");
            if (classPath.Any())
                args = args.Append("-classpath", string.Join(Separator, classPath.InQuotes()));
            if (bootClassPath != null)
                args = args.Append("-bootclasspath", bootClassPath.InQuotes());
            if (@unchecked)
                args = args.Append("-Xlint:unchecked");

            // command to delete & recreate the destination folder
            var rmDir = HostPlatform.IsWindows ?
                $"(if exist {destFolder.InQuotes(SlashMode.Backward)} (rmdir /s /q {destFolder.InQuotes(SlashMode.Backward)} && mkdir {destFolder.InQuotes(SlashMode.Backward)}))" :
                $"rm -rf {destFolder.InQuotes()} && mkdir {destFolder.InQuotes()}";

            var cmd = $"{rmDir} && ({JavacPath} {args.SeparateWithSpace()})";

            Backend.Current.AddAction("AndroidJavac", dstFiles, sourceFiles.Concat(classPath).ToArray(), cmd, new string[] {});
            return dstFiles;
        }

        public static NPath[] CompileJavaAndroid(NPath destFolder, NPath[] sourceFiles, NPath[] sourceFolders, NPath[] classPath, string api)
        {
            var sdkRoot = RootPath;
            var bootClassPath = sdkRoot.Combine($"platforms/{api}/android.jar");
            if (!bootClassPath.FileExists())
                Errors.Exit($"Android build; looks like {api} part of SDK is not set up? Could not find file {bootClassPath}");
            return CompileJava(destFolder, sourceFiles, sourceFolders, classPath, bootClassPath, "1.8", false);
        }

        public static NPath[] CompileJavaTools(NPath destFolder, NPath[] sourceFiles, NPath[] sourceFolders, NPath[] classPath)
        {
            return CompileJava(destFolder, sourceFiles, sourceFolders, classPath, null, "1.8", @unchecked: true);
        }

        public static void BuildJar(NPath[] classFolders, NPath[] classFilesToDependOn, NPath[] inputJars, NPath destJar, NPath manifest, bool noCompression = false, bool noManifest = false)
        {
            if (manifest != null)
                classFilesToDependOn = classFilesToDependOn.Append(manifest).ToArray();
            if (inputJars != null)
                classFilesToDependOn = classFilesToDependOn.Concat(inputJars).ToArray();

            // some JAR versions (seemingly older than JDK 1.9) can't put several class folders
            // into one .jar file. For example if you have this file structure:
            //   folder1/com/foo/Bar.class
            //   folder2/com/foo/Baz.class
            // then you can't do a "jar -C folder1 . -C folder2 ."; it will error with
            // "duplicate entry com/".
            //
            // So what we do is construct a command line that invokes jar multiple times;
            // first time with "create" argument, other times with "update", i.e.
            // (jar cf -C folder1 .) && (jar uf -C folder2 .)

            var cmd = new StringBuilder();
            for (var i = 0; i < classFolders.Length; ++i)
            {
                if (i != 0)
                    cmd.Append(" && ");
                cmd.Append($"({JarPath} ");
                cmd.Append(i == 0 ? "cf" : "uf");
                if (i == 0 && manifest != null)
                    cmd.Append('m');
                if (noManifest)
                    cmd.Append('M');
                if (noCompression)
                    cmd.Append('0');
                cmd.Append($" {destJar.InQuotes()}");
                if (i == 0 && manifest != null)
                    cmd.Append($" {manifest.InQuotes()}");
                cmd.Append($" -C {classFolders[i].InQuotes()} .");

                if (i == 0 && inputJars != null)
                {
                    foreach (var ij in inputJars)
                    {
                        cmd.Append($" -C {ij.Parent.InQuotes()} {ij.FileName.InQuotes()}");
                    }
                }
                cmd.Append(')');
            }
            Backend.Current.AddAction("AndroidJar", new[] {destJar}, classFilesToDependOn, cmd.ToString(), new string[] {});
        }

        // Builds JAR, obfuscates, builds DEX file
        public static void BuildAndroidJar(NPath[] classFolders, NPath[] classFiles, NPath tmpFolder, NPath destJar, NPath destDex, bool dontObfuscate, NPath[] extraJarsToMerge)
        {
            // create temporary JAR
            var tmpJar = tmpFolder.Combine(destJar.FileName);
            BuildJar(classFolders, classFiles, null, tmpJar, manifest: null, noCompression: true, noManifest: true);

            // obfuscate
            var sdkRoot = RootPath;
            var libraryJar = sdkRoot.Combine("platforms/android-28/android.jar");
            if (!libraryJar.FileExists())
                Errors.Exit($"Android build; looks like android-28 part of SDK is not set up? Could not find file {libraryJar}");
            Backend.Current.AddAction(
                "AndroidObfuscate",
                new[] {destJar},
                new[] {tmpJar, libraryJar}.Concat(extraJarsToMerge).ToArray(),
                $"{JavaPath}",
                new[]
                {
                    "-Dmaximum.inlined.code.length=16",
                    "-jar",
                    "PlatformDependent/AndroidPlayer/Tools/ProGuard/lib/proguard.jar",
                    "@PlatformDependent/AndroidPlayer/Jam/proguard-config.txt",
                    "-injars",
                    String.Join(
                        Separator,
                        extraJarsToMerge.Select(jar => jar.ToString() + "\\(!META-INF/MANIFEST.MF\\)")
                            .Append(tmpJar.InQuotes())),
                    "-outjars",
                    destJar.InQuotes(),
                    "-libraryjars",
                    libraryJar.InQuotes(),
                    dontObfuscate ? "-dontobfuscate" : ""
                }, allowUnwrittenOutputFiles: true);

            // produce Dalvik executable (DEX)
            var dxJar = GetDxJar(sdkRoot);
            Backend.Current.AddAction(
                "AndroidDex",
                new[] {destDex},
                new[] {destJar},
                $"{JavaPath}",
                new[] { "-Xmx1024M", "-jar", dxJar.InQuotes(), "--dex", $"--output={destDex.InQuotes()}", destJar.InQuotes() });
        }

        private static NPath GetDxJar(NPath sdkRoot)
        {
            var jar = sdkRoot.Combine("platform-tools/lib/dx.jar");
            if (jar.FileExists())
                return jar;

            // find latest in build-tools
            NPath latestDir = null;
            var latestVersion = new Version();
            var folders = sdkRoot.Combine("build-tools").Directories();
            foreach (var f in folders)
            {
                var version = GetToolsVersion(f);
                if (version <= latestVersion) continue;
                latestVersion = version;
                latestDir = f;
            }

            if (latestDir == null)
                Errors.Exit($"Android build; could not determine latest build tools folder in SDK {sdkRoot}. Is SDK set up correctly?");
            else
            {
                jar = latestDir.Combine("lib/dx.jar");
                if (!jar.FileExists())
                    Errors.Exit($"Android build; could not find dx.jar in SDK build tools folder {latestDir}. Is SDK set up correctly?");
            }

            return jar;
        }
    }
}
