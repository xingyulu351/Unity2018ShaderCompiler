#pragma once

#include "FeedbackProvider.h"
#include "FeedbackCommunicator.h"
#include <string>

namespace ureport
{
    class WebFeedbackProvider : public FeedbackProvider
    {
    public:
        WebFeedbackProvider(FeedbackCommunicator* c);
        void GiveFeedback(const Report& report, std::shared_ptr<FeedbackCollection> collection);
    private:
        std::unique_ptr<FeedbackCommunicator> communicator;
    };
    std::unique_ptr<FeedbackProvider> CreateWebFeedbackProvider();
}
