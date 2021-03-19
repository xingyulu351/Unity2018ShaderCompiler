using System.IO;
using System.Xml.Linq;
using System;

using ELanguage = UnderlyingModel.LanguageUtil.ELanguage;

namespace UnderlyingModel
{
    public class DocGenSettings
    {
        DocGenSettings()
        {
            LanguageName = "en";
            Language = ELanguage.en;
            IncludeAllPlatforms = false;
            RunningFromDocBrowser = false;
            IniFilesLoaded = false;
        }

        public void LoadIniFiles()
        {
            LoadMemberDirs();
            LoadPlatformsData();
            IniFilesLoaded = true;
        }

        private void LoadPlatformsData()
        {
            m_AllPlatformsInfo = new AllPlatformsInfo();
            var modularizeJsonPath = Path.Combine(DirectoryCalculator.DocToolsDirName, "Modularize.json");

            try
            {
                m_AllPlatformsInfo.LoadFromJson(modularizeJsonPath);
            }
            catch (Exception ex)
            {
                //we cannot proceed without modularization information
                var msg = string.Format("Unable to load Modularize.json: {0}", ex.Message);
                ExitOrThrow(msg);
            }
        }

        private void LoadMemberDirs()
        {
            var memberDirsPath = Path.Combine(DirectoryCalculator.DocToolsDirName, "MemberDirs.json");
            var dirs = DirectoryCalculator.GetMemberItemDirectoriesFromJson(memberDirsPath);
            Directories = dirs;
        }

        //if we're running the DocBrowser, we want to see the error in the console
        //otherwise we want to terminate
        private void ExitOrThrow(string message)
        {
            if (RunningFromDocBrowser)
                throw new Exception(message);

            Console.WriteLine(message);
            Environment.Exit(1);
        }

        public static DocGenSettings Instance
        {
            get
            {
                return instance;
            }
        }

        public ELanguage Language { get; set; }
        public string LanguageName { get; set; }
        public bool LongForm { get; set; }
        public bool IniFilesLoaded { get; private set; }

        //include platform mem files: for the DocBrowser and (in the future) the MemValidityChecker /verify.pl
        //The doc generator should process an individual platform or default
        public bool IncludeAllPlatforms { get; set; }
        public bool RunningFromDocBrowser { get; set; }
        public MemberItemDirectories Directories { get; set; }
        private AllPlatformsInfo m_AllPlatformsInfo;

        public AllPlatformsInfo PlatformsInfo
        {
            get { return m_AllPlatformsInfo; }
        }

        private static readonly DocGenSettings instance = new DocGenSettings();
    }
}
