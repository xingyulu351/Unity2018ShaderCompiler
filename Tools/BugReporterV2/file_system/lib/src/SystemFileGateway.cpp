#include "BasicFileGateway.h"
#include "FileSystem.h"

#include <memory>

namespace ureport
{
    std::unique_ptr<FileGateway> CreateSystemFileGateway()
    {
        return CreateBasicFileGateway(CreateFileSystem());
    }
}
