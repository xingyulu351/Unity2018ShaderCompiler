#include "BasicFeedbackManager.h"
#include "LocalFeedbackProvider.h"
#include "WebFeedbackProvider.h"

namespace ureport
{
    BasicFeedbackManager::BasicFeedbackManager(std::unique_ptr<FeedbackProvider> local, std::unique_ptr<FeedbackProvider> web)
    {
        m_LocalFeedbackProvider = std::move(local);
        m_WebFeedbackProvider = std::move(web);
        m_Collection = CreateFeedbackCollection();
        QObject::connect(m_Collection.get(), SIGNAL(FeedbackUpdated()), SLOT(FeedbackReceived()));
    }

    void BasicFeedbackManager::GiveFeedback(const Report& report, std::function<void(std::map<std::string, Feedback>)> callback)
    {
        m_Callback = callback;
        m_LocalFeedbackProvider->GiveFeedback(report, m_Collection);
        m_WebFeedbackProvider->GiveFeedback(report, m_Collection);
    }

    void BasicFeedbackManager::FeedbackReceived()
    {
        m_Callback(m_Collection->AllFeedback);
    }

    bool BasicFeedbackManager::ShouldSendBeBlocked()
    {
        return m_Collection->HasCriticalIssue();
    }

    bool BasicFeedbackManager::ShouldBugReporterBeDisabled()
    {
        return m_Collection->PreventReporting();
    }

    std::unique_ptr<FeedbackManager> CreateFeedbackManager()
    {
        auto manager = new BasicFeedbackManager(CreateLocalFeedbackProvider(), CreateWebFeedbackProvider());
        return std::unique_ptr<FeedbackManager>(manager);
    }
}
