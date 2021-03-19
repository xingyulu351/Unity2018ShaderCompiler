#pragma once

#include <unordered_map>
#include <vector>
#include <string>
#include "TestingUtils.h"

namespace testing
{
    class CmdLineArgs
    {
        std::unordered_map<std::string, std::vector<std::string> > m_Args;

    public:
        CmdLineArgs(int argc, char** argv)
        {
            std::string option;
            std::vector<std::string> vals;
            for (auto i = 1; i < argc; ++i)
            {
                if (argv[i][0] == '-')
                {
                    if (option.empty())
                    {
                        option = ToLower(&argv[i][1]);
                    }
                    else
                    {
                        m_Args[option] = vals;
                        vals.clear();
                        option = ToLower(&argv[i][1]);
                    }
                }
                else
                {
                    vals.push_back(ToLower(argv[i]));
                }
            }
            if (!option.empty())
            {
                m_Args[option] = vals;
            }
        }

        bool HasArg(const char* option) const
        {
            auto opt = ToLower(option);
            return m_Args.end() != m_Args.find(opt);
        }

        std::vector<std::string> GetArgValue(const char* option) const
        {
            auto opt = ToLower(option);
            auto it = m_Args.find(opt);
            return it == m_Args.end() ? std::vector<std::string>() : it->second;
        }

        bool Empty() const
        {
            return m_Args.empty();
        }
    };
}
