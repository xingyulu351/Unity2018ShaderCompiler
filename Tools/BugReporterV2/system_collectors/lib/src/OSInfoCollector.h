#pragma once

#include "reporter/Collector.h"
#include "reporter/ReportDraft.h"
#include "sysinfo/SystemInfoProvider.h"

#include <memory>

namespace ureport
{
    class SystemInfoProvider;

namespace system_collectors
{
    class OSInfoCollector : public ureport::Collector
    {
        std::shared_ptr<SystemInfoProvider> m_Provider;

    public:
        OSInfoCollector(std::shared_ptr<SystemInfoProvider> provider)
            : m_Provider(provider)
        {
        }

        void Collect(ReportDraft& report) const
        {
            report.WriteFact("OSName", m_Provider->GetOSName());
            report.WriteFact("OSVersion", m_Provider->GetOSVersion().GetTextualForm());
        }
    };
}
}
