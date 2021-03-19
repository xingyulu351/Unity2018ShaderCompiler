#pragma once

#include "reporter/ComposeReportController.h"
#include "attachment/Attachment.h"
#include "attachment_preview/AttachmentTextPreview.h"
#include "long_term_operation/LongTermOperObserver.h"
#include "Sleep.h"

#include <iostream>
#include <exception>
#include <sstream>

namespace ureport
{
namespace test
{
    class HumbleComposeReportController : public ureport::ComposeReportController
    {
        void SetReport(std::shared_ptr<ReportDraft> report) {}

        void SetTitle(const std::string& title)
        {
            std::cout << "SetTitle: " << title << std::endl;
        }

        void SetTextualDescription(const std::string& description)
        {
            std::cout << "SetTextualDescription: " << description << std::endl;
        }

        void SetReporterEmail(const std::string& email)
        {
            std::cout << "SetReporterEmail: " << email << std::endl;
        }

        void SetBugTypeID(const Report::BugTypeID id)
        {
            std::cout << "SetBugTypeID: " << id << std::endl;
        }

        void SetBugReproducibility(const Report::BugReproducibility repro)
        {
            std::cout << "SetBugReproducibility: " << repro << std::endl;
        }

        void AttachFile(const std::string& path)
        {
            std::cout << "AttachFile: " << path << std::endl;
        }

        void AttachDirectory(const std::string& path)
        {
            std::cout << "AttachDirectory: " << path << std::endl;
        }

        void RemoveAttachment(std::shared_ptr<Attachment> attachment)
        {
            std::cout << "RemoveAttachment: " <<
            attachment->GetTextPreview()->GetName() << std::endl;
        }

        void PrepareReport(LongTermOperObserver* observer)
        {
            std::cout << "PrepareReport: " << std::endl;
            SimulateLongOperation("Prepare simulation", 10, 100, *observer);
        }

        void SendReport(LongTermOperObserver* observer)
        {
            std::cout << "SendReport: " << std::endl;
            SimulateCombinedLongOperation("Send simulation", 10, 100, *observer);
        }

        void SaveReport(LongTermOperObserver* observer, const std::string& path)
        {
            std::cout << "SaveReport: " << std::endl;
            SimulateCombinedLongOperation("Save simulation (" + path + ")", 10, 100, *observer);
        }

        void WriteFact(const FactName& name, const Fact& fact)
        {
            std::cout << "WriteFact: " << name  << " = " << fact << std::endl;
        }

        void SimulateLongOperation(const std::string& name, size_t count, int delay,
            LongTermOperObserver& observer)
        {
            SimulateLongOperation(name, count, delay, observer, [&delay] () {
                Sleep(delay);
            });
        }

        void SimulateCombinedLongOperation(const std::string& name, size_t count, int delay,
            LongTermOperObserver& observer)
        {
            size_t iteration = 0;
            SimulateLongOperation(name, count, delay, observer, [&delay, &observer, &name, &iteration] () {
                const size_t  subCount = 10;
                std::stringstream status;
                status << name << " " << ++iteration;
                observer.ReportStatus(status.str());
                observer.ReportProgress(0, subCount);
                for (size_t step = 0; step < subCount && !observer.IsOperationCanceled(); ++step)
                {
                    Sleep(delay);
                    observer.ReportProgress(step + 1, subCount);
                }
            });
        }

        void SimulateLongOperationThatThrowsException(const std::string& name, size_t count,
            int delay, LongTermOperObserver& observer)
        {
            int counter = 0;
            SimulateLongOperation(name, count, delay, observer,
                [&delay, &count, &counter] () {
                    Sleep(delay);
                    if (++counter == count)
                        throw std::runtime_error("Exception simulation");
                });
        }

        void SimulateLongOperation(const std::string& name, size_t count, int delay,
            LongTermOperObserver& observer, std::function<void()> operation)
        {
            observer.ReportStatus(name);
            observer.ReportProgress(0, count);
            for (size_t step = 0; step < count && !observer.IsOperationCanceled(); ++step)
            {
                operation();
                observer.ReportProgress(step + 1, count);
            }
        }
    };
}
}
