#include "ComponentManager.h"
#include "DownloadManager.h"
#include "main.h"
#include "InstallationSetup.h"
#include "artifacts/generated/LinuxBootstrapper/settings.ini.h"

#include <glib/gstdio.h>
#include <cstring>
#include <cstdlib>

const char* kSettingsGroup = "settings";
const gint kInvalidHandle = -1;
const char* kInstallKey = "install";
const char* kTitleKey = "title";
const char* kDescriptionKey = "description";
const char* kInstalledSizeKey = "installedsize";
const char* kFirstPath = "0";
const int kInstallColumn = 0;
const int kTitleColumn = 1;

const char* kDefaultDescription = "Select a component to see its description.";
const char* kMultiselectDescription = "Multiple components selected.";
const char* kInstallSizePrefix = "Install space required";

extern char* g_Inifile;
extern gboolean g_Unattended;
extern char* g_ComponentListSaveLocation;
extern char* g_ComponentListLoadLocation;

int ComponentManager::currentSettingsKeyIndex;
gchar** ComponentManager::settingsKeys;
GKeyFile* ComponentManager::settingsKeyfile;
char* ComponentManager::baseUri;
GKeyFile* ComponentManager::componentKeyfile = NULL;
std::vector<ComponentManager::Component*> ComponentManager::components;

void ComponentManager::SettingsDownloadCompleted(char *uri, char *localFile, bool success)
{
    if (success)
    {
        printf("Successfully downloaded %s => %s\n", uri, localFile);
        baseUri = g_strdup(uri);
        g_free(uri);
        char* lastSeparator = strrchr(baseUri, '/');
        if (lastSeparator)
            *lastSeparator = '\0';
        componentKeyfile = g_key_file_new();
        if (g_key_file_load_from_file(componentKeyfile, localFile, G_KEY_FILE_NONE, NULL))
        {
            g_remove(localFile);
            g_free(localFile);
            g_strfreev(settingsKeys);
            g_key_file_free(settingsKeyfile);
            if (g_ComponentListSaveLocation)
            {
                gsize dataLength;
                gchar* data = g_key_file_to_data(componentKeyfile, &dataLength, NULL);
                g_file_set_contents(g_ComponentListSaveLocation, data, dataLength, NULL);
                g_free(data);
            }
            if (!g_Unattended)
                LoadComponents();
            return;
        }
        g_key_file_free(componentKeyfile);
        componentKeyfile = NULL;
    }

    ++currentSettingsKeyIndex;
    if (!settingsKeys[currentSettingsKeyIndex])
    {
        printf("Unable to download any valid component lists, aborting\n");
        Quit(false);
    }
    DownloadManager::DownloadFile(g_key_file_get_value(settingsKeyfile, kSettingsGroup, settingsKeys[currentSettingsKeyIndex], NULL), localFile, NULL, SettingsDownloadCompleted, NULL);
}

bool ComponentManager::IsReady()
{
    return (componentKeyfile != NULL);
}

void ComponentManager::Initialize()
{
    settingsKeyfile = g_key_file_new();
    gsize keyCount;
    gchar* componentListFile = NULL;

    if (g_Inifile)
    {
        if (!g_key_file_load_from_file(settingsKeyfile, g_Inifile, G_KEY_FILE_NONE, NULL) ||
            !(settingsKeys = g_key_file_get_keys(settingsKeyfile, kSettingsGroup, &keyCount, NULL)) ||
            !keyCount)
        {
            printf("Error loading settings, aborting\n");
            Quit(false);
        }
    }
    else if (g_ComponentListLoadLocation)
    {
        componentKeyfile = g_key_file_new();
        if (!g_key_file_load_from_file(componentKeyfile, g_ComponentListLoadLocation, G_KEY_FILE_NONE, NULL))
        {
            printf("Error loading components from '%s', aborting\n", g_ComponentListLoadLocation);
            Quit(false);
        }
        baseUri = g_strdup("");
        if (!g_Unattended)
            LoadComponents();
        return;
    }
    else if (!g_key_file_load_from_data(settingsKeyfile, g_SettingsIni, strlen(g_SettingsIni), G_KEY_FILE_NONE, NULL) ||
             !(settingsKeys = g_key_file_get_keys(settingsKeyfile, kSettingsGroup, &keyCount, NULL)) ||
             !keyCount)
    {
        printf("Error loading settings, aborting\n");
        Quit(false);
        return;
    }

    gint handle = g_file_open_tmp(NULL, &componentListFile, NULL);
    if (handle != kInvalidHandle)
        close(handle);
    currentSettingsKeyIndex = 0;
    DownloadManager::DownloadFile(g_key_file_get_value(settingsKeyfile, kSettingsGroup, settingsKeys[0], NULL), componentListFile, NULL, SettingsDownloadCompleted, NULL);
}

