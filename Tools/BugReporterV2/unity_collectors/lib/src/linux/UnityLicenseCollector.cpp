#include "../UnityLicenseCollector.h"

#include <string>
#include <sstream>
#include <fstream>

namespace ureport
{
namespace unity_collectors
{
namespace linuxos
{
    class UnityLicenseCollector : public unity_collectors::UnityLicenseCollector
    {
        std::unique_ptr<std::istream> GetLicense() const override
        {
            return std::unique_ptr<std::istream>(new std::ifstream(GetUnityLicenseFilePath()));
        }

        std::string GetUnityLicenseFilePath() const
        {
            std::stringstream path;

            path << getenv("HOME") << "/.local/share/unity3d/Unity/Unity_lic.ulf";

            return path.str();
        }
    };
}

    std::unique_ptr<UnityLicenseCollector> CreateUnityLicenseCollector()
    {
        return std::unique_ptr<UnityLicenseCollector>(new linuxos::UnityLicenseCollector());
    }
}
}
