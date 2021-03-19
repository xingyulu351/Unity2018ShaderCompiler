#pragma once

#include <iostream>
#include "search_integration/SimilarCasesSearch.h"

namespace ureport
{
    class SimilarCasesSearchFake : public SimilarCasesSearch
    {
    public:
        virtual void FindSimilarCases(const std::string& title,
            std::function<void(std::shared_ptr<SearchResult>)> callback)
        {
            std::cout << "Search for: " << title << std::endl;
        }
    };
}
