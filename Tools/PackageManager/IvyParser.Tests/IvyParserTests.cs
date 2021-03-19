using System;
using System.Text.RegularExpressions;
using NUnit.Framework;
using Unity.DataContract;
using Unity.PackageManager.Ivy;

namespace Unity.PackageManager.Tests
{
    public static class StringExtensions
    {
        public static string Clean(this string str, string s, string s1)
        {
            while (str.IndexOf(s, StringComparison.InvariantCulture) >= 0)
                str = str.Replace(s, s1);
            return str;
        }
    }

    [TestFixture]
    public class IvyParserTests
    {
        [Test]
        public void PackageInfoIvyModuleComparison()
        {
            IvyModule module = new IvyModule();
            module.Info = new IvyInfo() {
                Organisation = "Unity",
                Module = "PackageManager",
                Branch = "default",
                Version = new PackageVersion("0.1"),
                UnityVersion = new PackageVersion("4.3"),
                Revision = "beta1",
                Type = PackageType.PackageManager
            };

            PackageInfo pi = module.ToPackageInfo();

            Assert.True(module.Equals(pi));
            Assert.AreEqual(module, pi);
            Assert.AreNotEqual(pi, module);
        }

        [Test]
        public void TestMultipleInfoElements()
        {
            ModuleRepository repo = new ModuleRepository();
            IvyModule module;

            module = new IvyModule();
            module.Info = new IvyInfo() {
                Organisation = "Unity",
                Module = "PackageManager",
                Branch = "default",
                Version = new PackageVersion("0.1"),
                UnityVersion = new PackageVersion("4.3"),
                Revision = "beta1",
                Type = PackageType.PackageManager
            };

            repo.Modules.Add(module);

            string result = IvyParser.Serialize(repo);
            string expected = @"<?xml version=""1.0"" encoding=""utf-8""?>
<ivy-repository xmlns:e=""http://ant.apache.org/ivy/extra"">
<ivy-module version=""2.0"">
    <info version=""0.1"" organisation=""Unity"" module=""PackageManager"" revision=""beta1"" branch=""default"" e:packageType=""PackageManager"" e:unityVersion=""4.3"" />
</ivy-module>
</ivy-repository>
";


            result = NormalizeWhitespace(result);
            expected = NormalizeWhitespace(expected);

            Console.WriteLine(result);
            Assert.AreEqual(expected, result);
        }

        private static string NormalizeWhitespace(string result)
        {
            result = Regex.Replace(result, @"\s*\<", "<");
            result = Regex.Replace(result, @"\>\s*", ">");
            result = result.Clean("\r\n", "\n");
            result = result.Clean("\n\n", "\n");
            return result;
        }

        [Test]
        public void Serialize()
        {
            IvyModule module = new IvyModule();
            module.Info = new IvyInfo();
            module.Info.Organisation = "Unity";
            module.Info.Module = "PackageManager";
            module.Info.Revision = "0.1";
            module.Info.Branch = "4.3.0";
            module.Info.Type = PackageType.PackageManager;

            string result = IvyParser.Serialize(module);

            string expected = @"<?xml version=""1.0"" encoding=""utf-8""?>
<ivy-module version=""2.0"">
  <info organisation=""Unity"" module=""PackageManager"" revision=""0.1"" branch=""4.3.0"" e:packageType=""PackageManager"" xmlns:e=""http://ant.apache.org/ivy/extra"" />
</ivy-module>";

            result = NormalizeWhitespace(result);
            expected = NormalizeWhitespace(expected);

            Console.WriteLine(result);
            Assert.AreEqual(expected, result);
        }

