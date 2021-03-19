using System.IO;
using System.Text;
using System.Xml;
using System.Xml.Linq;

namespace UnderlyingModel
{
    public static class XmlUtils
    {
        public static XDocument DocumentFromXElement(XElement el)
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
            XDocument sourceXMLWithHeader = DocumentFromXElement(sourceXML);

            var builder = new StringBuilder();
            using (var writer = new Utf8StringWriter(builder))
            {
                sourceXMLWithHeader.Save(writer);
            }
            if (!builder.ToString().EndsWith("\n"))
                builder.AppendUnixLine();
            var correctOutput = builder.ToString().WithUnixLineEndings();
            File.WriteAllText(fullName, correctOutput);
        }
    }
}
