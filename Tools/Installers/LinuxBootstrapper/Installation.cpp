#include "Installation.h"
#include "main.h"
#include "InstallationSetup.h"
#include "StateManager.h"

#include <archive.h>
#include <archive_entry.h>
#include <glib/gstdio.h>
#include <cstring>

size_t Installation::stepCount;
size_t Installation::currentStep;
bool Installation::shouldCancel;
std::map<char*, char*> Installation::osxPathMap;
std::map<char*, char*> Installation::osxTitleMap;

const char* kInstallLocationSentinel = "install-location=\"";
const char* kOSXDefaultInstallLocation = "/Applications/Unity";
const char* kPkgSuffix = ".pkg";
const char* kArchiveSuffix = ".tar.xz";
const int kStepsPerPackage = 2;

extern gboolean g_Unattended;
extern gboolean g_Verbose;

enum
{
    kAbort,
    kIgnore
};

static void SetProgressText(char* text)
{
    if (g_Unattended)
        printf("%s\n", text);
    else
        gtk_progress_bar_set_text(GTK_PROGRESS_BAR(GetObject("installingProgressBar")), text);
}

static char* GetRelativeInstallationPathFromPackageInfo(char* packageInfo)
{
    char* contents = NULL;
    char* start = NULL;
    char* end = NULL;
    char* path = NULL;
    gsize length = 0;

    if (!g_file_get_contents(packageInfo, &contents, &length, NULL))
        return NULL;

    start = strstr(contents, kInstallLocationSentinel);
    if (start)
    {
        start += strlen(kInstallLocationSentinel);
        end = strstr(start, "\"");
        if (end)
        {
            if (g_str_has_prefix(start, kOSXDefaultInstallLocation))
                start += strlen(kOSXDefaultInstallLocation) + 1;
            if (start < end)
                path = g_strndup(start, end - start);
        }
    }

    g_free(contents);
    return path;
}

void Installation::Cancel()
{
    shouldCancel = true;
}

void Installation::SetCurrentArchiveExtractionProgress(double fraction)
{
    if (g_Unattended)
        return;
    double stepFraction = 1 / (double)stepCount;
    double base = currentStep * stepFraction;
    gtk_progress_bar_set_fraction(GTK_PROGRESS_BAR(GetObject("installingProgressBar")), std::min(base + (fraction * stepFraction), 1.0));
    FlushEvents();
}

static bool VerifySupportedArchiveFormats(struct archive* archive)
{
    return (ARCHIVE_OK == archive_read_support_format_xar(archive) &&
        ARCHIVE_OK == archive_read_support_format_cpio(archive) &&
        ARCHIVE_OK == archive_read_support_format_tar(archive) &&
        ARCHIVE_OK == archive_read_support_filter_xz(archive) &&
        ARCHIVE_OK == archive_read_support_filter_gzip(archive));
}

static size_t CountArchiveEntries(char* archiveFile)
{
    size_t count = 0;
    struct archive* archive = archive_read_new();

    if (!VerifySupportedArchiveFormats(archive))
        return count;

    if (ARCHIVE_OK == archive_read_open_filename(archive, archiveFile, BUFSIZ))
    {
        struct archive_entry* entry = NULL;
        do
        {
            int result = archive_read_next_header(archive, &entry);
            if (ARCHIVE_OK != result && ARCHIVE_WARN != result)
                break;
            ++count;
        }
        while (true);

        archive_read_close(archive);
    }

    archive_read_free(archive);
    return count;
}

static void LogDetails(const char* format, ...)
{
    va_list args;
    va_start(args, format);
    char* message = g_strdup_vprintf(format, args);
    va_end(args);

    if (g_Unattended)
    {
        if (g_Verbose)
            printf("%s\n", message);
    }
    else
    {
        GtkTextBuffer *details = GTK_TEXT_BUFFER(GetObject("installationDetailsBuffer"));
        GtkTextIter end;
        gtk_text_buffer_get_end_iter(details, &end);
        gtk_text_buffer_insert(details, &end, message, -1);
        GtkAdjustment *adjustment = gtk_scrolled_window_get_vadjustment(
            GTK_SCROLLED_WINDOW(GetObject("installationDetailsScrollView")));
        gtk_adjustment_set_value(adjustment, gtk_adjustment_get_upper(adjustment));   // autoscroll to bottom
    }
    g_free(message);
}

