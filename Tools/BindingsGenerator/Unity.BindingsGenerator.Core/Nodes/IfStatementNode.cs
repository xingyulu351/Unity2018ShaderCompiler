using System;
using System.Collections.Generic;
using System.Linq;

namespace Unity.BindingsGenerator.Core.Nodes
{
    public sealed class IfStatementNode : IStatementNode, IEquatable<IfStatementNode>
    {
        public IExpressionNode Condition { get; set; }
        public List<IStatementNode> Statements { get; set; }

        public IfStatementNode(IExpressionNode condition = null)
        {
            Condition = condition;
            Statements = new List<IStatementNode>();
        }

        public override string ToString()
        {
            return new CppGenerator().Generate(this);
        }

        public bool Equals(IfStatementNode other)
        {
            if (ReferenceEquals(null, other)) return false;
            if (ReferenceEquals(this, other)) return true;
            if (!Equals(Condition, other.Condition)) return false;
            if (Statements.Count != other.Statements.Count) return false;
            if (Statements.Where((t, i) => !Equals(t, other.Statements[i])).Any()) return false;
            return true;
        }

        public override bool Equals(object obj)
        {
            if (ReferenceEquals(null, obj)) return false;
            if (ReferenceEquals(this, obj)) return true;
            if (!(obj is IfStatementNode)) return false;
            if (Equals((IfStatementNode)obj)) return true;
            return false;
        }

        public override int GetHashCode()
        {
            unchecked
            {
                return ((Condition != null ? Condition.GetHashCode() : 0) * 397) ^ (Statements != null ? Statements.GetHashCode() : 0);
            }
        }
    }
}
