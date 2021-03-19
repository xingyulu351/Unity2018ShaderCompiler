using System;
using System.Collections.Generic;
using System.IO;
using System.Linq;
using System.Reflection;
using System.Xml.Linq;
using HistoryGenerator;
using ScriptRefBase;
using UnderlyingModel;
using XMLFormatter;

namespace APIDocumentationGenerator
{
    public partial class DirectoryProcessor
    {
        private readonly HtmlGenerator m_HTMLGenerator;
        private readonly XMLGenerator m_XMLGenerator;
        private readonly MonoDocGenerator m_MonoDocGenerator;
        private readonly SearchPopulator m_SearchPopulator;
        private readonly string m_Platform;
        private readonly string m_PlaybackEngine;
        private XsltUtils m_XsltUtils;
        private NewDataItemProject m_Project;
        public DirectoryProcessor(bool convertExamples, bool scanDlls, string oneFile, string platform, string folder = "")
        {
            m_SearchPopulator = new SearchPopulator();
            NewDataItemProject project;

            m_PlaybackEngine = "";
            if (oneFile != "") //one-file initialization
            {
                project = new NewDataItemProject(false);
                project.ReloadProjectDataMinimal(oneFile, platform);
            }
            //one-platform initialization
            else if (platform != "" && !DocGenSettings.Instance.IncludeAllPlatforms)
            {
                project = new NewDataItemProject(DocGenSettings.Instance.Directories, scanDlls, folder);
                if (!DocGenSettings.Instance.IncludeAllPlatforms)
                {
                    var thisPlatformInfo = new OnePlatformInfo();
                    try
                    {
                        thisPlatformInfo = DocGenSettings.Instance.PlatformsInfo.GetPlatformInfo(platform);
                    }
                    catch (KeyNotFoundException)
                    {
                        Console.WriteLine("No platform info was found for {0}: Did you for get to add your platform's info to Modularize.json?", platform);
                        Environment.Exit(1);
                    }
                    project.ReloadAllProjectData(platform);
                    m_PlaybackEngine = thisPlatformInfo.BuildOutputDir;
                }
            }
            else //full initialization
            {
                project = new NewDataItemProject(DocGenSettings.Instance.Directories, scanDlls, folder);
                project.ReloadAllProjectData();
            }
            m_Project = project;
            m_Platform = platform;
            m_XMLGenerator = new XMLGenerator(new MemberGetter(project), convertExamples, false, m_PlaybackEngine);
            m_XsltUtils = new XsltUtils(m_PlaybackEngine);
            m_MonoDocGenerator = new MonoDocGenerator(new MemberGetter(project));
            m_HTMLGenerator = new HtmlGenerator(m_SearchPopulator.SearchIndex, m_XsltUtils);
        }

        protected static string ExecutingAssemblyLocation
        {
            get { return Path.GetDirectoryName(Assembly.GetExecutingAssembly().Location); }
        }

        public XMLGenerator XMLGenerator
        {
            get { return m_XMLGenerator; }
        }

        private static XElement CreateRootElement()
        {
            XElement rootElement = new XElement("Root");
            XmlUtils4.SetRootElementProperties(rootElement);
            return rootElement;
        }

        private static XElement CreateRootElementWithContent(XElement xmlContent)
        {
            XElement rootElement = new XElement("Root", xmlContent);
            XmlUtils4.SetRootElementProperties(rootElement);
            return rootElement;
        }

