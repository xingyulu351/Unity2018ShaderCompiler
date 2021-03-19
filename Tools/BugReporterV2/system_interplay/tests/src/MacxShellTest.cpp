#include "PosixShell.h"
#include "shims/logical/IsEmpty.h"

#include <UnitTest++.h>

SUITE(MacxShell)
{
    using namespace ::ureport;

    TEST(ExecutePrintWorkingDir_ReturnsNonEmptyString)
    {
        posix::Shell shell;
        CHECK(!IsEmpty(shell.ExecuteCommand("pwd")));
    }
}
