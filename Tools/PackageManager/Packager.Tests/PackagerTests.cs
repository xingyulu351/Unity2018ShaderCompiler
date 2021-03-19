using System;
using System.Diagnostics;
using System.IO;
using System.Text;
using NUnit.Framework;
using Unity.CommonTools;
using System.Linq;
using Unity.DataContract;
using Unity.PackageManager.Ivy;

namespace Packager.Tests
{
    [TestFixture]
    public class Tests
    {
        [Test]
        public void BuildIvyTest()
        {
            string version = "0.1";
            string org = "Unity";
            string name = "PackageManager";
            string unityVersion = "4.2";
            string packageType = "pm";

            DirectoryInfo outputDir = Utils.GetTempDirectory();
            string packagePath = Utils.BuildPath(Workspace.BasePath, "Tools", "PackageManager", "PackageManager", "bin", "Debug");

            CreateIvy(outputDir, packagePath, org, name, version, unityVersion, packageType);

            string[] files = outputDir.GetFiles().Select(x => x.Name).OrderBy(s => s).ToArray();
            Assert.AreEqual(1, files.Count());
            Assert.That(files.Contains("ivy.xml"));

            IvyModule module = IvyParser.ParseFile<IvyModule>(Path.Combine(outputDir.FullName, files[0]));
            Assert.IsNotNull(module);
            Assert.AreEqual(string.Format("{0}.{1}.{2}", org, name, version), module.Info.FullName);
            Assert.AreEqual(name, module.Info.Module);
            Assert.AreEqual(org, module.Info.Organisation);
            Assert.AreEqual(version, module.Info.Version.ToString());
            Assert.AreEqual(PackageType.PackageManager, module.Info.Type);
            Assert.AreEqual(outputDir.FullName, module.BasePath);

            Assert.AreEqual(unityVersion, module.Info.UnityVersion.ToString());

            outputDir.Delete(true);
        }

        [Test]
        public void BuildPackageTest()
        {
            string version = "0.1";
            string org = "Unity";
            string name = "PackageManager";
            string unityVersion = "4.2";
            string packageType = "pm";

            string fileprefix = org + "." + name + "." + version;

            DirectoryInfo outputDir = Utils.GetTempDirectory();
            string packagePath = Utils.BuildPath(Workspace.BasePath, "Tools", "PackageManager", "PackageManager", "bin", "Debug");

            CreatePackage(outputDir, packagePath, org, name, version, unityVersion, packageType);

            string[] files = outputDir.GetFiles().Select(x => x.Name).OrderBy(s => s).ToArray();
            Assert.That(files.Contains(fileprefix + ".zip"));
            Assert.That(files.Contains(fileprefix + ".xml"));
            Assert.That(files.Contains(fileprefix + ".md5"));

            outputDir.Delete(true);
        }

        [Test]
        public void BuildIndexTest()
        {
            DirectoryInfo outputDir = Utils.GetTempDirectory();

            string[] versions = new[] { "0.1", "0.2", "0.3" };
            string[] fileprefixes = new string[3];
            string org = "Unity";
            string name = "PackageManager";
            string unityVersion = "4.2";
            string packageType = "pm";
            string packagePath = Utils.BuildPath(Workspace.BasePath, "Tools", "PackageManager", "PackageManager", "bin", "Debug");


            for (int i = 0; i < 3; ++i)
            {
                fileprefixes[i] = org + "." + name + "." + versions[i];
                CreatePackage(outputDir, packagePath, org, name, versions[i], unityVersion, packageType);
            }

            CreateIndex(outputDir, outputDir.FullName);

            string[] files = outputDir.GetFiles().Select(x => x.Name).OrderBy(s => s).ToArray();
            Assert.That(files.Contains("ivy.xml"));

            ModuleRepository repo = IvyParser.ParseFile<ModuleRepository>(Path.Combine(outputDir.FullName, "ivy.xml"));
            Assert.AreEqual(3, repo.Modules.Count);

            for (int i = 0; i < 3; ++i)
            {
                foreach (string extension in new[] { "zip", "xml", "md5" })
                    Assert.That(files.Contains(string.Format("{0}.{1}", fileprefixes[i], extension)));
                IvyModule module = repo.Modules[i];
                Assert.AreEqual(org, module.Info.Organisation);
                Assert.AreEqual(name, module.Info.Module);
                Assert.AreEqual(versions[i], module.Info.Version.ToString());
                Assert.AreEqual(null, module.BasePath);
            }

            outputDir.Delete(true);
        }

        static void CreatePackage(DirectoryInfo outputDir, string packagePath, string org, string name, string version,
            string unityVersion, string packageType)
        {
            StringBuilder args = new StringBuilder();

            args.AppendFormat("-s \"{0}\" ", packagePath);
            args.AppendFormat("-o \"{0}\" ", outputDir.FullName);

            args.AppendFormat("-g \"{0}\" ", org);
            args.AppendFormat("-n \"{0}\" ", name);
            args.AppendFormat("-v \"{0}\" ", version);
            args.AppendFormat("-u \"{0}\" ", unityVersion);
            args.AppendFormat("-t \"{0}\" ", packageType);

            var arguments = args.ToString();

            RunPackager(arguments);
        }

        static void CreateIvy(DirectoryInfo outputDir, string packagePath, string org, string name, string version,
            string unityVersion, string packageType)
        {
            StringBuilder args = new StringBuilder();

            args.AppendFormat("-y ");

            args.AppendFormat("-s \"{0}\" ", packagePath);
            args.AppendFormat("-o \"{0}\" ", outputDir.FullName);

            args.AppendFormat("-g \"{0}\" ", org);
            args.AppendFormat("-n \"{0}\" ", name);
            args.AppendFormat("-v \"{0}\" ", version);
            args.AppendFormat("-u \"{0}\" ", unityVersion);
            args.AppendFormat("-t \"{0}\" ", packageType);

            var arguments = args.ToString();

            RunPackager(arguments);
        }

        static void CreateIndex(DirectoryInfo outputDir, string packagePath)
        {
            StringBuilder args = new StringBuilder();

            args.AppendFormat("-x ");
            args.AppendFormat("-s \"{0}\" ", packagePath);
            args.AppendFormat("-o \"{0}\" ", outputDir.FullName);

            var arguments = args.ToString();

            RunPackager(arguments);
        }

        static void RunPackager(string arguments)
        {
            string app = Utils.BuildPath(Workspace.BasePath, "Tools", "PackageManager", "Packager", "bin", "Debug", "Unity.Packager.exe");
            var p = new Process() {
                StartInfo = new ProcessStartInfo() {
                    FileName = app,
                    Arguments = arguments,
                    RedirectStandardOutput = true,
                    RedirectStandardError = true,
                    UseShellExecute = false,
                    CreateNoWindow = true
                }
            };

            p.Start();

            if (!p.WaitForExit(5000))
                Console.WriteLine("timed out.");

            Console.WriteLine(p.StandardOutput.ReadToEnd());
        }
    }

    public static class Utils
    {
        public static DirectoryInfo GetTempDirectory()
        {
            string temp = Path.GetTempFileName();
            try { File.Delete(temp); } catch {}
            temp = Path.Combine(Path.GetDirectoryName(temp), Path.GetFileNameWithoutExtension(temp));
            return Directory.CreateDirectory(temp);
        }

        public static string BuildPath(params string[] parts)
        {
            string path = "";
            for (int i = parts.Length - 1; i >= 0; i--)
            {
                path = Path.Combine(parts[i], path);
            }
            return path;
        }
    }
}
