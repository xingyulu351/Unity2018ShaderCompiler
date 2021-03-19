using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading;
using NUnit.Framework;
using Unity.PackageManager;
using Unity.DataContract;
using System.IO;
using packager = Unity.Packager;
using ivy = Unity.PackageManager.Ivy;
using System.Diagnostics;
using Unity.PackageManager.Ivy;

namespace Tests
{
    [TestFixture]
    public class InstallerTests
    {
        IPackageManagerModule module;
        DirectoryInfo tmpdir;

        public static void Main()
        {
            new InstallerTests().RunAll();
        }

        public void RunAll()
        {
            Setup();
            DownloadProgressTest();
            TearDown();

            Console.WriteLine("done");
            Console.ReadLine();
        }

        [SetUp]
        public void Setup()
        {
            tmpdir = packager.Utils.GetTempDirectory();
            module = AssemblyHelper.FindImplementors<IPackageManagerModule>(typeof(PackageManager).Assembly).FirstOrDefault();
            Settings.CacheAllSettings();
        }

        [TearDown]
        public void TearDown()
        {
            module.Shutdown(true);
            Settings.ResetAllSettings();
            tmpdir.Delete(true);
        }

        //[Test]
        public void DownloadProgressTest()
        {
            // signals when the webserver is ready to fulfill requests
            ManualResetEvent ready = new ManualResetEvent(false);
            // signals when the installe is done downloading the package
            ManualResetEvent done = new ManualResetEvent(false);

            var baseDir = Directory.CreateDirectory(Path.Combine(tmpdir.FullName, Guid.NewGuid().ToString()));

            var installLocation = Directory.CreateDirectory(Path.Combine(baseDir.FullName, "Unity"));
            var editorLocation = Directory.CreateDirectory(Path.Combine(baseDir.FullName, "Editor"));
            Settings.installLocation = installLocation.FullName;
            Settings.editorInstallPath = editorLocation.FullName;

            string uri = string.Format("http://localhost:{0}/", new Random().Next(1024, 65534));

            IvyModule mod = new IvyModule();
            mod.Info = new IvyInfo() {
                Organisation = "Unity",
                Module = "BlackBerry",
                Version = new PackageVersion("0.1"),
                UnityVersion = new PackageVersion("4.5"),
                Type = PackageType.PlaybackEngine
            };
            mod.Artifacts.Add(new IvyArtifact()
            {
                Name = "BlackBerry",
                Url = new Uri(uri + "BlackBerry.zip"),
                Type = ArtifactType.Package,
                Extension = "zip"
            }
            );

            byte[] buffer = new byte[1024 * 1024 * 10];
            Random rng = new Random();
            rng.NextBytes(buffer);
            byte[] buffer2 = new byte[1024 * 1024 * 20];
            rng = new Random();
            rng.NextBytes(buffer2);
            byte[] buffer3 = new byte[1024 * 2048 * 20];
            rng = new Random();
            rng.NextBytes(buffer3);

            List<string> filenames = new List<string>() { "file1", "file2", "file3" };
            List<byte[]> files = new List<byte[]>() { buffer, buffer2, buffer3 };
            var tempdir = Directory.CreateDirectory(Path.Combine(baseDir.FullName, "input"));
            var zipdir = Directory.CreateDirectory(Path.Combine(baseDir.FullName, "output"));
            for (int i = 0; i < filenames.Count; i++)
                File.WriteAllBytes(Path.Combine(tempdir.FullName, filenames[i]), files[i]);
            string hash = packager.ZipUtils.CompressFolder(tempdir.FullName, tempdir.FullName, Path.Combine(zipdir.FullName, "test.zip"));
            byte[] zip = File.ReadAllBytes(Path.Combine(zipdir.FullName, "test.zip"));

            WebServer ws = new WebServer(uri, new List<byte[]>()
            {
                Encoding.UTF8.GetBytes(hash),
                zip
            });

            ws.OnStart += _ => ready.Set();
            ws.Run();
            ready.WaitOne();

            PackageManager.Start();

            Installer installer = PackageManager.Instance.SetupPackageInstall(mod);
            installer.OnProgress += (Task task, float progress) => {
                Console.WriteLine(progress);
            };
            installer.OnFinish += (Task task, bool success) => {
                done.Set();
            };

            // time how long it takes for the indexer to run, just for statistical data
            Stopwatch watch = new Stopwatch();
            watch.Start();
            installer.Run();
            while (!done.WaitOne(1000)) {}
            watch.Stop();
            Console.WriteLine("Task ran for {0} ms", watch.ElapsedMilliseconds);

            Assert.AreEqual(1, installer.Progress);
        }
    }
}
