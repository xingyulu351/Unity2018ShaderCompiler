#include "ReportModifier.h"

namespace ureport
{
    class SimpleReportModifier : public ReportModifier
    {
    public:
        SimpleReportModifier(std::function<void(ReportDraft&)> action)
            : m_Action(action)
        {
        }

        void Modify(ReportDraft& report) const
        {
            m_Action(report);
        }

    private:
        std::function<void(ReportDraft&)> m_Action;
    };

    std::shared_ptr<ReportModifier> MakeReportModifier(
        std::function<void(ReportDraft&)> action)
    {
        return std::make_shared<SimpleReportModifier>(action);
    }
}