        [Test]
        public void Deserialize()
        {
            IvyModule ret = IvyParser.Parse<IvyModule>(@"<?xml version=""1.0"" encoding=""UTF-8""?>
<ivy-module version=""2.0"" xmlns:e=""http://ant.apache.org/ivy/extra"">
    <info organisation=""Unity""
        module=""UnityEngine""
        revision=""4.0.0f1""
        status=""integration""
        publication=""20121017102532""
        e:packageType=""PackageManager""
        e:unityVersion=""4.3""
    />
    <publications>
        <artifact name=""UnityEngine"" type=""package"" ext=""zip"" conf=""archives,editor"" />
    </publications>
</ivy-module>
");
            Assert.NotNull(ret.Info);
            Assert.NotNull(ret.Artifacts);

            Assert.AreEqual("Unity", ret.Info.Organisation);
            Assert.AreEqual("UnityEngine", ret.Info.Module);
            Assert.AreEqual("4.0.0f1", ret.Info.Revision);
            Assert.AreEqual("integration", ret.Info.Status);
            Assert.AreEqual("20121017102532", ret.Info.Publication);
            Assert.AreEqual(PackageType.PackageManager, ret.Info.Type);
            Assert.AreEqual("4.3", ret.Info.UnityVersion.ToString());
            Assert.IsFalse(ret.Selected);

            Console.WriteLine(IvyParser.Serialize(ret));
        }

        [Test]
        public void DependenciesAndConfigurations()
        {
            IvyModule module = new IvyModule();
            module.Info = new IvyInfo();
            module.Info.Organisation = "Unity";
            module.Info.Module = "PackageManager";
            module.Info.Revision = "0.1";
            module.Info.Branch = "4.3.0";
            module.Info.Type = PackageType.PackageManager;
            module.Info.UnityVersion = new PackageVersion("4.3");
            IvyDependency dep = new IvyDependency();
            dep.Name = "DataContract";
            dep.Revision = "1.0.+";
            module.Dependencies.Add(dep);

            string result = IvyParser.Serialize(module);

            string expected = @"<?xml version=""1.0"" encoding=""utf-8""?>
<ivy-module version=""2.0"">
  <info organisation=""Unity"" module=""PackageManager"" revision=""0.1"" branch=""4.3.0"" e:packageType=""PackageManager"" e:unityVersion=""4.3"" xmlns:e=""http://ant.apache.org/ivy/extra"" />
  <dependencies xmlns:e=""http://ant.apache.org/ivy/extra"">
    <dependency name=""DataContract"" rev=""1.0.+"" force=""false"" />
  </dependencies>
</ivy-module>";

            result = NormalizeWhitespace(result);
            expected = NormalizeWhitespace(expected);

            Console.WriteLine(expected);
            Console.WriteLine(result);
            Assert.AreEqual(expected, result);
        }

        [Test]
        public void IndexTest2()
        {
            string xml = @"<?xml version=""1.0"" encoding=""utf-8""?>

  <ivy-repository xmlns:e=""http://ant.apache.org/ivy/extra"">
    <ivy-module version=""2.0"" e:selected=""True"">
      <info version=""0.1"" organisation=""Unity"" module=""PackageManager"" e:packageType=""PackageManager"" e:unityVersion=""4.2"">
        <repository name=""internal"" url=""file:///C:/Users/shana/AppData/Local/Temp/tmp294C"" />
      </info>
      <publications>
        <artifact name=""Unity.PackageManager.0.1"" type=""package"" ext=""zip"" url=""file:///C:/Users/shana/AppData/Local/Temp/tmp294C/packages/Unity.PackageManager.0.1.zip"" />
        <artifact name=""Unity.PackageManager.0.1"" type=""ivy"" ext=""xml"" url=""file:///C:/Users/shana/AppData/Local/Temp/tmp294C/packages/Unity.PackageManager.0.1.xml"" />
      </publications>
    </ivy-module>
  </ivy-repository>
";
            ModuleRepository repo = IvyParser.Deserialize<ModuleRepository>(xml);

            Assert.IsFalse(IvyParser.HasErrors);
            Assert.AreEqual(1, repo.Modules.Count);
            Assert.IsTrue(repo.Modules[0].Selected);
        }
    }
}
