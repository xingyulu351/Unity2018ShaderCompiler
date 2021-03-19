#pragma once

#include "feedback/Feedback.h"
#include <string>
#include <memory>
#include <QObject>


namespace ureport
{
    class FeedbackCollection : public QObject
    {
        Q_OBJECT

    public:
        FeedbackCollection();
        bool HasCriticalIssue();
        bool PreventReporting();
        void UpdateLocalFeedback(std::map<std::string, Feedback> input);
        void UpdateRemoteFeedback(std::map<std::string, Feedback> input);
        void ResetRemoteFeedback();
        std::map<std::string, Feedback> AllFeedback;
    private:
        void AddToCollection(std::map<std::string, Feedback> *collection, std::map<std::string, Feedback> data);
        bool Contains(std::map<std::string, Feedback> collection, std::string key);
        std::map<std::string, Feedback> m_LocalFeedback;
        std::map<std::string, Feedback> m_RemoteFeedback;
        void MergeFeedback();
        bool m_shouldReportBePrevented;
    signals:
        void FeedbackUpdated();
    };

    std::unique_ptr<FeedbackCollection> CreateFeedbackCollection();
}
