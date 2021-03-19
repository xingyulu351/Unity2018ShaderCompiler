using System.Linq;
using System.Collections.Generic;
using System.Collections.ObjectModel;

namespace Unity.BindingsGenerator.Core.Nodes
{
    public class FileNode
    {
        readonly List<string> _includeFiles = new List<string>();

        public FileNode()
        {
            IncludeFiles = new ReadOnlyCollection<string>(_includeFiles);
            Namespaces = new List<NamespaceNode> { DefaultNamespace };
        }

        public List<NamespaceNode> Namespaces { get; set; }

        public ReadOnlyCollection<string> IncludeFiles { get; private set; }

        public NamespaceNode DefaultNamespace { get; } = new NamespaceNode { Name = "" };

        public void AddInclude(string fileName)
        {
            if (!_includeFiles.Any(f => fileName == f))
                _includeFiles.Add(fileName);
        }

        public void AddIncludes(IEnumerable<string> fileNames)
        {
            foreach (var fileName in fileNames)
                AddInclude(fileName);
        }

        public override string ToString()
        {
            return new CppGenerator().Generate(this);
        }

        public bool Equals(FileNode other)
        {
            if (ReferenceEquals(null, other)) return false;
            if (ReferenceEquals(this, other)) return true;
            if (!Equals(_includeFiles, other._includeFiles)) return false;
            if (!Equals(IncludeFiles, other.IncludeFiles)) return false;
            if (Namespaces.Count != other.Namespaces.Count) return false;
            if (Namespaces.Where((t, i) => !Equals(t, other.Namespaces[i])).Any()) return false;
            return true;
        }

        public override bool Equals(object obj)
        {
            if (ReferenceEquals(null, obj)) return false;
            if (ReferenceEquals(this, obj)) return true;
            if (!(obj is FileNode)) return false;
            if (Equals((FileNode)obj)) return true;
            return false;
        }

        public override int GetHashCode()
        {
            unchecked
            {
                var hashCode = (_includeFiles != null ? _includeFiles.GetHashCode() : 0);
                hashCode = (hashCode * 397) ^ (IncludeFiles != null ? IncludeFiles.GetHashCode() : 0);
                hashCode = (hashCode * 397) ^ (Namespaces != null ? Namespaces.GetHashCode() : 0);
                return hashCode;
            }
        }
    }
}
