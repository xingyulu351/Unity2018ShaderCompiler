using System;
using System.Collections.Generic;
using System.Text;
using System.Xml;

namespace UnderlyingModel
{
    class XmlHtmlWriter : XmlTextWriter
    {
        string m_OpeningElement = String.Empty;
        readonly List<string> m_SelfClosingElements = new List<string>();

        public XmlHtmlWriter(System.IO.Stream stream, Encoding en) : base(stream, en)
        {
            //// Put all the elements for which you want self closing tags in this list.
            //// Rest of the tags would be explicitely closed
            m_SelfClosingElements.AddRange(new[] {"link", "meta", "br", "img", "hr"});
        }

        public override void WriteEndElement()
        {
            if (!m_SelfClosingElements.Contains(m_OpeningElement))
            {
                WriteFullEndElement();
            }
            else
            {
                base.WriteEndElement();
            }
        }

        public override void WriteStartElement(string prefix, string localName, string ns)
        {
            base.WriteStartElement(prefix, localName, ns);
            m_OpeningElement = localName;
        }
    }
}
