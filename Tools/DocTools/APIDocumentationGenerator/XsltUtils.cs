using System;
using System.IO;
using System.Text;
using System.Xml;
using System.Xml.Xsl;
using UnderlyingModel;

namespace APIDocumentationGenerator
{
    public class XsltUtils
    {
        internal string PlaybackEngine { get; private set; }

        public XsltUtils(string playbackEngine)
        {
            PlaybackEngine = playbackEngine;
        }

        public enum XslType
        {
            Offline,
            Online,
            SearchPage,
            SearchResults,
            History
        }

        public static XslCompiledTransform InitXslt(XslType xslType)
        {
            var myXslTrans = new XslCompiledTransform();
            var xslName = "";
            switch (xslType)
            {
                case XslType.Offline:
                    xslName = "staticMemberPage.xsl";
                    break;
                case XslType.Online:
                    xslName = "memberPage.xsl";
                    break;
                case XslType.SearchPage:
                    xslName = "searchPage.xsl";
                    break;
                case XslType.SearchResults:
                    xslName = "searchResults.xsl";
                    break;
                case XslType.History:
                    xslName = "history.xsl";
                    break;
            }
            var xsltFileName = Path.Combine(DocGenSettings.Instance.Directories.XSLTSource, xslName);
            try
            {
                myXslTrans.Load(xsltFileName,
                    new XsltSettings(enableDocumentFunction: true, enableScript: false),
                    new XmlUrlResolver());
            }
            catch (XsltException ex)
            {
                Console.WriteLine("Error loading {0}: {1} at Line {2}, Pos {3}", xsltFileName, ex.SourceUri, ex.LineNumber,
                    ex.LinePosition);
            }
            return myXslTrans;
        }

        public void XmlToHtml(string xmlFileName, XslCompiledTransform trans, string htmlFile = "")
        {
            var shortXmlName = Path.GetFileName(xmlFileName);

            var shortHtmlName = htmlFile.IsEmpty() ? shortXmlName.Replace(".xml", ".html") : htmlFile;

            var htmlDir = DirectoryCalculator.LocalizedHtmlDir(PlaybackEngine);

            DirectoryUtil.CreateDirectoryIfNeeded(htmlDir);
            htmlFile = Path.Combine(htmlDir, shortHtmlName);

            var output = new MemoryStream();
            var writer = new XmlHtmlWriter(output, Encoding.UTF8) {Formatting = Formatting.None};

            var args = new XsltArgumentList();
            args.AddParam("Lang", "", DocGenSettings.Instance.Language.ToString());

            try
            {
                trans.Transform(xmlFileName, args, writer);
            }
            catch (XmlException e)
            {
                Console.WriteLine(e.Message);
            }
            Console.WriteLine("Generating {0}", htmlFile);

            output.Seek(0, SeekOrigin.Begin);
            var reader = new StreamReader(output);

            string result = reader.ReadToEnd();
            File.WriteAllText(htmlFile, result);
        }
    }
}
