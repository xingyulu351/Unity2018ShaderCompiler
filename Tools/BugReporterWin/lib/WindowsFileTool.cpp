#include "WindowsFileTool.h"

namespace unity
{
    static const size_t kPathBufSize = 1024;

    HANDLE WindowsFileTool::CreateFile(const std::string& name, DWORD desiredAccess, DWORD shareMode,
        LPSECURITY_ATTRIBUTES securityAttributes, DWORD disposition, DWORD flagsAndAttributes,
        HANDLE templateFile)
    {
        WCHAR fileName[kPathBufSize];
        ::MultiByteToWideChar(CP_UTF8, 0, name.c_str(), -1, fileName, kPathBufSize);
        return ::CreateFileW(fileName, desiredAccess, shareMode, securityAttributes,
            disposition, flagsAndAttributes, templateFile);
    }

    bool WindowsFileTool::WriteFile(HANDLE file, LPCVOID buffer, DWORD numberOfBytesToWrite,
        LPDWORD numberOfBytesWritten, LPOVERLAPPED overlapped)
    {
        return ::WriteFile(file, buffer, numberOfBytesToWrite, numberOfBytesWritten, overlapped) == TRUE;
    }

    void WindowsFileTool::DeleteFile(const std::string& name)
    {
        WCHAR fileName[kPathBufSize];
        ::MultiByteToWideChar(CP_UTF8, 0, name.c_str(), -1, fileName, kPathBufSize);
        ::DeleteFileW(fileName);
    }

    bool WindowsFileTool::CreateDirectory(const std::string& name, LPSECURITY_ATTRIBUTES securityAttributes)
    {
        WCHAR dirName[kPathBufSize];
        ::MultiByteToWideChar(CP_UTF8, 0, name.c_str(), -1, dirName, kPathBufSize);
        return ::CreateDirectoryW(dirName, securityAttributes) == TRUE;
    }

    bool WindowsFileTool::RemoveDirectory(const std::string& name)
    {
        WCHAR dirName[kPathBufSize];
        ::MultiByteToWideChar(CP_UTF8, 0, name.c_str(), -1, dirName, kPathBufSize);
        return ::RemoveDirectoryW(dirName) == TRUE;
    }
}
