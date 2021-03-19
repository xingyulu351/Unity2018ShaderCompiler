#pragma once

#include <string>
#include <vector>

namespace ureport
{
namespace macosx
{
    class SystemProfile
    {
    public:
        typedef std::string DataType;

        virtual std::string GetData(const std::vector<DataType>& types) const = 0;

        virtual ~SystemProfile() {}
    };
}
}
