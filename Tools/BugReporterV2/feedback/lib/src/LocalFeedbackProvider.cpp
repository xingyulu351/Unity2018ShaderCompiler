#include "LocalFeedbackProvider.h"
#include "attachment/Attachment.h"
#include "attachment_preview/AttachmentTextPreview.h"
#include <regex>


namespace ureport
{
    void LocalFeedbackProvider::GiveFeedback(const Report& report, std::shared_ptr<FeedbackCollection> collection)
    {
        auto values = std::map<std::string, Feedback>();
        values["Summary"] = GetReportSummary(report);
        collection->UpdateLocalFeedback(values);
    }

    Feedback LocalFeedbackProvider::GetReportSummary(const Report& report)
    {
        if (report.GetBugTypeID() == Report::kBugUnknown)
            return Feedback("You must specify what is the problem related to before sending the report.", 3);
        if (report.GetBugReproducibility() == Report::kReproUnknown)
            return Feedback("In order to send the report you need to specify how often does the problem happen.", 3);
        if (report.GetReporterEmail().empty())
            return Feedback("You must provide an email address before sending the report.", 3);
        else if (!IsEmailCorrect(report.GetReporterEmail()))
            return Feedback("In order to send the report you must provide a valid email address.", 3);
        if (report.GetTitle().empty())
            return Feedback("You must provide a title to be able to send the report.", 3);
        if (report.GetTextualDescription().length() > Report::DESCRIPTION_MAX_CHARS)
            return Feedback("The description is too long - shorten it before sending the report.", 3);
        return Feedback("", 0);
    }

    bool LocalFeedbackProvider::IsEmailCorrect(const std::string &email)
    {
        #ifdef LINUX
        // std::regex on build machines isn't good enough
        std::string::size_type atIndex = email.find('@');
        return (atIndex != std::string::npos) && // contains @
            (atIndex > 0) && // and stuff before
            (atIndex < (email.size() - 1)); // and stuff after
        #else
        std::regex base_regex("^[_A-Za-z0-9-\\+]+(\\.[_A-Za-z0-9-]+)*@[A-Za-z0-9-]+(\\.[A-Za-z0-9]+)*(\\.[A-Za-z]{2,})$");
        return std::regex_match(email, base_regex);
        #endif
    }

    std::unique_ptr<FeedbackProvider> CreateLocalFeedbackProvider()
    {
        return std::unique_ptr<FeedbackProvider>(new LocalFeedbackProvider());
    }
}
