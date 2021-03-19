#pragma once

#include "feedback/Feedback.h"

#include <functional>
#include <map>
#include <string>
#include <QByteArray>

namespace ureport
{
    class FeedbackCommunicator
    {
    public:
        virtual void GiveFeedback(QByteArray data, std::function<void(std::map<std::string, Feedback>)> callback, std::function<void()> errorCallback) = 0;
        virtual ~FeedbackCommunicator() {}
    };
}
