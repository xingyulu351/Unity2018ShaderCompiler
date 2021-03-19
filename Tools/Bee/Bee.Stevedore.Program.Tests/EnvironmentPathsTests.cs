using System.Collections.Generic;
using Bee.TestHelpers;
using NiceIO;
using NUnit.Framework;

namespace Bee.Stevedore.Program.Tests
{
    public class EnvironmentPathsTests : StevedoreTestsBase
    {
        [Test]
        public void TypicalWindowsSetup()
        {
            var env = new Dictionary<string, string>()
            {
                {"USERPROFILE", @"C:\Users\Alice"},
                {"LOCALAPPDATA", @"C:\Users\Alice\AppData\Local"}
            };

            var ep = new EnvironmentPaths(env);

            Assert.AreEqual(new NPath(@"C:\Users\Alice"), ep.Home);
            Assert.AreEqual(new NPath(@"C:\Users\Alice"), ep.Config);
            Assert.AreEqual(new NPath(@"C:\Users\Alice\AppData\Local"), ep.Cache);
        }

        [Test]
        public void TypicalMacOSSetup()
        {
            using (var tmp = new TempDirectory())
            {
                var env = new Dictionary<string, string>()
                {
                    {"HOME", tmp.Combine("Users/bob").ToString(SlashMode.Native)}
                };

                tmp.EnsureDirectoryExists("Users/bob/Library");
                var ep = new EnvironmentPaths(env);

                Assert.AreEqual(tmp.Combine("Users/bob"), ep.Home);
                Assert.AreEqual(tmp.Combine("Users/bob/Library"), ep.Config);
                Assert.AreEqual(tmp.Combine("Users/bob/Library/Caches"), ep.Cache);
            }
        }

        [Test]
        public void TypicalLinuxSetup()
        {
            using (var tmp = new TempDirectory())
            {
                var env = new Dictionary<string, string>()
                {
                    {"HOME", tmp.Combine("home/eve").ToString(SlashMode.Native)}
                };

                var ep = new EnvironmentPaths(env);

                Assert.AreEqual(tmp.Combine("home/eve"), ep.Home);
                Assert.AreEqual(tmp.Combine("home/eve/.config"), ep.Config);
                Assert.AreEqual(tmp.Combine("home/eve/.cache"), ep.Cache);
            }
        }
    }
}
