using System;
using System.IO;
using System.Net;
using System.Net.Sockets;
using System.Text;
using System.Threading;

// WARNING!!:  This script is not only used as a tool for tests,  but this actual .cs file also gets copied into unity projects by the "ListeningSocketSecurity" tests.
// when you change this file, make sure those tests still pass


namespace Unity.CommonTools
{
    public abstract class SocketResponderBase : IDisposable
    {
        static SocketResponderBase()
        {
            EnableCodeFlowLogging = false;
        }

        public SocketResponderBase(int port, string respondto, string response)
        {
            this.respondto = respondto;
            id = port + "-" + (new Random().Next(100));
            this.response = response;
            GotConnectedTo = false;

            responsebytes = Encoding.ASCII.GetBytes(this.response);
        }

        protected string response;
        protected string respondto;
        protected string id;
        protected byte[] responsebytes;
        public bool GotConnectedTo
        {
            get { lock (this) { return getConnectedTo; } }
            protected set { lock (this) { getConnectedTo = value; } }
        }
        private bool getConnectedTo;
        public int Port { get; protected set; }
        public bool Strict { get; set; }

        protected bool DoesRequestMatchExpectedRequest(byte[] requestbytes)
        {
            var msg = Encoding.ASCII.GetString(requestbytes);
            CodeFlow("Got requeststring: " + msg);
            return Strict ? msg == respondto : msg.Contains(respondto);
        }

        protected void CodeFlow(string msg)
        {
            if (EnableCodeFlowLogging)
                Console.WriteLine("CodeFlow " + id + " " + msg);
        }

        public static bool EnableCodeFlowLogging { get; set; }

        public abstract void Dispose();
    }

    public class SocketResponder : SocketResponderBase
    {
        private TcpListener listener;

        public SocketResponder(int port)
            : this(port, "Ping", "Pong")
        {
        }

        public SocketResponder(int port, string respondto, string response) : base(port, respondto, response)
        {
            CodeFlow("Creating tcplistener");
            listener = new TcpListener(IPAddress.Loopback, port);


            AttemptMultipleTimes<SocketException>(() =>
            {
                listener.Start();
                Port = ((IPEndPoint)listener.Server.LocalEndPoint).Port;
            }, "Couldnt bind to port " + port);

            CodeFlow("BeginAcceptTcpClient");
            BeginAcceptTcpClient();
        }

        protected void BeginAcceptTcpClient()
        {
            listener.BeginAcceptTcpClient(DoAcceptTcpClientCallback, null);
        }

        // Process the client connection.
        public void DoAcceptTcpClientCallback(IAsyncResult ar)
        {
            CodeFlow("DoAcceptTcpClientCallback");
            if (listener == null)
            {
                CodeFlow("Early aborting DoAcceptTcpClientCallback because listener==null");
                return;
            }

            try
            {
                CodeFlow("calling EndAcceptTcpClient");
                using (var client = listener.EndAcceptTcpClient(ar))
                {
                    CodeFlow("EndAcceptTcpClient returned");
                    OnClientConnection(client);
                }
            }
            catch (ObjectDisposedException)
            {
                CodeFlow("Caught ObjectDisposedException");
                //there must be a better way for this, but I don't know it.
                return;
            }
            CodeFlow("DoAcceptTcpClientCallback finished");
        }

        protected virtual void OnClientConnection(TcpClient client)
        {
            CodeFlow("SocketResponder is being connected to");
            var requestbytes = ReadRequestBytes(client);
            var match = DoesRequestMatchExpectedRequest(requestbytes);

            if (match)
            {
                lock (this) {
                    WriteResponseString(client);
                    GotConnectedTo = true;
                    CodeFlow("Wrote responsestring to socket, setting GotConnectedTo=true");
                }
            }
        }

        private void WriteResponseString(TcpClient client)
        {
            client.GetStream().Write(responsebytes, 0, responsebytes.Length);
        }

        private byte[] ReadRequestBytes(TcpClient client)
        {
            var request = new byte[100];

            var totalread = 0;
            var stream = client.GetStream();
            try
            {
                stream.ReadTimeout = 2000;
            }
            catch (Exception)
            {
                //in the unity_web profile, it turns out stream.ReadTimeout always throws.
            }
            try
            {
                totalread += stream.Read(request, 0, 100);
            }
            catch (IOException)
            {
            }
            var result = new byte[totalread];
            Array.Copy(request, result, totalread);
            return result;
        }

        public override void Dispose()
        {
            CodeFlow("Codeflow: Dispose called. listener null?" + (listener == null));
            if (listener == null) return;
            listener.Server.Close();
            listener.Stop();
            listener = null;
        }

        //This is here and not somewhere more logical because this cs file is actually used as a standalone monobehaviour in a test,
        //and it cnanot depend on anything outside the file.

        public static void AttemptMultipleTimes<T>(Action a, string msg) where T : Exception
        {
            var attempts = 0;
            while (true)
            {
                attempts++;
                try
                {
                    a();
                }
                catch (T)
                {
                    if (attempts > 4)
                        throw new Exception(msg);

                    Console.WriteLine(msg + ", sleeping for a while and trying again");
                    Thread.Sleep(500);
                    continue;
                }
                break;
            }
        }
    }
}
