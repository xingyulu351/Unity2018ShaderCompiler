#include "UnityPrefix.h"
#include "ProcessIncludes.h"
#include "ShaderImportErrors.h"
#include "Runtime/Core/Containers/hash_map.h"
#include "Runtime/Threads/Mutex.h"
#include "Runtime/Utilities/PathNameUtility.h"
#include "Runtime/Utilities/Word.h"
#include "Editor/Src/Utility/TextUtilities.h"
#include "Editor/Src/AssetPipeline/ShaderImporter.h"
#include "External/HashFunctions/SpookyV2.h"

#if PLATFORM_WIN
#include "PlatformDependent/Win/PathUnicodeConversion.h"
#include <fileapi.h>
#include <handleapi.h>
#else
#include <sys/stat.h>
#include <unistd.h>
#endif
#include <set>

using core::string;

const int kMaxIncludeDepth = 16;

typedef bool (*ReadTextFileFunc)(const core::string& pathName, core::string& output);

typedef core::hash_map<core::string, core::string> IncludedFilesCache;

static IncludedFilesCache s_IncludesCache;
static Mutex s_IncludesCacheMutex;


// This helper func finds the next include directive in the shader source code string
// starting from the given position. Returns position after the "#include" part.
static size_t FindNextIncludeDirective(const core::string &shaderSource, size_t startPos)
{
    size_t pos = startPos;

    while (pos != core::string::npos && pos < shaderSource.length())
    {
        pos = shaderSource.find('#', pos);
        if (pos != core::string::npos)
        {
            pos = shaderSource.find_first_not_of(" \t", pos + 1);
            if (strncmp(shaderSource.c_str() + pos, "include", 7) == 0)
                return pos + 7; // return position right after "#include"
        }
    }

    return core::string::npos; // did not find
}

// Goes through the given shader source code string looking for lines beginning with #include.
// Then replaces all backslashes on those lines with forward ones.
// Not utilizing e.g. ConvertSeparatorsToUnity to avoid some string copying back and forth.
void ConvertIncludePathSlashes(core::string &shaderSource)
{
    size_t pos = FindNextIncludeDirective(shaderSource, 0);

    while (pos != core::string::npos)
    {
        size_t endPos = shaderSource.find('\n', pos);
        while (pos < endPos && pos < shaderSource.length())
        {
            if (shaderSource[pos] == '\\')
                shaderSource[pos] = '/';

            pos++;
        }
        pos = FindNextIncludeDirective(shaderSource, pos);
    }
}

bool WriteTextToFile(const char* fname, const char* text)
{
    const size_t length = strlen(text);

#   if PLATFORM_WIN

    core::wstring widePath;
    ConvertUnityPathName(fname, widePath);
    HANDLE fileHandle = CreateFileW(widePath.c_str(), FILE_GENERIC_WRITE, 0, NULL, CREATE_ALWAYS, NULL, NULL);
    if (fileHandle == INVALID_HANDLE_VALUE)
        return false;

    DWORD bytesWritten = 0;
    BOOL ok = WriteFile(fileHandle, text, static_cast<DWORD>(length), &bytesWritten, NULL);
    if (!ok || bytesWritten != length)
    {
        CloseHandle(fileHandle);
        return false;
    }

    CloseHandle(fileHandle);
    return true;

#   else // if PLATFORM_WIN

    FILE* f = fopen(fname, "wb");
    if (!f)
        return false;
    fwrite(text, length, 1, f);
    fclose(f);
    return true;

#   endif // !if PLATFORM_WIN
}

void CreateDirectoryAtPath(const char* name)
{
#   if PLATFORM_WIN

    core::wstring widePath;
    ConvertUnityPathName(name, widePath);
    CreateDirectoryW(widePath.c_str(), NULL);

#   else // if PLATFORM_WIN

    mkdir(name, 0755);

#   endif // !if PLATFORM_WIN
}

