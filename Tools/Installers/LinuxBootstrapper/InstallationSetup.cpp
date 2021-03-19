#include "InstallationSetup.h"
#include "main.h"

#include <sys/vfs.h>
#include <sys/stat.h>
#include <cstdlib>
#include <cstring>
#include <cerrno>

#include "artifacts/generated/Configuration/UnityConfigureVersion.gen.h"

extern gboolean g_Unattended;
extern char* g_DownloadLocation;

static InstallationSettings s_InstallationSettings;

const char* kInstallationSizePrefix = "Total space required";
const char* kAvailableSpacePrefix = "Space available";

static bool DirectoryExists(GFile* file)
{
    return (G_FILE_TYPE_DIRECTORY == g_file_query_file_type(file, G_FILE_QUERY_INFO_NONE, NULL));
}

static bool IsWritableDirectory(GFile* file)
{
    if (!DirectoryExists(file))
        return false;
    GFileInfo *info = g_file_query_info(file, G_FILE_ATTRIBUTE_ACCESS_CAN_WRITE, G_FILE_QUERY_INFO_NONE, NULL, NULL);
    gboolean writable = g_file_info_get_attribute_boolean(info, G_FILE_ATTRIBUTE_ACCESS_CAN_WRITE);
    g_object_unref(info);
    return writable != FALSE;
}

static void Validate()
{
    if (g_Unattended)
    {
        if (!s_InstallationSettings.downloadLocationIsTemporary && !IsWritableDirectory(s_InstallationSettings.downloadLocation))
        {
            gchar* downloadLocation = g_file_get_path(s_InstallationSettings.downloadLocation);
            printf("Unable to write to download location '%s', aborting\n", downloadLocation);
            g_free(downloadLocation);
            exit(1);
        }
        if (!s_InstallationSettings.installLocation || !IsWritableDirectory(s_InstallationSettings.installLocation))
        {
            gchar* installLocation = g_file_get_path(s_InstallationSettings.installLocation);
            printf("Unable to write to install location '%s', aborting\n", installLocation);
            g_free(installLocation);
            exit(1);
        }
        if (s_InstallationSettings.totalInstallationSizeGB > s_InstallationSettings.availableSpaceGB)
        {
            printf("Required space %0.2fGB is greater than available space %0.2fGB on volume, aborting\n", s_InstallationSettings.totalInstallationSizeGB, s_InstallationSettings.availableSpaceGB);
            exit(1);
        }
        return;
    }

    GtkFileChooser* downloadLocationButton = GTK_FILE_CHOOSER(GetObject("downloadLocationButton"));
    GFile* downloadLocation = gtk_file_chooser_get_file(downloadLocationButton);
    gboolean useTemporaryDownloadLocation = gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON(GetObject("downloadLocationTemporaryCheckbox")));
    gboolean validDownloadLocationChosen =  useTemporaryDownloadLocation ||
        (downloadLocation && IsWritableDirectory(downloadLocation));
    if (downloadLocation)
        g_object_unref(downloadLocation);

    GFile* installationLocation = gtk_file_chooser_get_file(GTK_FILE_CHOOSER(GetObject("installLocationButton")));
    gboolean validInstallationLocationChosen = installationLocation && IsWritableDirectory(installationLocation);
    if (installationLocation)
        g_object_unref(installationLocation);

    gtk_widget_set_sensitive(GTK_WIDGET(downloadLocationButton), !useTemporaryDownloadLocation);
    gtk_widget_set_sensitive(GTK_WIDGET(GetObject("backButton")), TRUE);
    gtk_widget_set_sensitive(GTK_WIDGET(GetObject("forwardButton")), validDownloadLocationChosen &&
        validInstallationLocationChosen &&
        s_InstallationSettings.availableSpaceGB > s_InstallationSettings.totalInstallationSizeGB);
}

static char* GetUnityNameWithVersion()
{
    return "Unity-" UNITY_VERSION;
}

