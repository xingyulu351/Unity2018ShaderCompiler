#include "UnityPrefix.h"
#include "FileInformation.h"
#include "Runtime/Core/Containers/String.h"
#include "WinDriverUtils.h"
#include <cstdint>
#include <memory>
#include <sstream>
#include <vector>

namespace systeminfo
{
    core::string GetOperatingSystem();
    core::string GetProcessorType();
    int GetProcessorCount();
}

namespace unity_bridge
{
namespace internal
{
namespace
{
    const uint32_t bufferSize = 1024;
    core::string operatingSystem;
    core::string processorType;
    core::string displayDriverName;
    core::string displayDriverVersion;
    core::string fileVersion;
    core::string productVersion;
    std::vector<char> companyName(bufferSize);
    std::vector<char> productName(bufferSize);
    std::vector<char> fileDescription(bufferSize);
}

    __declspec(dllexport) char const* GetOperatingSystem()
    {
        operatingSystem = systeminfo::GetOperatingSystem();
        return operatingSystem.c_str();
    }

    __declspec(dllexport) char const* GetProcessorType()
    {
        processorType = systeminfo::GetProcessorType();
        return processorType.c_str();
    }

    __declspec(dllexport) int GetProcessorCount()
    {
        return systeminfo::GetProcessorCount();
    }

    __declspec(dllexport) char const* GetDisplayDriverName()
    {
        windriverutils::VersionInfo version;
        windriverutils::GetDisplayDriverInfoRegistry(nullptr, &displayDriverName,
            version);
        return displayDriverName.c_str();
    }

    __declspec(dllexport) char const* GetDisplayDriverVersion()
    {
        windriverutils::VersionInfo version;
        windriverutils::GetDisplayDriverInfoRegistry(nullptr, &displayDriverName,
            version);
        std::stringstream versionText;
        versionText << HIWORD(version.hipart) << "."
        << LOWORD(version.hipart) << "."
        << HIWORD(version.lopart) << "."
        << LOWORD(version.lopart);
        displayDriverVersion = versionText.str();
        return displayDriverVersion.c_str();
    }

    std::string GetVersionText(const WORD(&version)[4])
    {
        std::stringstream text;
        text << version[0] << "." << version[1] << "." << version[2] << "."
        << version[3];
        return text.str();
    }

    std::string GetVersionText(const FileInformation::Version& version)
    {
        std::stringstream text;
        text << version.v1 << "." << version.v2 << "." << version.v3 << "."
        << version.v4;
        return text.str();
    }

    __declspec(dllexport) char const* GetFileVersion(wchar_t const* path)
    {
        FileInformation fileInfo(path);
        fileVersion = GetVersionText(fileInfo.GetFileVersion());
        return fileVersion.c_str();
    }

    __declspec(dllexport) char const* GetFileDescription(wchar_t const* path)
    {
        FileInformation fileInfo(path);
        fileInfo.GetFileDescription(&fileDescription[0], fileDescription.size());
        return &fileDescription[0];
    }

    __declspec(dllexport) char const* GetProductName(wchar_t const* path)
    {
        FileInformation fileInfo(path);
        fileInfo.GetProductName(&productName[0], productName.size());
        return &productName[0];
    }

    __declspec(dllexport) char const* GetProductVersion(wchar_t const* path)
    {
        FileInformation fileInfo(path);
        productVersion = GetVersionText(fileInfo.GetProductVersion());
        return productVersion.c_str();
    }

    __declspec(dllexport) char const* GetCompanyName(wchar_t const* path)
    {
        FileInformation fileInfo(path);
        fileInfo.GetCompanyName(&companyName[0], companyName.size());
        return &companyName[0];
    }

    __declspec(dllexport) unsigned int GetVideoMemorySizeMB(void* monitor)
    {
        wchar_t const* method = nullptr;
        return windriverutils::GetVideoMemorySizeMB(
            reinterpret_cast<HMONITOR>(monitor), &method);
    }
}
}
