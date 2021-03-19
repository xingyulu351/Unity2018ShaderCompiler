#include "Runtime/Utilities/RapidJSONConfig.h"
#include <RapidJSON/document.h>

#include "Runtime/Testing/Testing.h"
#include "IndexCombiner.h"
#include "RapidJsonIoUtil.h"
#include "JsonTestUtils.h"
#include <iostream>

#if ENABLE_UNIT_TESTS

using namespace Unity::rapidjson;

UNIT_TEST_SUITE(IndexCombiner)
{
    const char ASimpleJson[] = "{\"pages\":\n"
        "[[\"A\", \"A\"]],\n"
        "\"info\": \n[[\"A text\",0]],\n"
        "\"common\": {\"a\":1},\n"
        "\"searchIndex\": \n{\"a\":[1]}}\n";
    const char BSimpleJson[] = "{\"pages\":\n"
        "[[\"B\", \"B\"]],\n"
        "\"info\": \n[[\"B text\",0]],\n"
        "\"common\": {\"a\":1},\n"
        "\"searchIndex\": \n{\"b\":[2]}}\n";

    //"pages" combined correctly
    //"indices" are offset by the number of indices in all previous modules
    TEST(TryGetJsonDocFromString_ValidString_ReturnsTrue)
    {
        Document main, other, result;
        bool success = RapidJsonIoUtil::TryGetJsonDocFromString(ASimpleJson, main);
        CHECK(success);
        success = RapidJsonIoUtil::TryGetJsonDocFromString(BSimpleJson, other);
        CHECK(success);
    }

    TEST(AppendJsonDocument_OneValueJson_ProducesThatJson)
    {
        Document A;
        Document expected;

        IndexCombiner jsonCombiner;
        RapidJsonIoUtil::TryGetJsonDocFromString(ASimpleJson, A);
        RapidJsonIoUtil::TryGetJsonDocFromString(ASimpleJson, expected);

        // add module info to all documents
        jsonCombiner.AddModuleIndexToEachEntry(A, 0);
        jsonCombiner.AddModuleIndexToEachEntry(expected, 0);

        jsonCombiner.AppendJsonDocument(A);

        Document& actual = jsonCombiner.GetCombinedDoc();
        std::string actualSt, expectedSt;
        actualSt = RapidJsonIoUtil::OutputJsonValueToString(actual);
        expectedSt = RapidJsonIoUtil::OutputJsonValueToString(expected);
        CHECK_EQUAL(expectedSt, actualSt);
    }


    TEST(AppendJsonDocument_TwoValidJsons_ProducesCombinedJson)
    {
        std::string expectedSt = "{\"pages\":[[\"A\",\"A\",0],[\"B\",\"B\",1]],"
            "\"info\":[[\"A text\",0],[\"B text\",1]],"
            "\"common\":{\"a\":1},"
            "\"searchIndex\":{\"a\":[1],\"b\":[3]}}";

        IndexCombiner jsonCombiner;

        VerifyCombineJsons(jsonCombiner, ASimpleJson, 0, BSimpleJson, 1, expectedSt);
    }
}

#endif // ENABLE_UNIT_TESTS
