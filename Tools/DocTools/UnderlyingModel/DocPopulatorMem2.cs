using System;
using System.Collections.Generic;
using System.IO;
using System.Linq;
using UnderlyingModel.ItemSerializers;
using UnderlyingModel.MemDoc;

namespace UnderlyingModel
{
    public partial class NewDataItemProject
    {
        public void PopulateInfoFromMem2()
        {
            const string mem2ExtFilter = "*." + DirectoryUtil.Mem2Extension;
            string[] memFiles = Directory.GetFiles(m_MemberItemDirectories.Mem2files, mem2ExtFilter).ToArray();

            foreach (var memFile in memFiles)
            {
                // Get member name
                var memberName = Path.GetFileNameWithoutExtension(memFile);

                //scan mem2 file for submembers and populate their MemDocModels

                // Create or find MemberItem
                if (!m_MapNameToItem.ContainsKey(memberName))
                {
                    //note that if this is a CsNone, MemberItemIo.LoadDoc will switch the type to AssemblyType.CsNone
                    var thisMember = new MemberItem(memberName, AssemblyType.Unknown);

                    m_MapNameToItem[memberName] = thisMember;
                    var names = MemberItem.GetNamesFromMemberID(memberName);

                    //method or a property - add to the class, if it's already present in the symbols
                    if (!names.MemberName.IsEmpty())
                    {
                        var parentName = names.ClassName;
                        if (m_MapNameToItem.ContainsKey(parentName))
                        {
                            var parentItem = m_MapNameToItem[parentName];
                            parentItem.ChildMembers.Add(thisMember);
                        }
                    }
                    else
                        //class
                        FindChildrenAndAdoptThem(memberName);
                }
            }
        }

        public List<MemberItem> GetMem2AbleItems(bool onlyWithDoc)
        {
            var allItems = GetAllMembers();

            //anything of type Class, Struct, or Enum
            var mem2Able = allItems.Where(m => IsMem2Able(m)).ToList();

            //anything of another type that doesn't have a parent
            var topLevelExtras = GetTopLevelExtras(allItems, mem2Able);
            mem2Able.AddRange(topLevelExtras);
            if (onlyWithDoc)
                mem2Able = mem2Able.Where(m => m.AnyHaveDoc || m.IsUndoc).ToList();
            return mem2Able;
        }

        private static bool IsMem2Able(MemberItem m)
        {
            return AssemblyTypeUtils.CanHaveChildren(m.RealOrSuggestedItemType)
                || m.RealOrSuggestedItemType == AssemblyType.Interface;
        }

        //anything that is not a class, that could have its own mem2 file
        private static IEnumerable<MemberItem> GetTopLevelExtras(IEnumerable<MemberItem> allItems, IEnumerable<MemberItem> mem2Able)
        {
            var topLevelDelegates = allItems.Where(m => AssemblyTypeUtils.CanBeStandalone(m.RealOrSuggestedItemType)).ToList();
            foreach (var cl in mem2Able)
            {
                var delegatesOrUnknownsOfThisClass =
                    cl.ChildMembers.Where(m => AssemblyTypeUtils.CanBeStandalone(m.RealOrSuggestedItemType));
                foreach (var del in delegatesOrUnknownsOfThisClass)
                    topLevelDelegates.Remove(del);
            }
            return topLevelDelegates;
        }

        //if platform is not specified, we assume default, which is Documentation/ApiDocs
        //if a platform is specified, we look in PlatformDependent/<platform>/ApiDocs
        //if we're running with IncludeAllPlatforms option, first we pick up Documentation/ApiDocs and then all of the platform dependent docs
        public void LoadMem2Files(string platform = "")
        {
            var ser = new MemberItemIOMem2(this);

            string mem2Dir = platform != "" && !DocGenSettings.Instance.IncludeAllPlatforms
                ? DirectoryCalculator.GetDocSourceDirectory(platform)
                : m_MemberItemDirectories.Mem2files;
            if (!Directory.Exists(mem2Dir))
            {
                Console.WriteLine("directory {0} does not exist", mem2Dir);
                return;
            }

            var mem2Files = Directory.GetFiles(mem2Dir, DirectoryUtil.Mem2ExtensionMask, SearchOption.AllDirectories).ToList();

            if (DocGenSettings.Instance.IncludeAllPlatforms)
            {
                var allPlatformMemFiles = DirectoryCalculator.AllPlatformDependentMemFiles();
                mem2Files.AddRange(allPlatformMemFiles);
            }

            foreach (var file in mem2Files)
                ItemFromMem2File(ser, file);

            foreach (var mem in m_MapNameToItem.Values)
            {
                if (mem.AnyHaveDoc || mem.IsUndoc)
                    continue;
                mem.DocModel = new MemDocModel();
                mem.PopulateDocModelBitsFromMemberItem();
                mem.UpdateFlags();
            }
        }

        public void ItemFromMem2File(MemberItemIOMem2 ser, string file)
        {
            var model = ser.LoadDocFromPath(file);
            var itemName = model.ItemNameFromMember;
            var itemWithThatName = GetMember(itemName);
            if (itemWithThatName == null && model.IsDocOnly())
            {
                itemWithThatName = new MemberItem(itemName, model.AssemblyKind);
                m_MapNameToItem[itemName] = itemWithThatName;
            }
            if (itemWithThatName != null)
                ser.CrossPolinateModelAndItem(itemWithThatName, model);
        }

        //returns the item that maps to the mem2 file that contains data for this member
        //for classes and enums - it's the item itself
        //for enum values, properties, and methods - it's the containing class
        public MemberItem GetParentMember(MemberItem member)
        {
            if (AssemblyTypeUtils.CanHaveChildren(member.ItemType))
                return member;
            var firstParent = GetAllMembers().FirstOrDefault(m => m.ChildMembers.Select(n => n.ItemName).Contains(member.ItemName));
            return firstParent ?? member;
        }
    }
}
