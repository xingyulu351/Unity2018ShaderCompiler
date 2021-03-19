using System.Collections.Generic;

namespace UnderlyingModel.MemDoc.Comparers
{
    public static class ComparerUtils
    {
        public static CompareResult CompareLists<T>(IList<T> xList, IList<T> yList, IComparerWithError<T> comparer)
        {
            if (xList == null && yList == null)
                return CompareResult.SuccessfulResult();
            var resultNullComparison = CompareNullableElement(xList, yList);
            if (!resultNullComparison.Success)
                return resultNullComparison;
            if (xList.Count != yList.Count)
                return CompareResult.FailureWithExpectedAndActual("lists differ in the element count", xList.Count, yList.Count);
            for (int i = 0; i < xList.Count; i++)
            {
                var result = comparer.Compare(xList[i], yList[i]);
                if (!result.Success)
                    return result;
            }
            return CompareResult.SuccessfulResult();
        }

        public static CompareResult CompareNullableElement(object x, object y)
        {
            if (x == null && y == null)
                return CompareResult.SuccessfulResult();
            if (x == null)
                return CompareResult.FailureWithExpectedAndActual("The X element is null", null, y);
            if (y == null)
                return CompareResult.FailureWithExpectedAndActual("The Y element is null", x, null);
            return CompareResult.SuccessfulResult();
        }
    }
}
