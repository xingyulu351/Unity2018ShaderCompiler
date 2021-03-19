#include "ZipUtils.h"
#include "ZipFile.h"
#include "EmptyZipPathException.h"
#include "shims/logical/IsNull.h"
#include "shims/attribute/GetCString.h"
#include "test/AssertFileExists.h"
#include "ZipFileSpy.h"
#include "FileFake.h"

#include <UnitTest++.h>
#include <algorithm>
#include <cassert>
#include <cstdio>
#include <string>
#include <vector>

SUITE(ZipUtils)
{
    using namespace ureport;
    using namespace ureport::test;

    TEST(CreateNewZip_GivenEmptyPath_ThrowsEmptyZipPathException)
    {
        CHECK_THROW(CreateNewZip(""), EmptyZipPathException);
    }

    class Utils
    {
        std::vector<std::string> m_Files;

    public:
        void DeleteAfterTest(const std::string file)
        {
            m_Files.push_back(file);
        }

        ~Utils()
        {
            std::for_each(m_Files.begin(), m_Files.end(),
                [] (const std::string& file) {
                    auto const deleted = std::remove(GetCString(file));
                    assert(deleted == 0);
                });
        }
    };

    TEST_FIXTURE(Utils, CreateNewZip_GivenNonEmptyPath_CreatesAndReturnsZipFile)
    {
        auto zip = CreateNewZip("file.zip");
        DeleteAfterTest("file.zip");
        CHECK(!IsNull(zip));
        AssertFileExists("file.zip");
        zip.reset();
    }

    TEST(Pack_GivenFile_OpensNewEntryWithPathOfGivenFile)
    {
        ZipFileSpy zip;
        FileFake file("~/directory/file");
        Pack(zip, file);
        CHECK(zip.GetEntryName() == "~/directory/file");
    }

    TEST(Pack_GivenFile_WritesItsContent)
    {
        ZipFileSpy zip;
        FileFake file("");
        file.SetContent("Content");
        Pack(zip, file);
        CHECK_EQUAL("Content", zip.GetEntryContent());
    }
}
