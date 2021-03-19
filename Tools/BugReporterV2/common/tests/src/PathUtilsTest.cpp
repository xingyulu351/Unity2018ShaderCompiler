#include "../src/PathUtils.h"

#include <UnitTest++.h>

SUITE(PathUtils)
{
    using namespace ureport;
    using namespace path_utils;

    TEST(GetBaseName_Given_Empty_Path_Returns_Empty_Base_Name)
    {
        CHECK(GetBaseName("").empty());
    }

    TEST(GetBaseName_Given_Path_With_Slash_Returns_Correct_Base_Name)
    {
        CHECK_EQUAL("file.ext", GetBaseName("dir/file.ext"));
    }

    TEST(GetBaseName_Given_Path_With_Back_Slash_Returns_Correct_Base_Name)
    {
        CHECK_EQUAL("file.ext", GetBaseName("dir\\file.ext"));
    }

    TEST(GetBaseName_Given_Path_With_Slash_And_Back_Slash_Returns_Correct_Base_Name)
    {
        CHECK_EQUAL("file.ext", GetBaseName("dir1/dir\\file.ext"));
    }

    TEST(GetBaseName_Given_Path_With_Back_Slash_And_Slash_Returns_Correct_Base_Name)
    {
        CHECK_EQUAL("file.ext", GetBaseName("dir1\\dir/file.ext"));
    }

    TEST(GetRelativePath_Given_Empty_Base_Path_And_Path_Returns_Empty_Path)
    {
        CHECK_EQUAL("", GetRelativePath("", ""));
    }

    TEST(GetRelativePath_Given_Empty_Base_Path_Returns_Empty_Path)
    {
        CHECK_EQUAL("", GetRelativePath("dir", ""));
    }

    TEST(GetRelativePath_Given_Empty_Path_Returns_Empty_Path)
    {
        CHECK(GetRelativePath("", "dir").empty());
    }

    TEST(GetRelativePath_Given_Path_With_Slash_Returns_Correct_Path)
    {
        CHECK_EQUAL("dir2/file.ext", GetRelativePath("dir1/dir2/file.ext", "dir1"));
    }

    TEST(GetRelativePath_Given_Path_With_Back_Slash_Returns_Correct_Path)
    {
        CHECK_EQUAL("dir2\\file.ext", GetRelativePath("dir1\\dir2\\file.ext", "dir1"));
    }

    TEST(GetRelativePath_Given_Base_Path_With_Ending_Slash_Returns_Correct_Path)
    {
        CHECK_EQUAL("dir2/file.ext", GetRelativePath("dir1/dir2/file.ext", "dir1/"));
    }

    TEST(GetRelativePath_Given_Base_Path_With_Ending_Back_Slash_Returns_Correct_Path)
    {
        CHECK_EQUAL("dir2\\file.ext", GetRelativePath("dir1\\dir2\\file.ext", "dir1\\"));
    }

    TEST(GetRelativePath_Given_Slashed_Base_Path_And_Backslahed_Path_Returns_Correct_Path)
    {
        CHECK_EQUAL("dir3\\file.ext",
            GetRelativePath("dir1\\dir2\\dir3\\file.ext", "dir1/dir2"));
    }

    TEST(GetRelativePath_Given_Backslashed_Base_Path_And_Slahed_Path_Returns_Correct_Path)
    {
        CHECK_EQUAL("dir3/file.ext",
            GetRelativePath("dir1/dir2/dir3/file.ext", "dir1\\dir2"));
    }

    TEST(ConvertToSlashedPath_Given_Empty_Path_Returns_Empty_Path)
    {
        CHECK_EQUAL("", ConvertToSlashedPath(""));
    }

    TEST(ConvertToSlashedPath_Given_Slashed_Path_Returns_The_Same_Path)
    {
        CHECK_EQUAL("dir1/dir2", ConvertToSlashedPath("dir1/dir2"));
    }

    TEST(ConvertToSlashedPath_Given_Back_Slashed_Path_Returns_Slashed_Path)
    {
        CHECK_EQUAL("dir1/dir2", ConvertToSlashedPath("dir1\\dir2"));
    }

    TEST(GetSuffix_GivenEmpty_ReturnsEmptyType)
    {
        CHECK_EQUAL("", GetSuffix(""));
    }

    TEST(GetSuffix_GivenFilenameWithNoSuffix_ReturnsEmptyType)
    {
        CHECK_EQUAL("", GetSuffix("file"));
    }

    TEST(GetSuffix_GivenFullPathWithFilenameWithNoSuffix_ReturnsEmptyType)
    {
        CHECK_EQUAL("", GetSuffix("C:/Program Files/test/file"));
    }

    TEST(GetSuffix_GivenFullPathAndFilenameWithTxtSuffix_ReturnsTxt)
    {
        CHECK_EQUAL(".txt", GetSuffix("C:/Program Files/test/file.txt"));
    }

    TEST(GetSuffix_GivenFilenameWithTxtSuffixAndNoPath_ReturnsTxt)
    {
        CHECK_EQUAL(".txt", GetSuffix("file.txt"));
    }

    TEST(GetSuffix_GivenFullPathAndFilenameWithASuffix_ReturnsA)
    {
        CHECK_EQUAL(".a", GetSuffix("C:/some very long path with spaces/file.a"));
    }

    TEST(GetSuffix_GivenFullPathAndFilenameWithMultipleDotsAndSuffix_ReturnsSuffixAfterLastDot)
    {
        CHECK_EQUAL(".gz", GetSuffix("C:/some very long path with spaces/this.is.my.file.tar.gz"));
    }

    TEST(GetSuffix_GivenFullPathAndFolder_ReturnsEmptyType)
    {
        CHECK_EQUAL("", GetSuffix("C:/Program Files"));
    }

    TEST(GetSuffix_GivenFullPathAndFolderWithTrailingSlash_ReturnsEmptyType)
    {
        CHECK_EQUAL("", GetSuffix("C:/Program Files/"));
    }

    TEST(GetSuffix_GivenPathWithDotsAndFileWithoutSuffix_ReturnsEmptyType)
    {
        CHECK_EQUAL("", GetSuffix("C:/Program Files/Directory.Files/file"));
    }
}
