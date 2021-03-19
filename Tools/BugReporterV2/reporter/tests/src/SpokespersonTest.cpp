#include "reporter/Spokesperson.h"

#include <UnitTest++.h>

using namespace ureport;

SUITE(Spokesperson)
{
    TEST(GetEmail_NewlyCreatedSpokesperson_ReturnsEmptyString)
    {
        Spokesperson person;
        CHECK_EQUAL("", person.GetEmail());
    }

    TEST(SetEmail_NewlyCreatedSpokesperson_SetsEmail)
    {
        Spokesperson person;
        person.SetEmail("user@site.org.ctr");
        CHECK_EQUAL("user@site.org.ctr", person.GetEmail());
    }
}
