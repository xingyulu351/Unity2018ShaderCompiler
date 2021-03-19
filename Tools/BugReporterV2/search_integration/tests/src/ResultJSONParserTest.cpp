#include <UnitTest++.h>

#include <list>

#include "search_integration/ResultJSONParser.h"

SUITE(ResultJSONParser)
{
    using namespace ureport;

    TEST(GetValue_WhenEmptyJSONString_GivenEmptyKey_ReturnsFalse)
    {
        ResultJSONParser parser("");

        int value;
        bool result = parser.GetValue("", value);

        CHECK_EQUAL(false, result);
    }

    TEST(GetValue_WhenEmptyJSONString_GivenAnyKey_ReturnsFalse)
    {
        ResultJSONParser parser("");

        int value;
        bool result = parser.GetValue("anykey", value);

        CHECK_EQUAL(false, result);
    }

    TEST(GetValue_WhenRandomString_GivenAnyKey_ReturnsFalse)
    {
        ResultJSONParser parser("randomstring");

        int value;
        bool result = parser.GetValue("anykey", value);

        CHECK_EQUAL(false, result);
    }

    TEST(GetValue_WhenEmptyJson_GivenEmptyKey_ReturnsFalse)
    {
        ResultJSONParser parser("{}");

        int value;
        bool result = parser.GetValue("", value);

        CHECK_EQUAL(false, result);
    }

    TEST(GetValue_WhenEmptyJson_GivenAnyKey_ReturnsFalse)
    {
        ResultJSONParser parser("{}");

        int value;
        bool result = parser.GetValue("anykey", value);

        CHECK_EQUAL(false, result);
    }

    TEST(GetValue_GivenCorrectJSON_GivenWrongKey_ReturnsFalse)
    {
        ResultJSONParser parser("{\"intkey\": 1}");

        int value;
        bool result = parser.GetValue("anykey", value);

        CHECK_EQUAL(false, result);
    }

    TEST(GetValue_GivenJsonWithOneIntEntry_GivenCorrectKey_ReturnsTrueAndValue)
    {
        ResultJSONParser parser("{\"intkey\": 1}");

        int value;
        bool result = parser.GetValue("intkey", value);

        CHECK_EQUAL(1, value);
        CHECK(result);
    }

    TEST(GetValue_GivenJsonWithOneEntryAndIntValue_OnGetValueAsStringAndGivenCorrectKey_ReturnsFalse)
    {
        ResultJSONParser parser("{\"intkey\": 1}");

        std::string value;
        bool result = parser.GetValue("intkey", value);

        CHECK_EQUAL(false, result);
    }

    TEST(GetValue_GivenCorrectJson_GivenCorrectKey_ReturnsTrueAndValue)
    {
        ResultJSONParser parser("{\"intkey\": 1,\n\"strkey\": \"strvalue\"}");

        int value;
        bool result = parser.GetValue("intkey", value);

        CHECK_EQUAL(1, value);
        CHECK(result);
    }

    TEST(GetValue_GivenJsonWithOneStringEntry_GivenCorrectKey_ReturnsTrueAndValue)
    {
        ResultJSONParser parser("{\"key\": \"value\"}");

        std::string value;
        bool result = parser.GetValue("key", value);

        CHECK_EQUAL("value", value);
        CHECK(result);
    }

    TEST(GetValue_GivenJsonNestedElements_GivenCorrectKeys_ReturnsTrueAndIntValue)
    {
        ResultJSONParser parser("{\"topkey\": {\"nestedkey\": 1} }");

        int value;
        bool result = parser.GetValue("topkey/nestedkey", value);

        CHECK_EQUAL(1, value);
        CHECK(result);
    }

    TEST(GetValue_GivenJsonNestedElements_GivenCorrectKeys_ReturnsTrueAndStringValue)
    {
        ResultJSONParser parser("{\"topkey\": {\"nestedkey\": \"value\"} }");

        std::string value;
        bool result = parser.GetValue("topkey/nestedkey", value);

        CHECK_EQUAL("value", value);
        CHECK(result);
    }

    TEST(GetValue_GivenJsonNested3LevelsElements_GivenCorrectKeys_ReturnsTrueAndIntValue)
    {
        ResultJSONParser parser("{\"topkey\": {\"nestedkey\": {\"subkey\" : 1} } }");

        int value;
        bool result = parser.GetValue("topkey/nestedkey/subkey", value);

        CHECK_EQUAL(1, value);
        CHECK(result);
    }

    TEST(GetValue_GivenJsonWithArray_GivenCorrectKeyAndIndex_ReturnsTrueAndValue)
    {
        std::string json("{\"arrkey\": [1,2,3]}");
        ResultJSONParser parser(json);

        int value;
        bool result = parser.GetValue("arrkey/[0]", value);

        CHECK_EQUAL(1, value);
        CHECK(result);
    }

    TEST(GetValue_GivenJsonWithArray_GivenCorrectKeyAndIndexOver10_ReturnsTrueAndValue)
    {
        std::string json("{\"arrkey\": [1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12]}");
        ResultJSONParser parser(json);

        int value;
        bool result = parser.GetValue("arrkey/[11]", value);

        CHECK_EQUAL(12, value);
        CHECK(result);
    }

    TEST(GetValue_GivenJsonWithArray_GivenCorrectKeyAndNegativeIndex_ReturnsFalse)
    {
        std::string json("{\"arrkey\": [1,2,3]}");
        ResultJSONParser parser(json);

        int value;
        bool result = parser.GetValue("arrkey/[-2]", value);

        CHECK_EQUAL(false, result);
    }

    TEST(GetValue_GivenJsonWithArray_GivenCorrectKeyAndNonExistentIndex_ReturnsFalse)
    {
        std::string json("{\"arrkey\": [1,2,3]}");
        ResultJSONParser parser(json);

        int value;
        bool result = parser.GetValue("arrkey/[5]", value);

        CHECK_EQUAL(false, result);
    }

    TEST(GetValue_GivenJsonWithArrayAndStrValues_GivenCorrectKeyAndIndex_ReturnsTrueAndStrValue)
    {
        std::string json("{\"arrkey\": [\"one\", \"two\", \"three\"]}");
        ResultJSONParser parser(json);

        std::string value;
        bool result = parser.GetValue("arrkey/[0]", value);

        CHECK_EQUAL("one", value);
        CHECK(result);
    }

    TEST(GetValue_GivenJsonWithArrayASNestedElement_GivenCorrectKeysAndIndex_ReturnsTrueAndValue)
    {
        std::string json("{\"key\" : {\"arrkey\": [1,2,3]}}");
        ResultJSONParser parser(json);

        int value;
        bool result = parser.GetValue("key/arrkey/[1]", value);

        CHECK_EQUAL(2, value);
        CHECK(result);
    }

    TEST(GetValue_GivenJsonValuesNestedInsideArray_GivenCorrectKeysAndIndex_ReturnsTrueAndValue)
    {
        std::string json("{\"arrkey\": [{\"key\": 1}, {\"key\" : 2}]}");
        ResultJSONParser parser(json);

        int value;
        bool result = parser.GetValue("arrkey/[0]/key", value);

        CHECK_EQUAL(1, value);
        CHECK(result);
    }

    TEST(GetNumberOfArrayItems_GivenEmptyStringAsJson_ReturnsZero)
    {
        ResultJSONParser parser("");

        auto result = parser.GetNumberOfArrayItems("key");

        CHECK_EQUAL(0, result);
    }

    TEST(GetNumberOfArrayItems_GivenEmptyJson_ReturnsZero)
    {
        ResultJSONParser parser("{}");

        auto result = parser.GetNumberOfArrayItems("key");

        CHECK_EQUAL(0, result);
    }

    TEST(GetNumberOfArrayItems_GivenInvalidJson_ReturnsZero)
    {
        ResultJSONParser parser("{Invalid JSON here}");

        auto result = parser.GetNumberOfArrayItems("key");

        CHECK_EQUAL(0, result);
    }

    TEST(GetNumberOfArrayItems_GivenKeyOfNonArray_ReturnsZero)
    {
        ResultJSONParser parser("{\"intkey\": 1}");

        auto result = parser.GetNumberOfArrayItems("intkey");

        CHECK_EQUAL(0, result);
    }

    TEST(GetNumberOfArrayItems_GivenKeyOfEmptyArray_ReturnsZero)
    {
        ResultJSONParser parser("{\"key\": []}");

        auto result = parser.GetNumberOfArrayItems("key");

        CHECK_EQUAL(0, result);
    }

    TEST(GetNumberOfArrayItems_GivenKeyOfArrayOfOneItem_ReturnsOne)
    {
        ResultJSONParser parser("{\"key\": [{\"key\": \"value\"}]}");

        auto result = parser.GetNumberOfArrayItems("key");

        CHECK_EQUAL(1, result);
    }

    TEST(GetNumberOfArrayItems_GivenKeyOfArrayOfTwoItems_ReturnsTwo)
    {
        ResultJSONParser parser("{\"key\": [{\"key\": \"value\"}, {\"key\": \"value\"}]}");

        auto result = parser.GetNumberOfArrayItems("key");

        CHECK_EQUAL(2, result);
    }
}
