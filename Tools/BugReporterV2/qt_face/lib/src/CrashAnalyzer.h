#pragma once

#include "reporter/ReportDraft.h"
#include "attachment/Attachment.h"
#include "attachment/AttachmentProperties.h"
#include <QJsonObject>
#include <QJsonValue>
#include <QJsonDocument>
#include <QHttpMultiPart>
#include <QHttpPart>
#include <QFileInfo>
#include <QNetworkReply>
#include <QEventLoop>
#include <QCoreApplication>
#include <QProcessEnvironment>
#include <QUrlQuery>

#include <QDebug>

#define DEBUG_CRAN_URL              "DEBUG_CRAN_URL"
#define DEBUG_CRAN_UNITY_VERSION    "DEBUG_CRAN_UNITY_VERSION"
#define DEBUG_CRAN_PLATFORM         "DEBUG_CRAN_PLATFORM"
#define DEBUG_CRAN_STATUS_URL       "DEBUG_CRAN_STATUS_URL"


inline QString GetEnvVariable(QString variableName, QString defaultValue = "")
{
    QProcessEnvironment e = QProcessEnvironment::systemEnvironment();
    bool variableExists = e.contains(variableName);
    if (!variableExists)
    {
        variableName = variableName.toUpper();
        variableExists = e.contains(variableName);
    }

    if (!variableExists)
    {
        return defaultValue;
    }
    QString variableValue = e.value(variableName);
    return variableValue;
}

namespace ureport
{
namespace qt_face
{
    enum CrashServiceStatus { NotAvailable = 0, Available };

    class CrashAnalyzer : public QObject
    {
    public:
        CrashAnalyzer(std::string serverUrl, std::shared_ptr<ReportDraft> report)
            : m_ServerUrl(serverUrl), m_Report(report)
        {
            m_NetworkAccessManager = std::unique_ptr<QNetworkAccessManager>(new QNetworkAccessManager(this));
        }

        CrashServiceStatus GetServiceStatus()
        {
            auto serverUrl = GetServiceStatusServer();
            QUrlQuery statusQuery;
            statusQuery.addQueryItem(QString("platform"), QString::fromStdString(GetPlatform()).trimmed());
            statusQuery.addQueryItem(QString("version"), QString::fromStdString(GetUnityVersion()).trimmed());
            QUrl statusRequestUrl(QString::fromStdString(serverUrl));
            statusRequestUrl.setPath("/crashes/api/v1.0/status");
            statusRequestUrl.setQuery(statusQuery.query(QUrl::FullyEncoded));
            QNetworkRequest statusRequest(statusRequestUrl);
            auto networkReply = GetServiceStatusReply(statusRequest);
            if (networkReply->error() != QNetworkReply::NoError)
            {
                return CrashServiceStatus::NotAvailable;
            }
            QString replyContent(networkReply->readAll());
            QJsonDocument jsonResponse = QJsonDocument::fromJson(replyContent.toUtf8());
            networkReply->deleteLater();
            auto collectionEnabled = jsonResponse.object().toVariantMap()["enabled"].toBool();
            return collectionEnabled ? CrashServiceStatus::Available : CrashServiceStatus::NotAvailable;
        }

        QJsonObject GetReport()
        {
            if (!IsReportValid())
            {
                return QJsonObject();
            }

            return GetReport(GetUnityVersion(), GetPlatform(), GetLogFilePath());
        }

        static QHttpPart CreateVersionPart(std::string version)
        {
            QHttpPart versionPart;
            versionPart.setHeader(QNetworkRequest::ContentDispositionHeader, QVariant("form-data; name=\"version\""));
            versionPart.setBody(QByteArray(version.c_str()));
            return versionPart;
        }

        static QHttpPart CreatePlatformPart(std::string platform)
        {
            QHttpPart platformPart;
            platformPart.setHeader(QNetworkRequest::ContentDispositionHeader, QVariant("form-data; name=\"platform\""));
            platformPart.setBody(QByteArray(platform.c_str()));
            return platformPart;
        }

        static QHttpPart CreateLogFilePart(std::shared_ptr<QHttpMultiPart> parentMultiPart, std::string logFilePath)
        {
            QFileInfo logFileInfo(QByteArray(logFilePath.c_str()));
            QHttpPart logFilePart;
            logFilePart.setHeader(QNetworkRequest::ContentTypeHeader, QVariant("text/plain"));
            logFilePart.setHeader(QNetworkRequest::ContentDispositionHeader, QVariant(QString("form-data; name=\"log_file\"; filename=\"%1\"").arg(logFileInfo.fileName())));
            QFile *file = new QFile(logFileInfo.filePath());
            file->open(QIODevice::ReadOnly);
            logFilePart.setBodyDevice(file);
            file->setParent(parentMultiPart.get());
            return logFilePart;
        }

