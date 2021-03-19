#pragma once

#include "shims/logical/IsEmpty.h"

#include <string>
#include <exception>

namespace ureport
{
namespace macosx
{
    class CrashReport
    {
        std::string m_Path;
        std::string m_Application;
        std::string m_Timestamp;

    public:
        class EmptyReportPathException : public std::exception
        {
        };

    public:
        std::string GetPath() const
        {
            return m_Path;
        }

        std::string GetApplication() const
        {
            return m_Application;
        }

        std::string GetTimestamp() const
        {
            return m_Timestamp;
        }

        static CrashReport MakeFromPath(const std::string& path)
        {
            CheckPath(path);
            CrashReport report;
            report.m_Path = path;
            report.m_Application = GetApplication(path);
            report.m_Timestamp = GetTimestamp(path);
            return report;
        }

        static void CheckPath(const std::string& path)
        {
            if (IsEmpty(path))
                throw EmptyReportPathException();
        }

        static std::string GetApplication(const std::string& path)
        {
            auto const base = GetBaseName(path);
            return base.substr(0, base.find_first_of('_'));
        }

        static std::string GetBaseName(const std::string& path)
        {
            auto const pos = path.find_last_of('/');
            auto const nameStart = pos == std::string::npos ? 0 : pos + 1;
            auto const suffixStart =  path.find_last_of('.');
            return path.substr(nameStart, suffixStart - nameStart);
        }

        static std::string GetTimestamp(const std::string& path)
        {
            auto const base = GetBaseName(path);
            auto pos = base.find_first_of('_');
            auto const timeStart = pos == std::string::npos ? 0 : pos + 1;
            pos = base.find_first_of('_', timeStart);
            return base.substr(timeStart, pos - timeStart);
        }
    };
}
}
