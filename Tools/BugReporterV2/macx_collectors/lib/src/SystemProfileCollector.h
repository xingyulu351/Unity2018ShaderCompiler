#pragma once

#include "SystemProfile.h"
#include "reporter/Collector.h"
#include "reporter/ReportDraft.h"
#include "attachment/Attachment.h"
#include "attachment_preview/AttachmentTextPreview.h"
#include "shims/logical/IsEmpty.h"
#include "shims/attribute/GetSize.h"

#include <memory>
#include <algorithm>
#include <iterator>

namespace ureport
{
namespace collectors
{
    class SystemProfileCollector : public ureport::Collector
    {
        std::shared_ptr<macosx::SystemProfile> m_Profile;
        std::shared_ptr<AttachmentFactory> m_AttachmentFactory;
        std::string m_Name;

    public:
        SystemProfileCollector(std::shared_ptr<macosx::SystemProfile> profile,
                               std::string name)
            : m_Profile(profile),
            m_Name(name)
        {
        }

        void Collect(ReportDraft& report) const override
        {
            std::vector<std::string> types;
            types.push_back("SP" + m_Name + "DataType");
            auto const data = m_Profile->GetData(types);
            if (!IsEmpty(data))
                report.WriteFact(m_Name, data);
        }
    };
}
}
