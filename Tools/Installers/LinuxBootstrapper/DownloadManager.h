#pragma once

#include <libsoup/soup.h>
#include <glib.h>

class DownloadManager
{
public:
    typedef void (*DownloadCompletedCallback)(char* uri, char* localFile, bool success);
    typedef void (*DownloadProgressCallback)(char* uri, size_t downloaded);
    static void Initialize();
    static void Cleanup();
    static void DownloadFile(char* uri, char* localFile, char* md5, DownloadCompletedCallback completedCallback, DownloadProgressCallback progressCallback);
    static void CancelAllDownloads();
    static bool ShouldCancelDownloads() { return cancelDownloads; }

    struct Download
    {
        Download(char* uri, char* localFile, char* md5, DownloadCompletedCallback completedCallback, DownloadProgressCallback progressCallback);
        ~Download();
        void WriteChunk(const char* data, gsize length);
        bool Finalize(bool success);

        char* uri;
        char* localFile;
        char* temporaryFile;
        char* expectedChecksum;
        gint temporaryFileHandle;
        DownloadCompletedCallback completedCallback;
        DownloadProgressCallback progressCallback;
        size_t downloaded;
        GChecksum* checksum;
    };
    static void CleanupDownload(SoupMessage* message, Download* download);

private:
    static SoupSession* s_Session;
    static bool cancelDownloads;
};
