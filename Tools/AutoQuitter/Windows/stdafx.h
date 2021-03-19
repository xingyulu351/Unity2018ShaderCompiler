// stdafx.h : include file for standard system include files,
// or project specific include files that are used frequently, but
// are changed infrequently
//

#pragma once


#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#include <iostream>
#include <tchar.h>

extern "C" {
    WINBASEAPI
    HWND
    APIENTRY
        GetConsoleWindow(
        VOID
        );
};

// TODO: reference additional headers your program requires here
