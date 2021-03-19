#pragma once
#include "UnityPrefix.h"
#include "Runtime/Utilities/RapidJSONConfig.h"
#include <RapidJSON/document.h>

#include <string>
#include <list>

using namespace Unity::rapidjson;

class JsonCombiner;

class RapidJsonIoUtil
{
public:
    static bool TryGetJsonDocFromString(const char* string, Document& d);
    static bool OutputJsonValueToFile(const Value& jsonVal, const char* fname);
    static std::string OutputJsonValueToString(const Value& jsonVal);

    static bool TryGetJsonDocFromFile(const char* fname, Document& d);
};
