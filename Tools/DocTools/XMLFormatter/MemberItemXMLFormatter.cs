using System.Collections.Generic;
using System.Linq;
using System.Xml.Linq;
using ScriptRefBase;
using UnderlyingModel;
using UnderlyingModel.MemDoc;
using UnityExampleConverter;

namespace XMLFormatter
{
    public class MemberItemXMLFormatter
    {
        private readonly SnippetConverter m_SnippetConverter;
        private readonly LinkResolverWithMemSubstitutions m_LinkResolver;
        private string m_CurrentClass;

        public MemberItemXMLFormatter(bool convertExamples, IMemberGetter mg, bool resolveLinksToMemsWithNoDocs)
        {
            m_SnippetConverter = new SnippetConverter(convertExamples);
            m_LinkResolver = new LinkResolverWithMemSubstitutions(mg) {ResolveLinkWithoutDoc = resolveLinksToMemsWithNoDocs};
        }

        public List<XElement> SectionElements(MemberItem member)
        {
            var names = member.GetNames();
            m_CurrentClass = names.ClassName;

            m_CurrentClass = member.MakeGenericNameDisplayable(m_CurrentClass);
            m_LinkResolver.CurClass = m_CurrentClass;

            var memberSubSections = new List<MemberSubSection>();
            if (member.DocModel != null)
                memberSubSections = member.DocModel.SubSections.Where(m => !m.IsUndoc).ToList();
            var sectionElements = memberSubSections.Select(m => new XMLFormatterMemberSubsection(m, m_LinkResolver, m_SnippetConverter).FormattedXML()).ToList();
            return sectionElements;
        }

        public XElement FormMemberXElement(MemberItem child)
        {
            var displayName = child.SignatureDisplayName();
            XElement xmlOutputFromSummary = GetXMLFromDoc(XmlUtils4.XmlUnescape(child.DocModel.Summary), "summary");
            return new XElement("member",
                new XAttribute("id", child.HtmlName),
                new XElement("name", displayName),
                xmlOutputFromSummary);
        }

        private XElement GetXMLFromDoc(string docString, string markup)
        {
            var representation = m_LinkResolver.ParseDescription(docString);
            return MemDocOutputterXML.GetXML(markup, representation);
        }

        public static XElement GetXMLOutputFromDocWithClass(string curClass, string docString, string markup)
        {
            var linkResolver = new LinkResolver {CurClass = curClass};
            var representation = linkResolver.ParseDescription(docString);
            return MemDocOutputterXML.GetXML(markup, representation);
        }

        public static XElement GetXMLOutputFromDocWithClassPlainText(string curClass, string docString, string markup)
        {
            var linkResolver = new LinkResolver { CurClass = curClass };
            var representation = linkResolver.ParseDescription(docString);
            return MemDocOutputterXML.GetXML(markup, LinkResolver.MakeSimpleList(representation.PlainText));
        }
    }
}
