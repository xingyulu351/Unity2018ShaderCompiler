// Unity Crash Handler, somewhat based on XCrashReport by Hans Dietrich:
//   http://www.codeproject.com/debug/XCrashReportPt4.asp

#include "UnityPrefix.h"
#include "FileUtilities.h"
#include <assert.h>
#include <objbase.h>
#include <cstring>

const char* crhfileutils::GetFilePart(const char* lpszFile)
{
    const char *result = strrchr(lpszFile, '\\');
    if (result)
        result++;
    else
        result = lpszFile;
    return result;
}

const wchar_t* crhfileutils::GetFilePartWide(const wchar_t* lpszFile)
{
    const wchar_t *result = wcsrchr(lpszFile, L'\\');
    if (result)
        result++;
    else
        result = lpszFile;
    return result;
}

std::wstring crhfileutils::GetTempDirectoryWide()
{
    wchar_t buf[kDefaultPathBufferSize];
    ::GetTempPathW(kDefaultPathBufferSize, buf);
    // try creating it, just in case it does not exist
    ::CreateDirectoryW(buf, NULL);
    return buf;
}

std::string crhfileutils::GetTempDirectory()
{
    std::wstring buf = GetTempDirectoryWide();

    // return UTF8 string
    std::string bufUtf8;
    ConvertWindowsPathName(buf, bufUtf8);
    return bufUtf8;
}

namespace
{
    const char kHexToLiteral[16] = {'0', '1', '2', '3', '4', '5', '6', '7', '8', '9', 'a', 'b', 'c', 'd', 'e', 'f'};
}

std::string crhfileutils::GenerateGuidString()
{
    GUID guid;
    ::CoCreateGuid(&guid);
    const int GUID_SIZE = sizeof(guid);

    std::string res;
    res.resize(GUID_SIZE * 2);

    const unsigned char* data = (const unsigned char*)&guid;
    for (int i = 0; i < GUID_SIZE; ++i)
    {
        unsigned char d = data[i];
        res[i * 2 + 0] = kHexToLiteral[d >> 4];
        res[i * 2 + 1] = kHexToLiteral[d & 0xF];
    }

    return res;
}

static bool IsNameDots(const char* str)
{
    return (strcmp(str, ".") == 0) || (strcmp(str, "..") == 0);
}

bool crhfileutils::RemoveDirectoryRecursiveWide(const std::wstring& path)
{
    if (path.empty())
        return false;

    // base path
    std::wstring basePath = path;
    if (basePath[basePath.size() - 1] != L'\\')
        basePath += L'\\';

    // search pattern: anything inside the directory
    std::wstring searchPat = basePath + L'*';

    // find the first file
    WIN32_FIND_DATAW findData;
    HANDLE hFind = ::FindFirstFileW(searchPat.c_str(), &findData);
    if (hFind == INVALID_HANDLE_VALUE)
        return false;

    bool hadFailures = false;

    bool bSearch = true;
    while (bSearch)
    {
        if (::FindNextFileW(hFind, &findData))
        {
            if (wcscmp(findData.cFileName, L".") == 0 || wcscmp(findData.cFileName, L"..") == 0)
                continue;

            std::wstring filePath = basePath + findData.cFileName;
            if ((findData.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY))
            {
                // we have found a directory, recurse
                if (!RemoveDirectoryRecursiveWide(filePath))
                {
                    hadFailures = true;
                }
                else
                {
                    ::RemoveDirectoryW(filePath.c_str());   // remove the empty directory
                }
            }
            else
            {
                if (findData.dwFileAttributes & FILE_ATTRIBUTE_READONLY)
                    ::SetFileAttributesW(filePath.c_str(), FILE_ATTRIBUTE_NORMAL);
                if (!::DeleteFileW(filePath.c_str()))
                {
                    hadFailures = true;
                }
            }
        }
        else
        {
            if (::GetLastError() == ERROR_NO_MORE_FILES)
            {
                bSearch = false; // no more files there
            }
            else
            {
                // some error occurred
                ::FindClose(hFind);
                return false;
            }
        }
    }
    ::FindClose(hFind);

    if (!RemoveDirectoryW(path.c_str()))      // remove the empty directory
    {
        hadFailures = true;
    }

    return !hadFailures;
}

bool crhfileutils::RemoveDirectoryRecursive(const std::string& pathUtf8)
{
    std::wstring path;
    ConvertUnityPathName(pathUtf8, path);
    return RemoveDirectoryRecursiveWide(path);
}
