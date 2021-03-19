#include "WindowsArgUtils.h"

#include <Windows.h>
#include <vector>

int Launch(const std::vector<std::string>& args);

int CALLBACK WinMain(HINSTANCE instance, HINSTANCE prevInstance, LPSTR cmdLine, int show)
{
    using namespace ureport::windows;
    return Launch(MakeArgsVector(GetCommandLineW()));
}
