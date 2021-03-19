#pragma once

#include <string>
#include <istream>
#include <memory>

namespace ureport
{
    class File
    {
    public:
        virtual std::string GetPath() const = 0;

        virtual bool IsDir() const = 0;

        virtual std::unique_ptr<std::istream> Read() const = 0;

        virtual void Write(std::istream& data) = 0;

        virtual ~File() {}
    };
}
