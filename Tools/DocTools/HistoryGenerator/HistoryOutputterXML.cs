using System.Collections.Generic;
using System.Linq;
using System.Xml.Linq;
using ScriptRefBase;
using UnderlyingModel;
using XMLFormatter;

namespace HistoryGenerator
{
    public class HistoryOutputterXML
    {
        private readonly IMemberGetter m_MemberItemProject;
        private readonly IMemberGetter m_OldItemProject;
        private readonly List<APIVersionEntry> m_Items;

        public HistoryOutputterXML(IMemberGetter memberItemProject, IMemberGetter oldItemProject, List<APIVersionEntry> items)
        {
            m_MemberItemProject = memberItemProject;
            m_OldItemProject = oldItemProject;
            m_Items = items;
        }

        public XElement GetXML()
        {
            var list = new List<XElement>();

            var sortedItems = m_Items.OrderByDescending(m => m.VersionLatestChange)
                .ThenBy(m => m.ApiStatus)
                .ThenBy(m => m.Namespace)
                .ThenBy(m => m.MemberID);
            foreach (var item in sortedItems)
            {
                var memberWithName = m_MemberItemProject.GetMember(item.MemberID)
                    ?? m_OldItemProject.GetMember(item.MemberID);
                if (memberWithName != null && (!memberWithName.AnyHaveDoc || memberWithName.IsUndoc))
                    continue;

                bool aliveAPI = memberWithName != null && memberWithName.AnyHaveAsm;
                IMemDocElement el;
                if (aliveAPI)
                    el = new MemDocElementLink(memberWithName.HtmlName, memberWithName.DisplayName);
                else
                {
                    var nicerId = item.MemberID.Replace("._", ".");
                    el = new MemDocElementPlainText(nicerId);
                }
                var api = MemDocOutputterXML.GetXML("api", new MemDocElementList(el));

                var elem = new XElement("item", api);
                elem.SetAttributeValue("memberID", item.MemberID);
                elem.SetAttributeValue("alive", aliveAPI);
                elem.SetAttributeValue("type", item.AssemblyType.ToString());
                elem.SetAttributeValue("isStatic", item.IsStatic);
                elem.SetAttributeValue("namespace", item.Namespace);
                elem.SetAttributeValue("versionAdded", item.VersionAdded);
                if (item.VersionObsolete != null)
                    elem.SetAttributeValue("versionObsolete", item.VersionObsolete);
                if (item.VersionRemoved != null)
                    elem.SetAttributeValue("versionRemoved", item.VersionRemoved);

                list.Add(elem);
            }
            return new XElement("items", list);
        }
    }
}
