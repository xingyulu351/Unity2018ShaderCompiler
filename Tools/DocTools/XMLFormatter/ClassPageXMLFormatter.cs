using System;
using System.Collections.Generic;
using System.Linq;
using System.Xml.Linq;
using UnderlyingModel;

namespace XMLFormatter
{
    public interface IMemberXMLFormatter
    {
        XElement GetXML(List<XElement> sectionElements);
    }

    public class ClassPageXMLFormatter : IMemberXMLFormatter
    {
        private readonly Func<MemberItem, XElement> m_FormMemberFunction;
        private readonly MemberItem m_Member;
        private readonly List<MemberItem> m_Ancestors;
        private readonly bool m_IsComponent;

        public ClassPageXMLFormatter(Func<MemberItem, XElement> formMemberFunction, MemberItem member, List<MemberItem> ancestors, bool isComponent)
        {
            m_FormMemberFunction = formMemberFunction;
            m_Member = member;
            m_Ancestors = ancestors;
            m_IsComponent = isComponent;
        }

        //gets lists of members for each category: fields / props / methods, etc.
        private ClassMemberCategories GetMemberCategoriesUnsorted(MemberItem member)
        {
            var ret = new ClassMemberCategories();
            var childMembers = member.ChildMembers.Where(m => !m.IsObsolete && m.DocModel != null && !m.DocModel.IsUndoc() && (m.AnyHaveAsm || m.IsDocOnly) && m.AnyHaveDoc).ToList();

            var fieldsAndProps = childMembers.Where(m => AssemblyTypeUtils.IsFieldOrProperty(m.RealOrSuggestedItemType)).ToList();
            ret.SetCategoryValue("StaticFieldAndProps", fieldsAndProps.Where(m => m.IsStatic && (m.FirstSignatureAsm == null || !m.FirstSignatureAsm.IsProtected)).ToList());
            ret.SetCategoryValue("RegularFieldsAndProps", fieldsAndProps.Where(m => !m.IsStatic && (m.FirstSignatureAsm == null || !m.FirstSignatureAsm.IsProtected)).ToList());
            ret.SetCategoryValue("ProtectedVars", fieldsAndProps.Where(m => m.FirstSignatureAsm != null && m.FirstSignatureAsm.IsProtected && !m.IsStatic).ToList());
            ret.SetCategoryValue("StaticProtectedVars", fieldsAndProps.Where(m => m.FirstSignatureAsm != null && m.FirstSignatureAsm.IsProtected && m.IsStatic).ToList());
            ret.SetCategoryValue("Constructors", childMembers.Where(m => m.RealOrSuggestedItemType == AssemblyType.Constructor).ToList());

            var messageMembers = childMembers.Where(m => m.IsDocOnly && !member.IsDocOnly).ToList();
            ret.SetCategoryValue("Messages", messageMembers);
            ret.SetCategoryValue("Events", childMembers.Where(m => m.RealOrSuggestedItemType == AssemblyType.Event).ToList());

            var allFunctions = childMembers.Where(m => m.RealOrSuggestedItemType == AssemblyType.Method && !messageMembers.Contains(m)).ToList();
            ret.SetCategoryValue("ProtectedFunctions", allFunctions.Where(m => !m.IsStatic && m.AreAllProtected()).ToList());
            ret.SetCategoryValue("RegularFunctions", allFunctions.Where(m => !m.IsStatic && !m.AreAllProtected()).ToList());
            ret.SetCategoryValue("StaticFunctions", allFunctions.Where(m => m.IsStatic).ToList());
            ret.SetCategoryValue("Operators", childMembers.Where(
                m => AssemblyTypeUtils.IsOperatorOrImpl(m.RealOrSuggestedItemType)).ToList());
            ret.SetCategoryValue("Delegates", childMembers.Where(m => m.RealOrSuggestedItemType == AssemblyType.Delegate).ToList());
            ret.SetCategoryValue("ImplementedInterfaces", member.ImplementedInterfaces);
            return ret;
        }

