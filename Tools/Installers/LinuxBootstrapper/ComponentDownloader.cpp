#include "ComponentDownloader.h"
#include "InstallationSetup.h"
#include "ComponentManager.h"
#include "DownloadManager.h"
#include "StateManager.h"
#include "main.h"

#include <glib/gstdio.h>
#include <sys/time.h>
#include <cstdio>
#include <cstring>
#include <cstdlib>

size_t ComponentDownloader::totalProgress;
size_t ComponentDownloader::totalSize;
size_t ComponentDownloader::currentDownload;
size_t ComponentDownloader::currentSize;
double ComponentDownloader::lastUpdate;
size_t ComponentDownloader::downloadedSinceLastUpdate;
double ComponentDownloader::downloadRate;
bool ComponentDownloader::doneDownloading;

static bool userCancelled;
static const double kInvalidTime = -1;
static const int kMegabyte = 1024 * 1024;
extern gboolean g_Unattended;
extern char* g_ComponentListLoadLocation;

enum
{
    kAbort,
    kRetry,
    kIgnore
};

static void AbortDownloads()
{
    DownloadManager::CancelAllDownloads();
    StateManager::PreviousState();
}

static bool VerifyChecksumForFile(char* file, char* md5)
{
    if (!g_file_test(file, G_FILE_TEST_EXISTS))
        return false;
    unsigned char buffer[BUFSIZ];
    size_t read;
    bool matched = false;
    GChecksum* checksum = g_checksum_new(G_CHECKSUM_MD5);
    FILE* stream = fopen(file, "rb");
    if (!stream)
        return false;

    for (read = fread(buffer, 1, BUFSIZ, stream); read > 0; read = fread(buffer, 1, BUFSIZ, stream))
        g_checksum_update(checksum, buffer, read);
    fclose(stream);

    if (read == 0)
        // EOF reached without incident
        matched = (0 == strcmp(md5, g_checksum_get_string(checksum)));
    g_checksum_free(checksum);

    return matched;
}

static void SetProgressText(char* format, ...)
{
    va_list args;
    va_start(args, format);
    char* text = g_strdup_vprintf(format, args);
    va_end(args);

    if (g_Unattended)
        printf("%s\n", text);
    else
        gtk_progress_bar_set_text(GTK_PROGRESS_BAR(GetObject("currentDownloadProgressBar")), text);
    g_free(text);
    FlushEvents();
}

void ComponentDownloader::ProgressUpdated(char* uri, size_t downloaded)
{
    if (g_Unattended)
        return;

    struct timeval time;
    gettimeofday(&time, NULL);
    double currentUpdate = time.tv_sec + (((double)time.tv_usec) / 1000000);

    if (lastUpdate == kInvalidTime)
        lastUpdate = currentUpdate;
    if (currentUpdate - lastUpdate > 0.25)
    {
        // Only update once per second
        downloadRate = (downloaded - downloadedSinceLastUpdate) / (currentUpdate - lastUpdate);
        lastUpdate = currentUpdate;
        downloadedSinceLastUpdate = downloaded;
    }

    GtkProgressBar* currentProgressBar = GTK_PROGRESS_BAR(GetObject("currentDownloadProgressBar"));
    if (currentSize)
        gtk_progress_bar_set_fraction(currentProgressBar, std::min(downloaded / (double)currentSize, 1.0));
    else
        gtk_progress_bar_pulse(currentProgressBar);

    GtkProgressBar* totalProgressBar = GTK_PROGRESS_BAR(GetObject("overallDownloadProgressBar"));
    size_t totalDownloaded = downloaded + totalProgress;
    double fraction = std::min(totalDownloaded / (double)totalSize, 1.0);
    // TODO: time remaining
    char* text = g_strdup_printf("%zu kB (%d%%) of %zu kB @ %0.2f MB/s", totalDownloaded / 1024, (int)(fraction * 100), totalSize / 1024, downloadRate / kMegabyte);
    gtk_progress_bar_set_fraction(totalProgressBar, fraction);
    gtk_progress_bar_set_text(totalProgressBar, text);
    g_free(text);
}

void ComponentDownloader::AllDownloadsCompleted()
{
    doneDownloading = true;
    if (!g_Unattended)
        // Use idle handler for this because, if all components already exist locally,
        // this can be called before we're fully done switching to _this_ state
        g_idle_add((GSourceFunc)StateManager::NextState, NULL);
}