        public void GetExistingXml()
        {
            List<MemberItem> membersWithDoc = m_XMLGenerator.GetMembersWithPages();

            //special case handling for UnityEditor.Sprites which had a name collision with "UnityEngine.Sprites"
            membersWithDoc = membersWithDoc.Where(m => !m.IsEditorSpritesDataUtility).ToList();

            var orderedMembers = membersWithDoc.OrderBy(m => m.ItemName);
            int i = 0;
            foreach (var member in orderedMembers)
            {
                if (!member.AnyHaveDoc)
                    continue;
                var currentXmlFileName = member.HtmlName + ".xml";
                var xmlDir = DirectoryCalculator.LocalizedXmlDir("");
                currentXmlFileName = Path.Combine(xmlDir, currentXmlFileName);
                Console.WriteLine("Loading XML for {0}", member.ItemName);
                XElement xmlContent = XElement.Load(currentXmlFileName);
                m_SearchPopulator.SearchIndexFromXmlContent(xmlContent, i);
                m_SearchPopulator.AppendPotentialSearchResult(member, MemberItemXMLFormatter.GetXMLOutputFromDocWithClassPlainText);
                i++;
            }
        }

        public void ProcessXML(bool fullSweep, bool clearDir)
        {
            var originalTime = DateTime.Now;

            List<MemberItem> membersWithDoc = m_XMLGenerator.GetMembersWithPages();

            //special case handling for UnityEditor.Sprites which had a name collision with "UnityEngine.Sprites"
            membersWithDoc = membersWithDoc.Where(m => !m.IsEditorSpritesDataUtility).ToList();
            if (!fullSweep)
            {
                const char startLetter = 'F';
                Console.WriteLine("brief parse: only parsing members starting with {0}", startLetter);
                membersWithDoc = membersWithDoc.Where(m => m.ItemName[0] == startLetter).ToList();
            }
            TimeSpan timeSpan;
            if (clearDir)
            {
                Console.WriteLine("Deleting XML files...");
                var outputDir = DirectoryCalculator.LocalizedXmlDir(DocGenSettings.Instance.PlatformsInfo.GetBuildOutputDir(m_Platform));
                DirectoryUtil.DeleteAllFiles(outputDir);
                DirectoryUtil.CreateDirectoryIfNeeded(outputDir);
            }
            var orderedMembers = membersWithDoc.OrderBy(m => m.ItemName);

            Console.WriteLine("Generating XML ...");

            int i = 0;
            string currentClassName = String.Empty; // for use when generating "LongForm" version
            string currentHtmlName = String.Empty;
            XElement rootElement = null;

            foreach (var member in orderedMembers)
            {
                if (!member.AnyHaveDoc)
                    continue;

                string props = "";
                props += (member.FirstSignatureAsm != null) ? " " + member.FirstSignatureAsm.EntryType : "";
                props += (member.IsObsolete) ? " obsolete" : "";
                props += (member.IsStatic) ? " static" : "";
                props += (member.IsProtected) ? " protected" : "";

                Console.WriteLine("Generating XML for {0} which is a :{1}", member.ItemName, props);
                var xmlContent = m_XMLGenerator.XMLContent(member);

                if ((rootElement == null) || !DocGenSettings.Instance.LongForm || (currentClassName != member.GetNames().ClassName))
                {
                    if (rootElement != null)
                        // save XML for previous class
                        m_XMLGenerator.XMLToFile(currentHtmlName, rootElement);

                    if (DocGenSettings.Instance.LongForm)
                        currentClassName = member.GetNames().ClassName; // for performance reason, only get this if generating "LongForm" pages

                    currentHtmlName = member.HtmlName;
                    rootElement = CreateRootElement();
                }

                rootElement.Add(xmlContent);

                var obsoleteInfo = member.ObsoleteInfo;
                if (member.IsObsolete)
                {
                    //Console.WriteLine("obsolete: " + member.HtmlName);
                    //obsolete error is treated like removed
                    xmlContent.SetAttributeValue("status", obsoleteInfo.IsError ? "removed" : "obsolete");
                    xmlContent.SetAttributeValue("obsoleteText", "");
                    if (LanguageUtil.IsEnglish(DocGenSettings.Instance.Language))
                        xmlContent.SetAttributeValue("version", HistoryUtils.CurrentVersionString);
                }
                if (member.ItemType == AssemblyType.Field && member.FirstSignatureAsm != null && member.FirstSignatureAsm.IsProtected)
                {
                    Console.WriteLine("Field {0} is a protected field", member.ItemName);
                    //xmlContent.SetAttributeValue("scope", "protected");
                }

                m_SearchPopulator.SearchIndexFromXmlContent(xmlContent, i);
                m_SearchPopulator.AppendPotentialSearchResult(member, MemberItemXMLFormatter.GetXMLOutputFromDocWithClassPlainText);
                i++;
            }
            if (rootElement != null)
                // save XML for last class
                m_XMLGenerator.XMLToFile(currentHtmlName, rootElement);

            timeSpan = DateTime.Now - originalTime;

            Console.WriteLine("(sequential) XML generation took {0} ({1} items)", XmlUtils4.FormatTimeSpan(timeSpan), i);
        }

