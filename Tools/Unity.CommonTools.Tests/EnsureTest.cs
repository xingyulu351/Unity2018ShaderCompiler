using System;
using NUnit.Framework;

namespace Unity.CommonTools.Test
{
    public class EnsureTest
    {
        [Test]
        public void IsNotNull_WhenArgumentIsNull_ThenArgumentNullExceptionIsThrown()
        {
            object value = null;

            TestDelegate act = () => Ensure.IsNotNull(value, "foo");

            Assert.That(act, Throws.TypeOf<ArgumentNullException>()
                .And.Property("ParamName").EqualTo("foo"));
        }

        [Test]
        public void IsNotNull_WhenArgumentIsNotNull_ThenNothingIsThrown()
        {
            object value = new object();

            TestDelegate act = () => Ensure.IsNotNull(value, "foo");

            Assert.That(act, Throws.Nothing);
        }

        [Test]
        public void IsNotNullOrEmpty_WhenArgumentIsNull_ThenArgumentNullExceptionIsThrown()
        {
            string value = null;

            TestDelegate act = () => Ensure.IsNotNullOrEmpty(value, "foo");

            Assert.That(act, Throws.TypeOf<ArgumentNullException>()
                .And.Property("ParamName").EqualTo("foo"));
        }

        [Test]
        public void IsNotNullOrEmpty_WhenArgumentIsEmpty_ThenArgumentExceptionIsThrown()
        {
            string value = string.Empty;

            TestDelegate act = () => Ensure.IsNotNullOrEmpty(value, "foo");

            Assert.That(act, Throws.ArgumentException
                .And.Property("ParamName").EqualTo("foo"));
        }

        [Test]
        public void IsNotNullOrEmpty_WhenArgumentIsNotNullOrEmpty_ThenNothingIsThrown()
        {
            string value = "foobarbaz";

            TestDelegate act = () => Ensure.IsNotNullOrEmpty(value, "foo");

            Assert.That(act, Throws.Nothing);
        }
    }
}
