#pragma once

#include <string>
#include "Runtime/Utilities/RapidJSONConfig.h"
#include "JsonCombiner.h"

#include <RapidJSON/allocators.h>
using namespace Unity::rapidjson;

class TocCombiner : public JsonCombiner
{
public:
    TocCombiner();
    virtual bool VerifyJSONStructure(const Value &json);

    void AppendJsonDocument(Document& dSource);
    void AddModuleIndexToEachEntry(Document& doc, int index);

    virtual bool OutputJsonValueAsJs(const char *currentPlatformPath, const Document& doc, const char* fname);

private:
    static bool SameLinkAndTitle(Value& target, Value& source);
    void AppendJsonValue(Value& target, Value& source);
    void AddPlatformPrefixToLinks(Value& val, int index);
    bool OutputValueAsJs(FILE* f, const Value& val, int depth);
    bool WriteIndentedString(FILE* f, const char* s, int n, bool useNewline);
    bool WriteQuotedStringPlusExtra(FILE* f, const char* quotable, const char* extra, int n, bool useNewline);
    bool WriteNSpaces(FILE* f, int n);
    bool WritePrefixAndString(FILE* f, int num, const char* st);
    bool useNewline;
    bool useIndents;
};
