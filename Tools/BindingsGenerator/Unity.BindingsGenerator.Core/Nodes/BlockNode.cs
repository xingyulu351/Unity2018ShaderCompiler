using System;
using System.Collections.Generic;
using System.Linq;

namespace Unity.BindingsGenerator.Core.Nodes
{
    public sealed class BlockNode : IStatementNode, IEquatable<BlockNode>
    {
        public List<IStatementNode> Statements { get; set; }

        public BlockNode()
        {
            Statements = new List<IStatementNode>();
        }

        public override string ToString()
        {
            return new CppGenerator().Generate(this);
        }

        public bool Equals(BlockNode other)
        {
            if (ReferenceEquals(null, other)) return false;
            if (ReferenceEquals(this, other)) return true;
            if (Equals(Statements, other.Statements)) return true;
            if (Statements.Count != other.Statements.Count) return false;
            if (Statements.Where((t, i) => !Equals(t, other.Statements[i])).Any()) return false;
            return true;
        }

        public override bool Equals(object obj)
        {
            if (ReferenceEquals(null, obj)) return false;
            if (ReferenceEquals(this, obj)) return true;
            if (obj is BlockNode && Equals((BlockNode)obj)) return true;
            return false;
        }

        public override int GetHashCode()
        {
            return (Statements != null ? Statements.GetHashCode() : 0);
        }
    }
}
