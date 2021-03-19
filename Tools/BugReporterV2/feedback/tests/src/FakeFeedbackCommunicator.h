#pragma once

#include "feedback/Feedback.h"
#include "FeedbackCommunicator.h"

#include <string>
#include <functional>
#include <QJsonDocument>
#include <QJsonObject>

namespace ureport
{
    class FakeFeedbackCommunicator : public FeedbackCommunicator
    {
    public:
        FakeFeedbackCommunicator() {}

        void GiveFeedback(QByteArray input, std::function<void(std::map<std::string, Feedback>)> callback, std::function<void()> errorCallback)
        {
            m_Input = input;
            m_Callback = callback;
            m_Doc = QJsonDocument::fromJson(input);
        }

        std::string GetFeedbackServerUrl() const {return ""; }

        ~FakeFeedbackCommunicator() {}

        QByteArray m_Input;
        QJsonDocument m_Doc;
        std::function<void(std::map<std::string, Feedback>)> m_Callback;
    };
}
