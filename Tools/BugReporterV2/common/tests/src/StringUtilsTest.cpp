#include "../src/StringUtils.h"
#include "shims/attribute/GetSize.h"
#include "shims/logical/IsEmpty.h"

#include <UnitTest++.h>
#include <vector>
#include <string>
#include <algorithm>

using namespace ureport;

SUITE(StringUtils)
{
#ifndef LINUX
    // Widen/narrow makes no sense here
    TEST(Widen_GivenEmptyString_ReturnsEmptyString)
    {
        CHECK(Widen("") == L"");
    }

    TEST(Widen_GivenNonEmptyString_ReturnsWidenRepresentationOfTheString)
    {
        CHECK(Widen("narrow") == L"narrow");
    }

    TEST(Narrow_GivenEmptyString_ReturnsEmptyString)
    {
        CHECK_EQUAL("", Narrow(L""));
    }

    TEST(Narrow_GivenNonEmptyString_ReturnsNarrowRepresentationOfTheString)
    {
        CHECK_EQUAL("widen", Narrow(L"widen"));
    }
#endif

    TEST(Join_GivenEmptyRange_ReturnsEmptyString)
    {
        std::vector<std::string> strings;
        std::string result = Join(strings.begin(), strings.end(), "");
        CHECK_EQUAL("", result);
    }

    TEST(Join_GivenEmptyRangeAndSeparator_ReturnsEmptyString)
    {
        std::vector<std::string> strings;
        std::string result = Join(strings.begin(), strings.end(), "separator");
        CHECK_EQUAL("", result);
    }

    TEST(Join_GivenOneStringRange_ReturnsThisString)
    {
        std::vector<std::string> strings;
        strings.push_back("string");
        std::string result = Join(strings.begin(), strings.end(), "");
        CHECK_EQUAL("string", result);
    }

    TEST(Join_GivenOneStringRangeAndSeparator_ReturnsOnlyTheString)
    {
        std::vector<std::string> strings;
        strings.push_back("string");
        std::string result = Join(strings.begin(), strings.end(), "separator");
        CHECK_EQUAL("string", result);
    }

    TEST(Join_GivenFirstAndSecond_ReturnsFirstSecond)
    {
        std::vector<std::string> strings;
        strings.push_back("First");
        strings.push_back("Second");
        std::string result = Join(strings.begin(), strings.end(), "");
        CHECK_EQUAL("FirstSecond", result);
    }

    TEST(Join_GivenFirstAndSecondAndSeparator_ReturnsFirstSeparatorSecond)
    {
        std::vector<std::string> strings;
        strings.push_back("First");
        strings.push_back("Second");
        std::string result = Join(strings.begin(), strings.end(), "Separator");
        CHECK_EQUAL("FirstSeparatorSecond", result);
    }

    TEST(Join_GivenTwoStringsWhereSecondIsEmptyAndEmptySeparator_ReturnsOnlyFirst)
    {
        std::vector<std::string> strings;
        strings.push_back("First");
        strings.push_back("");
        std::string result = Join(strings.begin(), strings.end(), "");
        CHECK_EQUAL("First", result);
    }

    TEST(Join_GivenTwoStringsWhereFirstIsEmptyAndEmptySeparator_ReturnsOnlySecond)
    {
        std::vector<std::string> strings;
        strings.push_back("");
        strings.push_back("Second");
        std::string result = Join(strings.begin(), strings.end(), "");
        CHECK_EQUAL("Second", result);
    }

    TEST(Join_GivenTwoStringsWhereSecondIsEmptyAndSeparator_ReturnsOnlyFirst)
    {
        std::vector<std::string> strings;
        strings.push_back("First");
        strings.push_back("");
        std::string result = Join(strings.begin(), strings.end(), "Separator");
        CHECK_EQUAL("First", result);
    }

    TEST(Join_GivenTwoStringsWhereFirstIsEmptyAndSeparator_ReturnsOnlySecond)
    {
        std::vector<std::string> strings;
        strings.push_back("");
        strings.push_back("Second");
        std::string result = Join(strings.begin(), strings.end(), "Separator");
        CHECK_EQUAL("Second", result);
    }

    TEST(MakeRandomString_Given0_ReturnsEmptyString)
    {
        CHECK_EQUAL("", MakeRandomString(0, " "));
    }

    TEST(MakeRandomString_Given10_ReturnsNonEmptyStringWithSizeEqualTo10)
    {
        const auto string = MakeRandomString(10, " ");
        CHECK(!IsEmpty(string));
        CHECK_EQUAL(10, GetSize(string));
    }

    TEST(MakeRandomString_Given10AndEmptyCharSet_ReturnsEmptyString)
    {
        CHECK_EQUAL("", MakeRandomString(10, ""));
    }

    TEST(MakeRandomString_Given5AndOneLetterSet_ReturnsStringContainsOnlyTheCharacter)
    {
        CHECK_EQUAL(std::string(5, 'a'), MakeRandomString(5, "a"));
    }

    /* DISABLED as flip-flopper
    TEST (MakeRandomString_CallTwiceWithTheSameInput_ReturnsDifferentResults)
    {
        const std::string first = MakeRandomString (5, "alz");
        const std::string second = MakeRandomString (5, "alz");
        CHECK (first != second);
    }*/
}
