#pragma once

#include "FeedbackProvider.h"
#include <string>

namespace ureport
{
    class LocalFeedbackProvider : public FeedbackProvider
    {
    public:
        void GiveFeedback(const Report& report, std::shared_ptr<FeedbackCollection> collection);
    private:
        bool IsEmailCorrect(const std::string &email);
        Feedback GetReportSummary(const Report& report);
    };
    std::unique_ptr<FeedbackProvider> CreateLocalFeedbackProvider();
}