        public XElement GetXML(List<XElement> sectionElements)
        {
            var memberCategories = GetMemberCategoriesUnsorted(m_Member);
            memberCategories.Sort();
            var currentClassXML = memberCategories.GetXML(m_FormMemberFunction);

            XElement modelElement;
            if (m_Ancestors.Count == 0)
            {
                modelElement = new XElement("Model", currentClassXML, sectionElements);
            }
            else
            {
                //get signatures present in the leaf class
                var leafSigs = memberCategories.GetKnownSigs();

                var combinedMemberCategories = new ClassMemberCategories();
                foreach (var ancestor in m_Ancestors)
                    combinedMemberCategories.Append(GetMemberCategoriesUnsorted(ancestor));

                //exclude signatures present in the leaf class from the parent class member listings
                if (leafSigs.Count > 0)
                    combinedMemberCategories.CleanMembersWithKnownSigs(leafSigs);
                combinedMemberCategories.Sort();
                var combinedAncestorXML = combinedMemberCategories.GetXML(m_FormMemberFunction, eliminateConstructors: true);

                var directParent = m_Ancestors[0];

                var xElement = new XElement("BaseType", combinedAncestorXML);
                if (!directParent.IsUndoc)
                    xElement.SetAttributeValue("id", directParent.HtmlName);

                modelElement = new XElement("Model", currentClassXML, sectionElements, xElement);
                if (m_IsComponent)
                    modelElement.SetAttributeValue("isComponent", "true");
            }
            var ns = m_Member.ActualNamespace;
            if (!ns.IsEmpty())
                modelElement.SetAttributeValue("Namespace", ns);

            var parent = m_Member.ParentMember;
            if (parent != null && parent.ItemType == AssemblyType.Assembly)
            {
                if (!parent.IsObsolete && parent.DocModel != null && !parent.DocModel.IsUndoc() && (parent.AnyHaveAsm || parent.IsDocOnly) && parent.AnyHaveDoc)
                    modelElement.SetAttributeValue("Assembly", parent.ItemName);
            }

            //note that "inheritsFrom" is actually a misnomer, but it remains for legacy reasons
            modelElement.Add(new XElement("InheritsFrom", m_Member.GetNames().NamespaceAndClass));

            return modelElement;
        }
    }

    public class EnumPageXMLFormatter : IMemberXMLFormatter
    {
        private readonly MemberItem m_Member;
        private readonly Func<MemberItem, XElement> m_FormMemberFunction;

        public EnumPageXMLFormatter(Func<MemberItem, XElement> formXML, MemberItem member)
        {
            m_Member = member;
            m_FormMemberFunction = formXML;
        }

        public XElement GetXML(List<XElement> sectionElements)
        {
            var childMembers = m_Member.ChildMembers.Where(m => !m.IsObsolete && m.DocModel != null && !m.DocModel.IsUndoc() && m.AnyHaveDoc && m.AnyHaveAsm).ToList();

            var modelElement = new XElement("Model", sectionElements,
                new XElement("Vars", childMembers.Select(m_FormMemberFunction)));
            return modelElement;
        }
    }

    public class RegularMemberPageXMLFormatter : IMemberXMLFormatter
    {
        private MemberItem.MemberItemNames m_MemberItemNames;
        private readonly bool m_IsComponent;
        private readonly MemberItem m_Member;

        public RegularMemberPageXMLFormatter(MemberItem.MemberItemNames memberItemNames, bool isComponent, MemberItem member)
        {
            m_MemberItemNames = memberItemNames;
            m_IsComponent = isComponent;
            m_Member = member;
        }

        public XElement GetXML(List<XElement> sectionElements)
        {
            var memberItemNamesForDisplay = m_MemberItemNames;
            m_Member.MakeNamesGeneric(ref memberItemNamesForDisplay);
            var classDisplayName = memberItemNamesForDisplay.ClassName;
            var parentEl = new XElement("ParentClass", m_MemberItemNames.ClassName.IsEmpty() ? "" : m_MemberItemNames.NamespaceAndClass);
            if (!classDisplayName.IsEmpty() && classDisplayName != m_MemberItemNames.ClassName || !m_MemberItemNames.NamespaceName.IsEmpty())
                parentEl.SetAttributeValue("DisplayName", classDisplayName);
            var modelElement = new XElement("Model", parentEl, sectionElements);
            if (DocGenSettings.Instance.LongForm)
            {
                modelElement.SetAttributeValue("id", m_Member.HtmlName);
            }
            if (m_IsComponent)
                modelElement.SetAttributeValue("isComponent", "true");
            return modelElement;
        }
    }
}
