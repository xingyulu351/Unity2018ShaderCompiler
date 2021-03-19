#pragma once

#include "long_term_operation/LongTermOperObserver.h"

namespace ureport
{
namespace test
{
    class OperationObserverDummy : public ureport::LongTermOperObserver
    {
        void ReportProgress(size_t current, size_t total) override
        {
        }

        void ReportStatus(const std::string& status) override
        {
        }

        bool IsOperationCanceled() const override
        {
            return false;
        }
    };
}
}
