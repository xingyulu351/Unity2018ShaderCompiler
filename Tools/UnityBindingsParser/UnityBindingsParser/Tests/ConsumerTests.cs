using System;
using NUnit.Framework;
using Moq;
using System.Text;
using System.IO;
using System.Linq.Expressions;

namespace UnityBindingsParser
{
    [TestFixture]
    public class ConsumerTests
    {
        #region Support methods
        private Mock<IBindingsConsumer> consumer;
        private int buildSeqNum;
        private int execSeqNum;
        private StringBuilder sb;

        [SetUp]
        public void Init()
        {
            consumer = new Mock<IBindingsConsumer>(MockBehavior.Strict);
            buildSeqNum = 0;
            execSeqNum = 0;
            sb = new StringBuilder();
        }

        private void Input(string line) { sb.AppendLine(line); }
        private void ExpectCall(Expression<Action<IBindingsConsumer>> expr)
        {
            // Moq's sequence support is broken: http://stackoverflow.com/questions/10602264/using-moq-to-verify-calls-are-made-in-the-correct-order
            // So, we have to do it ourselves with a counter

            var localSeqNum = buildSeqNum++; // capture the shared variable into a local for the lambda to use
            consumer.When(() => execSeqNum == localSeqNum).Setup(expr).Callback(() => ++ execSeqNum);
        }

        [TearDown]
        public void Teardown()
        {
            using (var sr = new StringReader(sb.ToString()))
                Parser.Parse(sr, consumer.Object);
        }

        #endregion

        [Test]
        public void TestClass()
        {
            Input("CLASS MyClass");
            Input("END");

            ExpectCall(c => c.StartClass("MyClass"));
            ExpectCall(c => c.EndClass());
        }

        [Test]
        public void TestStaticClass()
        {
            Input("STATIC_CLASS MyClass");
            Input("END");

            ExpectCall(c => c.StartStaticClass("MyClass"));
            ExpectCall(c => c.EndStaticClass());
        }

        [Test]
        public void TestInternalStaticClass()
        {
            Input("STATIC_CLASS internal MyClass");
            Input("END");

            ExpectCall(c => c.StartStaticClass("internal MyClass"));
            ExpectCall(c => c.EndStaticClass());
        }

        [Test]
        public void TestObsoleteStaticClass()
        {
            Input("OBSOLETE warning This class is obsolete!");
            Input("STATIC_CLASS MyClass");
            Input("END");

            ExpectCall(c => c.OnObsolete("warning", "This class is obsolete!"));
            ExpectCall(c => c.StartStaticClass("MyClass"));
            ExpectCall(c => c.EndStaticClass());
        }
    }
}
