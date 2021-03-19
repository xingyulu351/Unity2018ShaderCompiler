#include <SDKDDKVer.h>
#include <tchar.h>
#include <iostream>
#include <sstream>
#include <map>
#include <cassert>
#include <windows.h>

#define DBGHELP_TRANSLATE_TCHAR
#include <DbgHelp.h>

#include "Settings.h"

struct AutoHandle
{
    AutoHandle() : _value(INVALID_HANDLE_VALUE) {}
    AutoHandle(HANDLE h) : _value(h) {}

    AutoHandle& operator=(HANDLE h)
    {
        if (*this)
            CloseHandle(_value);
        _value = h;
        return *this;
    }

    ~AutoHandle()
    {
        if (*this)
            CloseHandle(_value);
    }

    operator HANDLE() { return _value; }
    operator bool() { return _value != INVALID_HANDLE_VALUE; }

private:
    HANDLE _value;
};

class MiniDump
{
    AutoHandle file;
    AutoHandle fileMapping;
    void*  view;

public:
    MiniDump(const std::wstring& path) : file(INVALID_HANDLE_VALUE), fileMapping(INVALID_HANDLE_VALUE), view(NULL)
    {
        file = CreateFile(path.c_str(), GENERIC_READ, FILE_SHARE_READ, NULL, OPEN_EXISTING, NULL, NULL);
        if (!file)
            throw "Couldn't open dump file";

        fileMapping = CreateFileMapping(file, NULL, PAGE_READONLY, 0, 0, NULL);
        if (!fileMapping)
            throw "Couldn't create dump file mapping";

        view = MapViewOfFile(fileMapping, FILE_MAP_READ, 0, 0, 0);
        if (!view)
            throw "Couldn't map view of dump file";
    }

    ~MiniDump()
    {
        if (view != NULL)
            UnmapViewOfFile(view);
    }

    operator const void*() const { return view; }

    const BYTE* getBytes(RVA rva) const { return ((BYTE*)view + rva); }

    std::wstring getString(RVA rva) const
    {
        const MINIDUMP_STRING* string = reinterpret_cast<const MINIDUMP_STRING*>(getBytes(rva));

        assert(!IsBadReadPtr(string, sizeof(MINIDUMP_STRING)));
        assert(!IsBadStringPtrW(string->Buffer, string->Length / sizeof(WCHAR)));

        return string->Buffer;
    }

    const MINIDUMP_MODULE_LIST* getModuleList() const
    {
        MINIDUMP_DIRECTORY* directory;
        MINIDUMP_MODULE_LIST* moduleStream;
        DWORD moduleStreamLengthBytes;
        MiniDumpReadDumpStream(view, ModuleListStream, &directory, (PVOID*)&moduleStream, &moduleStreamLengthBytes);
        return moduleStream;
    }
};

class SymbolSession
{
    HANDLE _fakeProcess;

    struct ModuleInfo
    {
        std::wstring moduleName;
        ULONG32 timeDateStamp;
        DWORD imageSize;

        bool operator==(const ModuleInfo& other) const { return moduleName == other.moduleName && timeDateStamp == other.timeDateStamp && imageSize == other.imageSize; }
        bool operator<(const ModuleInfo& other) const
        {
            if (timeDateStamp != other.timeDateStamp) return timeDateStamp < other.timeDateStamp;
            if (imageSize != other.imageSize) return imageSize < other.imageSize;
            return (moduleName < other.moduleName);
        }
    };

    std::map<ModuleInfo, std::wstring> _cachedResults;

public:
    SymbolSession(std::wstring searchPath) : _fakeProcess(reinterpret_cast<HANDLE>(1))
    {
        SymInitialize(_fakeProcess, searchPath.c_str(), FALSE);
    }

    ~SymbolSession()
    {
        SymCleanup(_fakeProcess);
    }

    std::wstring FindFileInPath(const std::wstring& moduleName, ULONG32 timeDateStamp, DWORD imageSize)
    {
        ModuleInfo module = { moduleName, timeDateStamp, imageSize };

        std::map<ModuleInfo, std::wstring>::const_iterator it = _cachedResults.find(module);
        if (it != _cachedResults.end())
            return it->second;

        WCHAR symbolFilePath[MAX_PATH + 1];
        if (!SymFindFileInPath(_fakeProcess, NULL, moduleName.c_str(), static_cast<void*>(&timeDateStamp), imageSize, 0, SSRVOPT_DWORDPTR, symbolFilePath, NULL, NULL))
            symbolFilePath[0] = 0;

        _cachedResults[module] = symbolFilePath;
        return symbolFilePath;
    }