void DeleteDirectoryAtPath(const char* name)
{
#   if PLATFORM_WIN

    core::wstring widePath;
    ConvertUnityPathName(name, widePath);
    RemoveDirectoryW(widePath.c_str());

#   else // if PLATFORM_WIN

    rmdir(name);

#   endif // !if PLATFORM_WIN
}

bool DeleteFileAtPath(const char* fname)
{
#   if PLATFORM_WIN

    core::wstring widePath;
    ConvertUnityPathName(fname, widePath);
    bool result = DeleteFileW(widePath.c_str()) ? true : false;

#   else // if PLATFORM_WIN

    bool result = (unlink(fname) == 0);

#   endif // !if PLATFORM_WIN

    return result;
}

bool ReadTextFile(const core::string& pathName, core::string& output)
{
    {
        Mutex::AutoLock lock(s_IncludesCacheMutex);
        IncludedFilesCache::const_iterator it = s_IncludesCache.find(pathName);
        if (it != s_IncludesCache.end())
        {
            output = it->second;
            return true;
        }
    }

#   if PLATFORM_WIN
    core::wstring widePath;
    ConvertUnityPathName(pathName, widePath);
    // Convert to unicode path to overcome max 260 chars limit
    wchar_t fullPath[1024] = L"\\\\?\\";
    GetFullPathNameW(widePath.c_str(), 1020, fullPath + 4, NULL);
    FILE* file = _wfopen(fullPath, L"rt");
#   else // if PLATFORM_WIN
    FILE* file = fopen(pathName.c_str(), "rt");
#   endif // !if PLATFORM_WIN

    if (file == NULL)
        return false;

    fseek(file, 0, SEEK_END);
    int rawLength = ftell(file);
    fseek(file, 0, SEEK_SET);
    if (rawLength < 0)
    {
        fclose(file);
        return false;
    }

    output.resize(rawLength);
    size_t readLength = fread(&*output.begin(), 1, rawLength, file);
    fclose(file);

    // We're reading in text mode, so CRLF can be turned into just LF; in which case read length will be smaller than actual file
    // length.
    output.resize(readLength);
    RemoveUTF8BOM(output);
    ConvertIncludePathSlashes(output);

    // Only cache includes with standard extensions as otherwise the content could be modified
    // unnoticed keeping invalid version in the cache.
    const core::string ext = GetPathNameExtension(pathName);
    if (IsShaderIncludeFileExt(ext.c_str()))
    {
        Mutex::AutoLock lock(s_IncludesCacheMutex);
        s_IncludesCache.insert(pathName, output);
    }

    return true;
}

void ClearReadTextFilesCache()
{
    s_IncludesCache.clear_dealloc();
}

static size_t FindIncludeLine(const core::string& codeString, size_t startPos, size_t& outTokenLen)
{
    while (startPos < codeString.length())
    {
        outTokenLen = 7;

        // find next occurrence of include directive by first finding string "include"
        size_t includePos = FindTokenInText(codeString, "include", startPos);
        if (includePos == string::npos)
            return string::npos;

        // then search backwards for # making sure there are only spaces or tabs in between
        size_t helperPos = includePos;
        while (helperPos >= startPos)
        {
            helperPos--;
            outTokenLen++;

            if (codeString[helperPos] == '#')
                return helperPos;
            else if (codeString[helperPos] != ' ' && codeString[helperPos] != '\t')
                break; // invalid token -> skip this and try to find a new one
        }

        startPos = includePos + 7;
    }

    return string::npos;
}

