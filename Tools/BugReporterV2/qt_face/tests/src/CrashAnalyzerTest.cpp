#include "ReportDraftStub.h"
#include "NetworkReplyStub.h"
#include "CrashAnalyzerStub.h"
#include "AttachmentStub.h"
#include "reporter/ReportDraft.h"
#include <QJsonObject>
#include <QHttpPart>
#include <QHttpMultiPart>
#include <QNetworkReply>
#include <QApplication>
#include <UnitTest++.h>
#include <cstring>

SUITE(CrashAnalyzerSuite)
{
    using namespace ureport;
    using namespace ureport::qt_face;
    using namespace ureport::test;

    static std::shared_ptr<ReportDraftStub> CreateStubReport(std::string version, std::string platform, std::string logFilePath)
    {
        std::shared_ptr<ReportDraftStub> report(new ReportDraftStub());
        report->WriteFact("UnityVersion", version);
        report->WriteFact("OSName", platform);
        auto attachment = std::make_shared<AttachmentStub>(logFilePath);
        report->Attach(attachment);
        report->SetBugTypeID(ureport::Report::kBugCrashAuto);
        return report;
    }

    TEST(GetServerUrl_GivenUrlInConstructor_ReturnsTheUrl)
    {
        auto expectedServerUrl = "http://localhost:8080";
        auto report = CreateStubReport("", "", "/Users/root/Editor.log");
        auto target = new CrashAnalyzerStub(expectedServerUrl, report);
        auto actualServerUrl = target->GetServerUrl();

        CHECK_EQUAL(expectedServerUrl, actualServerUrl);
    }

    TEST(CreateVersionPart_GivenVersion_ContainsHeaderAndBody)
    {
        QHttpPart expectedVersionPart;
        expectedVersionPart.setHeader(QNetworkRequest::ContentDispositionHeader, QVariant("form-data; name=\"version\""));
        expectedVersionPart.setBody("5.3");

        auto versionPart = CrashAnalyzer::CreateVersionPart("5.3");

        CHECK(expectedVersionPart == versionPart);
    }

    TEST(CreatePlatformPart_GivenPlatform_ContainsHeaderAndBody)
    {
        QHttpPart expectedPlatformPart;
        expectedPlatformPart.setHeader(QNetworkRequest::ContentDispositionHeader, QVariant("form-data; name=\"platform\""));
        expectedPlatformPart.setBody("linux");

        auto platformPart = CrashAnalyzer::CreatePlatformPart("linux");

        CHECK(expectedPlatformPart == platformPart);
    }

    TEST(CreateLogFilePart_GivenLogFilePath_ContainsHeaders)
    {
        auto multiPart = std::make_shared<QHttpMultiPart>(QHttpMultiPart::FormDataType);
        QHttpPart expectedLogFilePart;
        expectedLogFilePart.setHeader(QNetworkRequest::ContentTypeHeader, QVariant("text/plain"));
        expectedLogFilePart.setHeader(QNetworkRequest::ContentDispositionHeader, QVariant("form-data; name=\"log_file\"; filename=\"case_883364-Editor.log.txt\""));

        auto logFilePart = CrashAnalyzer::CreateLogFilePart(multiPart, "/Users/root/case_883364-Editor.log.txt");
        QFile *file = (QFile*)multiPart->children().at(0);
        expectedLogFilePart.setBodyDevice(file);

        CHECK(expectedLogFilePart == logFilePart);
    }

    TEST(CreateLogFilePart_GivenMultiPart_ContainsFileAsChild)
    {
        auto multiPart = std::make_shared<QHttpMultiPart>(QHttpMultiPart::FormDataType);

        auto logFilePart = CrashAnalyzer::CreateLogFilePart(multiPart, "/Users/root/case_883364-Editor.log.txt");

        CHECK(multiPart->children().size() == 1);
        CHECK(multiPart->children().at(0)->metaObject()->className() == QFile::staticMetaObject.className());
    }

    TEST(GetReport_GivenErrorReply_ReturnsJsonReportWithError)
    {
        auto report = CreateStubReport("", "", "/Users/root/Editor.log");
        std::unique_ptr<CrashAnalyzerStub> target(new CrashAnalyzerStub(QNetworkReply::TimeoutError,
            "{\"aaa\":\"bbb\"}",
            report));

        auto actualReport = target->GetReport();

        QJsonDocument doc(actualReport);
        QString strJson(doc.toJson(QJsonDocument::Compact));
        CHECK(!actualReport.isEmpty());
        CHECK(actualReport.contains("error_code"));
        CHECK(actualReport.contains("error_text"));
    }

    TEST(GetReport_GivenReplyWithoutError_ReturnsNotEmptyJsonReport)
    {
        auto report = CreateStubReport("", "", "/Users/root/Editor.log");
        auto target = std::make_shared<CrashAnalyzerStub>(QNetworkReply::NoError, "{\"aaa\":\"bbb\"}", report);

        auto actualReport = target->GetReport();

        QJsonDocument doc(actualReport);
        QString strJson(doc.toJson(QJsonDocument::Compact));
        CHECK(!actualReport.isEmpty());
    }

    TEST(GetReport_UsesReportsPathInRequestUrl)
    {
        auto report = CreateStubReport("", "", "/Users/root/Editor.log");
        auto target = std::make_shared<CrashAnalyzerStub>("http://localhost:8080", report);


        auto actualReport = target->GetReport();

        QUrl actualRequestUrl = target->GetRequestUrl();
        CHECK(actualRequestUrl.toString().endsWith("/reports/"));
    }

    TEST(GetReport_UsesNotNullHttpMutliPart)
    {
        auto report = CreateStubReport("", "", "/Users/root/Editor.log");
        auto target = std::make_shared<CrashAnalyzerStub>("", report);

        auto actualReport = target->GetReport();

        auto actualRequestMultiPart = target->GetRequestMultiPart();
        CHECK(NULL != actualRequestMultiPart);
        CHECK(actualRequestMultiPart->metaObject()->className() == QHttpMultiPart::staticMetaObject.className());
    }

    TEST(GetReport_GivenNullReport_ReturnsEmptyJsonObject)
    {
        auto target = std::make_shared<CrashAnalyzerStub>("", nullptr);

        auto actualCrashReport = target->GetReport();

        CHECK(actualCrashReport.isEmpty());
    }

    TEST(GetReport_GivenReport_UsesVersionAndPlatfromFacts)
    {
        std::shared_ptr<ReportDraftStub> report = CreateStubReport("5.3", "MacOS", "/Users/root/Editor.log");
        std::unique_ptr<CrashAnalyzerStub> target(new CrashAnalyzerStub(QNetworkReply::NoError,
            "{\"aaa\":\"bbb\"}",
            report));

        auto crashReport = target->GetReport();

        CHECK_EQUAL("5.3", target->GetVersion());
        CHECK_EQUAL("MacOS", target->GetPlatform());
    }

    TEST(GetReport_GivenReportWithNoAttachments_ReturnsEmptyJsonObject)
    {
        std::shared_ptr<ReportDraftStub> report(new ReportDraftStub());
        std::unique_ptr<CrashAnalyzerStub> target(new CrashAnalyzerStub(QNetworkReply::NoError,
            "{\"aaa\":\"bbb\"}",
            report));

        auto crashReport = target->GetReport();

        CHECK(crashReport.isEmpty());
    }

    TEST(GetReport_GivenReportWithNoEditorLogAttachment_ReturnsEmptyJsonObject)
    {
        std::shared_ptr<ReportDraftStub> report = CreateStubReport("", "", "/Users/root/readme.txt");
        std::unique_ptr<CrashAnalyzerStub> target(new CrashAnalyzerStub(QNetworkReply::NoError,
            "{\"aaa\":\"bbb\"}",
            report));

        auto crashReport = target->GetReport();

        CHECK(crashReport.isEmpty());
    }

    TEST(GetReport_GivenReportWithEditorLog_UsesEditorLog)
    {
        const std::string expectedEditorLog = "/Users/root/Editor.log";
        std::shared_ptr<ReportDraftStub> report = CreateStubReport("", "", "/Users/root/Editor.log");
        std::unique_ptr<CrashAnalyzerStub> target(new CrashAnalyzerStub(QNetworkReply::NoError,
            "{\"aaa\":\"bbb\"}",
            report));

        auto crashReport = target->GetReport();

        CHECK_EQUAL(expectedEditorLog, target->GetEditorLogPath());
    }

    TEST(GetReport_GivenNonCrashReport_ReturnsEmptyJsonObject)
    {
        std::shared_ptr<ReportDraftStub> report = CreateStubReport("", "", "/Users/root/Editor.log");
        std::unique_ptr<CrashAnalyzerStub> target(new CrashAnalyzerStub(QNetworkReply::NoError,
            "{\"aaa\":\"bbb\"}",
            report));
        report->SetBugTypeID(ureport::Report::kBugProblemWithEditor);

        auto crashReport = target->GetReport();

        CHECK(crashReport.isEmpty());
    }

    TEST(GetServiceStatus_GivenReportWithTurnedOffCollecting_ReturnsNotAvailableServiceStatus)
    {
        std::shared_ptr<ReportDraftStub> report = CreateStubReport("5.3", "win32", "");
        std::unique_ptr<CrashAnalyzerStub> target(new CrashAnalyzerStub(QNetworkReply::NoError,
            "{\"version\":\"5.3\",\"platform\":\"win32\",\"enabled\":\"false\"}",
            report));
        auto crashServiceStatus = target->GetServiceStatus();
        CHECK_EQUAL(CrashServiceStatus::NotAvailable, crashServiceStatus);
    }

    TEST(GetServiceStatus_GivenReportWithTurnedOnCollecting_ReturnsAvailableServiceStatus)
    {
        std::shared_ptr<ReportDraftStub> report = CreateStubReport("5.3", "win32", "");
        std::unique_ptr<CrashAnalyzerStub> target(new CrashAnalyzerStub(QNetworkReply::NoError,
            "{\"version\":\"5.3\",\"platform\":\"win32\",\"enabled\":\"true\"}",
            report));
        auto crashServiceStatus = target->GetServiceStatus();
        CHECK_EQUAL(CrashServiceStatus::Available, crashServiceStatus);
    }

    TEST(GetServiceStatus_GivenErrorHttpResponse_ReturnsNotAvailableServiceStatus)
    {
        std::shared_ptr<ReportDraftStub> report = CreateStubReport("5.3", "win32", "");
        std::unique_ptr<CrashAnalyzerStub> target(new CrashAnalyzerStub(QNetworkReply::TimeoutError,
            "{\"version\":\"5.3\",\"platform\":\"win32\",\"enabled\":\"true\"}",
            report));
        auto crashServiceStatus = target->GetServiceStatus();
        CHECK_EQUAL(CrashServiceStatus::NotAvailable, crashServiceStatus);
    }
}
