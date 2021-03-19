#pragma once

#include <memory>
#include <string>
#include <vector>

namespace ureport
{
    class Args
    {
    public:
        static std::unique_ptr<Args> Parse(const std::string& scheme,
            const std::vector<std::string>& args);

        virtual std::vector<std::string> GetValues(const std::string& keyword) const = 0;

        virtual ~Args() {}
    };
}
