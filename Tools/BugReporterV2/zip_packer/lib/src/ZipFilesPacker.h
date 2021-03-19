#pragma once

#include <memory>
#include <string>

namespace ureport
{
    class FilesPacker;
    class FileGateway;
    class SystemEnvironment;

    std::unique_ptr<FilesPacker> CreateZipFilesPacker(std::shared_ptr<FileGateway> files,
        std::shared_ptr<SystemEnvironment> environment);
}
