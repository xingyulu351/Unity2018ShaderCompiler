#pragma once

#include "search_integration/ResultJSONParser.h"
#include "search_integration/SearchResult.h"
#include "search_integration/SearchResultItem.h"

namespace ureport
{
    class SearchResultGenerator
    {
    public:
        std::shared_ptr<SearchResult> GenerateResultFromJSON(const std::string& json);

    private:
        int GetTotalResultsCount(ResultJSONParser& parser) const;
        std::vector<SearchResultItem> GetResults(ResultJSONParser& parser) const;
    };
}
