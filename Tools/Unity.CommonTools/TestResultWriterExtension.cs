namespace Unity.CommonTools
{
    public static class TestResultWriterExtensions
    {
        public static void IncomingResult(this ITestResultWriter writer, string testname, TestExecutionResult result, string reason, long time)
        {
            switch (result)
            {
                case TestExecutionResult.Success:
                    writer.TestPassed(testname, time);
                    break;
                case TestExecutionResult.Failure:
                    writer.TestFailed(testname, reason, time);
                    break;
                case TestExecutionResult.Inconclusive:
                    writer.TestInconclusive(testname, reason);
                    break;
            }
        }
    }
}
