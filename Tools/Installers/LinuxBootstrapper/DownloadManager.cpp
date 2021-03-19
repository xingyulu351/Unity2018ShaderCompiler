#include "DownloadManager.h"

#include <cstdio>
#include <cstring>
#include <glib/gstdio.h>
#include <errno.h>

SoupSession* DownloadManager::s_Session = NULL;
bool DownloadManager::cancelDownloads = false;

const char* kGet = "GET";
const gint kInvalidFileHandle = -1;

void DownloadManager::Initialize()
{
    s_Session = soup_session_async_new_with_options("use-thread-context", TRUE, NULL);
}

void DownloadManager::Cleanup()
{
    if (s_Session)
        soup_session_abort(s_Session);
}

DownloadManager::Download::Download(char* aUri, char* aFile, char* md5, DownloadCompletedCallback aCompletedCallback, DownloadProgressCallback aProgressCallback) :
    uri(aUri)
    , localFile(aFile)
    , temporaryFile(NULL)
    , expectedChecksum(md5)
    , temporaryFileHandle(kInvalidFileHandle)
    , completedCallback(aCompletedCallback)
    , progressCallback(aProgressCallback)
    , downloaded(0)
    , checksum(g_checksum_new(G_CHECKSUM_MD5))
{
}

DownloadManager::Download::~Download()
{
    if (temporaryFile)
        g_free(temporaryFile);
    if (temporaryFileHandle != kInvalidFileHandle)
        close(temporaryFileHandle);
    g_checksum_free(checksum);
}

void DownloadManager::Download::WriteChunk(const char *data, gsize length)
{
    gsize remaining = length;
    gsize written = 0;
    do
    {
        written = write(temporaryFileHandle, data, remaining);
        if (written < 0)
        {
            printf("Error writing temporary file: %s\n", strerror(errno));
            return;
        }
        remaining -= written;
    }
    while (remaining > 0);
    downloaded += length;
    g_checksum_update(checksum, (const guchar*)data, length);
    if (progressCallback)
        progressCallback(uri, downloaded);
}

bool DownloadManager::Download::Finalize(bool success)
{
    if (temporaryFileHandle == kInvalidFileHandle || !temporaryFile)
        return false;
    close(temporaryFileHandle);
    temporaryFileHandle = kInvalidFileHandle;
    if (success)
    {
        // GFile api for reliable move between different filesystems
        GFile* source = g_file_new_for_path(temporaryFile);
        GFile* destination = g_file_new_for_path(localFile);
        bool success = (FALSE != g_file_move(source, destination, G_FILE_COPY_OVERWRITE, NULL, NULL, NULL, NULL));
        g_object_unref(source);
        g_object_unref(destination);
        return success;
    }
    else
        return (0 == g_remove(temporaryFile));
}

void DownloadManager::CleanupDownload(SoupMessage* message, DownloadManager::Download* download)
{
    if (message->status_code == SOUP_STATUS_CANCELLED)
        // Already cancelled
        return;
    // This calls the DownloadCompleted callback
    soup_session_cancel_message(s_Session, message, SOUP_STATUS_CANCELLED);
    download->Finalize(false);
    download->completedCallback(download->uri, download->localFile, false);
    delete download;
}

static void GotDownloadHeaders(SoupMessage* message, DownloadManager::Download* download)
{
    bool success = false;
    if (SOUP_STATUS_IS_SUCCESSFUL(message->status_code))
        success = true;
    else if (SOUP_STATUS_IS_REDIRECTION(message->status_code))
    {
        success = true;
        SoupMessageHeaders* headers;
        g_object_get(message, SOUP_MESSAGE_RESPONSE_HEADERS, &headers, NULL);
        const char* redirectLocation = soup_message_headers_get_one(headers, "Location");
        printf("Redirecting to %s\n", redirectLocation ? redirectLocation : "(Unknown)");
    }

    if (!success)
    {
        printf("Error %d getting '%s'\n", message->status_code, download->uri);
        DownloadManager::CleanupDownload(message, download);
        return;
    }
    if (DownloadManager::ShouldCancelDownloads())
    {
        printf("Download of '%s' cancelled by user\n", download->uri);
        DownloadManager::CleanupDownload(message, download);
        return;
    }

    download->temporaryFileHandle = g_file_open_tmp(NULL, &download->temporaryFile, NULL);
}

static void GotDownloadChunk(SoupMessage* message, SoupBuffer* buffer, DownloadManager::Download* download)
{
    if (DownloadManager::ShouldCancelDownloads())
    {
        printf("Download of '%s' cancelled by user\n", download->uri);
        DownloadManager::CleanupDownload(message, download);
        return;
    }
    download->WriteChunk(buffer->data, buffer->length);
}

static void DownloadCompleted(SoupSession* session, SoupMessage* message, DownloadManager::Download* download)
{
    if (DownloadManager::ShouldCancelDownloads() || message->status_code == SOUP_STATUS_CANCELLED)
    {
        printf("Download of '%s' failed\n", download->uri);
        DownloadManager::CleanupDownload(message, download);
        return;
    }
    const gchar* checksumString = g_checksum_get_string(download->checksum);
    if (download->expectedChecksum && strcmp(checksumString, download->expectedChecksum))
        printf("Checksum for downloaded '%s' (%s) doesn't match expectation (%s)", download->localFile, checksumString, download->expectedChecksum);
    bool success = SOUP_STATUS_IS_SUCCESSFUL(message->status_code) &&
        (!download->expectedChecksum || (0 == strcmp(download->expectedChecksum, checksumString)));
    success = download->Finalize(success);
    download->completedCallback(download->uri, download->localFile, success);
    delete download;
}

void DownloadManager::DownloadFile(char* uri, char* localFile, char* md5, DownloadCompletedCallback completedCallback, DownloadProgressCallback progressCallback)
{
    cancelDownloads = false;
    Download* download = new Download(uri, localFile, md5, completedCallback, progressCallback);
    SoupMessage* message = soup_message_new(kGet, uri);
    soup_message_set_flags(message, SOUP_MESSAGE_OVERWRITE_CHUNKS);
    g_signal_connect(G_OBJECT(message), "got-headers", (GCallback)GotDownloadHeaders, download);
    g_signal_connect(G_OBJECT(message), "got-chunk", (GCallback)GotDownloadChunk, download);
    soup_session_queue_message(s_Session, message, (SoupSessionCallback)DownloadCompleted, download);
}

void DownloadManager::CancelAllDownloads()
{
    cancelDownloads = true;
}
