#include "reporter/Collector.h"
#include "reporter/ReportDraft.h"
#include "shims/logical/IsEmpty.h"

namespace ureport
{
namespace collectors
{
    class LogAttacher : public ureport::Collector
    {
        std::shared_ptr<AttachmentFactory> m_AttachmentFactory;
        std::vector<std::string> m_Stats;

    public:
        LogAttacher(std::shared_ptr<AttachmentFactory> attachmentFactory, std::vector<std::string> stats) : m_AttachmentFactory(attachmentFactory),
            m_Stats(stats)
        {
        }

        void Collect(ReportDraft& report) const override
        {
            std::string data = "";
            for (auto iter = m_Stats.begin(); iter != m_Stats.end(); iter++)
            {
                data.append(report.ReadFact(*iter));
            }
            if (!IsEmpty(data))
            {
                std::shared_ptr<Attachment> att = m_AttachmentFactory->CreateTextAttachment(data, "System Profile");
                att->GetTextPreview()->SetRemovable(false);
                report.Attach(att);
            }
        }
    };
}
}
