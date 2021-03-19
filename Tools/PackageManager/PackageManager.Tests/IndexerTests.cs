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

namespace Tests
{
    [TestFixture]
    public class IndexerTests
    {
        IPackageManagerModule module;
        /*
                public static void Main ()
                {
                    new IndexerTests ().RunAll ();
                }

                public void RunAll ()
                {
                    TestRemoteIndexer ();

                    Console.WriteLine ("done");
                    Console.ReadLine ();
                }
        */
        [SetUp]
        public void Setup()
        {
            module = AssemblyHelper.FindImplementors<IPackageManagerModule>(typeof(PackageManager).Assembly).FirstOrDefault();
            Settings.CacheAllSettings();
            Settings.editorInstallPath = Path.Combine(System.Reflection.Assembly.GetExecutingAssembly().CodeBase, "../..");
        }

        [TearDown]
        public void TearDown()
        {
            module.Shutdown(true);
            Settings.ResetAllSettings();
        }

        [Test]
        public void LocalIndexerIsCancellable()
        {
            ManualResetEvent done = new ManualResetEvent(false);
            ManualResetEvent go = new ManualResetEvent(false);
            bool success = false;

            // OnTask is called for every task that is fired up from package manager so we can hook up listeners before they run
            // the OnFinish event is always fired regardless of whether a task is cancelled or not, signaling completion
            PackageManager.Instance.OnTask += (t) => {
                if (t.Name == "Local Indexer")
                {
                    t.OnStart += _ => go.Set();
                    t.OnFinish += (_, s) => {
                        success = s;
                        done.Set();
                    };
                }
            };
            PackageManager.Start();  // calling Start without calling Initialize first should only happen on standalone tests
            PackageManager.Instance.CheckForLocalUpdates();
            go.WaitOne();
            PackageManager.Stop(false);
            Assert.IsTrue(done.WaitOne(TimeSpan.FromMilliseconds(100)));   // it really shouldn't take more than this to cancel the task
            Assert.IsFalse(success);
        }
    }
}
