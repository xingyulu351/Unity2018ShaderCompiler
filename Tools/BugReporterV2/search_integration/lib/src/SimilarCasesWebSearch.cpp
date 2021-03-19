#include "GoogleSearch.h"
#include "search_integration/SimilarCasesWebSearch.h"

namespace ureport
{
    SimilarCasesWebSearch::SimilarCasesWebSearch(SearchPerformer* searchPerformer)
        : m_SearchPerformer(searchPerformer)
    {}

    void SimilarCasesWebSearch::FindSimilarCases(const std::string& title,
        std::function<void(std::shared_ptr<SearchResult>)> callback)
    {
        m_SearchCallback = callback;
        std::function<void(std::string)> processSearchCallback =
            [&] (const std::string& result) {this->ProcessSearchResult(result); };
        m_SearchPerformer->PerformSearch(title, processSearchCallback);
    }

    void SimilarCasesWebSearch::ProcessSearchResult(const std::string& searchResult)
    {
        std::shared_ptr<SearchResult> result(m_ResultGenerator.GenerateResultFromJSON(searchResult));
        m_SearchCallback(result);
    }

    std::unique_ptr<SimilarCasesWebSearch> CreateSimilarCasesWebSearch()
    {
        SearchPerformer* search = new GoogleSearch();
        return std::unique_ptr<SimilarCasesWebSearch>(new SimilarCasesWebSearch(search));
    }
}
