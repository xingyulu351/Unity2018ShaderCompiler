#pragma once

#if ENABLE_UNIT_TESTS
#include "UnityPrefix.h"
#include "Runtime/Testing/Testing.h"
#endif
#include <string>
#include <vector>
#include <list>
#include <map>

#include "Runtime/Utilities/RapidJSONConfig.h"
#include <RapidJSON/document.h>
#include "TCError.h"

#ifdef WIN32
#define alloca _alloca
#define unlink _unlink
#define RenameWithOverwrite(source, dest) MoveFileEx(source,dest,MOVEFILE_REPLACE_EXISTING)
#else
#include <unistd.h>
#define RenameWithOverwrite(source, dest) rename(source,dest)
#endif

#if !defined(BUILD_WITHOUT_UNITY) && defined(WIN32)
#define DbgPause() system("pause")
#else
#define DbgPause()
#endif

using namespace Unity::rapidjson;

class JsonCombiner
{
public:
    JsonCombiner()
        : m_EmptyDocString("{}")
    {
    }

    virtual ~JsonCombiner();

    virtual bool VerifyJSONStructure(const Value &json) = 0;

    virtual void AppendJsonDocument(Document& dSource) = 0;
    virtual void AddModuleIndexToEachEntry(Document& doc, int index) = 0;

    // Writes the final javascript into specified file
    // returns true on success
    virtual bool OutputJsonValueAsJs(const char *currentPlatformPath, const Document& doc, const char* fname) = 0;

    bool CombineJson(const std::list<std::string>& platformList, const std::string& jsonRelativePath);
    Document& GetCombinedDoc() {return m_CombinedToc; }
    void SetModulePathAtIndex(int index, const std::string& prefix);

    static void EscapeDoubleQuotedString(const char *str, char *buffer, int buffer_size);

protected:

    std::map<int, std::string> m_Modules;
    std::string m_EmptyDocString;
    std::vector<Document *> m_Documents;
    Document m_CombinedToc;
    bool OutputPrefixVars(const char *currentPlatformPath, FILE* f);
};
