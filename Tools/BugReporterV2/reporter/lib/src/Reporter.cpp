#include "Reporter.h"
#include "ReportDraft.h"
#include "ReportWorksheet.h"
#include "Sender.h"
#include "Exporter.h"
#include "FilesPacker.h"
#include "Collector.h"
#include "NoWorksheetException.h"
#include "NoSenderException.h"
#include "NoFilesPackerException.h"
#include "file_system/File.h"
#include "shims/logical/IsNull.h"
#include "reporter/UnityBugMessage.h"
#include "settings/Settings.h"
#include "long_term_operation/LongTermOperObserver.h"

#include <memory>
#include <vector>
#include <algorithm>
#include <fstream>

#include <iostream>

namespace ureport
{
namespace details
{
    class Reporter : public ureport::Reporter
    {
        typedef std::vector<std::shared_ptr<Collector> > CollectorsVector;
        typedef std::vector<std::shared_ptr<ReportModifier> > ModifiersVector;

        std::shared_ptr<ReportWorksheet> m_Worksheet;
        std::shared_ptr<Sender> m_Sender;
        std::shared_ptr<Exporter> m_Exporter;
        CollectorsVector m_Collectors;
        ModifiersVector m_Initializers;
        std::shared_ptr<FilesPacker> m_FilesPacker;
        std::shared_ptr<Settings> m_Settings;

        void SetWorksheet(std::shared_ptr<ReportWorksheet> worksheet)
        {
            m_Worksheet = worksheet;
        }

        void SetSender(std::shared_ptr<Sender> sender)
        {
            m_Sender = sender;
        }

        void SetExporter(std::shared_ptr<Exporter> exporter)
        {
            m_Exporter = exporter;
        }

        void SetFilesPacker(std::shared_ptr<FilesPacker> packer)
        {
            m_FilesPacker = packer;
        }

        void SetSettings(std::shared_ptr<Settings> settings)
        {
            m_Settings = settings;
        }

        void AddCollector(std::shared_ptr<Collector> collector)
        {
            m_Collectors.push_back(collector);
        }

        void AddReportInitializer(std::shared_ptr<ReportModifier> initializer)
        {
            m_Initializers.push_back(initializer);
        }

        void DoReport(std::shared_ptr<ReportDraft> report)
        {
            if (IsNull(m_Worksheet))
                throw NoWorksheetException();
            if (IsNull(m_FilesPacker))
                throw NoFilesPackerException();
            ApplyReportInitializers(*report);
            m_Worksheet->FillReport(
                report,
                [this] (ReportDraft& rep, LongTermOperObserver* observer)
                {
                    CallCollectors(rep, observer);
                },
                [this] (Report& rep, LongTermOperObserver* observer)
                {
                    SendReport(rep, observer);
                },
                [this] (Report& rep, LongTermOperObserver* observer, const std::string& path)
                {
                    SaveReport(rep, observer, path);
                });
        }

        void ApplyReportInitializers(ReportDraft& report)
        {
            std::for_each(m_Initializers.begin(), m_Initializers.end(),
                [&report] (const std::shared_ptr<ReportModifier>& modifier) {
                    modifier->Modify(report);
                });
        }

        void SendReport(Report& report, LongTermOperObserver* observer)
        {
            observer->ReportProgress(0, 2);
            auto message = UnityBugMessage::Compose(
                report, *m_FilesPacker, observer);
            observer->ReportProgress(1, 2);
            if (IsNull(m_Sender))
                throw NoSenderException();
            m_Sender->Send(message, observer);
            observer->ReportProgress(2, 2);
            if (!IsNull(m_Settings))
                SaveSettings(report);
        }

        void SaveReport(Report& report, LongTermOperObserver* observer, const std::string& path)
        {
            observer->ReportProgress(0, 2);

            auto message = UnityBugMessage::Compose(
                report, *m_FilesPacker, observer);
            observer->ReportProgress(1, 2);
            m_Exporter->Save(message, observer, path);
            observer->ReportProgress(2, 2);
            if (!IsNull(m_Settings))
                SaveSettings(report);
        }

        void CallCollectors(ReportDraft& report, LongTermOperObserver* observer)
        {
            LongTermBlockedOperObserver helper(observer);
            helper.ReportStatus("Collecting data for the report...");
            helper.ReportProgress(0, m_Collectors.size());
            int collCnt = 0;
            auto previousAttachments = report.GetAttachments();
            for (auto collectorIter = m_Collectors.begin(); collectorIter != m_Collectors.end();
                 ++collectorIter)
            {
                try
                {
                    (*collectorIter)->Collect(report);
                    if (observer->IsOperationCanceled())
                        break;
                }
                catch (...)
                {
                }
                helper.ReportProgress(++collCnt, m_Collectors.size());
            }
            if (observer->IsOperationCanceled())
            {
                ResetAttachments(report, previousAttachments);
            }
        }

        void SaveSettings(const Report& report)
        {
            m_Settings->SetCustomerEmail(report.GetReporterEmail());
        }

        void ResetAttachments(ReportDraft& report, std::list<std::shared_ptr<Attachment> > previous)
        {
            auto current = report.GetAttachments();
            for (auto currentAttachment = current.begin(); currentAttachment != current.end(); currentAttachment++)
            {
                bool newFile = true;
                for (auto old = previous.begin(); old != previous.end(); old++)
                {
                    if (currentAttachment->get() == old->get())
                    {
                        newFile = false;
                        break;
                    }
                }
                if (newFile)
                    report.Detach(*currentAttachment);
            }
        }
    };
}

    std::unique_ptr<Reporter> CreateNewReporter()
    {
        return std::unique_ptr<Reporter>(new details::Reporter());
    }
}