static void UpdateAvailableSpace()
{
    struct statfs buf;
    gchar* path = g_file_get_path(s_InstallationSettings.installLocation);
    if (statfs(path, &buf) == 0)
    {
        s_InstallationSettings.availableSpaceGB = buf.f_bavail * buf.f_bsize / (double)kGigabyte;
        char* availableSizeText = g_strdup_printf("%s: %0.2f GB", kAvailableSpacePrefix, s_InstallationSettings.availableSpaceGB);
        if (g_Unattended)
            printf("Available space: %s\n", availableSizeText);
        else
            gtk_label_set_text(GTK_LABEL(GetObject("availableSpaceLabel")), availableSizeText);
        g_free(availableSizeText);
    }
    else
    {
        printf("Unable to query available space for '%s'.\n", path);
    }
    g_free(path);
}

static void SetDefaultLocations()
{
    GFile* baseLocation = g_file_new_for_path("/opt");
    if (!IsWritableDirectory(baseLocation))
    {
        g_object_unref(baseLocation);
        baseLocation = g_file_new_for_path(g_get_home_dir());
    }
    if (s_InstallationSettings.installLocation)
        g_object_unref(s_InstallationSettings.installLocation);
    s_InstallationSettings.installLocation = g_file_get_child(baseLocation, GetUnityNameWithVersion());
    g_object_unref(baseLocation);
    if (!DirectoryExists(s_InstallationSettings.installLocation))
    {
        s_InstallationSettings.createdInstallLocation = true;
        g_file_make_directory(s_InstallationSettings.installLocation, NULL, NULL);
    }

    if (!g_Unattended)
    {
        gtk_file_chooser_set_file(GTK_FILE_CHOOSER(GetObject("installLocationButton")), s_InstallationSettings.installLocation, NULL);

        const char* downloadPath = g_get_user_special_dir(G_USER_DIRECTORY_DOWNLOAD);
        if (g_DownloadLocation)
        {
            downloadPath = g_DownloadLocation;
            gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(GetObject("downloadLocationCustomCheckbox")), TRUE);
        }
        GFile *userDownloadDirectory = g_file_new_for_path(downloadPath);
        gtk_file_chooser_set_file(GTK_FILE_CHOOSER(GetObject("downloadLocationButton")), userDownloadDirectory, NULL);
        g_object_unref(userDownloadDirectory);
    }
}

void DoInstallationSetup()
{
    char* installationSizeLabel = g_strdup_printf("%s: %0.2f GB", kInstallationSizePrefix, s_InstallationSettings.totalInstallationSizeGB);
    if (g_Unattended)
        printf("Required installation size: %s\n", installationSizeLabel);
    else
        gtk_label_set_text(GTK_LABEL(GetObject("requiredSpaceLabel")), installationSizeLabel);
    g_free(installationSizeLabel);
    SetDefaultLocations();
    UpdateAvailableSpace();
    Validate();
}

void DoInstallationCleanup(bool installationSucceeded)
{
    if (s_InstallationSettings.downloadLocationIsTemporary && s_InstallationSettings.downloadLocation)
        g_file_trash(s_InstallationSettings.downloadLocation, NULL, NULL);
    if (!installationSucceeded && s_InstallationSettings.createdInstallLocation)
        g_file_trash(s_InstallationSettings.installLocation, NULL, NULL);
}

static GFile* CleanPath(gchar* path)
{
    if (path[0] == '~')
    {
        const gchar* homeDir = g_getenv("HOME");
        if (homeDir == NULL || !*homeDir) // can't substitute, so return original
            return g_file_new_for_path(path);

        size_t fullPathSize = strlen(homeDir) + strlen(path); // minus ~, plus \0
        gchar* fullPath = (gchar*)malloc(fullPathSize);
        g_strlcpy(fullPath, homeDir, fullPathSize);
        g_strlcat(fullPath, path + 1, fullPathSize);

        GFile* out = g_file_new_for_path(fullPath);
        free(fullPath);
        return out;
    }
    else
        return g_file_new_for_path(path);
}

