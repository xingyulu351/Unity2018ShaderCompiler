using System;
using NUnit.Framework;

namespace AssemblyWithTests1
{
    [AttributeUsage(AttributeTargets.Method, AllowMultiple = false)]
    public class NestedTestCaseAttribute : CategoryAttribute
    {
        public NestedTestCaseAttribute() : base("NestedTestCase") {}
    }

    [TestFixture]
    public class TestCases
    {
        [Test]
        [TestCase("1")]
        [TestCase("2")]
        [TestCase("3")]
        [NestedTestCase]
        public void TestCase1(string testCase)
        {
        }

        [TestCase("1")]
        [TestCase("2")]
        [NestedTestCase]
        public void TestCase2(string testCase)
        {
        }
    }
}
