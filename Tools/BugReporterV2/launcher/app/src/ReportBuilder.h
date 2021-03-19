#pragma once

#include <memory>

namespace ureport
{
    class ReportDraft;
    class Args;
    class AttachmentFactory;
    class FileSystem;

    class ReportBuilder
    {
    public:
        virtual ~ReportBuilder() {}

        virtual std::unique_ptr<ReportDraft> BuildReport(const Args& args) = 0;

    protected:
        ReportBuilder() {}

    private:
        ReportBuilder(const ReportBuilder&) {}

        ReportBuilder& operator=(const ReportBuilder&) { return *this; }
    };

    std::unique_ptr<ReportBuilder> CreateReportBuilder(
        std::shared_ptr<AttachmentFactory> attachmentFactory,
        std::shared_ptr<FileSystem> fileSystem);
}