        public void ProcessAll(bool offlineHTML, bool generateXML, bool fullSweep)
        {
            var staticFilesOutput = DirectoryCalculator.LocalizedStaticFilesOutput(m_PlaybackEngine);
            var localizedHtmlOutput = DirectoryCalculator.LocalizedHtmlDir(m_PlaybackEngine);
            var docDataDir = Path.Combine(localizedHtmlOutput, "docdata");
            DirectoryUtil.CreateDirectoryIfNeeded(docDataDir);

            //copies the directory to the output
            DirectoryUtil.CopyDirectoryFromScratch(DocGenSettings.Instance.Directories.StaticFilesSource, staticFilesOutput);

            if (generateXML)
                ProcessXML(fullSweep, true);
            else
                GetExistingXml();

            var tocRepr = TocPopulator.MakeTableOfContents(m_XMLGenerator.MemberItemProject, m_Platform);
            TocPopulator.WriteTableOfContents(tocRepr, docDataDir); //-> creates toc.json

            if (LanguageUtil.IsEnglish(DocGenSettings.Instance.Language))
            {
                var previousJsonFile = Path.Combine(DirectoryCalculator.DocToolsDirName, "MemberDirsPrevious.json");
                var dirs = DirectoryCalculator.GetMemberItemDirectoriesFromJson(previousJsonFile);
                var oldItemProject = new NewDataItemProject(dirs, false) {Mem2Source = true};
                oldItemProject.ReloadAllProjectData();

                var oldGetter = new MemberGetter(oldItemProject);
                var newGetter = m_XMLGenerator.MemberItemProject;
                ProcessPreviousVersion(newGetter, oldGetter);
                HistoryUtils.MakeHistory(newGetter, oldGetter, staticFilesOutput); //-> creates history.xml
            }
            if (offlineHTML)
                m_HTMLGenerator.GenerateHtml();
        }

        private void ProcessPreviousVersion(IMemberGetter newProject, IMemberGetter oldProject)
        {
            //don't waste time converting examples for old .mem files
            var xmlGen = new XMLGenerator(oldProject, false);

            var removedMembersWithDoc = oldProject.GetAllMembers()
                .Where(oldItem => {
                    var newItem = newProject.GetMember(oldItem.ItemName);
                    return oldItem.AnyHaveAsm && oldItem.AnyHaveDoc && !(newItem != null && newItem.AnyHaveAsm && newItem.AnyHaveDoc);
                }).ToList();

            foreach (var member in removedMembersWithDoc)
            {
                var xmlContent = xmlGen.XMLContent(member);
                xmlContent.SetAttributeValue("status", "removed");
                if (LanguageUtil.IsEnglish(DocGenSettings.Instance.Language))
                    xmlContent.SetAttributeValue("version", HistoryUtils.CurrentVersionString);
                xmlGen.XMLToFile(member.HtmlName, xmlContent);
                //not populating search index here
            }
        }

