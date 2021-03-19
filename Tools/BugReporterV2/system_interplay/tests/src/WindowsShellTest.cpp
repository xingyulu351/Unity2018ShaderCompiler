#include "WindowsShell.h"
#include "shims/logical/IsEmpty.h"

#include <UnitTest++.h>

SUITE(WindowsShell)
{
    using namespace ::ureport;

    TEST(ExecuteDirForCurrentDirectory_ReturnsNonEmptyString)
    {
        windows::Shell shell;
        CHECK(!IsEmpty(shell.ExecuteCommand("dir .")));
    }
}
