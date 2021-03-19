using System;
using System.IO;
using NUnit.Framework;

namespace Unity.CommonTools.Test
{
    [TestFixture]
    public class DebugViewHelperTests
    {
        private StringWriter m_Output;

        private static TextReader MakeInput(params string[] lines)
        {
            return new StringReader(string.Join(Environment.NewLine, lines));
        }

        private static string MakeExpectedOutput(params string[] lines)
        {
            return string.Join(Environment.NewLine, lines) + Environment.NewLine;
        }

        [SetUp]
        public void Setup()
        {
            m_Output = new StringWriter();
        }

        [Test]
        public void FetchProcessMessages_NoInputData_WritesNothingToOutput()
        {
            var input = StreamReader.Null;
            DebugViewHelper.FetchProcessMessages(0, input, m_Output);
            Assert.That(m_Output.ToString(), Is.Empty);
        }

        [Test]
        public void FetchProcessMessages_OneProcessMessages_WritesAllMessages()
        {
            var input = MakeInput("[100]message1", "[100]messsage2");
            DebugViewHelper.FetchProcessMessages(100, input, m_Output);
            Assert.That(m_Output.ToString(), Is.EquivalentTo(MakeExpectedOutput("[100]message1", "[100]messsage2")));
        }

        [Test]
        public void FetchProcessMessages_TwoProcessMessages_WritesMessagesForRequestedProcess()
        {
            var input = MakeInput("[100]message1", "[200]messsage2");
            DebugViewHelper.FetchProcessMessages(200, input, m_Output);
            Assert.That(m_Output.ToString(), Is.EquivalentTo(MakeExpectedOutput("[200]messsage2")));
        }

        [Test]
        public void FetchProcessMessages_NoProcessMessages_WritesNothing()
        {
            var input = MakeInput("message1", "messsage2", "100", "0");
            DebugViewHelper.FetchProcessMessages(200, input, m_Output);
            Assert.That(m_Output.ToString(), Is.Empty);
        }
    }
}
