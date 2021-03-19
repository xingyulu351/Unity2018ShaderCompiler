using System;
using System.Collections.Generic;
using System.Linq;

namespace Unity.BindingsGenerator.Core.Nodes
{
    public sealed class FunctionNode : IEquatable<FunctionNode>
    {
        public string Name { get; set; }
        public string ManagedName { get; set; }
        public string DeclSpec { get; set; }
        public string CallingConvention { get; set; }
        public bool IsFriend { get; set; }
        public bool IsStatic { get; set; }
        public bool IsInline { get; set; }
        public bool UseDefineSentinel { get; set; }

        public List<IStatementNode> Statements { get; set; }
        public List<FunctionParameter> Parameters { get; set; }
        public string ReturnType { get; set; }
        public List<string> TemplateSpecialization { get; set; }

        public FunctionNode(string name = null)
        {
            Name = name;
            ReturnType = Naming.VoidType;
            Statements = new List<IStatementNode>();
            Parameters = new List<FunctionParameter>();
            TemplateSpecialization = new List<string>();
        }

        public override string ToString()
        {
            return new CppGenerator().Generate(this);
        }

        public bool Equals(FunctionNode other)
        {
            if (ReferenceEquals(null, other)) return false;
            if (ReferenceEquals(this, other)) return true;
            if (!string.Equals(Name, other.Name)) return false;
            if (!string.Equals(ManagedName, other.ManagedName)) return false;
            if (!string.Equals(DeclSpec, other.DeclSpec)) return false;
            if (!string.Equals(CallingConvention, other.CallingConvention)) return false;
            if (IsFriend != other.IsFriend || IsStatic != other.IsStatic) return false;
            if (IsInline != other.IsInline) return false;
            if (UseDefineSentinel != other.UseDefineSentinel) return false;
            if (!string.Equals(ReturnType, other.ReturnType)) return false;
            if (!string.Equals(TemplateSpecialization, other.TemplateSpecialization)) return false;
            if (Statements.Count != other.Statements.Count) return false;
            if (Statements.Where((t, i) => !Equals(t, other.Statements[i])).Any()) return false;
            if (Parameters.Count != other.Parameters.Count) return false;
            if (Parameters.Where((t, i) => !Equals(t, other.Parameters[i])).Any()) return false;
            return true;
        }

        public override bool Equals(object obj)
        {
            if (ReferenceEquals(null, obj)) return false;
            if (ReferenceEquals(this, obj)) return true;
            if (!(obj is FunctionNode)) return false;
            if (Equals((FunctionNode)obj)) return true;
            return false;
        }

        public override int GetHashCode()
        {
            unchecked
            {
                var hashCode = (Name != null ? Name.GetHashCode() : 0);
                hashCode = (hashCode * 397) ^ (ManagedName != null ? ManagedName.GetHashCode() : 0);
                hashCode = (hashCode * 397) ^ (DeclSpec != null ? DeclSpec.GetHashCode() : 0);
                hashCode = (hashCode * 397) ^ (CallingConvention != null ? CallingConvention.GetHashCode() : 0);
                hashCode = (hashCode * 397) ^ IsFriend.GetHashCode();
                hashCode = (hashCode * 397) ^ IsStatic.GetHashCode();
                hashCode = (hashCode * 397) ^ IsInline.GetHashCode();
                hashCode = (hashCode * 397) ^ UseDefineSentinel.GetHashCode();
                hashCode = (hashCode * 397) ^ (Statements != null ? Statements.GetHashCode() : 0);
                hashCode = (hashCode * 397) ^ (Parameters != null ? Parameters.GetHashCode() : 0);
                hashCode = (hashCode * 397) ^ (ReturnType != null ? ReturnType.GetHashCode() : 0);
                hashCode = (hashCode * 397) ^ (TemplateSpecialization != null ? TemplateSpecialization.GetHashCode() : 0);
                return hashCode;
            }
        }
    }

    public sealed class FunctionParameter : IEquatable<FunctionParameter>
    {
        public bool IsSelf { get; set; }
        public string Name { get; set; }
        public string Type { get; set; }
        public TypeUsage Usage { get; set; }
        public bool IsOutReturn { get; set; }

        public FunctionParameter(string name = null, string type = null, bool isSelf = false, bool isOutReturn = false)
        {
            Name = name;
            Type = type;
            IsSelf = isSelf;
            IsOutReturn = isOutReturn;
            Usage = TypeUsage.Parameter;
        }

        public override string ToString()
        {
            return new CppGenerator().Generate(this);
        }

        public bool Equals(FunctionParameter other)
        {
            if (ReferenceEquals(null, other)) return false;
            if (ReferenceEquals(this, other)) return true;
            return IsSelf == other.IsSelf && string.Equals(Name, other.Name) && string.Equals(Type, other.Type) && Usage == other.Usage && IsOutReturn == other.IsOutReturn;
        }

        public override bool Equals(object obj)
        {
            if (ReferenceEquals(null, obj)) return false;
            if (ReferenceEquals(this, obj)) return true;
            return obj is FunctionParameter && Equals((FunctionParameter)obj);
        }

        public override int GetHashCode()
        {
            unchecked
            {
                var hashCode = IsSelf.GetHashCode();
                hashCode = (hashCode * 397) ^ (Name != null ? Name.GetHashCode() : 0);
                hashCode = (hashCode * 397) ^ (Type != null ? Type.GetHashCode() : 0);
                hashCode = (hashCode * 397) ^ (int)Usage;
                hashCode = (hashCode * 397) ^ IsOutReturn.GetHashCode();
                return hashCode;
            }
        }
    }
}
