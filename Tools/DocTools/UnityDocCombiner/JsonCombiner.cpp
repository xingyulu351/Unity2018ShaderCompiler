#include "UnityPrefix.h"
#include "Runtime/Utilities/RapidJSONConfig.h"
#include "JsonCombiner.h"
#include "RapidJsonIoUtil.h"
#include <RapidJSON/allocators.h>

JsonCombiner::~JsonCombiner()
{
    for (std::vector<Document *>::iterator it = m_Documents.begin(); it != m_Documents.end(); it++)
    {
        delete *it;
    }
}

void JsonCombiner::SetModulePathAtIndex(int index, const std::string& prefix)
{
    m_Modules[index] = prefix;
}

bool JsonCombiner::CombineJson(const std::list<std::string>& platformList, const std::string& jsonRelativePath)
{
    int index = 0;
    for (std::list<std::string>::const_iterator itr = platformList.begin(); itr != platformList.end(); ++itr)
    {
        std::string curPlatform = *itr;
        std::string curPrefix = curPlatform;
        std::string platformJsonPath = curPrefix + '/' + jsonRelativePath;

        m_Documents.push_back(new Document());
        Document &dCurrent = *(m_Documents.back());
        if (!RapidJsonIoUtil::TryGetJsonDocFromFile(platformJsonPath.c_str(), dCurrent))
        {
            continue;
        }

        if (!VerifyJSONStructure(dCurrent))
        {
            continue;
        }

        SetModulePathAtIndex(index, curPrefix + '/');

        AddModuleIndexToEachEntry(dCurrent, index);
        AppendJsonDocument(dCurrent);

        index++;
    }
    return index > 0;
}

// convert absolute path to relative, with respect to the given root
static std::string GetRelativePathForRoot(const char *absolutePath, const char *root)
{
    const char *absPathStart = absolutePath;

    // skip common base path
    while (*absolutePath && *root && *absolutePath == *root)
    {
        absolutePath++;
        root++;
    }

    // make sure to stop on path separator
    while (*absolutePath != '/' && absolutePath > absPathStart)
    {
        absolutePath--;
        root--;
    }

    // skip trailing slashes
    while (*absolutePath == '/')
    {
        absolutePath++;
    }

    // build the final path by prepending ../ for each nestedness level
    std::string out = absolutePath;
    const char *path = root;
    while (const char *slash = strchr(path, '/'))
    {
        out = "../" + out;
        path = slash + 1;
    }

    return out;
}

bool JsonCombiner::OutputPrefixVars(const char *currentPlatformPath, FILE* f)
{
    for (std::map<int, std::string>::const_iterator i = m_Modules.begin(); i != m_Modules.end(); i++)
    {
        // find common root, and convert to relative
        std::string relativePath = GetRelativePathForRoot(i->second.c_str(), currentPlatformPath);
        if (0 > fprintf(f, "var p%d = \"%s\";\n", i->first, relativePath.c_str()))
        {
            return false;
        }
    }
    return true;
}

void JsonCombiner::EscapeDoubleQuotedString(const char *str, char *buffer, int buffer_size)
{
    const char *in = str;
    char *out = buffer;
    while (*in && buffer_size-- > 1)
    {
        if (*in == '"')
        {
            *out++ = '\\';
        }
        *out++ = *in++;
    }
    *out = 0;
}
