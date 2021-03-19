#pragma once

#include "ComponentManager.h"

class ComponentDownloader
{
public:
    static void DoDownloadComponents();
    static void Cleanup();
    static bool DoneDownloading() { return doneDownloading; }

private:
    static gboolean DownloadAllComponents();
    static void ProgressUpdated(char* uri, size_t downloaded);
    static void AllDownloadsCompleted();
    static void BeginNextDownload();
    static void DownloadCompleted(char* uri, char* localFile, bool success);

    static size_t totalProgress;
    static size_t totalSize;
    static size_t currentDownload;
    static size_t currentSize;
    static double lastUpdate;
    static size_t downloadedSinceLastUpdate;
    static double downloadRate;
    static bool doneDownloading;
};
