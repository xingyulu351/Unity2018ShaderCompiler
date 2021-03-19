#pragma once

#include <vector>
#include <string>

namespace ureport
{
    struct WindowSettings
    {
        WindowSettings(const std::string& name, const std::vector<char>& state)
            : m_WindowName(name)
            , m_WindowState(state)
        {}

        std::string m_WindowName;
        std::vector<char> m_WindowState;
    };
}
