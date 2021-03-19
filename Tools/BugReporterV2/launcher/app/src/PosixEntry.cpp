#include "ArgUtils.h"

#include <vector>

int Launch(const std::vector<std::string>& args);

int main(int argc, char* argv[])
{
    using namespace ureport;
    return Launch(MakeArgsVector(argc, const_cast<const char**>(argv)));
}
