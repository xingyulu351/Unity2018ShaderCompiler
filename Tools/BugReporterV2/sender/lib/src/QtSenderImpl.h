#pragma once

#include "reporter/UnityBugMessage.h"
#include "reporter/Sender.h"
#include "file_system/File.h"
#include "shims/logical/IsNull.h"
#include "shims/attribute/GetCString.h"
#include "settings/Settings.h"
#include "long_term_operation/LongTermOperObserver.h"
#include "NetworkReplyProgressListener.h"
#include "ProxyUtils.h"

#include <QtNetwork>
#include <QObject>
#include <QCoreApplication>

#include <memory>
#include <stdexcept>
#include <limits>

namespace ureport
{
namespace details
{
    class QtSender : public Sender
    {
    public:
        QtSender() : m_proxyIsUsed(false), m_editorBugsSubmitUrl("https://editorbugs.unity3d.com/submit_bug_api.cgi")
        {
            QString proxyVariableName = "http_proxy";
            QString proxyUrl = GetEnvironmentVariableValue(proxyVariableName);
            if (!proxyUrl.isEmpty())
            {
                QNetworkProxy proxy = ProxyUtils::CreateProxyFromUrl(proxyUrl);
                QNetworkProxy::setApplicationProxy(proxy);
                m_proxyIsUsed = true;
            }

            QString editorBugsVariableName = "editorbugs_url";
            QString envEditorBugsUrl = GetEnvironmentVariableValue(editorBugsVariableName);
            if (envEditorBugsUrl.isEmpty())
            {
                auto settings = ureport::GetSettings();
                envEditorBugsUrl = QString::fromStdString(settings->GetSendUrl());
            }
            if (!envEditorBugsUrl.isEmpty())
            {
                m_editorBugsSubmitUrl = envEditorBugsUrl;
            }
        }

        void Send(const UnityBugMessage& message, LongTermOperObserver* observer)
        {
            PostData(message, observer);
        }

    private:
        bool m_proxyIsUsed;
        QString m_editorBugsSubmitUrl;

        QString GetEnvironmentVariableValue(QString variableName)
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
                return "";
            }
            QString variableValue = e.value(variableName);
            return variableValue;
        }

        void PostData(const UnityBugMessage& message, LongTermOperObserver* observer)
        {
            LongTermBlockedOperObserver blockedOperObserver(observer);
            if (blockedOperObserver.IsOperationCanceled())
                return;
            QNetworkAccessManager netManager;
            QUrl requestUrl(m_editorBugsSubmitUrl);
            QNetworkRequest request(requestUrl);
            auto multiPart = ComposePostData(message);
            std::unique_ptr<QNetworkReply> netReply(netManager.post(request, multiPart.get()));
            std::unique_ptr<NetworkReplyProgressListener> listener(
                new NetworkReplyProgressListener(observer, netReply.get()));
            blockedOperObserver.ReportStatus("Uploading the report...");
            while (netReply->isRunning() && !blockedOperObserver.IsOperationCanceled())
            {
                QCoreApplication::processEvents();
            }

            std::string errorText = listener->GetErrorString();
            if (!errorText.empty())
            {
                if (m_proxyIsUsed)
                {
                    errorText = errorText + ". If you are using a proxy server ensure it is configured correctly.";
                }

                throw std::runtime_error(errorText);
            }
        }

        static std::unique_ptr<QHttpMultiPart> ComposePostData(const UnityBugMessage& message)
        {
            std::unique_ptr<QHttpMultiPart> multiPart(
                new QHttpMultiPart(QHttpMultiPart::FormDataType));
            multiPart->append(GetTimestamp(message));
            multiPart->append(GetTitle(message));
            multiPart->append(GetCustomerEmail(message));
            multiPart->append(GetVersion(message));
            multiPart->append(GetComputer(message));
            multiPart->append(GetEvent(message));
            multiPart->append(GetBugTypeID(message));
            multiPart->append(GetBugReproducibility(message));
            multiPart->append(GetBugFiles(multiPart.get(), message));
            return std::move(multiPart);
        }

