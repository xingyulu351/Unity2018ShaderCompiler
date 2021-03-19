using System.Linq;
using System.Xml.Linq;
using ScriptRefBase;

namespace XMLFormatter
{
    //this class is not used anymore, as our TOC is in .js / .json format
    internal class TocOutputterXML
    {
        private readonly TocRepresentation m_TocRepresentation;

        public TocOutputterXML(TocRepresentation tocRepresentation)
        {
            m_TocRepresentation = tocRepresentation;
        }

        private XElement GetOutput(TocEntry entry)
        {
            var topElement = new XElement("item", new XAttribute("type", entry.EntryType), new XAttribute("name", entry.ItemName));
            if (entry.Children.Count == 0)
                return topElement;
            var list = entry.Children.Select(GetOutput).ToList();

            return new XElement("items", topElement, new XElement("items", list));
        }

        //output for top-level classes
        internal XElement GetOutput()
        {
            var classesXElements = m_TocRepresentation.Elements.Select(GetOutput).ToList();

            return new XElement("TOC", classesXElements);
        }
    }
}
