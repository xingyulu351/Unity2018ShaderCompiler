#include "MainWindow.h"
#include "ComposeReportControllerDummy.h"
#include "../../reporter/tests/src/ReportFake.h"
#include "../../include/settings/SettingsStub.h"

#include <UnitTest++.h>
#include <QApplication>
#include <memory>

SUITE(ReportMainForm)
{
    using namespace ureport::test;

    class Fixture
    {
    public:
        std::unique_ptr<QApplication> m_App;
        std::unique_ptr<MainWindow> m_Form;

        Fixture()
        {
            int argc = 0;
            char** argv = nullptr;
            m_App.reset(new QApplication(argc, argv));
            m_Form.reset(new MainWindow());
            m_Form->SetReportController(std::make_shared<ComposeReportControllerDummy>());
        }
    };

    TEST_FIXTURE(Fixture, CanSetReportTitle)
    {
        m_Form->SetReportTitle("Title");
        CHECK(m_Form->GetReportTitle() == "Title");
    }

    TEST_FIXTURE(Fixture, CanSetAdditionalInfo)
    {
        m_Form->SetAdditionalInfo("Additional info");
        CHECK(m_Form->GetAdditionalInfo() == "Additional info");
    }

    TEST_FIXTURE(Fixture, AutoCrashProblemComboBoxDisabled)
    {
        std::shared_ptr<BugTypeIDReportFake> report = std::make_shared<BugTypeIDReportFake>();
        report->m_BugTypeID = ureport::Report::kBugCrashAuto;
        m_Form->SetReport(report);
        QComboBox *box = m_Form->findChild<QComboBox *>("problemComboBox");
        CHECK(!box->isEnabled());
    }

    TEST_FIXTURE(Fixture, NormalReportProblemComboBoxEnabled)
    {
        std::shared_ptr<BugTypeIDReportFake> report = std::make_shared<BugTypeIDReportFake>();
        report->m_BugTypeID = ureport::Report::kBugUnknown;
        m_Form->SetReport(report);
        QComboBox *box = m_Form->findChild<QComboBox *>("problemComboBox");
        CHECK(box->isEnabled());
    }

    TEST_FIXTURE(Fixture, SendUrlNotConfiguredSendButtonVisible)
    {
        auto settings = GetSettingsStub();
        settings->m_SendUrl = "";
        m_Form.reset(new MainWindow());
        std::shared_ptr<BugTypeIDReportFake> report = std::make_shared<BugTypeIDReportFake>();
        report->m_BugTypeID = ureport::Report::kBugUnknown;
        m_Form->SetReport(report);
        QPushButton *save = m_Form->findChild<QPushButton *>("saveButton");
        QPushButton *send = m_Form->findChild<QPushButton *>("sendButton");
        CHECK(!(send->isHidden()));
        CHECK(save->isHidden());
    }

    TEST_FIXTURE(Fixture, ConfigWithSendUrlSendButtonVisible)
    {
        auto settings = GetSettingsStub();
        settings->m_SendUrl = "https://send.bug";
        m_Form.reset(new MainWindow());
        std::shared_ptr<BugTypeIDReportFake> report = std::make_shared<BugTypeIDReportFake>();
        report->m_BugTypeID = ureport::Report::kBugUnknown;
        m_Form->SetReport(report);
        QPushButton *save = m_Form->findChild<QPushButton *>("saveButton");
        QPushButton *send = m_Form->findChild<QPushButton *>("sendButton");
        CHECK(!(send->isHidden()));
        CHECK(save->isHidden());
    }

    TEST_FIXTURE(Fixture, ConfigWithSendUrlDisabledSendButtonHidden)
    {
        auto settings = GetSettingsStub();
        settings->m_SendUrl = ureport::constants::kDisabled;
        m_Form.reset(new MainWindow());
        std::shared_ptr<BugTypeIDReportFake> report = std::make_shared<BugTypeIDReportFake>();
        report->m_BugTypeID = ureport::Report::kBugUnknown;
        m_Form->SetReport(report);
        QPushButton *save = m_Form->findChild<QPushButton *>("saveButton");
        QPushButton *send = m_Form->findChild<QPushButton *>("sendButton");
        CHECK(send->isHidden());
        CHECK(!(save->isHidden()));
    }
}
