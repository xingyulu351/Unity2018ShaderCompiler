#include "FeedbackServiceCommunicator.h"
#include "settings/Settings.h"
#include "Runtime/Utilities/RapidJSONConfig.h"
#include "External/RapidJSON/document.h"
#include <QUrl>

namespace ureport
{
    FeedbackServiceCommunicator::FeedbackServiceCommunicator() : m_Settings(GetSettings())
    {
        QObject::connect(&m_NetworkManager, SIGNAL(finished(QNetworkReply*)),
            this, SLOT(HandleReply(QNetworkReply*)));
    }

    FeedbackServiceCommunicator::FeedbackServiceCommunicator(std::shared_ptr<Settings> settings) : m_Settings(settings)
    {
        QObject::connect(&m_NetworkManager, SIGNAL(finished(QNetworkReply*)),
            this, SLOT(HandleReply(QNetworkReply*)));
    }

    void FeedbackServiceCommunicator::GiveFeedback(QByteArray data, std::function<void(std::map<std::string, Feedback>)> callback, std::function<void()> errorCallback)
    {
        m_Callback = callback;
        m_ErrorCallback = errorCallback;
        QNetworkRequest request(QString::fromStdString(GetFeedbackServerUrl()));
        request.setHeader(QNetworkRequest::ContentTypeHeader, QString("application/json"));
        m_NetworkManager.post(request, data);
    }

    void FeedbackServiceCommunicator::HandleReply(QNetworkReply *networkReply)
    {
        if (networkReply->error() || networkReply->size() == 0)
        {
            m_ErrorCallback();
        }
        else
        {
            m_Callback(m_Parser.FromJson(networkReply->readAll()));
        }
        networkReply->deleteLater();
    }

    std::string FeedbackServiceCommunicator::GetFeedbackServerUrl() const
    {
        QUrl feedbackServerUrl;
        feedbackServerUrl.setScheme(QString("https"));
        feedbackServerUrl.setHost(QString::fromStdString(m_Settings->GetServerUrl()));
        feedbackServerUrl.setPath(QString("/api/v1.0/bugreporter/feedback"));
        return feedbackServerUrl.toString().toStdString();
    }

    FeedbackServiceCommunicator::~FeedbackServiceCommunicator()
    {
        deleteLater();
    }
}