        static std::shared_ptr<QHttpMultiPart> CreateMultiPart(std::string version, std::string platform, std::string logFilePath)
        {
            auto multiPart = std::make_shared<QHttpMultiPart>(QHttpMultiPart::FormDataType);
            QHttpPart versionPart = CreateVersionPart(version);
            QHttpPart platformPart = CreatePlatformPart(platform);
            QHttpPart logFilePart = CreateLogFilePart(multiPart, logFilePath);
            multiPart->append(versionPart);
            multiPart->append(platformPart);
            multiPart->append(logFilePart);
            return multiPart;
        }

        std::string GetServerUrl() const
        {
            return GetEnvVariable(DEBUG_CRAN_URL,
                QString::fromStdString(m_ServerUrl)).toStdString();
        }

    protected:
        virtual QJsonObject GetReport(std::string version, std::string platform, std::string logFilePath)
        {
            auto serverUrl = GetServerUrl();
            QUrl reportsRequestUrl(QByteArray(serverUrl.c_str()));
            reportsRequestUrl.setPath("/reports/");
            QNetworkRequest reportRequest(reportsRequestUrl);
            auto requestMultiPart = CreateMultiPart(version, platform, logFilePath);
            auto networkReply = GetReportReply(reportRequest, requestMultiPart);
            if (networkReply->error() != QNetworkReply::NoError)
            {
                QJsonObject errorResponse;
                errorResponse.insert("error_code", networkReply->error());
                errorResponse.insert("error_text", networkReply->errorString());
                return errorResponse;
            }
            QString replyContent(networkReply->readAll());
            QJsonDocument jsonResponse = QJsonDocument::fromJson(replyContent.toUtf8());
            networkReply->deleteLater();
            return jsonResponse.object();
        }

        virtual QNetworkReply* GetReportReply(QNetworkRequest reportRequest, std::shared_ptr<QHttpMultiPart> reportMultiPart)
        {
            QNetworkReply* reply = m_NetworkAccessManager->post(reportRequest, reportMultiPart.get());
            reportMultiPart->setParent(reply);
            QEventLoop eventLoop;
            QObject::connect(reply, SIGNAL(finished()), &eventLoop, SLOT(quit()));
            eventLoop.exec();
            return reply;
        }

        virtual QNetworkReply* GetServiceStatusReply(QNetworkRequest statusRequest)
        {
            QNetworkReply* reply = m_NetworkAccessManager->get(statusRequest);
            QEventLoop eventLoop;
            QObject::connect(reply, SIGNAL(finished()), &eventLoop, SLOT(quit()));
            eventLoop.exec();
            return reply;
        }

    private:
        std::string m_ServerUrl;
        std::shared_ptr<ReportDraft> m_Report;
        std::unique_ptr<QNetworkAccessManager> m_NetworkAccessManager;

        static std::string GetLogFilePath(std::list<std::shared_ptr<Attachment> > attachments)
        {
            const std::string editorLogFileName = "Editor.log";
            std::string logFilePath("");
            auto logFileNameLength = editorLogFileName.size();
            for (auto it = attachments.begin(); it != attachments.end(); ++it)
            {
                auto file = *it;
                if (file)
                {
                    std::string path = file->GetProperty(attachment::kName);
                    auto fileName = path.substr(std::max(logFileNameLength, path.size()) - logFileNameLength);
                    if (fileName == editorLogFileName)
                    {
                        logFilePath = path;
                        break;
                    }
                }
            }
            return logFilePath;
        }

        bool IsReportValid() const
        {
            if (!m_Report)
            {
                return false;
            }
            if (m_Report->GetBugTypeID() != ureport::Report::kBugCrashAuto)
            {
                return false;
            }

            auto attachments = m_Report->GetAttachments();
            if (attachments.size() == 0)
            {
                return false;
            }

            auto logFilePath = GetLogFilePath(attachments);
            if (logFilePath.empty())
            {
                return false;
            }

            return true;
        }

        std::string GetUnityVersion() const
        {
            return GetEnvVariable(DEBUG_CRAN_UNITY_VERSION,
                QString::fromStdString(m_Report->ReadFact("UnityVersion"))).toStdString();
        }

        std::string GetPlatform() const
        {
            return GetEnvVariable(DEBUG_CRAN_PLATFORM,
                QString::fromStdString(m_Report->ReadFact("OSName"))).toStdString();
        }

        std::string GetServiceStatusServer() const
        {
            return GetEnvVariable(DEBUG_CRAN_STATUS_URL,
                QString::fromStdString("https://bugservices.unity3d.com")).toStdString();
        }

        std::string GetLogFilePath() const
        {
            auto attachments = m_Report->GetAttachments();
            return GetLogFilePath(attachments);
        }
    };
}
}
