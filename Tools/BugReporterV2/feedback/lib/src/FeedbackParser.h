#pragma once

#include "feedback/Feedback.h"
#include <QJsonDocument>
#include <QJsonObject>
#include <map>


namespace ureport
{
    class FeedbackParser
    {
    public:
        std::map<std::string, Feedback> FromJson(QByteArray reply)
        {
            std::map<std::string, Feedback> feedbackCollection;
            QJsonDocument doc = QJsonDocument::fromJson(reply);
            QJsonObject root = doc.object();
            for (auto it = root.begin(); it != root.end(); it++)
            {
                std::string key = it.key().toStdString();
                auto object = it.value().toObject();
                feedbackCollection[key] = Feedback(object["Contents"].toString().toStdString(), object["Severity"].toInt());
            }
            return feedbackCollection;
        }
    };
}
