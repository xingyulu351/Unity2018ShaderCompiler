using System;
using System.IO;
using UnityEditor;
using Unity.DataContract;

namespace Unity.PackageManager
{
    public static class Settings
    {
        internal const string kInternal = "http://pm-test.hq.unity3d.com/repo/split/";
        internal const string kExternal = "http://update.unity3d.com/";
        static Uri defaultRepoUri = null;

        public static string downloadLocation { get; set; }
        public static string editorInstallPath { get; set; }
        public static PackageVersion unityVersion { get; set; }
        public static bool teamcity { get { return Environment.GetEnvironmentVariable("UNITY_THISISABUILDMACHINE") == "1"; } }
        public static bool inTestMode { get { return Environment.GetEnvironmentVariable("UNITY_PACKAGEMANAGERTESTMODE") == "1"; } }

        static string s_InstallLocation = null;
        public static string installLocation
        {
            get
            {
                if (s_InstallLocation == null)
                {
                    if (unityVersionPath != null)
                        s_InstallLocation = Path.Combine(Locator.installLocation, unityVersionPath);
                    else
                        s_InstallLocation = Locator.installLocation;

                    if (!Directory.Exists(s_InstallLocation))
                        Directory.CreateDirectory(s_InstallLocation);
                }
                return s_InstallLocation;
            }
            set
            {
                Locator.installLocation = value;
                s_InstallLocation = value;
                if (s_InstallLocation != null && !Directory.Exists(s_InstallLocation))
                    Directory.CreateDirectory(s_InstallLocation);
            }
        }

        static string s_CachedRepoType;
        public static string repoType
        {
            get { return s_CachedRepoType; }
            private set
            {
                if (!ThreadUtils.InMainThread)
                    return;
                EditorPrefs.SetString("pmRepositoryType", value);
                s_CachedRepoType = value;
            }
        }

        public static Uri RepoUrl
        {
            get
            {
                if (defaultRepoUri == null)
                    defaultRepoUri = new Uri(kExternal);

                if (baseRepoUrl == null)
                    return defaultRepoUri;

                Uri uri;
                if (unityVersionPath == null)
                {
                    if (!Uri.TryCreate(baseRepoUrl, UriKind.Absolute, out uri))
                        return defaultRepoUri;
                }
                else // all repos serve indexes split by unity version
                {
                    if (!Uri.TryCreate(baseRepoUrl + unityVersionPath + "/", UriKind.Absolute, out uri))
                        return defaultRepoUri;
                }
                return uri;
            }
        }

        static string s_CachedCustomRepoUrl;
        public static string baseRepoUrl
        {
            get
            {
                if (repoType != "custom")
                    return MapRepoTypeToUrl(repoType);
                string repo = s_CachedCustomRepoUrl;
                if (String.IsNullOrEmpty(repo))
                    return null;
                if (!repo.EndsWith("/"))
                    repo += "/";
                return repo;
            }
            private set
            {
                string type = MapUrlToRepoType(value);
                repoType = type;
                if (type != "custom")
                    value = null;
                else if (!ThreadUtils.InMainThread)
                    return;
                if (!String.IsNullOrEmpty(value) && !value.EndsWith("/"))
                    value += "/";
                EditorPrefs.SetString("pmCustomRepositoryUrl", value);
                s_CachedCustomRepoUrl = value;
            }
        }

        static UpdateMode s_CachedUpdateMode;
        public static UpdateMode updateMode
        {
            get { return s_CachedUpdateMode; }
            set
            {
                if (!ThreadUtils.InMainThread)
                    return;
                EditorPrefs.SetInt("pmUpdateMode", (int)value);
                s_CachedUpdateMode = value;
            }
        }

        public static string unityVersionPath
        {
            get
            {
                if (unityVersion == null)
                    return null;
                return unityVersion.major + "." + unityVersion.minor;
            }
        }

        static Settings()
        {
            downloadLocation = Path.Combine(Path.GetTempPath(), "unity");
        }

        static string MapRepoTypeToUrl(string repoType)
        {
            return MapRepoTypeToUrl(repoType, null);
        }

        static string MapRepoTypeToUrl(string repoType, string repoUrl)
        {
            switch (repoType)
            {
                case "internal":
                    return kInternal;
                case "external":
                    return kExternal;
                case "custom":
                    return repoUrl;
                default:
                    return kExternal;
            }
        }

        static string MapUrlToRepoType(string url)
        {
            if (url == kInternal)
                return "internal";
            if (url == kExternal)
                return "external";
            return "custom";
        }

        public static string SelectRepo(string repoType, string repoUrl)
        {
            repoUrl = MapRepoTypeToUrl(repoType, repoUrl);
            baseRepoUrl = repoUrl;
            return repoUrl;
        }

        public static void CacheAllSettings()
        {
            s_CachedCustomRepoUrl = EditorPrefs.GetString("pmCustomRepositoryUrl");
            s_CachedUpdateMode = (UpdateMode)EditorPrefs.GetInt("pmUpdateMode");
            s_CachedRepoType = EditorPrefs.GetString("pmRepositoryType");
        }

        public static void ResetAllSettings()
        {
            EditorPrefs.SetString("pmCustomRepositoryUrl", null);
            updateMode = UpdateMode.Automatic;
            repoType = null;
            installLocation = null;
        }
    }
}
