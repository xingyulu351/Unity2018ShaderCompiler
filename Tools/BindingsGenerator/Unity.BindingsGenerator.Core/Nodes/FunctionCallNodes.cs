using System;
using System.Collections.Generic;
using System.Linq;

namespace Unity.BindingsGenerator.Core.Nodes
{
    public sealed class FunctionCallNode : IExpressionNode, IEquatable<FunctionCallNode>
    {
        public IMemberQualifier Qualifier { get; set; }
        public string FunctionName { get; set; }
        public List<FunctionArgument> Arguments { get; set; }

        public FunctionCallNode(string name = null, IMemberQualifier qualifier = null, params FunctionArgument[] arguments)
        {
            FunctionName = name;
            Qualifier = qualifier;
            Arguments = arguments.ToList();
        }

        public override string ToString()
        {
            return new CppGenerator().Generate(this);
        }

        public bool Equals(FunctionCallNode other)
        {
            if (ReferenceEquals(null, other)) return false;
            if (ReferenceEquals(this, other)) return true;
            if (!Equals(Qualifier, other.Qualifier)) return false;
            if (!string.Equals(FunctionName, other.FunctionName)) return false;
            if (Arguments.Count != other.Arguments.Count) return false;
            if (Arguments.Where((t, i) => !Equals(t, other.Arguments[i])).Any()) return false;
            return true;
        }

        public override bool Equals(object obj)
        {
            if (ReferenceEquals(null, obj)) return false;
            if (ReferenceEquals(this, obj)) return true;
            if (!(obj is FunctionCallNode)) return false;
            if (Equals((FunctionCallNode)obj)) return true;
            return false;
        }

        public override int GetHashCode()
        {
            unchecked
            {
                var hashCode = (Qualifier != null ? Qualifier.GetHashCode() : 0);
                hashCode = (hashCode * 397) ^ (FunctionName != null ? FunctionName.GetHashCode() : 0);
                hashCode = (hashCode * 397) ^ (Arguments != null ? Arguments.GetHashCode() : 0);
                return hashCode;
            }
        }
    }

    public sealed class FunctionArgument : IEquatable<FunctionArgument>
    {
        public string Name { get; set; }

        public FunctionArgument(string name = null)
        {
            Name = name;
        }

        public static implicit operator FunctionArgument(string name)
        {
            return new FunctionArgument(name);
        }

        public override string ToString()
        {
            return new CppGenerator().Generate(this);
        }

        public bool Equals(FunctionArgument other)
        {
            if (ReferenceEquals(null, other)) return false;
            if (ReferenceEquals(this, other)) return true;
            return string.Equals(Name, other.Name);
        }

        public override bool Equals(object obj)
        {
            if (ReferenceEquals(null, obj)) return false;
            if (ReferenceEquals(this, obj)) return true;
            return obj is FunctionArgument && Equals((FunctionArgument)obj);
        }

        public override int GetHashCode()
        {
            return (Name != null ? Name.GetHashCode() : 0);
        }
    }
}
