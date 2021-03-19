using System;
using System.Linq;
using NUnit.Framework;

namespace Bee.NativeProgramSupport.Building.Tests
{
    [TestFixture]
    public class SdkTests
    {
        class Sdk : MockSdk
        {
            public override Version Version { get; }
            public Sdk(Version version) => Version = version;
        }

        [Test]
        public void Matches_ReturnTrue_ForMatchingFixedVersions()
        {
            Assert.True(new Sdk(new Version(1, 1, 1, 1)).Matches(new Version(1, 1, 1, 1)));
            Assert.True(new Sdk(new Version(0, 1, 1, 1)).Matches(new Version(0, 1, 1, 1)));
            Assert.True(new Sdk(new Version(0, 0, 1, 1)).Matches(new Version(0, 0, 1, 1)));
            Assert.True(new Sdk(new Version(0, 0, 0, 1)).Matches(new Version(0, 0, 0, 1)));
            Assert.True(new Sdk(new Version(0, 0, 0, 0)).Matches(new Version(0, 0, 0, 0)));
            Assert.True(new Sdk(new Version(1, 0, 0, 0)).Matches(new Version(1, 0, 0, 0)));
            Assert.True(new Sdk(new Version(1, 1, 0, 0)).Matches(new Version(1, 1, 0, 0)));
            Assert.True(new Sdk(new Version(1, 1, 1, 0)).Matches(new Version(1, 1, 1, 0)));
        }

        [Test]
        public void Matches_ReturnFalse_ForMismatchingVersions()
        {
            Assert.False(new Sdk(new Version(1, 1, 1, 1)).Matches(new Version(2, 1, 1, 1)));
            Assert.False(new Sdk(new Version(0, 1, 1, 1)).Matches(new Version(1, 1, 1, 1)));
            Assert.False(new Sdk(new Version(0, 0, 1, 1)).Matches(new Version(0, 1, 1, 1)));
            Assert.False(new Sdk(new Version(0, 0, 0, 1)).Matches(new Version(0, 0, 0, 2)));
            Assert.False(new Sdk(new Version(0, 0, 0, 0)).Matches(new Version(0, 0, 0, 1)));
            Assert.False(new Sdk(new Version(1, 0, 0, 0)).Matches(new Version(1, 0, 0, 1)));
            Assert.False(new Sdk(new Version(1, 1, 0, 0)).Matches(new Version(1, 1, 0, 1)));
            Assert.False(new Sdk(new Version(1, 1, 1, 0)).Matches(new Version(1, 1, 1, 1)));
        }

        [Test]
        public void Matches_ReturnTrue_ForMatchingMinMaxVersions()
        {
            Assert.True(new Sdk(new Version(1, 1, 1, 1)).Matches(new Version(1, 1, 1, 1), new Version(1, 1, 1, 1)));
            Assert.True(new Sdk(new Version(1, 1, 1, 1)).Matches(new Version(1, 1, 1, 0), new Version(1, 1, 1, 1)));
            Assert.True(new Sdk(new Version(1, 1, 1, 1)).Matches(new Version(0, 1, 1, 1), new Version(1, 1, 1, 1)));
            Assert.True(new Sdk(new Version(1, 1, 1, 1)).Matches(new Version(1, 1, 1, 1), new Version(1, 1, 1, 2)));
            Assert.True(new Sdk(new Version(1, 1, 1, 1)).Matches(new Version(1, 1, 1, 1), new Version(2, 0, 0, 0)));
            Assert.True(new Sdk(new Version(1, 1, 1, 1)).Matches(new Version(0, 1, 1, 1), new Version(2, 0, 0, 0)));
            Assert.True(new Sdk(new Version(1, 1, 1, 1)).Matches(new Version(1, 1, 1, 0), new Version(1, 1, 1, 2)));
        }

        [Test]
        public void Matches_ReturnFalse_ForMismatchingMinMaxVersions()
        {
            Assert.False(new Sdk(new Version(1, 1, 1, 1)).Matches(new Version(1, 1, 1, 2), new Version(1, 1, 1, 1)));
            Assert.False(new Sdk(new Version(1, 1, 1, 1)).Matches(new Version(1, 1, 1, 1), new Version(1, 1, 1, 0)));
            Assert.False(new Sdk(new Version(1, 1, 1, 1)).Matches(new Version(2, 1, 1, 1), new Version(1, 1, 1, 1)));
            Assert.False(new Sdk(new Version(1, 1, 1, 1)).Matches(new Version(1, 1, 1, 1), new Version(0, 1, 1, 1)));
        }

        [Test]
        public void LatestMatching_IgnoresOmittedParameters()
        {
            var expected = new Sdk(new Version(1, 2, 3, 4));
            var sdks = new[] { expected };

            Assert.AreEqual(expected, sdks.LatestMatching());
            Assert.AreEqual(expected, sdks.LatestMatching(1));
            Assert.AreEqual(expected, sdks.LatestMatching(1, 2));
            Assert.AreEqual(expected, sdks.LatestMatching(1, 2, 3));
            Assert.AreEqual(expected, sdks.LatestMatching(1, 2, 3, 4));
            Assert.AreEqual(expected, sdks.LatestMatching(minor: 2));
            Assert.AreEqual(expected, sdks.LatestMatching(minor: 2, build: 3));
            Assert.AreEqual(expected, sdks.LatestMatching(minor: 2, build: 3, revision: 4));
            Assert.AreEqual(expected, sdks.LatestMatching(build: 3));
            Assert.AreEqual(expected, sdks.LatestMatching(build: 3, revision: 4));
            Assert.AreEqual(expected, sdks.LatestMatching(revision: 4));
        }

