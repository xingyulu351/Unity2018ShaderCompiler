#pragma once

#include <exception>

namespace ureport
{
    class WorkspaceIsNotInitializedException : public std::exception
    {
    };
}
