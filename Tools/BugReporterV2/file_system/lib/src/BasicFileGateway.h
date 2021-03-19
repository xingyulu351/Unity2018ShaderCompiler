#pragma once

#include "FileGateway.h"

#include <memory>

namespace ureport
{
    class FileSystem;

    std::unique_ptr<FileGateway> CreateBasicFileGateway(std::shared_ptr<FileSystem> fileSystem);
}
