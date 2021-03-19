using System.Collections.Generic;
using UnderlyingModel;

namespace XMLFormatter
{
    public class AncestorGetter
    {
        private readonly IMemberGetter m_MemberGetter;

        public AncestorGetter(IMemberGetter memberGetter)
        {
            m_MemberGetter = memberGetter;
        }

        public List<MemberItem> GetAncestors(MemberItem member)
        {
            var baseClass = (member.FirstCecilType != null) ? member.FirstCecilType.BaseType : null;
            string baseClassStr = "";
            if (baseClass != null && !CecilHelpers.IsSystemObject(baseClass))
            {
                baseClassStr = CecilHelpers.GetTypeName(baseClass);
            }
            var baseMember = baseClassStr.IsEmpty() ? null : m_MemberGetter.GetMember(baseClassStr);

            var ancestors = new List<MemberItem>();
            if (baseMember != null)
            {
                ancestors.Add(baseMember);
                var otherAncestors = GetAncestors(baseMember);
                ancestors.AddRange(otherAncestors);
            }
            return ancestors;
        }
    }
}
