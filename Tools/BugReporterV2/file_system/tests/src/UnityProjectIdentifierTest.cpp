#include "UnityProjectIdentifier.h"
#include "file_system/FileSystemStub.h"

#include <UnitTest++.h>

SUITE(DirectoryAttachmentUnityProjectIdentifier)
{
    using namespace ureport;
    using namespace ureport::test;

    TEST(IsUnityproject_GivenEmpty_ReturnsFalse)
    {
        std::shared_ptr<FileSystemStub> filesystem = std::make_shared<FileSystemStub>();
        UnityProjectIdentifier identifier(filesystem);
        CHECK_EQUAL(false, identifier.IsUnityProjectDir(""));
    }

    TEST(IsUnityproject_GivenRandomEntry_ReturnsFalse)
    {
        std::shared_ptr<FileSystemStub> filesystem = std::make_shared<FileSystemStub>();
        filesystem->m_DirEntries.push_back("entry");
        UnityProjectIdentifier identifier(filesystem);
        CHECK_EQUAL(false, identifier.IsUnityProjectDir(""));
    }

    TEST(IsUnityproject_GivenEntriesWithOneSympton_ReturnsFalse)
    {
        std::shared_ptr<FileSystemStub> filesystem = std::make_shared<FileSystemStub>();
        filesystem->m_DirEntries.push_back("/some/path/Assets/somefile");
        UnityProjectIdentifier identifier(filesystem);
        CHECK_EQUAL(false, identifier.IsUnityProjectDir(""));
    }

    TEST(IsUnityproject_GivenEntriesWithNotEnoughSymptoms_ReturnsFalse)
    {
        std::shared_ptr<FileSystemStub> filesystem = std::make_shared<FileSystemStub>();
        filesystem->m_DirEntries.push_back("/some/path/Assets/somefile");
        filesystem->m_DirEntries.push_back("/some/path/Library/somefile");
        filesystem->m_DirEntries.push_back("/some/path/ProjectSettings/somefile");
        filesystem->m_DirEntries.push_back("/some/path/.unity");
        UnityProjectIdentifier identifier(filesystem);
        CHECK_EQUAL(false, identifier.IsUnityProjectDir(""));
    }

    TEST(IsUnityproject_GivenEntriesWithEnoughSymptoms_ReturnsTrue)
    {
        std::shared_ptr<FileSystemStub> filesystem = std::make_shared<FileSystemStub>();
        filesystem->m_DirEntries.push_back("/some/path/Assets/somefile");
        filesystem->m_DirEntries.push_back("/some/path/Library/somefile");
        filesystem->m_DirEntries.push_back("/some/path/ProjectSettings/somefile");
        filesystem->m_DirEntries.push_back("/some/path/.unity");
        filesystem->m_DirEntries.push_back("/some/path/GiCache/somefile");
        UnityProjectIdentifier identifier(filesystem);
        CHECK_EQUAL(true, identifier.IsUnityProjectDir(""));
    }

    TEST(IsUnityProject_GivenEntriesThatContainTempFolderNamesInFilenames_ReturnsFalse)
    {
        std::shared_ptr<FileSystemStub> filesystem = std::make_shared<FileSystemStub>();
        filesystem->m_DirEntries.push_back("/some/path/LibraryLoader.cpp");
        filesystem->m_DirEntries.push_back("/some/path/LibraryLoader.cpp");
        filesystem->m_DirEntries.push_back("/some/path/LibraryLoader.cpp");
        filesystem->m_DirEntries.push_back("/some/path/LibraryLoader.cpp");
        filesystem->m_DirEntries.push_back("/some/path/LibraryLoader.cpp");
        filesystem->m_DirEntries.push_back("/some/path/LibraryLoader.cpp");
        filesystem->m_DirEntries.push_back("/some/path/LibraryLoader.cpp");

        UnityProjectIdentifier identifier(filesystem);
        auto isProjectFolder = identifier.IsUnityProjectDir("");

        CHECK_EQUAL(false, isProjectFolder);
    }
}