        [Test]
        public void LatestMatching_ReturnLatestVersion_WhichMatchesParameters()
        {
            var sdks = new[]
            {
                new Sdk(new Version(1, 2, 3, 4)),
                new Sdk(new Version(1, 2, 3, 5)),
                new Sdk(new Version(1, 2, 3, 6)),
                new Sdk(new Version(1, 2, 3, 7)),
                new Sdk(new Version(1, 2, 3, 8)),
                new Sdk(new Version(1, 2, 3, 9)),
                new Sdk(new Version(1, 2, 4, 0)),
                new Sdk(new Version(1, 2, 4, 1)),
                new Sdk(new Version(9, 0, 0, 0)),
                new Sdk(new Version(0, 0, 0, 0)),
            };
            Assert.AreEqual(new Sdk(new Version(1, 2, 4, 1)), sdks.LatestMatching(1));
            Assert.AreEqual(new Sdk(new Version(1, 2, 3, 9)), sdks.LatestMatching(1, 2, 3));
            Assert.AreEqual(new Sdk(new Version(1, 2, 4, 1)), sdks.LatestMatching(1, 2, 4));
            Assert.AreEqual(new Sdk(new Version(1, 2, 4, 0)), sdks.LatestMatching(1, 2, 4, 0));
            Assert.AreEqual(new Sdk(new Version(9, 0, 0, 0)), sdks.LatestMatching(9));
        }

        [Test]
        public void LatestMatching_ReturnNull_WhenNoSdkMatches()
        {
            var sdks = new[] { new Sdk(new Version(1, 2, 3, 4)) };
            Assert.Null(sdks.LatestMatching(99));
            Assert.Null(sdks.LatestMatching(1, 99));
            Assert.Null(sdks.LatestMatching(1, 2, 99));
            Assert.Null(sdks.LatestMatching(1, 2, 3, 99));
        }

        [Test]
        public void LatestMatching_WithVersion_RequiresExactMatch()
        {
            var sdks = new[] { new Sdk(new Version(1, 2, 3, 4)) };
            Assert.NotNull(sdks.LatestMatching(new Version(1, 2, 3, 4)));
            Assert.Null(sdks.LatestMatching(new Version(1, 2)));
            Assert.Null(sdks.LatestMatching(new Version(1, 2, 3)));
            Assert.Null(sdks.LatestMatching(new Version(0, 2, 3, 4)));
            Assert.Null(sdks.LatestMatching(new Version(1, 0, 3, 4)));
            Assert.Null(sdks.LatestMatching(new Version(1, 2, 0, 4)));
        }

        [Test]
        public void LatestMatching_WithMinMax_ReturnLatestSdk_WhichMatchesTheInterval()
        {
            var sdks = new[]
            {
                new Sdk(new Version(1, 2, 3, 4)),
                new Sdk(new Version(1, 2, 3, 5)),
                new Sdk(new Version(1, 2, 3, 6)),
                new Sdk(new Version(1, 2, 3, 7)),
                new Sdk(new Version(1, 2, 3, 8)),
                new Sdk(new Version(1, 2, 3, 9)),
                new Sdk(new Version(1, 2, 4, 0)),
                new Sdk(new Version(1, 2, 4, 1)),
                new Sdk(new Version(0, 0, 0, 0)),
            };
            Assert.AreEqual(new Sdk(new Version(1, 2, 3, 4)), sdks.LatestMatching(new Version(1, 2, 3, 4), new Version(1, 2, 3, 4)));
            Assert.AreEqual(new Sdk(new Version(1, 2, 3, 5)), sdks.LatestMatching(new Version(1, 2, 3, 4), new Version(1, 2, 3, 5)));
            Assert.AreEqual(new Sdk(new Version(1, 2, 4, 1)), sdks.LatestMatching(new Version(1, 2, 3, 4), new Version(1, 2, 4, 1)));
            Assert.AreEqual(new Sdk(new Version(1, 2, 4, 1)), sdks.LatestMatching(new Version(1, 2, 3, 4), new Version(1, 2, 5, 0)));
            Assert.AreEqual(new Sdk(new Version(1, 2, 4, 1)), sdks.LatestMatching(new Version(1, 2, 3, 4), new Version(99, 0, 0, 0)));
        }

        [Test]
        public void Latest_ReturnLatestSdk_AvailableInCollection()
        {
            var sdks = new[]
            {
                new Sdk(new Version(1, 2, 3, 4)),
                new Sdk(new Version(1, 2, 3, 5)),
                new Sdk(new Version(1, 2, 3, 6)),
                new Sdk(new Version(1, 2, 3, 7)),
                new Sdk(new Version(1, 2, 3, 8)),
                new Sdk(new Version(1, 2, 3, 9)),
                new Sdk(new Version(1, 2, 4, 0)),
                new Sdk(new Version(1, 2, 4, 1)),
                new Sdk(new Version(0, 0, 0, 0)),
            };
            Assert.AreEqual(new Sdk(new Version(1, 2, 4, 1)), sdks.Latest());
        }
    }

    [TestFixture]
    class SdkLocatorTests
    {
        class DefaultSdk : MockSdk {}

        class MySdkLocator : MockSdkLocator
        {
            public override MockSdk UserDefault { get; } = new DefaultSdk();
        }

        private static readonly MySdkLocator Locator = new MySdkLocator();

        [Test]
        public void All_Contain_Dummy()
        {
            Assert.True(Locator.All.Contains(Locator.Dummy));
        }

        [Test]
        public void All_Contain_UserDefault()
        {
            Assert.True(Locator.All.Contains(Locator.UserDefault));
        }

        [Test]
        public void All_DoesNotContain_Null()
        {
            Assert.False(Locator.All.Contains(null));
        }
    }
}
