using System.Collections.Generic;
using System.Xml.Linq;
using MemDoc;
using ScriptRefBase;
using UnderlyingModel;
using UnderlyingModel.MemDoc;

namespace XMLFormatter
{
    internal interface IXMLFormatter
    {
        XElement FormattedXML();
    }

    internal class XMLFormatterDescriptionBlock : IXMLFormatter
    {
        private readonly DescriptionBlock m_Block;
        private readonly LinkResolver m_LinkResolver;

        public XMLFormatterDescriptionBlock(DescriptionBlock block, LinkResolver lr)
        {
            m_Block = block;
            m_LinkResolver = lr;
        }

        public XElement FormattedXML()
        {
            var representation = m_LinkResolver.ParseDescription(m_Block.Text);
            return MemDocOutputterXML.GetXML("Description", representation);
        }
    }

    internal class XMLFormatterParamElement : IXMLFormatter
    {
        private readonly ParamElement m_ParamElement;
        private readonly IMemberGetter m_Getter;

        public XMLFormatterParamElement(ParamElement paramElement, IMemberGetter linkResolver)
        {
            m_ParamElement = paramElement;
            m_Getter = linkResolver;
        }

        public XElement FormattedXML()
        {
            var paramType = m_ParamElement.Type;
            var paramTypeItem = m_Getter.GetClosestNonObsoleteMember(paramType);
            if (paramTypeItem != null)
                paramType = paramTypeItem.ItemName;
            var attList = new List<XAttribute>
            {
                new XAttribute("name", m_ParamElement.Name),
                new XAttribute("type", paramType)
            };
            var member = m_Getter.GetMember(paramType);
            if (member != null && !member.IsUndoc)
                attList.Add(new XAttribute("hasLink", true));
            if (!m_ParamElement.Modifiers.IsEmpty())
            {
                string modifier = m_ParamElement.Modifiers;
                attList.Add(new XAttribute("modifier", modifier));
            }
            if (!m_ParamElement.DefaultValue.IsEmpty())
                attList.Add(new XAttribute("default", m_ParamElement.DefaultValue));
            return new XElement("ParamElement", attList);
        }
    }


    internal class XMLFormatterReturnWithDoc : IXMLFormatter
    {
        private readonly ReturnWithDoc m_ReturnWithDoc;
        private readonly LinkResolver m_LinkResolver;

        public XMLFormatterReturnWithDoc(ReturnWithDoc returnWithDoc, LinkResolver linkResolver)
        {
            m_ReturnWithDoc = returnWithDoc;
            m_LinkResolver = linkResolver;
        }

        public XElement FormattedXML()
        {
            var representation = m_LinkResolver.ParseDescription(m_ReturnWithDoc.Doc);
            return new XElement("ReturnWithDoc", new XAttribute("type", m_ReturnWithDoc.ReturnType ?? "void"),
                MemDocOutputterXML.GetXML("doc", representation));
        }
    }

    internal class XMLFormatterParameterWithDoc : IXMLFormatter
    {
        private readonly ParameterWithDoc m_ParameterWithDoc;
        private readonly LinkResolver m_LinkResolver;

        public XMLFormatterParameterWithDoc(ParameterWithDoc parameterWithDoc, LinkResolver linkResolver)
        {
            m_ParameterWithDoc = parameterWithDoc;
            m_LinkResolver = linkResolver;
        }

        public XElement FormattedXML()
        {
            var representation = m_LinkResolver.ParseDescription(m_ParameterWithDoc.Doc);
            return new XElement("ParamWithDoc", new XElement("name", m_ParameterWithDoc.Name),
                MemDocOutputterXML.GetXML("doc", representation));
        }
    }
}
