#pragma once

#include <string>

namespace ureport
{
    class LongTermOperProgressListener
    {
    public:
        virtual void UpdateProgress(size_t current, size_t total) = 0;
        virtual void UpdateStatus(const std::string& status) = 0;
        virtual ~LongTermOperProgressListener() {}
    };
}
