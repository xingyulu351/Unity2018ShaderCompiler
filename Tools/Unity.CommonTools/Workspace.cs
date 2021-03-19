using System;
using System.Diagnostics;
using System.IO;
using System.Reflection;

namespace Unity.CommonTools
{
    public class Workspace
    {
        public static string AndroidSDKPath
        {
            get
            {
                return Path.Combine(GetBasePathWithoutDots(), "External/Android/NonRedistributable/sdk/builds");
            }
        }

        public static string AndroidNDKPath
        {
            get
            {
                // The path returned by Workspace.ResolvePath() with ../../../ makes the linker fail on Windows
                return Path.Combine(GetBasePathWithoutDots(), @"External/Android/NonRedistributable/ndk/builds");
            }
        }

        public static string AndroidADBPath
        {
            get
            {
                var path = Paths.Combine(AndroidSDKPath, "platform-tools", IsWindows() ? "adb.exe" : "adb");
                if (!File.Exists(path))
                {
                    throw new ApplicationException("Unable to locate ADB, no such file: " + path);
                }
                return path;
            }
        }

        public static string AndroidAAPTPath
        {
            get
            {
                var buildToolsPath = Paths.Combine(AndroidSDKPath, "build-tools");
                var buildTools = Directory.GetDirectories(buildToolsPath);
                if (buildTools.Length == 0)
                {
                    throw new ApplicationException("Unable to locate build-tools in: " + buildToolsPath);
                }
                var path = Paths.Combine(buildTools[buildTools.Length - 1], IsWindows() ? "aapt.exe" : "aapt");
                if (!File.Exists(path))
                {
                    throw new ApplicationException("Unable to locate AAPT, no such file: " + path);
                }
                return path;
            }
        }

        private static string m_JavaPath = null;
        private static bool IsValidJDKHome(string javaHome)
        {
            if (string.IsNullOrEmpty(javaHome))
                return false;
            string ext = IsWindows() ? ".exe" : "";
            string javaExe = Paths.Combine(javaHome, "java" + ext);
            string javacExe = Paths.Combine(javaHome, "javac" + ext);
            return File.Exists(javaExe) && File.Exists(javacExe);
        }

        public static string JavaPath
        {
            get
            {
                if (!string.IsNullOrEmpty(m_JavaPath))
                    return m_JavaPath;

                // Partially taken from PlatformDependent\AndroidPlayer\Editor\Managed\Android\AndroidJavaTools.cs
                string javaHome = Environment.GetEnvironmentVariable("JAVA_HOME") ?? "";

                string[] searchLocations = new string[]
                {
                    javaHome,
                    Path.Combine(javaHome, "bin"),
                    "/System/Library/Frameworks/JavaVM.framework/Versions/CurrentJDK/Home/",
                    "/System/Library/Frameworks/JavaVM.framework/Versions/Current/Home/",
                    "/usr/bin/"
                };

                foreach (var location in searchLocations)
                {
                    if (IsValidJDKHome(location))
                    {
                        m_JavaPath = Path.Combine(location, "java" + (IsWindows() ? ".exe" : ""));
                        return m_JavaPath;
                    }
                }

                throw new Exception("Failed to locate JAVA folder (java and javac should be present)");
            }
        }

        public static string WebExtractPath
        {
            get
            {
                var webExtractFiles = Directory.GetFiles(EditorDirectory, IsWindows() ? "WebExtract.exe" : "WebExtract", SearchOption.AllDirectories);
                if (webExtractFiles.Length == 1)
                    return webExtractFiles[0];
                throw new ArgumentException("Unable to find WebExtract");
            }
        }

        public static string Binary2TextPath
        {
            get
            {
                var binary2textFiles = Directory.GetFiles(EditorDirectory, IsWindows() ? "binary2text.exe" : "binary2text", SearchOption.AllDirectories);
                if (binary2textFiles.Length == 1)
                    return binary2textFiles[0];
                throw new ArgumentException("Unable to find binary2text");
            }
        }

        public static string AssetCacheServerPath
        {
            get
            {
                var AssetCacheServerFiles = Directory.GetFiles(EditorDirectory, IsWindows() ? "AssetCacheServer.exe" : "AssetCacheServer", SearchOption.AllDirectories);
                if (AssetCacheServerFiles.Length == 1)
                    return AssetCacheServerFiles[0];
                throw new ArgumentException("Unable to find AssetCacheServer");
            }
        }

        public static string EditorPath
        {
            get { return EditorExecutableIn(EditorDirectory); }
        }

        public static string EditorExecutableIn(string editorDirectory)
        {
            if (IsWindows())
                return Paths.Combine(editorDirectory, "Unity.exe");
            else if (IsOSX())
                return Paths.Combine(editorDirectory, "Unity.app", "Contents", "MacOS", "Unity");
            else if (IsLinux())
                return Paths.Combine(editorDirectory, "Unity");

            throw new ArgumentException("Unable to detect platform");
        }

