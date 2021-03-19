#pragma once

#include <string>
#include <map>

namespace ureport
{
    struct PlatformTraits
    {
        bool isNDA;
    };

    std::map<std::string, PlatformTraits> CreateUnityPlatformTraitsMap();
}
