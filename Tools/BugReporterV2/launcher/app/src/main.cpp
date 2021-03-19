#include "ArgUtils.h"
#include "ReportBuilder.h"
#include "Args.h"
#include "ExternalAttachmentFactory.h"
#include "ReportSettingsLoader.h"
#include "UnityPlatformTraits.h"
#include "reporter/Workspace.h"
#include "reporter/Reporter.h"
#include "reporter/FilesPacker.h"
#include "reporter/Collector.h"
#include "reporter/ReportDraft.h"
#include "reporter/ReportModifier.h"
#include "CollabReportModifier.h"
#include "UnityPackageManagerReportModifier.h"
#include "attachment/Attachment.h"
#include "attachment/AttachmentFactory.h"
#include "attachment_preview/AttachmentTextPreview.h"
#include "modules/QtFaceModule.h"
#include "modules/QtSenderModule.h"
#include "modules/QtExporterModule.h"
#include "modules/SystemCollectorsModule.h"
#include "modules/PlatformCollectors.h"
#include "modules/UnityCollectors.h"
#include "zip_packer/ZipFilesPacker.h"
#include "file_system/SystemFileGateway.h"
#include "file_system/FileSystem.h"
#include "system_interplay/SystemEnvironment.h"
#include "system_interplay/Gate.h"
#include "shims/logical/IsEmpty.h"
#include "settings/Settings.h"

#include <iostream>
#include <algorithm>
#include <vector>
#include <map>
#include "../../../system_collectors/lib/src/OSInfoCollector.h"

using namespace ureport;
using namespace launcher;

bool IsNDAPlatform(const std::string& name)
{
    static auto const traits = CreateUnityPlatformTraitsMap();
    auto const record = traits.find(name);
    if (record != traits.end())
        return record->second.isNDA;
    return false;
}

bool IsNDAPlatform(const Report& report)
{
    return IsNDAPlatform(report.ReadFact("UnityEditorMode"));
}

void SetReportDefaults(ReportDraft& report)
{
    report.SetTextualDescription(
        "1. What happened\n"
        "\n"
        "2. How we can reproduce it using the example you attached"
        "\n");
}

std::unique_ptr<Reporter> BuildReporter()
{
    auto& workspace = Workspace::Initialize();
    auto reporter = CreateNewReporter();
    reporter->SetWorksheet(workspace.GetReportWorksheet());
    reporter->SetSender(workspace.GetSender());
    reporter->SetExporter(workspace.GetExporter());
    reporter->SetSettings(GetSettings());
    reporter->SetFilesPacker(CreateZipFilesPacker(CreateSystemFileGateway(),
        CreateSystemEnvironment()));
    auto reportDefaults = MakeReportModifier(SetReportDefaults);
    auto platformModifier = MakeReportModifier([] (ReportDraft& report) {
        if (IsNDAPlatform(report))
            report.WriteFact("Publicity", "Non Public");
    });
    reporter->AddReportInitializer(reportDefaults);
    reporter->AddReportInitializer(std::make_shared<ReportSettingsLoader>(
        GetSettings()));
    reporter->AddReportInitializer(std::make_shared<unity_collectors::UnityVersionCollector>(
        unity_collectors::UnityVersionCollector(unity_version::CreateUnityVersion())));
    reporter->AddReportInitializer(std::make_shared<system_collectors::OSInfoCollector>(
        system_collectors::OSInfoCollector(CreateSystemInfoProvider())));
    reporter->AddReportInitializer(platformModifier);
    auto collectors = workspace.GetAllCollectors();
    std::for_each(collectors.begin(), collectors.end(),
        [&reporter] (std::shared_ptr<Collector>& collector)
        {
            reporter->AddCollector(collector);
        });
    return reporter;
}

std::unique_ptr<ReportDraft> BuildReport(
    const Args& args,
    std::shared_ptr<AttachmentFactory> attachmentFactory,
    std::shared_ptr<FileSystem> fileSystem)
{
    auto builder = CreateReportBuilder(attachmentFactory, fileSystem);
    return builder->BuildReport(args);
}

int Launch(const std::vector<std::string>& args)
{
    auto commandLineArgs = Args::Parse(
        "unity_project,attach,editor_mode,bugtype", args);

    std::shared_ptr<AttachmentFactory> factory = CreateExternalAttachmentFactory();
    std::shared_ptr<FileSystem> fileSystem = CreateFileSystem();

    auto reporter = BuildReporter();
    if (commandLineArgs->GetValues("unity_project").size() > 0)
    {
        reporter->AddReportInitializer(std::make_shared<CollabReportModifier>(
            commandLineArgs->GetValues("unity_project")[0], fileSystem, factory));
        reporter->AddReportInitializer(std::make_shared<UnityPackageManagerReportModifier>(
            commandLineArgs->GetValues("unity_project")[0], fileSystem, factory));
    }

    std::shared_ptr<ReportDraft> report = BuildReport(*commandLineArgs, factory, fileSystem);
    reporter->DoReport(report);
    return 0;
}
