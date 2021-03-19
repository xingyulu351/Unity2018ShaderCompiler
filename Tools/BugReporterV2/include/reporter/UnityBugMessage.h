#pragma once

#include <string>
#include <vector>
#include <memory>

namespace ureport
{
    class Report;
    class FilesPacker;
    class File;
    class LongTermOperObserver;

    class UnityBugMessage
    {
        std::string m_UnityVersion;
        std::string m_Event;
        std::string m_CustomerEmail;
        std::string m_Computer;
        std::string m_Timestamp;
        std::shared_ptr<File> m_AttachedPackedFile;
        int m_BugTypeID;
        std::string m_BugReproducibility;

    public:

        std::string GetUnityVersion() const { return m_UnityVersion; }

        std::string GetEvent() const { return m_Event; }

        std::string GetCustomerEmail() const { return m_CustomerEmail; }

        std::string GetComputer() const { return m_Computer; }

        std::string GetAttachedFilePath() const;

        std::string GetTimestamp() const { return m_Timestamp; }

        int GetBugTypeID() const { return m_BugTypeID; }

        std::string GetBugReproducibility() const { return m_BugReproducibility; }

        static UnityBugMessage Compose(
            const Report& report,
            FilesPacker& packer,
            LongTermOperObserver* observer);
    };
}
