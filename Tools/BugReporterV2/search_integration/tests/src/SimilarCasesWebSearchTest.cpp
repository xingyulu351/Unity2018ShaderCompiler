#include <UnitTest++.h>

#include "shims/logical/IsNull.h"
#include "SearchPerformerStub.h"
#include "search_integration/SimilarCasesWebSearch.h"

SUITE(SimilarCasesWebSearch)
{
    using namespace ureport;

    TEST(FindSimilarCases_CallsCallback)
    {
        SimilarCasesWebSearch search(new SearchPerformerStub());
        int callbackCalls = 0;

        search.FindSimilarCases("", [&] (std::shared_ptr<SearchResult> result) {++callbackCalls; });

        CHECK_EQUAL(1, callbackCalls);
    }

    TEST(FindSimilarCases_CallsCallbackWithSearchResult)
    {
        SimilarCasesWebSearch search(new SearchPerformerStub());
        std::shared_ptr<SearchResult> searchResult;

        search.FindSimilarCases("", [&] (std::shared_ptr<SearchResult> result) {searchResult = result; });

        CHECK(!IsNull(searchResult));
    }

    /*
        TEST (FindSimilarCases_OnEmptySearch_ReturnsEmptyResult)
        {
            SimilarCasesWebSearch search (new SearchPerformerStub ());
            std::shared_ptr<SearchResult> searchResult;

            search.FindSimilarCases ("", [&] (std::shared_ptr<SearchResult> result) {searchResult = result;});

            CHECK_EQUAL (0, searchResult->GetTotalResultsCount ());
        }
        */
}
