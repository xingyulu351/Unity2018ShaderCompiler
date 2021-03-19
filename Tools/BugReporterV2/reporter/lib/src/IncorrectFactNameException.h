#pragma once

#include <exception>

namespace ureport
{
    class IncorrectFactNameException : public std::exception
    {
    };
}
