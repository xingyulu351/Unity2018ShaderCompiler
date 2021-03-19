#pragma once

#include <gtk/gtk.h>
#include <glib.h>
#include <vector>

class ComponentManager
{
public:
    static void Initialize();
    static void Cleanup();
    static void DoComponentSelection();
    static bool IsReady();
    static void SelectComponents(char* componentList);

    struct Component
    {
        Component(bool install, char* name, char* title, char* description, char* url, char* md5, bool mandatory, size_t size, size_t installedsize, bool requires_unity);
        ~Component();
        bool install;
        char* name;
        char* title;
        char* description;
        char* url;
        char* md5;
        bool mandatory;
        size_t size;
        size_t installedsize;
        bool requires_unity;
        char* localFile;
        bool downloaded;
    };
    static std::vector<Component*>& GetComponentsForInstall();
    static std::vector<Component*> GetAllComponents();
    static void UpdateInstalledSize();

private:
    static void SettingsDownloadCompleted(char *uri, char *localFile, bool success);
    static void LoadComponents();
    static Component* GetComponentForRow(GtkTreeModel* model, GtkTreeIter* cursor);
    static Component* GetComponentForGroup(char* group);
    static void OnSelectionChanged(GtkTreeSelection* selection, gpointer data);
    static void RowToggled(GtkCellRendererToggle* toggle, gchar* path, gpointer data);
    static void SetRowInstallState(GtkListStore* componentList, GtkTreeIter* cursor, bool wantToInstallComponent, bool willInstallUnity);

    static std::vector<Component*> components;
    static int currentSettingsKeyIndex;
    static gchar** settingsKeys;
    static GKeyFile* settingsKeyfile;
    static char* baseUri;
    static GKeyFile* componentKeyfile;
};
