#pragma once

#include <Windows.h>
#include <memory>
#include <string>

namespace unity
{
    class WindowsFileTool
    {
    public:
        virtual HANDLE CreateFile(const std::string& name, DWORD desiredAccess, DWORD shareMode,
            LPSECURITY_ATTRIBUTES securityAttributes, DWORD disposition, DWORD flagsAndAttributes,
            HANDLE templateFile);

        virtual bool WriteFile(HANDLE file, LPCVOID buffer, DWORD numberOfBytesToWrite,
            LPDWORD numberOfBytesWritten, LPOVERLAPPED overlapped);

        virtual void DeleteFile(const std::string& name);

        virtual bool CreateDirectory(const std::string& name, LPSECURITY_ATTRIBUTES securityAttributes);

        virtual bool RemoveDirectory(const std::string& name);

        virtual ~WindowsFileTool() {}
    };
}
