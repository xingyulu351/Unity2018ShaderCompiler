using System;

namespace Unity.BindingsGenerator.Core.Nodes
{
    public sealed class AssignNode : IExpressionNode, IEquatable<AssignNode>
    {
        public IExpressionNode Lhs { get; set; }
        public IExpressionNode Rhs { get; set; }

        public AssignNode(IExpressionNode lhs = null, IExpressionNode rhs = null)
        {
            Lhs = lhs;
            Rhs = rhs;
        }

        public override string ToString()
        {
            return new CppGenerator().Generate(this);
        }

        public bool Equals(AssignNode other)
        {
            if (ReferenceEquals(null, other)) return false;
            if (ReferenceEquals(this, other)) return true;
            if (!Equals(Lhs, other.Lhs)) return false;
            if (Equals(Rhs, other.Rhs)) return true;
            return false;
        }

        public override bool Equals(object obj)
        {
            if (ReferenceEquals(null, obj)) return false;
            if (ReferenceEquals(this, obj)) return true;
            if (!(obj is AssignNode)) return false;
            if (Equals((AssignNode)obj)) return true;
            return false;
        }

        public override int GetHashCode()
        {
            unchecked
            {
                return ((Lhs != null ? Lhs.GetHashCode() : 0) * 397) ^ (Rhs != null ? Rhs.GetHashCode() : 0);
            }
        }
    }
}
