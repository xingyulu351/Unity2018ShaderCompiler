#pragma once

#include <functional>
#include <memory>
#include <string>

#include "search_integration/SearchResultGenerator.h"
#include "search_integration/SearchPerformer.h"
#include "search_integration/SimilarCasesSearch.h"

namespace ureport
{
    class SimilarCasesWebSearch : public SimilarCasesSearch
    {
        // TODO add cache
    public:
        SimilarCasesWebSearch(SearchPerformer* searchPerformer);
        void FindSimilarCases(const std::string& title,
            std::function<void(std::shared_ptr<SearchResult>)> callback);
        void ProcessSearchResult(const std::string& searchResult);

    private:
        std::unique_ptr<SearchPerformer> m_SearchPerformer;
        std::function<void(std::shared_ptr<SearchResult>)> m_SearchCallback;
        SearchResultGenerator m_ResultGenerator;
    };

    std::unique_ptr<SimilarCasesWebSearch> CreateSimilarCasesWebSearch();
}
