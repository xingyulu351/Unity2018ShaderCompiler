using System;
using System.IO;

namespace Unity.CommonTools
{
    public class TeamCityTestResultWriter : ITestResultWriter
    {
        private TextWriter outputStream;
        public bool DebugOutput { get; set; }

        public TeamCityTestResultWriter()
        {
            outputStream = Console.Out;
        }

        public void TestPassed(string testName, long time)
        {
            DoMsg(string.Format("testStarted name='{0}'", testName));
            DoMsg(string.Format("testFinished name='{0}'", testName));
        }

        public void TestIgnored(string testName, string reason)
        {
            DoMsg(string.Format("testStarted name='{0}'", testName));
            DoMsg(string.Format("testIgnored name='{0}' message='{1}'", testName, EscapeForTeamcity(reason)));
            DoMsg(string.Format("testFinished name='{0}'", testName));
        }

        public void TestInconclusive(string testName, string reason)
        {
            // Let's not change behaviour in teamcity...
            TestIgnored(testName, reason);
        }

        public void TestFailed(string testName, string reason, long time)
        {
            DoMsg(string.Format("testStarted name='{0}'", testName));
            DoMsg(string.Format("testFailed name='{0}' message='{1}'", testName, EscapeForTeamcity(reason)));
            DoMsg(string.Format("testFinished name='{0}'", testName));
        }

        public void TestSuiteFailed(string reason)
        {
            DoMsg(string.Format("buildStatus status='FAILURE' text='{0}'", EscapeForTeamcity(reason)));
        }

        public static string EscapeForTeamcity(string msg)
        {
            msg = msg.Replace('\'', '_');
            msg = msg.Replace('\n', ' ');
            msg = msg.Replace('\r', ' ');
            msg = msg.Replace('[', '(');
            msg = msg.Replace(']', ')');
            if (msg.Length > 300)
                msg = msg.Substring(0, 300);
            return msg + " ...";
        }

        public void TestRunFinished()
        {
        }

        private void DoMsg(string msg)
        {
            outputStream.WriteLine("##teamcity[{0}]", msg);
            if (DebugOutput)
                outputStream.WriteLine("WroteTeamcityMsg: " + msg);
        }
    }
}
