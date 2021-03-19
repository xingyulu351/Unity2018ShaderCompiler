#include "FreshZipFile.h"
#include "EmptyZipPathException.h"
#include "shims/logical/IsEmpty.h"

#include <memory>
#include <string>

namespace ureport
{
    std::unique_ptr<ZipFile> CreateNewZip(const std::string& path)
    {
        if (IsEmpty(path))
            throw EmptyZipPathException();
        return std::unique_ptr<ZipFile>(new details::FreshZipFile(path));
    }

    void Pack(ZipFile& zip, const File& file)
    {
        if (file.IsDir())
        {
            zip.CreateDirectory(file.GetPath());
        }
        else
        {
            zip.OpenNewEntry(file.GetPath());
            zip.Write(*(file.Read()));
        }
    }
}
