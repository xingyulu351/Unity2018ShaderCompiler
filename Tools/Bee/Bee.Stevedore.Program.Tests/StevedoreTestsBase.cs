using System;
using System.Collections;
using System.Collections.Generic;
using System.IO;
using System.Linq;
using NUnit.Framework;

namespace Bee.Stevedore.Program.Tests
{
    /// <summary>
    /// Common SetUp/TearDown logic shared by all Stevedore tests.
    /// Provides a somewhat stable set of environment variables.
    /// </summary>
    public abstract class StevedoreTestsBase
    {
        protected static readonly ArtifactId DummyArtifactId = new ArtifactId("dummy/dummy_0000000000000000000000000000000000000000000000000000000000000000.zip");

        /// <summary>
        /// Environment variables to delete before every test.
        /// </summary>
        static readonly string[] CleanEnvironmentVariables =
        {
            "XDG_CONFIG_HOME",
            "XDG_CACHE_HOME",
            "LOCALAPPDATA",
            "USERPROFILE",
            "HOME",
            "all_proxy",
            "ALL_PROXY",
            "http_proxy",
            "HTTP_PROXY",
            "https_proxy",
            "HTTPS_PROXY",
            "no_proxy",
            "NO_PROXY",
        };

        // A lot of (other) tests depend on the system environment being pristine.
        // Meanwhile, we trash it thoroughly in our tests... so back it up first.
        IDictionary environmentBackup;

        [OneTimeSetUp]
        public void OneTimeSetUp()
        {
            environmentBackup = Environment.GetEnvironmentVariables();
        }

        [OneTimeTearDown]
        public void OneTimeTearDown()
        {
            foreach (var de in environmentBackup.Cast<DictionaryEntry>())
            {
                Environment.SetEnvironmentVariable((string)de.Key, (string)de.Value);
            }
            foreach (var key in Environment.GetEnvironmentVariables().Keys.Cast<string>())
            {
                if (!environmentBackup.Contains(key))
                    Environment.SetEnvironmentVariable(key, null);
            }
        }

        [SetUp]
        public void SetUp()
        {
            // Clean environment variables between tests (both these generic ones and anything with STEVEDORE in it).
            foreach (var variable in CleanEnvironmentVariables)
                Environment.SetEnvironmentVariable(variable, null);

            foreach (var variable in Environment.GetEnvironmentVariables().Keys.Cast<string>())
            {
                if (variable.Contains("STEVEDORE"))
                    Environment.SetEnvironmentVariable(variable, null);
            }
        }

        /// <summary>
        /// Create StevedoreConfig from string.
        /// </summary>
        protected static void ReadConfig(out StevedoreConfig config, out List<string> warnings, string text)
        {
            warnings = new List<string>();
            config = new StevedoreConfig();
            config.Read(new StringReader(text), reportParseError: warnings.Add);
        }

        /// <summary>
        /// Create StevedoreConfig from string, and assert that no warnings were produced.
        /// </summary>
        protected static void ReadConfig(out StevedoreConfig config, string text)
        {
            ReadConfig(out config, out var warnings, text);
            Assert.That(warnings, Is.Empty);
        }

        /// <summary>
        /// Create StevedoreConfig from environment, and assert that no warnings were produced.
        /// </summary>
        protected static void ReadEnvironmentConfig(out StevedoreConfig config)
        {
            var warnings = new List<string>();
            config = new StevedoreConfig();
            config.ReadEnvironmentDefaults(reportParseError: warnings.Add);
            Assert.That(warnings, Is.Empty);
        }
    }
}
