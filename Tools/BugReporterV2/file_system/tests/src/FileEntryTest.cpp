#include "FileEntry.h"
#include "AnyPathFileSystem.h"

#include <UnitTest++.h>

SUITE(FileEntry)
{
    using namespace ureport;
    using namespace ureport::details;
    using namespace ureport::test;

    class FileEntryTester
    {
    public:
        FileEntryTester()
            : m_FileSystem(std::make_shared<AnyPathFileSystem>())
        {
        }

        std::shared_ptr<FileSystem> m_FileSystem;
    };

    TEST_FIXTURE(FileEntryTester, IsDir_Returns_False)
    {
        const FileSystemEntry& entry = FileEntry("file.txt", m_FileSystem);
        CHECK_EQUAL(false, entry.IsDir());
    }

    TEST_FIXTURE(FileEntryTester, GetEntries_Returns_Exactly_One_Entry)
    {
        const FileSystemEntry& entry = FileEntry("file.txt", m_FileSystem);
        CHECK_EQUAL(1, entry.GetEntries("").size());
    }

    TEST_FIXTURE(FileEntryTester, GetEntries_First_Entry_Is_The_File_Path)
    {
        const FileSystemEntry& entry = FileEntry("dir/file.txt", m_FileSystem);
        CHECK(entry.GetPath() == entry.GetEntries("").front());
    }
}
