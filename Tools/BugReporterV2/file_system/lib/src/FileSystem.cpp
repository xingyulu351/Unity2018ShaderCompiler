#include "FileSystem.h"
#include "EmptyPathException.h"
#include "PathDoesNotExistException.h"
#include "DirEntry.h"
#include "ProjectDirEntry.h"
#include "FileEntry.h"
#include "UnityProjectIdentifier.h"

#include <memory>

namespace ureport
{
    std::shared_ptr<FileSystemEntry> CreateFileSystemEntry(
        std::shared_ptr<FileSystem> fileSystem, const std::string& path)
    {
        if (path.empty())
            throw EmptyPathException();
        if (!fileSystem->HasPath(path))
            throw PathDoesNotExistException(path);
        if (fileSystem->IsDir(path))
        {
            UnityProjectIdentifier identifier(fileSystem);
            if (identifier.IsUnityProjectDir(path))
                return std::shared_ptr<FileSystemEntry>(
                    new details::ProjectDirEntry(path, fileSystem)
                );
            else
                return std::shared_ptr<FileSystemEntry>(
                    new details::DirEntry(path, fileSystem));
        }
        else
            return std::shared_ptr<FileSystemEntry>(new details::FileEntry(path, fileSystem));
    }
}