        public static string EditorDirectory
        {
            get
            {
                var path = Environment.GetEnvironmentVariable("UNITY_USESYSTEMINSTALLEDEDITOR");
                if (path == "1")
                {
                    return SystemInstalledEditorDirectory;
                }

                string platformDependentPath = string.Empty;
                if (IsWindows())
                    platformDependentPath = "WindowsEditor";
                else if (IsOSX())
                    platformDependentPath = "MacEditor";
                else if (IsLinux())
                    platformDependentPath = "LinuxEditor";
                else
                    throw new ArgumentException("Unable to detect platform");

                return ResolvePath(Paths.Combine("build", platformDependentPath));
            }
        }

        static string SystemInstalledEditorDirectory
        {
            get
            {
                var editorInstallationPath = Environment.GetEnvironmentVariable("UNITY_EDITOR_PATH");
                if (!string.IsNullOrEmpty(editorInstallationPath))
                    return editorInstallationPath;

                if (IsWindows())
                {
                    // if you are on a 64-bit system and use the environment variable "ProgramFiles",
                    // the result you get depend on whether the process requesting the environment variable is 32-bit or 64-bit.
                    // Since we dropped support for Windows 32 bit editor, we want to make sure that we always try to find Unity in the 64 bit path.
                    // The environment variable "ProgramW6432" does that, as documented in the table at
                    // https://msdn.microsoft.com/en-us/library/windows/desktop/aa384274%28v=vs.85%29.aspx?f=255&MSPPError=-2147217396
                    var programFilesPath = Environment.GetEnvironmentVariable("ProgramW6432");
                    return Paths.Combine(programFilesPath, "Unity", "Editor");
                }

                if (IsOSX())
                    return Paths.Combine("/Applications", "Unity");

                if (IsLinux())
                    return Paths.Combine("/usr", "lib", "unity3d");

                throw new ArgumentException("Unable to detect platform");
            }
        }

        public static string ProgramFilesx86()
        {
            if (8 == IntPtr.Size
                || (!string.IsNullOrEmpty(Environment.GetEnvironmentVariable("PROCESSOR_ARCHITEW6432"))))
            {
                return Environment.GetEnvironmentVariable("ProgramFiles(x86)");
            }

            return Environment.GetEnvironmentVariable("ProgramFiles");
        }

        public static string EditorDataPath
        {
            get
            {
                string dataDir = string.Empty;

                if (IsWindows() || IsLinux())
                    dataDir = "Data";
                else if (IsOSX())
                    dataDir = Paths.Combine("Unity.app", "Contents");
                else
                    throw new ArgumentException("Unable to detect platform");

                return Paths.Combine(EditorDirectory, dataDir);
            }
        }

        public static string UnityExtensionsDataPath
        {
            get
            {
                return Path.Combine(EditorDirectory, (IsWindows() || IsLinux()) ?
                    Paths.Combine("Data", "UnityExtensions", "Unity") :
                    Paths.Combine("Unity.app", "Contents", "UnityExtensions", "Unity"));
            }
        }

        public static string NUnitDllPath
        {
            get
            {
                return Paths.Combine(UnityExtensionsDataPath, "TestRunner", "nunit.framework.dll");
            }
        }

        public static string UnityEngineAssemblyPath
        {
            get
            {
                return Paths.Combine(EditorDataPath, "Managed", "UnityEngine.dll");
            }
        }

        public static string UnityEngineCoreModuleAssemblyPath
        {
            get
            {
                return Paths.Combine(EditorDataPath, "Managed", "UnityEngine", "UnityEngine.CoreModule");
            }
        }


        public static string UnityEditorAssemblyPath
        {
            get
            {
                return Paths.Combine(EditorDataPath, "Managed", "UnityEditor.dll");
            }
        }

        public static string UnityEngineUIAssemblyPath
        {
            get
            {
                return Paths.Combine(UnityExtensionsDataPath, "GUISystem", "UnityEngine.UI.dll");
            }
        }

        public static string ResolvePath(string path)
        {
            return Path.Combine(BasePath, path);
        }

        public static string ResolvePath(params string[] path)
        {
            return ResolvePath(Paths.Combine(path));
        }

        public static string ResolveAutomationTestPath(string path)
        {
            return Path.Combine(BasePathAutomationTestPath, path);
        }

        public static string ResolveIntegrationTestPath(string path)
        {
            return Path.Combine(BasePathIntegrationTestPath, path);
        }

        public static string ResolveRuntimeTestPath(string path)
        {
            return Path.Combine(BasePathRuntimeTestPath, path);
        }

