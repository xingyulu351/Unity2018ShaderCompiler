using System;
using System.Collections.Generic;
using System.IO;
using System.Linq;
using UnderlyingModel;

namespace ScriptRefBase
{
    public class TocPopulator
    {
        public static TocRepresentation MakeTableOfContents(IMemberGetter newDataItemProject, string platform = "")
        {
            List<MemberItem> tocMembers = newDataItemProject.GetAllMembers().Where(item => TypeShownInToc(item) && item.DocModel != null && !item.IsUndoc).ToList();
            if (!platform.IsEmpty())
            {
                tocMembers = tocMembers.Where(m => m.AnyHaveDoc && !m.IsEditorSpritesDataUtility).ToList();
                if (tocMembers.Count == 0)
                    Console.WriteLine("WARNING: Found no documented members for platform {0}", platform);
            }
            var tocRep = GenerateNamespaceHierarchy(newDataItemProject);
            RepresentationFromTree(tocMembers, tocRep);
            return tocRep;
        }

        public static void WriteTableOfContents(TocRepresentation tocRep, string outputDir)
        {
            var tocOutputter = new TocOutputterJson(tocRep, pureJson: false);
            var contents = tocOutputter.GetOutput();
            File.WriteAllText(Path.Combine(outputDir, "toc.js"), contents);
            var tocOutputterJson = new TocOutputterJson(tocRep, pureJson: true);
            contents = tocOutputterJson.GetOutput();
            File.WriteAllText(Path.Combine(outputDir, "toc.json"), contents);
        }

        private static bool TypeShownInToc(MemberItem item)
        {
            return item.RealOrSuggestedItemType == AssemblyType.Class
                || item.RealOrSuggestedItemType == AssemblyType.Struct
                || item.RealOrSuggestedItemType == AssemblyType.Enum
                || item.RealOrSuggestedItemType == AssemblyType.Interface
                || item.RealOrSuggestedItemType == AssemblyType.Assembly;
        }

        private static void RepresentationFromTree(IEnumerable<MemberItem> tocMembers, TocRepresentation tocRepr)
        {
            foreach (var mem in tocMembers)
            {
                var ns = mem.ActualNamespace;
                if (CecilHelpers.IsNamespaceInternal(ns) && mem.ItemType != AssemblyType.Assembly)
                    continue;

                //we include the class in the TOC if it's not documented but has nested classes / enums with docs
                if (!mem.AnyHaveDoc && !mem.AnyNestedChildrenHaveDoc)
                    continue;
                var tocEntryType = TocEntryFromAsmType(mem);

                //for nested classes, add child to parent class
                var containerMem = mem.ContainerClassMember;
                if (mem.ItemType == AssemblyType.Assembly)
                {
                    // Assemblies don't have a namespace. But we will treat the first item of the name as a namespace, so that e.g. UnityEngine.*
                    // modules appear under UnityEngine, where they fit contextually.
                    tocRepr.AddChild(mem.ItemName.Split('.')[0], mem.ItemName, mem.DisplayName, tocEntryType);
                }
                else if (containerMem != null)
                {
                    tocRepr.AddChild(containerMem.ItemName, mem.ItemName, mem.DisplayName, tocEntryType);
                }
                //for non-nested classes, add class as a child of its namespace node
                else
                {
                    var memNamespaceParent = tocRepr.Elements.FirstOrDefault(m => m.ItemName == ns);
                    if (memNamespaceParent == null)
                        continue;
                    tocRepr.AddChild(memNamespaceParent.ItemName, mem.ItemName, mem.DisplayName, tocEntryType);
                }
            }
        }

        private static TocRepresentation GenerateNamespaceHierarchy(IMemberGetter newDataItemProject)
        {
            var docMembers = newDataItemProject.GetAllMembers().Where(m => m.AnyHaveDoc);
            var nsList = docMembers.Select(m => m.ActualNamespace).Distinct().ToList();

            return GetTocRepresentationFromNsList(nsList);
        }

        internal static TocRepresentation GetTocRepresentationFromNsList(List<string> nsList)
        {
            var tree = new TocRepresentation();
            var nsSet = AssignNestednessLevelToNamespaces(nsList);
            int maxNestednessLevel = nsSet.Count > 0 ? nsSet.Values.Max() : 0;

            for (int level = 1; level <= maxNestednessLevel; level++)
            {
                var namespacesAtThisLevel = nsSet.Where(m => m.Value == level).Select(m => m.Key).ToList();
                if (level == 1)
                {
                    foreach (var ns in namespacesAtThisLevel)
                        tree.AddBaseElement(ns, ns, TocEntryType.Namespace);
                }
                else
                {
                    var namespacesAtPreviousLevel = nsSet.Where(m => !m.Key.IsEmpty() && m.Value == level - 1).Select(m => m.Key).ToList();

                    foreach (var ns in namespacesAtThisLevel)
                    {
                        string potentialParent = namespacesAtPreviousLevel.FirstOrDefault(m => ns.StartsWith($"{m}."));

                        if (!string.IsNullOrEmpty(potentialParent))
                            tree.AddChild(potentialParent, ns, ns, TocEntryType.Namespace);
                    }
                }
            }

            return tree;
        }

        //takes a list of namespaces and generates a dictionary of namespaces to their nestedness level
        //if a namespace is not present directly, but is needed as a parent, that namespace will be added to the dictionary
        internal static Dictionary<string, int> AssignNestednessLevelToNamespaces(List<string> nsList)
        {
            var nsSet = new Dictionary<string, int>();
            foreach (var item in nsList)
            {
                var nsParts = item.Split('.');
                var nestednessLevel = nsParts.Length;
                nsSet[item] = nestednessLevel;

                //add namespace for parent node, if not already present
                while (nestednessLevel > 1)
                {
                    var partsWithoutLast = nsParts.Take(nsParts.Length - 1);
                    var parentNs = String.Join(".", partsWithoutLast);
                    if (!nsList.Contains(parentNs) && !nsSet.ContainsKey(parentNs))
                        nsSet[parentNs] = nestednessLevel - 1;
                    nestednessLevel--;
                    nsParts = partsWithoutLast.ToArray();
                }
            }
            return nsSet;
        }

        private static TocEntryType TocEntryFromAsmType(MemberItem mem)
        {
            TocEntryType entryType;
            switch (mem.RealOrSuggestedItemType)
            {
                case AssemblyType.Class:
                case AssemblyType.Struct:
                    if (mem.FirstCecilType != null && CecilHelpers.IsAttribute(mem.FirstCecilType) ||
                        mem.ItemName.EndsWith("Attribute"))
                    {
                        entryType = TocEntryType.Attribute;
                    }
                    else
                    {
                        entryType = TocEntryType.Class;
                    }
                    break;
                case AssemblyType.Interface:
                    entryType = TocEntryType.Interface;
                    break;
                case AssemblyType.Enum:
                    entryType = TocEntryType.Enum;
                    break;
                case AssemblyType.Assembly:
                    entryType = TocEntryType.Assembly;
                    break;
                default:
                    entryType = TocEntryType.Unknown;
                    break;
            }
            return entryType;
        }
    }
}
