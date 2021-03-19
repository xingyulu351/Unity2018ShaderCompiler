#pragma once

namespace ureport
{
    class SimilarCasesSearch
    {
    public:
        virtual void FindSimilarCases(const std::string& title,
            std::function<void(std::shared_ptr<SearchResult>)> callback) = 0;
        virtual ~SimilarCasesSearch() {}
    };
}
