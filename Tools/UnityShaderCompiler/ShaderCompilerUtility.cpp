#include "UnityPrefix.h"
#include "ShaderCompilerUtility.h"
#include "Runtime/Network/SocketStreams.h"
#include "Runtime/Profiler/TimeHelper.h"

// Magic number to ensure sender/receiver are in sync. Doesn't have
// to be bumped with protocol changes unless you might have different
// versions cross-talking.
const UInt32 kCompilerProtocolMagic = 201805020;

static std::vector<core::string> s_Commands;

void shadercompiler::Initialize()
{
    s_Commands.resize(kShaderCompilerCommandCount);
    const core::string commandPrefix("c:", kMemShader);
    s_Commands[kShaderCompilerInitialize] = commandPrefix + "initializeCompiler";
    s_Commands[kShaderCompilerClearIncludesCache] = commandPrefix + "clearIncludesCache";
    s_Commands[kShaderCompilerShutdown] = commandPrefix + "shutdown";
    s_Commands[kShaderCompilerPreprocess] = commandPrefix + "preprocess";
    s_Commands[kShaderCompilerCompileComputeKernel] = commandPrefix + "compileComputeKernel";
    s_Commands[kShaderCompilerTranslateComputeKernel] = commandPrefix + "translateComputeKernel";
    s_Commands[kShaderCompilerCompileSnippet] = commandPrefix + "compileSnippet";
    s_Commands[kShaderCompilerDisassembleShader] = commandPrefix + "disassembleShader";
    s_Commands[kShaderCompilerPreprocessCompute] = commandPrefix + "preprocessCompute";
}

void shadercompiler::Cleanup()
{
    s_Commands.clear();
}

void shadercompiler::WriteLine(ShaderCompilerConnection& conn, const core::string& msg)
{
    bool success = true;

    success &= conn.sock->SendAll(&kCompilerProtocolMagic, sizeof(kCompilerProtocolMagic), conn.writeTimeoutMS);

    UInt32 len = (UInt32)msg.length();
    success &= conn.sock->SendAll(&len, sizeof(len), conn.writeTimeoutMS);
    if (len > 0)
        success &= conn.sock->SendAll(msg.c_str(), len, conn.writeTimeoutMS);

    if (!success)
        throw SocketException("Failed writing data to socket");
}

bool shadercompiler::Read(ShaderCompilerConnection& conn, void* buffer, size_t numBytes)
{
    const bool useTimeout = (conn.readTimeoutMS > 0);
    const TimeFormat startTime = useTimeout ? GetProfilerTime() : 0;
    bool success = conn.sock->RecvAll(buffer, numBytes, conn.readTimeoutMS);
    if (!success && useTimeout)
    {
        if (TimeToMilliseconds(ELAPSED_TIME(startTime)) >= conn.readTimeoutMS)
            throw SocketException("Read timeout from shader compiler process", true);
    }
    return success;
}

bool shadercompiler::ReadHeader(ShaderCompilerConnection& conn, UInt32& outNumBytes)
{
    bool success = true;
    UInt32 magic = 0;
    success &= Read(conn, &magic, sizeof(magic));

    if (!success)
        throw SocketException("Protocol error - unexpected end of data (did the Editor crash?)");
    if (magic != kCompilerProtocolMagic)
        throw SocketException("Protocol error - failed to read correct magic number");

    outNumBytes = 0;
    success &= Read(conn, &outNumBytes, sizeof(outNumBytes));
    return success;
}

void shadercompiler::ReadLine(ShaderCompilerConnection& conn, core::string& outMsg)
{
    UInt32 len = 0;
    bool success = shadercompiler::ReadHeader(conn, len);
    outMsg.resize(len);
    if (len > 0)
        success &= Read(conn, &outMsg[0], len);

    if (!success)
        throw SocketException("Failed reading data from socket");
}

void shadercompiler::SendCommand(ShaderCompilerConnection& conn, ShaderCompilerCommand command)
{
    WriteLine(conn, s_Commands[command]);
}

bool shadercompiler::ReadCommand(ShaderCompilerConnection& conn, core::string& cmd)
{
    cmd.clear();

    core::string read;
    ReadLine(conn, read);

    if (read.empty())
    {
        // empty message -> error.
        if (conn.sock->IsConnected())
            return false;

        // socket closed -> shutdown.
        return true;
    }

    // Skip non-command lines if present
    size_t stopCounter = 1000;
    while ((read.size() < 2 || read[0] != 'c' || read[1] != ':') && stopCounter--)
    {
        ReadLine(conn, read);
    }

    if (!stopCounter)
        return false;

    cmd = read.substr(2);
    return true;
}

void shadercompiler::WriteBuffer(ShaderCompilerConnection& conn, const void* data, size_t length)
{
    bool success = true;

    success &= conn.sock->SendAll(&kCompilerProtocolMagic, sizeof(kCompilerProtocolMagic), conn.writeTimeoutMS);

    UInt32 len = (UInt32)length;
    success &= conn.sock->SendAll(&len, sizeof(len), conn.writeTimeoutMS);
    if (len > 0)
        success &= conn.sock->SendAll(data, len, conn.writeTimeoutMS);

    if (!success)
        throw SocketException("Failed writing data to socket");
}

void shadercompiler::ReadBuffer(ShaderCompilerConnection& conn, dynamic_array<UInt8>& buffer)
{
    UInt32 len = 0;
    bool success = shadercompiler::ReadHeader(conn, len);
    buffer.resize_uninitialized(len);
    if (len > 0)
        success &= Read(conn, &buffer[0], len);

    if (!success)
        throw SocketException("Failed reading buffer from socket");
}

core::string shadercompiler::ReadString(ShaderCompilerConnection& conn)
{
    core::string res;
    ReadLine(conn, res);
    return res;
}

int shadercompiler::ReadInt(ShaderCompilerConnection& conn)
{
    core::string res;
    ReadLine(conn, res);
    return StringToInt(res);
}

UInt32 shadercompiler::ReadUInt32(ShaderCompilerConnection& conn)
{
    core::string res;
    ReadLine(conn, res);
    return StringToUInt32(res);
}

bool shadercompiler::ReadBool(ShaderCompilerConnection& conn)
{
    core::string res;
    ReadLine(conn, res);
    return res == "1";
}

void shadercompiler::WriteString(ShaderCompilerConnection& conn, const core::string& v)
{
    WriteLine(conn, v);
}

void shadercompiler::WriteInt(ShaderCompilerConnection& conn, int v)
{
    WriteLine(conn, IntToString(v));
}

void shadercompiler::WriteUInt32(ShaderCompilerConnection& conn, UInt32 v)
{
    WriteLine(conn, UnsignedIntToString(v));
}

void shadercompiler::WriteBool(ShaderCompilerConnection& conn, bool v)
{
    WriteLine(conn, v ? "1" : "0");
}
