#include "Configuration/UnityConfigure.h"

#if UNITY_ASAN

// This function exists to provide default options we need in Unity to
// the clang address sanitizer. These options could also be passed via
// an environment variable. Since we always want to set these options
// for Unity, we pass them via this function with a special name. The
// options are:
//
// detect_container_overflow=0
//
// Our conainters (like dynamic_array) are not annotated for the address
// sanitizer, so we need to disable this check, as it will cause problems
// to be reported that are not really problems.
//
// handle_segv=0
//
// Disable the address sanitizers SIGSEGV handling. It installs its handler
// first, so Mono's handler won't be called when the address sanitizer is
// enabled. Since Mono uses SIGSEGV to implement NullReferenceException,
// this is a problem.

extern "C"
{
    __attribute__((visibility("default"))) const char* __asan_default_options()
    {
        return "detect_container_overflow=0:handle_segv=0";
    }
}

#endif
