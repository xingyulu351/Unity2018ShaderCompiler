#include "UnityVersion.h"
#include "artifacts/generated/Configuration/UnityConfigureRevision.gen.h"
#include "artifacts/generated/Configuration/UnityConfigureVersion.gen.h"
#include <string>

namespace ureport
{
namespace details
{
    class UnityVersion : public ureport::UnityVersion
    {
    private:
        std::string GetVersion() const
        {
            return UNITY_VERSION_FULL;
        }

        std::string GetMajorNumberStr() const
        {
            return std::to_string(static_cast<long long>(UNITY_VERSION_VER));
        }

        std::string GetMinorNumberStr() const
        {
            return std::to_string(static_cast<long long>(UNITY_VERSION_MAJ));
        }
    };
}

namespace unity_version
{
    std::unique_ptr<UnityVersion> CreateUnityVersion()
    {
        return std::unique_ptr<UnityVersion>(new details::UnityVersion());
    }
}
}
