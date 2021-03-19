#pragma once

#include <exception>

namespace ureport
{
    class NoSenderException : public std::exception
    {
    };
}