void ComponentManager::Cleanup()
{
    for (std::vector<Component*>::iterator i = components.begin(); i != components.end(); ++i)
        delete *i;
    components.clear();
}

void ComponentManager::DoComponentSelection()
{
    for (std::vector<Component*>::iterator i = components.begin(); i != components.end(); ++i)
        delete *i;
    components.clear();
    if (!g_Unattended)
        gtk_widget_set_sensitive(GTK_WIDGET(GetObject("forwardButton")), IsReady() ? TRUE : FALSE);
}

static char* GetGroupWithTitle(GKeyFile* keyfile, const char* title)
{
    gsize groupCount;
    gchar** groups = g_key_file_get_groups(keyfile, &groupCount);
    gchar* group = NULL;
    bool found = false;
    if (!groups || !groupCount)
        return group;

    for (gsize i = 0; i < groupCount && !found; ++i)
    {
        gchar* groupTitle = g_key_file_get_value(keyfile, groups[i], "title", NULL);
        found = (0 == strcmp(title, groupTitle));
        g_free(groupTitle);
        if (found)
            group = g_strdup(groups[i]);
    }

    g_strfreev(groups);
    return group;
}

static size_t GetSizeValue(GKeyFile* keyfile, char* group, char* key)
{
    char* value = g_key_file_get_value(keyfile, group, key, NULL);
    size_t sizeValue = value ? strtoull(value, NULL, 10) : 0;
    g_free(value);
    return sizeValue;
}

char* GetTitleForRow(GtkTreeModel* componentList, GtkTreeIter* cursor)
{
    GValue value = G_VALUE_INIT;
    char* title = NULL;
    gtk_tree_model_get_value(componentList, cursor, kTitleColumn, &value);
    title = g_value_dup_string(&value);
    g_value_unset(&value);
    return title;
}

gboolean RowIsChecked(GtkTreeModel* componentList, GtkTreeIter* cursor)
{
    GValue value = G_VALUE_INIT;
    gtk_tree_model_get_value(componentList, cursor, kInstallColumn, &value);
    gboolean checked = g_value_get_boolean(&value);
    g_value_unset(&value);
    return checked;
}

ComponentManager::Component* ComponentManager::GetComponentForGroup(char* group)
{
    char* title = g_key_file_get_value(componentKeyfile, group, "title", NULL);
    char* relativeUrl = g_key_file_get_value(componentKeyfile, group, "url", NULL);
    char* url = g_strdup_printf("%s/%s", baseUri, relativeUrl);
    char* description = g_key_file_get_value(componentKeyfile, group, "description", NULL);
    char* md5 = g_key_file_get_value(componentKeyfile, group, "md5", NULL);

    Component* component = new Component(
        true,
        group,
        title,
        description,
        url,
        md5,
        (bool)g_key_file_get_boolean(componentKeyfile, group, "mandatory", NULL),
        GetSizeValue(componentKeyfile, group, "size"),
        GetSizeValue(componentKeyfile, group, "installedsize"),
        (bool)g_key_file_get_boolean(componentKeyfile, group, "requires_unity", NULL)
    );

    g_free(title);
    g_free(relativeUrl);
    g_free(url);
    g_free(description);
    g_free(md5);

    return component;
}

ComponentManager::Component* ComponentManager::GetComponentForRow(GtkTreeModel* componentList, GtkTreeIter* cursor)
{
    char* title = GetTitleForRow(componentList, cursor);
    char* group = GetGroupWithTitle(componentKeyfile, title);
    Component* component = GetComponentForGroup(group);
    g_free(group);
    g_free(title);
    return component;
}

