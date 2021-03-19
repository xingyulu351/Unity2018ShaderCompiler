using System;
using System.IO;
using System.Text;
using System.Xml;
using System.Xml.Linq;
using System.Xml.Xsl;
using APIDocumentationGenerator;
using UnderlyingModel;

namespace XMLFormatter
{
    //same as XmlUtils, only using System.Xml from 4.0
    public static class XmlUtils4
    {
        private static XDocument DocumentFromXElement(XElement el)
        {
            return new XDocument(
                new XDeclaration("1.0", "utf-8", "yes"),
                el);
        }

        public static string EscapeSpecialChars(string original)
        {
            var ret = original.Trim();
            ret = ret.Replace("'", "\'"); //single quotes escaped
            ret = ret.Replace("\"", "\\\""); //double quotes escaped
            ret = ret.Replace(":\\", ":\\\\"); //backslashes preceded by columns escaped
            return ret;
        }

        public static string XmlEscape(string unescaped)
        {
            var doc = new XmlDocument();
            XmlNode node = doc.CreateElement("root");
            node.InnerText = unescaped;
            return node.InnerXml;
        }

        public static string XmlUnescape(string escaped)
        {
            var doc = new XmlDocument();
            XmlNode node = doc.CreateElement("root");
            try
            {
                node.InnerXml = escaped;
                return node.InnerText;
            }
            catch (XmlException)
            {
                return escaped;
            }
        }

        //xmlSource => stringReader => xmlReader => xmlWriter => stringWriter => output
        public static string XmlToHtml(XElement xmlSource, XslCompiledTransform xsltTransform)
        {
            string output;
            using (var stringReader = new StringReader(xmlSource.ToString()))
            {
                using (var xmlReader = XmlReader.Create(stringReader))
                {
                    using (var stringWriter = new StringWriter())
                    {
                        var settings = new XmlWriterSettings
                        {
                            OmitXmlDeclaration = true,
                            ConformanceLevel = ConformanceLevel.Fragment
                        };
                        using (var xmlWriter = XmlWriter.Create(stringWriter, settings))
                        {
                            xsltTransform.Transform(xmlReader, xmlWriter);
                            output = stringWriter.ToString();
                            output = EscapeSpecialChars(output);
                        }
                    }
                }
            }
            return output;
        }

        public sealed class Utf8StringWriter : StringWriter
        {
            public override Encoding Encoding
            {
                get { return Encoding.UTF8; }
            }

            public Utf8StringWriter(StringBuilder sb) : base(sb) {}
        }

        public static void WriteToFileWithHeader(string fullName, XElement sourceXML)
        {
            var correctOutput = XmlToStringWithHeader(sourceXML);
            File.WriteAllText(fullName, correctOutput);
        }

        public static string XmlToStringWithHeader(XElement sourceXML)
        {
            XDocument sourceXMLWithHeader = DocumentFromXElement(sourceXML);

            var builder = new StringBuilder();
            using (var writer = new Utf8StringWriter(builder))
            {
                sourceXMLWithHeader.Save(writer);
            }
            if (!builder.ToString().EndsWith("\n"))
                builder.AppendUnixLine();
            var correctOutput = builder.ToString().WithUnixLineEndings();
            return correctOutput;
        }

        public static string FormatTimeSpan(TimeSpan timeSpan, bool longFormat = true)
        {
            return longFormat
                ? String.Format("{0}:{1:00}:{2:00}", timeSpan.Hours, timeSpan.Minutes, timeSpan.Seconds)
                : String.Format("{0:00}:{1:00.0}", timeSpan.Minutes, timeSpan.Seconds);
        }

        public static void InjectRootElementPropertiesIntoFile(string landingXmlOutput)
        {
            XElement xElement = XElement.Load(landingXmlOutput);
            SetRootElementProperties(xElement);
            xElement.Save(landingXmlOutput);
        }

        public static void SetRootElementProperties(XElement rootElement)
        {
            rootElement.SetAttributeValue("LongForm", DocGenSettings.Instance.LongForm);
            //for including localized texts in scripting reference builds
            var translationData = DocGenSettings4.Instance.TranslationData;
            if (translationData != null)
            {
                rootElement.Add(translationData);
            }
        }
    }
}
