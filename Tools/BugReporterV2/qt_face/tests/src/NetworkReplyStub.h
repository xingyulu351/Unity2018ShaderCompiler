#include <QNetworkReply>

namespace ureport
{
namespace test
{
    class NetworkReplyStub : public QNetworkReply
    {
        char* m_Content;
        qint64 m_Size;
        qint64 m_Offset;
    public:
        NetworkReplyStub(QNetworkReply::NetworkError errorCode, QString content) : m_Offset(0)
        {
            auto errorString = errorCode != QNetworkReply::NoError ? "Error happened!" : "";
            setError(errorCode, errorString);
            setOpenMode(QIODevice::ReadOnly);
            m_Content = new char[content.size()];
            strcpy(m_Content, content.toStdString().c_str());
            m_Size = (qint64)std::strlen(m_Content);
        }

        void abort() {}

        qint64 readData(char *data, qint64 maxlen)
        {
            if (m_Offset >= m_Size)
            {
                return -1;
            }
            qint64 number = qMin(maxlen, m_Size - m_Offset);
            memcpy(data, m_Content + m_Offset, number);
            m_Offset += number;
            return number;
        }
    };
}
}
