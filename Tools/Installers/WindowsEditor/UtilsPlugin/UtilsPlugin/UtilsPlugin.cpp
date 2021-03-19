#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#include "nsis\pluginapi.h"


namespace
{
    bool GetAccountNameAndDomain(TCHAR* name, DWORD nameLength, TCHAR* domain, DWORD domainLength)
    {
        bool result = false;

        HANDLE token = NULL;
        DWORD length;
        PTOKEN_USER user = NULL;
        SID_NAME_USE use;

        // open access token

        if (!OpenThreadToken(GetCurrentThread(), TOKEN_QUERY, TRUE, &token))
        {
            if (ERROR_NO_TOKEN != GetLastError())
            {
                goto done;
            }

            if (!OpenProcessToken(GetCurrentProcess(), TOKEN_QUERY, &token))
            {
                goto done;
            }
        }

        // allocate memory for user account

        if (GetTokenInformation(token, TokenUser, NULL, 0, &length))
        {
            goto done;
        }

        if (ERROR_INSUFFICIENT_BUFFER != GetLastError())
        {
            goto done;
        }

        user = reinterpret_cast<PTOKEN_USER>(new BYTE[length]);

        if (!user)
        {
            goto done;
        }

        // get user account data

        if (!GetTokenInformation(token, TokenUser, user, length, &length))
        {
            goto done;
        }

        // retrieve account name and domain

        if (!LookupAccountSid(NULL, user->User.Sid, name, &nameLength, domain, &domainLength, &use))
        {
            goto done;
        }

        // everything went ok

        result = true;

        // cleanup

    done:

        if (user)
        {
            delete[] reinterpret_cast<BYTE *>(user);
        }

        if (token)
        {
            CloseHandle(token);
        }

        return result;
    }
}

extern "C" void __declspec(dllexport) GetAccountNameAndDomain(HWND /*hwndParent*/, int string_size, TCHAR * variables, stack_t** stacktop, extra_parameters* /*extra*/)
{
    const DWORD nameLength = 1024;
    const DWORD domainLength = 1024;

    TCHAR* name = NULL;
    TCHAR* domain = NULL;

    // initialize nsis

    EXDLL_INIT();

    // allocate memory for account name and domain

    name = new TCHAR[nameLength];

    if (!name)
    {
        goto error;
    }

    domain = new TCHAR[domainLength];

    if (!domain)
    {
        goto error;
    }

    // get account name and domain

    if (!GetAccountNameAndDomain(name, nameLength, domain, domainLength))
    {
        goto error;
    }

    // everything went ok

    pushstring(name);
    pushstring(domain);

    goto done;

    // handle error

error:

    pushstring(_T(""));
    pushstring(_T(""));

    // cleanup

done:

    if (domain)
    {
        delete[] domain;
    }

    if (name)
    {
        delete[] name;
    }
}

static HANDLE heap;

void* operator new(size_t size)
{
    return HeapAlloc(heap, 0, size);
}

void operator delete(void* ptr)
{
    if (ptr)
    {
        HeapFree(heap, 0, ptr);
    }
}

BOOL WINAPI DllMain(HINSTANCE /*module*/, DWORD reason, LPVOID /*reserved*/)
{
    if (DLL_PROCESS_ATTACH == reason)
    {
        heap = GetProcessHeap();

        if (!heap)
        {
            return FALSE;
        }
    }

    return TRUE;
}
