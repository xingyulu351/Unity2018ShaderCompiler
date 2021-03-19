#pragma once

#include "ReportActionThread.h"

namespace ureport
{
    class SaveReportThread : public ReportActionThread
    {
        void DoAction(ComposeReportController& controller) override;

    public:
        void SetFileName(const std::string& fileName);
        std::string GetFileName() const;

    private:
        std::string m_fileName;
    };
}
