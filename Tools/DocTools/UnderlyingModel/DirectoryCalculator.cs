using System;
using System.Collections.Generic;
using System.IO;
using System.Linq;
using Newtonsoft.Json;

namespace UnderlyingModel
{
    static public class DirectoryCalculator
    {
        ///absolute path to config file expected
        public static MemberItemDirectories GetMemberItemDirectoriesFromJson(string configFile)
        {
            var fullPath = Path.GetFullPath(configFile);

            var input = File.ReadAllText(fullPath);
            var ret = JsonConvert.DeserializeObject<MemberItemDirectories>(input);
            var lang = DocGenSettings.Instance == null ? LanguageUtil.ELanguage.en : DocGenSettings.Instance.Language;
            if (!LanguageUtil.IsEnglish(lang) && ret.Mem2TranslatedRoot != null)
                ret.Mem2files = Path.Combine(ret.Mem2TranslatedRoot, lang.ToString());
            ret.PrependRootDirectory(RootDirName);

            return ret;
        }

        public static MemberItemDirectories GetMemberItemDirectoriesAsmXml(string asmDir, string xmlDir)
        {
            var ret = new MemberItemDirectories
            {
                Assemblies = asmDir,
                Mem2files = xmlDir
            };
            ret.PrependRootDirectory(RootDirName);
            return ret;
        }

        public static string RootDirName
        {
            get
            {
                var curDir = Directory.GetCurrentDirectory();

                var curDirFwdSlash = curDir.Replace("\\", "/");

                //potential false positives are: Toolsmiths/.../Tools or /tools/doctools
                var toolsIndex = curDirFwdSlash.LastIndexOf("/Tools/", StringComparison.CurrentCultureIgnoreCase);

                if (toolsIndex < 0)
                {
                    //"Tools" not found in the current directory path, check if we are executing from the root?
                    var tryToolsDir = Path.Combine(curDir, "Tools");
                    if (Directory.Exists(tryToolsDir))
                        return curDir;
                    throw new Exception("no Tools directory found");
                }
                return curDir.Substring(0, toolsIndex);
            }
        }

        public static string DocToolsDirName
        {
            get
            {
                var toolsDir = Path.Combine(RootDirName, "Tools");
                return Path.Combine(toolsDir, "DocTools");
            }
        }

        public static string GetUserDocDirFromPlatformAndOutputDir(string platform)
        {
            return "UserDocumentation" + platform;
        }

        /// <summary>
        /// Gets the directory into which the XML / HTML is going to be built
        /// </summary>
        /// <param name="playbackEngineName">The engine name e.g. "PS4Player"</param>
        /// <returns>$root/build/temp/UserDocumentation$playbackEngine/xml</returns>
        public static string GetXmlDocBuildDirectory(string playbackEngineName)
        {
            var buildTempDir = Path.Combine(Path.Combine(RootDirName, "build"), "temp");
            var userDocDir = GetUserDocDirFromPlatformAndOutputDir(playbackEngineName);
            var platformXmlDir = Path.Combine(userDocDir, "xml");
            return Path.Combine(buildTempDir, platformXmlDir);
        }

        /// <summary>
        /// Gets the directory into which the XML / HTML is going to be built
        /// </summary>
        /// <param name="playbackEngineName">The engine name e.g. "PS4Player"</param>
        /// <returns>$root/build/UserDocumentation$playbackEngine/Documentation</returns>
        public static string GetHtmlDocBuildDirectory(string playbackEngineName)
        {
            var buildDir = Path.Combine(RootDirName, "build");
            var userDocDir = GetUserDocDirFromPlatformAndOutputDir(playbackEngineName);
            var docRoot = Path.Combine(userDocDir, "Documentation");
            return Path.Combine(buildDir, docRoot);
        }

        // Directory in which docs for this platform get stored in the repo (absolute path including root)
        // platformName is the NAME of the platform, not the relative path to it (e.g. PSVita, and not PSP2Player)
        public static string GetDocSourceDirectory(string platformName)
        {
            var platformDirectory = GetFullPlatformPath(platformName);
            if (!Directory.Exists(platformDirectory))
            {
                string msg = string.Format("{0} is an invalid directory for {1}: is your Modularize.json configured correctly?",
                    platformDirectory, platformName);
                throw new DirectoryNotFoundException(msg);
            }
            return Path.Combine(platformDirectory, DirectoryToken.ApiDocs);
        }

