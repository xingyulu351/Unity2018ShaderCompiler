#include "../src/WindowsArgUtils.h"
#include "shims/logical/IsEmpty.h"
#include "shims/attribute/GetSize.h"

#include <UnitTest++.h>
#include <vector>

SUITE(WindowsArgUtils)
{
    using namespace ureport;
    using namespace ureport::windows;

    TEST(MakeArgsVector_GivenNullString_ReturnsEmptyVector)
    {
        const auto args = MakeArgsVector(nullptr);
        CHECK(IsEmpty(args));
    }

    TEST(MakeArgsVector_GivenStringWithOneWord_ReturnsVectorWithTheWord)
    {
        const auto args = MakeArgsVector(L"word");
        CHECK_EQUAL(1, GetSize(args));
        CHECK_EQUAL("word", args[0]);
    }

    TEST(MakeArgsVector_GiveTwoWordsSeparatedBySpace_ReturnsTwoWordsVector)
    {
        const auto args = MakeArgsVector(L"first second");
        CHECK_EQUAL(2, GetSize(args));
        CHECK_EQUAL("first", args[0]);
        CHECK_EQUAL("second", args[1]);
    }

    TEST(MakeArgsVector_GivenTwoWordsInDoubleQuotes_ReturnsTheWordsAsOneArgumentWhitNoQuotes)
    {
        const auto args = MakeArgsVector(L"\"first second\"");
        CHECK_EQUAL(1, GetSize(args));
        CHECK_EQUAL("first second", args[0]);
    }
}
