using Bee.NativeProgramSupport.Running;
using NUnit.Framework;

namespace NiceIO.Tests
{
    [TestFixture]
    public class NPathWithoutInvalidCharacters
    {
        [Test]
        public void Throws_ForReplacementChar_IsInvalidFileNameChar()
        {
            Assert.That(() => new NPath("a/b").WithoutInvalidCharacters('/'), Throws.ArgumentException);
        }

        [Test]
        public void DoesNothingForValidPath()
        {
            Assert.That(new NPath("a/b").WithoutInvalidCharacters('_'), Is.EqualTo(new NPath("a/b")));
        }

        [Test]
        public void DoesNothingForAbsoluteWindowsPath()
        {
            Assert.That(new NPath("C:/a/b").WithoutInvalidCharacters('_'), Is.EqualTo(new NPath("C:/a/b")));
        }

        [Test]
        [Platform(Include = "Win")]
        public void ReplacesInvalidPathChars_Windows()
        {
            // Test some typical invalid windows chars. No point in going through all of them.
            Assert.That(new NPath("|/:/<?>/\n").WithoutInvalidCharacters('_'), Is.EqualTo(new NPath("_/_/___/_")));
        }

        [Test]
        public void ReplacesInvalidPathChars()
        {
            Assert.That(new NPath("test\0/path").WithoutInvalidCharacters('_'), Is.EqualTo(new NPath("test_/path")));
        }
    }
}
