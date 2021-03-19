#include "UnityPrefix.h"
#include "IndexCombiner.h"
#include "Runtime/Utilities/RapidJSONConfig.h"
#include <RapidJSON/prettywriter.h>
#include <RapidJSON/filestream.h>
#include <string>
#include "RapidJsonIoUtil.h"

using namespace Unity::rapidjson;

const char commonSt[] = "common";

IndexCombiner::IndexCombiner()
    : JsonCombiner()
{
    m_EmptyDocString =
        "{\"pages\":[],\n"
        "\"info\": [],\n"
        "\"common\": {},\n"
        "\"searchIndex\": {}}\n";
    if (!RapidJsonIoUtil::TryGetJsonDocFromString(m_EmptyDocString.c_str(), m_CombinedToc))
        TCDie("Failed to initialize IndexCombiner with the default JSON\n");
}

void IndexCombiner::CombinePagesJson(Value& targetPages, Value& sourcePages)
{
    assert(targetPages.IsArray());
    assert(sourcePages.IsArray());

    for (SizeType i = 0; i < sourcePages.Size(); i++)
    {
        SizeType rji = SizeType(i);
        Value& valAtIndex = sourcePages[rji];
        assert(valAtIndex.IsArray());

        //expects the original element pair plus the module number
        assert(valAtIndex.Size() == 3);
        Value& sourceURL = valAtIndex[SizeType(0)];
        assert(sourceURL.IsString());

        //There will be no duplicates if the pages are populated with prefixes in advance
        targetPages.PushBack(valAtIndex, m_CombinedToc.GetAllocator());
    }
}

void IndexCombiner::CombineInfoJson(Value& targetInfo, Value& sourceInfo, int numTargetPages)
{
    assert(targetInfo.IsArray());
    assert(sourceInfo.IsArray());

    // each entry is indexing into "pages" array
    for (SizeType i = 0; i < sourceInfo.Size(); i++)
    {
        SizeType sourceIndex = SizeType(i); //array index in the source file
        Value& sourceValue = sourceInfo[sourceIndex]; //this should return a [string,number] pair
        assert(sourceValue.IsArray());
        assert(sourceValue.Size() == 2);
        Value& key = sourceValue[SizeType(0)];
        assert(key.IsString());
        Value& value = sourceValue[SizeType(1)];
        assert(value.IsInt());

        value = value.GetInt() + numTargetPages;
        targetInfo.PushBack(sourceValue, m_CombinedToc.GetAllocator());
    }
}

void IndexCombiner::CombineSearchIndexJson(Value& targetSearchIndex, Value& sourceSearchIndex, int numTargetPages)
{
    assert(targetSearchIndex.IsObject());
    assert(sourceSearchIndex.IsObject());

    // iterate over all searchIndex object entries from source
    // each entry is key (search term), and value (list of page indexes, indexing into "info" array).
    for (Value::MemberIterator itr = sourceSearchIndex.MemberBegin(); itr != sourceSearchIndex.MemberEnd(); itr++)
    {
        Value& sourceKey = itr->name;
        assert(sourceKey.IsString());
        Value& sourcePageList = itr->value;
        assert(sourcePageList.IsArray());

        // add current pageCount to each page index
        for (SizeType i = 0; i < sourcePageList.Size(); i++)
        {
            int number = sourcePageList[SizeType(i)].GetInt();
            sourcePageList[SizeType(i)] = number + numTargetPages;
        }

        // find entry with the same key (term) in the target searchIndex;
        // if found -- append entries, to the list;
        // add new otherwise
        const char* chSourceKey = sourceKey.GetString();

        Value::MemberIterator targetMember = targetSearchIndex.FindMember(chSourceKey);
        if (targetMember != targetSearchIndex.MemberEnd())
        {
            Value& targetPageList = targetMember->value;
            assert(targetPageList.IsArray());
            for (SizeType i = 0; i < sourcePageList.Size(); i++)
            {
                targetPageList.PushBack(sourcePageList[SizeType(i)], m_CombinedToc.GetAllocator());
            }
        }
        else
        {
            targetSearchIndex.AddMember(sourceKey, sourcePageList, m_CombinedToc.GetAllocator());
        }
    }
}