std::vector<char*> Installation::ExtractArchive(char* archiveFile, char* destinationPath, bool logDetails)
{
    std::vector<char*> extractedFiles;

    // libarchive extracts into the current directory
    gchar* originalDirectory = g_get_current_dir();
    if (0 != g_mkdir_with_parents(destinationPath, 0755))
        return extractedFiles;

    g_chdir(destinationPath);

    size_t entryCount = CountArchiveEntries(archiveFile);

    struct archive* archive = archive_read_new();
    if (!VerifySupportedArchiveFormats(archive))
    {
        const char* errorString = "Fatal error: required formats (xar, cpio, tar, gzip, xz) are not supported by system libarchive\n";
        if (logDetails)
            LogDetails(errorString);
        printf(errorString);
        return extractedFiles;
    }

    if (ARCHIVE_OK != archive_read_open_filename(archive, archiveFile, BUFSIZ))
    {
        char* message = g_strdup_printf("Error opening archive %s: %s\n", archiveFile, archive_error_string(archive));
        if (logDetails)
            LogDetails("%s", message);
        printf("%s", message);
        g_free(message);
        return extractedFiles;
    }

    struct archive_entry* entry = NULL;
    bool eof = false;
    size_t count = 0;

    do
    {
        int result = archive_read_next_header(archive, &entry);
        switch (result)
        {
            case ARCHIVE_OK:
            case ARCHIVE_WARN:
                if (logDetails)
                    LogDetails("%s\n", archive_entry_pathname(entry));
                archive_read_extract(archive, entry, 0); // TODO: flags?
                extractedFiles.push_back(g_strdup(archive_entry_pathname(entry)));
                ++count;
                SetCurrentArchiveExtractionProgress(count / (double)entryCount);
                break;
            case ARCHIVE_EOF:
                eof = true;
                break;
            case ARCHIVE_RETRY:
            // TODO: retry
            case ARCHIVE_FATAL:
            default:
            {
                char* errorMessage = g_strdup_printf("Error reading archive %s: %s\n", archiveFile, archive_error_string(archive));
                if (logDetails)
                    LogDetails("%s", errorMessage);
                printf("%s", errorMessage);
                g_free(errorMessage);
                eof = true;
                extractedFiles.clear();
                break;
            }
        }
    }
    while (!eof && !shouldCancel);

    archive_read_close(archive);
    archive_read_free(archive);
    g_chdir(originalDirectory);
    g_free(originalDirectory);

    return extractedFiles;
}

char* Installation::RemapInstallationPath(char* originalPath)
{
    if (!originalPath || !*originalPath)
        return NULL;

    for (std::map<char*, char*>::const_iterator pair = osxPathMap.begin(); pair != osxPathMap.end(); ++pair)
    {
        if (g_str_has_prefix(originalPath, pair->first))
        {
            char *cursor = originalPath + strlen(pair->first);
            char *remappedPath = g_strdup_printf("%s%s", pair->second, cursor);
            return remappedPath;
        }
    }

    return NULL;
}

char* Installation::RemapTitle(char* componentTitle)
{
    if (!componentTitle || !*componentTitle)
        return NULL;

    for (std::map<char *, char *>::const_iterator pair = osxTitleMap.begin(); pair != osxTitleMap.end(); ++pair)
    {
        if (0 == strcmp(pair->first, componentTitle))
            return g_strdup(pair->second);
    }

    return NULL;
}