ComponentManager::Component::Component(bool toInstall, char* aName, char* aTitle, char* aDescription, char* aUrl, char* aMd5, bool isMandatory, size_t aSize, size_t anInstalledSize, bool requiresUnity) :
    install(toInstall)
    , mandatory(isMandatory)
    , size(aSize)
    , installedsize(anInstalledSize)
    , requires_unity(requiresUnity)
    , localFile(NULL)
    , downloaded(false)
{
    name = g_strdup(aName);
    title = g_strdup(aTitle);
    description = g_strdup(aDescription);
    url = g_strdup(aUrl);
    md5 = g_strdup(aMd5);
}

ComponentManager::Component::~Component()
{
    g_free(name);
    g_free(title);
    g_free(description);
    g_free(url);
    g_free(md5);
    g_free(localFile);
}

std::vector<ComponentManager::Component*> ComponentManager::GetAllComponents()
{
    std::vector<Component*> tmp;
    gchar **groups = g_key_file_get_groups(componentKeyfile, NULL);
    for (gchar **group = groups; *group; ++group)
    {
        Component* component = GetComponentForGroup(*group);
        tmp.push_back(component);
    }
    return tmp;
}

std::vector<ComponentManager::Component*>& ComponentManager::GetComponentsForInstall()
{
    if (!components.empty() || g_Unattended)
        return components;
    GtkTreeModel* componentList = GTK_TREE_MODEL(GetObject("componentListStore"));
    GtkTreeIter cursor;

    for (gboolean done = !gtk_tree_model_get_iter_first(componentList, &cursor); !done; done = !gtk_tree_model_iter_next(componentList, &cursor))
    {
        if (RowIsChecked(componentList, &cursor))
            components.push_back(GetComponentForRow(componentList, &cursor));
    }

    return components;
}

void ComponentManager::UpdateInstalledSize()
{
    GtkTreeModel* componentList = GTK_TREE_MODEL(GetObject("componentListStore"));
    GtkTreeIter cursor;
    double totalInstalledSize = 0;

    for (gboolean done = !gtk_tree_model_get_iter_first(componentList, &cursor); !done; done = !gtk_tree_model_iter_next(componentList, &cursor))
    {
        if (!RowIsChecked(componentList, &cursor))
            continue;

        char *title = GetTitleForRow(componentList, &cursor);
        char *group = GetGroupWithTitle(componentKeyfile, title);
        totalInstalledSize += GetSizeValue(componentKeyfile, group, (char*)kInstalledSizeKey) / (double)kGigabyte;
        g_free(title);
        g_free(group);
    }

    gchar* installSpace = g_strdup_printf("%s: %0.2f GB", kInstallSizePrefix, totalInstalledSize);
    gtk_label_set_text(GTK_LABEL(GetObject("componentSizeLabel")), installSpace);
    GetInstallationSettings()->totalInstallationSizeGB = totalInstalledSize;
    g_free(installSpace);
}

void ComponentManager::SetRowInstallState(GtkListStore* componentList, GtkTreeIter* cursor, bool wantToInstallComponent, bool willInstallUnity)
{
    Component* component = GetComponentForRow(GTK_TREE_MODEL(componentList), cursor);
    wantToInstallComponent = component->mandatory || (wantToInstallComponent && (!component->requires_unity || willInstallUnity));
    gtk_list_store_set(componentList, cursor, kInstallColumn, wantToInstallComponent, -1);
    delete component;
}

