#pragma once

#include "Args.h"

#include <map>
#include <vector>

namespace ureport
{
namespace test
{
    class ArgsStub : public ureport::Args
    {
    public:
        mutable std::map<std::string, std::vector<std::string> > m_Values;

        std::vector<std::string> GetValues(const std::string& keyword) const
        {
            return m_Values[keyword];
        }
    };
}
}
