#pragma once

#include <string>

namespace ureport
{
    struct Feedback
    {
    public:
        Feedback() {}
        Feedback(std::string contents, int severity) :
            contents(contents), severity(severity)  {}
        std::string contents;
        int severity;
    };
}
