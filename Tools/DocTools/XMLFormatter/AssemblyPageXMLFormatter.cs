using System;
using System.Collections.Generic;
using System.Linq;
using System.Xml.Linq;
using UnderlyingModel;

namespace XMLFormatter
{
    public class AssemblyPageXMLFormatter : IMemberXMLFormatter
    {
        private readonly Func<MemberItem, XElement> m_FormMemberFunction;
        private readonly MemberItem m_Member;

        public AssemblyPageXMLFormatter(Func<MemberItem, XElement> formMemberFunction, MemberItem member)
        {
            m_FormMemberFunction = formMemberFunction;
            m_Member = member;
        }

        public XElement GetXML(List<XElement> sectionElements)
        {
            var childMembers = m_Member.ChildMembers
                .Where(m => !m.IsObsolete && m.DocModel != null && !m.DocModel.IsUndoc() && (m.AnyHaveAsm || m.IsDocOnly) && m.AnyHaveDoc)
                .OrderBy(m => m.DisplayName)
                .ToList();
            var currentClassXML = new List<XElement>
            {
                new XElement("Classes", childMembers.Where(m => m.ItemType == AssemblyType.Class).Select(m_FormMemberFunction)),
                new XElement("Structs", childMembers.Where(m => m.ItemType == AssemblyType.Struct).Select(m_FormMemberFunction)),
                new XElement("Enumerations", childMembers.Where(m => m.ItemType == AssemblyType.Enum).Select(m_FormMemberFunction))
            };

            XElement modelElement;
            {
                modelElement = new XElement("Model", currentClassXML, sectionElements);
            }

            var ns = m_Member.ActualNamespace;
            if (!ns.IsEmpty())
                modelElement.SetAttributeValue("Namespace", ns);

            var assemblies = m_Member.GetFullDllPaths().Select(x => System.IO.Path.GetFileName(x));
            if (assemblies.Count() > 0)
            {
                // We want the module assembly, if possible, so skip the non-module UnityEngine.dll.
                var assembly = assemblies.FirstOrDefault(x => x != "UnityEngine.dll") ?? assemblies.First();
                modelElement.SetAttributeValue("Assembly", assembly);
            }

            return modelElement;
        }
    }
}
