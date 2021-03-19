#include "search_integration/SearchResult.h"

namespace ureport
{
    SearchResult::SearchResult()
        : m_TotalSearchResults(0)
    {}

    SearchResult::SearchResult(int total, const std::vector<SearchResultItem>& results)
        : m_TotalSearchResults(total)
        , m_Results(results)
    {}

    int SearchResult::GetTotalResultsCount()
    {
        return m_TotalSearchResults;
    }

    std::vector<SearchResultItem> SearchResult::GetResults() const
    {
        return m_Results;
    }
}
