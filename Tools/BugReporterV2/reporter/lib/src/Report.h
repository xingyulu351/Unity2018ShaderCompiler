#pragma once

#include <memory>
#include <string>
#include <vector>
#include <list>

namespace ureport
{
    typedef std::string FactName;
    typedef std::string Fact;

    class File;
    class Attachment;
    class ReportMonitor;

    class Report
    {
    public:
        static const int DESCRIPTION_MAX_CHARS = 5000;
        typedef const std::shared_ptr<File> FileRef;
        typedef std::vector<FileRef> FileRefSet;
        enum BugTypeID
        {
            kBugUnknown = -1,
            kBugProblemWithEditor = 0,
            kBugProblemWithPlayer = 1,
            kBugFeatureRequest = 2,
            kBugDocumentation = 3,
            kBugCrash = 4,
            kBugCrashAuto = 6,
            kBugServices = 11
        };

        enum BugReproducibility
        {
            kReproUnknown = -1,
            kReproFirstTime = 0,
            kReproSometimes = 1,
            kReproAlways = 2,
            kReproMinimumValue = kReproFirstTime,
            kReproMaximumValue = kReproAlways
        };

        virtual std::string GetTitle() const = 0;

        virtual std::string GetTextualDescription() const = 0;

        virtual std::string GetReporterEmail() const = 0;

        virtual BugTypeID GetBugTypeID() const = 0;

        virtual BugReproducibility GetBugReproducibility() const = 0;

        virtual std::string ReadFact(const FactName& name) const = 0;

        virtual std::list<std::shared_ptr<Attachment> > GetAttachments() const = 0;

        virtual void AddMonitor(std::shared_ptr<ReportMonitor> monitor) = 0;

        virtual ~Report() {}
    };
}
