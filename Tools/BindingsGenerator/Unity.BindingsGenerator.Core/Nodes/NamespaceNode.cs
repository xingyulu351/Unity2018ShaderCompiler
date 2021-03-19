using System;
using System.Collections.Generic;
using System.Linq;

namespace Unity.BindingsGenerator.Core.Nodes
{
    public sealed class NamespaceNode : IEquatable<NamespaceNode>
    {
        public string Name { get; set; }
        public string Comment { get; set; }

        public List<FunctionNode> Functions { get; set; }
        public List<StructDefinitionNode> Structs { get; set; }
        public List<TypeDefNode> TypeDefs { get; set; }

        public bool IsEmpty => Functions.Count == 0 && Structs.Count == 0 && TypeDefs.Count == 0;

        public NamespaceNode()
        {
            Functions = new List<FunctionNode>();
            Structs = new List<StructDefinitionNode>();
            TypeDefs = new List<TypeDefNode>();
        }

        public NamespaceNode(string name) : this()
        {
            Name = name;
        }

        public override string ToString()
        {
            return new CppGenerator().Generate(this);
        }

        public bool Equals(NamespaceNode other)
        {
            if (ReferenceEquals(null, other)) return false;
            if (ReferenceEquals(this, other)) return true;
            if (!string.Equals(Name, other.Name)) return false;
            if (!string.Equals(Comment, other.Comment)) return false;
            if (Functions.Count != other.Functions.Count) return false;
            if (Functions.Where((t, i) => !Equals(t, other.Functions[i])).Any()) return false;
            if (Structs.Count != other.Structs.Count) return false;
            if (Structs.Where((t, i) => !Equals(t, other.Structs[i])).Any()) return false;
            if (TypeDefs.Count != other.TypeDefs.Count) return false;
            if (TypeDefs.Where((t, i) => !Equals(t, other.TypeDefs[i])).Any()) return false;
            return true;
        }

        public override bool Equals(object obj)
        {
            if (ReferenceEquals(null, obj)) return false;
            if (ReferenceEquals(this, obj)) return true;
            if (!(obj is NamespaceNode)) return false;
            if (Equals((NamespaceNode)obj)) return true;
            return false;
        }

        public override int GetHashCode()
        {
            unchecked
            {
                var hashCode = (Name != null ? Name.GetHashCode() : 0);
                hashCode = (hashCode * 397) ^ (Comment != null ? Comment.GetHashCode() : 0);
                hashCode = (hashCode * 397) ^ (Functions != null ? Functions.GetHashCode() : 0);
                hashCode = (hashCode * 397) ^ (Structs != null ? Structs.GetHashCode() : 0);
                hashCode = (hashCode * 397) ^ (TypeDefs != null ? TypeDefs.GetHashCode() : 0);
                return hashCode;
            }
        }
    }
}
