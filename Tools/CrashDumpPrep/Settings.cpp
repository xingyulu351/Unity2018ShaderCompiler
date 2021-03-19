#include <windows.h>
#include <algorithm>
#include "Settings.h"

void FindDumpFilesRecursive(Settings& settings, const std::wstring& root)
{
    WIN32_FIND_DATAW findData;
    HANDLE hFind = FindFirstFile((root + L"\\*").c_str(), &findData);
    if (hFind == INVALID_HANDLE_VALUE)
        throw "Error: Could not open path to search";

    do
    {
        std::wstring entryName(findData.cFileName);

        if (findData.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY)
        {
            if (entryName[0] != L'.')
                FindDumpFilesRecursive(settings, root + L"\\" + entryName);
        }

        if (entryName.size() > 4 && entryName.substr(entryName.size() - 4) == L".dmp")
            settings.dumpsToPrepare.push_back(root + L"\\" + entryName);
    }
    while (FindNextFile(hFind, &findData));

    FindClose(hFind);
}

void ParseArguments(Settings& settings, int argc, TCHAR* argv[])
{
    const char* helpString = "Usage: CrashDumpPrep.exe [/cache cachePath] [/upstream upstream [/upstream upstream]...] [/debug] [/search searchRoot [/search searchRoot]...] dumpfile dumpfile... \n"
        "Options:\n"
        "\t /cache cachePath: Cache symbols to the given local directory (default: the path set in VS2015, or a temp directory)\n"
        "\t /upstream upstreamPath: Path or URL of a symbol server or store to search for symbols (default: paths set in VS2015, or the Unity + Microsoft symbol stores)\n"
        "\t /debug: If set, turn on Dbghelp debugging and route it to the debugger's output window\n"
        "\t /search path: Recursively search the given path for all .dmp files and add them to the list of dumps to process\n";

    if (argc == 0)
        throw helpString;

    for (int i = 0; i < argc; ++i)
    {
        std::wstring arg = argv[i];
        std::transform(arg.begin(), arg.end(), arg.begin(), ::tolower);

        if (arg == L"/cache")
        {
            ++i;
            if (i >= argc)
                throw "Error: /cache must supply a cache path value";

            settings.localCache = argv[i];
            continue;
        }

        if (arg == L"/upstream")
        {
            ++i;
            if (i >= argc)
                throw "Error: /upstream must supply an upstream URL";

            settings.upstreamStores.push_back(argv[i]);
            continue;
        }

        if (arg == L"/debug")
        {
            settings.debug = true;
            continue;
        }

        if (arg == L"/search")
        {
            ++i;
            if (i >= argc)
                throw "Error: /search must specify a path to search in for dump files";

            FindDumpFilesRecursive(settings, argv[i]);
            continue;
        }

        if (arg == L"/help" || arg == L"/?")
        {
            throw helpString;
        }

        settings.dumpsToPrepare.push_back(argv[i]);
    }
}

std::wstring GetDefaultLocalCachePath()
{
    TCHAR path[MAX_PATH + 1];

    // See if we can retrieve it from the VS settings in the registry
    HKEY key;
    if (ERROR_SUCCESS == RegOpenKeyEx(HKEY_CURRENT_USER, L"SOFTWARE\\Microsoft\\VisualStudio\\14.0\\Debugger", 0, KEY_READ, &key))
    {
        DWORD pathLength = MAX_PATH + 1;
        LONG result = RegQueryValueEx(key, L"SymbolCacheDir", NULL, NULL, reinterpret_cast<LPBYTE>(path), &pathLength);
        RegCloseKey(key);

        if (result == ERROR_SUCCESS && path[0] != 0)
            return path;
    }

    // Pick a temp directory to use
    GetTempPath(MAX_PATH + 1, path);
    return std::wstring(path) + L"SymStore";
}

void GetDefaultUpstreamSymbolStores(std::vector<std::wstring>& stores)
{
    // See if we can retrieve it from the VS settings in the registry
    HKEY key;
    if (ERROR_SUCCESS == RegOpenKeyEx(HKEY_CURRENT_USER, L"SOFTWARE\\Microsoft\\VisualStudio\\14.0\\Debugger", 0, KEY_READ, &key))
    {
        TCHAR path[1024];
        DWORD pathLength = 1024;
        LONG result = RegQueryValueEx(key, L"SymbolPath", NULL, NULL, reinterpret_cast<LPBYTE>(path), &pathLength);

        if (result == ERROR_SUCCESS)
        {
            // Split the path into pieces separated by semicolons
            wchar_t* buffer;
            wchar_t* token = wcstok_s(path, L";", &buffer);
            while (token)
            {
                stores.push_back(token);
                token = wcstok_s(NULL, L";", &buffer);
            }
        }

        DWORD useMSSymbolServers;
        DWORD size = sizeof(DWORD);
        result = RegQueryValueEx(key, L"SymbolUseMSSymbolServers", NULL, NULL, reinterpret_cast<LPBYTE>(&useMSSymbolServers), &size);

        if (result == ERROR_SUCCESS && useMSSymbolServers)
            stores.push_back(L"https://msdl.microsoft.com/download/symbols");

        RegCloseKey(key);

        if (stores.size() > 0)
            return;
    }

    stores.push_back(L"http://symbolserver.hq.unity3d.com/");
    stores.push_back(L"https://msdl.microsoft.com/download/symbols");
}
