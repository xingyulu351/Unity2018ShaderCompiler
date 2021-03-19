using System;
using System.Collections.Generic;
using System.IO;
using System.Text;
using System.Text.RegularExpressions;
using System.Xml;
using System.Xml.Linq;
using System.Xml.Schema;
using UnderlyingModel;
using System.Linq;

namespace MemValidityChecker
{
    class Program
    {
        [Flags]
        enum SuccessCode
        {
            Valid = 0,
            MissingDocs = 1,
            XmlErrors = 2,
            XsdErrors = 4,
            DuplicateFiles = 8,
        }

        //note: Mono is not particularly good at returning the exit code to the system caller.
        //We check for error log file presence instead
        static int Main(string[] args)
        {
            bool cleanupWhiteList = args.Length > 0 && args[0].Contains("cleanup");
            bool doVerifyDuplicateFiles = args.Length > 0 && args[0].Contains("duplicate");

            var successCode = SuccessCode.Valid;
            const string errorFile = "ApiDocErrors.txt";

            var sb = new StringBuilder();

            var dirs = DirectoryCalculator.GetMemberItemDirectoriesFromJson("MemberDirs.json");
            DocGenSettings.Instance.Directories = dirs;
            DocGenSettings.Instance.IncludeAllPlatforms = true;

            var mem2Files = DirectoryCalculator.DefaultPlusPlatformMemFiles().ToList();

            if (doVerifyDuplicateFiles)
                successCode |= VerifyDuplicateFiles(mem2Files, sb);
            successCode |= ValidateXsd(mem2Files, sb, errorFile);
            if (successCode != SuccessCode.Valid)
                return -1;

            //check all of the assemblies together
            var newDataItemProject = new NewDataItemProject(scanForDlls: false);
            newDataItemProject.ReloadAllProjectData();

            //magic methods "inherited" from  ScriptableObject or MonoBehaviour should not be flagged as undocumented
            var magicMethodNames = newDataItemProject.GetAllMembers()
                .Where(m => m.IsDocOnly).Select(m => m.GetNames())
                .Where(m => m.ClassName == "ScriptableObject" || m.ClassName == "MonoBehaviour")
                .Select(m => m.MemberName).Distinct().ToArray();
            var apiNoDoc = newDataItemProject.GetFilteredMembersForProgramming(Presence.AllPresent, Presence.AllAbsent);
            var reallyUndoc = apiNoDoc.Where(m => !m.IsUndoc && !m.IsObsolete).ToList();
            var whiteList = new HashSet<string>();

            var apiWithDocsItemNames = newDataItemProject.GetFilteredMembersForProgramming(Presence.AllPresent, Presence.AllPresent).Select(m => m.ItemName).ToList();
            var whiteListDocumentedEntries = new StringBuilder();
            var whiteListMissingApi = new StringBuilder();
            var cleanWhiteList = new StringBuilder();
            try
            {
                var splitRx = new Regex(@"^\s*([^;#\s]+)"); // support ; and # style comments, and auto-trim whitespace

                var lines = File.ReadAllLines("MissingDocsWhiteList.txt");
                foreach (var line in lines)
                {
                    var match = splitRx.Match(line);
                    if (match.Success)
                    {
                        var itemName = match.Groups[1].Value;
                        whiteList.Add(itemName);
                        if (!cleanupWhiteList) continue;
                        if (apiWithDocsItemNames.Contains(itemName))
                        {
                            whiteListDocumentedEntries.AppendUnixLine(line);
                        }
                        else if (!newDataItemProject.ContainsMember(itemName) && !itemName.StartsWith("JetBrains"))
                        {
                            Console.WriteLine("member {0} in whiteList but doesn't exist", itemName);
                            whiteListMissingApi.AppendUnixLine(line);
                        }
                        else
                        {
                            cleanWhiteList.AppendUnixLine(line);
                        }
                    }
                    else if (cleanupWhiteList)
                    {
                        //preserve empty lines and comments for the clean whitelist
                        cleanWhiteList.AppendUnixLine(line);
                    }
                }
            }
            catch (FileNotFoundException e)
            {
                Console.WriteLine(e.Message + " Try running this command from the directory Tools/DocTools");
                Environment.Exit(2);
            }
            if (cleanupWhiteList)
            {
                File.WriteAllText("MissingDocsWhiteList.txt", cleanWhiteList.ToString());
                File.WriteAllText("MissingDocsNoLongerMissing.txt", whiteListDocumentedEntries.ToString());
                File.WriteAllText("ApiNoLongerPresent.txt", whiteListMissingApi.ToString());
            }

            int numMissingDocs = 0;
            foreach (var really in reallyUndoc)
            {
                if (whiteList.Contains(really.ItemName) || whiteList.Contains(really.ActualNamespace))
                    continue;

                var memberName = really.GetNames().MemberName;
                if (magicMethodNames.Contains(memberName))
                    continue;
                var firstSignatureAsm = really.FirstSignatureAsm;
                if (firstSignatureAsm == null || firstSignatureAsm.IsOverride)
                    continue;
                numMissingDocs++;
                sb.AppendLine(really.ItemName);
                successCode |= SuccessCode.MissingDocs;
            }

            if (successCode != SuccessCode.Valid)
            {
                Console.WriteLine("Error = new APIs missing docs for {0} items:", numMissingDocs);
                Console.WriteLine(sb);
                File.WriteAllText(errorFile, sb.ToString());
            }
            else
            {
                Console.WriteLine("missing doc scan complete - all new APIs have docs");
            }

            return (int)successCode;
        }

