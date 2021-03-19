using System;
using System.IO;
using System.IO.Compression;
using System.Linq;
using System.Security.Cryptography;
using System.Threading;
using Bee.Core;
using Bee.Core.Tests;
using Bee.Stevedore.Program;
using Bee.Stevedore.Program.Tests;
using Bee.TestHelpers;
using Bee.Tools;
using Bee.TundraBackend.Tests;
using NiceIO;
using NUnit.Framework;
using Unity.BuildTools;

namespace Bee.Stevedore.Tests
{
    [TestFixture]
    public class StevedoreTests : StevedoreTestsBase
    {
        // Lower client timeout. We want tests to be snappy, and everything is
        // localhost, so it SHOULD be. Windows and Linux seem to be happy with
        // as little as 100 ms, but experience shows that macOS unfortunately
        // needs at least 500 ms to be stable (both locally and on build farm).
        const int DefaultClientTimeout = 1000;
        string DefaultUserConfig => $@"
            timeout = {DefaultClientTimeout}
            cache-folder = {SteveCache}
        ";

        static readonly StevedoreArtifact DummyArtifact = new StevedoreArtifact("example", DummyArtifactId) { ExtractUsingSevenZip = false };

        private BeeAPITestBackend<string> TestBackend;

        [SetUp]
        public new void SetUp()
        {
            //TestBackend = new BeeAPITestBackend_StandaloneBee<string>();
            TestBackend = new BeeAPITestBackend_Tundra<string>();
            TestBackend.Start();

            // Mind MAX_PATH here.
            SteveCache = TestRoot.Combine("cache").EnsureDirectoryExists();

            Environment.SetEnvironmentVariable("HOME", TestRoot.ToString(SlashMode.Native));
            WithUserConfigFile(""); // just the default text
        }

        [Test]
        public void SetupArtifact_ThrowsIfRepoIsUnavailable()
        {
            var e = Assert.Throws<UnavailableRepositoryException>(
                () => ((IBackendRegistrable)DummyArtifact).Register(new DummyBackend())
            );
            Assert.AreEqual("Cannot download 'dummy'. No repository named 'example' has been configured", e.Message);
        }

        [Test]
        public void SetupTwoDifferentArtifacts_DifferentPaths()
        {
            var fooArtifact = StevedoreArtifact.Public("foo/123_0000000000000000000000000000000000000000000000000000000000000000.zip");
            var barArtifact = StevedoreArtifact.Public("bar/123_0000000000000000000000000000000000000000000000000000000000000000.zip");
            Assert.AreNotEqual(fooArtifact.Path, barArtifact.Path);
        }

        [Test]
        public void UseSameArtifactTwice()
        {
            var artifact1 = new StevedoreArtifact("public", DummyArtifactId);
            var artifact2 = new StevedoreArtifact("public", DummyArtifactId);
            Assert.AreEqual(artifact1.Path, artifact2.Path);
        }

        [Test]
        public void BuildTwice_SecondBuildIsNoOp()
        {
            var zipPath = CreateTestArtifactZip("hello");
            zipPath.Move(StevedoreCachePathFor(zipPath, "foo/123_*.zip", out var artifactId));

            Action<string> buildCode = dummy =>
            {
                var artifact = new StevedoreArtifact("public", artifactId) { ExtractUsingSevenZip = false };
                Backend.Current.Register(artifact);
            };
            TestBackend.Build(() => "dummy", buildCode);
            var result = TestBackend.Build(() => "dummy", buildCode);
            StringAssert.Contains("0 items updated", result.StdOut);
        }

        [Test]
        public void BuildTwice_WithDifferentVersions()
        {
            var zip1Path = CreateTestArtifactZip("v1-content");
            zip1Path.Move(StevedoreCachePathFor(zip1Path, "foo/v1_*.zip", out var artifactId1));
            var zip2Path = CreateTestArtifactZip("v2-content");
            zip2Path.Move(StevedoreCachePathFor(zip2Path, "foo/v2_*.zip", out var artifactId2));

            var artifact1 = new StevedoreArtifact("public", artifactId1) { ExtractUsingSevenZip = false };
            TestBackend.Build(() => "dummy", dummy =>
            {
                Backend.Current.Register(artifact1);
            });
            Assert.AreEqual("v1-content", TestRoot.Combine(artifact1.Path, "subdirectory/myfile.txt").ReadAllText());

            var artifact2 = new StevedoreArtifact("public", artifactId2) { ExtractUsingSevenZip = false };
            TestBackend.Build(() => "dummy", dummy =>
            {
                Backend.Current.Register(artifact2);
            });
            Assert.AreEqual("v2-content", TestRoot.Combine(artifact2.Path, "subdirectory/myfile.txt").ReadAllText());

            Assert.AreEqual(artifact1.Path, artifact2.Path);
        }

