#include "unistd.h"

namespace ureport
{
    void Sleep(int milliseconds)
    {
        ::usleep(milliseconds * 1000);
    }
}
