using System;
using System.IO;
using UnderlyingModel;

namespace APIDocumentationGenerator
{
    internal class Program
    {
        private static void Main(string[] args)
        {
            bool convertExamples = true;
            bool offline = true;
            bool generateXML = true;
            bool completeSweep = true; //false if we want a short run for debugging
            bool generateMonoDoc = false;
            bool scanDlls = true;
            string oneFile = "";
            string platform = "";
            string folder = "";

            if (args.Length == 0)
                Console.WriteLine("no commandline args provided");
            else
                Console.WriteLine("running with commandline args: ({0})", string.Join(",", args));
            for (int i = 0; i < args.Length; i++)
            {
                var arg = args[i];
                switch (arg.ToLower())
                {
                    case "-noexamples":
                        convertExamples = false;
                        break;
                    case "-nooffline": //generate offline HTML
                        offline = false;
                        break;
                    case "-noxml":
                        generateXML = false;
                        break;
                    case "-brief":
                        completeSweep = false;
                        break;
                    case "-noscandlls":
                        scanDlls = false;
                        break;
                    case "-monodoc":
                        generateMonoDoc = true;
                        break;
                    case "-platform":
                        i++;
                        if (i > args.Length)
                        {
                            Console.WriteLine("Expecting a platform after '-platform', but none found");
                            continue;
                        }
                        platform = args[i];
                        if (platform == "all")
                        {
                            DocGenSettings.Instance.IncludeAllPlatforms = true;
                        }
                        break;
                    case "-lang":
                        i++;
                        if (i >= args.Length)
                        {
                            Console.WriteLine("expecting a language argument after -lang, but none found");
                            continue;
                        }
                        DocGenSettings.Instance.Language = LanguageUtil.StringToLocale(args[i]);
                        DocGenSettings.Instance.LanguageName = args[i];
                        break;
                    case "-onefile":
                        i++;
                        if (i >= args.Length)
                        {
                            Console.WriteLine("expecting a file name argument after -onefile, but none found");
                            continue;
                        }
                        oneFile = args[i];
                        break;
                    case "-folder":
                        i++;
                        if (i >= args.Length)
                        {
                            Console.WriteLine("expecting a folder name argument after -folder, but none found");
                            continue;
                        }
                        folder = args[i];
                        break;
                    case "-longform":
                        DocGenSettings.Instance.LongForm = true;
                        break;
                }
            }

            Console.WriteLine("Running from {0}", Directory.GetCurrentDirectory());

            DocGenSettings.Instance.LoadIniFiles();
            var processor = new DirectoryProcessor(convertExamples, scanDlls, oneFile, platform, folder);
            if (generateMonoDoc)
            {
                processor.GenerateMonoDoc();
                return;
            }

            //LoadTranslationData relies on .NET 4.0 Xml parser, so we deal with it in DocGenSettings4
            DocGenSettings4.Instance.LoadTranslationData(DocGenSettings.Instance.LanguageName);
            if (oneFile.IsEmpty())
                processor.ProcessAll(offline, generateXML, completeSweep);
            else
                processor.ProcessOneFileWithChildren(oneFile);
        }
    }
}
