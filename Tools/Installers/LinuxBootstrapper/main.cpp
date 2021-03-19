#include "main.h"
#include "StateManager.h"
#include "Introduction.h"
#include "Installation.h"
#include "DownloadManager.h"
#include "ComponentManager.h"
#include "InstallationSetup.h"
#include "ComponentDownloader.h"
#include "LicenseAgreement.h"
#include "artifacts/generated/LinuxBootstrapper/UI.h"

#include <string.h>

static GtkBuilder* s_UI;

char* g_Inifile = NULL;
gboolean g_Unattended = FALSE;
gboolean g_ListComponents = FALSE;
char* g_InstallLocation = NULL;
char* g_DownloadLocation = NULL;
char* g_ComponentNames = NULL;
char* g_ComponentListSaveLocation = NULL;
char* g_ComponentListLoadLocation = NULL;
gboolean g_Verbose = FALSE;

static GOptionEntry s_OptionEntries[] =
{
    { "inifile", 'i', 0, G_OPTION_ARG_STRING, &g_Inifile, "Use local ini file FILE", "FILE" },
    { "unattended", 'u', 0, G_OPTION_ARG_NONE, &g_Unattended, "Perform unattended install", NULL },
    { "list-components", 'L', 0, G_OPTION_ARG_NONE, &g_ListComponents, "List available components to install", NULL },
    { "install-location", 'l', 0, G_OPTION_ARG_STRING, &g_InstallLocation, "Use install location LOCATION in unattended mode", "LOCATION" },
    { "download-location", 'd', 0, G_OPTION_ARG_STRING, &g_DownloadLocation, "Use download location LOCATION in unattended mode", "LOCATION" },
    { "components", 'c', 0, G_OPTION_ARG_STRING, &g_ComponentNames, "Install comma separated list of components", "COMPONENTS" },
    { "save-component-list", 0, 0, G_OPTION_ARG_STRING, &g_ComponentListSaveLocation, "Save component list to FILE", "FILE" },
    { "use-component-list", 0, 0, G_OPTION_ARG_STRING, &g_ComponentListLoadLocation, "Use component list from FILE", "FILE" },
    { "verbose", 'v', 0, G_OPTION_ARG_NONE, &g_Verbose, "Print verbose output", NULL },
    { NULL }
};

