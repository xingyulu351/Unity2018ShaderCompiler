#pragma once

#include <glib.h>

class StateManager
{
public:
    static gboolean NextState();
    static gboolean PreviousState();

    enum InstallerState
    {
        // These are also the indices of the UI panels associated with the states
        kUninitialized = -1,
        kIntroduction,
        kLicenseAgreement,
        kChooseComponents,
        kInstallSettings,
        kDownload,
        kInstall,
        kComplete,
        kFinish
    };

private:
    static InstallerState state;
};
