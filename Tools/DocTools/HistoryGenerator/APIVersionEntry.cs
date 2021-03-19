using UnderlyingModel;

namespace HistoryGenerator
{
    /// <summary>
    /// this entry should have everything we need to reconstruct API information even when the API is not present in the DLL
    /// </summary>
    public class APIVersionEntry
    {
        internal string MemberID;
        internal AssemblyType AssemblyType;
        internal bool IsStatic;
        internal string Namespace;
        internal VersionNumber VersionAdded;
        internal VersionNumber VersionObsolete;
        internal VersionNumber VersionRemoved;
        internal APIStatus ApiStatus;

        internal VersionNumber VersionLatestChange
        {
            get
            {
                if (VersionObsolete == null && VersionRemoved == null)
                    return VersionAdded;
                return VersionObsolete ?? VersionRemoved;
            }
        }

        public override string ToString()
        {
            var typeString = AssemblyType == AssemblyType.ImplOperator ? "implop" : AssemblyType.ToString().ToLower();
            var ret = string.Format("{0} | {1} | {2} ||| {3}",
                MemberID,
                typeString + (IsStatic ? " (static)" : ""),
                Namespace,
                VersionAdded);
            if (VersionObsolete != null || VersionRemoved != null)
                ret += string.Format(" ||| {0} ||| {1}", VersionObsolete, VersionRemoved);
            return ret;
        }
    }
}
