#pragma once

#include <QObject>
#include "FeedbackProvider.h"
#include "FeedbackCollection.h"
#include "feedback/FeedbackManager.h"


namespace ureport
{
    class BasicFeedbackManager : public QObject, public FeedbackManager
    {
        Q_OBJECT
    public:
        BasicFeedbackManager(std::unique_ptr<FeedbackProvider> local, std::unique_ptr<FeedbackProvider> web);
        void GiveFeedback(const Report& report, std::function<void(std::map<std::string, Feedback>)> callback);
        bool ShouldSendBeBlocked();
        bool ShouldBugReporterBeDisabled();
    private:
        std::unique_ptr<FeedbackProvider> m_LocalFeedbackProvider;
        std::unique_ptr<FeedbackProvider> m_WebFeedbackProvider;
        std::shared_ptr<FeedbackCollection> m_Collection;
        std::function<void(std::map<std::string, Feedback>)> m_Callback;
    private slots:
        void FeedbackReceived();
    };

    std::unique_ptr<FeedbackManager> CreateFeedbackManager();
}