void ComponentManager::RowToggled(GtkCellRendererToggle* toggle, gchar* path, gpointer data)
{
    // We need to manually update the data model when the user toggles the checkbox
    GtkListStore* componentList = GTK_LIST_STORE(GetObject("componentListStore"));
    GtkTreeIter cursor;
    gboolean shouldSelect = gtk_cell_renderer_toggle_get_active(toggle) ? FALSE : TRUE;
    GtkTreeSelection* selection = gtk_tree_view_get_selection(GTK_TREE_VIEW(GetObject("componentList")));
    GList* selectedPaths = gtk_tree_selection_get_selected_rows(selection, (GtkTreeModel**)&componentList);
    bool unityToggled = false;
    bool shouldInstallUnity = false;
    bool toggledItemIsInSelection = false;

    if (0 == strcmp(kFirstPath, path))
    {
        // Unity is the toggled item
        unityToggled = true;
        shouldInstallUnity = shouldSelect;
    }
    else
    {
        // Look up the stored value for Unity
        GtkTreeIter first;
        gtk_tree_model_get_iter_first(GTK_TREE_MODEL(componentList), &first);
        shouldInstallUnity = RowIsChecked(GTK_TREE_MODEL(componentList), &first);
    }

    if (selectedPaths)
    {
        // Multiselection
        bool unityIsInSelection = unityToggled;

        for (GList* i = g_list_first(selectedPaths); i; i = g_list_next(i))
        {
            gchar* selectionPath = gtk_tree_path_to_string((GtkTreePath*)i->data);
            if (!strcmp(path, selectionPath))
                toggledItemIsInSelection = true;
            if (!strcmp(kFirstPath, selectionPath))
                unityIsInSelection = true;
        }

        if (unityIsInSelection && toggledItemIsInSelection)
        {
            // Unity is in the toggled selection, override the stored value
            shouldInstallUnity = shouldSelect;
            unityToggled = true;
        }

        if (toggledItemIsInSelection)
        {
            // Set the state of each selected item
            for (GList* i = g_list_first(selectedPaths); i; i = g_list_next(i))
            {
                gtk_tree_model_get_iter(GTK_TREE_MODEL(componentList), &cursor, (GtkTreePath*)i->data);
                SetRowInstallState(componentList, &cursor, shouldSelect, shouldInstallUnity);
            }
        }
        g_list_foreach(selectedPaths, (GFunc)gtk_tree_path_free, NULL);
        g_list_free(selectedPaths);
    }

    if (!toggledItemIsInSelection)
    {
        // Single item outside selection was toggled
        gtk_tree_model_get_iter_from_string(GTK_TREE_MODEL(componentList), &cursor, path);
        SetRowInstallState(componentList, &cursor, shouldSelect, shouldInstallUnity);
    }

    if (unityToggled)
    {
        // "should install unity" value changed, need to update other packages
        gtk_tree_model_get_iter_first(GTK_TREE_MODEL(componentList), &cursor);
        while (gtk_tree_model_iter_next(GTK_TREE_MODEL(componentList), &cursor))
            SetRowInstallState(componentList, &cursor, RowIsChecked(GTK_TREE_MODEL(componentList), &cursor), shouldInstallUnity);
    }

    UpdateInstalledSize();
}

void ComponentManager::LoadComponents()
{
    GtkTreeView* componentView = GTK_TREE_VIEW(GetObject("componentList"));
    GtkListStore* componentList = GTK_LIST_STORE(gtk_tree_view_get_model(componentView));
    GtkTreeIter cursor;
    gsize groupCount;
    gchar** groups = g_key_file_get_groups(componentKeyfile, &groupCount);

    if (!groups || !groupCount)
        return;

    // We need to set the columns up manually because glade doesn't do it for us :'-(
    GtkCellRendererToggle* toggleRenderer = GTK_CELL_RENDERER_TOGGLE(gtk_cell_renderer_toggle_new());
    g_signal_connect(G_OBJECT(toggleRenderer), "toggled", (GCallback)ComponentManager::RowToggled, NULL);
    gtk_cell_renderer_toggle_set_activatable(toggleRenderer, TRUE);
    gtk_tree_view_insert_column_with_attributes(componentView, kInstallColumn, "Install", GTK_CELL_RENDERER(toggleRenderer), "active", 0, NULL);
    gtk_tree_view_insert_column_with_attributes(componentView, kTitleColumn, "Title", gtk_cell_renderer_text_new(), "text", 1, NULL);
    GtkTreeSelection* selection = gtk_tree_view_get_selection(componentView);
    gtk_tree_selection_set_mode(selection, GTK_SELECTION_MULTIPLE);
    g_signal_connect(G_OBJECT(selection), "changed", (GCallback)OnSelectionChanged, NULL);
    gtk_tree_view_set_rubber_banding(componentView, TRUE);

    for (gsize i = 0; i < groupCount; ++i)
    {
        gchar* title = g_key_file_get_value(componentKeyfile, groups[i], "title", NULL);
        gtk_list_store_append(componentList, &cursor);
        gtk_list_store_set(componentList, &cursor,
            kInstallColumn, g_key_file_get_boolean(componentKeyfile, groups[i], "install", NULL),
            kTitleColumn, title,
            -1);
        g_free(title);
    }

    g_strfreev(groups);

    UpdateInstalledSize();

    gtk_widget_set_sensitive(GTK_WIDGET(GetObject("forwardButton")), TRUE);
}

