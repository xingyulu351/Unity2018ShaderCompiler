#include "reporter/UnityBugMessage.h"
#include "ReportDummy.h"
#include "ReportFake.h"
#include "FilesPackerDummy.h"
#include "FilesPackerSpy.h"
#include "FilePathPackerSpy.h"
#include "AttachmentStub.h"
#include "FileAttachmentDummy.h"
#include "OperationObserverDummy.h"
#include "test/AssertStringContains.h"

#include <UnitTest++.h>

SUITE(UnityBugMessage)
{
    using namespace ureport;
    using namespace ureport::test;

    FilesPackerDummy dummyPacker;
    OperationObserverDummy dummyObserver;

    TEST(Compose_GivenReportContainsNoUnityVersionFact_ReturnsMessageWithNoUnityVersion)
    {
        ReportDummy report;
        const auto message = UnityBugMessage::Compose(report, dummyPacker, &dummyObserver);
        CHECK(message.GetUnityVersion() == "");
    }

    TEST(Compose_GivenReportContainsUnityVersionFact_ReturnsMessageWithUnityVersion)
    {
        FactsReportFake report;
        report.m_Facts["UnityVersion"] = "Unity version";
        const auto message = UnityBugMessage::Compose(report, dummyPacker, &dummyObserver);
        CHECK(message.GetUnityVersion() == "Unity version");
    }

    TEST(Compose_GivenReportWithTextualDescription_ReturnsMessageWithEvent)
    {
        DescriptionReportFake report;
        report.m_Description = "Description";
        const auto message = UnityBugMessage::Compose(report, dummyPacker, &dummyObserver);
        CHECK(message.GetEvent() == "Description");
    }

    TEST(Compose_GivenReportWithTitle_ReturnsMessageWithEventStartsWithTheTitle)
    {
        TitleReportFake report;
        report.m_Title = "Title";
        const auto message = UnityBugMessage::Compose(report, dummyPacker, &dummyObserver);
        CHECK_EQUAL(0, message.GetEvent().find("Title"));
    }

    TEST(Compose_GivenReportWithTitleAndTextualDescription_ReturnsMessageWithEventContainsReportTitleAndDescription)
    {
        TitleAndDescriptionReportFake report;
        report.m_Title = "Title";
        report.m_Description = "Description";
        const auto message = UnityBugMessage::Compose(report, dummyPacker, &dummyObserver);
        CHECK(message.GetEvent() == "Title\n\nDescription");
    }

    TEST(Compose_GivenReportWithReporterEmail_ReturnsMessageWithCustomerEmail)
    {
        EmailReportFake report;
        report.m_Email = "email";
        const auto message = UnityBugMessage::Compose(report, dummyPacker, &dummyObserver);
        CHECK_EQUAL("email", message.GetCustomerEmail());
    }

    TEST(Compose_GivenReportWithBugProblemWithEditor_ReturnsMessageWithBugTypeID)
    {
        BugTypeIDReportFake report;
        report.m_BugTypeID = Report::kBugProblemWithEditor;
        const auto message = UnityBugMessage::Compose(report, dummyPacker, &dummyObserver);
        CHECK_EQUAL(0, message.GetBugTypeID());
    }

    TEST(Compose_GivenReportWithServices_ReturnsMessageWithBugTypeID)
    {
        BugTypeIDReportFake report;
        report.m_BugTypeID = Report::kBugServices;
        const auto message = UnityBugMessage::Compose(report, dummyPacker, &dummyObserver);
        CHECK_EQUAL(11, message.GetBugTypeID());
    }

    TEST(Compose_GivenReportWithBugReproAlways_ReturnsMessageWithBugRepro)
    {
        BugReproReportFake report;
        report.m_BugRepro = Report::kReproAlways;
        const auto message = UnityBugMessage::Compose(report, dummyPacker, &dummyObserver);
        CHECK_EQUAL("Always", message.GetBugReproducibility());
    }

    TEST(Compose_GivenReportWithBugReproSometimes_ReturnsMessageWithBugRepro)
    {
        BugReproReportFake report;
        report.m_BugRepro = Report::kReproSometimes;
        const auto message = UnityBugMessage::Compose(report, dummyPacker, &dummyObserver);
        CHECK_EQUAL("Sometimes, but not always", message.GetBugReproducibility());
    }

    TEST(Compose_GivenReportWithBugReproFirstTime_ReturnsMessageWithBugRepro)
    {
        BugReproReportFake report;
        report.m_BugRepro = Report::kReproFirstTime;
        const auto message = UnityBugMessage::Compose(report, dummyPacker, &dummyObserver);
        CHECK_EQUAL("This is the first time", message.GetBugReproducibility());
    }

    TEST(Compose_GivenReportWithBugReproMinimumValue_DoesntReturnsUnknownRepro)
    {
        BugReproReportFake report;
        report.m_BugRepro = Report::kReproMinimumValue;
        const auto message = UnityBugMessage::Compose(report, dummyPacker, &dummyObserver);
        CHECK(message.GetBugReproducibility() != "Unknown");
    }

    TEST(Compose_GivenReportWithBugReproMaximumValue_DoesntReturnsUnknownRepro)
    {
        BugReproReportFake report;
        report.m_BugRepro = Report::kReproMaximumValue;
        const auto message = UnityBugMessage::Compose(report, dummyPacker, &dummyObserver);
        CHECK(message.GetBugReproducibility() != "Unknown");
    }

    TEST(Compose_GivenReportWithNoFiles_ReturnsMessageWithEmptyFiles)
    {
        ReportDummy report;
        const auto message = UnityBugMessage::Compose(report, dummyPacker, &dummyObserver);
        CHECK(message.GetAttachedFilePath().empty());
    }

    TEST(Compose_GivenReportContainsAttachedFile_PackTheFile)
    {
        FilesReportFake report;
        report.m_Attachments.push_back(std::make_shared<AttachmentStub>("attachment"));
        FilesPackerSpy packer;
        auto message = UnityBugMessage::Compose(report, packer, &dummyObserver);
        CHECK_EQUAL(1, packer.GetFiles().size());
        CHECK_EQUAL("attachment", packer.GetFiles().front()->GetPath());
    }

    TEST(Compose_GivenReportContainsOSNameFact_ReturnsMessageWithComputerStartsWithTheOSName)
    {
        FactsReportFake report;
        report.m_Facts["OSName"] = "OS name";
        const auto message = UnityBugMessage::Compose(report, dummyPacker, &dummyObserver);
        CHECK_EQUAL(0, message.GetComputer().find("OS name"));
    }

    TEST(Compose_GivenEmptyReport_ReturnsMessageWithNonEmptyTimestamp)
    {
        ReportDummy report;
        const auto message = UnityBugMessage::Compose(report, dummyPacker, &dummyObserver);
        CHECK(!message.GetTimestamp().empty());
    }

    TEST(Compose_GivenReportWithBasicHardwareTypeFact_ReturnsMessageWithComputerContainsTheBasicHardwareType)
    {
        FactsReportFake report;
        report.m_Facts["BasicHardwareType"] = "Basic hardware type";
        const auto message = UnityBugMessage::Compose(report, dummyPacker, &dummyObserver);
        CHECK(message.GetComputer() == "Basic hardware type");
    }

    TEST(Compose_GivenReportWithGrapicsHardwareTypeFact_ReturnsMessageWithComputerContainsTheGraphicsHarwareType)
    {
        FactsReportFake report;
        report.m_Facts["GraphicsHardwareType"] = "Graphics hardware type";
        const auto message = UnityBugMessage::Compose(report, dummyPacker, &dummyObserver);
        CHECK(message.GetComputer() == "Graphics hardware type");
    }

    TEST(Compose_GivenReportWithBasicAndGraphicsHardwareTypeFacts_ReturnsMessageWithComputerComposedFromTheTypesSeparatedBySemicolon)
    {
        FactsReportFake report;
        report.m_Facts["BasicHardwareType"] = "Basic hardware type";
        report.m_Facts["GraphicsHardwareType"] = "Graphics hardware type";
        const auto message = UnityBugMessage::Compose(report, dummyPacker, &dummyObserver);
        CHECK(message.GetComputer() == "Basic hardware type; Graphics hardware type");
    }

    TEST(Compose_GivenReportWithOSNameBasicAndGraphicsHardwareTypeFacts_ReturnsMessageWithComputerComposedFromTheFactsSeparatedBySemicolon)
    {
        FactsReportFake report;
        report.m_Facts["OSName"] = "OS name";
        report.m_Facts["BasicHardwareType"] = "Basic hardware type";
        report.m_Facts["GraphicsHardwareType"] = "Graphics hardware type";
        const auto message = UnityBugMessage::Compose(report, dummyPacker, &dummyObserver);
        CHECK(message.GetComputer() == "Basic hardware type; Graphics hardware type; OS name");
    }

    TEST(Compose_GivenReportWithUnityEditorModeFact_ReturnsMessageWithComputerEqualToTheFact)
    {
        FactsReportFake report;
        report.m_Facts["UnityEditorMode"] = "Unity editor mode";
        const auto message = UnityBugMessage::Compose(report, dummyPacker, &dummyObserver);
        CHECK(message.GetComputer() == "Unity editor mode");
    }

    TEST(Compose_GivenReportWithOSNameAndUnityEditorMode_ReturnsMessageWithComputerContainsOSNameThenUnityEditorModeSeparatedBySemicolon)
    {
        FactsReportFake report;
        report.m_Facts["UnityEditorMode"] = "Unity editor mode";
        report.m_Facts["OSName"] = "OS name";
        const auto message = UnityBugMessage::Compose(report, dummyPacker, &dummyObserver);
        CHECK(message.GetComputer() == "OS name; Unity editor mode");
    }

    TEST(Compose_GivenReportWithBasicHardwareTypeAndUnityEditorMode_ReturnsMessageWithComputerContainsBasicHardwareTypeThenUnityEditorModeSeparatedBySemicolon)
    {
        FactsReportFake report;
        report.m_Facts["UnityEditorMode"] = "Unity editor mode";
        report.m_Facts["BasicHardwareType"] = "Basic hardware";
        const auto message = UnityBugMessage::Compose(report, dummyPacker, &dummyObserver);
        CHECK(message.GetComputer() == "Basic hardware; Unity editor mode");
    }

    TEST(Compose_GivenReportWithGraphicsHardwareTypeAndUnityEditorMode_ReturnsMessageWithComputerContainsGraphicsHardwareTypeThenUnityEditorModeSeparatedBySemicolon)
    {
        FactsReportFake report;
        report.m_Facts["UnityEditorMode"] = "Unity editor mode";
        report.m_Facts["GraphicsHardwareType"] = "Graphics hardware";
        const auto message = UnityBugMessage::Compose(report, dummyPacker, &dummyObserver);
        CHECK(message.GetComputer() == "Graphics hardware; Unity editor mode");
    }

    TEST(Compose_GivenReportWithAllFactsAboutComputer_ReturnsMessageWithComputerProperlyComposedFromTheFacts)
    {
        FactsReportFake report;
        report.m_Facts["UnityEditorMode"] = "Unity editor mode";
        report.m_Facts["OSName"] = "Operating system";
        report.m_Facts["GraphicsHardwareType"] = "Graphics hardware";
        report.m_Facts["BasicHardwareType"] = "Basic hardware";
        const auto message = UnityBugMessage::Compose(report, dummyPacker, &dummyObserver);
        CHECK(message.GetComputer() ==
            "Basic hardware; Graphics hardware; Operating system; Unity editor mode");
    }

    TEST(Compose_GivenReportWithBugPublicityFact_ReturnsMessageWithEventContainsPublicStatus)
    {
        FactsReportFake report;
        report.m_Facts["Publicity"] = "status";
        const auto message = UnityBugMessage::Compose(report, dummyPacker, &dummyObserver);
        AssertStringContains(message.GetEvent(), "Public status: status");
    }

    TEST(Compose_GivenReportWithTwoAttachmentsWithDifferentNames_PacksBoth)
    {
        FilesReportFake report;
        std::list<std::shared_ptr<Attachment> > attachments;
        attachments.push_back(std::make_shared<FileAttachmentDummy>("a"));
        attachments.push_back(std::make_shared<FileAttachmentDummy>("b"));
        report.m_Attachments = attachments;
        FilePathPackerSpy packer;
        UnityBugMessage::Compose(report, packer, &dummyObserver);
        CHECK_EQUAL("a", packer.m_Paths[0]);
        CHECK_EQUAL("b", packer.m_Paths[1]);
    }

    TEST(Compose_GivenReportWithTwoAttachmentsWithSameName_PacksBothAndPostfixSecondName)
    {
        FilesReportFake report;
        std::list<std::shared_ptr<Attachment> > attachments;
        attachments.push_back(std::make_shared<FileAttachmentDummy>("c"));
        attachments.push_back(std::make_shared<FileAttachmentDummy>("c"));
        report.m_Attachments = attachments;
        FilePathPackerSpy packer;
        UnityBugMessage::Compose(report, packer, &dummyObserver);
        CHECK_EQUAL("c", packer.m_Paths[0]);
        CHECK_EQUAL("c(2)", packer.m_Paths[1]);
    }

    TEST(Compose_GivenReportWithManyAttachmentsWithSameName_PacksAllAndPostfixDuplicateNames)
    {
        const int totalFiles = 1000;
        FilesReportFake report;
        std::list<std::shared_ptr<Attachment> > attachments;
        for (int i = 0; i < totalFiles; ++i)
            attachments.push_back(std::make_shared<FileAttachmentDummy>("d"));
        report.m_Attachments = attachments;
        FilePathPackerSpy packer;
        UnityBugMessage::Compose(report, packer, &dummyObserver);
        CHECK_EQUAL("d", packer.m_Paths[0]);
        for (int i = 1; i < totalFiles; ++i)
        {
            std::string expectedName = "d(" + std::to_string(static_cast<long long>(i + 1)) + ")";
            CHECK_EQUAL(expectedName, packer.m_Paths[i]);
        }
    }

    TEST(Compose_GivenReportWithLicenseTypeFact_ReturnsMessageWithEventContainsLicenseType)
    {
        FactsReportFake report;
        report.m_Facts["LicenseType"] = "license type";
        const auto message = UnityBugMessage::Compose(report, dummyPacker, &dummyObserver);
        AssertStringContains(message.GetEvent(), "License type: license type");
    }

    TEST(Compose_GivenReportWithNoLicenseTypeFact_ReturnsMessageWithEventDoeNotContainLicenseType)
    {
        FactsReportFake report;
        const auto message = UnityBugMessage::Compose(report, dummyPacker, &dummyObserver);
        CHECK_EQUAL("", message.GetEvent());
    }
}
