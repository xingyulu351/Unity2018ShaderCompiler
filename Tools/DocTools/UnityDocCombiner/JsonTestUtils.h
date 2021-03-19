#pragma once

#include <string>

class JsonCombiner;

void VerifyCombineJsons(JsonCombiner& jsonCombiner,
    const std::string& jsonA, int moduleIndexA,
    const std::string& jsonB, int moduleIndexB,
    const std::string& expectedSt);
