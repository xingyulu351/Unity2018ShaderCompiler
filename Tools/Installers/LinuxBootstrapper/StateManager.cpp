#include "StateManager.h"
#include "main.h"
#include "Introduction.h"
#include "Installation.h"
#include "LicenseAgreement.h"
#include "InstallationSetup.h"
#include "DownloadManager.h"
#include "ComponentManager.h"
#include "ComponentDownloader.h"

#include <glib/gstdio.h>

StateManager::InstallerState StateManager::state = StateManager::kUninitialized;
const char* kUnityExecutable = "Unity";
const char* kEditorDirectory = "Editor";

static void LaunchUnity()
{
    const InstallationSettings* settings = GetInstallationSettings();
    gchar* installLocation = g_file_get_path(settings->installLocation);
    gchar* editorPath = g_strdup_printf("%s/%s", installLocation, kEditorDirectory);
    gchar* unityExecutable = g_strdup_printf("%s/%s", editorPath, kUnityExecutable);
    gchar* argv[] = { unityExecutable, NULL };
    GPid pid;

    // TODO: Make Unity not freak out if it's not launched from its own directory
    g_chdir(editorPath);
    g_spawn_async(NULL, argv, NULL, G_SPAWN_DO_NOT_REAP_CHILD, NULL, NULL, &pid, NULL);

    g_free(editorPath);
    g_free(installLocation);
    g_free(unityExecutable);
}

gboolean StateManager::NextState()
{
    gint newState = (gint)state + 1;
    state = (StateManager::InstallerState)newState;
    if (state == StateManager::kFinish)
    {
        if (gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON(GetObject("launchUnityCheckbox"))))
            LaunchUnity();
        CleanupEverything(true);
        Quit(true);
    }
    gtk_notebook_set_current_page(GTK_NOTEBOOK(GetObject("contentPane")), state);
    return FALSE;
}

gboolean StateManager::PreviousState()
{
    gint newState = (gint)state - 1;
    state = (StateManager::InstallerState)newState;
    gtk_notebook_set_current_page(GTK_NOTEBOOK(GetObject("contentPane")), state);
    return FALSE;
}

extern "C"
{
    void GoForward(GtkButton* button, gpointer data)
    {
        StateManager::NextState();
    }

    void GoBack(GtkButton* button, gpointer data)
    {
        StateManager::PreviousState();
    }

    void CancelEverything(GtkButton* button, gpointer data)
    {
        CleanupEverything(false);
        Quit(false);
    }

    void RunUIForState(GtkNotebook* notebook, gpointer page, guint pageIndex, gpointer data)
    {
        switch ((StateManager::InstallerState)pageIndex)
        {
            case StateManager::kIntroduction:
                DoIntroduction();
                break;
            case StateManager::kLicenseAgreement:
                DoLicenseAgreement();
                break;
            case StateManager::kChooseComponents:
                ComponentManager::DoComponentSelection();
                break;
            case StateManager::kInstallSettings:
                DoInstallationSetup();
                break;
            case StateManager::kDownload:
                ComponentDownloader::DoDownloadComponents();
                break;
            case StateManager::kInstall:
            {
                GtkButton *forwardButton = GTK_BUTTON(GetObject("forwardButton"));
                gtk_button_set_label(forwardButton, "gtk-go-forward");
                gtk_button_set_use_stock(forwardButton, TRUE);
                Installation::DoInstallation();
            }
            break;
            case StateManager::kComplete:
            {
                GtkButton* forwardButton = GTK_BUTTON(GetObject("forwardButton"));
                gtk_button_set_use_stock(forwardButton, FALSE);
                gtk_button_set_label(forwardButton, "Finish");
                gtk_widget_set_sensitive(GTK_WIDGET(forwardButton), TRUE);
                gtk_widget_set_sensitive(GTK_WIDGET(GetObject("backButton")), FALSE);
                gtk_widget_set_sensitive(GTK_WIDGET(GetObject("cancelButton")), FALSE);
                break;
            }
            case StateManager::kFinish:
            case StateManager::kUninitialized:
            default:
                g_warn_if_reached();
        }
    }
}
