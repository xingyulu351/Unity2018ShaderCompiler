#pragma once

#include <functional>
#include <string>

namespace ureport
{
    class SearchPerformer
    {
    public:
        virtual void PerformSearch(std::string searchString,
            std::function<void(std::string)> callback) = 0;
        virtual ~SearchPerformer() {}
    };
}