        public static string ResolvePerformanceTestPath(string path)
        {
            return Path.Combine(BasePathPerformanceTestPath, path);
        }

        public static string ResolvePerformanceRuntimeTestPath(string path)
        {
            return Path.Combine(BasePathPerformanceRuntimeTestPath, path);
        }

        public static string ResolveEditorUITestPath(string path)
        {
            return Path.Combine(BasePathEditorUITestPath, path);
        }

        public static string ResolveEditorUITestFrameworkPath(string path)
        {
            return Path.Combine(BasePathEditorUITestFrameworkPath, path);
        }

        public static string ResolveIMGUIControlTestsPath(string path)
        {
            return Path.Combine(BasePathIMGUIControlTestsPath, path);
        }

        public static string ResolveIMGUIControlTestsFrameworkPath(string path)
        {
            return Path.Combine(BasePathIMGUIControlTestsFrameworkPath, path);
        }

        public static string ResolveRuntimeTestFrameworkPath(string path)
        {
            return Path.Combine(BasePathRuntimeTestFrameworkPath, path);
        }

        public static string BasePath
        {
            get { return _basePath ?? (_basePath = ComputeBasePath()); }
        }

        private static string ComputeBasePath()
        {
            var candidate = Paths.Combine(Path.GetDirectoryName(new Uri(typeof(Workspace).Assembly.CodeBase).LocalPath), "..", "..", "..", "..");
            while (!File.Exists(Path.Combine(candidate, "build.pl")))
            {
                candidate = Path.GetDirectoryName(candidate);
                if (!Directory.Exists(candidate))
                {
                    candidate = Path.GetDirectoryName(Assembly.GetEntryAssembly().Location);
                    Console.Out.WriteLine("BasePath not found! (Are you not in the repo?)");
                    Console.Out.WriteLine("BasePath is set to: " + candidate);
                    break;
                }
            }
            return candidate;
        }

        private static string GetBasePathWithoutDots()
        {
            var basePath = Path.GetDirectoryName(new Uri(typeof(Workspace).Assembly.CodeBase).LocalPath);
            while (basePath != null && !File.Exists(Path.Combine(basePath, "build.pl")))
            {
                basePath = Path.GetDirectoryName(basePath);
            }
            return basePath;
        }

        private static string _basePath;

        public static string BasePathAutomationTestPath
        {
            get { return Paths.Combine(BasePath, "Tests", "Unity.Automation.Tests"); }
        }

        public static string BasePathIntegrationTestPath
        {
            get { return Paths.Combine(BasePath, "Tests", "Unity.IntegrationTests"); }
        }

        public static string BasePathIntegrationTestFramework
        {
            get { return Paths.Combine(BasePath, "Tests", "Unity.IntegrationTests.Framework"); }
        }

        public static string BasePathRuntimeTestsFramework
        {
            get { return Paths.Combine(BasePath, "Tests", "Unity.RuntimeTests.Framework"); }
        }

        public static string BasePathRuntimeTestPath
        {
            get { return Paths.Combine(BasePath, "Tests", "Unity.RuntimeTests"); }
        }
        public static string BasePathEditorUITestPath
        {
            get { return Paths.Combine(BasePath, "Tests", "Unity.EditorUITests"); }
        }
        public static string BasePathEditorUITestFrameworkPath
        {
            get { return Paths.Combine(BasePath, "Tests", "Unity.EditorUITests.Framework"); }
        }
        public static string BasePathIMGUIControlTestsPath
        {
            get { return Paths.Combine(BasePath, "Tests", "Unity.IMGUIControlTests"); }
        }
        public static string BasePathIMGUIControlTestsFrameworkPath
        {
            get { return Paths.Combine(BasePath, "Tests", "Unity.IMGUIControlTests.Framework"); }
        }

        public static string BasePathRuntimeTestFrameworkPath
        {
            get { return Paths.Combine(BasePath, "Tests", "Unity.RuntimeTests.Framework"); }
        }

        public static string BasePathCommonTools
        {
            get { return Paths.Combine(BasePath, "Tools", "Unity.CommonTools"); }
        }

        public static string BasePathPerformanceTestPath
        {
            get { return Paths.Combine(BasePath, "Tests", "Unity.PerformanceTests"); }
        }

        public static string BasePathPerformanceRuntimeTestPath
        {
            get { return Paths.Combine(BasePath, "Tests", "Unity.PerformanceRuntimeTests"); }
        }

        public static string AndroidToolsPath
        {
            get { return ResolvePath("build/AndroidPlayer/Tools"); }
        }

        public static string MetroToolsDirectoryPath
        {
            get { return ResolvePath("build/MetroSupport/Tools"); }
        }


