#pragma once

#include "FeedbackCollection.h"
#include "reporter/Report.h"

#include <functional>
#include <map>
#include <memory>

namespace ureport
{
    class FeedbackProvider
    {
    public:
        virtual void GiveFeedback(const Report& report, std::shared_ptr<FeedbackCollection> collection) = 0;
    };
}