        //returns the absolute relativePlatformPath directory from platformName
        public static string GetFullPlatformPath(string platformName)
        {
            var platformSourceDir = DocGenSettings.Instance.PlatformsInfo.GetPlatformInfo(platformName).PlatformSourceDir;
            return Path.Combine(RootDirName, Path.Combine(DirectoryToken.PlatformDependent, platformSourceDir));
        }

        //assumes DocGenSettings.Instance.IncludeAllPlatforms is true
        public static IEnumerable<string> DefaultPlusPlatformMemFiles()
        {
            var mem2Files = Directory.GetFiles(DocGenSettings.Instance.Directories.Mem2files, DirectoryUtil.Mem2ExtensionMask, SearchOption.AllDirectories).ToList();

            var allPlatformMemFiles = AllPlatformDependentMemFiles();
            mem2Files.AddRange(allPlatformMemFiles);

            return mem2Files;
        }

        public static IEnumerable<string> AllPlatformDependentMemFiles()
        {
            var platformDependentDir = Path.Combine(RootDirName, DirectoryToken.PlatformDependent);
            var allPlaformMemFiles = Directory.GetFiles(platformDependentDir,
                DirectoryUtil.Mem2ExtensionMask, SearchOption.AllDirectories).Where(m => m.Contains(DirectoryToken.ApiDocs));
            return allPlaformMemFiles;
        }

        public static string GetFullPathWithPlatform(MemberItem member, string platformName)
        {
            if (platformName.IsEmpty() || AllPlatformsInfo.IsDefault(platformName))
            {
                var docApiDocs = Path.Combine(DirectoryToken.Documentation, DirectoryToken.ApiDocs);
                var apiDocsDir = Path.Combine(RootDirName, docApiDocs);
                return Path.Combine(apiDocsDir, member.Mem2FileName);
            }
            //this will throw a KeyNotFoundException if the relativePlatformPath is not in the map
            var platformSourceDir = DocGenSettings.Instance.PlatformsInfo.GetPlatformInfo(platformName).PlatformSourceDir;
            if (platformSourceDir.IsEmpty())
                return string.Empty;
            platformSourceDir = GetDocSourceDirectory(platformName);
            Console.WriteLine("platformSourceDir = {0}", platformSourceDir);

            var fullPath = Path.Combine(platformSourceDir, member.Mem2FileName);
            return fullPath;
        }

        //this can be expensive if we don't find the member and have to do a directory scan
        public static string GetFullPathFromMember(MemberItem member)
        {
            var fullPath = Path.Combine(DocGenSettings.Instance.Directories.Mem2files, member.Mem2FileName);
            if (!File.Exists(fullPath))
            {
                var includeAllPlatforms = DocGenSettings.Instance.IncludeAllPlatforms;
                if (includeAllPlatforms)
                {
                    fullPath = AllPlatformDependentMemFiles().FirstOrDefault(m => m.EndsWith(member.Mem2FileName));

                    if (fullPath != null)
                    {
                        return fullPath;
                    }
                    Console.WriteLine("Did not find relativePlatformPath version for {0}", member.Mem2FileName);
                    return null;
                }
            }

            return fullPath;
        }

        public static string LocalizedXmlDir(string playbackEngine)
        {
            var xmlDir = GetXmlDocBuildDirectory(playbackEngine);
            return LocalizedOutputPath(xmlDir, "");
        }

        public static string LocalizedHtmlDir(string playbackEngine)
        {
            var htmlDir = GetHtmlDocBuildDirectory(playbackEngine);
            return LocalizedOutputPath(htmlDir, "ScriptReference");
        }

        public static string LocalizedStaticFilesOutput(string playbackEngine)
        {
            var htmlDir = GetHtmlDocBuildDirectory(playbackEngine);
            return LocalizedOutputPath(htmlDir, "StaticFiles");
        }

        public static string LocalizedOutputPath(string root, string suffix)
        {
            var locale = DocGenSettings.Instance.Language;
            var localizedPath = Path.Combine(root, locale.ToString());
            localizedPath = Path.Combine(localizedPath, suffix);
            return localizedPath;
        }
    }
}
