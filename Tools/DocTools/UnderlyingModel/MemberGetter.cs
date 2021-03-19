using System;
using System.Collections.Generic;
using System.Linq;

namespace UnderlyingModel
{
    //this is an adapter for NewDataItemProject
    public class MemberGetter : IMemberGetter
    {
        private readonly NewDataItemProject m_Project;

        private const string commonWords = "a all also an and are as at attached be both by can class for "
            + "from function get if in int into is it just new no none not object of off on one only or print "
            + "ref returns see set that the this to two use used var when wiki will with you"
            + "private public protected static void float int implements extends bool true false"
            + "// some string it's member summary description detail";
        private string[] m_CommonWordsList;

        //closest non-obsolete match by name
        private Dictionary<string, MemberItem> m_ClosestMembersMatchMap;
        public MemberGetter(NewDataItemProject project)
        {
            m_Project = project;
            m_CommonWordsList = commonWords.Split(' ');
            m_ClosestMembersMatchMap = new Dictionary<string, MemberItem>();
        }

        public List<MemberItem> GetAllMembers()
        {
            return m_Project.GetAllMembers();
        }

        public MemberItem GetMember(string memberName)
        {
            if (m_Project.ContainsMember(memberName))
                return m_Project.GetMember(memberName);
            var memID = GetMemberIDFromSimpleName(memberName);
            return m_Project.GetMember(memID);
        }

        public MemberItem GetClosestNonObsoleteMember(string memberName)
        {
            //special handling for System.Type, which happens often, but doesn't have a page
            if (memberName == "Type")
                return null;

            //common words cannot be links, the reduces the number of lookups
            if (m_CommonWordsList.Contains(memberName.ToLower()))
                return null;

            if (memberName.Contains('/') || memberName.Contains('\"') || memberName.Contains('='))
                return null;

            if (m_ClosestMembersMatchMap.ContainsKey(memberName))
                return m_ClosestMembersMatchMap[memberName];

            var elem = GetMember(memberName);

            if (elem == null || elem.IsObsolete)
            {
                elem = m_Project.GetAllMembers().FirstOrDefault(m => !m.IsObsolete && m.DisplayName == memberName);
            }

            //even if the elem is null, we have already done the lookup
            m_ClosestMembersMatchMap[memberName] = elem;

            return elem;
        }

        public bool ContainsMember(string memberName)
        {
            if (m_Project.ContainsMember(memberName))
                return true;
            var memID = GetMemberIDFromSimpleName(memberName);
            return m_Project.ContainsMember(memID);
        }

        //used for converting links like [[Color.red]] to the ID Color._red
        private static string GetMemberIDFromSimpleName(string entry)
        {
            var splitString = entry.Split(new[] { '.' }, StringSplitOptions.RemoveEmptyEntries);
            //lambda expression should not be converted to "method group"
            var underscoreNames = splitString.Select(s =>
                s.StartsWith("iOS") || s.StartsWith("tvOS") ? s : StringConvertUtils.LowerCaseNeedsUnderscore(s)
                ).ToArray();
            return String.Join(".", underscoreNames);
        }
    }
}
