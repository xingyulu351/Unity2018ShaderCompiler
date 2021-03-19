using System.Collections.Generic;

namespace UnderlyingModel
{
    public interface IMemberGetter
    {
        MemberItem GetMember(string memberName);
        MemberItem GetClosestNonObsoleteMember(string memberName);
        bool ContainsMember(string memberName);
        List<MemberItem> GetAllMembers();
    }
}
