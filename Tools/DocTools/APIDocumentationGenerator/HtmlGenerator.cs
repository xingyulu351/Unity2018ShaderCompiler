using System;
using System.IO;
using System.Threading.Tasks;
using System.Linq;
using MemSyntax;
using UnderlyingModel;
using XMLFormatter;

namespace APIDocumentationGenerator
{
    public class HtmlGenerator
    {
        private readonly SearchIndexRepresentation m_SearchIndex;
        //private LanguageUtil.ELanguage m_eLang;
        //private readonly string m_PlaybackEngine;
        private readonly XsltUtils m_XsltUtils;

        public HtmlGenerator(SearchIndexRepresentation searchIndex, XsltUtils utils)
        {
            m_SearchIndex = searchIndex;
            m_XsltUtils = utils;
        }

        //TODO: separate Html / index writing and static file copying
        public void GenerateHtml()
        {
            var localizedXMLOutput = DirectoryCalculator.LocalizedXmlDir(m_XsltUtils.PlaybackEngine);
            var localizedHtmlOutput = DirectoryCalculator.LocalizedHtmlDir(m_XsltUtils.PlaybackEngine);
            var mainXslTrans = XsltUtils.InitXslt(XsltUtils.XslType.Offline);
            var searchPageTrans = XsltUtils.InitXslt(XsltUtils.XslType.SearchPage);
            var historyTrans = XsltUtils.InitXslt(XsltUtils.XslType.History);
            Console.WriteLine("Writing search index ...");

            DirectoryUtil.CreateDirectoryIfNeeded(localizedHtmlOutput);
            var indexJsOutputter = new SearchIndexJSOutput(m_SearchIndex);
            var indexJsonOutputter = new SearchIndexJsonOutput(m_SearchIndex);

            var staticFilesOutput = DirectoryCalculator.LocalizedStaticFilesOutput(m_XsltUtils.PlaybackEngine);

            indexJsOutputter.WriteSearchIndexTo(localizedHtmlOutput + "/docdata/index.js");
            indexJsonOutputter.WriteSearchIndexTo(localizedHtmlOutput + "/docdata/index.json");

            Console.WriteLine("Injecting localized website terms ...");
            var landingXmlOutput = Path.Combine(staticFilesOutput, "landing.xml");
            XmlUtils4.InjectRootElementPropertiesIntoFile(landingXmlOutput);
            m_XsltUtils.XmlToHtml(landingXmlOutput, mainXslTrans, htmlFile: "index.html");

            var searchXmlOutput = Path.Combine(staticFilesOutput, "search.xml");
            XmlUtils4.InjectRootElementPropertiesIntoFile(searchXmlOutput);
            m_XsltUtils.XmlToHtml(searchXmlOutput, searchPageTrans, htmlFile: "30_search.html");

            if (LanguageUtil.IsEnglish(DocGenSettings.Instance.Language))
            {
                var historyXmlOutput = Path.Combine(staticFilesOutput, "history.xml");
                XmlUtils4.InjectRootElementPropertiesIntoFile(historyXmlOutput);
                m_XsltUtils.XmlToHtml(historyXmlOutput, historyTrans, htmlFile: "40_history.html");
            }

            // generate page containing the feedback form
            var feedbackXmlOutput = Path.Combine(staticFilesOutput, "leaveFeedback.xml");
            XmlUtils4.InjectRootElementPropertiesIntoFile(feedbackXmlOutput);
            m_XsltUtils.XmlToHtml(feedbackXmlOutput, mainXslTrans, htmlFile: "LeaveFeedback.html");

            Console.WriteLine("Generating HTML...");
            DateTime originalTime = DateTime.Now;

            convertXmlToHTML(localizedXMLOutput, mainXslTrans);

            TimeSpan timeSpan = DateTime.Now - originalTime;
            Console.WriteLine("HTML generation took {0}", XmlUtils4.FormatTimeSpan(timeSpan, false));
        }

        private void convertXmlToHTML(string localizedXMLOutput, System.Xml.Xsl.XslCompiledTransform mainXslTrans)
        {
            string[] xml_files = Directory.GetFiles(localizedXMLOutput).Where(file => Path.GetExtension(file) == ".xml").ToArray();


            if (xml_files.Length <= 0)
            {
                foreach (var file in Directory.GetFiles(localizedXMLOutput).Where(file => Path.GetExtension(file) == ".xml"))
                    m_XsltUtils.XmlToHtml(file, mainXslTrans);
            }
            else
            {
                Task[] tasks = new Task[xml_files.Length];
                for (int i = 0; i < xml_files.Length; i++)
                {
                    int task_index = i;
                    tasks[i] = Task.Factory.StartNew(() =>
                    {
                        m_XsltUtils.XmlToHtml(xml_files[task_index], mainXslTrans);
                    });
                }

                Task.WaitAll(tasks);
            }
        }
    }
}
