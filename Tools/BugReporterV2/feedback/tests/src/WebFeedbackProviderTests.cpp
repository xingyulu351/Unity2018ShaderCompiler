#include <UnitTest++.h>

#include "shims/logical/IsNull.h"
#include "WebFeedbackProvider.h"
#include "FakeFeedbackCommunicator.h"
#include "FeedbackCollection.h"
#include "ReportFake.h"
#include "AttachmentDummy.h"
#include <QJsonArray>

SUITE(WebFeedbackProvider)
{
    using namespace ureport;
    using namespace test;
    struct Feedbackers
    {
    public:
        Feedbackers()
        {
            m_Communicator = new FakeFeedbackCommunicator();
            m_Provider = std::unique_ptr<WebFeedbackProvider>(new WebFeedbackProvider(m_Communicator));
        }

        void GetFeedback(ReportDummy& fake)
        {
            std::shared_ptr<FeedbackCollection> coll;
            m_Provider->GiveFeedback(fake, coll);
        }

        FakeFeedbackCommunicator* m_Communicator;
        std::unique_ptr<WebFeedbackProvider> m_Provider;
    };

    TEST_FIXTURE(Feedbackers, TitleSent)
    {
        TitleReportFake reportFake;
        std::string title = "Unity has a bug";
        reportFake.m_Title = title;
        GetFeedback(reportFake);
        CHECK_EQUAL(title, m_Communicator->m_Doc.object()["title"].toString().toStdString());
    }

    TEST_FIXTURE(Feedbackers, DescriptionSent)
    {
        DescriptionReportFake reportFake;
        std::string description = "The bug causes a debuff that adds more bugs to the system.\nThis is absolutely horrible.";
        reportFake.m_Description = description;
        GetFeedback(reportFake);
        CHECK_EQUAL(description, m_Communicator->m_Doc.object()["description"].toString().toStdString());
    }

    TEST_FIXTURE(Feedbackers, AttachmentsSent)
    {
        FilesReportFake reportFake;
        std::shared_ptr<Attachment> attachment(new AttachmentDummy());
        std::list<std::shared_ptr<Attachment> > attachments;
        attachments.push_back(attachment);
        reportFake.m_Attachments = attachments;
        GetFeedback(reportFake);
        CHECK_EQUAL("fileName", m_Communicator->m_Doc.object()["attachments"].toArray()[0].toString().toStdString());
    }

    TEST_FIXTURE(Feedbackers, FolderAttachmentZipAppended)
    {
        FilesReportFake reportFake;
        AttachmentDummy* attachment = new AttachmentDummy();
        attachment->fileType = "folder";
        std::list<std::shared_ptr<Attachment> > attachments;
        attachments.push_back(std::shared_ptr<Attachment>(attachment));
        reportFake.m_Attachments = attachments;
        GetFeedback(reportFake);
        CHECK_EQUAL("fileName.zip", m_Communicator->m_Doc.object()["attachments"].toArray()[0].toString().toStdString());
    }

    TEST_FIXTURE(Feedbackers, EmailSent)
    {
        EmailReportFake reportFake;
        std::string email = "someone@unity3d.com";
        reportFake.m_Email = email;
        GetFeedback(reportFake);
        CHECK_EQUAL(email, m_Communicator->m_Doc.object()["email"].toString().toStdString());
    }
}
