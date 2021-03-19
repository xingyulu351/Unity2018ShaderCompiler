namespace UnderlyingModel.MemDoc.Comparers
{
    public class CompareResult
    {
        public bool Success { get; private set; }
        public string CompareMessage { get; private set; }

        public static CompareResult SuccessfulResult()
        {
            return new CompareResult { Success = true };
        }

        public static CompareResult FailureWithExpectedAndActual(string message, object expValue, object actValue)
        {
            return new CompareResult
            {
                Success = false,
                CompareMessage = string.Format("{0}:\nexp={1}, \nact={2}", message, expValue, actValue)
            };
        }
    }

    public interface IComparerWithError<T>
    {
        CompareResult Compare(T x, T y);
    }
}