int main(int argc, char **argv)
{
    // Check for switches that imply headless mode separately. We use these to decide
    // whether or not to look for GTK switches, which require a valid GTK context.
    for (int i = 0; i < argc; ++i)
    {
        if (strcmp(argv[i], "--unattended") == 0 ||
            strcmp(argv[i], "-u") == 0 ||
            strcmp(argv[i], "--list-components") == 0 ||
            strcmp(argv[i], "-L") == 0 ||
            strcmp(argv[i], "--help") == 0)
        {
            g_Unattended = TRUE;
            break;
        }
    }

    //
    // Initialize GTK
    //
    gboolean gtkOK = gtk_init_check(&argc, &argv);
    if (!g_Unattended && !gtkOK)
    {
        printf("GTK cannot open DISPLAY. Are you trying to run --unattended or --list-components?\n");
        return 1;
    }

    //
    // Initialize GLib's command line parsing routines
    //
    GOptionContext* optionContext = g_option_context_new("");
    g_option_context_add_main_entries(optionContext, s_OptionEntries, NULL);

    if (!g_Unattended)
    {
        g_option_context_add_group(optionContext, gtk_get_option_group(TRUE));
    }

    GError* cmdLineErr = NULL;
    if (!g_option_context_parse(optionContext, &argc, &argv, &cmdLineErr))
    {
        printf("%s: %s\n", argv[0], cmdLineErr->message);
        return 1;
    }

    g_option_context_free(optionContext);

    //
    // List components if requested
    //
    if (g_ListComponents)
    {
        g_Unattended = TRUE;
        InitializeEverything();
        while (!ComponentManager::IsReady())
            FlushEvents();

        printf("Usage:\n");
        printf("  For default component set, do not specify --components.\n");
        printf("  For all components: --components=All\n");
        printf("  For select components: --components=(comma separated list)\n");
        printf("Available components (case sensitive):\n");
        const std::vector<ComponentManager::Component*>& components = ComponentManager::GetAllComponents();
        for (std::vector<ComponentManager::Component*>::const_iterator i = components.begin();
             i != components.end(); ++i)
        {
            char extraInfo = ' ';
            if ((*i)->mandatory)
                extraInfo = '!';
            else if ((*i)->install)
                extraInfo = '*';
            printf(" (%c) %s: %s\n", extraInfo, (*i)->name, (*i)->description);
            delete *i;
        }
        printf("Key: '!' = required; '*' = selected by default.\n");
        return 0;
    }

    //
    // Do unattended setup if requested
    //
    if (g_Unattended)
    {
        if (!g_InstallLocation)
        {
            printf("Error: Unattended installations must provide install location\n");
            return 1;
        }
    }
    else if (g_InstallLocation || g_ComponentNames || (g_DownloadLocation && !g_ComponentListLoadLocation))
        printf("Warning: install-location, download-location, and components are only for unattended builds\n");

    if (g_ComponentListLoadLocation && !g_DownloadLocation)
    {
        printf("Error: Local component list requires local component cache (download-location)\n");
        return 1;
    }

    if (g_Unattended)
    {
        InitializeEverything();
        if (!DoLicenseAgreementUnattended())
        {
            printf("License agreement rejected, quitting\n");
            return 0;
        }
        printf("\nBeginning unattended installation to '%s', downloading packages to '%s'\n", g_InstallLocation, g_DownloadLocation ? g_DownloadLocation : "<temporary location>");
        while (!ComponentManager::IsReady())
            FlushEvents();
        ComponentManager::DoComponentSelection();
        ComponentManager::SelectComponents(g_ComponentNames);
        if (ComponentManager::GetComponentsForInstall().empty())
        {
            printf("No components selected. See --help.\n");
            return 1;
        }
        DoInstallationSetup();
        SetInstallationLocations(g_InstallLocation, g_DownloadLocation);
        ComponentDownloader::DoDownloadComponents();
        while (!ComponentDownloader::DoneDownloading())
            FlushEvents();
        Installation::DoInstallation();
        FlushEvents();
        CleanupEverything(true);
        printf("Installation succeeded! Run 'cd %s/Editor; ./Unity'\n", g_InstallLocation);
        return 0;
    }

    //
    // Graphical installation
    //
    s_UI = gtk_builder_new();
    GError *error = NULL;

    if (!gtk_builder_add_from_string(s_UI, g_GladeUI, -1, &error))
    {
        printf("%s\nError loading UI definition, aborting\n", error->message);
        return 1;
    }
    gtk_builder_connect_signals(s_UI, NULL);
    gtk_window_present(GTK_WINDOW(GetObject("mainWindow")));
    gtk_main();
    return 0;
}

GObject* GetObject(const char* name)
{
    return gtk_builder_get_object(s_UI, name);
}

void FlushEvents()
{
    if (g_Unattended)
    {
        GMainContext* context = g_main_context_default();
        while (g_main_context_pending(context))
            g_main_context_iteration(context, TRUE);
    }
    else
        while (gtk_events_pending())
            gtk_main_iteration();
}

void InitializeEverything()
{
    DownloadManager::Initialize();
    ComponentManager::Initialize();
}

void CleanupEverything(bool installationSucceeded)
{
    Installation::Cancel();
    DownloadManager::CancelAllDownloads();
    DownloadManager::Cleanup();
    ComponentDownloader::Cleanup();
    ComponentManager::Cleanup();
    DoInstallationCleanup(installationSucceeded);
}

void Quit(bool success)
{
    if (g_Unattended)
        exit(success ? 0 : 1);
    gtk_main_quit();
}

extern "C"
{
    gboolean OnMainWindowShown(GtkWidget* mainWindow, GdkEvent* event, gpointer data)
    {
        InitializeEverything();
        StateManager::NextState();
        DoIntroduction();
        return TRUE;
    }
}
