using System;
using System.Linq;
using NUnit.Framework;

namespace Unity.BuildSystem.Tests
{
    public class ExtractEnabledDefinesTests
    {
        [Test]
        public void Empty_ReturnsEmpty()
        {
            var defines = new string[0];

            var extracted = PlatformSupportUtils.ExtractEnabledDefines(defines);

            Assert.IsNotNull(extracted);
            Assert.IsEmpty(extracted);
        }

        [Test]
        public void Selects_OnlyEnabledAndEmptyMacros()
        {
            var defines = new[] { "ENABLE_A", "ENABLE_B=1", "ENABLE_C=0", "ENAYBLE_FOO", "EENABLE_FOO=1" };

            var extracted = PlatformSupportUtils.ExtractEnabledDefines(defines).ToList();

            Assert.AreEqual(2, extracted.Count);
            Assert.Contains("ENABLE_A", extracted);
            Assert.Contains("ENABLE_B", extracted);
        }

        [Test]
        public void Throws_OnUnexpectedFormat()
        {
            // ReSharper disable ReturnValueOfPureMethodIsNotUsed
            Assert.Throws<Exception>(() => PlatformSupportUtils.ExtractEnabledDefines(new[] { "ENABLE_A=" }).ToList());
            Assert.Throws<Exception>(() => PlatformSupportUtils.ExtractEnabledDefines(new[] { "ENABLE_A=2" }).ToList());
            Assert.Throws<Exception>(() => PlatformSupportUtils.ExtractEnabledDefines(new[] { "ENABLE_A=B" }).ToList());
            // ReSharper restore ReturnValueOfPureMethodIsNotUsed
        }
    }
}
