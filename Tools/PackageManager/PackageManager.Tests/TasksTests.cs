using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading;
using NUnit.Framework;
using Unity.PackageManager;
using Unity.DataContract;
using System.Net;
using System.IO;
using System.Diagnostics;

namespace Tests
{
    //[TestFixture]
    public class TasksTests
    {
        string uri = string.Format("http://localhost:{0}/", new Random().Next(1024, 65534));

        //public static void Main ()
        //{
        //    new TasksTests ().RunAll ();
        //}

        //public void RunAll ()
        //{
        //    TestDownloader ();
        //    TestDownloaderBig ();

        //    Console.WriteLine ("done");
        //    Console.ReadLine ();
        //}

        //[Test]
        public void TestDownloader()
        {
            byte[] buffer = new byte[1024 * 1024];
            buffer[512] = 0xDE;
            buffer[1024] = 0xAD;
            buffer[512 * 512] = 0xBE;
            buffer[1024 * 1024 - 1] = 0xEF;

            WebServer ws = new WebServer(uri, buffer);
            ws.Run();

            Task download = new DownloaderTask(new Uri(uri));

            Stopwatch watch = new Stopwatch();
            watch.Start();

            download.Run();
            while (!download.Wait(100))
            {
                Console.WriteLine("Waiting {0} for response", download.EstimatedDuration);
            }
            watch.Stop();
            Console.WriteLine("Took {0} milliseconds", watch.ElapsedMilliseconds);

            ws.Stop();

            string filename = (string)download.Result;
            Assert.IsNotNull(filename);
            Assert.True(File.Exists(filename));

            byte[] response = File.ReadAllBytes(filename);

            Assert.AreEqual(buffer.Length, response.Length);
            Assert.AreEqual(0xDE, response[512]);
            Assert.AreEqual(0xAD, response[1024]);
            Assert.AreEqual(0xBE, response[512 * 512]);
            Assert.AreEqual(0xEF, response[1024 * 1024 - 1]);

            Directory.Delete(Path.GetDirectoryName(filename), true);
        }
    }
}
