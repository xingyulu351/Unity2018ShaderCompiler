using System.Xml.Linq;
using ScriptRefBase;

namespace XMLFormatter
{
    public interface IElementOutputterXML
    {
        XNode GetXml();
    }

    public class ElementOutputterLineBreak : IElementOutputterXML
    {
        public XNode GetXml()
        {
            return new XElement("br");
        }
    }

    public class ElementOutputterStyle : IElementOutputterXML
    {
        private readonly MemDocElementStyle m_style;

        public ElementOutputterStyle(MemDocElementStyle style)
        {
            m_style = style;
        }

        public XNode GetXml()
        {
            return new XElement(StyleMarkup(m_style.Style), m_style.Name);
        }

        private static string StyleMarkup(StyleType selfType)
        {
            switch (selfType)
            {
                case StyleType.Bold:
                    return "b";
                case StyleType.Italic:
                    return "i";
                case StyleType.BoldItalic:
                    return "bi";
                case StyleType.MonoType:
                    return "monotype";
                case StyleType.TrueType:
                    return "tt";
                case StyleType.Var:
                    return "varname";
                default:
                    return "unknown";
            }
        }
    }

    public class ElementOutputterImage : IElementOutputterXML
    {
        private readonly MemDocElementImage m_img;

        public ElementOutputterImage(MemDocElementImage img)
        {
            m_img = img;
        }

        public XNode GetXml()
        {
            return new XElement("image", m_img.ImgName);
        }
    }

    public class ElementOutputterLink : IElementOutputterXML
    {
        private readonly MemDocElementLink m_link;

        public ElementOutputterLink(MemDocElementLink link)
        {
            m_link = link;
        }

        public XNode GetXml()
        {
            string linkText = "link";
            string linkAttribute = "ref";
            if (m_link.LinkType == LinkType.External)
            {
                linkText = "a";
                linkAttribute = "href";
            }
            var realLink = m_link.Link;
            if (m_link.LinkType == LinkType.Wiki)
            {
                realLink = "../Manual/" + realLink;
            }
            return new XElement(linkText, new XAttribute(linkAttribute, realLink), m_link.Content);
        }
    }

    public class ElementOutputterText : IElementOutputterXML
    {
        private readonly MemDocElementPlainText m_text;

        public ElementOutputterText(MemDocElementPlainText text)
        {
            m_text = text;
        }

        public XNode GetXml()
        {
            return new XCData(m_text.Text);
        }
    }
}
