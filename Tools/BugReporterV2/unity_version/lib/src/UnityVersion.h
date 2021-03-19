#pragma once

#include <string>
#include <memory>

namespace ureport
{
    class UnityVersion
    {
    public:
        virtual std::string GetVersion() const = 0;
        virtual std::string GetMajorNumberStr() const = 0;
        virtual std::string GetMinorNumberStr() const = 0;

        virtual ~UnityVersion() {}
    };

namespace unity_version
{
    std::unique_ptr<UnityVersion> CreateUnityVersion();
}
}
