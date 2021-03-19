#include <UnitTest++.h>

#include <list>
#include <memory>

#include "search_integration/SearchResultGenerator.h"

SUITE(SearchResultGenerator)
{
    using namespace ureport;

    class SearchResultGeneratorTest
    {
    public:
        SearchResultGenerator m_SearchResultGenerator;
    };

    TEST_FIXTURE(SearchResultGeneratorTest, GivenEmptyString_ReturnsEmptyResult)
    {
        auto result = m_SearchResultGenerator.GenerateResultFromJSON("");

        CHECK_EQUAL(0, result->GetTotalResultsCount());
    }

    TEST_FIXTURE(SearchResultGeneratorTest, GivenEmptyJSON_ReturnsEmptyResult)
    {
        auto result = m_SearchResultGenerator.GenerateResultFromJSON("{}");

        CHECK_EQUAL(0, result->GetTotalResultsCount());
    }

    TEST_FIXTURE(SearchResultGeneratorTest, GivenMalformedJSON_ReturnsEmptyResult)
    {
        auto result = m_SearchResultGenerator.GenerateResultFromJSON("{aaa}");

        CHECK_EQUAL(0, result->GetTotalResultsCount());
    }

    TEST_FIXTURE(SearchResultGeneratorTest, GivenJSONWithResultsCount_ReturnsResultWithCorrectCount)
    {
        std::string json = "{\"searchInformation\": {\"totalResults\": \"100\", \"otherkey\": \"othervalue\"}}";

        auto result = m_SearchResultGenerator.GenerateResultFromJSON(json);

        CHECK_EQUAL(100, result->GetTotalResultsCount());
    }

    TEST_FIXTURE(SearchResultGeneratorTest, GivenJSONWithEmptySearchResultItems_ReturnsResultWithZeroItems)
    {
        std::string json = "{\"items\": [] }";

        auto result = m_SearchResultGenerator.GenerateResultFromJSON(json);

        CHECK_EQUAL(0, result->GetResults().size());
    }

    TEST_FIXTURE(SearchResultGeneratorTest, GivenJSONWithTwoSearchResultItems_ReturnsResultWithTwoItems)
    {
        std::string json = "{\"items\": [{\"title\": \"titlevalue\", \"link\": \"linkvalue\", \"displayLink\": \"displaylinkvalue\"}, {\"title\": \"titlevalue2\", \"link\": \"linkvalue2\", \"displayLink\": \"displaylinkvalue2\"}] }";

        auto result = m_SearchResultGenerator.GenerateResultFromJSON(json);

        CHECK_EQUAL(2, result->GetResults().size());
    }

    TEST_FIXTURE(SearchResultGeneratorTest, GivenJSONWithCorrectResultItems_ReturnsWithCorrecctValues)
    {
        std::string json = "{\"items\": [{\"title\": \"titlevalue\", \"link\": \"linkvalue\", \"displayLink\": \"displaylinkvalue\"}] }";

        auto result = m_SearchResultGenerator.GenerateResultFromJSON(json);

        auto results = result->GetResults();
        CHECK_EQUAL(1, results.size());
        CHECK_EQUAL("titlevalue", results[0].m_Title);
        CHECK_EQUAL("linkvalue", results[0].m_Url);
        CHECK_EQUAL("displaylinkvalue", results[0].m_SourceName);
    }
}
