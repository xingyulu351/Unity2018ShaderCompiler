#pragma once

#include <string>

namespace unity_bridge
{
namespace internal
{
    char const* GetOperatingSystem();
    char const* GetProcessorType();
    int GetProcessorCount();
    char const* GetDisplayDriverName();
    char const* GetDisplayDriverVersion();
    char const* GetFileVersion(wchar_t const* path);
    char const* GetFileDescription(wchar_t const* path);
    char const* GetProductName(wchar_t const* path);
    char const* GetProductVersion(wchar_t const* path);
    char const* GetCompanyName(wchar_t const* path);
    unsigned int GetVideoMemorySizeMB(void* monitor);
}

    inline std::string GetOperatingSystem()
    {
        return std::string(internal::GetOperatingSystem());
    }

    inline std::string GetProcessorType()
    {
        return std::string(internal::GetProcessorType());
    }

    inline int GetProcessorCount()
    {
        return internal::GetProcessorCount();
    }

    inline std::string GetDisplayDriverName()
    {
        return std::string(internal::GetDisplayDriverName());
    }

    inline std::string GetDisplayDriverVersion()
    {
        return std::string(internal::GetDisplayDriverVersion());
    }

    inline std::string GetCompanyName(wchar_t const* path)
    {
        return std::string(internal::GetCompanyName(path));
    }

    inline std::string GetFileVersion(wchar_t const* path)
    {
        return std::string(internal::GetFileVersion(path));
    }

    inline std::string GetFileDescription(wchar_t const* path)
    {
        return std::string(internal::GetFileDescription(path));
    }

    inline std::string GetProductName(wchar_t const* path)
    {
        return std::string(internal::GetProductName(path));
    }

    inline std::string GetProductVersion(wchar_t const* path)
    {
        return std::string(internal::GetProductVersion(path));
    }

    inline unsigned int GetVideoMemorySizeMB(void* monitor)
    {
        return internal::GetVideoMemorySizeMB(monitor);
    }
}