        static QHttpPart GetTimestamp(const UnityBugMessage& message)
        {
            QHttpPart timestamp;
            timestamp.setHeader(QNetworkRequest::ContentDispositionHeader,
                QVariant("form-data; name=\"timestamp\""));
            timestamp.setBody(GetCString(message.GetTimestamp()));
            return timestamp;
        }

        static QHttpPart GetTitle(const UnityBugMessage& message)
        {
            QHttpPart title;
            title.setHeader(QNetworkRequest::ContentDispositionHeader,
                QVariant("form-data; name=\"sTitle\""));
            title.setBody("user submitted bug");
            return title;
        }

        static QHttpPart GetCustomerEmail(const UnityBugMessage& message)
        {
            QHttpPart email;
            email.setHeader(QNetworkRequest::ContentDispositionHeader,
                QVariant("form-data; name=\"sCustomerEmail\""));
            email.setBody(GetCString(message.GetCustomerEmail()));
            return email;
        }

        static QHttpPart GetVersion(const UnityBugMessage& message)
        {
            QHttpPart version;
            version.setHeader(QNetworkRequest::ContentDispositionHeader,
                QVariant("form-data; name=\"sVersion\""));
            version.setBody(GetCString(message.GetUnityVersion()));
            return version;
        }

        static QHttpPart GetComputer(const UnityBugMessage& message)
        {
            QHttpPart computer;
            computer.setHeader(QNetworkRequest::ContentDispositionHeader,
                QVariant("form-data; name=\"sComputer\""));
            computer.setBody(Encode(message.GetComputer()));
            return computer;
        }

        static QHttpPart GetEvent(const UnityBugMessage& message)
        {
            QHttpPart event;
            event.setHeader(QNetworkRequest::ContentDispositionHeader,
                QVariant("form-data; name=\"sEvent\""));
            event.setBody(Encode(message.GetEvent()));
            return event;
        }

        static QHttpPart GetBugTypeID(const UnityBugMessage& message)
        {
            QHttpPart bugTypeId;
            bugTypeId.setHeader(QNetworkRequest::ContentDispositionHeader,
                QVariant("form-data; name=\"sBugTypeID\""));
            auto id(message.GetBugTypeID());
            auto str(std::to_string(static_cast<long long>(id)));
            bugTypeId.setBody(GetCString(str));
            return bugTypeId;
        }

        static QHttpPart GetBugReproducibility(const UnityBugMessage& message)
        {
            QHttpPart bugRepro;
            bugRepro.setHeader(QNetworkRequest::ContentDispositionHeader,
                QVariant("form-data; name=\"sBugReproducibility\""));
            bugRepro.setBody(GetCString(message.GetBugReproducibility()));
            return bugRepro;
        }

        static QHttpPart GetBugFiles(QHttpMultiPart* multiPart, const UnityBugMessage& message)
        {
            QHttpPart bugFiles;
            bugFiles.setHeader(QNetworkRequest::ContentDispositionHeader,
                QVariant("form-data; name=\"File1\"; filename=\"bugfiles.zip\""));
            bugFiles.setHeader(QNetworkRequest::ContentTypeHeader,
                QVariant("application/octet-stream"));
            auto file = GetAttachedFilePath(message);
            if (!IsNull(file))
            {
                file->open(QIODevice::ReadOnly);
                bugFiles.setBodyDevice(file.get());
                file->setParent(multiPart);
                file.release();
            }
            return bugFiles;
        }

        static std::unique_ptr<QFile> GetAttachedFilePath(const UnityBugMessage& message)
        {
            auto path = message.GetAttachedFilePath();
            if (path.empty())
                return std::unique_ptr<QFile>();
            return std::unique_ptr<QFile>(new QFile(QString::fromStdString(path)));
        }

        static QByteArray Encode(const std::string& text)
        {
            QByteArray data = GetCString(text);
            return data.toPercentEncoding();
        }
    };
}
}
