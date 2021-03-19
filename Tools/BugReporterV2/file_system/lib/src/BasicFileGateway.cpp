#include "FileGateway.h"
#include "FileSystem.h"
#include "FileToRead.h"
#include "EmptyPathException.h"
#include "PathDoesNotExistException.h"
#include "shims/logical/IsEmpty.h"

#include <memory>

namespace ureport
{
namespace details
{
    class BasicFileGateway : public ureport::FileGateway
    {
        std::shared_ptr<FileSystem> m_FileSystem;

    public:
        BasicFileGateway(std::shared_ptr<FileSystem> fileSystem)
            : m_FileSystem(fileSystem)
        {
        }

        std::unique_ptr<File> GetFile(const std::string& path) const override
        {
            if (IsEmpty(path))
                throw EmptyPathException();
            if (!m_FileSystem->HasPath(path))
                throw PathDoesNotExistException(path);
            return std::unique_ptr<File>(new details::FileToRead(path));
        }

        size_t GetSize(const std::string& path) const override
        {
            return m_FileSystem->GetSize(path);
        }
    };
}

    std::unique_ptr<FileGateway> CreateBasicFileGateway(std::shared_ptr<FileSystem> fileSystem)
    {
        return std::unique_ptr<FileGateway>(new details::BasicFileGateway(fileSystem));
    }
}
