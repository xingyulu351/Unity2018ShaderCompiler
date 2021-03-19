#include "ProjectDirEntry.h"
#include "file_system/FileSystemStub.h"

#include <UnitTest++.h>
#include <list>

SUITE(ProjectDirEntry)
{
    using namespace ureport;
    using namespace ureport::test;
    using namespace ureport::details;

    class ProjectDirEntryTester
    {
    public:
        std::shared_ptr<FileSystemStub> m_FileSystem;
        ProjectDirEntry m_ProjectDir;

        ProjectDirEntryTester()
            : m_FileSystem(std::make_shared<FileSystemStub>())
            , m_ProjectDir("", m_FileSystem)
        {
        }

        void AssertDirEntriesEqual(std::list<std::string> expected,
            std::list<std::string> actual) const
        {
            actual.sort();
            expected.sort();
            if (expected.size() == actual.size())
                CHECK(std::equal(expected.begin(), expected.end(), actual.begin()));
            CHECK_EQUAL(expected.size(), actual.size());
        }
    };

    TEST_FIXTURE(ProjectDirEntryTester, GetEntries_GivenEmptyDir_ReturnsEmpty)
    {
        CHECK_EQUAL(0, m_ProjectDir.GetEntries("").size());
    }

    TEST_FIXTURE(ProjectDirEntryTester, GetEntries_GivenOneEntryInDir_ReturnsOneEntry)
    {
        m_FileSystem->m_DirEntries.push_back("project/subdir");
        CHECK_EQUAL(1, m_ProjectDir.GetEntries("").size());
    }

    TEST_FIXTURE(ProjectDirEntryTester, GetEntries_GivenOneRegularAndOneTempEntry_ReturnsOnlyRegular)
    {
        m_FileSystem->m_DirEntries.push_back("project/subdir");
        m_FileSystem->m_DirEntries.push_back("project/Temp");

        std::list<std::string> expected;
        expected.push_back("project/subdir");

        AssertDirEntriesEqual(expected, m_ProjectDir.GetEntries(""));
    }

    TEST_FIXTURE(ProjectDirEntryTester, GetEntries_GivenOneRegularAndOneTempEntryWithSubentries_ReturnsOnlyRegular)
    {
        m_FileSystem->m_DirEntries.push_back("project/subdir");
        m_FileSystem->m_DirEntries.push_back("project/Temp");
        m_FileSystem->m_DirEntries.push_back("project/Temp/a");
        m_FileSystem->m_DirEntries.push_back("project/Temp/a/b");

        std::list<std::string> expected;
        expected.push_back("project/subdir");

        AssertDirEntriesEqual(expected, m_ProjectDir.GetEntries(""));
    }

    TEST_FIXTURE(ProjectDirEntryTester, GetEntries_GivenSeveralRegularAndThreeTempEntries_ReturnsOnlyRegular)
    {
        m_FileSystem->m_DirEntries.push_back("project/subdir1");
        m_FileSystem->m_DirEntries.push_back("project/subdir2");
        m_FileSystem->m_DirEntries.push_back("project/subdir3");
        m_FileSystem->m_DirEntries.push_back("project/Temp");
        m_FileSystem->m_DirEntries.push_back("project/Temp/entry");
        m_FileSystem->m_DirEntries.push_back("project/Library/GiCache");
        m_FileSystem->m_DirEntries.push_back("project/Library/ShaderCache");
        m_FileSystem->m_DirEntries.push_back("project/Library/ShaderCache/entry");

        std::list<std::string> expected;
        expected.push_back("project/subdir1");
        expected.push_back("project/subdir2");
        expected.push_back("project/subdir3");

        AssertDirEntriesEqual(expected, m_ProjectDir.GetEntries(""));
    }

    TEST_FIXTURE(ProjectDirEntryTester, GetEntries_ProjectUnderTempDirectory_ReturnsAllProjectEntries)
    {
        m_FileSystem->m_DirEntries.push_back("Temp/project/a");
        m_FileSystem->m_DirEntries.push_back("Temp/project/a/b");
        std::list<std::string> expected;
        expected.push_back("Temp/project/a");
        expected.push_back("Temp/project/a/b");
        AssertDirEntriesEqual(expected, m_ProjectDir.GetEntries(""));
    }

    TEST_FIXTURE(ProjectDirEntryTester, GetEntries_ProjectUnderShaderCacheDirectory_ReturnsAllProjectEntries)
    {
        m_FileSystem->m_DirEntries.push_back("Library/ShaderCache/project/a");
        m_FileSystem->m_DirEntries.push_back("Library/ShaderCache/project/a/b");
        std::list<std::string> expected;
        expected.push_back("Library/ShaderCache/project/a");
        expected.push_back("Library/ShaderCache/project/a/b");
        AssertDirEntriesEqual(expected, m_ProjectDir.GetEntries(""));
    }

    TEST_FIXTURE(ProjectDirEntryTester, GetEntries_ProjectUnderGiCacheDirectory_ReturnsAllProjectEntries)
    {
        m_FileSystem->m_DirEntries.push_back("Library/GiCache/project/a");
        m_FileSystem->m_DirEntries.push_back("Library/GiCache/project/a/b");
        std::list<std::string> expected;
        expected.push_back("Library/GiCache/project/a");
        expected.push_back("Library/GiCache/project/a/b");
        AssertDirEntriesEqual(expected, m_ProjectDir.GetEntries(""));
    }
}
