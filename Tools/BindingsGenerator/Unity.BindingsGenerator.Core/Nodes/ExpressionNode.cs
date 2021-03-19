using System;

namespace Unity.BindingsGenerator.Core.Nodes
{
    public interface IExpressionNode
    {
    }

    public sealed class StringExpressionNode : IExpressionNode, IEquatable<StringExpressionNode>
    {
        public string Str { get; set; }

        public StringExpressionNode(string str = null)
        {
            Str = str;
        }

        public override string ToString()
        {
            return new CppGenerator().Generate(this);
        }

        public bool Equals(StringExpressionNode other)
        {
            if (ReferenceEquals(null, other)) return false;
            if (ReferenceEquals(this, other)) return true;
            if (string.Equals(Str, other.Str)) return true;
            return false;
        }

        public override bool Equals(object obj)
        {
            if (ReferenceEquals(null, obj)) return false;
            if (ReferenceEquals(this, obj)) return true;
            if (!(obj is StringExpressionNode)) return false;
            if (Equals((StringExpressionNode)obj)) return true;
            return false;
        }

        public override int GetHashCode()
        {
            return (Str != null ? Str.GetHashCode() : 0);
        }
    }
}
