using System.Collections.Generic;
using System.IO;
using System.Linq;
using System.Xml.Linq;
using UnderlyingModel;

namespace XMLFormatter
{
    public class XMLGenerator
    {
        private readonly IMemberGetter m_NewDataItemProject;
        private readonly MemberItemXMLFormatter m_MemberItemXMLFormatter;
        private readonly bool m_ConvertExamples;
        private readonly string m_PlaybackEngine;
        private readonly bool m_ResolveLinksToUndoc;

        public XMLGenerator(IMemberGetter project, bool convertExamples, bool resolveLinksToUndoc = false, string playbackEngine = "")
        {
            m_ConvertExamples = convertExamples;
            m_NewDataItemProject = project;
            m_ResolveLinksToUndoc = resolveLinksToUndoc;
            m_MemberItemXMLFormatter = new MemberItemXMLFormatter(m_ConvertExamples, m_NewDataItemProject, resolveLinksToUndoc);
            m_PlaybackEngine = playbackEngine;
        }

        public IMemberGetter MemberItemProject
        {
            get { return m_NewDataItemProject; }
        }

        //member files that merit an XML / HTML page
        public List<MemberItem> GetMembersWithPages()
        {
            var allMembers = m_NewDataItemProject.GetAllMembers();

            var undocMembersWithChildren = allMembers.Where(m => m.IsUndoc && AssemblyTypeUtils.CanHaveChildren(m.RealOrSuggestedItemType));
            //list of names of members that belong to classes/structs/enums marked as undoc
            var childrenOfUndocParents = (from mem in undocMembersWithChildren from child in mem.ChildMembers select child.ItemName).ToList();

            return allMembers.Where(m => m.AnyHaveAsm
                && !m.IsUndoc                         //don't include members explicitly marked as undoc
                && !childrenOfUndocParents.Contains(m.ItemName)                         //don't include members that belong to undoc classes
                || m.IsDocOnly)                         //include members that have no Asm equivalent ("doconly")
                .ToList();
        }

        private MemberItem GetParentableClass(MemberItem member)
        {
            MemberItem classWithAncestors = null;
            if (AssemblyTypeUtils.IsClassOrStruct(member.ItemType)
                || member.RealOrSuggestedItemType == AssemblyType.Class
                || member.RealOrSuggestedItemType == AssemblyType.Interface)
            {
                classWithAncestors = member;
            }
            else if (member.ItemType != AssemblyType.Enum)
            {
                var firstParent = m_NewDataItemProject.GetAllMembers().FirstOrDefault(m => m.ChildMembers.Contains(member));
                classWithAncestors = firstParent;
            }
            return classWithAncestors;
        }

        public XElement XMLContent(MemberItem member)
        {
            var sectionElements = m_MemberItemXMLFormatter.SectionElements(member);
            MemberItem classWithAncestors = GetParentableClass(member);

            List<MemberItem> ancestors = null;
            bool isComponent = false;
            if (classWithAncestors != null)
            {
                var ancestorGetter = new AncestorGetter(m_NewDataItemProject);
                ancestors = ancestorGetter.GetAncestors(classWithAncestors);
                if (!m_ResolveLinksToUndoc)
                    ancestors = ancestors.Where(m => m.AnyHaveDoc).ToList();
                isComponent = ancestors.Any(m => m.ItemName == "Component");
            }
            IMemberXMLFormatter ourFormatter;
            if (AssemblyTypeUtils.IsClassOrStruct(member.ItemType)
                || member.RealOrSuggestedItemType == AssemblyType.Class
                || member.RealOrSuggestedItemType == AssemblyType.Interface)
            {
                ourFormatter = new ClassPageXMLFormatter(m_MemberItemXMLFormatter.FormMemberXElement, member, ancestors, isComponent);
            }
            else if (member.ItemType == AssemblyType.Enum)
                ourFormatter = new EnumPageXMLFormatter(m_MemberItemXMLFormatter.FormMemberXElement, member);
            else if (member.ItemType == AssemblyType.Assembly)
                ourFormatter = new AssemblyPageXMLFormatter(m_MemberItemXMLFormatter.FormMemberXElement, member);
            else
                ourFormatter = new RegularMemberPageXMLFormatter(member.GetNames(), isComponent, member);

            return ourFormatter.GetXML(sectionElements);
        }

        internal List<MemberItem> GetAncestors(string memberName)
        {
            var ret = new List<MemberItem>();
            var member = m_NewDataItemProject.GetMember(memberName);
            var ancestorGeter = new AncestorGetter(m_NewDataItemProject);
            if (member != null)
                ret = ancestorGeter.GetAncestors(member);
            return ret;
        }

        public string XMLToFile(string htmlName, XElement xmlContent)
        {
            var xmlOutputDir = DirectoryCalculator.LocalizedXmlDir(m_PlaybackEngine);

            return XmlToFileAtThisDirectory(htmlName, xmlContent, xmlOutputDir);
        }

        internal static string XmlToFileAtThisDirectory(string htmlName, XElement xmlContent, string xmlOutputDir)
        {
            DirectoryUtil.CreateDirectoryIfNeeded(xmlOutputDir);
            var xmlFileName = Path.Combine(xmlOutputDir, htmlName + ".xml");

            XmlUtils4.WriteToFileWithHeader(xmlFileName, xmlContent);
            return xmlFileName;
        }
    }
}
