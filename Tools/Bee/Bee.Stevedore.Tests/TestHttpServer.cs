using System;
using System.Collections.Generic;
using System.Diagnostics;
using System.IO;
using System.Linq;
using System.Net;
using System.Net.Sockets;
using System.Runtime.ExceptionServices;
using System.Text;
using System.Text.RegularExpressions;
using System.Threading;
using NUnit.Framework;

namespace Bee.Stevedore.Tests
{
    public class TestHttpServer : IDisposable
    {
        // We should usually listen on port 0 (making the OS pick an available
        // port), but during debugging it can be useful to force specific port.
        const int ListenPort = 0; // 0 = auto-assign port

        /// <summary>
        /// A request handler return false to pass on handling a request, or
        /// otherwise handles it and returns true.
        /// </summary>
        public delegate bool RequestHandler(Connection connection, Lines request);

        public struct Lines
        {
            static readonly Regex reSplitIntoLines = new Regex(@" *\r?\n *");

            public readonly string[] Value;

            public Lines(string[] lines)
            {
                Value = lines;
            }

            public Lines(string text) : this(reSplitIntoLines.Split(text.Trim())) {}

            public override string ToString()
            {
                return string.Join("\r\n", Value) + "\r\n";
            }

            public static implicit operator Lines(string text) => new Lines(text);
            public static implicit operator Lines(string[] lines) => new Lines(lines);
        }

        /// <summary>
        /// Wraps a socket and provides the ability to read line-by-line in a
        /// binary safe manner, as needed by the HTTP protocol.
        /// </summary>
        public class Connection
        {
            // These three buffering fields are only used for reading. The class does not buffer writes.
            int m_ReadOffset;
            int m_WriteOffset;
            readonly byte[] m_Buffer;
            readonly Socket m_Socket;

            int SpareCapacity => m_Buffer.Length - m_WriteOffset;

            public Connection(Socket socket, int readBufferSize)
            {
                m_Socket = socket;
                m_Buffer = new byte[readBufferSize];
                m_ReadOffset = m_WriteOffset = 0;
            }

            public void Write(string text)
            {
                Write(Encoding.ASCII.GetBytes(text));
            }

            public void Write(byte[] buffer)
            {
                m_Socket.Send(buffer);
            }

            /// <summary>
            /// Read a single CRLF-terminated ASCII line from the socket.
            /// Throws if line ending is missing or invalid, or if line length
            /// exceeds the configured read buffer size.
            /// </summary>
            public string ReadLine()
            {
                int searchFrom = m_ReadOffset;
                int newlineOffset;
                while (true)
                {
                    newlineOffset = Array.IndexOf(m_Buffer, (byte)'\n', searchFrom, m_WriteOffset - searchFrom);
                    if (newlineOffset >= 0) break;

                    searchFrom = m_WriteOffset; // On next iteration, only search the newly read data
                    FeedBuffer();
                }

                --newlineOffset;
                if (newlineOffset < m_ReadOffset || m_Buffer[newlineOffset] != (byte)'\r')
                    throw new IOException($"{nameof(Connection)} received LF terminated line (HTTP lines must be CR LF terminated)");

                var line = Encoding.ASCII.GetString(m_Buffer, m_ReadOffset, newlineOffset - m_ReadOffset);
                m_ReadOffset = newlineOffset + 2; // skip the CR LF
                return line;
            }

            /// <summary>
            /// Moves buffered data to a new offset within m_Buffer (no error checking).
            /// </summary>
            void MoveBufferedDataToOffset(int newReadOffset)
            {
                int bufferedDataSize = m_WriteOffset - m_ReadOffset;
                Array.Copy(m_Buffer, m_ReadOffset, m_Buffer, newReadOffset, bufferedDataSize);
                m_WriteOffset = newReadOffset + bufferedDataSize;
                m_ReadOffset = newReadOffset;
            }

            /// <summary>
            /// Read more data from socket into the read buffer.
            /// </summary>
            void FeedBuffer()
            {
                if (SpareCapacity <= 0)
                {
                    if (m_ReadOffset == 0)
                        throw new IOException($"{nameof(Connection)} is full (cannot read any more)");

                    MoveBufferedDataToOffset(0);
                }
                int nBytesRead = m_Socket.Receive(m_Buffer, m_WriteOffset, SpareCapacity, SocketFlags.None);
                if (nBytesRead == 0) throw new EndOfStreamException();
                m_WriteOffset += nBytesRead;
            }
        }

        class RegisteredRequest
        {
            public string Description;
            public RequestHandler Handler;
            public override string ToString() => Description;
        }

        readonly List<RegisteredRequest> m_ActualRequestOrder = new List<RegisteredRequest>();
        readonly List<RegisteredRequest> m_ExpectedRequestOrder = new List<RegisteredRequest>();
        readonly int m_Timeout;
        readonly bool m_ThrowOnError;
        readonly Socket m_Socket;
        Exception m_RequestHandlerException;
        readonly Thread m_Thread;
        bool m_IsDisposed;

        public string Host { get; }
        public string Origin { get; }