core::string ResolveRedirectedPath(const core::string& path, const IncludeFolderMap& redirectedFolders)
{
    // Note: Atm we use redirected folders for packages which are always of format
    // Packages/packagename/the-rest-of-the-path
    // Therefore we specialize the search for this pattern:
    // Replace the Packages/packagename with the absolute location
    size_t searchPos = 0;
    for (int i = 0; i < 2; i++) // Find the second slash in the path
    {
        searchPos = path.find_first_of(kPathNameSeparator, searchPos);
        if (searchPos == 0 || searchPos == core::string::npos)
            return "";
        searchPos++;
    }

    core::string pathStart = path.substr(0, searchPos - 1);
    IncludeFolderMap::iterator itr = redirectedFolders.find(pathStart);
    if (itr != redirectedFolders.end())
    {
        core::string fullPath = AppendPathName(itr->second, path.substr(searchPos));
        ConvertSeparatorsToUnity(fullPath);
        return fullPath;
    }

    return "";
}

static bool ProcessIncludesRecursive(
    const core::string& parentFileName,
    core::string& codeString,
    int includeDepth,
    const IncludeSearchContext& includeContext,
    ReadTextFileFunc readFileFunc,
    ShaderImportErrors& outErrors,
    ShaderCompilerPlatform api)
{
    ++includeDepth;
    if (includeDepth > kMaxIncludeDepth)
    {
        outErrors.AddImportError("Maximum include depth reached", 0, false, api);
        return false;
    }

    bool ok = true;

    size_t includePos = 0;
    int prevIncludeLine = 0;
    while (true)
    {
        const size_t prevIncludePos = includePos;
        size_t tokenLen;
        // find next occurrence of include directive
        includePos = FindIncludeLine(codeString, includePos, tokenLen);

        if (includePos == string::npos)
            break; // no more includes found, break the loop

        // extract line after include token
        core::string includeLine = ExtractRestOfLine(codeString, includePos);
        core::string line = includeLine.substr(tokenLen);
        const int includeLineNumber = (int)CountLines(codeString, prevIncludePos, includePos, true, prevIncludeLine) + 1;
        prevIncludeLine = includeLineNumber - 1;
        // remove include line from the code string
        codeString.erase(includePos, tokenLen + line.size());

        size_t lineEnd = line.size();
        size_t linePos = 0;
        // skip whitespace
        linePos = SkipWhiteSpace(line, linePos);

        const bool angledIncludeStart = line[linePos] == '<';
        if (linePos == lineEnd || (line[linePos] != '\"' && !angledIncludeStart))
        {
            outErrors.AddImportError("Malformed #include (no directive start found): " + includeLine, includeLineNumber, true, api);
            ok = false;
            continue;
        }

        // read include name up to next quote
        core::string fileName;
        ++linePos;
        size_t startPos = linePos;
        while (linePos < lineEnd && line[linePos] != '\"' && line[linePos] != '>')
            ++linePos;
        if (linePos == lineEnd)
        {
            outErrors.AddImportError("Malformed #include (no directive end found): " + includeLine, includeLineNumber, true, api);
            ok = false;
            continue;
        }
        fileName = line.substr(startPos, linePos - startPos);
        if (fileName.empty())
        {
            outErrors.AddImportError("Malformed #include (empty file name): " + includeLine, includeLineNumber, true, api);
            ok = false;
            continue;
        }
        const bool angledIncludeEnd = line[linePos] == '>';
        if (angledIncludeStart != angledIncludeEnd)
        {
            outErrors.AddImportError("Malformed #include (mixed quoted and angle-bracket form): " + includeLine, includeLineNumber, true, api);
            ok = false;
            continue;
        }

        // we assume include <> means "system" header - no need to look into it
        if (angledIncludeStart)
        {
            codeString.insert(includePos, includeLine);
            includePos += includeLine.size();
            continue;
        }

        // try to find the included file
        bool hasFile = false;
        for (size_t i = 0; i <= includeContext.includePaths.size(); ++i)
        {
            core::string fullFileName;

            // First try the include path list
            if (i < includeContext.includePaths.size())
            {
                fullFileName = AppendPathName(includeContext.includePaths[i], fileName);
                ConvertSeparatorsToUnity(fullFileName);
            }
            else // Then try the redirected folders
            {
                ConvertSeparatorsToUnity(fileName);
                fullFileName = ResolveRedirectedPath(fileName, includeContext.redirectedFolders);
            }

            core::string contents;
            if (readFileFunc(fullFileName, contents))
            {
                // found included file
                core::string lineDir;

                lineDir = Format("#line %d\n", 1);
                contents.insert(0, lineDir);

                IncludeSearchContext childContext = includeContext;
                core::string filePath = DeleteLastPathNameComponent(fullFileName);
                if (!filePath.empty())
                    childContext.includePaths.push_back(filePath);
                // recursively perform includes in the child file
                ok &= ProcessIncludesRecursive(fileName, contents, includeDepth, childContext, readFileFunc, outErrors, api);

                // paste processed contents
                codeString.insert(includePos, contents);
                includePos += contents.size();
                // insert line directive to get us back to original line number
                lineDir = Format("\n#line %d\n", includeLineNumber);
                codeString.insert(includePos, lineDir);
                includePos += lineDir.size();

                hasFile = true;
                break;
            }
        }

        if (!hasFile)
        {
            outErrors.AddImportError("Can't find include file " + fileName, includeLineNumber, false, api);
        }
    }

    return ok;
}

