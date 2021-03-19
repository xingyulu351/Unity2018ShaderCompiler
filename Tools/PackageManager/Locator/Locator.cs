using System;
using System.IO;
using System.Linq;
using System.Text;
using System.Collections.Generic;

using Unity.DataContract;
using Unity.PackageManager.Ivy;

namespace Unity.PackageManager
{
    class InternalPackageInfo
    {
        public IvyModule module;
        public PackageInfo package;
    }

    public class Locator
    {
        // The directory structure should be in the form of
        // [Type of Module]/[Org]/[Name]/[Revision], so we shouldn't need to
        // scan more than 4 levels deep
        const int kMaxLevels = 4;

        static List<InternalPackageInfo> s_Tree = new List<InternalPackageInfo>();
        static Func<bool> s_ScanningCallback; // gets called before every recursive call to see if the caller wants to cancel
        static bool s_Cancelled;

        static string moduleFile { get { return "ivy.xml"; } }

        static bool teamcity { get { return Environment.GetEnvironmentVariable("UNITY_THISISABUILDMACHINE") == "1"; } }

        // ugh. ves_icall_System_Environment_get_Platform needs fixing.
        static bool isLinux { get { return Environment.OSVersion.Platform == PlatformID.Unix && Directory.Exists("/proc"); } }

        public static bool Completed { get; private set; }

        static string s_InstallLocation;
        public static string installLocation
        {
            get
            {
                if (s_InstallLocation == null)
                {
                    if (isLinux)
                        s_InstallLocation = Path.Combine(moduleLocation, "unity3d");
                    else
                        s_InstallLocation = Path.Combine(moduleLocation, "Unity");
                }
                return s_InstallLocation;
            }
            set { s_InstallLocation = value; }
        }


        static string m_ModuleLocation;
        public static string moduleLocation
        {
            get
            {
                if (m_ModuleLocation == null)
                {
                    // these end up on ~/.local/share and %USERPROFILE%\AppData\Local
                    m_ModuleLocation = Environment.GetFolderPath(Environment.SpecialFolder.LocalApplicationData);
                }
                return m_ModuleLocation;
            }
            set { m_ModuleLocation = value; }
        }

        public static void Scan(string editorInstallPath, string unityVersion)
        {
            Completed = false;

            PackageVersion version = new PackageVersion(unityVersion);
            s_Cancelled = false;
            s_Tree = new List<InternalPackageInfo>();

            if (Directory.Exists(editorInstallPath))
                ScanDirectory(editorInstallPath, editorInstallPath, 0);

            string scanPath = CombinePaths(installLocation, string.Format("{0}.{1}", version.major, version.minor));
            if (Directory.Exists(scanPath))
                ScanDirectory(scanPath, scanPath, 0);

            Completed = true;
        }

        public static void Scan(string[] scanPaths, string unityVersion)
        {
            Completed = false;

            PackageVersion version = new PackageVersion(unityVersion);
            s_Cancelled = false;
            s_Tree = new List<InternalPackageInfo>();

            foreach (var path in scanPaths)
                if (path != null && Directory.Exists(path))
                    ScanDirectory(path, path, 0);

            string scanPath = CombinePaths(installLocation, string.Format("{0}.{1}", version.major, version.minor));
            if (Directory.Exists(scanPath))
                ScanDirectory(scanPath, scanPath, 0);

            Completed = true;
        }

        /// <summary>
        ///
        /// </summary>
        /// <param name="editorInstallPath"></param>
        /// <param name="unityVersion"></param>
        /// <param name="progressCallback">Called before every recurse call. Return false to cancel scanning.</param>
        /// <param name="scanDoneCallback"></param>
        public static void Scan(string editorInstallPath, string unityVersion,
            Func<bool> scanInProgressCallback, Action scanDoneCallback)
        {
            s_ScanningCallback = scanInProgressCallback;
            Scan(editorInstallPath, unityVersion);
            if (scanDoneCallback != null)
                scanDoneCallback();
        }

        public static IEnumerable<PackageInfo> QueryAll()
        {
            return s_Tree.Select(t => t.package);
        }

        public static IEnumerable<object> QueryAllModules()
        {
            return s_Tree.Select(t => t.module as object);
        }

        public static PackageInfo GetPackageManager(string unityVersion)
        {
            PackageVersion version = new PackageVersion(unityVersion);
            var pm = s_Tree.Where(p => p.package.type == PackageType.PackageManager &&
                p.module.UnityVersion.IsCompatibleWith(version))
                .OrderByDescending(p => p.package.version).FirstOrDefault();
            if (pm != null)
                return pm.package;
            return null;
        }

        static bool UserWantsToContinue()
        {
            if (s_ScanningCallback != null)
            {
                s_Cancelled = !s_ScanningCallback();
                if (s_Cancelled)
                    s_ScanningCallback = null;
            }
            return !s_Cancelled;
        }

        static InternalPackageInfo Parse(string moduleFile)
        {
            if (null == moduleFile)
                return null;

            IvyModule module = IvyParser.ParseFile<IvyModule>(moduleFile);
            if (IvyParser.HasErrors)
            {
                Console.WriteLine("Error parsing module description from {0}. {1}", moduleFile, IvyParser.ErrorMessage);
                return null;
            }
            return new InternalPackageInfo() { module = module, package = module.ToPackageInfo() };
        }

        public static string CombinePaths(params string[] paths)
        {
            if (null == paths)
                throw new ArgumentNullException("paths");
            if (1 == paths.Length)
                return paths[0];

            StringBuilder builder = new StringBuilder(paths[0]);
            for (int i = 1; i < paths.Length; ++i)
                builder.AppendFormat("{0}{1}", Path.DirectorySeparatorChar, paths[i]);
            return builder.ToString();
        }

        static void ScanDirectory(string rootPath, string path, int level)
        {
            if (!UserWantsToContinue())
                return;

            if (level > kMaxLevels)
                return;

            if (path == null || rootPath == null)
                return;

            if (File.Exists(Path.Combine(path, moduleFile)))
            {
                try
                {
                    InternalPackageInfo info = Parse(CombinePaths(path, moduleFile));
                    if (info != null)
                    {
                        s_Tree.Add(info);
                        return;
                    }
                }
                catch (Exception ex)
                {
                    Console.WriteLine("Error parsing module from {0}. {1}", CombinePaths(path, moduleFile), ex.Message);
                    return;
                }
            }

            string[] dirs = Directory.GetDirectories(path);

            if (dirs.Length == 0)
                return;

            foreach (string dir in dirs)
                ScanDirectory(rootPath, dir, level + 1);
        }
    }
}
