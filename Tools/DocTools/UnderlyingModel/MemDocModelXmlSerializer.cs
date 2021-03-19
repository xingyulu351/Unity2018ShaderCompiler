using System;
using System.Collections.Generic;
using System.Linq;
using System.Xml;
using System.Xml.Linq;

namespace UnderlyingModel.MemDoc.Serializers
{
    public class MemDocModelXmlSerializer
    {
        public uint Version { private get; set; }

        public XElement Serialize(MemDocModel parentModel)
        {
            var childDocModels = parentModel.ChildModels;
            bool includeSignatureDeclaration = AssemblyTypeUtils.MultipleSignaturesPossibleForType(parentModel.AssemblyKind);
            var parentSectionsXml = new List<XElement>();

            foreach (var section in parentModel.SubSections)
            {
                var sectionSerializer = new MemberSectionXmlSerializer {Version = this.Version};
                var oneSectionXml = sectionSerializer.Serialize(section, includeSignatureDeclaration);
                parentSectionsXml.Add(oneSectionXml);
            }

            var childMembers = childDocModels == null
                ? null
                : childDocModels.Select(m => Serialize(m)).ToList(); //do not convert to method group, as the DocBrowser will fail to build

            var topElement = new XElement(XmlTags.Member, parentSectionsXml, childMembers);

            topElement.SetAttributeValue(XmlTags.ID, parentModel.ItemNameFromMember);
            topElement.SetAttributeValue(XmlTags.Type, parentModel.AssemblyKind.ToString());
            if (AssemblyTypeUtils.CanHaveChildren(parentModel.AssemblyKind))
                topElement.SetAttributeValue(XmlTags.Namespace, parentModel.NamespaceFromMember);

            return topElement;
        }

        //returns MemDocModel (with children, if any)
        public MemDocModel Deserialize(string fname)
        {
            var ret = new MemDocModel();
            var xmlDoc = new XmlDocument();
            xmlDoc.Load(fname);
            var topMemberXpath = string.Format("//{0}/{1}", XmlTags.Root, XmlTags.Member);
            var childMembersXPath = string.Format("//{0}/{1}/{2}", XmlTags.Root, XmlTags.Member, XmlTags.Member);
            var topLevelSectionsXPath = string.Format("//{0}/{1}/{2}", XmlTags.Root, XmlTags.Member, XmlTags.Section);

            var topMember = xmlDoc.SelectSingleNode(topMemberXpath) as XmlElement;
            if (topMember != null)
            {
                PopulateModelFromXmlAttributes(topMember, ret);
            }
            var sectionElemsXml = xmlDoc.SelectNodes(topLevelSectionsXPath); //toplevel sections

            ret.SubSections = PopulateSubsectionsFromXml(sectionElemsXml);


            var childrenXml = xmlDoc.SelectNodes(childMembersXPath); //child members xml
            if (childrenXml != null)
            {
                ret.ChildModels = new List<MemDocModel>();
                foreach (var childXml in childrenXml)
                {
                    var childModel = new MemDocModel();
                    var childElement = childXml as XmlElement;

                    PopulateModelFromXmlAttributes(childElement, childModel);
                    var sections = childElement.SelectNodes(XmlTags.Section);
                    childModel.SubSections = PopulateSubsectionsFromXml(sections);
                    ret.ChildModels.Add(childModel);
                }
            }
            return ret;
        }

        private void PopulateModelFromXmlAttributes(XmlElement xmlMember, MemDocModel model)
        {
            model.ItemNameFromMember = xmlMember.GetAttribute(XmlTags.ID);
            model.NamespaceFromMember = xmlMember.GetAttribute(XmlTags.Namespace);
            var assemblyTypeString = xmlMember.GetAttribute(XmlTags.Type);
            model.AssemblyKind = (AssemblyType)Enum.Parse(typeof(AssemblyType), assemblyTypeString);
        }

        private List<MemberSubSection> PopulateSubsectionsFromXml(XmlNodeList subsectionsXml)
        {
            var ret = new List<MemberSubSection>();
            foreach (var xmlSection in subsectionsXml)
            {
                var sectionElement = (XmlElement)xmlSection;
                var sectionSerializer = new MemberSectionXmlSerializer {Version = this.Version};
                var subsection = sectionSerializer.Deserialize(sectionElement);

                ret.Add(subsection);
            }
            return ret;
        }
    }
}
