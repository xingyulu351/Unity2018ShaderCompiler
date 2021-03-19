using System.Collections.Generic;
using System.Linq;
using System.Xml.Linq;
using MemDoc;
using ScriptRefBase;
using UnderlyingModel.MemDoc;
using UnityExampleConverter;

namespace XMLFormatter
{
    internal class XMLFormatterMemberSubsection : IXMLFormatter
    {
        private readonly SnippetConverter m_SnippetConverter;
        private readonly MemberSubSection m_MemberSubSection;
        private readonly LinkResolverWithMemSubstitutions m_LinkResolver;

        public XMLFormatterMemberSubsection(MemberSubSection memberSubsection, LinkResolverWithMemSubstitutions linkResolver, SnippetConverter snippetConverter)
        {
            m_MemberSubSection = memberSubsection;
            m_LinkResolver = linkResolver;
            m_SnippetConverter = snippetConverter;
        }

        public XElement FormattedXML()
        {
            var finalList = new List<XElement>();
            var asmSigList = m_MemberSubSection.SignatureEntryList.Where(sig => sig.Asm != null && !sig.IsOverload).Select(
                sig => new XMLFormatterAsmEntry(sig.Asm, m_LinkResolver.MemberGetter, m_LinkResolver.CurClass).FormattedXML()
            );
            finalList.AddRange(asmSigList);
            var csNoneSigList =
                m_MemberSubSection.SignatureEntryList.Where(sig => sig.Asm == null && sig.InDoc).Select(sig => DeclarationXMLfromDocOnlyName(sig.Name));
            finalList.AddRange(csNoneSigList);

            var representation = m_LinkResolver.ParseDescription(m_MemberSubSection.Summary);
            finalList.Add(MemDocOutputterXML.GetXML("Summary", representation));
            if (m_MemberSubSection.Parameters != null && m_MemberSubSection.Parameters.Count > 0)
            {
                var paramsList = m_MemberSubSection.Parameters.Select(m => new XMLFormatterParameterWithDoc(m, m_LinkResolver).FormattedXML());
                finalList.AddRange(paramsList);
            }

            if (m_MemberSubSection.ReturnDoc != null)
                finalList.Add(new XMLFormatterReturnWithDoc(m_MemberSubSection.ReturnDoc, m_LinkResolver).FormattedXML());

            var blocks = new List<XElement>();
            foreach (var block in m_MemberSubSection.TextBlocks)
            {
                var blockXML = new XElement("dummy");
                var descriptionBlock = block as DescriptionBlock;
                if (descriptionBlock != null)
                    blockXML = new XMLFormatterDescriptionBlock(descriptionBlock, m_LinkResolver).FormattedXML();

                var exampleBlock = block as ExampleBlock;
                if (exampleBlock != null)
                    blockXML = new XMLFormatterExampleBlock(exampleBlock, m_SnippetConverter, m_LinkResolver).FormattedXML();
                blocks.Add(blockXML);
            }
            finalList.AddRange(blocks);
            return new XElement("Section", finalList);
        }

        private static XElement DeclarationXMLfromDocOnlyName(string name)
        {
            return new XElement("Signature",
                new XElement("Declaration", new XAttribute("name", name), new XAttribute("type", "csnone")));
        }
    }
}
