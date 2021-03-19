#pragma once

#include <string>
#include <vector>

#include "search_integration/SearchResultItem.h"

namespace ureport
{
    class SearchResult
    {
    public:
        SearchResult();
        SearchResult(int total, const std::vector<SearchResultItem>& results);
        int GetTotalResultsCount();
        std::vector<SearchResultItem> GetResults() const;

    private:
        std::vector<SearchResultItem> m_Results;
        int m_TotalSearchResults;
    };
}
