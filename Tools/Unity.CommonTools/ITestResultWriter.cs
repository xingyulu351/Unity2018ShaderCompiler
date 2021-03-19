namespace Unity.CommonTools
{
    public interface ITestResultWriter
    {
        void TestPassed(string testName, long time);
        void TestIgnored(string testName, string reason);
        void TestInconclusive(string testName, string reason);
        void TestFailed(string testName, string reason, long time);
        void TestSuiteFailed(string reason);
        void TestRunFinished();
    }
}