        public TestHttpServer(int timeout = 2000, bool throwOnError = true)
        {
            // Too low timeout can cause timeouts (obviously), which can manifest
            // themselves as the "Operation on non-blocking socket would block"
            // error (less obviously).
            m_Timeout = timeout;
            m_ThrowOnError = throwOnError;

            m_Socket = new Socket(AddressFamily.InterNetwork, SocketType.Stream, ProtocolType.Tcp);
            m_Socket.SendTimeout = m_Socket.ReceiveTimeout = m_Timeout;
            m_Socket.Bind(new IPEndPoint(IPAddress.Parse("127.0.0.1"), ListenPort));
            Host = $"127.0.0.1:{Port}";
            Origin = $"http://127.0.0.1:{Port}";
            m_Socket.Listen(1);

            m_Thread = new Thread(RequestHandlerThread);
            m_Thread.IsBackground = true;
            m_Thread.Name = "TestHttpServer";
            m_Thread.Start();
        }

        private int Port => ((IPEndPoint)m_Socket.LocalEndPoint).Port;

        [Conditional("false")]
        void Log(Socket clientSocket, string message)
        {
            var clientName = "";
            if (clientSocket != null)
                clientName = ":" + ((IPEndPoint)clientSocket.RemoteEndPoint).Port;
            Console.Error.Write($"{clientName,-6} {message}\n");
        }

        void RequestHandlerThread()
        {
            try
            {
                while (true)
                {
                    Log(null, "Waiting for client");
                    var clientSocket = m_Socket.Accept();
                    Log(clientSocket, "- accepted");

                    try
                    {
                        clientSocket.SendTimeout = clientSocket.ReceiveTimeout = m_Timeout;
                        var conn = new Connection(clientSocket, 8192);

                        var requestLines = new List<string>();
                        while (true)
                        {
                            var line = conn.ReadLine();
                            Log(clientSocket, "< " + line);
                            if (line == "") break;
                            requestLines.Add(line);
                        }
                        var request = new Lines(requestLines.ToArray());

                        bool wasHandled = false;
                        foreach (var registeredRequest in m_ExpectedRequestOrder)
                        {
                            if (registeredRequest.Handler(conn, request))
                            {
                                m_ActualRequestOrder.Add(registeredRequest);
                                wasHandled = true;
                                break;
                            }
                        }
                        if (!wasHandled)
                            throw new AssertionException($"Unexpected request: {string.Join("\n", request.Value)}");
                    }
                    catch
                    {
                        Log(clientSocket, "- error while responding");
                        clientSocket.Send(Encoding.ASCII.GetBytes("HTTP/1.0 999 Everything is wrong\r\nConnection: close\r\n\r\n"));
                        throw;
                    }
                    finally
                    {
                        Log(clientSocket, "- closing");
                        clientSocket.Dispose();
                    }
                }
            }
            // When disposing TestHttpServer, there's a race between
            // Accept noticing the call to m_Socket.Close and throwing
            // ObjectDisposedException, the OS noticing the closed
            // socket and throwing SocketException, and the main
            // thread calling m_Thread.Abort and causing the runtime
            // to throw ThreadAbortException. It's all to be expected
            // during a clean shutdown.
            catch (ThreadAbortException)
            {
                // Additional .NET black magic required to handle this.
                Thread.ResetAbort();
            }
            catch (Exception e)
            {
                lock (this)
                {
                    if (!m_IsDisposed)
                        m_RequestHandlerException = e;
                }
            }
        }

        public void Dispose()
        {
            lock (this)
            {
                if (m_IsDisposed) return;
                m_IsDisposed = true;
            }

            m_Socket?.Close(); // Interrupt e.g. Accept.
            if (m_Thread != null)
            {
                m_Thread.Abort();
                m_Thread.Join();
                lock (this)
                {
                    if (m_ThrowOnError && m_RequestHandlerException != null)
                    {
                        ExceptionDispatchInfo.Capture(m_RequestHandlerException).Throw();
                    }
                }
            }

            if (m_ThrowOnError)
            {
                // Verify we actually received all registered requests, only once each, and in registration order.
                CollectionAssert.AreEqual(m_ExpectedRequestOrder, m_ActualRequestOrder);
            }
        }

        public void ExpectRequest(string description, RequestHandler handler)
        {
            m_ExpectedRequestOrder.Add(new RegisteredRequest { Description = description, Handler = handler });
        }

        public void ExpectRequest(Lines expectedRequest, Action<Connection> andThen)
        {
            var requestLine = expectedRequest.Value[0];
            ExpectRequest(requestLine, (connection, request) =>
            {
                if (request.Value[0] != requestLine) return false;

                AssertHeaderLinesEquivalent(expectedRequest.Value, request.Value, requestLine);

                andThen(connection);

                return true;
            });
        }

        /// <summary>
        /// Check header lines (but not the initial request line), ignoring
        /// order and case (as both differ across .NET implementations).
        /// </summary>
        public static void AssertHeaderLinesEquivalent(IEnumerable<string> expectedRequest, IEnumerable<string> actualRequest, string whichRequest)
        {
            CollectionAssert.AreEquivalent(
                expectedRequest.Skip(1).Select(s => s.ToLowerInvariant()),
                actualRequest.Skip(1).Select(s => s.ToLowerInvariant()),
                "Unexpected headers in request " + whichRequest);
        }
    }
}
