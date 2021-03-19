using System;
using System.IO;
using System.Net;
using System.Net.Http;
using System.Security.Cryptography;
using System.Threading;
using System.Threading.Tasks;
using System.Web;
using Bee.Tools;

namespace Bee.Stevedore.Program
{
    public class Downloader
    {
        // Should be a multiple of the biggest NTFS cluster size (64 kiB).
        // Too big and it might interfere with progress reporting or responsiveness
        // to "managed" Ctrl+C, but we don't have either of those at this point.
        const int PreferredIOBlockSize = 1024 * 1024;

        static Downloader()
        {
            // Register for SSL certificate checks GLOBALLY.
            // TODO: once we get to .NET 4.7.1, we can drop this horrible global variable and use
            // per-connection HttpClientHandler.ServerCertificateCustomValidationCallback instead.

            if (ServicePointManager.ServerCertificateValidationCallback != null)
                throw new InvalidOperationException("Someone else also wants to control ServerCertificateValidationCallback");

            ServicePointManager.ServerCertificateValidationCallback = SslCertificateException.ServerCertificateValidationCallback;
        }

        public void Download(string repoName, StevedoreConfig.RepoConfig repoConfig, ArtifactId artifactId, FileStream ostream)
        {
            if (string.IsNullOrEmpty(repoConfig.Url))
                throw new UnavailableRepositoryException($"No URL configured for repository '{repoName}'");

            var url = repoConfig.Url + artifactId;

            var request = new HttpRequestMessage(HttpMethod.Get, url);
            request.Headers.Add("Accept", "*/*");
            if (repoConfig.CustomHttpHeader != null)
            {
                var split = repoConfig.CustomHttpHeader.Split(new[] {':'}, 2);
                request.Headers.Add(split[0], split[1].Trim());
            }

            DownloadWrap(repoConfig, request, (client, cancellationToken) =>
            {
                var task = Task.Run(async() =>
                {
                    var response = await client.SendAsync(request, HttpCompletionOption.ResponseHeadersRead, cancellationToken);
                    if (!response.IsSuccessStatusCode)
                        throw new HumaneException($"Failed to download: {url}\nServer response was '{(int)response.StatusCode} {response.ReasonPhrase}'");
                    cancellationToken.ThrowIfCancellationRequested();
                    return await response.Content.ReadAsStreamAsync();
                }, cancellationToken);

                // Since SendAsync is not cooperatively cancellable at all times
                // during its execution, this Wait call may abort but leave the
                // ongoing request running in the background, to be cancelled later.
                task.Wait(cancellationToken);

                // Now that we've received the response headers and can commence
                // the download proper, the timeout no longer applies.
                using (var istream = task.Result)
                using (ostream)
                using (var hasher = SHA256.Create())
                {
                    var buffer = new byte[PreferredIOBlockSize];
                    int nRead;
                    while ((nRead = istream.Read(buffer, 0, buffer.Length)) != 0)
                    {
                        hasher.TransformBlock(buffer, 0, nRead, buffer, 0);
                        ostream.Write(buffer, 0, nRead);
                    }
                    hasher.TransformFinalBlock(buffer, 0, 0);

                    if (hasher.Hash.ToHexString() != artifactId.Version.Hash)
                        throw new HumaneException($"Failed to download (integrity check failed): {url}");
                }
            });
        }

        void DownloadWrap(StevedoreConfig.RepoConfig config, HttpRequestMessage request, Action<HttpClient, CancellationToken> action)
        {
            var proxyUrl = request.RequestUri.Scheme == "https" ? config.HttpsProxyUrl : config.HttpProxyUrl;

            // TODO: what happens on network error?
            using (request)
            using (var timeoutCts = new CancellationTokenSource(config.Timeout))
            using (var client = new HttpClient(
                handler: new HttpClientHandler { Proxy = ParseHttpProxyUrl(proxyUrl) },
                disposeHandler: true
            ))
            {
                // We can't use client.Timeout; it's not reliably implemented across .NET implementations.
                // Instead, disable that one, and implement our own via CancellationTokenSource.
                client.Timeout = Timeout.InfiniteTimeSpan;
                try
                {
                    try
                    {
                        action(client, timeoutCts.Token);
                    }
                    catch (AggregateException e) when (e.InnerExceptions.Count == 1)
                    {
                        // Unwrap AggregateException.
                        throw e.InnerExceptions[0];
                    }
                }
                catch (HttpRequestException e)
                {
                    // Something went wrong. HttpRequestException tells us basically nothing, so we have to do a little digging.
                    Exception bestExplanation = e;
                    var inner = e.InnerException;
                    while (inner != null)
                    {
                        // Presumably SslCertificateException, but we can generalize to any HumaneException.
                        if (inner is HumaneException) throw inner;     // use as-is

                        // Generally, the useful exceptions come deeper in the exception chain.
                        if (inner is CryptographicException ||     // e.g. cert too big (https://1000-sans.badssl.com/)
                            inner is WebException ||               // e.g. HTTP error responses from proxy
                            inner is IOException)                  // e.g. unsupported algorithm (https://ecc384.badssl.com/)
                            bestExplanation = inner;

                        inner = inner.InnerException;
                    }
                    throw new HumaneException($"Failed to download (network{(string.IsNullOrEmpty(proxyUrl) ? "" : " or proxy")} error): {request.RequestUri}\n{bestExplanation.Message}", e);
                }
                catch (OperationCanceledException) when (timeoutCts.IsCancellationRequested)
                {
                    throw new HumaneException($"Failed to download (timeout: {config.Timeout} ms): {request.RequestUri}");
                }
            }
        }

        static ICredentials DecodeUserInfo(string userInfo)
        {
            if (userInfo.Length == 0) return null;
            var userPass = userInfo.Split(new[] {':'}, count: 2);
            return new NetworkCredential(HttpUtility.UrlDecode(userPass[0]), HttpUtility.UrlDecode(userPass[1]));
        }

        internal static WebProxy ParseHttpProxyUrl(string url)
        {
            if (string.IsNullOrEmpty(url)) return null;

            var proxyUrl = new Uri(url, UriKind.Absolute);
            return new WebProxy(
                Address: proxyUrl.GetComponents(UriComponents.HttpRequestUrl, UriFormat.UriEscaped),
                BypassOnLocal: false,
                Credentials: DecodeUserInfo(proxyUrl.UserInfo),
                BypassList: Array.Empty<string>()
            );
        }
    }
}
