#include "Workspace.h"
#include "WorkspaceIsNotInitializedException.h"
#include "ModuleGatewayFake.h"
#include "ModuleInitializeSpy.h"
#include "ModuleWithCollectors.h"
#include "CollectorDummy.h"
#include "test/AssertContains.h"

#include <UnitTest++.h>
#include <algorithm>

using namespace ureport;
using namespace ureport::test;

SUITE(Workspace)
{
    TEST(GetWorkspace_NotInitializedWorkspace_ThrowsWorkspaceIsNotInitializedException)
    {
        CHECK_THROW(Workspace::GetWorkspace(), WorkspaceIsNotInitializedException);
    }

    TEST(Initialize_GivenOneModule_InitializesTheModule)
    {
        auto module = std::make_shared<test::ModuleInitializeSpy>();
        test::ModuleGatewayFake modules;
        modules.AddModule(module);
        const Workspace& workspace = Workspace::Initialize(modules);
        CHECK_EQUAL(1, module->GetCallCount());
    }

    TEST(CanRegisterCollector)
    {
        using namespace ureport::test;
        std::shared_ptr<Collector> collectors[1];
        collectors[0] = std::make_shared<CollectorDummy>();
        ModuleGatewayFake modules;
        modules.AddModule(std::make_shared<ModuleWithCollectors>(collectors));
        const Workspace& workspace = Workspace::Initialize(modules);
        const auto registered = workspace.GetAllCollectors();
        AssertContains(registered, collectors[0]);
    }
}
