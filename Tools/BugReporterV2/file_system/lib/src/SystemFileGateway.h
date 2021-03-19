#pragma once

#include "FileGateway.h"

#include <memory>

namespace ureport
{
    class FileGateway;

    std::unique_ptr<FileGateway> CreateSystemFileGateway();
}
