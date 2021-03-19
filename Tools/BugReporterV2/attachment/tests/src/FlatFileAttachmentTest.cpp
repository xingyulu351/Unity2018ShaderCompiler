#include <UnitTest++.h>

#include <sstream>

#include "../../lib/src/FlatFileAttachment.h"

SUITE(FlatFileAttachment)
{
    using namespace ureport;
    using namespace ureport::details;

    TEST(GetShortPreview_EmptyStream)
    {
        std::stringstream ss;
        auto preview = GetShortPreview(ss, 0, 0);

        CHECK(preview.empty());
    }

    TEST(GetShortPreview_EmptyStream_TryToGetData)
    {
        std::stringstream ss;
        auto preview = GetShortPreview(ss, 10, 10);

        CHECK(preview.empty());
    }

    TEST(GetShortPreview_ExtractsWholeLine)
    {
        std::stringstream ss("line");
        auto preview = GetShortPreview(ss, 4, 1);

        CHECK_EQUAL(1, preview.size());
        CHECK_EQUAL("line", preview[0]);
    }

    TEST(GetShortPreview_ExtractMoreThenLine)
    {
        std::stringstream ss("line");
        auto preview = GetShortPreview(ss, 5, 1);

        CHECK_EQUAL(1, preview.size());
        CHECK_EQUAL("line", preview[0]);
    }

    TEST(GetShortPreview_ExtractLessThenLine)
    {
        std::stringstream ss("line");
        auto preview = GetShortPreview(ss, 3, 1);

        CHECK_EQUAL(1, preview.size());
        CHECK_EQUAL("lin", preview[0]);
    }

    TEST(GetShortPreview_ExtractMoreThenLine_GivenTwoLines)
    {
        std::stringstream ss("line1\nline2");
        auto preview = GetShortPreview(ss, 6, 2);

        CHECK_EQUAL(2, preview.size());
        CHECK_EQUAL("line1", preview[0]);
        CHECK_EQUAL("line2", preview[1]);
    }

    TEST(GetShortPreview_ExtractLessThenLine_GivenTwoLines)
    {
        std::stringstream ss("line1\nline2");
        auto preview = GetShortPreview(ss, 3, 2);

        CHECK_EQUAL(2, preview.size());
        CHECK_EQUAL("lin", preview[0]);
        CHECK_EQUAL("lin", preview[1]);
    }

    TEST(GetShortPreview_ExtractMoreLinesThenStreamContains)
    {
        std::stringstream ss("line");
        auto preview = GetShortPreview(ss, 4, 3);

        CHECK_EQUAL(1, preview.size());
    }

    TEST(GetShortPreview_ExtractLessLinesThenStreamContains)
    {
        std::stringstream ss("line1\nline2\nline3");
        auto preview = GetShortPreview(ss, 4, 2);

        CHECK_EQUAL(2, preview.size());
    }

    TEST(GetShortPreview_ExtractAllLess)
    {
        std::stringstream ss("\nline1\nline2\n\nline \nline_4\nline5\n");
        auto preview = GetShortPreview(ss, 5, 7);
        CHECK_EQUAL(7, preview.size());
        CHECK_EQUAL("", preview[0]);
        CHECK_EQUAL("line1", preview[1]);
        CHECK_EQUAL("line2", preview[2]);
        CHECK_EQUAL("", preview[3]);
        CHECK_EQUAL("line ", preview[4]);
        CHECK_EQUAL("line_", preview[5]);
        CHECK_EQUAL("line5", preview[6]);
    }
}
