#pragma once

#include "search_integration/SearchPerformer.h"

#include <QtNetwork>
#include <QObject>
#include <string>

namespace ureport
{
    class GoogleSearch : public QObject, public SearchPerformer
    {
        Q_OBJECT

    public:
        GoogleSearch();

        virtual void PerformSearch(std::string searchString,
            std::function<void(std::string)> callback);

    public slots:
        void HandleReply(QNetworkReply *networkReply);

    private:
        static std::string GetSearchAPIUrl();

        static QSslConfiguration ConfigureSsl();

        void MakeQuery(const std::string& url) const;

        std::string GetQueryUrl(const std::string& q) const;

    private:
        const std::string m_SearchAPIUrl;
        const QSslConfiguration m_SslConfiguration;
        mutable QNetworkAccessManager m_NetworkManager;
        std::function<void(std::string)> m_Callback;
    };
}
