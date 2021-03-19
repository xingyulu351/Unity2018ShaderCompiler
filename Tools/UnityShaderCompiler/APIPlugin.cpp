#include "UnityPrefix.h"
#include "ShaderCompiler.h"
#include "Runtime/Modules/LoadDylib.h"
#include "Runtime/GfxDevice/BuiltinShaderParamsNames.h"
#include "Runtime/Utilities/Word.h"
#include "Runtime/Utilities/PathNameUtility.h"
#include "Runtime/Utilities/File.h"
#include "Utilities/ProcessIncludes.h"
#include "APIPlugin.h"
#include "PlatformPlugin.h"

#if defined(_WIN32)
#include <Shlwapi.h>
#include "PlatformDependent/Win/WinUnicode.h"
#endif

PlatformPluginInterface* sPlatformPlugins[MAX_PLATFORM_PLUGINS] = { NULL };
int sPlatformPluginCount = 0;

int RunPluginUnitTests(const char* includesPath)
{
    // Ask each plugin and try to call RunUnitTests on it to allow any unit tests it
    // defines to run.
    int failures = 0;
    for (int i = 0; i < sPlatformPluginCount; i++)
    {
        if (sPlatformPlugins[i])
        {
            printf_console("====================\n** PLUGIN: [%s] **\n", sPlatformPlugins[i]->GetPlatformName());
            failures += sPlatformPlugins[i]->RunPluginUnitTests(includesPath);
        }
    }
    return failures;
}

typedef PlatformPluginInterface * (*tGetPluginInterface)(int, PlatformPluginDispatcher*);

// Search for all the dll's in the given path, can be called multiple times from different paths
void ReadPlatformPlugins(const core::string & pluginpath)
{
    const int cgBatchVersion = 0x0002; // current version of CgBatch ... can be incremented to stop old plugins working
    std::set<core::string> allFiles;
    GetFolderContentsAtPath(pluginpath, allFiles);
    for (std::set<core::string>::const_iterator i = allFiles.begin(); i != allFiles.end(); ++i)
    {
        const core::string& pluginname = *i;

        // reject any file that does not have the correct platform specific extension
        if (GetPathWithPlatformSpecificDllExtension(DeletePathNameExtension(pluginname)) != pluginname)
            continue;

        // When UTF16 encoded path names are converted to UTF8, any Asian characters will expand to three bytes.
        // Such paths cannot be passed to Windows API functions, they're purely for Unity's internal use.
        const int MaxPathUtf8 = MAX_PATH * 3;
        char dllpath[MaxPathUtf8];
        dllpath[0] = 0;
        strncpy(dllpath, pluginname.c_str(), MaxPathUtf8);

        tGetPluginInterface GetPluginInterface;

        // If you add new methods here, be sure to update ALL CgBatch plugins e.g. PS3, etc.
        bool dylibresult = LoadAndLookupSymbols(dllpath,
            "GetPluginInterface", &GetPluginInterface,
            NULL);

        if (dylibresult == true)
        {
            if (sPlatformPluginCount < MAX_PLATFORM_PLUGINS)
            {
                PlatformPluginInterface* plugin = GetPluginInterface(cgBatchVersion, gPluginDispatcher);
                if (plugin == NULL)
                {
                    printf("plugin %s not initialized\n", pluginname.c_str());
                }
                else
                {
                    Assert(plugin->GetShaderPlatforms() != 0);
                    sPlatformPlugins[sPlatformPluginCount] = plugin;
                    sPlatformPluginCount++;
                }
            }
            else
            {
                printf("Exceeded max number of platform plugins! ... ignoring plugin\n");
            }
        }
        else
        {
            printf("failed to load plugin %s, not a valid plugin or was unable to load a dependency\n", dllpath);
        }
    }
}
