using System;
using NUnit.Framework;
using static Bee.Toolchain.Android.AndroidSdk;

namespace Bee.Toolchain.Android.Tests
{
    class AndroidSdkVersionTests
    {
        [Test]
        public void GetToolsVersion_ValidSimpleVersion_Works()
        {
            Assert.AreEqual(new Version(7, 0), GetToolsVersionFromFileContents("Pkg.Revision=7"));
            Assert.AreEqual(new Version(7, 8), GetToolsVersionFromFileContents("Pkg.Revision=7.8"));
            Assert.AreEqual(new Version(1, 2, 3), GetToolsVersionFromFileContents("Pkg.Revision=1.2.3"));
            Assert.AreEqual(new Version(1, 2, 3, 4), GetToolsVersionFromFileContents("Pkg.Revision=1.2.3.4"));
            Assert.AreEqual(new Version(1, 2, 3), GetToolsVersionFromFileContents("\nPkg.Revision=1.2.3\n"));
        }

        [Test]
        public void GetToolsVersion_ValidComplexVersion_Works()
        {
            Assert.AreEqual(new Version(1, 2), GetToolsVersionFromFileContents("Pkg.Revision=1.2 # test"));
            Assert.AreEqual(new Version(1, 2), GetToolsVersionFromFileContents("Pkg.Revision=1.2 rc1"));
            Assert.AreEqual(new Version(1, 2), GetToolsVersionFromFileContents("Pkg.Revision=1.2\trc1"));
            // multiple entries present; invalid ones ignored
            Assert.AreEqual(new Version(3, 4), GetToolsVersionFromFileContents("Pkg.Revision=Foobar\nPkg.Revision=3.4"));
        }

        [Test]
        public void GetToolsVersion_ValidVersion_CommentsOrWhiteSpace_Works()
        {
            Assert.AreEqual(new Version(8, 0), GetToolsVersionFromFileContents("# Pkg.Revision=7\nPkg.Revision=8"));
            Assert.AreEqual(new Version(1, 2, 3), GetToolsVersionFromFileContents(" Pkg.Revision = 1.2.3"));
            Assert.AreEqual(new Version(1, 2, 3), GetToolsVersionFromFileContents("   Pkg.Revision\t= 1.2.3   "));
        }

        [Test]
        public void GetToolsVersion_NoVersionFound_ReturnsZero()
        {
            Assert.AreEqual(new Version(0, 0), GetToolsVersionFromFileContents(""));
            Assert.AreEqual(new Version(0, 0), GetToolsVersionFromFileContents("\n"));
            Assert.AreEqual(new Version(0, 0), GetToolsVersionFromFileContents("foo\nbar\n"));
            Assert.AreEqual(new Version(0, 0), GetToolsVersionFromFileContents("1.2.3"));
        }

        [Test]
        public void GetToolsVersion_InvalidVersion_ReturnsZero()
        {
            Assert.AreEqual(new Version(0, 0), GetToolsVersionFromFileContents("Pkg.Revision="));
            Assert.AreEqual(new Version(0, 0), GetToolsVersionFromFileContents("Pkg.Revision=A"));
            Assert.AreEqual(new Version(0, 0), GetToolsVersionFromFileContents("Pkg.Revision=Foobar Baz"));
            Assert.AreEqual(new Version(0, 0), GetToolsVersionFromFileContents("Pkg.Revision=\t  "));
            // we don't handle versions with more than four fields
            Assert.AreEqual(new Version(0, 0), GetToolsVersionFromFileContents("Pkg.Revision=1.2.3.4.5"));
        }
    }
}
