using System;

namespace Unity.BindingsGenerator.Core.Nodes
{
    public class FieldDeclarationStatementNode : IStatementNode, IEquatable<FieldDeclarationStatementNode>
    {
        public bool IsStatic { get; set; }
        public bool IsConst { get; set; }
        public string Type { get; set; }
        public string Name { get; set; }
        public IExpressionNode Initializer { get; set; }
        public int? ElementCount { get; set; }

        public FieldDeclarationStatementNode(string name = null, string type = null, bool isStatic = false, bool isConst = false, IExpressionNode initializer = null, int? elementCount = null)
        {
            Name = name;
            Type = type;
            Initializer = initializer;
            IsStatic = isStatic;
            IsConst = isConst;
            ElementCount = elementCount;
        }

        public override string ToString()
        {
            return new CppGenerator().Generate(this);
        }

        public bool Equals(FieldDeclarationStatementNode other)
        {
            if (ReferenceEquals(null, other)) return false;
            if (ReferenceEquals(this, other)) return true;
            if (!string.Equals(Type, other.Type)) return false;
            if (!string.Equals(Name, other.Name)) return false;
            if (Equals(Initializer, other.Initializer)) return true;
            if (Equals(IsStatic, other.IsStatic)) return true;
            if (Equals(IsConst, other.IsConst)) return true;
            if (Equals(Type, other.Type)) return true;
            if (Equals(Name, other.Name)) return true;
            if (Equals(ElementCount, other.ElementCount)) return true;
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
                hashCode = (hashCode * 2) ^ (IsStatic ? 1 : 0);
                hashCode = (hashCode * 2) ^ (IsConst ? 1 : 0);
                return hashCode;
            }
        }
    }
}
