#pragma once

#include <list>
#include <string>

#include "Runtime/Utilities/RapidJSONConfig.h"
#include "External/RapidJSON/document.h"
#include "search_integration/SearchResult.h"

using namespace Unity::rapidjson;

namespace ureport
{
    class ResultJSONParser
    {
    public:
        ResultJSONParser(std::string json);

        bool GetValue(const std::string& key, int& value);
        bool GetValue(const std::string& key, std::string& value);
        size_t GetNumberOfArrayItems(const std::string& key);

    private:
        const Value* GetElement(Value& document, std::list<std::string>& keys);
        std::list<std::string> GetKeysList(const std::string& key);
        Value* GetMember(Value& document, const std::string& key) const;
        bool IsArrayIndex(const std::string& key) const;
        int GetArrayIndexFromKey(const std::string& key) const;

    private:
        Document m_Document;
    };
}