void ComponentManager::SelectComponents(char* componentList)
{
    bool unknownComponentsRequested = false;
    gchar **groups = g_key_file_get_groups(componentKeyfile, NULL);
    double totalInstalledSize = 0;

    if (componentList && 0 == strcasecmp(componentList, "all"))
    {
        // Select all components
        for (gchar **group = groups; *group; ++group)
        {
            printf("Selecting %s\n", *group);
            Component* component = GetComponentForGroup(*group);
            totalInstalledSize += component->installedsize / (double)kGigabyte;
            components.push_back(component);
        }
    }
    else if (componentList)
    {
        // Select user-specified components
        gchar** componentTokens = g_strsplit(componentList, ",", -1);
        for (gchar** group = componentTokens; *group; ++group)
        {
            if (g_key_file_has_group(componentKeyfile, *group))
            {
                printf("Selecting %s\n", *group);
                Component* component = GetComponentForGroup(*group);
                totalInstalledSize += component->installedsize / (double)kGigabyte;
                components.push_back(component);
            }
            else
            {
                unknownComponentsRequested = true;
                printf("Unknown component '%s' requested\n", *group);
            }
        }
        g_strfreev(componentTokens);

        if (unknownComponentsRequested)
        {
            gchar* joinedGroups = g_strjoinv(",", groups);
            printf("Available components: %s\n", joinedGroups);
            g_free(joinedGroups);
        }
    }
    else
    {
        // Select default components
        for (gchar **group = groups; *group; ++group)
        {
            if (g_key_file_get_boolean(componentKeyfile, *group, "install", NULL))
            {
                printf("Selecting %s\n", *group);
                Component* component = GetComponentForGroup(*group);
                totalInstalledSize += component->installedsize / (double)kGigabyte;
                components.push_back(component);
            }
        }
    }

    GetInstallationSettings()->totalInstallationSizeGB = totalInstalledSize;
    g_strfreev(groups);
}

void ComponentManager::OnSelectionChanged(GtkTreeSelection* selection, gpointer data)
{
    gint count = gtk_tree_selection_count_selected_rows(selection);

    // Update description
    switch (count)
    {
        case 0:
            gtk_label_set_text(GTK_LABEL(GetObject("componentDescriptionLabel")), kDefaultDescription);
            break;
        case 1:
        {
            GtkTreeModel* componentList = GTK_TREE_MODEL(GetObject("componentListStore"));
            GtkTreeIter cursor;
            GList* selected = gtk_tree_selection_get_selected_rows(selection, &componentList);
            gtk_tree_model_get_iter(componentList, &cursor, (GtkTreePath*)selected->data);
            char* title = GetTitleForRow(componentList, &cursor);
            char* group = GetGroupWithTitle(componentKeyfile, title);
            char* description = g_key_file_get_value(componentKeyfile, group, kDescriptionKey, NULL);
            gtk_label_set_text(GTK_LABEL(GetObject("componentDescriptionLabel")), description);

            g_list_foreach(selected, (GFunc)gtk_tree_path_free, NULL);
            g_list_free(selected);
            g_free(title);
            g_free(group);
            g_free(description);
        }
        break;
        default:
            gtk_label_set_text(GTK_LABEL(GetObject("componentDescriptionLabel")), kMultiselectDescription);
            break;
    }
}
