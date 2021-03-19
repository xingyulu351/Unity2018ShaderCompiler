#include "UnityPrefix.h"
#include "RapidJsonIoUtil.h"
#include "TocCombiner.h"
#include "IndexCombiner.h"
#include <string>
#include <list>
#include <sys/stat.h>
#ifndef WIN32
#include <dirent.h>
#else
#include <windows.h>
#endif

using namespace Unity::rapidjson;

//here we're only validating the Json, not the existence of files. Local setup might not have Manual docs
bool VerifyTocAndIndex(const std::string& rootDocPath)
{
    Document dCurrent;
    std::list<std::string> paths;
    paths.push_back(rootDocPath + "/en/Manual/docdata/toc.json");
    paths.push_back(rootDocPath + "/en/Manual/docdata/index.json");
    paths.push_back(rootDocPath + "/en/ScriptReference/docdata/toc.json");
    paths.push_back(rootDocPath + "/en/ScriptReference/docdata/index.json");

    for (std::list<std::string>::const_iterator itr = paths.begin(); itr != paths.end(); ++itr)
    {
        std::string curPath = *itr;
        struct stat buf;
        if (stat(curPath.c_str(), &buf))
        {
            TCMessage("Skipping %s\n", curPath.c_str());
            continue;
        }
        Document dCurrent;

        TCMessage("Verifying %s\n", curPath.c_str());
        if (!RapidJsonIoUtil::TryGetJsonDocFromFile(curPath.c_str(), dCurrent))
        {
            return false;
        }
    }
    return true;
}

int CombineAllTocAndIndex(const std::list<std::string>& platformPathsOrig, const std::string& postfix)
{
    std::list<std::string> platformPaths;
    for (std::list<std::string>::const_iterator itr = platformPathsOrig.begin(); itr != platformPathsOrig.end(); ++itr)
    {
        std::string curPath = *itr + "/" + postfix;
        platformPaths.push_back(curPath);
    }
    std::string tocJsonRelativePath = "docdata/toc.json";
    std::string indexJsonRelativePath = "docdata/index.json";

    IndexCombiner indexCombiner;
    if (!indexCombiner.CombineJson(platformPaths, indexJsonRelativePath))
    {
        return -1;
    }

    TocCombiner tocCombiner;
    if (!tocCombiner.CombineJson(platformPaths, tocJsonRelativePath))
    {
        return -1;
    }

    for (std::list<std::string>::const_iterator itr = platformPaths.begin(); itr != platformPaths.end(); ++itr)
    {
        std::string basepath = *itr;

        std::string indexjsonpath = basepath + "/docdata/global_index.json";
        std::string indexjspath = basepath + "/docdata/global_index.js";
        std::string tocjsonpath = basepath + "/docdata/global_toc.json";
        std::string tocjspath = basepath + "/docdata/global_toc.js";

        if (RapidJsonIoUtil::OutputJsonValueToFile(indexCombiner.GetCombinedDoc(), (indexjsonpath + ".tmp").c_str()))
        {
            RenameWithOverwrite((indexjsonpath + ".tmp").c_str(), indexjsonpath.c_str());
        }

        if (indexCombiner.OutputJsonValueAsJs(basepath.c_str(), indexCombiner.GetCombinedDoc(), (indexjspath + ".tmp").c_str()))
        {
            RenameWithOverwrite((indexjspath + ".tmp").c_str(), indexjspath.c_str());
        }

        if (RapidJsonIoUtil::OutputJsonValueToFile(tocCombiner.GetCombinedDoc(), (tocjsonpath + ".tmp").c_str()))
        {
            RenameWithOverwrite((tocjsonpath + ".tmp").c_str(), tocjsonpath.c_str());
        }

        if (tocCombiner.OutputJsonValueAsJs(basepath.c_str(), tocCombiner.GetCombinedDoc(), (tocjspath + ".tmp").c_str()))
        {
            RenameWithOverwrite((tocjspath + ".tmp").c_str(), tocjspath.c_str());
        }
    }
    return 0;
}