void ComponentDownloader::DownloadCompleted(char* uri, char* localFile, bool success)
{
    if (userCancelled)
        return;

    if (!success)
    {
        char* message = g_strdup_printf("Error downloading '%s' to '%s'", uri, localFile);
        printf("%s\n", message);
        if (g_Unattended)
            exit(1);

        GtkDialog* errorDialog = GTK_DIALOG(gtk_dialog_new_with_buttons("Download failed",
            GTK_WINDOW(GetObject("mainWindow")),
            (GtkDialogFlags)(GTK_DIALOG_MODAL | GTK_DIALOG_DESTROY_WITH_PARENT),
            GTK_STOCK_QUIT, kAbort,
            "Retry", kRetry,
            "Continue", kIgnore,
            NULL
        ));
        int response = gtk_dialog_run(errorDialog);
        bool shouldBeginNextDownload = false;
        switch (response)
        {
            case kAbort:
                AbortDownloads();
                break;
            case kRetry:
                shouldBeginNextDownload = true;
                break;
            case kIgnore:
            default:
                ++currentDownload;
                shouldBeginNextDownload = true;
                break;
        }
        g_free(message);
        gtk_widget_destroy(GTK_WIDGET(errorDialog));
        if (shouldBeginNextDownload)
            BeginNextDownload();
        return;
    }
    printf("Finished downloading '%s' to '%s'\n", uri, localFile);
    ComponentManager::GetComponentsForInstall()[currentDownload]->downloaded = true;
    ++currentDownload;
    totalProgress += currentSize;
    BeginNextDownload();
}

static void BeginFileDownload(char* title, char* url, char* localFile, char* expectedMD5, DownloadManager::DownloadCompletedCallback onCompletion, DownloadManager::DownloadProgressCallback onProgressUpdate)
{
    SetProgressText("Downloading %s", title);
    DownloadManager::DownloadFile(url, localFile, expectedMD5, onCompletion, onProgressUpdate);
}

void ComponentDownloader::BeginNextDownload()
{
    lastUpdate = kInvalidTime;
    downloadRate = 0;
    downloadedSinceLastUpdate = 0;
    std::vector<ComponentManager::Component*>& components = ComponentManager::GetComponentsForInstall();
    if (currentDownload >= components.size())
    {
        AllDownloadsCompleted();
        return;
    }

    ComponentManager::Component* component = components[currentDownload];
    currentSize = component->size;
    ProgressUpdated(component->url, 0);
    FlushEvents();

    const InstallationSettings* settings = GetInstallationSettings();
    char* basename = g_path_get_basename(component->url);

    if (settings->downloadLocationIsTemporary)
    {
        component->localFile = g_strdup_printf("%s/%s", g_get_tmp_dir(), basename);
        BeginFileDownload(component->title, component->url, component->localFile, component->md5, (DownloadManager::DownloadCompletedCallback)DownloadCompleted, ProgressUpdated);
    }
    else
    {
        GFile* downloadFile = g_file_get_child(settings->downloadLocation, basename);
        component->localFile = g_file_get_path(downloadFile);
        g_object_unref(downloadFile);

        SetProgressText("Verifying %s", component->title);
        if (VerifyChecksumForFile(component->localFile, component->md5))
        {
            printf("Using existing '%s'\n", component->localFile);
            DownloadCompleted(component->url, component->localFile, true);
        }
        else if (g_ComponentListLoadLocation)
        {
            printf("Checksum failed for '%s', skipping\n", component->localFile);
            DownloadCompleted(component->url, component->localFile, false);
        }
        else
            BeginFileDownload(component->title, component->url, component->localFile, component->md5, (DownloadManager::DownloadCompletedCallback)DownloadCompleted, ProgressUpdated);
    }

    g_free(basename);
}

void ComponentDownloader::DoDownloadComponents()
{
    g_idle_add((GSourceFunc)DownloadAllComponents, G_PRIORITY_DEFAULT);
}

gboolean ComponentDownloader::DownloadAllComponents()
{
    userCancelled = false;
    doneDownloading = false;
    std::vector<ComponentManager::Component*>& components = ComponentManager::GetComponentsForInstall();
    totalSize = totalProgress = currentSize = currentDownload = 0;

    for (std::vector<ComponentManager::Component*>::iterator i = components.begin(); i != components.end(); ++i)
        totalSize += (*i)->size;

    if (!g_Unattended)
    {
        gtk_widget_set_sensitive(GTK_WIDGET(GetObject("forwardButton")), FALSE);
        gtk_widget_set_sensitive(GTK_WIDGET(GetObject("backButton")), FALSE);
    }
    BeginNextDownload();
    return FALSE;
}

void ComponentDownloader::Cleanup()
{
    std::vector<ComponentManager::Component*>& components = ComponentManager::GetComponentsForInstall();
    if (GetInstallationSettings()->downloadLocationIsTemporary)
        for (std::vector<ComponentManager::Component*>::iterator i = components.begin(); i != components.end(); ++i)
            if ((*i)->localFile)
                g_remove((*i)->localFile);
}

extern "C"
{
    void OnCancelDownloadClicked(GtkButton* cancelDownloadButton, gpointer data)
    {
        userCancelled = true;
        AbortDownloads();
    }
}
