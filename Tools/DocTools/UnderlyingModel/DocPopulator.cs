namespace UnderlyingModel
{
    public partial class NewDataItemProject
    {
        //note that if this is a CsNone, MemberItemIo.LoadDoc will switch the type to AssemblyType.CsNone
        public MemberItem CreateAndAddMemberItem(string memberName)
        {
            var thisMember = new MemberItem(memberName, AssemblyType.Unknown);

            m_MapNameToItem[memberName] = thisMember;
            var names = MemberItem.GetNamesFromMemberID(memberName);

            //method or a property - add to the class, if it's already present in the symbols
            if (!names.MemberName.IsEmpty())
            {
                var parentName = names.ClassName;
                if (m_MapNameToItem.ContainsKey(parentName))
                {
                    var parentItem = m_MapNameToItem[parentName];
                    parentItem.ChildMembers.Add(thisMember);
                }
            }
            else
                //class
                FindChildrenAndAdoptThem(memberName);
            return thisMember;
        }

        private void FindChildrenAndAdoptThem(string parentClassName)
        {
            var parentClass = m_MapNameToItem[parentClassName];
            foreach (var memberName in m_MapNameToItem.Keys)
            {
                if (memberName.StartsWith(parentClassName + "."))
                {
                    var memberItem = m_MapNameToItem[memberName];
                    if (!parentClass.ChildMembers.Contains(memberItem))
                        parentClass.ChildMembers.Add(memberItem);
                }
            }
        }
    }
}
