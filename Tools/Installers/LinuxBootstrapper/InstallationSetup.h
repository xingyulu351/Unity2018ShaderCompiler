#pragma once

#include <gtk/gtk.h>

struct InstallationSettings
{
    GFile* installLocation;
    GFile* downloadLocation;
    bool createdInstallLocation;
    bool downloadLocationIsTemporary;
    double totalInstallationSizeGB;
    double availableSpaceGB;

    InstallationSettings();
};

void DoInstallationSetup();
void DoInstallationCleanup(bool installationSucceeded);
InstallationSettings* GetInstallationSettings();
void SetInstallationLocations(char* installLocation, char* downloadLocation);
