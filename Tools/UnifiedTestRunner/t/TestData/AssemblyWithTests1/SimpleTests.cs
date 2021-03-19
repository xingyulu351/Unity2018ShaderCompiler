using System;
using NUnit.Framework;

namespace AssemblyWithTests1
{
    [AttributeUsage(AttributeTargets.Method, AllowMultiple = false)]
    public class SimpleAttribute : CategoryAttribute
    {
        public SimpleAttribute() : base("Simple") {}
    }

    [TestFixture]
    public class SimpleTests
    {
        [Test]
        [Simple]
        public void SimpleTest1()
        {
        }
    }
}