//appends creates a combination of A and B and "returns" it as combined
void IndexCombiner::AppendJsonDocument(Document& dSource)
{
    assert(!m_CombinedToc.IsNull());

    assert(m_CombinedToc.HasMember(commonSt));

    Value& commonValue = m_CombinedToc[commonSt];

    if (commonValue.MemberBegin() == commonValue.MemberEnd())
    {
        m_CombinedToc[commonSt].CopyFrom(dSource[commonSt], m_CombinedToc.GetAllocator());
    }

    // the following counts are added to each index in the "info" and "searchIndex" values
    int numTargetPages = m_CombinedToc["pages"].Size();
    int numTargetInfos = m_CombinedToc["info"].Size();

    CombinePagesJson(m_CombinedToc["pages"], dSource["pages"]);

    CombineInfoJson(m_CombinedToc["info"], dSource["info"], numTargetPages);

    assert(m_CombinedToc["pages"].IsArray());

    CombineSearchIndexJson(m_CombinedToc["searchIndex"], dSource["searchIndex"], numTargetInfos);
}

void IndexCombiner::AddModuleIndexToEachEntry(Document& doc, int index)
{
    Value& pagesVal = doc["pages"];
    assert(pagesVal.IsArray());

    for (SizeType i = 0; i < pagesVal.Size(); i++)
    {
        SizeType rji = SizeType(i);
        Value& valAtIndex = pagesVal[rji];
        assert(valAtIndex.IsArray());
        //the original size is 2, before we inject the prefix
        assert(valAtIndex.Size() == 2);
        Value& sourceURL = valAtIndex[SizeType(0)]; //url
        assert(sourceURL.IsString());
        Value rj_index(index);
        valAtIndex.PushBack(rj_index, doc.GetAllocator());
    }
}

