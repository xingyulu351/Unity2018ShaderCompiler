#pragma once

#include "ComponentManager.h"

#include <map>
#include <vector>

class Installation
{
public:
    static void DoInstallation();
    static void SetCurrentArchiveExtractionProgress(double fraction);
    static void Cancel();

private:
    static bool InstallComponentFromPkg(ComponentManager::Component* component);
    static bool InstallComponentFromArchive(ComponentManager::Component* component);
    static gboolean InstallAllComponents();
    static char* RemapInstallationPath(char* originalPath);
    static char* RemapTitle(char* componentTitle);
    static std::vector<char*> ExtractArchive(char* archiveFile, char* destinationPath, bool logDetails);

    static size_t stepCount;
    static size_t currentStep;
    static std::map<char*, char*> osxPathMap;
    static std::map<char*, char*> osxTitleMap;
    static bool shouldCancel;
};