        public static string MetroSourceBuildPath
        {
            get { return ResolvePath("build/MetroSupport/SourceBuild"); }
        }

        public static string MetroLogsPath
        {
            get { return ResolvePath("build/MetroSupport/Logs"); }
        }

        public static bool IsLinux()
        {
            return Platform.IsLinux;
        }

        public static bool IsOSX()
        {
            return Platform.IsOSX;
        }

        public static bool IsWindows()
        {
            return Platform.IsWindows;
        }

        public static bool IsWindowsXP()
        {
            return Environment.OSVersion.Platform == PlatformID.Win32NT &&
                Environment.OSVersion.Version.Major == 5 && Environment.OSVersion.Version.Minor == 1;
        }

        public static bool IsWindowsVista()
        {
            return Environment.OSVersion.Platform == PlatformID.Win32NT &&
                Environment.OSVersion.Version.Major == 6 && Environment.OSVersion.Version.Minor == 0;
        }

        public static bool IsWindows7()
        {
            return Environment.OSVersion.Platform == PlatformID.Win32NT &&
                Environment.OSVersion.Version.Major == 6 &&
                (Environment.OSVersion.Version.Minor == 2 || Environment.OSVersion.Version.Minor == 1);
        }

        private static string GetWindowsProductName()
        {
            var key = Microsoft.Win32.Registry.LocalMachine.OpenSubKey(@"SOFTWARE\Microsoft\Windows NT\CurrentVersion");
            if (key == null)
                return "";
            var productName = key.GetValue("ProductName") as string;
            if (productName == null)
                return "";
            return productName;
        }

        public static bool IsWindows81()
        {
            if (Environment.OSVersion.Platform != PlatformID.Win32NT)
                return false;

            // Note: Alternatively we could use VerifyVersionInfo function. See PlatformDependent\Win\VersionHelpers.h
            // But that requires tweaking application manifest specifying that the application is compatible with Windows 10
            return GetWindowsProductName().Contains("Windows 8.1");
        }

        public static bool IsWindows10()
        {
            if (Environment.OSVersion.Platform != PlatformID.Win32NT)
                return false;

            return GetWindowsProductName().Contains("Windows 10");
        }

        public static bool AmRunningOnBuildServer()
        {
            return Environment.GetEnvironmentVariable("UNITY_THISISABUILDMACHINE") == "1";
        }

        public static string ResourceName(Type type, string resource)
        {
            return type.Namespace + ".Resources." + resource;
        }

        public static string NamespacePath(Type type)
        {
            if (type.Namespace.IndexOf("Unity.IntegrationTests.Framework") == 0)
            {
                return ResolvePath("Tests", "Unity.IntegrationTests.Framework",
                    Paths.Combine(type.Namespace.Substring("Unity.IntegrationTests.Framework".Length).Split('.')));
            }

            if (type.Namespace.IndexOf("Unity.IntegrationTests") == 0)
            {
                return ResolvePath("Tests", "Unity.IntegrationTests",
                    Paths.Combine(type.Namespace.Substring("Unity.IntegrationTests".Length).Split('.')));
            }

            if (type.Namespace.IndexOf("Unity.Automation") == 0)
            {
                return ResolvePath("Tests", "Unity.Automation",
                    Paths.Combine(type.Namespace.Substring("Unity.Automation".Length).Split('.')));
            }

            return null;
        }

        public static string ResourcePath(Type type, string resource)
        {
            return Paths.Combine(NamespacePath(type), "Resources", resource);
        }

        private static string s_CurrentVersionString;
        public static string CurrentVersionString
        {
            get
            {
                if (s_CurrentVersionString == null)
                {
                    s_CurrentVersionString = File.ReadAllText(ResolvePath(Path.Combine("Configuration",
                        "BuildConfig.pm")));
                    s_CurrentVersionString = s_CurrentVersionString.Substring(s_CurrentVersionString.IndexOf("$unityVersion") + "$unityVersion".Length);
                    s_CurrentVersionString = s_CurrentVersionString.Substring(s_CurrentVersionString.IndexOf("\"") + 1);
                    s_CurrentVersionString = s_CurrentVersionString.Substring(0,
                        s_CurrentVersionString.IndexOf("\""));
                }
                return s_CurrentVersionString;
            }
        }

        public static string GetUnityArtifactsFolder()
        {
            var artifactsFolder = Environment.GetEnvironmentVariable("UNITY_UNIFIED_TEST_RUNNER_UNITY_ARTIFACTS_FOLDER");
            if (!string.IsNullOrEmpty(artifactsFolder))
            {
                return artifactsFolder;
            }

            return Path.Combine(Path.GetTempPath(), Path.GetRandomFileName());
        }

        public static bool SupportsHiddenFiles()
        {
            return !IsLinux();
        }
    }
}
