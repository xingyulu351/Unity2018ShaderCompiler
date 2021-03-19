#include "sysinfo/Version.h"

#include <UnitTest++.h>

SUITE(Version)
{
    using namespace ureport;

    TEST(GetMajor_ForDefaultVersion_ReturnsZero)
    {
        const Version version;
        CHECK_EQUAL(0, version.GetMajor());
    }

    TEST(GetMinor_ForDefaultVersion_ReturnsZero)
    {
        const Version version;
        CHECK_EQUAL(0, version.GetMinor());
    }

    TEST(Constructor_GivenMajorAndMinorVersionComponents_ConstructsVersionWithTheComponents)
    {
        const Version version(1, 2);
        CHECK_EQUAL(1, version.GetMajor());
        CHECK_EQUAL(2, version.GetMinor());
    }

    TEST(GetBuild_ForDefaultVersion_ReturnsZero)
    {
        const Version version;
        CHECK_EQUAL(0, version.GetBuild());
    }

    TEST(Constructor_GivenMajorAndMinorVersionComponents_ConstructsVersionWithZeroBuild)
    {
        const Version version(1, 2);
        CHECK_EQUAL(0, version.GetBuild());
    }

    TEST(Constructor_GivenBuild_ConstructsVersionWithSpecifiedBuild)
    {
        const Version version(0, 0, 1);
        CHECK_EQUAL(1, version.GetBuild());
    }

    TEST(GetTextualForm_ForDefaultVersion_ReturnsZerosAndDots)
    {
        const Version version;
        CHECK(version.GetTextualForm() == "0.0.0");
    }

    TEST(GetTextualForm_ForVersionWithMajorMinorAndBuildComponents_ReturnsMajorMinorAndBuildSeparatedWithDots)
    {
        const Version version(2, 3, 1);
        CHECK(version.GetTextualForm() == "2.3.1");
    }
}
