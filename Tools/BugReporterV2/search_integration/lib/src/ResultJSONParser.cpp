#include <sstream>

#include "search_integration/ResultJSONParser.h"

using namespace Unity::rapidjson;

namespace ureport
{
    ResultJSONParser::ResultJSONParser(std::string json)
    {
        m_Document.Parse(json.c_str());
    }

    bool ResultJSONParser::GetValue(const std::string& key, int& value)
    {
        if (m_Document.HasParseError())
            return false;

        auto keys = GetKeysList(key);
        auto element = GetElement(m_Document, keys);
        if (element != nullptr && element->IsInt())
        {
            value = element->GetInt();
            return true;
        }

        return false;
    }

    bool ResultJSONParser::GetValue(const std::string& key, std::string& value)
    {
        if (m_Document.HasParseError())
            return false;

        auto keys = GetKeysList(key);
        auto element = GetElement(m_Document, keys);
        if (element != nullptr && element->IsString())
        {
            value = element->GetString();
            return true;
        }

        return false;
    }

    size_t ResultJSONParser::GetNumberOfArrayItems(const std::string& key)
    {
        if (m_Document.HasParseError())
            return false;

        auto keys = GetKeysList(key);
        auto element = GetElement(m_Document, keys);
        if (element != nullptr && element->IsArray())
        {
            return element->Size();
        }

        return 0;
    }

    const Value* ResultJSONParser::GetElement(
        Value& document,
        std::list<std::string>& keys)
    {
        if (!(document.IsObject() || document.IsArray()) || keys.size() == 0)
            return nullptr;

        auto key = keys.front().c_str();
        Value* value = GetMember(document, key);
        if (value != nullptr)
        {
            if (keys.size() > 1)
            {
                keys.pop_front();
                return GetElement(*value, keys);
            }
            return value;
        }

        return nullptr;
    }

    Value* ResultJSONParser::GetMember(Value& document, const std::string& key) const
    {
        if (IsArrayIndex(key) && document.IsArray())
        {
            int index = GetArrayIndexFromKey(key);
            if (index < 0 || document.Size() < index + 1)
                return nullptr;

            return &document[index];
        }

        auto itr = document.FindMember(key.c_str());
        if (itr == document.MemberEnd())
            return nullptr;
        return &itr->value;
    }

    bool ResultJSONParser::IsArrayIndex(const std::string& key) const
    {
        return (key.length() > 0 && key[0] == '[');
    }

    int ResultJSONParser::GetArrayIndexFromKey(const std::string& key) const
    {
        auto start = key.find("[");
        auto end = key.find("]");

        if (start == std::string::npos || end == std::string::npos)
            return -1;

        auto strValue = key.substr(start + 1, end - start - 1);
        int value = std::atoi(strValue.c_str());

        return value;
    }

    std::list<std::string> ResultJSONParser::GetKeysList(const std::string& key)
    {
        char delimiter = '/';
        std::list<std::string> keys;

        std::istringstream ss(key);
        std::string token;

        while (std::getline(ss, token, delimiter))
        {
            keys.push_back(token);
        }

        return keys;
    }
}
