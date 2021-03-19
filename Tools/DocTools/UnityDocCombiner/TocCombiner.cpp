#include "UnityPrefix.h"
#include "Runtime/Utilities/RapidJSONConfig.h"
#include <RapidJSON/prettywriter.h>
#include <RapidJSON/filestream.h>

#include "TocCombiner.h"
#include "RapidJsonIoUtil.h"

using namespace Unity::rapidjson;

TocCombiner::TocCombiner()
    : JsonCombiner()
{
    useNewline = false;
    useIndents = false;
    if (!RapidJsonIoUtil::TryGetJsonDocFromString(m_EmptyDocString.c_str(), m_CombinedToc))
        TCDie("Failed to initialize the base JSON object\n");
}

// NOTE: target object must have the "link" and "title" keys
bool TocCombiner::SameLinkAndTitle(Value& target, Value& source)
{
    Value& sourceLink = source["link"];
    Value& sourceTitle = source["title"];

    assert(sourceLink.IsString() && sourceTitle.IsString());

    Value& targetLink = target["link"];
    Value& targetTitle = target["title"];
    assert(targetLink.IsString() && targetTitle.IsString());

    bool linksEqual = strcmp(targetLink.GetString(), sourceLink.GetString()) == 0;
    bool titlesEqual = strcmp(targetTitle.GetString(), sourceTitle.GetString()) == 0;
    return linksEqual && titlesEqual;
}

void TocCombiner::AppendJsonValue(Value& target, Value& source)
{
    if (source.IsArray())
    {
        if (!target.IsArray())
        {
            target.CopyFrom(source, m_CombinedToc.GetAllocator());
            return;
        }

        //children array, append, assume no duplicates for now
        for (SizeType i = 0; i < source.Size(); i++)
        {
            Value& curSource = source[SizeType(i)];

            bool sourceFoundInTarget = false;
            for (SizeType j = 0; j < target.Size(); j++)
            {
                Value& curTarget = target[SizeType(j)];
                if (SameLinkAndTitle(curTarget, curSource))
                {
                    sourceFoundInTarget = true;
                    Value& sourceChildren = curSource["children"];
                    Value& targetChildren = curTarget["children"];
                    AppendJsonValue(targetChildren, sourceChildren);
                    break;
                }
            }
            //leaf node of the recursion
            if (!sourceFoundInTarget)
                target.PushBack(curSource, m_CombinedToc.GetAllocator());
        }
    }
    else if (source.IsObject())
    {
        if ((target.FindMember("link") == target.MemberEnd()) || (target.FindMember("title") == target.MemberEnd()))
        {
            target = source;
        }
        else if (SameLinkAndTitle(target, source))
        {
            Value& sourceChildren = source["children"];
            Value& targetChildren = target["children"];
            if (!sourceChildren.IsNull() && !sourceChildren.IsString())
            {
                //process the children recursively
                AppendJsonValue(targetChildren, sourceChildren);
            }
        }
    }
}

//appends creates dSource to m_CombinedToc
void TocCombiner::AppendJsonDocument(Document& dSource)
{
    //starts the recursion
    AppendJsonValue(m_CombinedToc, dSource);
}

void TocCombiner::AddPlatformPrefixToLinks(Value& val, int index)
{
    if (val.IsArray())
    {
        //children array
        for (SizeType i = 0; i < val.Size(); i++)
        {
            Value& valAtIndex = val[SizeType(i)];
            AddPlatformPrefixToLinks(valAtIndex, index);
        }
    }
    else if (val.IsObject())
    {
        Value& valueOfLink = val["link"];
        assert(valueOfLink.IsString());

        val.AddMember("module", index, m_CombinedToc.GetAllocator());
        Value& valueOfChildren = val["children"];

        if (!valueOfChildren.IsNull())
        {
            AddPlatformPrefixToLinks(valueOfChildren, index);
        }
    }
}

void TocCombiner::AddModuleIndexToEachEntry(Document& doc, int index)
{
    assert(doc.IsObject());
    AddPlatformPrefixToLinks(doc, index);
}

bool TocCombiner::WriteNSpaces(FILE* f, int n)
{
    if (!useIndents)
        return true;
    for (int i = 0; i < n; i++)
    {
        if (0 > fprintf(f, " "))
            return false;
    }
    return true;
}

bool TocCombiner::WriteIndentedString(FILE* f, const char* s, int n, bool useNewline)
{
    if (!WriteNSpaces(f, n))
        return false;
    if (0 > fprintf(f, "%s", s))
        return false;
    if (useNewline)
    {
        if (0 > fprintf(f, "\n"))
        {
            return false;
        }
    }
    return true;
}

bool TocCombiner::WritePrefixAndString(FILE* f, int num, const char* st)
{
    if (strcmp(st, "null") != 0)
    {
        //add a prefix if the element is not null
        if (0 > fprintf(f, "p%d+", num))
        {
            return false;
        }
    }
    if (0 > fprintf(f, "\"%s\",", st))
        return false;
    if (useNewline)
    {
        if (0 > fprintf(f, "\n"))
        {
            return false;
        }
    }
    return true;
}

