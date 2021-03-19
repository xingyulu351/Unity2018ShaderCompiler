#pragma once

#include "long_term_operation/LongTermOperObserver.h"
#include <QNetworkReply>
#include <string>

namespace ureport
{
namespace details
{
    class NetworkReplyProgressListener : public QObject
    {
        Q_OBJECT;

    public:
        NetworkReplyProgressListener(LongTermOperObserver* observer,   QNetworkReply* netReply)
            : m_Observer(observer)
            , m_NetworkReply(netReply)
            , m_ChunksCount(0)
        {
            QObject::connect(m_NetworkReply,
                SIGNAL(error(QNetworkReply::NetworkError)),
                this,
                SLOT(NetworkReplyError(QNetworkReply::NetworkError)));
            QObject::connect(m_NetworkReply,
                SIGNAL(uploadProgress(qint64, qint64)),
                this,
                SLOT(NetworkReplyUploadProgress(qint64, qint64)));
        }

        std::string GetErrorString()
        {
            return m_ErrorString;
        }

    private slots:
        void NetworkReplyError(QNetworkReply::NetworkError code)
        {
            m_ErrorString = m_NetworkReply->errorString().toStdString();
        }

        void NetworkReplyUploadProgress(qint64 bytesSent, qint64 bytesTotal)
        {
            if (bytesTotal == 0)
                return;
            if (m_Observer)
            {
                if (m_ChunksCount == 0)
                    m_Observer->ReportProgress(0, bytesTotal);
                m_Observer->ReportProgress(bytesSent, bytesTotal);
            }
            ++m_ChunksCount;
        }

    private:
        NetworkReplyProgressListener(const NetworkReplyProgressListener&);
        LongTermOperObserver* m_Observer;
        QNetworkReply* m_NetworkReply;
        std::string m_ErrorString;
        size_t m_ChunksCount;
    };
}
}
