#pragma once

#include "unity_version/UnityVersion.h"

namespace ureport
{
namespace test
{
    class UnityVersionFake : public ureport::UnityVersion
    {
        std::string GetVersion() const
        {
            return "4.5.0a1";
        }

        std::string GetMajorNumberStr() const
        {
            return "4";
        }

        std::string GetMinorNumberStr() const
        {
            return "5";
        }
    };
}
}