        public void ProcessOneFileWithChildren(string oneFile)
        {
            var staticFilesOutput = DirectoryCalculator.LocalizedStaticFilesOutput(m_PlaybackEngine);
            //copies the directory to the output
            if (!Directory.Exists(staticFilesOutput))
            {
                Directory.CreateDirectory(staticFilesOutput);
                DirectoryUtil.CopyDirectory(DocGenSettings.Instance.Directories.StaticFilesSource, staticFilesOutput);
            }
            var oneMember = m_XMLGenerator.MemberItemProject.GetMember(oneFile);
            if (oneMember == null)
            {
                Console.WriteLine("could not find file {0}", oneFile);
                return;
            }
            if (DocGenSettings.Instance.LongForm)
            {
                XElement rootElement = CreateRootElement();
                AppendFileRecurse(oneMember, rootElement);

                var xmlFileName = m_XMLGenerator.XMLToFile(oneMember.HtmlName, rootElement);
                var trans = XsltUtils.InitXslt(XsltUtils.XslType.Offline);
                m_XsltUtils.XmlToHtml(xmlFileName, trans);
            }
            else
            {
                ProcessOneFile(oneMember);
                foreach (var child in oneMember.ChildMembers)
                    ProcessOneFile(child);
            }
        }

        private void ProcessOneFile(MemberItem oneMember)
        {
            var xmlContent = m_XMLGenerator.XMLContent(oneMember);

            XElement rootElement = CreateRootElementWithContent(xmlContent);
            var xmlFileName = m_XMLGenerator.XMLToFile(oneMember.HtmlName, rootElement);
            var trans = XsltUtils.InitXslt(XsltUtils.XslType.Offline);
            m_XsltUtils.XmlToHtml(xmlFileName, trans);
        }

        private void AppendFileRecurse(MemberItem oneMember, XElement pageXMLContent)
        {
            var xmlContent = m_XMLGenerator.XMLContent(oneMember);
            pageXMLContent.Add(xmlContent);
            foreach (var child in oneMember.ChildMembers)
                AppendFileRecurse(child, pageXMLContent);
        }

        public void GenerateMonoDoc()
        {
            List<MemberItem> membersWithDoc = m_XMLGenerator.GetMembersWithPages();

            var orderedMembers = membersWithDoc.OrderBy(m => m.ItemName);

            Console.WriteLine("Generating Monodoc XML ...");

            var dllsMemberElements = new Dictionary<string, XElement>();

            //Generate headers
            var moduleNames = m_Project.GetAsmModuleNames();
            foreach (var module in moduleNames)
            {
                var assemblyName = Path.GetFileNameWithoutExtension(module);
                var membersElem = new XElement("members");
                membersElem.Add(new XElement("assembly",
                    new XElement("name", assemblyName)));
                dllsMemberElements[module] = membersElem;
            }

            //add the elements to each Dll (sometimes one element belongs to multiple Dlls)
            foreach (var member in orderedMembers)
            {
                if (!member.AnyHaveDoc)
                    continue;

                var xmlContentList = m_MonoDocGenerator.XmlContentList(member);
                foreach (var xmlContent in xmlContentList)
                {
                    var myDlls = member.GetFullDllPaths();
                    foreach (var myDll in myDlls)
                    {
                        var membersElem = dllsMemberElements[myDll];
                        if (membersElem != null)
                        {
                            membersElem.Add(xmlContent);
                        }
                        else
                        {
                            Console.WriteLine("No XML entries exist for {0}", myDll);
                        }
                    }
                }
            }

            Console.WriteLine("Writing Monodoc XML files to disk ...");
            //write the xml files
            foreach (var dllPath in dllsMemberElements.Keys)
            {
                var membersElem = dllsMemberElements[dllPath];
                var docElem = new XElement("doc", membersElem);
                var xmlPath = dllPath.Replace(".dll", ".xml");
                Console.WriteLine(xmlPath);
                XmlUtils4.WriteToFileWithHeader(xmlPath, docElem);
            }
        }
    }
}
