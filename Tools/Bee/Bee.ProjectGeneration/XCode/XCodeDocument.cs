using System;
using System.Collections.Generic;
using System.IO;
using System.Linq;
using NiceIO;
using Unity.BuildSystem.NativeProgramSupport;
using Unity.BuildTools;
using Unity.TinyProfiling;

namespace Bee.ProjectGeneration.XCode
{
    class XCodeDocument
    {
        readonly List<XCodeNode> _allNodes = new List<XCodeNode>();
        readonly Dictionary<NPath, XCodeNode> _files = new Dictionary<NPath, XCodeNode>();
        public Dictionary<NPath, XCodeNode> Groups = new Dictionary<NPath, XCodeNode>();
        public XCodeNode RootNode { get; set; }
        const string kAbsolute = "\"<absolute>\"";

        public override string ToString()
        {
            var root = new XCodeNode("root");
            root.Add("archiveVersion", "1");
            root.Add("classes", new XCodeNode[0]);
            root.Add("objectVersion", "45");
            using (TinyProfiler.Section("OrderBy"))
                root.Add("objects", _allNodes.OrderBy(n => n.Id).ToArray());
            root.Add("rootObject", RootNode);

            RootNode = root;
            var writer = new XCodeWriter();
            var stringWriter = new StringWriter();
            writer.Write(this, stringWriter);

            return stringWriter.ToString();
        }

        private static bool UseHintAsGuidToFacilitateComparisons = false;

        public XCodeNode MakeNode(string guidHint)
        {
            string guid = UseHintAsGuidToFacilitateComparisons ? guidHint : ProjectFile.GuidForName(guidHint, "N").Substring(8);
            var node = new XCodeNode(guid);
            _allNodes.Add(node);
            return node;
        }

        private XCodeNode GroupNodeFor(NPath directory)
        {
            XCodeNode group;
            if (Groups.TryGetValue(directory, out group))
                return group;
            group = MakeNode(directory.ToString());
            Groups.Add(directory, group);
            group.Add("isa", "PBXGroup");
            group.Add("children", new List<XCodeNode>());
            group.Add("name", directory.FileName.InQuotes());
            group.Add("path", Paths.UnityRoot.Combine(directory).InQuotes());
            group.Add("sourceTree", kAbsolute);

            if (directory.ToString().Length > 1)
                AddToParentChildrenListSorted(directory.Parent, group);

            return group;
        }

        void AddToParentChildrenListSorted(NPath directory, XCodeNode node)
        {
            var children = GroupNodeFor(directory).Children;
            children.Add(node);
            children.Sort((a, b) =>
            {
                var nameA = a.GetStringMember("name");
                var nameB = b.GetStringMember("name");
                // build target nodes don't have name, but have paths:
                // we want them to appear after all regular folders so prepend
                // "|" which is after all letters.
                if (string.IsNullOrEmpty(nameA))
                    nameA = "|" + a.GetStringMember("path");
                if (string.IsNullOrEmpty(nameB))
                    nameB = "|" + b.GetStringMember("path");

                return string.Compare(nameA, nameB, StringComparison.InvariantCultureIgnoreCase);
            });
        }

        public XCodeNode MakeSourceFileNode(NPath arg)
        {
            var sourcenode = MakeNode("source--" + arg);
            sourcenode.Add("isa", "PBXBuildFile");
            sourcenode.Add("fileRef", MakeFileNode(arg));
            return sourcenode;
        }

        public XCodeNode MakeFileNode(NPath arg, string explicitFileType = null, string sourceTree = kAbsolute)
        {
            XCodeNode result;
            if (_files.TryGetValue(arg, out result))
                return result;
            result = MakeNode(arg.ToString());
            _files.Add(arg, result);
            result.Add("isa", "PBXFileReference");
            result.Add("fileEncoding", "4");

            if (explicitFileType != null)
                result.Add("explicitFileType", $"\"{explicitFileType}\"");

            if (sourceTree == kAbsolute)
            {
                result.Add("name", arg.FileName.InQuotes());
                result.Add("path", Paths.UnityRoot.Combine(arg).InQuotes());
            }
            else
                result.Add("path", arg.FileName.InQuotes());

            result.Add("sourceTree", sourceTree);
            AddToParentChildrenListSorted(arg.Parent, result);

            return result;
        }
    }
}
