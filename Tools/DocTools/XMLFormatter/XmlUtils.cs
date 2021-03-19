using System;
using System.IO;
using System.Text;
using System.Xml;
using System.Xml.Linq;
using System.Xml.Xsl;

namespace ScriptRefBase
{
    public static class XmlUtils
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

        public static string FormatTimeSpan(TimeSpan timeSpan, bool longFormat = true)
        {
            return longFormat
                ? string.Format("{0}:{1:00}:{2:00}", timeSpan.Hours, timeSpan.Minutes, timeSpan.Seconds)
                : string.Format("{0:00}:{1:00.0}", timeSpan.Minutes, timeSpan.Seconds);
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

        public static void WriteToFileWithHeader(string tocXMLFileName, XElement tocXML)
        {
            var tocXMLWithHeader = DocumentFromXElement(tocXML);

            var builder = new StringBuilder();
            using (TextWriter writer = new Utf8StringWriter(builder))
                tocXMLWithHeader.Save(writer);
            File.WriteAllText(tocXMLFileName, builder.ToString());
        }
    }
}