static bool HashIncludesRecursive(
    const core::string& parentFileName,
    const core::string& codeString,
    int includeDepth,
    const IncludeSearchContext& includeContext,
    std::set<core::string>& includedFiles,
    ReadTextFileFunc readFileFunc,
    SpookyHash& hash)
{
    ++includeDepth;
    if (includeDepth > kMaxIncludeDepth)
        return false; // stop bothering with hashing includes after this depth

    bool ok = true;

    size_t includePos = 0;
    while (true)
    {
        // find next occurrence of include directive
        size_t tokenLen;
        includePos = FindIncludeLine(codeString, includePos, tokenLen);
        if (includePos == string::npos)
            break;

        // extract line after include token
        core::string line = ExtractRestOfLine(codeString, includePos + tokenLen);
        includePos += line.length() + tokenLen;

        size_t lineEnd = line.size();
        size_t linePos = 0;
        linePos = SkipWhiteSpace(line, linePos);

        const bool angledIncludeStart = line[linePos] == '<';
        if (linePos == lineEnd || (line[linePos] != '\"' && !angledIncludeStart))
        {
            ok = false; // malformed include; skip
            continue;
        }

        // read include name up to next quote
        core::string fileName;
        fileName.reserve(lineEnd - linePos);
        ++linePos;
        while (linePos < lineEnd && line[linePos] != '\"' && line[linePos] != '>')
        {
            fileName += line[linePos];
            ++linePos;
        }
        if (linePos == lineEnd)
        {
            ok = false; // malformed include; skip
            ok = false;
            continue;
        }
        if (fileName.empty())
        {
            ok = false; // empty filename; skip
            continue;
        }
        const bool angledIncludeEnd = line[linePos] == '>';
        if (angledIncludeStart != angledIncludeEnd)
        {
            ok = false; // mixed quotes and angle-brackets; skip
            continue;
        }

        // we assume include <> means "system" header - no need to look into it
        if (angledIncludeStart)
            continue;

        // try to find the included file
        for (size_t i = 0; i <= includeContext.includePaths.size(); ++i)
        {
            core::string fullFileName;
            core::string flattenedFilePath;

            // First try the include path list
            if (i < includeContext.includePaths.size())
            {
                fullFileName = AppendPathName(includeContext.includePaths[i], fileName);
                ConvertSeparatorsToUnity(fullFileName);
                flattenedFilePath = FlattenRelativePath(fullFileName, false);
            }
            else // Then try the redirected folders
            {
                ConvertSeparatorsToUnity(fileName);
                fullFileName = ResolveRedirectedPath(fileName, includeContext.redirectedFolders);
                flattenedFilePath = FlattenRelativePath(fullFileName, false);
            }

            // Check if we already saw this same file; skip it from hashing
            if (includedFiles.find(flattenedFilePath) != includedFiles.end())
            {
                break;
            }

            core::string contents;
            if (readFileFunc(fullFileName, contents))
            {
                // found included file
                hash.Update(contents.c_str(), contents.size());

                // recursively hash includes in the child file
                IncludeSearchContext childContext = includeContext;
                core::string filePath = DeleteLastPathNameComponent(fullFileName);
                if (!filePath.empty())
                    childContext.includePaths.push_back(filePath);
                ok &= HashIncludesRecursive(fileName, contents, includeDepth, childContext, includedFiles, readFileFunc, hash);

                includedFiles.insert(flattenedFilePath);
                break;
            }
        }
    }

    return ok;
}

