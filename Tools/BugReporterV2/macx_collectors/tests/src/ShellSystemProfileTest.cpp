#include "ShellSystemProfile.h"

#include <UnitTest++.h>

SUITE(ShellSystemProfile)
{
    using namespace ::ureport;
    using namespace ::ureport::macosx;

    TEST(GetData_GivenNoDataType_ReturnsEmptyString)
    {
        ShellSystemProfile profile(nullptr);
        CHECK_EQUAL("", profile.GetData(std::vector<std::string>()));
    }
}
