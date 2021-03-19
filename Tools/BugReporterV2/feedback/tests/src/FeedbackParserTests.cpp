#include <UnitTest++.h>
#include "FeedbackParser.h"


SUITE(FeedbackParser) {
    using namespace ureport;

    TEST(GivenInvalidJsonReturnsEmptyVector)
    {
        FeedbackParser parser;
        QString json("#json{not all that valid\"][@@");
        std::map<std::string, Feedback> result = parser.FromJson(json.toUtf8());
        CHECK(result.empty());
    }

    TEST(GivenJsonWithSingleFeedbackItemReturnsSingleResult)
    {
        FeedbackParser parser;
        QString json("{\"Description\":{\"Contents\":\"Some Feedback\", \"Severity\":1}}");
        std::map<std::string, Feedback> result = parser.FromJson(json.toUtf8());
        CHECK_EQUAL(1, result.size());
        Feedback f = result["Description"];
        CHECK_EQUAL("Some Feedback", f.contents);
        CHECK_EQUAL(1, f.severity);
    }

    TEST(GivenMultipleFeedbackReturnsMultipleResults)
    {
        FeedbackParser parser;
        QString json("{\"Description\":{\"Contents\":\"\", \"Severity\":0}, \"Title\":{\"Contents\":\"\",\"Severity\":0}}");
        std::map<std::string, Feedback> result = parser.FromJson(json.toUtf8());
        CHECK_EQUAL(2, result.size());
    }
}