static void ProcessIncludesImpl(core::string& text, const IncludeSearchContext& includeContext, ReadTextFileFunc readFileFunc, ShaderImportErrors& outErrors, ShaderCompilerPlatform api)
{
    ProcessIncludesRecursive("", text, 0, includeContext, readFileFunc, outErrors, api);
}

static void HashIncludesImpl(const core::string& text, const IncludeSearchContext& includeContext, ReadTextFileFunc readFileFunc, UInt32 outHash[4], std::vector<core::string> &outIncludedFiles)
{
    std::set<core::string> includedFiles;
    SpookyHash hasher;
    hasher.Init(0, 0);
    HashIncludesRecursive("", text, 0, includeContext, includedFiles, readFileFunc, hasher);

    UInt64 hash1, hash2;
    hasher.Final(&hash1, &hash2);
    outHash[0] = hash1 & 0xFFFFFFFF;
    outHash[1] = hash1 >> 32;
    outHash[2] = hash2 & 0xFFFFFFFF;
    outHash[3] = hash2 >> 32;

    outIncludedFiles.resize(includedFiles.size());
    std::copy(includedFiles.begin(), includedFiles.end(), outIncludedFiles.begin());
}

void ProcessIncludeDirectives(core::string& text, const IncludeSearchContext& includeContext, ShaderImportErrors& outErrors, ShaderCompilerPlatform api)
{
    ProcessIncludesImpl(text, includeContext, &ReadTextFile, outErrors, api);
}

void ComputeIncludesHash(const core::string& text, const IncludeSearchContext& includeContext, UInt32 outHash[4], std::vector<core::string> &outIncludedFiles)
{
    HashIncludesImpl(text, includeContext, &ReadTextFile, outHash, outIncludedFiles);
}

// -------------------------------------------------------------------


#if ENABLE_UNIT_TESTS
#include "Runtime/Testing/Testing.h"

