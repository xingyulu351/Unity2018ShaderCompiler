#include "reporter/ReportDraft.h"
#include "CrashAnalyzer.h"
#include <QHttpMultiPart>
#include <QNetworkReply>
#include <QUrl>

namespace ureport
{
namespace test
{
    class CrashAnalyzerStub : public qt_face::CrashAnalyzer
    {
        std::shared_ptr<QHttpMultiPart> m_RequestMultiPart;
        QUrl m_RequestUrl;
        std::string m_Version, m_Platform, m_EditorLogPath;
        std::string m_ReplyContent;
        QNetworkReply::NetworkError m_Error;
    public:
        CrashAnalyzerStub(std::string serverUrl, std::shared_ptr<ReportDraft> report) :
            CrashAnalyzer(serverUrl, report),
            m_Error(QNetworkReply::NoError),
            m_ReplyContent("") {}

        CrashAnalyzerStub(QNetworkReply::NetworkError error, std::string replyContent, std::shared_ptr<ReportDraft> report) :
            CrashAnalyzer("http://localhost", report), m_Error(error), m_ReplyContent(replyContent) {}

        QJsonObject GetReport()
        {
            return CrashAnalyzer::GetReport();
        }

        QUrl GetRequestUrl()
        {
            return m_RequestUrl;
        }

        std::string GetVersion()
        {
            return m_Version;
        }

        std::string GetPlatform()
        {
            return m_Platform;
        }

        std::string GetEditorLogPath()
        {
            return m_EditorLogPath;
        }

        std::shared_ptr<QHttpMultiPart> GetRequestMultiPart()
        {
            return m_RequestMultiPart;
        }

    protected:
        QJsonObject GetReport(std::string version, std::string platform, std::string logFilePath) override
        {
            m_Version = version;
            m_Platform = platform;
            m_EditorLogPath = logFilePath;
            return CrashAnalyzer::GetReport(version, platform, logFilePath);
        }

        QNetworkReply* GetReportReply(QNetworkRequest reportRequest, std::shared_ptr<QHttpMultiPart> reportMultiPart) override
        {
            m_RequestUrl = reportRequest.url();
            m_RequestMultiPart = reportMultiPart;
            return new NetworkReplyStub(m_Error, QString(m_ReplyContent.c_str()));
        }

        QNetworkReply* GetServiceStatusReply(QNetworkRequest statusRequest) override
        {
            m_RequestUrl = statusRequest.url();
            return new NetworkReplyStub(m_Error, QString(m_ReplyContent.c_str()));
        }
    };
}
}
