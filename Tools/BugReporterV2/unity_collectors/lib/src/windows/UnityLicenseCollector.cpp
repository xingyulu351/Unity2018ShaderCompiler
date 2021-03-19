#include "../UnityLicenseCollector.h"

#include <Shlobj.h>
#include <string>
#include <fstream>
#include <sstream>
#include <exception>

namespace ureport
{
namespace unity_collectors
{
namespace windows
{
    class UnityLicenseCollector : public unity_collectors::UnityLicenseCollector
    {
        std::unique_ptr<std::istream> GetLicense() const override
        {
            return std::unique_ptr<std::istream>(new std::ifstream(GetUnityLicenseFilePath()));
        }

        std::wstring GetUnityLicenseFilePath() const
        {
            std::wstringstream path;
            path << GetCommonAppDataDirectoryPath()
            << "/Unity/Unity_lic.ulf";
            return path.str();
        }

        std::wstring GetCommonAppDataDirectoryPath() const
        {
            wchar_t path[MAX_PATH];
            if (FAILED(SHGetFolderPathW(0, CSIDL_COMMON_APPDATA, 0, SHGFP_TYPE_CURRENT,   path)))
                throw std::runtime_error("Failed to get common application data path");
            return path;
        }
    };
}

    std::unique_ptr<UnityLicenseCollector> CreateUnityLicenseCollector()
    {
        return std::unique_ptr<UnityLicenseCollector>(new windows::UnityLicenseCollector());
    }
}
}
