using System;
using System.Text;
using System.Collections.Generic;
using System.Linq;
using Microsoft.VisualStudio.TestTools.UnitTesting;

namespace HistoryGenerator.Tests
{
    [TestClass]
    public class VersionNumberTests
    {
        [TestMethod]
        public void TestCompareGreaterThanMajor()
        {
            VersionNumber low = new VersionNumber("1.0.0");
            VersionNumber high = new VersionNumber("2.0.0");

            Assert.AreEqual(-1, low.CompareTo(high));
        }

        [TestMethod]
        public void TestCompareGreaterThanMinor()
        {
            VersionNumber low = new VersionNumber("1.1.0");
            VersionNumber high = new VersionNumber("1.2.0");

            Assert.AreEqual(-1, low.CompareTo(high));
        }

        [TestMethod]
        public void TestCompareGreaterThanPoint()
        {
            VersionNumber low = new VersionNumber("1.0.0");
            VersionNumber high = new VersionNumber("1.0.1");

            Assert.AreEqual(-1, low.CompareTo(high));
        }

        [TestMethod]
        public void TestCompareEquals()
        {
            VersionNumber low = new VersionNumber("1.0.0");
            VersionNumber high = new VersionNumber("1.0.0");

            Assert.AreEqual(0, low.CompareTo(high));
        }
    }
}