        [Test]
        public void CanCopyFileFromInsideArtifact()
        {
            const string targetFile = "targetfile";

            var content = "hello";
            var zipPath = CreateTestArtifactZip(content);
            zipPath.Move(StevedoreCachePathFor(zipPath, "testartifact/versionX_*.zip", out var artifactId));

            // cannot use closures to pass to standalone Bee backend
            Environment.SetEnvironmentVariable("_TEST", artifactId.ToString());

            TestBackend.Build(() => "dummy", _ =>
            {
                var artifact = StevedoreArtifact.Public(Environment.GetEnvironmentVariable("_TEST"));
                artifact.ExtractUsingSevenZip = false;
                Backend.Current.Register(artifact);
                var fileInArtifact = artifact.Path.Combine("subdirectory/myfile.txt");
                Assert.IsFalse(fileInArtifact.FileExists());

                Backend.Current.AddAction("myaction",
                    targetFiles: new[] {new NPath(targetFile)},
                    // deps added magically - TundraBackend.cs
                    inputs: new[] {fileInArtifact},
                    executableStringFor: HostPlatform.IsWindows ? "copy" : "cp",
                    commandLineArguments: new[]
                    {
                        fileInArtifact.InQuotes(SlashMode.Native),
                        new NPath(targetFile).InQuotes(SlashMode.Native),
                    }
                );
            });

            Assert.AreEqual(content, TestRoot.Combine(targetFile).ReadAllText());
        }

