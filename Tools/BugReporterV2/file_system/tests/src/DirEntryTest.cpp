#include "DirEntry.h"
#include "file_system/FileSystemStub.h"

#include <UnitTest++.h>

SUITE(DirEntry)
{
    using namespace ureport;
    using namespace ureport::details;
    using namespace ureport::test;

    TEST(IsDir_Returns_True)
    {
        auto fileSystem = std::make_shared<FileSystemStub>();
        const FileSystemEntry& entry = DirEntry("dir", fileSystem);
        CHECK_EQUAL(true, entry.IsDir());
    }

    TEST(GetEntries_Returns_Children_Paths)
    {
        auto fileSystem = std::make_shared<FileSystemStub>();
        fileSystem->m_DirEntries.push_back("dir/child");
        const FileSystemEntry& entry = DirEntry("dir", fileSystem);
        CHECK_EQUAL("dir/child", entry.GetEntries("").front());
    }
}
