using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Net;
using Unity.PackageManager;
using System.IO;

namespace Tests
{
    public class WebServer : Task
    {
        public bool QueueEmpty;
        Queue<byte[]> responses;
        HttpListener listener;
        string uri;
        public WebServer(string uri, byte[] responseData)
            : this(uri, new List<byte[]>() {responseData}
            )
        {}

        public WebServer(string uri, List<byte[]> responses)
            : base()
        {
            this.uri = uri;
            this.responses = new Queue<byte[]>(responses);
        }

        protected override bool TaskRunning()
        {
            try
            {
                while (responses.Count > 0)
                {
                    listener = new HttpListener();
                    listener.Prefixes.Add(uri);
                    listener.Start();

                    byte[] responseData = responses.Dequeue();

                    Console.WriteLine("Waiting for request");
                    var context = listener.GetContext();

                    Console.WriteLine("Sending {0} bytes in response", responseData.Length);

                    context.Response.ContentLength64 = responseData.LongLength;
                    //context.Response.OutputStream.Write (responseData, 0, responseData.Length);
                    Unity.PackageManager.Utils.Copy(new MemoryStream(responseData), context.Response.OutputStream, 256 * 256);
                    Console.WriteLine("Done sending response");

                    listener.Close();
                }
            }
            catch (Exception ex)
            {
                Console.WriteLine(ex);
                return false;
            }
            QueueEmpty = true;
            return base.TaskRunning();
        }

        public override void Stop()
        {
            /*
                        while (!QueueEmpty)
                        {
                            System.Threading.Thread.Sleep (1000);
                        }
                        Console.WriteLine ("Stopping listener");
                        listener.Stop ();
            */
        }
    }
}
