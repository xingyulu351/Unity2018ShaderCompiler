using System;
using System.IO;
using System.Linq;
using System.Text;
using System.Collections.Generic;
using NUnit.Framework;

using Unity.DataContract;

namespace Unity.PackageManager
{
    [TestFixture()]
    public class LocatorPaths
    {
        [Test()]
        public void InvalidScanPathDoesNotThrow()
        {
            Assert.DoesNotThrow(() => {
                DirectoryInfo tempDir = Util.GetTemporaryTestDirectory();
                Locator.moduleLocation = tempDir.FullName;
                string path = Locator.CombinePaths("moduletype", "org", "modulename", "4.3.1");
                tempDir.CreateSubdirectory(path);
                Locator.Scan(tempDir.FullName, "4.3.0");
            });
        }

        [Test()]
        public void ShallowPathDoesNotThrow()
        {
            Assert.DoesNotThrow(() => {
                DirectoryInfo tempDir = Util.GetTemporaryTestDirectory();
                Locator.moduleLocation = tempDir.FullName;
                Locator.Scan(tempDir.FullName, "4.3.0");
            });
        }

        [Test()]
        public void DeepPathDoesNotThrow()
        {
            Assert.DoesNotThrow(() => {
                DirectoryInfo tempDir = Util.GetTemporaryTestDirectory();
                Locator.moduleLocation = tempDir.FullName;
                string path = Locator.CombinePaths("moduletype", "org", "modulename", "4.3.1", "scripting", "scripting", "scripting", "scripting", "scripting", "scripting");
                tempDir.CreateSubdirectory(path);
                Locator.Scan(tempDir.FullName, "4.3.0");
            });
        }

        [Test()]
        public void FindsIvyModule()
        {
            DirectoryInfo tempDir = Util.GetTemporaryTestDirectory();
            Locator.moduleLocation = tempDir.FullName;

            string path = Locator.CombinePaths("moduletype", "org", "modulename", "4.3.1.omg.wat");
            string subPath = Locator.CombinePaths("Unity", "4.3.0", path);
            tempDir.CreateSubdirectory(subPath);
            string fullPath = Path.Combine(tempDir.FullName, subPath);
            PrepareIvyModule(fullPath);
            Locator.Scan(Locator.CombinePaths(tempDir.FullName, "Unity", "4.3.0"), "4.3.0");
            IEnumerable<PackageInfo> queryResult = Locator.QueryAll();
            PackageInfo foundModule = queryResult.Single();
            Assert.AreEqual("org", foundModule.organisation);
            Assert.AreEqual("4.3.1.omg.wat", foundModule.version.text);
            Assert.AreEqual("4.3", foundModule.unityVersion.text);
            Assert.AreEqual("modulename", foundModule.name);
            Assert.AreEqual(PackageType.UnityExtension, foundModule.type);
            Assert.AreEqual(fullPath, foundModule.basePath);
            Util.DeleteTemporaryTestDirectory(tempDir);
        }

        static readonly string kIvyModuleFile = "ivy.xml";
        static readonly string kPackageManagerModuleDescription =
@"<?xml version=""1.0"" encoding=""UTF-8""?>
<ivy-module version=""2.0"" xmlns:m=""http://ant.apache.org/ivy/extra"">
    <info organisation=""org""
        module=""modulename""
        m:unityVersion=""4.3""
        m:packageType=""UnityExtension""
        branch=""4.3.0""
        version=""4.3.1.omg.wat""
        status=""integration""
        publication=""20121017102532""
    />
    <configurations>
        <conf name=""archives"" visibility=""public"" description=""Configuration for archive artifacts.""/>
        <conf name=""default"" visibility=""public"" description=""Configuration for default artifacts.""/>
        <conf name=""editor"" visibility=""public""/>
    </configurations>
    <publications>
        <artifact name=""UnityEngine"" type=""package"" ext=""zip"" conf=""archives,editor""/>
    </publications>
</ivy-module>
";

        static void PrepareIvyModule(string descriptionPath)
        {
            File.WriteAllText(Path.Combine(descriptionPath, kIvyModuleFile), kPackageManagerModuleDescription);
        }
    }
}