bool Installation::InstallComponentFromPkg(ComponentManager::Component* component)
{
    const InstallationSettings* settings = GetInstallationSettings();
    char* text = g_strdup_printf("Preparing %s", component->title);
    ++currentStep;
    SetProgressText(text);
    g_free(text);
    if (!g_Unattended)
        gtk_progress_bar_set_fraction(GTK_PROGRESS_BAR(GetObject("installingProgressBar")), std::min(currentStep / (double)stepCount, 1.0));
    FlushEvents();
    char* payload = NULL;
    char* packageInfo = NULL;
    char* basePackageExtractionDirectory = g_dir_make_tmp(NULL, NULL);
    bool success = false;

    LogDetails("Extracting %s\n========================\n", component->localFile);
    std::vector<char*> extractedFiles = ExtractArchive(component->localFile, basePackageExtractionDirectory, false);
    for (std::vector<char*>::iterator file = extractedFiles.begin(); file != extractedFiles.end(); ++file)
    {
        if (strstr(*file, "Payload"))
            payload = g_strdup_printf("%s/%s", basePackageExtractionDirectory, *file);
        if (strstr(*file, "PackageInfo"))
            packageInfo = g_strdup_printf("%s/%s", basePackageExtractionDirectory, *file);
        g_free(*file);
    }
    extractedFiles.clear();

    if (!payload)
        printf("Error: Missing payload in package\n");
    else if (!packageInfo)
        printf("Error: Missing package info in package\n");
    else
    {
        // scrape extraction path from package info
        gchar* baseInstallLocation = g_file_get_path(settings->installLocation);
        char* relativeInstallationPath = RemapTitle(component->title);

        if (!relativeInstallationPath)
        {
            relativeInstallationPath = GetRelativeInstallationPathFromPackageInfo(packageInfo);

            if (relativeInstallationPath)
            {
                char* remappedPath = RemapInstallationPath(relativeInstallationPath);
                if (remappedPath)
                {
                    g_free(relativeInstallationPath);
                    relativeInstallationPath = remappedPath;
                }
            }
        }

        gchar* installationPath = relativeInstallationPath ?
            g_strdup_printf("%s/%s", baseInstallLocation, relativeInstallationPath) :
            g_strdup(baseInstallLocation);

        ++currentStep;
        text = g_strdup_printf("Installing %s", component->title);
        SetProgressText(text);
        g_free(text);
        if (!g_Unattended)
            gtk_progress_bar_set_fraction(GTK_PROGRESS_BAR(GetObject("installingProgressBar")), std::min(currentStep / (double)stepCount, 1.0));
        FlushEvents();

        if (0 == g_mkdir_with_parents(installationPath, 0755))
        {
            // extract payload into extraction path
            extractedFiles = ExtractArchive(payload, installationPath, true);
            success = !extractedFiles.empty();
        }
        else
        {
            char* errorMessage = g_strdup_printf("Error creating extraction directory '%s'\n", installationPath);
            LogDetails("%s", errorMessage);
            printf("%s", errorMessage);
            g_free(errorMessage);
        }

        LogDetails("\n\n");

        // cleanup
        GFile* basePackageExtractionDirectoryFile = g_file_new_for_path(basePackageExtractionDirectory);
        g_file_trash(basePackageExtractionDirectoryFile, NULL, NULL);
        g_object_unref(basePackageExtractionDirectoryFile);
        g_free(basePackageExtractionDirectory);
        g_free(installationPath);
        g_free(baseInstallLocation);
        g_free(relativeInstallationPath);
        g_free(payload);
        g_free(packageInfo);
        for (std::vector<char*>::iterator file = extractedFiles.begin(); file != extractedFiles.end(); ++file)
            g_free(*file);
    }
    return success;
}

