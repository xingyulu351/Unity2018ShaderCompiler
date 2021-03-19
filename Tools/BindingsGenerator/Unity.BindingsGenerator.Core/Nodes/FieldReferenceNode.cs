using System;

namespace Unity.BindingsGenerator.Core.Nodes
{
    public sealed class FieldReferenceNode : IExpressionNode, IEquatable<FieldReferenceNode>
    {
        public IMemberQualifier Qualifier { get; set; }
        public string Name { get; set; }

        public FieldReferenceNode(string name = null, IMemberQualifier qualifier = null)
        {
            Name = name;
            Qualifier = qualifier;
        }

        public override string ToString()
        {
            return new CppGenerator().Generate(this);
        }

        public bool Equals(FieldReferenceNode other)
        {
            if (ReferenceEquals(null, other)) return false;
            if (ReferenceEquals(this, other)) return true;
            if (!Equals(Qualifier, other.Qualifier)) return false;
            if (string.Equals(Name, other.Name)) return true;
            return false;
        }

        public override bool Equals(object obj)
        {
            if (ReferenceEquals(null, obj)) return false;
            if (ReferenceEquals(this, obj)) return true;
            if (!(obj is FieldReferenceNode)) return false;
            if (Equals((FieldReferenceNode)obj)) return true;
            return false;
        }

        public override int GetHashCode()
        {
            unchecked
            {
                return ((Qualifier != null ? Qualifier.GetHashCode() : 0) * 397) ^ (Name != null ? Name.GetHashCode() : 0);
            }
        }
    }
}
