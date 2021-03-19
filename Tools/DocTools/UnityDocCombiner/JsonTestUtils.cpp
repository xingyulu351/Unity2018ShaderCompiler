#include "UnityPrefix.h"
#include "Runtime/Utilities/RapidJSONConfig.h"
#include <RapidJSON/document.h>

#include "Runtime/Testing/Testing.h"
#include "RapidJsonIoUtil.h"
#include "JsonCombiner.h"

#if ENABLE_UNIT_TESTS

using namespace Unity::rapidjson;

void VerifyCombineJsons(JsonCombiner& jsonCombiner,
    const std::string& jsonA, int moduleIndexA,
    const std::string& jsonB, int moduleIndexB,
    const std::string& expectedSt)
{
    Document docA;
    Document docB;
    // add module info to all documents
    RapidJsonIoUtil::TryGetJsonDocFromString(jsonA.c_str(), docA);
    RapidJsonIoUtil::TryGetJsonDocFromString(jsonB.c_str(), docB);
    jsonCombiner.AddModuleIndexToEachEntry(docA, moduleIndexA);
    jsonCombiner.AddModuleIndexToEachEntry(docB, moduleIndexB);

    jsonCombiner.AppendJsonDocument(docA);
    jsonCombiner.AppendJsonDocument(docB);

    Document& actual = jsonCombiner.GetCombinedDoc();
    std::string actualSt = RapidJsonIoUtil::OutputJsonValueToString(actual);
    CHECK_EQUAL(expectedSt, actualSt);
}

#endif
