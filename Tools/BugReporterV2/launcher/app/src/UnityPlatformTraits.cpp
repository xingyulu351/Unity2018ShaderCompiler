#include "UnityPlatformTraits.h"

namespace ureport
{
    std::map<std::string, PlatformTraits> CreateUnityPlatformTraitsMap()
    {
        std::map<std::string, PlatformTraits> traits;
        traits["XboxOnePlayer"].isNDA = true;
        traits["PS4Player"].isNDA = true;
        traits["PSP2Player"].isNDA = true;
        return traits;
    }
}
