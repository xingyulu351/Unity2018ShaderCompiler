using System.Collections.Generic;
using System.Net;
using System.Net.Security;
using System.Security.Cryptography.X509Certificates;

namespace Bee.Stevedore.Program
{
    public class SslCertificateException : HumaneException
    {
        public SslCertificateException(string message) : base(message) {}

        public static bool ServerCertificateValidationCallback(object sender, X509Certificate certificate, X509Chain chain, SslPolicyErrors sslPolicyErrors)
        {
            if (sslPolicyErrors == SslPolicyErrors.None) return true; // cert is valid (by normal .NET standards)

            var detailList = new List<string>();

            if ((sslPolicyErrors & SslPolicyErrors.RemoteCertificateNotAvailable) != 0)
                detailList.Add("Server did not present a certificate");

            if ((sslPolicyErrors & SslPolicyErrors.RemoteCertificateNameMismatch) != 0)
                detailList.Add("Server certificate did not match hostname");

            foreach (var item in chain.ChainStatus)
            {
                if (item.Status == X509ChainStatusFlags.NoError) continue;

                var text = item.Status.ToString();
                if (item.StatusInformation != text)
                    text += ": " + item.StatusInformation.TrimEnd('\r', '\n', '.');
                detailList.Add(text);
            }

            // MSDN: "[...] the sender parameter [...] can be a host string name or an object derived from WebRequest [...]"
            if (sender is WebRequest)
                sender = ((WebRequest)sender).RequestUri;

            var details = detailList.Count == 0 ? "No details available" : string.Join("; ", detailList);
            throw new SslCertificateException($"SSL certificate validation failed for {sender} ({details})");
        }
    }
}
