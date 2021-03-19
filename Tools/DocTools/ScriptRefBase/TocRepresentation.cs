using System.Collections.Generic;
using System.Linq;

namespace ScriptRefBase
{
    public class TocEntry
    {
        public string ItemName { get; private set; } //aka HTML name
        public string DisplayName {get; set; } //shortened name for display
        public TocEntry Parent { get; internal set; }
        public List<TocEntry> Children { get; internal set; }

        public TocEntryType EntryType { get; internal set; }

        public TocEntry(string itemName, string displayName, TocEntryType asmType)
        {
            Children = new List<TocEntry>();
            ItemName = itemName;
            DisplayName = displayName;
            EntryType = asmType;
        }
    }

    public class TocRepresentation
    {
        public readonly List<TocEntry> Elements;

        public TocRepresentation()
        {
            Elements = new List<TocEntry>();
        }

        //add an enum, an attribute or a class
        public void AddBaseElement(string itemName, string displayName, TocEntryType asmType)
        {
            var tocEntry = GetOrCreate(itemName, displayName, asmType);
            tocEntry.EntryType = asmType;
        }

        public void AddChild(string parent, string childItemName, string childDisplayName, TocEntryType entryType)
        {
            var parentTocEntry = GetOrCreate(parent, parent, entryType);

            var tocEntry = new TocEntry(childItemName, childDisplayName, entryType)
            {
                Parent = parentTocEntry
            };
            if (!Elements.Contains(tocEntry))
                Elements.Add(tocEntry);
            parentTocEntry.Children.Add(tocEntry);
        }

        private TocEntry GetOrCreate(string itemName, string displayName,  TocEntryType entryType)
        {
            var tocEntry = Elements.FirstOrDefault(m => m.ItemName == itemName);
            if (tocEntry == null)
            {
                tocEntry = new TocEntry(itemName, displayName, entryType);
                Elements.Add(tocEntry);
            }
            return tocEntry;
        }
    }
}
