using System;

namespace Unity.BindingsGenerator.Core.Nodes
{
    public sealed class VariableDeclarationStatementNode : IStatementNode, IEquatable<VariableDeclarationStatementNode>
    {
        public string Type { get; set; }
        public string Name { get; set; }
        public IExpressionNode Initializer { get; set; }

        public VariableDeclarationStatementNode(string name = null, string type = null, IExpressionNode initializer = null)
        {
            Name = name;
            Type = type;
            Initializer = initializer;
        }

        public override string ToString()
        {
            return new CppGenerator().Generate(this);
        }

        public bool Equals(VariableDeclarationStatementNode other)
        {
            if (ReferenceEquals(null, other)) return false;
            if (ReferenceEquals(this, other)) return true;
            if (!string.Equals(Type, other.Type)) return false;
            if (!string.Equals(Name, other.Name)) return false;
            if (Equals(Initializer, other.Initializer)) return true;
            return false;
        }

        public override bool Equals(object obj)
        {
            if (ReferenceEquals(null, obj)) return false;
            if (ReferenceEquals(this, obj)) return true;
            if (!(obj is VariableDeclarationStatementNode)) return false;
            if (Equals((VariableDeclarationStatementNode)obj)) return true;
            return false;
        }

        public override int GetHashCode()
        {
            unchecked
            {
                var hashCode = (Type != null ? Type.GetHashCode() : 0);
                hashCode = (hashCode * 397) ^ (Name != null ? Name.GetHashCode() : 0);
                hashCode = (hashCode * 397) ^ (Initializer != null ? Initializer.GetHashCode() : 0);
                return hashCode;
            }
        }
    }
}
