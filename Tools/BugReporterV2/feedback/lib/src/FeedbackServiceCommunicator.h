#pragma once

#include "feedback/Feedback.h"
#include "FeedbackCommunicator.h"
#include "FeedbackParser.h"
#include "settings/Settings.h"

#include <QtNetwork>
#include <QObject>


namespace ureport
{
    class FeedbackServiceCommunicator : public QObject, public FeedbackCommunicator
    {
        Q_OBJECT
    public:
        FeedbackServiceCommunicator();
        FeedbackServiceCommunicator(std::shared_ptr<Settings> settings);
        void GiveFeedback(QByteArray data, std::function<void(std::map<std::string, Feedback>)> callback, std::function<void()> errorCallback);
        std::string GetFeedbackServerUrl() const;
        ~FeedbackServiceCommunicator();
    public slots:
        void HandleReply(QNetworkReply *networkReply);
    private:
        QNetworkAccessManager m_NetworkManager;
        std::function<void(std::map<std::string, Feedback>)> m_Callback;
        std::function<void()> m_ErrorCallback;
        FeedbackParser m_Parser;
        std::shared_ptr<Settings> m_Settings;
    };
}
