using System;

namespace Unity.BindingsGenerator.Core.Nodes
{
    public sealed class ReturnStatementNode : IStatementNode, IEquatable<ReturnStatementNode>
    {
        public IExpressionNode Expression { get; set; }

        public ReturnStatementNode()
        {
        }

        public ReturnStatementNode(IExpressionNode expressionNode)
        {
            Expression = expressionNode;
        }

        public override string ToString()
        {
            return new CppGenerator().Generate(this);
        }

        public bool Equals(ReturnStatementNode other)
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
            if (!(obj is ReturnStatementNode)) return false;
            if (Equals((ReturnStatementNode)obj)) return true;
            return false;
        }

        public override int GetHashCode()
        {
            return (Expression != null ? Expression.GetHashCode() : 0);
        }
    }
}
