using System.IO;
using NUnit.Framework;

namespace UnderlyingModel.Tests
{
    [TestFixture]
    public class PlatformTests
    {
        [OneTimeSetUp]
        public void Init()
        {
            Directory.SetCurrentDirectory(TestContext.CurrentContext.TestDirectory);
            DocGenSettings.Instance.LoadIniFiles();
        }

        [TestCase("C:/unity/bugfixes/Documentation/ApiDocs/UnityEngine", "Default")]
        [TestCase("C:/unity/bugfixes/PlatformDependent/XboxOne/ApiDocs/UnityEngine", "XboxOne")]
        [TestCase("C:/unity/tntdoc/PlatformDependent/PS4/ApiDocs/UnityEngine", "PS4")]
        [TestCase("C:/unity/bugfixes/PlatformDependent/blah/ApiDocs/UnityEngine", "Unknown")]
        [TestCase("", "Unknown")]
        public void PathResolvesToPlatform(string path, string expectedPlatform)
        {
            var actualPlatform = DocGenSettings.Instance.PlatformsInfo.GetPlatformNameFromPath(path);
            Assert.AreEqual(expectedPlatform, actualPlatform);
        }
    }
}
