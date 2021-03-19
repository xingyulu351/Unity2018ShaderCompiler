using System.Collections.Generic;
using System.Linq;

namespace ScriptRefBase
{
    internal class TocOutputterJson
    {
        private readonly TocRepresentation m_TocRepresentation;
        private readonly bool m_PureJson;

        public TocOutputterJson(TocRepresentation tocRepresentation, bool pureJson)
        {
            m_TocRepresentation = tocRepresentation;
            m_PureJson = pureJson;
        }

        //System.Web.extensions not supported by Mono on .NET 4.0, so doing this manually
        private static string GetOutput(TocEntry entry, int indent)
        {
            if (entry.Children.Count == 0)
                return LinkChildrenAndTitle(indent, entry, "null");

            var nsChildren = entry.Children.Where(m => m.EntryType == TocEntryType.Namespace).ToList();
            var clChildren = entry.Children.Where(m => m.EntryType == TocEntryType.Class).ToList();
            var enChildren = entry.Children.Where(m => m.EntryType == TocEntryType.Enum).ToList();
            var atChildren = entry.Children.Where(m => m.EntryType == TocEntryType.Attribute).ToList();
            var inChildren = entry.Children.Where(m => m.EntryType == TocEntryType.Interface).ToList();
            var asChildren = entry.Children.Where(m => m.EntryType == TocEntryType.Assembly).ToList();

            var childrenJson = ChildrenJson(indent, nsChildren);
            var nsChildrenJson = JoinedWithCommas(childrenJson);
            string nsString = nsChildren.Any() ? nsChildrenJson : "";
            string clString = clChildren.Any() ? LinkChildrenAndTitle(indent, "null", GetChildrenString(indent + 1, clChildren), "Classes") : "";
            string enString = enChildren.Any() ? LinkChildrenAndTitle(indent, "null", GetChildrenString(indent + 1, enChildren), "Enumerations") : "";
            string atString = atChildren.Any() ? LinkChildrenAndTitle(indent, "null", GetChildrenString(indent + 1, atChildren), "Attributes") : "";
            string inString = inChildren.Any() ? LinkChildrenAndTitle(indent, "null", GetChildrenString(indent + 1, inChildren), "Interfaces") : "";
            string asString = asChildren.Any() ? LinkChildrenAndTitle(indent, "null", GetChildrenString(indent + 1, asChildren), "Assemblies") : "";
            var list = new List<string> {nsString, clString, inString, enString, atString, asString}.Where(m => m != "");

            var childrenString = GetFormattedChildren(indent + 1, list);
            return LinkChildrenAndTitle(indent, entry, childrenString);
        }

        private static IEnumerable<string> ChildrenJson(int indent, IEnumerable<TocEntry> children)
        {
            return children.Select(m => GetOutput(m, indent + 1)).OrderBy(m => m.ToString()).ToList();
        }

        //returns children in []
        private static string GetChildrenString(int indent, IEnumerable<TocEntry> children)
        {
            var childrenJson = ChildrenJson(indent, children);
            return GetFormattedChildren(indent, childrenJson);
        }

        private static string GetFormattedChildren(int indent, IEnumerable<string> children)
        {
            var joinedWithCommas = JoinedWithCommas(children);

            return string.Format("\n{0}[\n{0}{1}\n{0}]", GetTabString(indent), joinedWithCommas);
        }

        private static string JoinedWithCommas(IEnumerable<string> childrenJson)
        {
            return string.Join(",\n", childrenJson);
        }

        private static string GetTabString(int indent)
        {
            var tabString = "";
            for (var i = 0; i < indent; i++)
                tabString = tabString + "\t";
            return tabString;
        }

        private static string LinkChildrenAndTitle(int indent, TocEntry entry, string children)
        {
            var link = entry.EntryType == TocEntryType.Namespace ? "null" : entry.ItemName;
            var title = entry.DisplayName;
            return LinkChildrenAndTitle(indent, link, children, title);
        }

        private static string LinkChildrenAndTitle(int indent, string link, string children, string title)
        {
            var tabString = GetTabString(indent);
            return string.Format("{0}{{\"link\":\"{1}\",\"title\":\"{2}\",\"children\":{3}}}",
                tabString, link, title,
                children != "null" ? children + "\n" + tabString : children);
        }

        internal string OrderName(string displayName)
        {
            var sortIndex = 0;
            switch (displayName)
            {
                // By convention, we have ordered the TOC like this, but that order
                // was coincidental, based on the input order. We want to keep this order, so
                // this little helper function applies a sort index so it does not change.
                case "UnityEngine":
                    sortIndex = 0;
                    break;
                case "UnityEditor":
                    sortIndex = 1;
                    break;
                case "Other":
                    sortIndex = 3;
                    break;
                default:
                    sortIndex = 2;
                    break;
            }
            return $"{sortIndex}_{displayName}";
        }

        //output for root-level entries
        internal string GetOutput()
        {
            var tocRoots = m_TocRepresentation.Elements.Where(m => m.Parent == null).ToList();

            var emptyNs = tocRoots.FirstOrDefault(m => m.DisplayName == "");
            if (emptyNs != null)
                emptyNs.DisplayName = "Other";

            tocRoots = tocRoots.OrderBy(m => OrderName(m.DisplayName)).ToList();

            var jSonString = tocRoots.Select(m => GetOutput(m, 0)).ToList();

            var tocJsonString = "{\"link\":\"toc\",\"title\":\"toc\",\"children\":[" + string.Join(",", jSonString) + "]}";
            return m_PureJson ? tocJsonString : "var toc = " + tocJsonString;
        }
    }
}
