using System;
using System.IO;
using NUnit.Framework;

namespace Bee.Stevedore.Program.Tests
{
    public class DownloaderTests : StevedoreTestsBase
    {
        // Test different address types, and with and without trailing slash or credentials.
        [TestCase(null,                                 null, null, null)]
        [TestCase("",                                   null, null, null)]
        [TestCase("http://example.org",                 "http://example.org/", null, null)]
        [TestCase("http://example.org/",                "http://example.org/", null, null)]
        [TestCase("http://example.com:1234",            "http://example.com:1234/", null, null)]
        [TestCase("http://127.0.0.1:1234/",             "http://127.0.0.1:1234/", null, null)]
        [TestCase("http://bob:fred@[2001:db8::beef]:1", "http://[2001:db8::beef]:1/", "bob", "fred", "http://[2001:0DB8:0000:0000:0000:0000:0000:BEEF]:1/")]
        [TestCase("http://r%c3%b8d:gr%c3%b8d@example.com:1234",    "http://example.com:1234/", "r\u00f8d", "gr\u00f8d")]
        public void ParseHttpProxyUrl(string url, string expectedAddress, string expectedUsername, string expectedPassword, string expectedAddressAlternative = null)
        {
            var proxy = Downloader.ParseHttpProxyUrl(url);

            if (expectedAddressAlternative != null) // Yet another .NET implementation incompatibility
                Assert.That(proxy.Address.ToString() == expectedAddress || proxy.Address.ToString() == expectedAddressAlternative);
            else
                Assert.AreEqual(expectedAddress, proxy?.Address?.ToString());

            var credentials = proxy?.Credentials?.GetCredential(new Uri("http://localhost/"), "basic");
            Assert.AreEqual(expectedUsername, credentials?.UserName);
            Assert.AreEqual(expectedPassword, credentials?.Password);
        }

        [Test]
        public void NoRepoUrlConfigured_DownloadThrows()
        {
            var config = new StevedoreConfig();
            config.Read(new StringReader("repo.xyzzy.http-header = Foo: Bar"), reportParseError: null);
            Assert.Throws<UnavailableRepositoryException>(() => new Downloader().Download("xyzzy", config.RepoConfigFor("xyzzy"), DummyArtifactId, null));
        }
    }
}
