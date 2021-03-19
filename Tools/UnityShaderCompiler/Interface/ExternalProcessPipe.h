#pragma once
#include <string>
#include <exception>
#include <cstdlib>

#if PLATFORM_WIN
#include <windef.h>
#endif

namespace extprocess
{
    class PipeException : public std::exception
    {
    public:
        PipeException(const core::string& about) : m_What(about) {}
        ~PipeException() throw () {}
        virtual const char* what() const throw () { return m_What.c_str(); }
    private:
        core::string m_What;
    };

    // Pipe that uses unix pipes on mac and linux. NamedPipes on windows.
    class Pipe
    {
    public:
        Pipe(const core::string& pipeName);
        ~Pipe();

        void Flush();

        Pipe& Write(const core::string& str);
        core::string& ReadLine(core::string& target);
        core::string& PeekLine(core::string& dest);
        bool IsEOF() const;

    private:
        bool m_LineBufferValid;
        core::string m_LineBuffer;

#   if defined(_WINDOWS)
        HANDLE m_NamedPipe;
        core::string m_Buffer;
#   endif // if defined(_WINDOWS)
    };
} // namespace extprocess
