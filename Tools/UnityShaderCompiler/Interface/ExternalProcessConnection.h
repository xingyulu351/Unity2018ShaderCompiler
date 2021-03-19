#pragma once
#include <string>
#include "ExternalProcessLog.h"
#include "ExternalProcessPipe.h"
#include "Runtime/Utilities/Word.h"
#include "Runtime/Utilities/dynamic_array.h"

namespace extprocess
{
    class Connection
    {
    public:
        Connection(LogStream& log, const core::string& pipeName);
        ~Connection();

        // Connect to Unity
        void Connect();
        bool IsConnected() const;

        // Read a command from unity
        bool ReadCommand(core::string& cmd);

        // Get the log stream
        LogStream& Log();

        void Flush();

        core::string& ReadLine(core::string& target);
        core::string& PeekLine(core::string& target);

        // Encode newlines in strings
        Connection& Write(const core::string& v);
        Connection& Write(const char* v);
        Connection& WriteEndl();
        Connection& WriteLine(const core::string& v)
        {
            Write(v);
            WriteEndl();
            Flush();
            return *this;
        }

        core::string ReadString()
        {
            core::string res;
            ReadLine(res);
            return res;
        }

        int ReadInt()
        {
            core::string res;
            ReadLine(res);
            return StringToInt(res);
        }

        bool ReadBool()
        {
            core::string res;
            ReadLine(res);
            return res == "1";
        }

        void WriteString(const core::string& v)
        {
            WriteLine(v);
        }

        void WriteInt(int v)
        {
            WriteLine(IntToString(v));
        }

        void WriteBool(bool v)
        {
            WriteLine(v ? "1" : "0");
        }

        // Really the underlying pipe should be pushing a binary standard, but since it is not
        // we need to convert to text before pushing a generic buffer over the pipe.
    #define ITOC(NIB) (NIB >= 10 ? (unsigned char)('a' + (NIB - 10)) : '0' + NIB)
        template<typename ARRAYTYPE>
        void WriteBuffer(ARRAYTYPE & buffer)
        {
            core::string data;
            data.resize(buffer.size() * 2);
            for (unsigned int i = 0; i < buffer.size(); ++i)
            {
                unsigned char c = buffer[i];
                unsigned char nib = c & 0xF;
                data[(i * 2) + 0] = ITOC(nib);
                nib = c >> 4;
                data[(i * 2) + 1] = ITOC(nib);
            }
            WriteLine(data);
        }

    #undef ITOC

    private:
        LogStream& m_Log;
        Pipe* m_Pipe;
        core::string m_PipeName;
    };
} // namespace extprocess