        [Test]
        public void SetupArtifact_DownloadsAndUnpacksArtifact()
        {
            var content = "hello";
            var tempZipFile = CreateTestArtifactZip(content);
            var artifactBytes = File.ReadAllBytes(tempZipFile.ToString());
            var artifactId = $"foo/bar_{Sha256Of(tempZipFile)}.zip";

            using (var server = new TestHttpServer())
            {
                WithUserConfigFile($@"
                    # (just to test explicitly disabling proxy, somewhere)
                    repo.xyzzy.proxy =
                    repo.xyzzy.url = {server.Origin}/r/xyzzy/
                    repo.xyzzy.http-header = X-Yzzy: Plugh!
                ");

                server.ExpectRequest($@"
                    GET /r/xyzzy/{artifactId} HTTP/1.1
                    Accept: */*
                    Connection: Keep-Alive
                    Host: {server.Host}
                    X-Yzzy: Plugh!
                ", conn =>
                {
                    conn.Write("HTTP/1.0 200 OK\r\nConnection: close\r\n\r\n");
                    conn.Write(artifactBytes);
                });

                var artifact = new StevedoreArtifact("xyzzy", new ArtifactId(artifactId)) { ExtractUsingSevenZip = false };
                var fileInArtifact = TestRoot.Combine(artifact.Path.Combine("subdirectory/myfile.txt"));
                TestBackend.Build(() => "dummy", _ =>
                {
                    Backend.Current.Register(artifact);
                    Assert.IsFalse(fileInArtifact.FileExists());
                });
                Assert.AreEqual(content, fileInArtifact.ReadAllText());
            }
        }

        [Test]
        public void SetupArtifact_FromLocalFile()
        {
            var content = "Local artifact file";
            var localArtifactPath = CreateTestArtifactZip(content);

            // check that StevedoreArtifact.FromLocalFile handles CWD changes correctly.
            StevedoreArtifact artifact;
            using (new TempChangeDirectory(localArtifactPath.Parent))
                artifact = StevedoreArtifact.FromLocalFile("my-artifact", localArtifactPath.FileName);
            artifact.ExtractUsingSevenZip = false;

            var fileInArtifact = TestRoot.Combine(artifact.Path.Combine("subdirectory/myfile.txt"));
            var result = TestBackend.Build(() => "dummy", _ =>
            {
                Backend.Current.Register(artifact);
                Assert.IsFalse(fileInArtifact.FileExists());
            });
            StringAssert.Contains("Unpack from local path", result.StdOut);
            Assert.AreEqual(content, fileInArtifact.ReadAllText());

            // TODO: how to test that changing the artifact causes build graph regeneration (and FromLocalFile to rehash)?
        }

        [Test]
        public void SetupArtifact_RejectsDownloadWithBadHash()
        {
            using (var server = new TestHttpServer())
            {
                WithUserConfigFile($@"
                    repo.example.url = {server.Origin}/
                ");

                server.ExpectRequest($@"
                    GET /{DummyArtifactId} HTTP/1.1
                    Accept: */*
                    Connection: Keep-Alive
                    Host: {server.Host}
                ", conn =>
                {
                    conn.Write("HTTP/1.0 200 OK\r\nConnection: close\r\n\r\n");
                    conn.Write(new byte[42]);     // definitely invalid content
                });

                var result = TestBackend.Build(() => "dummy", _ =>
                {
                    Backend.Current.Register(DummyArtifact);
                }, throwOnFailure: false);
                Assert.That(DownloadCache.PathFor(SteveCache, DummyArtifactId).Exists(), Is.False);
                StringAssert.Contains("Failed to download (integrity check failed):", result.StdOut);
            }
        }

        [Test]
        public void SetupArtifact_SlowDownloadDoesNotTimeOut()
        {
            var content = "slow";
            var tempZipFile = CreateTestArtifactZip(content);
            var artifactBytes = File.ReadAllBytes(tempZipFile.ToString());
            var artifactId = $"foo/bar_{Sha256Of(tempZipFile)}.zip";

            using (var server = new TestHttpServer(throwOnError: false))
            {
                WithUserConfigFile($@"
                    repo.xyzzy.url = {server.Origin}/whatever/
                ");

                server.ExpectRequest("very slow download", (conn, _) =>
                {
                    // Send response headers immediately.
                    conn.Write("HTTP/1.0 200 OK\r\nConnection: close\r\n\r\n");
                    // Delay before sending actual content (emulating a big download on a slow connection).
                    Thread.Sleep(DefaultClientTimeout * 2);
                    conn.Write(artifactBytes);
                    return true;
                });

                var artifact = new StevedoreArtifact("xyzzy", new ArtifactId(artifactId)) { ExtractUsingSevenZip = false };
                var fileInArtifact = TestRoot.Combine(artifact.Path, "subdirectory/myfile.txt");
                TestBackend.Build(() => "dummy", _ =>
                {
                    Backend.Current.Register(artifact);
                    Assert.IsFalse(fileInArtifact.FileExists());
                });
                Assert.AreEqual(content, fileInArtifact.ReadAllText());
            }
        }

        [Test]
        public void SetupArtifact_TimesOut()
        {
            using (var server = new TestHttpServer(throwOnError: false))
            {
                WithUserConfigFile($@"
                    ; Make the test time out (and thus succeed) quickly.
                    timeout = 100
                    repo.example.url = {server.Origin}/whatever/
                ");

                server.ExpectRequest("ignore request and hang", (conn, _) =>
                {
                    Thread.Sleep(5000);
                    conn.Write("HTTP/1.0 500 Bad bad bad\r\nConnection: close\r\n\r\n");
                    return true;
                });

                var result = TestBackend.Build(() => "dummy", _ =>
                {
                    Backend.Current.Register(DummyArtifact);
                }, throwOnFailure: false);
                StringAssert.Contains("Failed to download (timeout: 100 ms):", result.StdOut);
            }
        }

        [TestCase("400 Bad Request")]
        [TestCase("401 Unauthorized")]
        [TestCase("403 Forbidden")]
        [TestCase("404 Not Found")]
        [TestCase("418 I'm a teapot")]
        [TestCase("500 Internal Server Error")]
        [TestCase("502 Bad Gateway")]
        public void SetupArtifact_HttpErrorIsShownAsIs(string error)
        {
            using (var server = new TestHttpServer())
            {
                WithUserConfigFile($"repo.example.url = {server.Origin}/whatever/");

                server.ExpectRequest($@"
                    GET /whatever/{DummyArtifactId} HTTP/1.1
                    Accept: */*
                    Connection: Keep-Alive
                    Host: {server.Host}
                ",  conn =>
                {
                    conn.Write($"HTTP/1.0 {error}\r\nConnection: close\r\n\r\n");
                });

                var result = TestBackend.Build(() => "dummy", _ =>
                {
                    Backend.Current.Register(DummyArtifact);
                }, throwOnFailure: false);
                StringAssert.Contains("stevedore: Failed to download:", result.StdOut);
                StringAssert.Contains($"Server response was '{error}", result.StdOut);
            }
        }

        [Test]
        public void SetupArtifact_FollowsRedirect()
        {
            var content = "12345";
            var tempZipFile = CreateTestArtifactZip(content);
            var artifactBytes = File.ReadAllBytes(tempZipFile.ToString());
            var artifactId = $"foo/bar_{Sha256Of(tempZipFile)}.zip";

            using (var server = new TestHttpServer())
            {
                WithUserConfigFile($@"
                    # Tell Steve to use the old url; server will redirect and Steve will follow.
                    repo.xyzzy.url={server.Origin}/old/xyzzy/
                ");

                server.ExpectRequest($@"
                    GET /old/xyzzy/{artifactId} HTTP/1.1
                    Accept: */*
                    Connection: Keep-Alive
                    Host: {server.Host}
                ", conn =>
                {
                    conn.Write($"HTTP/1.0 302 Found\r\nLocation: {server.Origin}/new/plugh/{artifactId}\r\nConnection: close\r\n\r\n");
                });

                server.ExpectRequest($@"
                    GET /new/plugh/{artifactId} HTTP/1.1
                    Accept: */*
                    Connection: Keep-Alive
                    Host: {server.Host}
                ", conn =>
                {
                    conn.Write("HTTP/1.0 200 OK\r\nConnection: close\r\n\r\n");
                    conn.Write(artifactBytes);
                });

                var artifact = new StevedoreArtifact("xyzzy", new ArtifactId(artifactId)) { ExtractUsingSevenZip = false };
                var fileInArtifact = artifact.Path.Combine("subdirectory/myfile.txt");
                TestBackend.Build(() => "dummy", _ =>
                {
                    Backend.Current.Register(artifact);
                    Assert.IsFalse(fileInArtifact.FileExists());
                });
                Assert.AreEqual(content, TestRoot.Combine(fileInArtifact).ReadAllText());
            }
        }

        [TestCase("http_proxy", "http://PROXY:1234", false)]
        [TestCase("HTTP_PROXY", "http://wizard:potrzebie@PROXY:1234/", true)]
        public void SetupArtifact_SupportsHttpProxy(string proxyVariableName, string proxyVariableValue, bool requiresAuthorization)
        {
            var expectedAuthzHeader = requiresAuthorization ? "Proxy-Authorization: Basic d2l6YXJkOnBvdHJ6ZWJpZQ==\n" : "";

            using (var server = new TestHttpServer())
            {
                WithUserConfigFile(@"
                    repo.example.url=http://invalid.example.org/r/main/
                ");
                Environment.SetEnvironmentVariable(proxyVariableName, proxyVariableValue.Replace("PROXY:1234", server.Host));

                var expectedRequestLine = $"GET http://invalid.example.org/r/main/{DummyArtifactId} HTTP/1.1";

                if (expectedAuthzHeader != "")
                {
                    // The client will first send an unauthorized request.
                    var unauthorizedRequest = new TestHttpServer.Lines($@"
                        {expectedRequestLine}
                        Accept: */*
                        Host: invalid.example.org
                        Proxy-Connection: Keep-Alive
                    ");

                    bool isFirstRequest = true;
                    server.ExpectRequest($"(unauthorized) {expectedRequestLine}", (conn, request) =>
                    {
                        if (!isFirstRequest) return false;
                        isFirstRequest = false;

                        Assert.AreEqual(expectedRequestLine, request.Value[0]);
                        TestHttpServer.AssertHeaderLinesEquivalent(unauthorizedRequest.Value, request.Value, expectedRequestLine);

                        conn.Write("HTTP/1.0 407 This proxy wants a password\r\nProxy-Authenticate: Basic\r\nConnection: close\r\n\r\n");
                        return true;
                    });
                }

                server.ExpectRequest($@"
                    {expectedRequestLine}
                    Accept: */*
                    Host: invalid.example.org
                    Proxy-Connection: Keep-Alive
                    {expectedAuthzHeader}
                ", conn =>
                {
                    conn.Write("HTTP/1.0 500 Got it\r\nConnection: close\r\n\r\n");
                });

                var result = TestBackend.Build(() => "dummy", _ =>
                {
                    Backend.Current.Register(DummyArtifact);
                }, throwOnFailure: false);
                StringAssert.Contains("Server response was '500 Got it'", result.StdOut);
            }
        }

        [TestCase("HTTPS_PROXY", "http://PROXY:1234", false)]
        [TestCase("HTTPS_PROXY", "http://wizard:potrzebie@PROXY:1234/", true)]
        public void SetupArtifact_SupportsHttpsProxy(string proxyVariableName, string proxyVariableValue, bool requiresAuthorization)
        {
            using (var server = new TestHttpServer())
            {
                WithUserConfigFile(@"
                    repo.example.url=https://secure.example.org/r/main/
                ");
                Environment.SetEnvironmentVariable(proxyVariableName, proxyVariableValue.Replace("PROXY:1234", server.Host));

                // Some .NET implementations use HTTP/1.0, others 1.1 with
                // Proxy-Connection: Keep-Alive, and others 1.1 without.
                // For the CONNECT method, the difference is not signifant.
                var requestVersions = new[]
                {
                    new TestHttpServer.Lines(@"
                        CONNECT secure.example.org:443 HTTP/1.0
                        Host: secure.example.org
                    "),
                    new TestHttpServer.Lines(@"
                        CONNECT secure.example.org:443 HTTP/1.1
                        Host: secure.example.org
                        Proxy-Connection: Keep-Alive
                    "),
                    new TestHttpServer.Lines(@"
                        CONNECT secure.example.org:443 HTTP/1.1
                        Host: secure.example.org
                    "),
                };
                int requestVersion = -1;

                // The client will first send an unauthorized request.
                server.ExpectRequest("(unauthorized) CONNECT secure.example.org:443 HTTP/1.x", (conn, request) =>
                {
                    bool isFirstRequest = requestVersion == -1;
                    if (!isFirstRequest) return false;

                    var reqLine = request.Value[0];
                    if (reqLine == requestVersions[0].Value[0]) requestVersion = 0;
                    else if (reqLine == requestVersions[1].Value[0])
                    {
                        if (request.Value.Any(line => line.Contains("Proxy-Connection")))
                            requestVersion = 1;
                        else
                            requestVersion = 2;
                    }
                    else  Assert.Fail($"Expected '{requestVersions[0].Value[0]}' or '{requestVersions[1].Value[0]}', got '{reqLine}'");

                    TestHttpServer.AssertHeaderLinesEquivalent(requestVersions[requestVersion].Value, request.Value, $"(unauthorized) {reqLine}");

                    if (requiresAuthorization)
                        conn.Write("HTTP/1.0 407 This proxy wants a password\r\nProxy-Authenticate: Basic\r\nConnection: close\r\n\r\n");
                    else
                        conn.Write("HTTP/1.0 500 Got it\r\nConnection: close\r\n\r\n");
                    return true;
                });

                if (requiresAuthorization)
                {
                    server.ExpectRequest("(authorized) CONNECT secure.example.org:443 HTTP/1.x", (conn, request) =>
                    {
                        var expectedAuthzHeader = "Proxy-Authorization: Basic d2l6YXJkOnBvdHJ6ZWJpZQ==\n";
                        var authorizedRequest = new TestHttpServer.Lines(requestVersions[requestVersion].ToString() + expectedAuthzHeader);
                        Assert.AreEqual(authorizedRequest.Value[0], request.Value[0]);
                        TestHttpServer.AssertHeaderLinesEquivalent(authorizedRequest.Value, request.Value, $"(authorized) {request.Value[0]}");
                        conn.Write("HTTP/1.0 500 Got it\r\nConnection: close\r\n\r\n");
                        return true;
                    });
                }

                var result = TestBackend.Build(() => "dummy", _ =>
                {
                    Backend.Current.Register(DummyArtifact);
                }, throwOnFailure: false);
                StringAssert.Contains("Failed to download (network or proxy error)", result.StdOut);
            }
        }

        // -----

        private NPath TestRoot => TestBackend.TestRoot;
        private NPath SteveCache;

        private NPath CreateTestArtifactZip(string content)
        {
            var artifactSourceDirectory = TestRoot.Combine("artifactSource");
            var artifactSourceFile = artifactSourceDirectory.Combine("subdirectory/myfile.txt");
            artifactSourceFile.EnsureParentDirectoryExists();
            artifactSourceFile.WriteAllText(content);

            var tempZipfile = TestRoot.Combine("tempzipfile.zip");
            ZipFile.CreateFromDirectory(artifactSourceDirectory.ToString(SlashMode.Native), tempZipfile.ToString(SlashMode.Native));
            return tempZipfile;
        }

        private NPath StevedoreCachePathFor(NPath sourceFile, string artifactIdTemplate, out ArtifactId artifactId)
        {
            artifactId = new ArtifactId(artifactIdTemplate.Replace("*", Sha256Of(sourceFile)));
            var path = DownloadCache.PathFor(SteveCache, artifactId);
            path.EnsureParentDirectoryExists();
            return path;
        }

        private static string Sha256Of(NPath file)
        {
            using (var fileStream = new FileStream(file.ToString(SlashMode.Native), FileMode.Open))
            using (var hasher = SHA256.Create())
            {
                return hasher.ComputeHash(fileStream).ToHexString();
            }
        }

        /// <summary>
        /// Append given text to the default user Stevedore.conf.
        /// </summary>
        void WithUserConfigFile(string configText)
        {
            TestRoot.Combine("Stevedore.conf").WriteAllText(DefaultUserConfig + configText);
        }
    }
}
