#include "Runtime/Utilities/RapidJSONConfig.h"
#include "RapidJSON/document.h"

#include <fstream>
#include <sstream>
#include <iostream>
#include "RapidJsonIoUtil.h"
#include "TocCombiner.h"
#include "IndexCombiner.h"

#include <RapidJSON/filestream.h>
#include <RapidJSON/prettywriter.h>
#include <RapidJSON/writer.h>
#include <RapidJSON/stringbuffer.h>

// NOTE this function is only used for debugging,
// and whilst it can't handle errors properly -- there's no need to fix it
bool RapidJsonIoUtil::OutputJsonValueToFile(const Value& jsonVal, const char* fname)
{
    FILE* f = fopen(fname, "w+b");
    if (!f)
    {
        TCWarn("Failed to open %s for writing\n", fname);
        return false;
    }
    FileStream fs(f);
    PrettyWriter<FileStream> writer(fs);
    jsonVal.Accept(writer); // Accept() traverses the DOM and generates Handler events.
    fclose(f);

    return true;
}

std::string RapidJsonIoUtil::OutputJsonValueToString(const Value& jsonVal)
{
    StringBuffer sb;
    Writer<StringBuffer> writer(sb);
    jsonVal.Accept(writer); // Accept() traverses the DOM and generates Handler events.
    return sb.GetString();
}

bool RapidJsonIoUtil::TryGetJsonDocFromFile(const char* fname, Document& d)
{
    FILE *fp = fopen(fname, "rb");
    if (!fp)
    {
        TCWarn("Unable to read %s\n", fname);
        return false;
    }
    fseek(fp, 0, SEEK_END);
    size_t size = ftell(fp);
    rewind(fp);

    char *buffer = new char[size + 1];
    size_t rb = fread(buffer, 1, size, fp);
    fclose(fp);

    if (rb != size)
    {
        TCWarn("Unable to read entire file %s, requested %d, got %d bytes\n", fname, (int)size, (int)rb);
        free(buffer);
        return false;
    }

    buffer[size] = 0;

    bool result = TryGetJsonDocFromString(buffer, d);
    if (!result)
    {
        size_t offset = d.GetErrorOffset();
        TCError("Failed to parse JSON file %s, error at offset: %d\n", fname, (int)offset);
        return false;
    }
    return true;
}

bool RapidJsonIoUtil::TryGetJsonDocFromString(const char* str, Document& d)
{
    d.Parse<0>(str);
    return !d.HasParseError();
}
