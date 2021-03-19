using System;

namespace Unity.BindingsGenerator.Core.Nodes
{
    public interface IMemberQualifier
    {
    }

    public sealed class FreeQualifier : IMemberQualifier, IEquatable<FreeQualifier>
    {
        public static FreeQualifier Instance = new FreeQualifier();

        public override string ToString()
        {
            return new CppGenerator().Generate(this);
        }

        public bool Equals(FreeQualifier other)
        {
            return true;
        }

        public override bool Equals(object obj)
        {
            if (ReferenceEquals(null, obj)) return false;
            if (ReferenceEquals(this, obj)) return true;
            if (!(obj is FreeQualifier)) return false;
            return Equals((FreeQualifier)obj);
        }

        public override int GetHashCode()
        {
            return base.GetHashCode();
        }
    }

    public sealed class InstanceMemberQualifier : IMemberQualifier, IEquatable<InstanceMemberQualifier>
    {
        public string Expression { get; set; }
        public bool IsReference { get; set; }

        public InstanceMemberQualifier(string expression = null, bool isReference = false)
        {
            Expression = expression;
            IsReference = isReference;
        }

        public override string ToString()
        {
            return new CppGenerator().Generate(this);
        }

        public bool Equals(InstanceMemberQualifier other)
        {
            if (ReferenceEquals(null, other)) return false;
            if (ReferenceEquals(this, other)) return true;
            if (!string.Equals(Expression, other.Expression)) return false;
            return IsReference == other.IsReference;
        }

        public override bool Equals(object obj)
        {
            if (ReferenceEquals(null, obj)) return false;
            if (ReferenceEquals(this, obj)) return true;
            if (!(obj is InstanceMemberQualifier)) return false;
            return Equals((InstanceMemberQualifier)obj);
        }

        public override int GetHashCode()
        {
            unchecked
            {
                return ((Expression != null ? Expression.GetHashCode() : 0) * 397) ^ IsReference.GetHashCode();
            }
        }
    }

    public sealed class StaticMemberQualifier : IMemberQualifier, IEquatable<StaticMemberQualifier>
    {
        public string ClassName { get; set; }

        public StaticMemberQualifier(string className = null)
        {
            ClassName = className;
        }

        public override string ToString()
        {
            return new CppGenerator().Generate(this);
        }

        public bool Equals(StaticMemberQualifier other)
        {
            if (ReferenceEquals(null, other)) return false;
            if (ReferenceEquals(this, other)) return true;
            if (string.Equals(ClassName, other.ClassName)) return true;
            return false;
        }

        public override bool Equals(object obj)
        {
            if (ReferenceEquals(null, obj)) return false;
            if (ReferenceEquals(this, obj)) return true;
            if (!(obj is StaticMemberQualifier)) return false;
            if (Equals((StaticMemberQualifier)obj)) return true;
            return false;
        }

        public override int GetHashCode()
        {
            return (ClassName != null ? ClassName.GetHashCode() : 0);
        }
    }
}
