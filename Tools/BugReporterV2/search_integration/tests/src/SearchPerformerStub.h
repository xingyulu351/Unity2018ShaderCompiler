#pragma once

#include "search_integration/SearchPerformer.h"

namespace ureport
{
    class SearchPerformerStub : public SearchPerformer
    {
    public:
        SearchPerformerStub()
            : m_SearchResult("")
        {}

        SearchPerformerStub(const std::string& searchResult)
            : m_SearchResult(searchResult)
        {}

        void PerformSearch(std::string searchString,
            std::function<void(std::string)> callback)
        {
            callback(m_SearchResult);
        }

        std::string m_SearchResult;
    };
}
