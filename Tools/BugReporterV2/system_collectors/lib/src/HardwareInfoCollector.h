#pragma once

#include "reporter/Collector.h"
#include "reporter/ReportDraft.h"
#include "sysinfo/HardwareInfoProvider.h"

#include <memory>

namespace ureport
{
namespace system_collectors
{
    class HardwareInfoCollector : public ureport::Collector
    {
        std::shared_ptr<HardwareInfoProvider> m_Provider;

    public:
        HardwareInfoCollector(std::shared_ptr<HardwareInfoProvider> provider)
            : m_Provider(provider)
        {
        }

        void Collect(ReportDraft& report) const
        {
            report.WriteFact("BasicHardwareType", m_Provider->GetBasicHardwareType());
            report.WriteFact("GraphicsHardwareType", m_Provider->GetGraphicsHardwareType());
        }
    };
}
}
