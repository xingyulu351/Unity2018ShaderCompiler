#include "UnityPrefix.h"
#include "ExternalProcessPipe.h"
#include "Runtime/Utilities/Word.h"

#ifdef _WINDOWS
#include "PlatformDependent/Win/WinUnicode.h"
#include "PlatformDependent/Win/WinUtils.h"
#include <fileapi.h>
#include <namedpipeapi.h>
#include <winerror.h>
#else
#include <iostream>
#endif


namespace extprocess
{
    Pipe::Pipe(const core::string& pipeName)
        : m_LineBufferValid(false)
    {
#   if defined(_WINDOWS)
        core::wstring pipeNameWide;
        ConvertUTF8ToWideString(pipeName, pipeNameWide);

        // All pipe instances are busy, so wait for 2 seconds.
        if (!WaitNamedPipeW(pipeNameWide.c_str(), 2000))
        {
            core::string msg = "Could not open pipe: 2 second wait timed out.\n";
            msg += winutils::ErrorCodeToMsg(GetLastError());
            throw PipeException(msg);
        }

        m_NamedPipe = CreateFileW(
            pipeNameWide.c_str(),                  // pipe name
            GENERIC_READ |                   // read and write access
            GENERIC_WRITE,
            0,                               // no sharing
            NULL,                            // default security attributes
            OPEN_EXISTING,                   // opens existing pipe
            0,                               // default attributes
            NULL);                           // no template file

        // Break if the pipe handle is valid.
        if (m_NamedPipe == INVALID_HANDLE_VALUE)
        {
            // Exit if an error other than ERROR_PIPE_BUSY occurs.
            core::string msg = "Could not open pipe. GLE=";
            msg += winutils::ErrorCodeToMsg(GetLastError());
            throw PipeException(msg);
        }
#   endif // if defined(_WINDOWS)
    }

    Pipe::~Pipe()
    {
#   if defined(_WINDOWS)
        if (m_NamedPipe != INVALID_HANDLE_VALUE)
        {
            FlushFileBuffers(m_NamedPipe);

            CloseHandle(m_NamedPipe);
        }
#   endif // if defined(_WINDOWS)
    }

    void Pipe::Flush()
    {
#   if defined(_WINDOWS)
        FlushFileBuffers(m_NamedPipe);
#   else
        std::cout << std::flush;
#   endif
    }

    Pipe& Pipe::Write(const core::string& str)
    {
#   if defined(_WINDOWS)
        const CHAR* buf = str.c_str();
        DWORD toWrite = static_cast<DWORD>(str.length());
        DWORD written;
        BOOL success = WriteFile(m_NamedPipe,        // pipe handle
            buf,                              // message
            toWrite,                               // message length
            &written,                              // bytes written
            NULL);                                   // not overlapped

        if (!success)
        {
            core::string msg = "WriteFile to pipe failed. GLE=";
            msg += winutils::ErrorCodeToMsg(GetLastError());
            throw PipeException(msg);
        }

#   else

        std::cout << str;

#   endif

        return *this;
    }

    core::string& Pipe::ReadLine(core::string& target)
    {
        if (m_LineBufferValid)
        {
            target.swap(m_LineBuffer);
            m_LineBufferValid = false;
            return target;
        }

#   if defined(_WINDOWS)

        BOOL success;
        DWORD bytesRead;
        const size_t kBufSize = 4096 * 8;
        char buffer[kBufSize];

        // Read until a newline appears in the m_Buffer
        while (true)
        {
            // First check if we have already buffered lines up for grabs
            core::string::size_type i = m_Buffer.find("\n");
            if (i != core::string::npos)
            {
                success = true;
                target = m_Buffer.substr(0, i);
                ++i; // Eat \n
                if (i >= m_Buffer.length())
                    m_Buffer.clear();
                else
                    m_Buffer = m_Buffer.substr(i);
                break; // success
            }

            // Read from the pipe.
            success = ReadFile(m_NamedPipe, // pipe handle
                buffer,               // buffer to receive reply
                kBufSize,             // size of buffer
                &bytesRead,             // number of bytes read
                NULL);               // not overlapped

            if (!success && GetLastError() != ERROR_MORE_DATA)
                break; // error

            // Append data to the buffer
            m_Buffer.append(buffer, bytesRead);
        }

        if (!success)
        {
            core::string msg = "Readfile from pipe failed. GLE=";
            msg += winutils::ErrorCodeToMsg(GetLastError());
            throw PipeException(msg);
        }
#   else
        std::getline(std::cin, target);
#   endif
        return target;
    }

    core::string& Pipe::PeekLine(core::string& dest)
    {
        ReadLine(m_LineBuffer);
        dest = m_LineBuffer;
        m_LineBufferValid = true;
        return dest;
    }

    bool Pipe::IsEOF() const
    {
#   if defined(_WINDOWS)
        return false; // TODO: Implement
#   else
        return std::cin.eof();
#   endif
    }
} // namespace extprocess
