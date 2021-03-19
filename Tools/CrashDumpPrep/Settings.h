#pragma once

#include <string>
#include <vector>
#include <tchar.h>

struct Settings
{
    Settings() : debug(false) {}

    std::wstring localCache;
    std::vector<std::wstring> upstreamStores;
    std::vector<std::wstring> dumpsToPrepare;
    bool debug;
};

void ParseArguments(Settings& settings, int argc, TCHAR* argv[]);

std::wstring GetDefaultLocalCachePath();
void GetDefaultUpstreamSymbolStores(std::vector<std::wstring>& stores);