    static std::wstring MakeSearchPath(const std::wstring& localCache, const std::vector<std::wstring>& upstreams)
    {
        std::wstring result;
        for (std::vector<std::wstring>::const_iterator it = upstreams.begin(); it != upstreams.end(); ++it)
        {
            if (!result.empty())
                result += L";";

            result += L"SRV*" + localCache + L"*" + *it;
        }

        return result;
    }
};

std::wstring GetVersionString(const VS_FIXEDFILEINFO& version)
{
    std::wstringstream stream;
    stream  << ((version.dwFileVersionMS >> 16) & 0xffff) << L"."
    << ((version.dwFileVersionMS >>  0) & 0xffff) << L"."
    << ((version.dwFileVersionLS >> 16) & 0xffff) << L"."
    << ((version.dwFileVersionLS >>  0) & 0xffff);

    std::wstring result;
    stream >> result;
    return result;
}

const wchar_t* kWindowsPlayerNames[] = { L"player_win.exe", L"WindowsPlayer.exe" };

void PrepareDump(const std::wstring& dumpPath, SymbolSession& symbols)
{
    MiniDump dump(dumpPath);
    const MINIDUMP_MODULE_LIST& moduleStream = *dump.getModuleList();

    std::wcout << "Reading modules from dump " << dumpPath << ":" << std::endl;

    for (ULONG32 i = 0; i < moduleStream.NumberOfModules; ++i)
    {
        const MINIDUMP_MODULE& module = moduleStream.Modules[i];

        std::wstring moduleName = dump.getString(module.ModuleNameRva);
        const void* id = &module.TimeDateStamp;
        DWORD imageSize = module.SizeOfImage;

        std::wstring symbolFilePath;

        if (moduleName.substr(moduleName.size() - 4) == L".exe")
        {
            bool foundModule = false;
            for (auto name : kWindowsPlayerNames)
            {
                // Exes will *usually* be the Windows Player, so check that first as it's likely to be a cache hit
                symbolFilePath = symbols.FindFileInPath(name, module.TimeDateStamp, module.SizeOfImage);
                if (!symbolFilePath.empty())
                {
                    // got it - copy the file to sit alongside the dump
                    std::wstring exePath = dumpPath.substr(0, dumpPath.rfind('\\')) + moduleName.substr(moduleName.rfind('\\'));

                    std::wcout << "\tModule " << moduleName << " is the Unity Player executable (version " << GetVersionString(module.VersionInfo) << ");\n\t\tcopying executable from " << symbolFilePath << "\n\t\tto " << exePath << std::endl;
                    CopyFile(symbolFilePath.c_str(), exePath.c_str(), FALSE);
                    foundModule = true;
                    break;
                }
            }

            if (foundModule)
                continue;
        }

        symbolFilePath = symbols.FindFileInPath(moduleName, module.TimeDateStamp, module.SizeOfImage);
        if (!symbolFilePath.empty())
            // Found the symbols OK
            continue;

        std::wcout << "\tModule " << moduleName << " (version " << GetVersionString(module.VersionInfo) << ") is not known to the symbol store." << std::endl;
    }
}

int _tmain(int argc, TCHAR* argv[])
{
    Settings settings;
    try
    {
        ParseArguments(settings, argc - 1, &argv[1]);
    }
    catch (const char* msg)
    {
        std::cout << msg << std::endl;
        return -1;
    }

    if (settings.localCache.empty())
    {
        settings.localCache = GetDefaultLocalCachePath();
        std::wcout << "Using default local cache path " << settings.localCache << std::endl;
    }

    if (settings.upstreamStores.empty())
    {
        GetDefaultUpstreamSymbolStores(settings.upstreamStores);

        std::wcout << "Using default upstream store paths:" << std::endl;
        for (std::vector<std::wstring>::const_iterator it = settings.upstreamStores.begin(); it != settings.upstreamStores.end(); ++it)
            std::wcout << "\t" << *it << std::endl;
    }

    if (settings.debug)
    {
        std::wcout << "Sending DbgHelper debug output to the debugger" << std::endl;
        SymSetOptions(SYMOPT_DEBUG);
    }

    SymbolSession symbols(SymbolSession::MakeSearchPath(settings.localCache, settings.upstreamStores));

    for (std::vector<std::wstring>::const_iterator it = settings.dumpsToPrepare.begin(); it != settings.dumpsToPrepare.end(); ++it)
    {
        struct _stat64 dummy;
        if (_wstat64(it->c_str(), &dummy))
        {
            std::wcout << "Could not find or open " << *it << ", skipping" << std::endl << std::endl;
            continue;
        }

        try
        {
            TCHAR buffer[MAX_PATH];
            _wfullpath(buffer, it->c_str(), MAX_PATH);
            PrepareDump(buffer, symbols);
        }
        catch (const char* msg)
        {
            std::cout << msg << std::endl;
        }
    }

    std::wcout << "Done." << std::endl << std::endl;
    return 0;
}
