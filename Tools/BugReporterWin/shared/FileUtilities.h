// Unity Crash Handler, somewhat based on XCrashReport by Hans Dietrich:
//   http://www.codeproject.com/debug/XCrashReportPt4.asp

#pragma once

#include <string>

namespace crhfileutils
{
    const char* GetFilePart(const char* lpszFile);
    const wchar_t* GetFilePartWide(const wchar_t* lpszFile);

    std::wstring GetTempDirectoryWide();
    std::string GetTempDirectory();
    std::string GenerateGuidString();

    bool RemoveDirectoryRecursive(const std::string& path);
    bool RemoveDirectoryRecursiveWide(const std::wstring& path);
}  // namespace utils
