using System;

namespace Unity.BindingsGenerator.Core.Nodes
{
    public interface IStatementNode
    {
    }

    public sealed class ExpressionStatementNode : IStatementNode, IEquatable<ExpressionStatementNode>
    {
        public IExpressionNode Expression { get; set; }

        public ExpressionStatementNode(IExpressionNode expressionNode = null)
        {
            Expression = expressionNode;
        }

        public override string ToString()
        {
            return new CppGenerator().Generate(this);
        }

        public bool Equals(ExpressionStatementNode other)
        {
            if (ReferenceEquals(null, other)) return false;
            if (ReferenceEquals(this, other)) return true;
            if (Equals(Expression, other.Expression)) return true;
            return false;
        }

        public override bool Equals(object obj)
        {
            if (ReferenceEquals(null, obj)) return false;
            if (ReferenceEquals(this, obj)) return true;
            if (!(obj is ExpressionStatementNode)) return false;
            if (Equals((ExpressionStatementNode)obj)) return true;
            return false;
        }

        public override int GetHashCode()
        {
            return (Expression != null ? Expression.GetHashCode() : 0);
        }
    }

    public sealed class StringStatementNode : IStatementNode, IEquatable<StringStatementNode>
    {
        public string Str { get; set; }

        public StringStatementNode(string str = null)
        {
            Str = str;
        }

        public override string ToString()
        {
            return new CppGenerator().Generate(this);
        }

        public bool Equals(StringStatementNode other)
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
            if (!(obj is StringStatementNode)) return false;
            if (Equals((StringStatementNode)obj)) return true;
            return false;
        }

        public override int GetHashCode()
        {
            return (Str != null ? Str.GetHashCode() : 0);
        }
    }
}
