using System;

namespace Unity.BindingsGenerator.Core.Nodes
{
    public sealed class LabelNode : IStatementNode, IEquatable<LabelNode>
    {
        public string Name { get; set; }

        public LabelNode(string name = null)
        {
            Name = name;
        }

        public override string ToString()
        {
            return new CppGenerator().Generate(this);
        }

        public bool Equals(LabelNode other)
        {
            if (ReferenceEquals(null, other)) return false;
            if (ReferenceEquals(this, other)) return true;
            if (string.Equals(Name, other.Name)) return true;
            return false;
        }

        public override bool Equals(object obj)
        {
            if (ReferenceEquals(null, obj)) return false;
            if (ReferenceEquals(this, obj)) return true;
            if (!(obj is LabelNode)) return false;
            if (Equals((LabelNode)obj)) return true;
            return false;
        }

        public override int GetHashCode()
        {
            return (Name != null ? Name.GetHashCode() : 0);
        }
    }
}
