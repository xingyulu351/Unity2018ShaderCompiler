#include "Args.h"
#include "shims/logical/IsNull.h"
#include "shims/logical/IsEmpty.h"
#include "shims/attribute/GetSize.h"

#include <UnitTest++.h>

SUITE(ArgsTest)
{
    using namespace ureport;

    TEST(Parse_GivenEmptySchemeAndEmptyInputSet_ReturnsArgs)
    {
        auto args = Args::Parse("", std::vector<std::string>());
        CHECK(!IsNull(args));
    }

    TEST(GetValues_GivenNonExistingName_ReturnsEmptySet)
    {
        auto args = Args::Parse("", std::vector<std::string>());
        CHECK(IsEmpty(args->GetValues("non existing name")));
    }

    TEST(Parse_GivenOneNameSchemeAndOneValue_ReturnsArgsContainsTheValue)
    {
        std::vector<std::string> input;
        input.push_back("--name");
        input.push_back("value");
        auto args = Args::Parse("name", input);
        auto values = args->GetValues("name");
        CHECK_EQUAL(1, GetSize(values));
        CHECK_EQUAL("value", values[0]);
    }

    TEST(Parse_GivenTwoArgumentsWithValues_ReturnsArgsContainsTheValues)
    {
        std::vector<std::string> input;
        input.push_back("--first");
        input.push_back("first-value");
        input.push_back("--second");
        input.push_back("second-value");
        auto args = Args::Parse("first,second", input);
        auto values = args->GetValues("first");
        CHECK_EQUAL(1, GetSize(values));
        CHECK_EQUAL("first-value", values[0]);
        values = args->GetValues("second");
        CHECK_EQUAL(1, GetSize(values));
        CHECK_EQUAL("second-value", values[0]);
    }

    TEST(Parse_GivenUnnamedValue_ReturnsArgsWithNoValue)
    {
        std::vector<std::string> input;
        input.push_back("value");
        auto args = Args::Parse("name", input);
        CHECK_EQUAL(0, GetSize(args->GetValues("name")));
    }

    TEST(Parse_GivenOneNameAndTwoValues_ReturnsArgsWithOneValue)
    {
        std::vector<std::string> input;
        input.push_back("--name");
        input.push_back("first");
        input.push_back("second");
        auto args = Args::Parse("name", input);
        auto values = args->GetValues("name");
        CHECK_EQUAL(1, GetSize(values));
        CHECK_EQUAL("first", values[0]);
    }

    TEST(Parse_GivenNameAndValueTwice_ReturnsArgsWithTwoValues)
    {
        std::vector<std::string> input;
        input.push_back("--name");
        input.push_back("first");
        input.push_back("--name");
        input.push_back("second");
        auto args = Args::Parse("name", input);
        auto values = args->GetValues("name");
        CHECK_EQUAL(2, GetSize(values));
        CHECK_EQUAL("first", values[0]);
        CHECK_EQUAL("second", values[1]);
    }

    TEST(Parse_GivenThreeNamesWithValues_ReturnsArgsWithTheValues)
    {
        std::vector<std::string> input;
        input.push_back("--first");
        input.push_back("first-value");
        input.push_back("--second");
        input.push_back("second-value");
        input.push_back("--third");
        input.push_back("third-value");
        auto args = Args::Parse("first,second,third", input);
        CHECK_EQUAL(1, GetSize(args->GetValues("second")));
        CHECK_EQUAL("second-value", args->GetValues("second")[0]);
        CHECK_EQUAL(1, GetSize(args->GetValues("first")));
        CHECK_EQUAL("first-value", args->GetValues("first")[0]);
        CHECK_EQUAL(1, GetSize(args->GetValues("third")));
        CHECK_EQUAL("third-value", args->GetValues("third")[0]);
    }
}
