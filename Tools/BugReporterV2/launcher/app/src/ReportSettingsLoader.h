#pragma once

#include "reporter/ReportModifier.h"
#include "reporter/ReportDraft.h"
#include "settings/Settings.h"

namespace ureport
{
namespace launcher
{
    class ReportSettingsLoader : public ReportModifier
    {
    public:
        ReportSettingsLoader(std::shared_ptr<Settings> settings)
            : m_Settings(settings)
        {
        }

        void Modify(ReportDraft& report) const
        {
            report.SetReporterEmail(m_Settings->GetCustomerEmail());
        }

    private:
        std::shared_ptr<Settings> m_Settings;
    };
}
}
