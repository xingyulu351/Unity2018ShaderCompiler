#include "BasicFileGateway.h"
#include "File.h"
#include "EmptyPathException.h"
#include "PathDoesNotExistException.h"
#include "AnyPathFileSystem.h"
#include "NoPathFileSystem.h"

#include <UnitTest++.h>
#include <memory>

SUITE(BasicFileGateway)
{
    using namespace ureport;
    using namespace ureport::test;

    TEST(GetFile_GivenEmptyPath_ThrowsEmptyPathException)
    {
        const auto gateway = CreateBasicFileGateway(nullptr);
        CHECK_THROW(gateway->GetFile(""), EmptyPathException);
    }

    TEST(GetFile_GivenNonExistentPath_ThrowsPathDoesNotExistException)
    {
        const auto gateway = CreateBasicFileGateway(std::make_shared<NoPathFileSystem>());
        CHECK_THROW(gateway->GetFile("file"), PathDoesNotExistException);
    }

    TEST(GetFile_GivenNonEmptyPath_ReturnsFileWithThePath)
    {
        const auto gateway = CreateBasicFileGateway(std::make_shared<AnyPathFileSystem>());
        const auto file = gateway->GetFile("file");
        CHECK(file->GetPath() == "file");
    }
}