UNIT_TEST_SUITE(ProcessIncludes)
{
    struct ProcessIncludesTestFixture
    {
        ProcessIncludesTestFixture()
        {
            context.includePaths.push_back(".");
        }

        static bool ReadTextFileMock(const core::string& pathName, core::string& output)
        {
            core::string fileName = GetLastPathNameComponent(pathName);
            if (fileName == "includetest1.txt")
            {
                output = "aaa";
                return true;
            }
            if (fileName == "includetest2.txt")
            {
                output = "ccc\n#include \"includetest1.txt\"\nddd";
                return true;
            }
            if (fileName == "includetest3.txt")
            {
                output =
                    "// line 1\n"
                    "// line 2\n"
                    "// line 3\n"
                    "// line 4\n";
                return true;
            }
            if (fileName == "includetokentest1.txt")
            {
                output = "#   include \"includetokentest2.txt\"\n";
                return true;
            }
            if (fileName == "includetokentest2.txt")
            {
                output = "#include \"includetest1.txt\"\n# include \"includetest1.txt\"\n#\tinclude \"includetest1.txt\"\n";
                return true;
            }

            if (pathName == "./relativetest1.txt")
            {
                output = "// relativetest1.txt\n#include \"sub/dir/relativetest2.txt\"";
                return true;
            }
            if (pathName == "./sub/dir/relativetest2.txt")
            {
                output = "// relativetest2.txt\n#include \"relativetest3.txt\"";
                return true;
            }
            if (pathName == "./sub/dir/relativetest3.txt")
            {
                output = "// relativetest3.txt\n#include \"relativetest4.txt\"";
                return true;
            }
            if (pathName == "./relativetest4.txt")
            {
                output = "// relativetest4.txt";
                return true;
            }
            return false;
        }

        void CheckInclude(const char* src, const char* ex)
        {
            res = src;
            errors.Clear();
            ProcessIncludesImpl(res, context, &ReadTextFileMock, errors, kShaderCompPlatformCount);
            CHECK(!errors.HasErrorsOrWarnings());
            CHECK_EQUAL(ex, res);
        }

        IncludeSearchContext context;
        ShaderImportErrors errors;
        core::string res;
    };


    TEST_FIXTURE(ProcessIncludesTestFixture, ProcessIncludes_Works)
    {
        const char* ex;
        ex =
            "bbb\n"
            "#line 1\n"
            "aaa"
            "\n#line 2\n";
        CheckInclude("bbb\n#include \"includetest1.txt\"", ex);
        ex =
            "bbb\n    "
            "#line 1\n"
            "aaa"
            "\n#line 2\n";
        CheckInclude("bbb\n    #include \"includetest1.txt\"", ex);
        ex =
            "bbb\n"
            "#line 1\n"
            "aaa"
            "\n#line 2\n"
            "\n";
        CheckInclude("bbb\n#include \"includetest1.txt\"\n", ex);
        ex =
            "bbb\n"
            "#line 1\n"
            "ccc\n"
            "#line 1\n"
            "aaa"
            "\n#line 2\n"
            "\n"
            "ddd"
            "\n#line 2\n";
        CheckInclude("bbb\n#include \"includetest2.txt\"", ex);
    }

    TEST_FIXTURE(ProcessIncludesTestFixture, ProcessIncludes_Errors_ReportedOnCorrectLines)
    {
        core::string src =
            "\n" // 1
            "#include \"includetest3.txt\"\n" // 2
            "\n" // 3
            "#include \"missing.txt\"\n" // error on line 4
            "\n" // 5
            "foobar\n" // 6
            "#include \"includetest3.txt\"\n" // 7
            "\n" // 8
            "#include \"missing2.txt\"\n"; // error on line 9
        errors.Clear();
        ProcessIncludesImpl(src, context, &ReadTextFileMock, errors, kShaderCompPlatformCount);
        CHECK(errors.HasErrorsOrWarnings());
        CHECK_EQUAL(2, errors.GetErrors().size());
        ShaderImportErrors::ErrorContainer::const_iterator it = errors.GetErrors().begin();
        CHECK_EQUAL("", it->file);
        CHECK_EQUAL(4, it->line);
        ++it;
        CHECK_EQUAL("", it->file);
        CHECK_EQUAL(9, it->line);
    }

    TEST_FIXTURE(ProcessIncludesTestFixture, ProcessIncludes_WithSpacesInDirective)
    {
        const char* ex;
        ex =
            "#line 1\n"
            "aaa\n#line 1\n";
        CheckInclude("# include \"includetest1.txt\"", ex);
        ex =
            "#line 1\n"
            "aaa\n#line 1\n";
        CheckInclude("#  include \"includetest1.txt\"", ex);
        ex =
            "#line 1\n"
            "aaa\n#line 1\n";
        CheckInclude("#\tinclude \"includetest1.txt\"", ex);
    }

    TEST_FIXTURE(ProcessIncludesTestFixture, ProcessIncludes_RelativeSearchPaths)
    {
        const char* ex;
        ex =
            "aaa\n"
            "#line 1\n"
            "// relativetest1.txt\n"
            "#line 1\n"
            "// relativetest2.txt\n"
            "#line 1\n"
            "// relativetest3.txt\n"
            "#line 1\n"
            "// relativetest4.txt\n"
            "#line 2\n\n"
            "#line 2\n\n"
            "#line 2\n\n"
            "#line 2\n";
        CheckInclude("aaa\n#include \"relativetest1.txt\"", ex);
    }

    TEST_FIXTURE(ProcessIncludesTestFixture, ComputeIncludesHash_ComputesHashForAllReferencedIncludeFiles)
    {
        // The includes hashing needs to compute hashes of all references include files, recursively,
        // but not the incoming source string itself. Check that this is done properly by comparing against
        // "expected" hash -- mostly so that further refactoring of the function still result in the same
        // hash.
        const char* src =
            "aaa\n"
            "#include \"relativetest1.txt\"\n"
            "#include \"includetest2.txt\"\n"
            "# include \"includetest2.txt\"\n"
            "#  include \"includetest2.txt\"\n"
            "bbb\n";
        UInt32 resultHash[4];
        core::string str = src;
        std::vector<core::string> includedFiles;
        HashIncludesImpl(str, context, &ReadTextFileMock, resultHash, includedFiles);

        // Expected hash; result of doing SpookyHash on these strings in sequence:
        // // relativetest1.txt\n#include "sub/dir/relativetest2.txt"
        // // relativetest2.txt\n#include "relativetest3.txt"
        // // relativetest3.txt\n#include "relativetest4.txt"
        // // relativetest4.txt
        // ccc\n#include "includetest1.txt"\nddd
        // aaa
        CHECK_EQUAL(2950011278, resultHash[0]);
        CHECK_EQUAL(868581701, resultHash[1]);
        CHECK_EQUAL(773486765, resultHash[2]);
        CHECK_EQUAL(842441736, resultHash[3]);
    }

    TEST_FIXTURE(ProcessIncludesTestFixture, ProcessIncludes_RecursiveWithVaryingTokens_Works)
    {
        const char* ex;
        ex =
            "bbb\n"
            "#line 1\n"
            "#line 1\n"
            "#line 1\n"
            "aaa\n"
            "#line 1\n\n"
            "#line 1\n"
            "aaa\n"
            "#line 2\n\n"
            "#line 1\n"
            "aaa\n"
            "#line 3\n\n\n"
            "#line 1\n\n\n"
            "#line 2\n";
        CheckInclude("bbb\n#include \"includetokentest1.txt\"", ex);
    }

    struct RedirectedPathsTestFixture
    {
        const char* kAliasOne = "Packages/com.packagename.one";
        const char* kAliasTwo = "Packages/com.packagename.two";

        const char* kFullPathOne = "path/to/package1";
        const char* kFullPathTwo = "path/to/package2";

        RedirectedPathsTestFixture()
        {
            redirectedFolders[kAliasOne] = kFullPathOne;
            redirectedFolders[kAliasTwo] = kFullPathTwo;
        }

        IncludeFolderMap redirectedFolders;
    };

    TEST_FIXTURE(RedirectedPathsTestFixture, ProcessIncludes_ResolveRedirectedPath_Works)
    {
        for (IncludeFolderMap::iterator itr = redirectedFolders.begin(); itr != redirectedFolders.end(); ++itr)
        {
            core::string testPath(itr->first);
            testPath.append("/MyInclude.cginc"); // From e.g. "Packages/com.packagename.one/MyInclude.cginc"

            core::string expectedPath(itr->second);
            expectedPath.append("/MyInclude.cginc"); // To e.g. "path/to/package1/MyInclude.cginc"

            core::string res = ResolveRedirectedPath(testPath, redirectedFolders);
            CHECK_EQUAL(expectedPath, res);
        }
    }

    TEST_FIXTURE(RedirectedPathsTestFixture, ProcessIncludes_ResolveRedirectedPath_ReturnsEmptyStringWhenNotFound)
    {
        core::string testPath("Packages/com.not.found/MyInclude.cginc");
        core::string res = ResolveRedirectedPath(testPath, redirectedFolders);
        CHECK_EQUAL("", res);
    }
}
#endif // ENABLE_UNIT_TESTS
