#include "UnityPrefix.h"
#include "ExternalProcessConnection.h"
#include "Runtime/Utilities/Word.h"
#include "Runtime/Utilities/dynamic_array.h"

namespace extprocess
{
    Connection::Connection(LogStream& log, const core::string& pipeName)
        : m_Log(log)
        , m_Pipe(NULL)
        , m_PipeName(pipeName)
    {
    }

    Connection::~Connection()
    {
        delete m_Pipe;
        m_Pipe = NULL;
    }

    void Connection::Connect()
    {
        Assert(m_Pipe == NULL);
        m_Pipe = new Pipe(m_PipeName);
    }

    bool Connection::ReadCommand(core::string& cmd)
    {
        cmd.clear();
        try
        {
            if (!IsConnected())
                Connect();
        }
        catch (std::exception& e)
        {
            Log().Notice() << "While reading command: " << e.what() << Endl;
            return false;
        }

        core::string read;
        ReadLine(read);

        if (read.empty())
        {
            // broken pipe -> error.
            if (!m_Pipe->IsEOF())
            {
                Log().Fatal() << "Read empty command from connection" << Endl;
                return false;
            }

            // end of pipe -> shutdown.
            Log().Info() << "End of pipe" << Endl;
            return true;
        }

        // Skip non-command lines if present
        size_t stopCounter = 1000;
        while (read.substr(0, 2) != "c:" && stopCounter--)
        {
            ReadLine(read);
        }

        if (!stopCounter)
        {
            Log().Fatal() << "Too many bogus lines" << Endl;
            return false;
        }

        cmd = read.substr(2);
        return true;
    }

    LogStream& Connection::Log()
    {
        return m_Log;
    }

    bool Connection::IsConnected() const
    {
        return m_Pipe != NULL;
    }

    void Connection::Flush()
    {
        m_Pipe->Flush();
    }

    core::string& Connection::ReadLine(core::string& target)
    {
        m_Pipe->ReadLine(target);
        DecodeStringWithNewlines(target);

        if (OPTIMIZER_UNLIKELY(target.size() > 300))
            m_Log.Debug() << "UNITY > " << target.substr(0, 300) << " ..."  << Endl;
        else
            m_Log.Debug() << "UNITY > " << target << Endl;

        return target;
    }

    core::string& Connection::PeekLine(core::string& target)
    {
        m_Pipe->PeekLine(target);
        DecodeStringWithNewlines(target);
        return target;
    }

    // Encode newlines in strings
    Connection& Connection::Write(const core::string& v)
    {
        core::string tmp = v;
        EncodeStringWithNewlines(tmp);

        if (OPTIMIZER_UNLIKELY(tmp.size() > 300))
            m_Log.Debug() << tmp.substr(0, 300) << " ...";
        else
            m_Log.Debug() << tmp;

        m_Pipe->Write(tmp);
        return *this;
    }

    // Encode newlines in strings
    Connection& Connection::Write(const char* v)
    {
        return Write(core::string(v));
    }

    Connection& Connection::WriteEndl()
    {
        m_Log.Debug() << "\n";
        m_Pipe->Write("\n");
        return *this;
    }
} // namespace extprocess