static bool CreateDirectoryIfNotExist(const char* path)
{
    if (mkdir(path, 0777) != 0)
    {
        if (errno != EEXIST)
        {
            // Error
            perror(path);
            return false;
        }
    }
    return true;
}

static void CreateInstallationPath(GFile* gpath)
{
    gchar* path = g_file_get_path(gpath);

    // Attempt to create the sub-paths
    bool ok = true;
    for (gchar* slash = strstr(path + 1, "/"); ok && slash != NULL; slash = strstr(slash + 1, "/"))
    {
        *slash = '\0'; // terminate this substring
        ok = CreateDirectoryIfNotExist(path);
        *slash = '/'; // restore the substring
    }

    // Attempt to create the full path
    if (ok)
        CreateDirectoryIfNotExist(path);
}

void SetInstallationLocations(char* installLocation, char* downloadLocation)
{
    if (s_InstallationSettings.installLocation)
        g_object_unref(s_InstallationSettings.installLocation);
    s_InstallationSettings.installLocation = CleanPath(installLocation);

    // No failure check here because this is checked in Validate()
    CreateInstallationPath(s_InstallationSettings.installLocation);

    if (downloadLocation)
    {
        if (s_InstallationSettings.downloadLocation)
            g_object_unref(s_InstallationSettings.downloadLocation);
        s_InstallationSettings.downloadLocation = CleanPath(downloadLocation);
        CreateInstallationPath(s_InstallationSettings.downloadLocation);
    }
    s_InstallationSettings.downloadLocationIsTemporary = (downloadLocation == NULL);

    UpdateAvailableSpace();
    Validate();
}

InstallationSettings* GetInstallationSettings()
{
    return &s_InstallationSettings;
}

InstallationSettings::InstallationSettings() :
    installLocation(NULL)
    , downloadLocation(NULL)
    , createdInstallLocation(false)
    , downloadLocationIsTemporary(true)
    , totalInstallationSizeGB(0)
    , availableSpaceGB(0)
{
}

extern "C"
{
    void OnDownloadLocationTypeToggled(GtkWidget* source, gpointer data)
    {
        s_InstallationSettings.downloadLocationIsTemporary = gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON(GetObject("downloadLocationTemporaryCheckbox")));
        if (s_InstallationSettings.downloadLocationIsTemporary)
        {
            if (s_InstallationSettings.downloadLocation)
                g_object_unref(s_InstallationSettings.downloadLocation);
            s_InstallationSettings.downloadLocation = NULL;
        }
        else
            s_InstallationSettings.downloadLocation = gtk_file_chooser_get_file(GTK_FILE_CHOOSER(GetObject("downloadLocationButton")));
        Validate();
    }

    void OnDownloadLocationChanged(GtkFileChooser* source, gpointer data)
    {
        if (s_InstallationSettings.downloadLocationIsTemporary)
            return;

        GFile *selectedLocation = gtk_file_chooser_get_file(source);
        if (selectedLocation && (!s_InstallationSettings.downloadLocation || !g_file_equal(s_InstallationSettings.downloadLocation, selectedLocation)))
        {
            if (s_InstallationSettings.downloadLocation)
                g_object_unref(s_InstallationSettings.downloadLocation);
            s_InstallationSettings.downloadLocation = selectedLocation;
        }
        Validate();
    }

    void OnInstallLocationChanged(GtkFileChooser* source, gpointer data)
    {
        GFile* selectedLocation = gtk_file_chooser_get_file(source);
        if (selectedLocation && !g_file_equal(s_InstallationSettings.installLocation, selectedLocation))
        {
            if (s_InstallationSettings.createdInstallLocation)
                g_file_trash(s_InstallationSettings.installLocation, NULL, NULL);
            if (s_InstallationSettings.installLocation)
                g_object_unref(s_InstallationSettings.installLocation);
            s_InstallationSettings.installLocation = selectedLocation;
            s_InstallationSettings.createdInstallLocation = false;
        }
        UpdateAvailableSpace();
        Validate();
    }
}
