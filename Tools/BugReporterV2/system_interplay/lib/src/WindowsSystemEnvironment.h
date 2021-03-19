#pragma once

#include "system_interplay/SystemEnvironment.h"

#include <Windows.h>
#include <vector>
#include <exception>

namespace ureport
{
namespace windows
{
    class SystemEnvironment : public ureport::SystemEnvironment
    {
        std::string GetTempDirectory() const override
        {
            const DWORD bufferSize = MAX_PATH;
            std::vector<wchar_t> buffer(bufferSize);
            char buffer2[bufferSize];
            const size_t size = ::GetTempPathW(bufferSize, &buffer[0]);
            if (size == 0)
                throw std::runtime_error(
                    "Failed to get path to temporary directory.");
            ::WideCharToMultiByte(CP_UTF8, 0, &buffer[0], static_cast<int>(bufferSize), &buffer2[0], static_cast<int>(bufferSize), NULL, NULL);
            return std::string(buffer2);
        }
    };
}
}
