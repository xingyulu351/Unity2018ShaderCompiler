using System.Text;
using System.Text.RegularExpressions;

namespace AssemblyPatcher.Configuration
{
    public class PatchRule
    {
        public PatchRule(string sourceMember, OperationKind operation = OperationKind.Throw)
        {
            Operation = operation;
            SourceMemberFullyQualifiedName = sourceMember;
            Kind = sourceMember.Contains("::") ? MemberKind.Other : MemberKind.Type;
            TargetMemberFullyQualifiedName = null;
        }

        public PatchRule(string sourceMember, string targetMember) : this(sourceMember, OperationKind.Replace)
        {
            TargetMemberFullyQualifiedName = targetMember;
            if (targetMember != null && Regex.IsMatch(targetMember, @"^\s*\{\s*\}\s*$"))
            {
                Operation = OperationKind.RemoveBody;
            }
        }

        public bool IsApplicableToOperands()
        {
            return Operation != OperationKind.RemoveBody;
        }

        public string Rewriter { get; set; }
        public string ApplicableWith { get; set; }
        public string SourceMemberFullyQualifiedName { get; set; }
        public string TargetMemberFullyQualifiedName { get; set; }
        public MemberKind Kind { get; set; }
        public OperationKind Operation { get; set; }
        public string DeclaringAssembly { get; set; }

        public bool IsNullTypeReplacement
        {
            get { return Kind == MemberKind.Type && TargetMemberFullyQualifiedName == null && DeclaringAssembly == null; }
        }

        protected bool Equals(PatchRule other)
        {
            return string.Equals(ApplicableWith, other.ApplicableWith) && string.Equals(SourceMemberFullyQualifiedName, other.SourceMemberFullyQualifiedName) && string.Equals(TargetMemberFullyQualifiedName, other.TargetMemberFullyQualifiedName) && string.Equals(DeclaringAssembly, other.DeclaringAssembly) && Kind == other.Kind && Operation == other.Operation;
        }

        public override bool Equals(object obj)
        {
            if (ReferenceEquals(null, obj)) return false;
            if (ReferenceEquals(this, obj)) return true;
            if (obj.GetType() != this.GetType()) return false;
            return Equals((PatchRule)obj);
        }

        public override int GetHashCode()
        {
            unchecked
            {
                var hashCode = (ApplicableWith != null ? ApplicableWith.GetHashCode() : 0);
                hashCode = (hashCode * 397) ^ (SourceMemberFullyQualifiedName != null ? SourceMemberFullyQualifiedName.GetHashCode() : 0);
                hashCode = (hashCode * 397) ^ (TargetMemberFullyQualifiedName != null ? TargetMemberFullyQualifiedName.GetHashCode() : 0);
                hashCode = (hashCode * 397) ^ (DeclaringAssembly != null ? DeclaringAssembly.GetHashCode() : 0);
                hashCode = (hashCode * 397) ^ (int)Kind;
                hashCode = (hashCode * 397) ^ (int)Operation;
                return hashCode;
            }
        }

        public override string ToString()
        {
            var ret = new StringBuilder();
            if (!string.IsNullOrWhiteSpace(ApplicableWith))
            {
                ret.AppendFormat("|{0}| ", ApplicableWith);
            }

            var declaringAssembly = string.IsNullOrWhiteSpace(DeclaringAssembly) ? "" : string.Format("[{0}] ", DeclaringAssembly);
            ret.AppendFormat("{0} -> {1}{2}", SourceMemberFullyQualifiedName, declaringAssembly, TargetMemberFullyQualifiedName);

            return ret.ToString();
        }
    }
}
