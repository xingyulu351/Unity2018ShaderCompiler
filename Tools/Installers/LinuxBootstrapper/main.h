#pragma once

#include <gtk/gtk.h>

const size_t kGigabyte = 1024 * 1024 * 1024;

GObject* GetObject(const char* name);
void InitializeEverything();
void CleanupEverything(bool installationSucceeded);
void FlushEvents();
void Quit(bool success);
