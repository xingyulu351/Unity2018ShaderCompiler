#include <QtNetwork>
#include <limits>
#include <stdexcept>

namespace ureport
{
namespace details
{
    class InvalidProxySettingsException : std::runtime_error
    {
    public:
        InvalidProxySettingsException() : std::runtime_error("Invalid proxy settings specified") {}
    };

    class ProxyUtils
    {
    public:
        static QNetworkProxy CreateProxyFromUrl(const QString& proxy)
        {
            const QString httpScheme = "http";

            QNetworkProxy networkProxy(QNetworkProxy::NoProxy);

            const QUrl proxyUrl(proxy, QUrl::StrictMode);

            if (!proxyUrl.isValid())
                return networkProxy;

            QString scheme = proxyUrl.scheme();
            if (!scheme.isEmpty() && 0 != httpScheme.compare(scheme, Qt::CaseInsensitive))
                return networkProxy;

            networkProxy.setType(QNetworkProxy::HttpProxy);
            SetupEndpoint(networkProxy, proxyUrl);
            SetupCredentials(networkProxy, proxyUrl);
            return networkProxy;
        }

    private:
        static void SetupEndpoint(QNetworkProxy& proxy, const QUrl& url)
        {
            const int defaultPort = -1;

            proxy.setHostName(url.host());
            if (url.port() != defaultPort)
                proxy.setPort(url.port());
        }

        static void SetupCredentials(QNetworkProxy& proxy, const QUrl& url)
        {
            const QString user = url.userName();
            if (!user.isEmpty())
            {
                proxy.setUser(user);
                const QString password = url.password();
                if (!password.isEmpty())
                    proxy.setPassword(password);
            }
        }
    };
}
}
