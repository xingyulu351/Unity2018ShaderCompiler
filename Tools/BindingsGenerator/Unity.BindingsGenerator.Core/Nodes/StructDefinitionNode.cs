using System;
using System.Collections.Generic;
using System.Linq;

namespace Unity.BindingsGenerator.Core.Nodes
{
    public sealed class StructDefinitionNode : IEquatable<StructDefinitionNode>
    {
        public List<TypeDefNode> TypeDefs { get; set; }
        public List<FieldDeclarationStatementNode> Members { get; set; }
        public List<FunctionNode> FunctionDefinitions { get; set; }

        public string Name { get; set; }
        public List<string> TemplateSpecialization { get; set; }
        public bool IsTemplate { get; set; }
        public List<string> TemplateArguments { get; set; }
        public bool ForceUniqueViaPreprocessor { get; set; }

        public StructDefinitionNode(string name = null)
        {
            Name = name;
            TypeDefs = new List<TypeDefNode>();
            Members = new List<FieldDeclarationStatementNode>();
            TemplateSpecialization = new List<string>();
            TemplateArguments = new List<string>();
            FunctionDefinitions = new List<FunctionNode>();
        }

        public override string ToString()
        {
            return new CppGenerator().Generate(this);
        }

        public bool Equals(StructDefinitionNode other)
        {
            if (ReferenceEquals(null, other)) return false;
            if (ReferenceEquals(this, other)) return true;
            if (!string.Equals(Name, other.Name)) return false;
            if (!TypeDefs.SequenceEqual(other.TypeDefs, EqualityComparer<TypeDefNode>.Default)) return false;
            if (!Members.SequenceEqual(other.Members, EqualityComparer<FieldDeclarationStatementNode>.Default)) return false;
            if (!FunctionDefinitions.SequenceEqual(other.FunctionDefinitions, EqualityComparer<FunctionNode>.Default)) return false;
            if (!TemplateSpecialization.SequenceEqual(other.TemplateSpecialization, EqualityComparer<string>.Default)) return false;
            if (!TemplateArguments.SequenceEqual(other.TemplateArguments, EqualityComparer<string>.Default)) return false;
            return true;
        }

        public override bool Equals(object obj)
        {
            if (ReferenceEquals(null, obj)) return false;
            if (ReferenceEquals(this, obj)) return true;
            if (!(obj is StructDefinitionNode)) return false;
            if (Equals((StructDefinitionNode)obj)) return true;
            return false;
        }

        public override int GetHashCode()
        {
            unchecked
            {
                var hashCode = (TypeDefs != null ? TypeDefs.GetHashCode() : 0);
                hashCode = (hashCode * 397) ^ (Members != null ? Members.GetHashCode() : 0);
                hashCode = (hashCode * 397) ^ (Name != null ? Name.GetHashCode() : 0);
                return hashCode;
            }
        }
    }
}
