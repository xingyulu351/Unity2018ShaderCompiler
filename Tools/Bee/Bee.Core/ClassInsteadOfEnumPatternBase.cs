using System;
using System.CodeDom;

namespace Bee.Core
{
    public abstract class ClassInsteadOfEnumPatternBase
    {
        public override bool Equals(object obj)
        {
            if (ReferenceEquals(null, obj)) return false;
            if (ReferenceEquals(this, obj)) return true;
            if (obj.GetType() != this.GetType()) return false;
            return Equals((ClassInsteadOfEnumPatternBase)obj);
        }

        public override int GetHashCode()
        {
            return GetType().GetHashCode();
        }

        private bool Equals(ClassInsteadOfEnumPatternBase other)
        {
            return other.GetType() == GetType();
        }

        public static bool operator==(ClassInsteadOfEnumPatternBase obj1, ClassInsteadOfEnumPatternBase obj2)
        {
            if ((object)obj1 == null && (object)obj2 == null)
                return true;

            if ((object)obj1 == null || (object)obj2 == null)
                return false;

            return obj1.Equals(obj2);
        }

        public static bool operator!=(ClassInsteadOfEnumPatternBase obj1, ClassInsteadOfEnumPatternBase obj2)
        {
            return !(obj1 == obj2);
        }
    }
}
