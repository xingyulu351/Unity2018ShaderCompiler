#include "ZipFilesPacker.h"
#include "ZipFile.h"
#include "ZipFileDummy.h"
#include "ZipFileSpy.h"
#include "FileFake.h"
#include "FileGatewayDummy.h"
#include "SystemEnvironmentDummy.h"
#include "reporter/FilesPacker.h"
#include "shims/logical/IsNull.h"

#include <UnitTest++.h>
#include <memory>
#include <vector>

SUITE(ZipFilePacker)
{
    using namespace ureport;
    using namespace ureport::test;

    TEST(Pack_GivenNoFiles_ReturnsNullFile)
    {
        auto packer = CreateZipFilesPacker(std::make_shared<FileGatewayDummy>(),
            std::make_shared<SystemEnvironmentDummy>());
        std::vector<FilesPacker::FileRef> files;
        CHECK(IsNull(packer->Pack(files, nullptr)));
    }
}
