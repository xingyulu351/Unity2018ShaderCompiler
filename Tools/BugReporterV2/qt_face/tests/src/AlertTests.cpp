#include <UnitTest++.h>

#include "alert.h"
#include <QApplication>
#include <QString>

SUITE(Alert)
{
    using namespace ureport;
    using namespace ui;
    struct Fixture
    {
    public:
        Fixture()
        {
            int argc = 0;
            char** argv = nullptr;
            m_App = new QApplication(argc, argv);
            m_HiddenParent = new QWidget();
            m_HiddenParent->hide();
            m_MiscLabel = new QLabel(m_HiddenParent);
            m_RatingLabel = new QLabel(m_HiddenParent);
            m_Form = new QFormLayout(m_HiddenParent);
            m_Alert = new Alert(m_MiscLabel, m_RatingLabel, m_Form);
        }

        void AddFeedback(std::string name, std::string contents, int severity)
        {
            m_Map[name] = Feedback(contents, severity);
        }

        void UpdateAlert()
        {
            m_Alert->RegenerateFromTasks(m_Map);
        }

        ~Fixture()
        {
            delete (m_Alert);
            delete (m_HiddenParent);
            delete (m_App);
        }

        QApplication* m_App;
        Alert* m_Alert;
        QLabel* m_MiscLabel;
        QLabel* m_RatingLabel;
        QFormLayout* m_Form;
        QWidget* m_HiddenParent;
        std::map<std::string, Feedback> m_Map;
    };

    TEST_FIXTURE(Fixture, EmailLabelUpdatedWithFeedback)
    {
        QLabel emailLabel(m_HiddenParent);
        m_Alert->SetFeedbackLabel(&emailLabel, "Email");
        std::string text = "Emails are important";
        AddFeedback("Email", text, 3);
        UpdateAlert();
        CHECK_EQUAL(text, emailLabel.text().toStdString());
    }

    TEST_FIXTURE(Fixture, EmptyFeedbackTopHidden)
    {
        m_MiscLabel->setHidden(true);

        UpdateAlert();

        CHECK(m_MiscLabel->isHidden());
    }

    TEST_FIXTURE(Fixture, FeedbackWithMiscTextTopShown)
    {
        m_MiscLabel->setHidden(true);

        AddFeedback("Misc", "Misc feedback.", 3);
        UpdateAlert();

        CHECK(!m_MiscLabel->isHidden());
    }

    TEST_FIXTURE(Fixture, GivenMiscFeedbackDisplaysItOnTop)
    {
        AddFeedback("Misc", "This is a test", 1);
        UpdateAlert();
        QDomDocument doc;
        doc.setContent(m_MiscLabel->text());
        auto element = doc.firstChildElement("html").firstChildElement("body");

        CHECK_EQUAL("This is a test", element.text().toStdString());
    }

    TEST_FIXTURE(Fixture, FeedbackUpdatedDisplaysNewInformation)
    {
        AddFeedback("Misc", "This is a test", 1);
        UpdateAlert();
        AddFeedback("Misc", "New information", 0);
        UpdateAlert();
        QDomDocument doc;
        doc.setContent(m_MiscLabel->text());
        auto element = doc.firstChildElement("html").firstChildElement("body");

        CHECK_EQUAL("New information", element.text().toStdString());
    }

    TEST_FIXTURE(Fixture, RatingFeedbackReceivedChangesRatingLabel)
    {
        m_RatingLabel->setText("Bad");
        AddFeedback("Rating", "3", 0);
        UpdateAlert();
        CHECK(QString("Bad") != m_RatingLabel->text());
    }

    TEST_FIXTURE(Fixture, EmailFeedbackEmptyRemovesWidgetFromLayout)
    {
        auto m_EmailLabel = new QLabel(m_HiddenParent);
        m_Alert->SetFeedbackLabel(m_EmailLabel, "Email");
        m_Form->insertRow(0, nullptr, m_EmailLabel);
        AddFeedback("Email", "", 0);
        UpdateAlert();
        CHECK_EQUAL(0, m_Form->count());
    }

    TEST_FIXTURE(Fixture, EmailFeedbackWithTextAddsWidgetToLayout)
    {
        auto m_EmailLabel = new QLabel(m_HiddenParent);
        m_Alert->SetFeedbackLabel(m_EmailLabel, "Email");
        AddFeedback("Email", "We use emails everyday.", 0);
        UpdateAlert();
        CHECK_EQUAL(1, m_Form->count());
    }

    TEST_FIXTURE(Fixture, ScoreComponentFeedbackAddsTooltipText)
    {
        AddFeedback("ScoreComponents", "Tooltip text", 0);
        UpdateAlert();
        CHECK_EQUAL("Tooltip text", m_RatingLabel->toolTip().toStdString());
    }
}