bool IndexCombiner::OutputJsonValueAsJs(const char *currentPlatformPath, const Document& doc, const char* fname)
{
    StringBuffer bufferCommon;
    Writer<StringBuffer> writerCommon(bufferCommon);
    StringBuffer bufferInfo;
    Writer<StringBuffer> writerInfo(bufferInfo);
    StringBuffer bufferSearch;
    Writer<StringBuffer> writerSearch(bufferSearch);

    bool res = false;

    const Value& pagesVal = doc["pages"];
    const Value& infoVal = doc["info"];
    const Value& commonVal = doc["common"];
    const Value& searchIndexVal = doc["searchIndex"];

    FILE* f = fopen(fname, "w+b");
    if (!f)
    {
        TCDie("Unable to open file %s for writing", fname)
        return false;
    }

    if (!OutputPrefixVars(currentPlatformPath, f))
    {
        goto error;
    }

    if (0 > fprintf(f, "pages = [\n"))
    {
        goto error;
    }

    for (SizeType i = 0; i < pagesVal.Size(); i++)
    {
        const Value& curPage = pagesVal[SizeType(i)];
        assert(curPage.IsArray() && curPage.Size() == 3);
        int curIndex = curPage[SizeType(2)].GetInt();
        const char* curUrl = curPage[SizeType(0)].GetString();
        const char* curName = curPage[SizeType(1)].GetString();

        int sz = (int)strlen(curName) * 2;
        char* escaped = new char[sz];
        EscapeDoubleQuotedString(curName, escaped, sz);

        int res = fprintf(f, "[p%d + \"%s\", \"%s\"]", curIndex, curUrl, escaped);
        delete[] escaped;

        if (0 > res)
        {
            goto error;
        }
        if (i < pagesVal.Size() - 1)
            if (0 > fprintf(f, ","))
            {
                goto error;
            }
        if (0 > fprintf(f, "\n"))
        {
            goto error;
        }
    }
    if (0 > fprintf(f, "];"))
    {
        goto error;
    }

    if (0 > fprintf(f, "\ninfo = "))
    {
        goto error;
    }

    infoVal.Accept(writerInfo);

    if (0 > fprintf(f, "%s", bufferInfo.GetString()))
    {
        goto error;
    }

    if (0 > fprintf(f, "\ncommon = "))
    {
        goto error;
    }

    commonVal.Accept(writerCommon);

    if (0 > fprintf(f, "%s", bufferInfo.GetString()))
    {
        goto error;
    }

    if (0 > fprintf(f, ";\nsearchIndex = "))
    {
        goto error;
    }

    searchIndexVal.Accept(writerSearch);
    if (0 > fprintf(f, "%s", bufferSearch.GetString()))
    {
        goto error;
    }

    if (0 > fprintf(f, ";\n"))
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

bool IndexCombiner::VerifyJSONStructure(const Value &json)
{
    // root note is object
    if (!json.IsObject())
        return false;

    // it must contain arrays: pages, info
    // and objects: common, searchIndex
    Value::ConstMemberIterator pages = json.FindMember("pages");
    Value::ConstMemberIterator info = json.FindMember("info");
    Value::ConstMemberIterator common = json.FindMember("common");
    Value::ConstMemberIterator searchIndex = json.FindMember("searchIndex");


    if ((pages == json.MemberEnd()) || !pages->value.IsArray())
    {
        TCWarn("Pages is not an array or absent\n");
        return false;
    }

    if ((info == json.MemberEnd()) || !info->value.IsArray())
    {
        TCWarn("Info is not an array or absent\n");
        return false;
    }

    if ((common == json.MemberEnd()) || !common->value.IsObject())
    {
        TCWarn("Common is not an object or absent\n");
        return false;
    }

    if ((searchIndex == json.MemberEnd()) || !searchIndex->value.IsObject())
    {
        TCWarn("SearchIndex is not an object or absent\n");
        return false;
    }

    // eash "pages" element is an array of 2 elements [string,string]
    for (SizeType i = 0; i < pages->value.Size(); i++)
    {
        const Value& page = pages->value[SizeType(i)];
        if (!page.IsArray() || page.Size() != 2)
        {
            TCWarn("Content in pages is not an array of 2 elements\n");
            return false;
        }

        for (SizeType p = 0; p < page.Size(); p++)
        {
            const Value &pvalue = page[SizeType(p)];
            if (!pvalue.IsString())
            {
                TCWarn("Content in pages sub-element is not a string\n");
                return false;
            }
        }
    }

    // each "info" element is an array of 2 elements [string,int]
    // where in is an index into "pages"
    for (SizeType i = 0; i < info->value.Size(); i++)
    {
        const Value &inf = info->value[SizeType(i)];
        if (!inf.IsArray() || inf.Size() != 2)
        {
            TCWarn("Content in info is not an array of 2 elements\n");
            return false;
        }
        if (!inf[SizeType(0)].IsString())
        {
            TCWarn("1st item in info sub-element is not a string\n");
            return false;
        }
        if (!inf[SizeType(1)].IsInt())
        {
            TCWarn("2nd item in info sub-element is not an int\n");
            return false;
        }

        if (inf[SizeType(1)].GetInt() >= pages->value.Size())
        {
            TCWarn("2nd item in info sub-element is out of bounds\n");
            return false;
        }
    }

    // each "common" element key-value {string,int=1}
    for (Value::ConstMemberIterator itr = common->value.MemberBegin(); itr != common->value.MemberEnd(); itr++)
    {
        if (!itr->name.IsString())
        {
            TCWarn("Key in common is not a string\n");
            return false;
        }
        if (!itr->value.IsInt() || itr->value.GetInt() != 1)
        {
            TCWarn("Value in common is not an int(1)\n");
            return false;
        }
    }

    // each searchIndex element is key-value {string,[indexes]},
    // where indexes index into "info" array
    for (Value::ConstMemberIterator itr = searchIndex->value.MemberBegin(); itr != searchIndex->value.MemberEnd(); itr++)
    {
        if (!itr->name.IsString())
        {
            TCWarn("Key in searchIndex is not string\n");
            return false;
        }
        if (!itr->value.IsArray())
        {
            TCWarn("Value in searchIndex is not array\n");
            return false;
        }

        const Value& index = itr->value;
        for (SizeType i = 0; i < index.Size(); i++)
        {
            if (!index[SizeType(i)].IsInt())
            {
                TCWarn("Value in index is not int\n");
                return false;
            }
            if (index[SizeType(i)].GetInt() >= info->value.Size())
            {
                TCWarn("Value in index is out of bounds\n");
                return false;
            }
        }
    }

    return true;
}
