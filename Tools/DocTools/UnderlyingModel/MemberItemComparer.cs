using UnderlyingModel.MemDoc.Comparers;

namespace UnderlyingModel
{
    public class MemberItemComparer : IComparerWithError<MemberItem>
    {
        public CompareResult Compare(MemberItem x, MemberItem y)
        {
            if (x.ItemName != y.ItemName)
                return CompareResult.FailureWithExpectedAndActual("ItemNames are different: ", x.ItemName, y.ItemName);
            if (x.ChildMembers.Count != y.ChildMembers.Count)
                return CompareResult.FailureWithExpectedAndActual("ChildMembers have different counts: ", x.ChildMembers.Count, y.ChildMembers.Count);
            for (int i = 0; i < x.ChildMembers.Count; i++)
            {
                var result = Compare(x.ChildMembers[i], y.ChildMembers[i]);
                if (!result.Success)
                    return result;
            }

            return new MemDocModelComparer().Compare(x.DocModel, y.DocModel);
        }
    }
}
