#include "../src/ModuleGateway.h"
#include "../src/NullModuleException.h"
#include "ModuleDummy.h"

#include <UnitTest++.h>
#include <algorithm>

using namespace ureport;

SUITE(ModuleGateway)
{
    TEST(GetAllModules_NoModuleAdded_ReturnsEmptySet)
    {
        ModuleGateway& gateway = GetModuleGateway();
        CHECK_EQUAL(0, gateway.GetAllModules().size());
    }

    TEST(AddModule_GivenNull_ThrowsNullModuleException)
    {
        ModuleGateway& gateway = GetModuleGateway();
        CHECK_THROW(gateway.AddModule(nullptr), NullModuleException);
    }

    TEST(AddModule_GivenModule_AddsTheModule)
    {
        ModuleGateway& gateway = GetModuleGateway();
        auto module = std::make_shared<test::ModuleDummy>();
        gateway.AddModule(module);
        ModuleGateway::Modules modules = gateway.GetAllModules();
        CHECK(std::find(modules.begin(), modules.end(), module) != modules.end());
    }
}