bool TocCombiner::WriteQuotedStringPlusExtra(FILE* f, const char* quotable, const char* extra, int n, bool useNewline)
{
    if (!WriteNSpaces(f, n))
    {
        return false;
    }

    int sz = (int)strlen(quotable) * 2;
    char* escaped = new char[sz];
    EscapeDoubleQuotedString(quotable, escaped, sz);

    int res = fprintf(f, "\"%s\"%s", escaped, extra);
    delete[] escaped;
    if (0 > res)
    {
        return false;
    }
    if (useNewline)
    {
        if (0 > fprintf(f, "\n"))
        {
            return false;
        }
    }
    return true;
}

bool TocCombiner::OutputValueAsJs(FILE* f, const Value& val, int depth)
{
    if (val.IsArray())
    {
        //children array
        for (SizeType i = 0; i < val.Size(); i++)
        {
            const Value& valAtIndex = val[SizeType(i)];
            if (!OutputValueAsJs(f, valAtIndex, depth + 2))
                return false;
            if (i < val.Size() - 1)
            {
                if (!WriteIndentedString(f, ",", depth + 2, useNewline))
                {
                    return false;
                }
            }
        }
    }
    else if (val.IsObject())
    {
        //link, title, children dictionary
        const Value& valueOfLink = val["link"];
        assert(valueOfLink.IsString());
        const char* st = valueOfLink.GetString();

        if (!WriteIndentedString(f, "{", depth, useNewline))
        {
            return false;
        }

        if (!WriteQuotedStringPlusExtra(f, "link", ": ", depth + 1, false))
        {
            return false;
        }
        bool trivialLink = true;
        Value::ConstMemberIterator member = val.FindMember("module");
        if (member != val.MemberEnd())
        {
            int moduleNum = member->value.GetInt();

            if (!WritePrefixAndString(f, moduleNum, st))
            {
                return false;
            }
            trivialLink = false;
        }
        if (trivialLink)
        {
            if (!WriteQuotedStringPlusExtra(f, st, ",", 0, useNewline))
            {
                return false;
            }
        }
        if (!WriteQuotedStringPlusExtra(f, "title", ": ", depth + 1, false))
        {
            return false;
        }
        if (!WriteQuotedStringPlusExtra(f, val["title"].GetString(), ",", 0, useNewline))
        {
            return false;
        }
        if (!WriteQuotedStringPlusExtra(f, "children", ": ", depth + 1, false))
        {
            return false;
        }
        const Value& valueOfChildren = val["children"];

        if (!valueOfChildren.IsNull())
        {
            if (!WriteIndentedString(f, "[", 0, useNewline))
            {
                return false;
            }
            if (!OutputValueAsJs(f, valueOfChildren, depth + 2))
            {
                return false;
            }
            if (!WriteIndentedString(f, "]", depth + 1, useNewline))
            {
                return false;
            }
        }
        else
        {
            if (!WriteIndentedString(f, "null", 0, useNewline))
            {
                return false;
            }
        }
        if (!WriteIndentedString(f, "}", depth, useNewline))
        {
            return false;
        }
    }
    return true;
}

bool TocCombiner::OutputJsonValueAsJs(const char *currentPlatformPath, const Document& doc, const char* fname)
{
    FILE *f = NULL;
    bool res = false;

    if (!doc.HasMember("link") || !doc.HasMember("title"))
        return false;

    f = fopen(fname, "w+b");
    if (!f)
    {
        TCError("Failed to open output file %s\n", fname);
        return false;
    }

    const Value& childrenVal = doc["children"];

    if (!OutputPrefixVars(currentPlatformPath, f))
    {
        goto error;
    }

    if (0 > fprintf(f, "toc = {\n"))
    {
        goto error;
    }

    if (!WriteQuotedStringPlusExtra(f, "link", ": ", 1, false))
        goto error;
    if (!WriteQuotedStringPlusExtra(f, doc["link"].GetString(), ",", 0, useNewline))
        goto error;
    if (!WriteQuotedStringPlusExtra(f, "title", ": ", 1, false))
        goto error;
    if (!WriteQuotedStringPlusExtra(f, doc["title"].GetString(), ",", 0, useNewline))
        goto error;
    if (!WriteQuotedStringPlusExtra(f, "children", ": [", 1, useNewline))
        goto error;

    if (!OutputValueAsJs(f, childrenVal, 2))
        goto error;

    if (0 > fprintf(f, "]};\n"))
    {
        goto error;
    }

    res = true;
error:
    fclose(f);
    if (!res)
    {
        unlink(fname);
    }
    return res;
}

bool TocCombiner::VerifyJSONStructure(const Value &json)
{
    // rules: each node has "link", "title", "children"
    // the link and title are strings
    // children is list of the same objects, can be null


    Value::ConstMemberIterator title = json.FindMember("title");
    Value::ConstMemberIterator link = json.FindMember("link");
    Value::ConstMemberIterator children = json.FindMember("children");

    if ((title == json.MemberEnd()) || !title->value.IsString())
    {
        TCWarn("Title is non-string or absent\n");
        return false;
    }
    if ((link == json.MemberEnd()) || !link->value.IsString())
    {
        TCWarn("Link is non-string or absent\n");
        return false;
    }
    if ((children == json.MemberEnd()) || !(children->value.IsArray() || children->value.IsNull()))
    {
        TCWarn("Children is not an array or null\n");
        return false;
    }

    if (children->value.IsArray())
    {
        for (SizeType i = 0; i < children->value.Size(); i++)
        {
            const Value& c = children->value[SizeType(i)];
            bool valid = VerifyJSONStructure(c);
            if (!valid)
                return false;
        }
    }

    return true;
}
