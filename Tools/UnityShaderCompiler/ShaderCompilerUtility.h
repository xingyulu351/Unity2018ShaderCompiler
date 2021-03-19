#pragma once

#include "Runtime/Utilities/dynamic_array.h"

class ExternalProcess;
class SocketStream;

struct ShaderCompilerConnection
{
    ExternalProcess *proc;
    SocketStream *sock;

    UInt32 readTimeoutMS;
    UInt32 writeTimeoutMS;

    UInt32 timeoutRetryCount;
};

enum ShaderCompilerCommand
{
    kShaderCompilerInitialize,
    kShaderCompilerClearIncludesCache,
    kShaderCompilerShutdown,
    kShaderCompilerPreprocess,
    kShaderCompilerCompileComputeKernel,
    kShaderCompilerTranslateComputeKernel,
    kShaderCompilerCompileSnippet,
    kShaderCompilerDisassembleShader,
    kShaderCompilerPreprocessCompute,

    kShaderCompilerCommandCount
};

namespace shadercompiler
{
    class SocketException : public std::exception
    {
    public:
        SocketException(const core::string& about) : m_What(about), m_TimeOut(false) {}
        SocketException(const core::string& about, bool timeOut) : m_What(about), m_TimeOut(timeOut) {}
        ~SocketException() throw () {}
        virtual const char* what() const throw () { return m_What.c_str(); }
        bool IsTimeOut() const { return m_TimeOut; }
    private:
        core::string m_What;
        bool m_TimeOut;
    };

    bool Read(ShaderCompilerConnection& conn, void* buffer, size_t numBytes);
    bool ReadHeader(ShaderCompilerConnection& conn, UInt32& outNumBytes);

    void WriteLine(ShaderCompilerConnection& conn, const core::string& msg);
    void ReadLine(ShaderCompilerConnection& conn, core::string& outMsg);

    void SendCommand(ShaderCompilerConnection& conn, ShaderCompilerCommand command);
    bool ReadCommand(ShaderCompilerConnection& conn, core::string& cmd);

    void WriteBuffer(ShaderCompilerConnection& conn, const void* data, size_t length);
    void ReadBuffer(ShaderCompilerConnection& conn, dynamic_array<UInt8>& buffer);

    core::string ReadString(ShaderCompilerConnection& conn);
    int ReadInt(ShaderCompilerConnection& conn);
    UInt32 ReadUInt32(ShaderCompilerConnection& conn);
    bool ReadBool(ShaderCompilerConnection& conn);

    void WriteString(ShaderCompilerConnection& conn, const core::string& v);
    void WriteInt(ShaderCompilerConnection& conn, int v);
    void WriteUInt32(ShaderCompilerConnection& conn, UInt32 v);
    void WriteBool(ShaderCompilerConnection& conn, bool v);

    void Initialize();
    void Cleanup();
} // namespace extprocess
