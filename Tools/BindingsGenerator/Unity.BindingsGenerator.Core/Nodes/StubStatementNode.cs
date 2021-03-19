using System;
using System.Collections.Generic;
using System.Linq;

namespace Unity.BindingsGenerator.Core.Nodes
{
    public sealed class StubStatementNode : IStatementNode, IEquatable<StubStatementNode>
    {
        public StringExpressionNode Condition { get; set; }
        public List<IStatementNode> Statements { get; set; }
        public List<IStatementNode> StatementsForStub { get; set; }

        public StubStatementNode()
        {
            Statements = new List<IStatementNode>();
            StatementsForStub = new List<IStatementNode>();
        }

        public override string ToString()
        {
            return new CppGenerator().Generate(this);
        }

        public bool Equals(StubStatementNode other)
        {
            if (ReferenceEquals(null, other)) return false;
            if (ReferenceEquals(this, other)) return true;
            if (!Equals(Condition, other.Condition)) return false;
            if (Statements.Count != other.Statements.Count) return false;
            if (Statements.Where((t, i) => !Equals(t, other.Statements[i])).Any()) return false;
            if (StatementsForStub.Where((t, i) => !Equals(t, other.StatementsForStub[i])).Any()) return false;
            return true;
        }

        public override bool Equals(object obj)
        {
            if (ReferenceEquals(null, obj)) return false;
            if (ReferenceEquals(this, obj)) return true;
            if (!(obj is StubStatementNode)) return false;
            if (Equals((StubStatementNode)obj)) return true;
            return false;
        }

        public override int GetHashCode()
        {
            unchecked
            {
                var hashCode = (Condition != null ? Condition.GetHashCode() : 0);
                hashCode = (hashCode * 397) ^ (Statements != null ? Statements.GetHashCode() : 0);
                hashCode = (hashCode * 397) ^ (StatementsForStub != null ? StatementsForStub.GetHashCode() : 0);
                return hashCode;
            }
        }
    }
}
