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
    public class SettingsTest
    {
        IPackageManagerModule module;

        [SetUp]
        public void Setup()
        {
            module = AssemblyHelper.FindImplementors<IPackageManagerModule>(typeof(PackageManager).Assembly).FirstOrDefault();
            Settings.CacheAllSettings();
        }

        [TearDown]
        public void TearDown()
        {
            module.Shutdown(true);
            Settings.ResetAllSettings();
        }

        [Test]
        public void InstallLocations()
        {
            if (IsLinux)
                Assert.AreEqual(Path.Combine(Environment.GetEnvironmentVariable("HOME"), ".local/share/unity3d"), Settings.installLocation);
            else if (Environment.OSVersion.Platform == PlatformID.MacOSX || Environment.OSVersion.Platform == PlatformID.Unix)
                Assert.AreEqual(Path.Combine(Environment.GetEnvironmentVariable("HOME"), ".local/share/Unity"), Settings.installLocation);
            else
                Assert.AreEqual(Path.Combine(Environment.GetFolderPath(Environment.SpecialFolder.CommonApplicationData), "Unity"), Settings.installLocation);
        }

        [Test]
        public void RemoteUrls()
        {
            Assert.AreEqual("http://update.unity3d.com/", Settings.baseRepoUrl);
            module.unityVersion = "4.3.0";
            Assert.AreEqual("http://update.unity3d.com/4.3/", Settings.RepoUrl.ToString());
            module.unityVersion = "4.5.3.2alpha1";
            Assert.AreEqual("http://update.unity3d.com/4.5/", Settings.RepoUrl.ToString());
        }

        static bool IsLinux { get { return Environment.OSVersion.Platform == PlatformID.Unix && Directory.Exists("/proc"); } }
    }
}
