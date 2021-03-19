#pragma once

#include "reporter/Collector.h"
#include "reporter/ReportDraft.h"
#include "unity_version/UnityVersion.h"

#include <memory>

namespace ureport
{
namespace unity_collectors
{
    class UnityVersionCollector : public ureport::Collector
    {
        std::shared_ptr<UnityVersion> m_UnityVersion;

    public:
        UnityVersionCollector(std::shared_ptr<UnityVersion> unityVersion)
            : m_UnityVersion(unityVersion)
        {
        }

        void Collect(ReportDraft& report) const
        {
            report.WriteFact("UnityVersion", m_UnityVersion->GetVersion());
        }
    };
}
}
