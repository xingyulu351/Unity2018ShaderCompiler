#pragma once

#include <memory>
#include <string>

namespace ureport
{
    class ZipFile;

    class File;

    std::unique_ptr<ZipFile> CreateNewZip(const std::string& path);

    void Pack(ZipFile& zip, const File& file);
}
