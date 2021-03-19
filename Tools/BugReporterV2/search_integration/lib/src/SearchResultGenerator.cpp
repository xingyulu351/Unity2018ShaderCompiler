#include <memory>
#include <sstream>

#include "search_integration/ResultJSONParser.h"
#include "search_integration/SearchResultGenerator.h"

namespace ureport
{
    std::shared_ptr<SearchResult> SearchResultGenerator::GenerateResultFromJSON(const std::string& json)
    {
        ResultJSONParser parser(json);

        int totalResults = GetTotalResultsCount(parser);
        std::vector<SearchResultItem> resultData = GetResults(parser);

        return std::shared_ptr<SearchResult>(new SearchResult(totalResults, resultData));
    }

    int SearchResultGenerator::GetTotalResultsCount(ResultJSONParser& parser) const
    {
        std::string totalResults;
        bool is_ok = parser.GetValue("searchInformation/totalResults", totalResults);
        if (!is_ok)
            return 0;
        return std::atoi(totalResults.c_str());
    }

    std::vector<SearchResultItem> SearchResultGenerator::GetResults(ResultJSONParser& parser) const
    {
        std::vector<SearchResultItem> results;

        std::string title;
        std::string link;
        std::string source;
        size_t resultsCount = parser.GetNumberOfArrayItems("items");
        for (size_t i = 0; i < resultsCount; ++i)
        {
            std::stringstream ss;
            ss << "items/[" << i << "]/";
            std::string elementKey = ss.str();

            if (parser.GetValue(elementKey + "title", title)
                && parser.GetValue(elementKey + "link", link)
                && parser.GetValue(elementKey + "displayLink", source))
            {
                results.push_back(SearchResultItem(title, link, source));
            }
        }

        return results;
    }
}
