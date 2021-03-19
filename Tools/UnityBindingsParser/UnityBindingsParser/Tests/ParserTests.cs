using System;
using NUnit.Framework;
using Moq;
using System.Text;
using System.IO;
using System.Linq.Expressions;

namespace UnityBindingsParser
{
    [TestFixture]
    public class ParserTests
    {
        private Mock<IBindingsConsumer> consumer;

        [SetUp]
        public void Init()
        {
            consumer = new Mock<IBindingsConsumer>(MockBehavior.Strict);
        }

        [Test]
        public void TestThreadSafeNotSupportedException()
        {
            try
            {
                Parse("THREAD_SAFE\nSTATIC_CLASS MyClass\nEnd");
            }
            catch (NotSupportedException ex)
            {
                Assert.AreEqual("THREAD_SAFE is deprecated. Use THREAD_AND_SERIALIZATION_SAFE instead", ex.Message);
            }
        }

        [Test]
        public void TestUnsafeThrowExceptionNotSupportedException()
        {
            try
            {
                Parse("THREAD_AND_SERIALIZATION_UNSAFE_THROW_EXCEPTION\nSTATIC_CLASS MyClass\nEnd");
            }
            catch (NotSupportedException ex)
            {
                Assert.AreEqual("THREAD_AND_SERIALIZATION_UNSAFE_THROW_EXCEPTION is deprecated. Methods now throw an exception by default.", ex.Message);
            }
        }

        void Parse(string str)
        {
            using (var sr = new StringReader(str))
                Parser.Parse(sr, consumer.Object);
        }
    }
}