void GetPathsOfDocs(const std::string& topDir, std::list<std::string>& finalPaths);

static inline void AddFoundPath(const std::string& topDir, const char *folderName, std::list<std::string>& finalPaths)
{
    std::string absFolderName = topDir + "/" + folderName;
    std::string manualDocDataPath = absFolderName + "/Manual/docdata";
    struct stat buf;
    if (stat(manualDocDataPath.c_str(), &buf) == 0 && (buf.st_mode & S_IFDIR))
    {
        finalPaths.push_back(absFolderName);
    }
    else
    {
        GetPathsOfDocs(absFolderName, finalPaths);
    }
}

#ifdef WIN32
void GetPathsOfDocs(const std::string& topDir, std::list<std::string>& finalPaths)
{
    WIN32_FIND_DATA FindFileData;
    memset(&FindFileData, 0, sizeof(FindFileData));

    HANDLE hFind = FindFirstFile((topDir + "/*").c_str(), &FindFileData);
    if (hFind != INVALID_HANDLE_VALUE)
    {
        do
        {
            if (FindFileData.cFileName[0] != '.' && (FindFileData.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY))
            {
                AddFoundPath(topDir, FindFileData.cFileName, finalPaths);
            }
        }
        while (FindNextFile(hFind, &FindFileData));
        FindClose(hFind);
    }
}

#else
void GetPathsOfDocs(const std::string& topDir, std::list<std::string>& finalPaths)
{
    struct dirent **namelist = NULL;
    int numfiles = scandir(topDir.c_str(), &namelist, NULL, alphasort);
    for (int f = 0; f < numfiles; f++)
    {
        if (namelist[f]->d_name[0] != '.' && namelist[f]->d_type == DT_DIR)
        {
            AddFoundPath(topDir, namelist[f]->d_name, finalPaths);
        }

        free(namelist[f]);
    }
    if (namelist)
    {
        free(namelist);
    }
}

#endif

#ifdef WIN32
int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, PSTR lpCmdLine, INT nCmdShow)
{
    int argc = __argc;
    char **argv = __argv;
#else
int main(int argc, const char** argv)
{
#endif
    if (argc <= 1)
    {
        TCMessage("executing DocCombiner\n");
        TCError("Usage: %s <path1> <path2> ...\n or %s -autopaths <rootdir>\n", argv[0], argv[0]);
        return -1;
    }

#if ENABLE_UNIT_TESTS
    if (argc >= 2 && !strcmp(argv[1], "-runNativeTests"))
    {
        core::string testFilter;
        if (argc > 2)
            testFilter = argv[2];
        RunNativeTests(testFilter);
        return 0;
    }
#endif
    TCMessage("executing DocCombiner\n");
    if (argc >= 2 && !strcmp(argv[1], "-verify"))
    {
        if (argc <= 2)
        {
            TCError("Usage: %s -verify <rootDocPath>\n", argv[0]);
            return -1;
        }

        std::string rootDocPath = argv[2];
        TCMessage("rootDocPath = %s\n", rootDocPath.c_str());
        if (!VerifyTocAndIndex(rootDocPath))
        {
            TCError("VerifyTocAndIndex failed\n");
            return -1;
        }
        TCMessage("VerifyTocAndIndex succeeded\n");
        return 0;
    }
    std::list<std::string> platformPaths;

    if (argc >= 2 && !strcmp(argv[1], "-autopaths"))
    {
        if (argc <= 2)
        {
            TCError("Usage: %s <path1> <path2> ...\n or %s -autopaths <rootdir>\n", argv[0], argv[0]);
            return 0;
        }
        std::string rootPath = argv[2];
        replace(rootPath.begin(), rootPath.end(), '\\', '/');
        GetPathsOfDocs(rootPath, platformPaths);
    }
    else
    {
        for (int i = 1; i < argc; i++)
        {
            platformPaths.push_back(argv[i]);
        }
    }

    CombineAllTocAndIndex(platformPaths, "Manual");
    CombineAllTocAndIndex(platformPaths, "ScriptReference");

    return 0;
}