bool Installation::InstallComponentFromArchive(ComponentManager::Component* component)
{
    const InstallationSettings* settings = GetInstallationSettings();
    bool success = false;
    char* text = g_strdup_printf("Installing %s", component->title);
    SetProgressText(text);
    g_free(text);
    ++currentStep;
    if (!g_Unattended)
        gtk_progress_bar_set_fraction(GTK_PROGRESS_BAR(GetObject("installingProgressBar")), std::min(currentStep / (double)stepCount, 1.0));
    FlushEvents();

    LogDetails("Extracting %s\n========================\n", component->localFile);
    char* installationPath = g_file_get_path(settings->installLocation);
    std::vector<char*> extractedFiles;

    if (0 == g_mkdir_with_parents(installationPath, 0755))
    {
        extractedFiles = ExtractArchive(component->localFile, installationPath, true);
        success = !extractedFiles.empty();
    }
    else
    {
        char* errorMessage = g_strdup_printf("Error creating extraction directory '%s'\n", installationPath);
        LogDetails("%s", errorMessage);
        printf("%s", errorMessage);
        g_free(errorMessage);
    }
    LogDetails("\n\n");
    ++currentStep;

    // cleanup
    g_free(installationPath);
    for (std::vector<char*>::iterator file = extractedFiles.begin(); file != extractedFiles.end(); ++file)
        g_free(*file);
    return success;
}

gboolean Installation::InstallAllComponents()
{
    std::vector<ComponentManager::Component*>& components = ComponentManager::GetComponentsForInstall();
    stepCount = components.size() * kStepsPerPackage;
    for (std::vector<ComponentManager::Component*>::iterator i = components.begin(); i != components.end() && !shouldCancel; ++i)
    {
        bool result;

        if (!(*i)->downloaded)
        {
            printf("Skipping package %s\n", (*i)->title);
            currentStep += kStepsPerPackage;
            continue;
        }

        if (g_str_has_suffix((*i)->localFile, kPkgSuffix))
            result = InstallComponentFromPkg(*i);
        else if (g_str_has_suffix((*i)->localFile, kArchiveSuffix))
            result = InstallComponentFromArchive(*i);
        else
        {
            printf("Unsupported package type: %s\n", (*i)->localFile);
            result = true;
            currentStep += kStepsPerPackage;
        }

        if (!result)
        {
            if (g_Unattended)
            {
                printf("Installation failed, aborting\n");
                exit(1);
            }
            GtkDialog* errorDialog = GTK_DIALOG(gtk_dialog_new_with_buttons("Installation failed",
                GTK_WINDOW(GetObject("mainWindow")),
                (GtkDialogFlags)(GTK_DIALOG_MODAL | GTK_DIALOG_DESTROY_WITH_PARENT),
                GTK_STOCK_QUIT, kAbort,
                "Continue", kIgnore,
                NULL
            ));
            int response = gtk_dialog_run(errorDialog);

            switch (response)
            {
                case kAbort:
                    shouldCancel = true;
                    break;
                case kIgnore:
                default:
                    // pretend everything's ok, lalala
                    break;
            }
            gtk_widget_destroy(GTK_WIDGET(errorDialog));
        }
    }
    if (!shouldCancel && !g_Unattended)
        StateManager::NextState();
    return FALSE;
}

void Installation::DoInstallation()
{
    stepCount = currentStep = 0;
    shouldCancel = false;
    osxPathMap["/Users/Shared/Unity"] = "";
    osxPathMap["PlaybackEngines"] = "Editor/Data/PlaybackEngines";
    osxTitleMap["Documentation"] = "Editor/Data";
    if (!g_Unattended)
        gtk_text_buffer_set_text(GTK_TEXT_BUFFER(GetObject("installationDetailsBuffer")), "", 0);

    // Need the notebook page switch to finish
    g_idle_add((GSourceFunc)InstallAllComponents, NULL);
}

extern "C"
{
    void OnShowInstallationDetailsButtonToggled(GtkToggleButton* toggleButton, gpointer data)
    {
        gtk_widget_set_visible(GTK_WIDGET(GetObject("installationDetailsScrollView")), gtk_toggle_button_get_active(toggleButton));
    }
}
