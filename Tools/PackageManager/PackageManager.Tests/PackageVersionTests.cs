using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading;
using System.IO;
using NUnit.Framework;
using Unity.PackageManager;
using Unity.DataContract;

namespace Tests
{
    [TestFixture]
    public class PackageVersionTests
    {
        [Test]
        public void Comparison()
        {
            var v1234 = new PackageVersion("1.2.3.4");
            var v1235 = new PackageVersion("1.2.3.5");
            var v1234again = new PackageVersion("1.2.3.4");

            #region Positive
            Assert.AreEqual(v1234, v1234again);
            Assert.AreEqual(v1234again, v1234);
            Assert.AreNotEqual(v1234, v1235);

            Assert.Greater(v1235, v1234);
            Assert.GreaterOrEqual(v1235, v1234);
            Assert.GreaterOrEqual(v1234, v1234again);
            Assert.GreaterOrEqual(v1234again, v1234);

            Assert.Less(v1234, v1235);
            Assert.LessOrEqual(v1234, v1235);
            Assert.LessOrEqual(v1234, v1234again);
            Assert.LessOrEqual(v1234again, v1234);

            Assert.AreEqual(0, v1234.CompareTo(v1234again));
            Assert.AreEqual(0, v1234again.CompareTo(v1234));
            Assert.Greater(0, v1234.CompareTo(v1235));
            Assert.Less(0, v1235.CompareTo(v1234));

            Assert.AreEqual("1.2.3.4", v1234.text);
            Assert.AreEqual("1.2.3.5", v1235.text);
            #endregion

            #region Negative
            Assert.That(!(v1234 != v1234again));
            Assert.That(!(v1234again != v1234));
            Assert.That(!(v1234 == v1235));

            Assert.That(!(v1235 <= v1234));
            Assert.That(!(v1235 < v1234));
            Assert.That(!(v1234 < v1234again));
            Assert.That(!(v1234again < v1234));

            Assert.That(!(v1234 >= v1235));
            Assert.That(!(v1234 > v1235));
            Assert.That(!(v1234 > v1234again));
            Assert.That(!(v1234again > v1234));
            #endregion
        }

        [Test]
        public void SpecialComparisons()
        {
            Assert.Greater(new PackageVersion("4.3.0.18"), new PackageVersion("4.3.0"));

            Assert.Greater(new PackageVersion("4.3.0b5"), new PackageVersion("4.3.0b4"));
            Assert.Greater(new PackageVersion("4.3.12b5"), new PackageVersion("4.3.0b4"));
            Assert.Less(new PackageVersion("4.3.0b5"), new PackageVersion("4.3.0b6"));

            Assert.Less(new PackageVersion("4.3.0b5"), new PackageVersion("4.3.0.2"));
            Assert.Less(new PackageVersion("4.3"), new PackageVersion("4.3.0.2"));

            Assert.Less(new PackageVersion("4.3.0a5"), new PackageVersion("4.3.0b1"));
            Assert.Less(new PackageVersion("4.3.0a5"), new PackageVersion("4.3.01"));

            Assert.Less(new PackageVersion("4.3.0.1a5"), new PackageVersion("4.3.0.21"));
            Assert.Greater(new PackageVersion("4.3.0.31a5"), new PackageVersion("4.3.0.21"));
            Assert.Greater(new PackageVersion("4.3.0.31"), new PackageVersion("4.3.0.21"));
            Assert.Greater(new PackageVersion("4.4.0.1"), new PackageVersion("4.3.0.21"));

            var v = new PackageVersion("4.32a5");
            var v2 = new PackageVersion("4.32.0.a5");
            Assert.AreEqual(v, v2);
            Assert.Greater(v, new PackageVersion("4.3.0.21"));
            Assert.AreEqual(32, v.minor);
            Assert.AreEqual(0, v.micro);
            Assert.AreEqual("a5", v.special);
            Assert.AreEqual(4, v.parts);
        }

        [Test]
        public void NullIsValid()
        {
            var v = new PackageVersion(null);
            Assert.AreEqual(0, v.major);
            Assert.AreEqual(0, v.minor);
        }

        [Test]
        public void Parts()
        {
            var v1 = new PackageVersion("1.2");
            Assert.AreEqual(2, v1.parts);
            v1 = new PackageVersion("1.2.3");
            Assert.AreEqual(3, v1.parts);
            v1 = new PackageVersion("1.2.3a3");
            Assert.AreEqual(4, v1.parts);
            v1 = new PackageVersion("1.2.3.4");
            Assert.AreEqual(4, v1.parts);
        }

        [Test]
        public void CompatibleWith()
        {
            var v1 = new PackageVersion("4.3.0");
            var v2 = new PackageVersion("4.3.0a4");
            Assert.IsTrue(v1.IsCompatibleWith(v2));
        }

        [Test]
        public void Validation()
        {
            Assert.Throws<ArgumentException>(() => new PackageVersion("4.3.0a0"));
            Assert.Throws<ArgumentException>(() => new PackageVersion("4.3.0.0a0"));
            Assert.Throws<ArgumentException>(() => new PackageVersion("5.1.2.0alpha1"));
            Assert.Throws<ArgumentException>(() => new PackageVersion("5.1.2.alpha0"));
            Assert.Throws<ArgumentException>(() => new PackageVersion("5"));
            Assert.Throws<ArgumentException>(() => new PackageVersion("5.a"));
            Assert.Throws<ArgumentException>(() => new PackageVersion("5.1.2.4alpha1b0d"));

            Assert.DoesNotThrow(() => new PackageVersion("5.1.0.alpha1"));
            Assert.DoesNotThrow(() => new PackageVersion("5.1.2.4alpha1b01d"));
        }
    }
}
