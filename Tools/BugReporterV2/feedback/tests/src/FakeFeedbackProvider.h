#pragma once

#include "feedback/Feedback.h"
#include "FeedbackProvider.h"
#include "FeedbackCollection.h"

#include <string>
#include <map>


namespace ureport
{
namespace test
{
    class FakeFeedbackProvider : public FeedbackProvider
    {
    public:
        void GiveFeedback(const Report& report, std::shared_ptr<FeedbackCollection> collection)
        {
            collection->UpdateLocalFeedback(std::map<std::string, Feedback>());
        }
    };
}
}
