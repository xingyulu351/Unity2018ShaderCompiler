using System;
using System.Collections.Generic;
using System.IO;
using Bee.TestHelpers;
using NiceIO;
using NUnit.Framework;

namespace Bee.Stevedore.Program.Tests
{
    public class StevedoreConfigTests : StevedoreTestsBase
    {
        [TestCase("USERPROFILE", "X:/nosuchpath/")]
        [TestCase("HOME", "/foo/bar/users/bob")]
        public void Read_CacheFolder(string homeEnvVar, string homeFolder)
        {
            var warnings = new List<string>();
            var config = new StevedoreConfig();
            Environment.SetEnvironmentVariable(homeEnvVar, homeFolder);
            config.ReadEnvironmentDefaults(warnings.Add);
            config.Read(new StringReader(@"
cache-folder = ~/Steve
"), warnings.Add);
            Assert.AreEqual(new NPath(homeFolder).Combine("Steve"), config.CacheFolder);
            Assert.That(warnings, Is.Empty);
        }

        [Test]
        public void Read_Comments()
        {
            ReadConfig(out var config, @"
# this is a comment
; this is also a comment
   # cache-folder = more comment
cache-folder = test # this is NOT a comment
# etc.
");
            Assert.AreEqual(new NPath("test # this is NOT a comment"), config.CacheFolder);
        }

        [Test]
        public void Read_Garbage()
        {
            ReadConfig(out var config, out var warnings, @"garbage
invalid.sætting = 1
unknown.setting = 2
    cache-folder=valid
cache-folder
repo.foo.url = http://foo
");
            Assert.AreEqual(new NPath("valid"), config.CacheFolder);
            Assert.That(warnings, Is.EqualTo(new[]
            {
                "invalid config file syntax: garbage",
                "invalid config file syntax: invalid.sætting = 1",
                "unrecognized config setting: unknown.setting",
                "invalid config file syntax: cache-folder",
                "Invalid 'repo.foo.url' value (a slash must follow the hostname in a repository URL prefix): http://foo",
            }));
        }

        [Test]
        public void Read_RepoConfig()
        {
            ReadConfig(out var config, @"
repo.public.http-header = X-Api-Key: Y29tbWx
repo.public.url = http://example.org/
");
            var rc = config.RepoConfigFor("public");
            Assert.AreEqual($"http://example.org/", rc.Url);
            Assert.AreEqual("X-Api-Key: Y29tbWx", rc.CustomHttpHeader);
        }

        [TestCase("http://aa", "http://aa", "proxy=http://aa")]
        [TestCase("http://aa", null, "proxy.http=http://aa")]
        [TestCase(null, "http://bb", "proxy.https=http://bb")]
        [TestCase("http://bb", "http://cc", "proxy.http=http://aa  proxy=http://bb  proxy.https=http://cc")]
        public void Read_HttpProxy(string expectedHttpProxy, string expectedHttpsProxy, string configText)
        {
            configText = configText.Replace(' ', '\n'); // because Rider chokes on newlines in TestCase args...

            ReadConfig(out var config, configText);
            Assert.AreEqual(expectedHttpProxy, config.DefaultRepoConfig.HttpProxyUrl);
            Assert.AreEqual(expectedHttpsProxy, config.DefaultRepoConfig.HttpsProxyUrl);
        }

        [Test]
        public void Read_Timeout()
        {
            ReadConfig(out var config, "timeout = 12345");
            Assert.AreEqual(12345, config.DefaultRepoConfig.Timeout);
        }

        [Test]
        public void ReadEnvironmentDefaults_CacheFolder()
        {
            Environment.SetEnvironmentVariable("LOCALAPPDATA", "/my/user/cache/");
            ReadEnvironmentConfig(out var config);
            Assert.AreEqual(new NPath("/my/user/cache/Stevedore"), config.CacheFolder);
        }

        [TestCase("USERPROFILE")]
        [TestCase("HOME")]
        public void ReadEnvironmentDefaults_UserConfigExists(string homeEnvVar)
        {
            var warnings = new List<string>();
            var config = new StevedoreConfig();
            using (var homeDir = new TempDirectory())
            {
                Environment.SetEnvironmentVariable(homeEnvVar, homeDir.ToString(SlashMode.Native));
                var configFile = homeDir.Combine("Stevedore.conf");
                configFile.WriteAllText("cache-folder = alrighty");

                Assert.AreEqual(null, config.CacheFolder);
                config.ReadEnvironmentDefaults(warnings.Add);
                Assert.AreEqual(new NPath("alrighty"), config.CacheFolder);
                Assert.That(warnings, Is.Empty);
                Assert.That(config.ConfigFiles, Contains.Item(configFile));
            }
        }

        [Test]
        public void ReadEnvironmentDefaults_MultipleUserConfigs_LinuxLayout()
        {
            var warnings = new List<string>();
            var config = new StevedoreConfig();
            using (var homeDir = new TempDirectory())
            {
                var confDir = homeDir.CreateDirectory(".config");
                Environment.SetEnvironmentVariable("HOME", homeDir.ToString(SlashMode.Native));
                var configFile1 = confDir.Combine("Stevedore.conf");
                var configFile2 = homeDir.Combine("Stevedore.conf");
                configFile1.WriteAllText("cache-folder = one");
                configFile2.WriteAllText("cache-folder = two");

                Assert.AreEqual(null, config.CacheFolder);
                config.ReadEnvironmentDefaults(warnings.Add);
                Assert.AreEqual(new NPath("two"), config.CacheFolder);
                Assert.That(warnings, Is.Empty);
                Assert.That(config.ConfigFiles, Is.EqualTo(new[] { configFile1, configFile2 }));
            }
        }

        // Test both 'http_proxy' (non-standard but conventional casing, as e.g. used
        // in libcurl) and 'HTTP_PROXY' (standard casing for "well-known" env vars).
        [TestCase("http_proxy", "http://example.org")]
        [TestCase("HTTP_PROXY", "http://example.com:1234")]
        public void ReadEnvironmentDefaults_HttpProxy_FromEnvironmentVariable(string variableName, string value)
        {
            Environment.SetEnvironmentVariable(variableName, value);
            ReadEnvironmentConfig(out var config);
            Assert.AreEqual(value, config.DefaultRepoConfig.HttpProxyUrl);
        }

        // Test only 'HTTPS_PROXY' casing. 'https_proxy' casing is neither standard nor conventional.
        [TestCase("HTTPS_PROXY", "http://example.com:1234")]
        public void ReadEnvironmentDefaults_HttpsProxy_FromEnvironmentVariable(string variableName, string value)
        {
            Environment.SetEnvironmentVariable(variableName, value);
            ReadEnvironmentConfig(out var config);
            Assert.AreEqual(value, config.DefaultRepoConfig.HttpsProxyUrl);
        }

        [Test]
        public void ReadEnvironmentDefaults_HttpProxy_UserConfigOverridesEnvironmentVariable()
        {
            using (var homeDir = new TempDirectory())
            {
                Environment.SetEnvironmentVariable("http_proxy", "http://1.2.3.4");
                Environment.SetEnvironmentVariable("HOME", homeDir.ToString(SlashMode.Native));
                homeDir.Combine("Stevedore.conf").WriteAllText("proxy.http = http://user-override");
                ReadEnvironmentConfig(out var config);
                Assert.AreEqual("http://user-override", config.DefaultRepoConfig.HttpProxyUrl);
            }
        }

        [Test]
        public void ReadEnvironmentDefaults_BuildFarmUserConfig()
        {
            using (var homeDir = new TempDirectory())
            {
                Environment.SetEnvironmentVariable("http_proxy", "http://donotuse");
                Environment.SetEnvironmentVariable("HTTPS_PROXY", "http://notthiseither");
                Environment.SetEnvironmentVariable("HOME", homeDir.ToString(SlashMode.Native));
                homeDir.Combine("Stevedore.conf").WriteAllText(@"
                    # ~/Stevedore.conf (contact: #devs-stevedore)

                    # Do not use OAuth interactive authorization
                    oauth.client-id =

                    # No proxy needed to reach build farm mirror (and as of this writing, Stevedore does not support NO_PROXY)
                    proxy =

                    # The timeout limits time spent waiting for the HTTP response headers.
                    # For a cache miss, bfartifactory won't respond until it has downloaded
                    # the entire artifact from the origin server. 100s should be enough for
                    # even the largest artifacts and a slow origin (e.g. 1 GB at 10 MB/s).
                    timeout = 100000

                    # Use build farm mirror for retrieving all artifacts
                    repo.public.url = https://bfartifactory.bf.unity3d.com/artifactory/public-generic-stevedore-public/
                    repo.main.url = https://bfartifactory.bf.unity3d.com/artifactory/public-generic-stevedore-main/
                    repo.unity-internal.url = https://bfartifactory.bf.unity3d.com/artifactory/public-generic-stevedore-unity-internal/
                    repo.testing.url = https://bfartifactory.bf.unity3d.com/artifactory/public-generic-stevedore-testing/
                    repo.ms-consoles.url = https://bfartifactory.bf.unity3d.com/artifactory/public-generic-stevedore-ms-consoles/
                    repo.sony-consoles.url = https://bfartifactory.bf.unity3d.com/artifactory/public-generic-stevedore-sony-consoles/
                    repo.nintendo-consoles.url = https://bfartifactory.bf.unity3d.com/artifactory/public-generic-stevedore-nintendo-consoles/
                ");

                ReadEnvironmentConfig(out var config);
                Assert.AreEqual(100000, config.DefaultRepoConfig.Timeout);
                Assert.AreEqual("", config.DefaultRepoConfig.HttpProxyUrl);
                Assert.AreEqual("", config.DefaultRepoConfig.HttpsProxyUrl);
                Assert.AreEqual("https://bfartifactory.bf.unity3d.com/artifactory/public-generic-stevedore-public/", config.RepoConfigFor("public").Url);
                Assert.AreEqual("https://bfartifactory.bf.unity3d.com/artifactory/public-generic-stevedore-ms-consoles/", config.RepoConfigFor("ms-consoles").Url);
            }
        }

        [Test]
        public void RepoConfigFor_Defaults()
        {
            var config = new StevedoreConfig();

            var internalRepo = config.RepoConfigFor("unity-internal");
            Assert.AreEqual($"http://stevedore.hq.unity3d.com/r/unity-internal/", internalRepo.Url);

            var publicRepo = config.RepoConfigFor("public");
            Assert.AreEqual($"https://stevedore.unity3d.com/r/public/", publicRepo.Url);
        }

        [Test]
        public void RepoConfigFor_UnknownRepo()
        {
            var config = new StevedoreConfig();
            Assert.Throws<UnavailableRepositoryException>(() => config.RepoConfigFor("xyzzy"));
        }
    }
}
