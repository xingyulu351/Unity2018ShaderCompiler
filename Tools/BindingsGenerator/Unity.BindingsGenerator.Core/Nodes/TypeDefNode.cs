using System;

namespace Unity.BindingsGenerator.Core.Nodes
{
    public sealed class TypeDefNode : IEquatable<TypeDefNode>
    {
        public string SourceType { get; set; }
        public string DestType { get; set; }

        public TypeDefNode(string sourceType = null, string destType = null)
        {
            SourceType = sourceType;
            DestType = destType;
        }

        public override string ToString()
        {
            return new CppGenerator().Generate(this);
        }

        public bool Equals(TypeDefNode other)
        {
            if (ReferenceEquals(null, other)) return false;
            if (ReferenceEquals(this, other)) return true;
            if (!string.Equals(SourceType, other.SourceType)) return false;
            if (string.Equals(DestType, other.DestType)) return true;
            return false;
        }

        public override bool Equals(object obj)
        {
            if (ReferenceEquals(null, obj)) return false;
            if (ReferenceEquals(this, obj)) return true;
            if (!(obj is TypeDefNode)) return false;
            if (Equals((TypeDefNode)obj)) return true;
            return false;
        }

        public override int GetHashCode()
        {
            unchecked
            {
                return ((SourceType != null ? SourceType.GetHashCode() : 0) * 397) ^ (DestType != null ? DestType.GetHashCode() : 0);
            }
        }
    }
}
