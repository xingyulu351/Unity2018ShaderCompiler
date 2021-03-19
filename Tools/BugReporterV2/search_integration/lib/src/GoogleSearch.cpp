#include "GoogleSearch.h"

#include <sstream>

namespace ureport
{
    const char* const equifaxCertData =
        "-----BEGIN CERTIFICATE-----\n"
        "MIIDIDCCAomgAwIBAgIENd70zzANBgkqhkiG9w0BAQUFADBOMQswCQYDVQQGEwJV\n"
        "UzEQMA4GA1UEChMHRXF1aWZheDEtMCsGA1UECxMkRXF1aWZheCBTZWN1cmUgQ2Vy\n"
        "dGlmaWNhdGUgQXV0aG9yaXR5MB4XDTk4MDgyMjE2NDE1MVoXDTE4MDgyMjE2NDE1\n"
        "MVowTjELMAkGA1UEBhMCVVMxEDAOBgNVBAoTB0VxdWlmYXgxLTArBgNVBAsTJEVx\n"
        "dWlmYXggU2VjdXJlIENlcnRpZmljYXRlIEF1dGhvcml0eTCBnzANBgkqhkiG9w0B\n"
        "AQEFAAOBjQAwgYkCgYEAwV2xWGcIYu6gmi0fCG2RFGiYCh7+2gRvE4RiIcPRfM6f\n"
        "BeC4AfBONOziipUEZKzxa1NfBbPLZ4C/QgKO/t0BCezhABRP/PvwDN1Dulsr4R+A\n"
        "cJkVV5MW8Q+XarfCaCMczE1ZMKxRHjuvK9buY0V7xdlfUNLjUA86iOe/FP3gx7kC\n"
        "AwEAAaOCAQkwggEFMHAGA1UdHwRpMGcwZaBjoGGkXzBdMQswCQYDVQQGEwJVUzEQ\n"
        "MA4GA1UEChMHRXF1aWZheDEtMCsGA1UECxMkRXF1aWZheCBTZWN1cmUgQ2VydGlm\n"
        "aWNhdGUgQXV0aG9yaXR5MQ0wCwYDVQQDEwRDUkwxMBoGA1UdEAQTMBGBDzIwMTgw\n"
        "ODIyMTY0MTUxWjALBgNVHQ8EBAMCAQYwHwYDVR0jBBgwFoAUSOZo+SvSspXXR9gj\n"
        "IBBPM5iQn9QwHQYDVR0OBBYEFEjmaPkr0rKV10fYIyAQTzOYkJ/UMAwGA1UdEwQF\n"
        "MAMBAf8wGgYJKoZIhvZ9B0EABA0wCxsFVjMuMGMDAgbAMA0GCSqGSIb3DQEBBQUA\n"
        "A4GBAFjOKer89961zgK5F7WF0bnj4JXMJTENAKaSbn+2kmOeUJXRmm/kEd5jhW6Y\n"
        "7qj/WsjTVbJmcVfewCHrPSqnI0kBBIZCe/zuf6IWUrVnZ9NA2zsmWLIodz2uFHdh\n"
        "1voqZiegDfqnc1zqcPGUIWVEX/r87yloqaKHee9570+sB3c4\n"
        "-----END CERTIFICATE-----";

    const char* const site = "https://www.googleapis.com/customsearch/v1";
    const char* const engineId = "008783607536934399950:lei0vctt55s";
    const char* const devKey = "AIzaSyCNyKM-AyJoNmx0oRgl7XIrgbB7pMXA7VM";
    const char* const prodKey = "AIzaSyCMGfdDaSfjqv5zYoS0mTJnOT3e9MURWkU";

    GoogleSearch::GoogleSearch()
        : m_SearchAPIUrl(GetSearchAPIUrl())
        , m_SslConfiguration(ConfigureSsl())
        , m_NetworkManager()
    {
        QObject::connect(&m_NetworkManager, SIGNAL(finished(QNetworkReply*)),
            this, SLOT(HandleReply(QNetworkReply*)));
    }

    std::string GoogleSearch::GetSearchAPIUrl()
    {
        return std::string(site) + "?key=" + prodKey + "&cx=" + engineId + "&q=";
    }

    QSslConfiguration GoogleSearch::ConfigureSsl()
    {
        auto config = QSslConfiguration::defaultConfiguration();
        auto certBase = config.caCertificates();
        certBase.append(QSslCertificate(QByteArray(equifaxCertData)));
        config.setCaCertificates(certBase);
        return config;
    }

    void GoogleSearch::PerformSearch(std::string searchString,
        std::function<void(std::string)> callback)
    {
        m_Callback = callback;
        MakeQuery(GetQueryUrl(searchString));
    }

    void GoogleSearch::MakeQuery(const std::string& url) const
    {
        QNetworkRequest request(QString::fromStdString(url));
        request.setSslConfiguration(m_SslConfiguration);
        m_NetworkManager.get(request);
    }

    std::string GoogleSearch::GetQueryUrl(const std::string& q) const
    {
        return m_SearchAPIUrl + q;
    }

    void GoogleSearch::HandleReply(QNetworkReply *networkReply)
    {
        if (networkReply->error())
            m_Callback("");
        auto data = networkReply->readAll();
        QString dataStr(data);
        m_Callback(dataStr.toStdString());
    }
}
