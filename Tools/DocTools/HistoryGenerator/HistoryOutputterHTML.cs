using System.Collections.Generic;
using System.Linq;
using System.Text;
using UnderlyingModel;

namespace HistoryGenerator
{
    public class HistoryOutputterHTML
    {
        private readonly IMemberGetter m_MemberItemProject;
        private readonly List<APIVersionEntry> m_Items;

        public HistoryOutputterHTML(IMemberGetter memberItemProject, List<APIVersionEntry> items)
        {
            m_MemberItemProject = memberItemProject;
            m_Items = items;
        }

        public string GetHTML()
        {
            var versionNumbers = m_Items.Select(m => m.VersionAdded.ToString()).Distinct().OrderByDescending(m => m.ToString()).ToList();
            var sb = new StringBuilder();
            var mapVersionToDiv = new Dictionary<string, string>();
            sb.AppendLine(@"<?xml version=""1.0""?>
<History>
<form>
Select Unity version:
<select id=""versionList"">");
            foreach (var version in versionNumbers)
            {
                sb.Append(@"  <option>");
                sb.Append(version);
                sb.AppendLine(@"</option>");

                var version1 = new VersionNumber(version);
                var itemsWithThisVersion = m_Items.Where(m => m.VersionAdded != null && m.VersionAdded.ToString() == version1.ToString()).ToList();
                var sbDiv = new StringBuilder();
                sbDiv.Append(@"<div class=""version"" id=");
                sbDiv.AppendFormat("\"{0}\">\n", version);
                PopulateDivsFromThisVersion(itemsWithThisVersion, sbDiv);
                sbDiv.AppendLine("</div>");
                mapVersionToDiv[version] = sbDiv.ToString();
            }
            sb.AppendLine(@"</select>");
            sb.AppendLine(@"</form>");
            foreach (var version in versionNumbers)
            {
                sb.Append(mapVersionToDiv[version]);
            }
            sb.Append(@"
</History>");
            var html = sb.ToString();
            return html;
        }

        //sorted first by namespace, then by type
        private void PopulateDivsFromThisVersion(List<APIVersionEntry> versionItems, StringBuilder sbDiv)
        {
            var allNamespaces = versionItems.Select(m => m.Namespace).Distinct().OrderBy(m => m.ToString()).ToList();
            foreach (var ns in allNamespaces)
            {
                if (ns.IsEmpty())
                    sbDiv.Append("<p class=\"nsheading\"> APIs outside namespaces: </p>\n");
                else
                    sbDiv.AppendFormat("<p class= \"nsheading\"> {0} namespace APIs: </p>\n", ns);

                var namespaceItems = versionItems.Where(m => m.Namespace == ns).ToList();
                var allAsmTypes = namespaceItems.Select(m => m.AssemblyType).Distinct().OrderBy(m => m).ToList();
                foreach (var asmType in allAsmTypes)
                {
                    var itemsForOneNamespaceOneType = namespaceItems.Where(m => m.AssemblyType == asmType).ToList();

                    bool firstTime = true;
                    foreach (var item in itemsForOneNamespaceOneType)
                    {
                        var memberID = item.MemberID;
                        var memberWithName = m_MemberItemProject.GetMember(memberID);

                        if (memberWithName != null && !memberWithName.AnyHaveDoc)
                            continue;
                        if (firstTime)
                        {
                            sbDiv.AppendFormat("<p class=\"apitype\">{0}</p>\n", AssemblyTypeUtils.PluralForm(asmType));
                            firstTime = false;
                        }
                        if (memberWithName != null)
                            sbDiv.AppendFormat("<p class=\"aliveAPI\"><a href=\"{0}\"> {1} </a></p>\n",
                                memberWithName.HtmlName + ".html",
                                memberWithName.DisplayName);
                        else
                        {
                            var memID = item.MemberID;
                            if (memID.Contains("<"))
                            {
                                memID = memID.Replace("<", "&lt;");
                                memID = memID.Replace(">", "&gt;");
                            }
                            sbDiv.AppendFormat("<p class=\"deadAPI\"> {0} </p>\n", memID);
                        }
                    }
                }
            }
        }
    }
}
