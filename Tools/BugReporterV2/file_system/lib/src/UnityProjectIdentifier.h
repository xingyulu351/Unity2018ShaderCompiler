#pragma once

#include <list>
#include <memory>
#include <string>
#include <vector>

#include "file_system/FileSystem.h"

namespace ureport
{
    class UnityProjectIdentifier
    {
    public:
        UnityProjectIdentifier(std::shared_ptr<FileSystem> fileSystem)
            : m_FileSystem(fileSystem)
            , m_FolderContentMatchTheshold(0.5f)
        {
            m_UnityProjectFolderContent.push_back("/Assets/");
            m_UnityProjectFolderContent.push_back("/Library/");
            m_UnityProjectFolderContent.push_back("/ProjectSettings/");
            m_UnityProjectFolderContent.push_back(".unity");
            m_UnityProjectFolderContent.push_back("/GiCache/");
            m_UnityProjectFolderContent.push_back("CurrentLayout.dwlt");
            m_UnityProjectFolderContent.push_back("EditorUserBuildSettings.asset");
            m_UnityProjectFolderContent.push_back("ProjectSettings.asset");
            m_UnityProjectFolderContent.push_back("EditorBuildSettings.asset");
            m_UnityProjectFolderContent.push_back("InputManager.asset");
        }

        bool IsUnityProjectDir(const std::string& path) const
        {
            auto dirEntries = GetDirEntries(path);
            if (IsEnoughMatchingEntries(dirEntries))
                return true;
            return false;
        }

    private:
        std::list<std::string> GetDirEntries(const std::string& path) const
        {
            return m_FileSystem->GetDirEntries(path, "");
        }

        bool IsEnoughMatchingEntries(const std::list<std::string>& entries) const
        {
            return CountMatchingEntries(entries) >= m_UnityProjectFolderContent.size() *
                m_FolderContentMatchTheshold;
        }

        int CountMatchingEntries(const std::list<std::string>& entries) const
        {
            int matchingEntries = 0;

            for (auto entry = entries.begin(); entry != entries.end(); ++entry)
            {
                for (auto projectFolderEntry = m_UnityProjectFolderContent.begin();
                     projectFolderEntry != m_UnityProjectFolderContent.end();
                     ++projectFolderEntry)
                {
                    if ((*entry).find(*projectFolderEntry) != std::string::npos)
                        ++matchingEntries;
                }
            }

            return matchingEntries;
        }

    private:
        std::shared_ptr<FileSystem> m_FileSystem;
        std::vector<std::string> m_UnityProjectFolderContent;
        const float m_FolderContentMatchTheshold;
    };
}
