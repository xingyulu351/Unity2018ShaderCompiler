#include "WebFeedbackProvider.h"
#include "FeedbackServiceCommunicator.h"
#include "attachment/Attachment.h"
#include "attachment/AttachmentProperties.h"
#include "attachment_preview/AttachmentTextPreview.h"
#include "settings/Settings.h"


namespace ureport
{
    WebFeedbackProvider::WebFeedbackProvider(FeedbackCommunicator* c) : communicator(c) {}

    void WebFeedbackProvider::GiveFeedback(const Report& report, std::shared_ptr<FeedbackCollection> collection)
    {
        QJsonDocument doc;
        QJsonObject obj;
        obj.insert("title", QJsonValue(QString::fromStdString(report.GetTitle())));
        obj.insert("description", QJsonValue(QString::fromStdString(report.GetTextualDescription())));
        obj.insert("email", QJsonValue(QString::fromStdString(report.GetReporterEmail())));
        obj.insert("version", QJsonValue(QString::fromStdString(report.ReadFact("UnityVersion"))));
        obj.insert("reportType", QJsonValue(report.GetBugTypeID()));
        obj.insert("repro", QJsonValue(report.GetBugReproducibility()));
        QJsonArray attArray;
        auto attachments = report.GetAttachments();
        for (auto it = attachments.begin(); it != attachments.end(); it++)
        {
            if (it->get()->GetTextPreview()->IsRemovable())
            {
                QString name = QString::fromStdString(it->get()->GetTextPreview()->GetName());
                if (it->get()->GetProperty(attachment::kType) == "folder")
                    name.append(".zip");
                attArray.push_back(QJsonValue(name));
            }
        }
        obj.insert("attachments", attArray);
        doc.setObject(obj);
        communicator->GiveFeedback(doc.toJson(),
            [collection](std::map<std::string, Feedback> f) {collection->UpdateRemoteFeedback(f); },
            [collection]() {collection->ResetRemoteFeedback(); }
        );
    }

    std::unique_ptr<FeedbackProvider> CreateWebFeedbackProvider()
    {
        std::shared_ptr<Settings> settings = ureport::GetSettings();
        FeedbackCommunicator* communicator = new FeedbackServiceCommunicator(settings);
        return std::unique_ptr<FeedbackProvider>(new WebFeedbackProvider(communicator));
    }
}
