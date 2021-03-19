using System;
using System.IO;
using System.Linq;
using System.Text;
using System.Collections.Generic;
using NUnit.Framework;

using Unity.DataContract;

namespace Unity.PackageManager
{
    using Ivy;

    [TestFixture()]
    public class Cancellation
    {
        DirectoryInfo tempDir;
        [SetUp]
        public void Setup()
        {
            tempDir = Util.GetTemporaryTestDirectory();
        }

        public void Teardown()
        {
            tempDir.Delete(true);
        }

        [Test]
        public void CancellingWhileScanningWorks()
        {
            Locator.moduleLocation = tempDir.FullName;

            List<IvyModule> mods = new List<IvyModule>();
            for (int i = 0; i < 200; i++)
            {
                var mod = new IvyModule();
                mod.Info.Type = PackageType.PlaybackEngine;
                mod.Info.Organisation = "Unity";
                mod.Info.Module = "APackage";
                mod.Info.Version = new PackageVersion("1.0." + i);
                mod.Info.UnityVersion = new PackageVersion("4.3");
                mods.Add(mod);
            }

            foreach (var mod in mods)
            {
                var sourcepath = Locator.CombinePaths("Unity", "4.3.0", mod.Info.Type.ToString(), mod.Info.Organisation, mod.Info.Module, mod.Info.Version);
                tempDir.CreateSubdirectory(sourcepath);
                mod.WriteIvyFile(Path.Combine(tempDir.FullName, sourcepath));
            }

            Locator.Scan(tempDir.FullName, "4.3.0", () => false, () => {});
            Assert.IsEmpty(Locator.QueryAll());
        }
    }
}
