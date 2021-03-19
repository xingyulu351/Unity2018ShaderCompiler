#pragma once

#include <string>
#include "JsonCombiner.h"

using namespace Unity::rapidjson;

class IndexCombiner : public JsonCombiner
{
public:
    IndexCombiner();
    virtual bool VerifyJSONStructure(const Value &json);

    virtual void AppendJsonDocument(Document& dSource);
    virtual void AddModuleIndexToEachEntry(Document& doc, int index);

    virtual bool OutputJsonValueAsJs(const char *currentPlatformPath, const Document& doc, const char* fname);

private:
    void CombinePagesJson(Value& targetPages, Value& sourcePages);
    void CombineInfoJson(Value& targetInfo, Value& sourceInfo, int numTargetPages);
    void CombineSearchIndexJson(Value& targetInfo, Value& sourceInfo, int numTargetInfos);
};
