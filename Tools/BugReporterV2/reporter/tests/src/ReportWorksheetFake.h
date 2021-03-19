#pragma once

#include "../src/ReportWorksheet.h"
#include "../src/ReportDraft.h"
#include "OperationObserverDummy.h"

#include <string>

namespace ureport
{
namespace test
{
    class ReportWorksheetFake : public ureport::ReportWorksheet
    {
    public:
        std::string m_Title;
        std::string m_Description;
        std::string m_Email;
        std::string m_Publicity;
        bool m_Fill;

    public:
        ReportWorksheetFake()
            : m_Fill(true)
        {
        }

        bool FillReport(std::shared_ptr<ReportDraft> report,
            std::function<void(ReportDraft&, LongTermOperObserver*)> prepareReport,
            std::function<void(Report&, LongTermOperObserver*)> sendReport,
            std::function<void(Report&, LongTermOperObserver*, const std::string&)> saveReport)
        {
            report->SetTitle(m_Title);
            report->SetTextualDescription(m_Description);
            report->SetReporterEmail(m_Email);
            report->WriteFact("Publicity", m_Publicity);
            if (m_Fill)
            {
                OperationObserverDummy observer;
                prepareReport(*report, &observer);
                sendReport(*report, &observer);
                saveReport(*report, &observer, "");
            }
            return m_Fill;
        }
    };
}
}
