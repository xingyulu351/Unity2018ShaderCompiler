using System;
using System.Linq;
using System.Xml.Linq;
using ScriptRefBase;

namespace XMLFormatter
{
    public static class MemDocOutputterXML
    {
        public static XElement GetXML(string xName, MemDocElementList repr)
        {
            return new XElement(xName, repr.m_Elements.Select(GetElementOutput));
        }

        private static XNode GetElementOutput(IMemDocElement el)
        {
            IElementOutputterXML elementOutputter = null;
            var text = el as MemDocElementPlainText;
            if (text != null)
                elementOutputter = new ElementOutputterText(text);
            var link = el as MemDocElementLink;
            if (link != null)
                elementOutputter = new ElementOutputterLink(link);
            var img = el as MemDocElementImage;
            if (img != null)
                elementOutputter = new ElementOutputterImage(img);
            var style = el as MemDocElementStyle;
            if (style != null)
                elementOutputter = new ElementOutputterStyle(style);
            var linebreak = el as MemDocElementLineBreak;
            if (linebreak != null)
                elementOutputter = new ElementOutputterLineBreak();
            if (elementOutputter == null)
                throw new NotImplementedException();
            return elementOutputter.GetXml();
        }
    }
}
