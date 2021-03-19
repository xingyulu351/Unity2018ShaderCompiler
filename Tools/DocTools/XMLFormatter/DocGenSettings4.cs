using System.IO;
using System.Xml.Linq;
using System;
using UnderlyingModel;

namespace APIDocumentationGenerator
{
    //the .NET 4.0 version of DocGenSettings for XML-related stuff
    public class DocGenSettings4
    {
        DocGenSettings4()
        {
        }

        public static DocGenSettings4 Instance
        {
            get
            {
                return instance;
            }
        }

        public XElement TranslationData { get; private set; }

        //load this as a separate call, after we initialize the language
        public void LoadTranslationData(string languageName)
        {
            var translatedDocsPath = DirectoryCalculator.RootDirName + "/Documentation/ApiDocs_Translated/";
            string path = translatedDocsPath + languageName + "/website-translation.xml";
            if (languageName != "en" && !File.Exists(path))
            {
                Console.WriteLine("File not found: " + path);
                Console.WriteLine("The scriptref website won't be localized correctly");
                path = "../../Documentation/ApiDocs_Translated/en/website-translation.xml";
            }
            else
            {
                Console.WriteLine("website-translation.xml found for {0}", languageName);
            }
            if (!File.Exists(path))
            {
                Console.WriteLine("CRITICAL: File not found: " + path);
                Environment.Exit(1);
            }
            try
            {
                TranslationData = XElement.Load(path);
            }
            catch (Exception)
            {
                Console.WriteLine("CRITICAL: File is corrupt or missing: " + path);
                Environment.Exit(1);
            }
        }

        private static readonly DocGenSettings4 instance = new DocGenSettings4();
    }
}
