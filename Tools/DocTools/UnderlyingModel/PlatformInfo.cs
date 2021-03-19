using System;
using System.Collections.Generic;
using System.IO;
using System.Linq;
using Newtonsoft.Json;
using Newtonsoft.Json.Linq;

namespace UnderlyingModel
{
    public class AllPlatformsInfo
    {
        private readonly Dictionary<string, OnePlatformInfo> m_Platforms;

        public IEnumerable<string> Keys
        {
            get { return m_Platforms.Keys; }
        }

        public OnePlatformInfo GetPlatformInfo(string platform)
        {
            if (m_Platforms.ContainsKey(platform))
            {
                return m_Platforms[platform];
            }

            string msg = string.Format("unrecognized platform {0}", platform);
            throw new KeyNotFoundException(msg);
        }

        public static bool IsDefault(string platformName)
        {
            return platformName.StartsWith("Default");
        }

        public static bool IsUnknown(string platformName)
        {
            return platformName == "Unknown";
        }

        public string GetPlatformNameFromPath(string path)
        {
            if (string.IsNullOrEmpty(path))
                return "Unknown";
            path = path.Replace("\\", "/");
            if (path.Contains(DirectoryToken.Documentation + "/" + DirectoryToken.ApiDocs))
                return "Default";
            foreach (var plat in m_Platforms.Keys)
            {
                if (path.Contains(DirectoryToken.PlatformDependent + "/" + m_Platforms[plat].PlatformSourceDir))
                    return plat;
            }
            return "Unknown";
        }

        public AllPlatformsInfo()
        {
            m_Platforms = new Dictionary<string, OnePlatformInfo>();
        }

        public string GetBuildOutputDir(string platform)
        {
            if (string.IsNullOrEmpty(platform))
                return string.Empty;
            if (!m_Platforms.ContainsKey(platform))
            {
                Console.WriteLine("unrecognized platform {0}", platform);
                return string.Empty;
            }
            return m_Platforms[platform].BuildOutputDir;
        }

        //throws on error - catch higher up
        internal void LoadFromJson(string configFile)
        {
            var fullPath = Path.GetFullPath(configFile);

            var input = File.ReadAllText(fullPath);
            object obj = JsonConvert.DeserializeObject(input);

            var jObject = obj as JObject;

            foreach (var topPlatform in jObject)
            {
                var vals = topPlatform.Value;
                foreach (var plat in vals)
                {
                    var platProp = plat as JProperty;
                    var platformName = platProp.Name;
                    var thisPlatform = new OnePlatformInfo();

                    var nsList = plat.Values("Namespaces");
                    var prefixList = plat.Values("Prefixes");
                    var plBase = plat.Values("PlatformDependent");
                    var buildOutput = plat.Values("BuildOutput");

                    thisPlatform.InitFromJObjects(nsList, prefixList, plBase, buildOutput);
                    m_Platforms[platformName] = thisPlatform;
                }
            }
        }
    }

    public class OnePlatformInfo
    {
        public List<string> PrefixList { get; private set; }
        public List<string> NamespaceList { get; private set; }
        public string PlatformSourceDir { get; private set; }
        public string BuildOutputDir { get; private set; }

        public OnePlatformInfo()
        {
            NamespaceList = new List<string>();
            PrefixList = new List<string>();
            PlatformSourceDir = "";
            BuildOutputDir = "";
        }

        internal void InitFromJObjects(IEnumerable<JToken> nsList,
            IEnumerable<JToken> prefixList,
            IEnumerable<JToken> plBase,
            IEnumerable<JToken> buildOutput
        )
        {
            NamespaceList = nsList.Values().Select(m => m.ToString()).ToList();
            PrefixList = prefixList.Values().Select(m => m.ToString()).ToList();

            PlatformSourceDir = plBase.First().ToString();
            BuildOutputDir = PlatformSourceDir;
            if (buildOutput != null && buildOutput.Any())
                BuildOutputDir = buildOutput.First().ToString();
        }
    }
}