        private static SuccessCode ValidateXsd(List<string> mem2Files, StringBuilder sb, string errorFile)
        {
            var localSuccessCode = SuccessCode.Valid;
            var schemas = new XmlSchemaSet();
            try
            {
                schemas.Add(null, "../../Tools/DocTools/ApiDocs.xsd");
            }
            catch (XmlSchemaException ex)
            {
                Console.WriteLine("ApiDocs.xsd (line {0}, pos {1}): {2}", ex.LineNumber, ex.LinePosition, ex.Message);
                Environment.Exit(1);
            }
            foreach (var xmlFile in mem2Files)
            {
                var mgr = new XmlNamespaceManager(new NameTable());
                mgr.AddNamespace("", "http://www.unity3d.com/2014/06/03/ApiDocs");
                var ctx = new XmlParserContext(null, mgr, null, XmlSpace.Default);

                string msg = "";
                using (var reader = XmlReader.Create(xmlFile, null, ctx))
                {
                    var localErrorCode = SuccessCode.Valid;
                    string errorMessage;
                    try
                    {
                        var doc = XDocument.Load(reader);

                        doc.Validate(schemas, (o, e) =>
                        {
                            localErrorCode = SuccessCode.XsdErrors;
                            msg += e.Message + Environment.NewLine;
                        });
                        if (msg != "")
                        {
                            errorMessage = string.Format("{0} contains XML which doesn't match our XSD verification schema: {1} ", xmlFile, msg);
                            sb.AppendLine(errorMessage);
                        }
                    }
                    catch (XmlException ex)
                    {
                        localErrorCode = SuccessCode.XmlErrors;
                        errorMessage = string.Format("{0} contains invalid XML: {1} ", xmlFile, ex.Message);
                        sb.AppendLine(errorMessage);
                    }
                    if (localErrorCode != SuccessCode.Valid)
                    {
                        localSuccessCode |= localErrorCode;
                    }
                }
            }

            if (localSuccessCode != SuccessCode.Valid)
            {
                Console.WriteLine("ApiDocs contain XML or XSD errors");
                Console.WriteLine(sb.ToString());

                File.WriteAllText(errorFile, sb.ToString());
                return SuccessCode.XsdErrors;
            }
            return SuccessCode.Valid;
        }

        private static SuccessCode VerifyDuplicateFiles(List<string> mem2Files, StringBuilder sb)
        {
            var successCode = SuccessCode.Valid;

            var namesToPaths = new Dictionary<string, string>();
            var numDuplicateFiles = 0;
            foreach (var xmlFile in mem2Files)
            {
                var lastIndex = xmlFile.LastIndexOfAny(new[] {'/', '\\'});
                var shortName = xmlFile.Substring(lastIndex + 1);
                if (namesToPaths.ContainsKey(shortName))
                {
                    numDuplicateFiles++;
                    successCode |= SuccessCode.DuplicateFiles;
                    sb.AppendLine("duplicate entries exist for " + shortName);
                    sb.AppendLine("\t" + namesToPaths[shortName]);
                    sb.AppendLine("\t" + xmlFile);
                }
                namesToPaths[shortName] = xmlFile;
            }
            if (successCode != SuccessCode.Valid)
            {
                if (numDuplicateFiles > 0)
                    Console.WriteLine("{0} duplicate files found", numDuplicateFiles);
            }
            return successCode;
        }
    }
}
