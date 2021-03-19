#include "UnityPrefix.h"
#include "CgBatch.h"
#include "Runtime/Utilities/PathNameUtility.h"
#include "ShaderCompiler.h"
#include "ShaderCompilerUtility.h"
#include "Runtime/Network/SocketStreams.h"
#include "Runtime/Utilities/Argv.h"
#include "Runtime/Utilities/Word.h"
#include "Interface/ExternalProcessConnection.h"
#include "Interface/ExternalProcessLog.h"
#include "Utilities/ProcessIncludes.h"
#include "APIPlugin.h"
#include <stdexcept>
#if PLATFORM_WIN
#include "PlatformDependent/Win/WinUnicode.h"
#include "Runtime/Network/WinsockInclude.h"
#endif
#if PLATFORM_OSX
#include <mach-o/dyld.h>
#endif
#if PLATFORM_LINUX
#include <unistd.h>
#include <errno.h>
#endif

static extprocess::LogStream* s_Log;

#if PLATFORM_WIN
static LONG WINAPI CustomUnhandledExceptionFilter(EXCEPTION_POINTERS* pExInfo)
{
    if (EXCEPTION_BREAKPOINT == pExInfo->ExceptionRecord->ExceptionCode) // Breakpoint. Don't treat this as a normal crash.
        return EXCEPTION_CONTINUE_SEARCH;

    if (IsDebuggerPresent()) // let the debugger catch this
        return EXCEPTION_CONTINUE_SEARCH;

    if (s_Log)
    {
        s_Log->Fatal() << "\nCrashed!\n";
        s_Log->Flush();
    }

    return EXCEPTION_EXECUTE_HANDLER;
}

#endif // #if PLATFORM_WIN


static void SetupExceptionHandler()
{
#   if PLATFORM_WIN
    // On Windows, setup our own exception handler so that OS does not display "CgBatch has encountered a problem..."
    // dialogs in case we crash.
    ::SetUnhandledExceptionFilter(CustomUnhandledExceptionFilter);
    ::SetErrorMode(SEM_FAILCRITICALERRORS);
#   endif // if PLATFORM_WIN
}

#define ISMULT2(LEN) (((LEN >> 1) << 1) == LEN)
#define CTOI(NIB) (NIB >= 'a' ? (unsigned char)((NIB - 'a') + 10) : (unsigned char)(NIB - '0'))
static void ReadBuffer(extprocess::Connection & proc, dynamic_array<UInt8> & buffer)
{
    core::string data;
    proc.ReadLine(data);
    unsigned int len = (unsigned int)data.size();
    Assert(ISMULT2(len));

    unsigned int buflen = len >> 1;
    buffer.resize_uninitialized(buflen);
    for (unsigned int i = 0; i < buflen; ++i)
    {
        unsigned char value = 0;
        value =   CTOI(data[i * 2 + 0]);
        value |= (CTOI(data[i * 2 + 1]) << 4);
        buffer[i] = value;
    }
}

#undef ISMULT2
#undef CTOI


struct ShaderCompilerPreprocessCallbacksSend : public ShaderCompilerPreprocessCallbacks
{
public:
    ShaderCompilerConnection& m_Conn;
    explicit ShaderCompilerPreprocessCallbacksSend(ShaderCompilerConnection& conn) : m_Conn(conn) {}

    virtual void OnError(CgBatchErrorType type, int platform, int line, const char* fileStr, const char* messageStr)
    {
        if (fileStr == NULL)
            fileStr = "";
        if (type > kCgBatchErrorInfo)
            s_Log->Info() << "  error: " << (int)type << " plat " << platform << " at " << fileStr << "(" << line << "): " << messageStr << extprocess::Endl;
        shadercompiler::WriteLine(m_Conn, Format("err: %i %i %i", type, platform, line));
        shadercompiler::WriteLine(m_Conn, fileStr);
        shadercompiler::WriteLine(m_Conn, messageStr);
    }

    virtual void OnAddSnippet(const int id, const char* snippet, int startLine, UInt32 platformMask, UInt32 hardwareTierVariantsMask, UInt32 typesMask, ShaderSourceLanguage language, UInt32 includesHash[4])
    {
        shadercompiler::WriteLine(m_Conn, Format("snip: %i %i %i %i %i %i %i %i %i %i", id, platformMask, hardwareTierVariantsMask, typesMask, language, includesHash[0], includesHash[1], includesHash[2], includesHash[3], startLine));
        shadercompiler::WriteLine(m_Conn, snippet);
    }

    virtual void OnIncludeDependencies(const std::vector<core::string> &includedFiles)
    {
        shadercompiler::WriteLine(m_Conn, Format("includes: %i", includedFiles.size()));
        std::vector<core::string>::const_iterator it;
        for (it = includedFiles.begin(); it != includedFiles.end(); it++)
        {
            shadercompiler::WriteLine(m_Conn, *it);
        }
    }

    virtual void OnUserKeywordVariants(const int snippetID, const ShaderCompilerProgram programType, const std::vector<std::vector<core::string> >& keywords)
    {
        shadercompiler::WriteLine(m_Conn, Format("keywordsUser: %i %i %i", snippetID, programType, (int)keywords.size()));
        for (size_t i = 0; i < keywords.size(); ++i)
        {
            const std::vector<core::string>& kw = keywords[i];
            core::string msg;
            for (size_t j = 0; j < kw.size(); ++j)
            {
                if (!msg.empty())
                    msg += ' ';
                msg += kw[j];
            }
            shadercompiler::WriteLine(m_Conn, msg);
        }
    }

    virtual void OnBuiltinKeywordVariants(const int snippetID, const ShaderCompilerProgram programType, const std::vector<std::vector<core::string> >& keywords)
    {
        shadercompiler::WriteLine(m_Conn, Format("keywordsBuiltin: %i %i %i", snippetID, programType, (int)keywords.size()));
        for (size_t i = 0; i < keywords.size(); ++i)
        {
            const std::vector<core::string>& kw = keywords[i];
            core::string msg;
            for (size_t j = 0; j < kw.size(); ++j)
            {
                if (!msg.empty())
                    msg += ' ';
                msg += kw[j];
            }
            shadercompiler::WriteLine(m_Conn, msg);
        }
    }

    virtual void OnEndKeywordCombinations(const int snippetID, const std::vector<core::string>& alwaysIncludedUserKeywords, const std::vector<core::string>& builtinKeywords, const ShaderCompileRequirements& targetInfo)
    {
        shadercompiler::WriteLine(m_Conn, Format("keywordsEnd: %i", snippetID));
        // multi_compile FOO BAR
        core::string msg;
        for (size_t i = 0; i < alwaysIncludedUserKeywords.size(); ++i)
        {
            if (!msg.empty())
                msg += ' ';
            msg += alwaysIncludedUserKeywords[i];
        }
        shadercompiler::WriteLine(m_Conn, msg);

        // multi_compile_fwdbase etc.
        msg.clear();
        for (size_t i = 0; i < builtinKeywords.size(); ++i)
        {
            if (!msg.empty())
                msg += ' ';
            msg += builtinKeywords[i];
        }
        shadercompiler::WriteLine(m_Conn, msg);

        // target info
        CompileTimeAssert(sizeof(targetInfo.m_BaseRequirements) == 4, "ShaderCompileRequirements serialization needs to be updated if size is not a UInt32");
        shadercompiler::WriteUInt32(m_Conn, targetInfo.m_BaseRequirements);
        shadercompiler::WriteInt(m_Conn, targetInfo.m_KeywordTargetInfo.size());
        for (size_t i = 0, n = targetInfo.m_KeywordTargetInfo.size(); i != n; ++i)
        {
            const ShaderCompileRequirements::KeywordTargetInfo& info = targetInfo.m_KeywordTargetInfo[i];
            shadercompiler::WriteString(m_Conn, info.keywordName);
            shadercompiler::WriteUInt32(m_Conn, info.requirements);
        }
    }
};

struct ShaderCompilerReflectionReportSend : public ShaderCompilerReflectionReport
{
    ShaderCompilerConnection& m_Conn;
    explicit ShaderCompilerReflectionReportSend(ShaderCompilerConnection& conn) : m_Conn(conn) {}

    virtual void OnInputBinding(ShaderChannel dataChannel, VertexComponent inputSlot)
    {
        shadercompiler::WriteLine(m_Conn, Format("input: %i %i", dataChannel, inputSlot));
    }

    virtual void OnConstantBuffer(const char* name, int size, int varCount)
    {
        shadercompiler::WriteLine(m_Conn, Format("cb: %s %i %i", name, size, varCount));
    }

    virtual void OnConstant(const char* name, int bindIndex, ShaderParamType dataType, ConstantType constantType, int rows, int cols, int arraySize)
    {
        shadercompiler::WriteLine(m_Conn, Format("const: %s %i %i %i %i %i %i", name, bindIndex, dataType, constantType, rows, cols, arraySize));
    }

    virtual void OnCBBinding(const char* name, int bindIndex)
    {
        shadercompiler::WriteLine(m_Conn, Format("cbbind: %s %i", name, bindIndex));
    }

    virtual void OnTextureBinding(const char* name, int bindIndex, int samplerIndex, bool multisampled, TextureDimension dim)
    {
        shadercompiler::WriteLine(m_Conn, Format("texbind: %s %i %i %i %i", name, bindIndex, samplerIndex, multisampled ? 1 : 0, dim));
    }

    virtual void OnInlineSampler(InlineSamplerType sampler, int bindIndex)
    {
        shadercompiler::WriteLine(m_Conn, Format("sampler: %i %i", sampler, bindIndex));
    }

    virtual void OnUAVBinding(const char* name, int bindIndex, int origIndex)
    {
        shadercompiler::WriteLine(m_Conn, Format("uavbind: %s %i %i", name, bindIndex, origIndex));
    }

    virtual void OnBufferBinding(const char* name, int bindIndex)
    {
        shadercompiler::WriteLine(m_Conn, Format("bufferbind: %s %i", name, bindIndex));
    }

    virtual void OnStatsInfo(int alu, int tex, int flow, int tempRegister)
    {
        shadercompiler::WriteLine(m_Conn, Format("stats: %i %i %i %i", alu, tex, flow, tempRegister));
    }
};

static void ShaderCompilerErrorReportSend(
    void* userData,
    CgBatchErrorType type,
    int platform,
    int line,
    const char* fileStr,
    const char* messageStr
)
{
    extprocess::LogStream &log = *s_Log;
    ShaderCompilerConnection* conn = reinterpret_cast<ShaderCompilerConnection*>(userData);

    if (fileStr == NULL)
        fileStr = "";
    if (type > kCgBatchErrorInfo)
        log.Info() << "  error: " << (int)type << " plat " << platform << " at " << fileStr << "(" << line << "): " << messageStr << extprocess::Endl;
    shadercompiler::WriteLine(*conn, Format("err: %i %i %i", type, platform, line));
    shadercompiler::WriteLine(*conn, fileStr);
    shadercompiler::WriteLine(*conn, messageStr);
}

static void FillKeywordList(ShaderCompilerConnection& conn, std::vector<core::string>& outKeywords, core::string& allKeywords, int count)
{
    for (int i = 0, n = count; i < n; ++i)
    {
        core::string kw = shadercompiler::ReadString(conn);
        outKeywords.push_back(kw);
        if (!allKeywords.empty())
            allKeywords += ' ';
        allKeywords += kw;
    }
}

static bool DispatchCommand(ShaderCompilerConnection& conn, const core::string& cmd)
{
    extprocess::LogStream &log = *s_Log;

    if (cmd == "shutdown")
    {
        return false;
    }
    if (cmd == "preprocess")
    {
        log.Info() << "Cmd: preprocess" << extprocess::Endl;
        core::string inputShader, inputPath;
        shadercompiler::ReadLine(conn, inputShader);
        shadercompiler::ReadLine(conn, inputPath);
        bool surfaceOnly = shadercompiler::ReadBool(conn);
        core::string outputShader;
        ShaderCompilerPreprocessCallbacksSend cbs(conn);
        bool hadSurfaceShaders = false;
        bool ok = CgBatchPreprocessShader(inputShader, inputPath, outputShader, &cbs, surfaceOnly, &hadSurfaceShaders);
        shadercompiler::WriteLine(conn, core::string("shader: ") + (ok ? "1" : "0") + " " + (hadSurfaceShaders ? "1" : "0"));
        shadercompiler::WriteLine(conn, outputShader);
        log.Info() << "  insize=" << inputShader.size() << " outsize=" << outputShader.size() << " ok=" << ok << extprocess::Endl;
        return true;
    }
    if (cmd == "disassembleShader")
    {
        log.Info() << "Cmd: disassembleShader" << extprocess::Endl;

        ShaderCompilerPlatform platform = (ShaderCompilerPlatform)shadercompiler::ReadInt(conn);
        ShaderGpuProgramType programType = (ShaderGpuProgramType)shadercompiler::ReadInt(conn);
        UInt32 programMask = shadercompiler::ReadInt(conn);

        dynamic_array<UInt8> inputShader;
        shadercompiler::ReadBuffer(conn, inputShader);

        core::string outputDisassembly;
        bool ok = CgBatchDisassembleShader(platform, programType, programMask, inputShader, outputDisassembly, ShaderCompilerErrorReportSend, &conn);
        shadercompiler::WriteLine(conn, core::string("disasm: ") + (ok ? "1" : "0"));
        shadercompiler::WriteLine(conn, outputDisassembly);
        log.Info() << "  api=" << platform << " insize=" << inputShader.size() << " outsize=" << outputDisassembly.size() << " ok=" << ok << extprocess::Endl;
        return true;
    }
    if (cmd == "compileSnippet")
    {
        log.Info() << "Cmd: compileSnippet" << extprocess::Endl;
        core::string inputCode    = shadercompiler::ReadString(conn);
        core::string inputPath    = shadercompiler::ReadString(conn);

        const int platformKeywordCount = shadercompiler::ReadInt(conn);
        const int userKeywordCount = shadercompiler::ReadInt(conn);

        std::vector<core::string> keywords;
        keywords.reserve(userKeywordCount + platformKeywordCount);
        core::string allUserKeywords, allPlatformKeywords;

        FillKeywordList(conn, keywords, allPlatformKeywords, platformKeywordCount);
        FillKeywordList(conn, keywords, allUserKeywords, userKeywordCount);

        const CompileSnippetOptions options = (CompileSnippetOptions)shadercompiler::ReadUInt32(conn);
        const ShaderSourceLanguage language = (ShaderSourceLanguage)shadercompiler::ReadInt(conn);
        const ShaderCompilerProgram programType = (ShaderCompilerProgram)shadercompiler::ReadInt(conn);
        const ShaderCompilerPlatform platform = (ShaderCompilerPlatform)shadercompiler::ReadInt(conn);
        const ShaderRequirements requirements = (ShaderRequirements)shadercompiler::ReadUInt32(conn);
        UInt32 programMask = shadercompiler::ReadInt(conn);
        int startLine = shadercompiler::ReadInt(conn);
        tReturnedVector outputCode;
        ShaderGpuProgramType gpuProgramType;
        ShaderCompilerReflectionReportSend reflCB(conn);
        bool ok = CgBatchCompileSnippet(inputCode, startLine, inputPath, keywords.empty() ? NULL : &keywords[0], platformKeywordCount, userKeywordCount, options, outputCode, gpuProgramType, language, programType, programMask, platform, requirements, &reflCB, ShaderCompilerErrorReportSend, &conn);
        shadercompiler::WriteLine(conn, core::string("shader: ") + (ok ? "1" : "0"));
        shadercompiler::WriteInt(conn, gpuProgramType);
        shadercompiler::WriteBuffer(conn, &outputCode[0], outputCode.size());
        log.Info() << "  api=" << platform << " type=" << programType << " insize=" << inputCode.size() << " outsize=" << outputCode.size() << " kw=" << allUserKeywords << " pd=" << allPlatformKeywords << " ok=" << ok << extprocess::Endl;
        return true;
    }
    if (cmd == "initializeCompiler")
    {
        log.Info() << "Cmd: initializeCompiler" << extprocess::Endl;

        IncludeSearchContext includeContext;

        // folders with shader include file locations
        int includeFolderCount = shadercompiler::ReadInt(conn);
        includeContext.includePaths.resize_initialized(includeFolderCount);
        for (int i = 0; i < includeFolderCount; ++i)
            includeContext.includePaths[i] = shadercompiler::ReadString(conn);

        // Redirected paths that might contain shader includes
        int redirectedFolderCount = shadercompiler::ReadInt(conn);
        for (int i = 0; i < redirectedFolderCount; ++i)
        {
            core::string key = shadercompiler::ReadString(conn);
            core::string val = shadercompiler::ReadString(conn);
            includeContext.redirectedFolders.insert(key, val);
        }

        core::string additionalShaderCompilerConfiguration = shadercompiler::ReadString(conn);  // s_AdditionalShaderCompilerConfiguration
        CgBatchConfigureSystemFromJson(additionalShaderCompilerConfiguration);

        CgBatchSetIncludeSearchContext(includeContext);

        UInt32 platforms = ~0;
        for (int i = 0; i < kShaderCompPlatformCount; ++i)
        {
            if (!gPluginDispatcher->HasCompilerForAPI((ShaderCompilerPlatform)i))
            {
                platforms &= ~(1 << i); // disable the platform bit
            }
            else
            {
                gPluginDispatcher->InitializeCompiler((ShaderCompilerPlatform)i);    // allow per-platform initialization, also supports re-initialization with new parameters
            }
        }
        shadercompiler::WriteInt(conn, platforms);
        for (int i = 0; i < kShaderCompPlatformCount; ++i)
        {
            ShaderRequirements features = GetCompilePlatformSupportedFeatures((ShaderCompilerPlatform)i);
            shadercompiler::WriteUInt32(conn, features);
            int shaderCompilerVersion = gPluginDispatcher->GetShaderCompilerVersion((ShaderCompilerPlatform)i);
            shadercompiler::WriteInt(conn, shaderCompilerVersion);  // by sending back to the editor the platforms shader compiler version, we can trigger a rebuild of shader after an sdk update
        }
        return true;
    }
    if (cmd == "preprocessCompute")
    {
        log.Info() << "Cmd: preprocessCompute" << extprocess::Endl;
        core::string shaderCode, inputPath;
        shadercompiler::ReadLine(conn, shaderCode);
        shadercompiler::ReadLine(conn, inputPath);
        size_t inputSize = shaderCode.size();

        dynamic_array<core::string> kernelNames;
        dynamic_array<MacroArray> macros;
        ShaderCompilationFlags compileFlags;
        UInt32 includesHash[4];
        std::vector<core::string> includeFiles;
        UInt32 supportedAPIs = 0;

        bool ok = CgBatchPreprocessComputeShader(shaderCode,
            inputPath,
            kernelNames,
            macros,
            compileFlags,
            supportedAPIs,
            includesHash,
            includeFiles,
            ShaderCompilerErrorReportSend,
            &conn);

        for (int i = 0; i < kernelNames.size(); i++)
        {
            shadercompiler::WriteLine(conn, Format("kernel: %s %i", kernelNames[i].c_str(), macros[i].size()));
            for (MacroArray::iterator it = macros[i].begin(); it != macros[i].end(); ++it)
            {
                shadercompiler::WriteLine(conn, it->first);

                if (it->second.empty())
                    shadercompiler::WriteLine(conn, "1");
                else
                    shadercompiler::WriteLine(conn, it->second);
            }
        }
        shadercompiler::WriteLine(conn, Format("endKernels: %i %i %i %i %i %i", compileFlags, includesHash[0], includesHash[1], includesHash[2], includesHash[3], includeFiles.size()));
        for (std::vector<core::string>::iterator it = includeFiles.begin(); it != includeFiles.end(); ++it)
        {
            shadercompiler::WriteLine(conn, *it);
        }
        shadercompiler::WriteLine(conn, shaderCode);
        shadercompiler::WriteInt(conn, supportedAPIs);

        log.Info() << "  insize=" << inputSize << " outsize=" << shaderCode.size() << " ok=" << ok << extprocess::Endl;
        return true;
    }
    if (cmd == "compileComputeKernel")
    {
        log.Info() << "Cmd: compileComputeKernel" << extprocess::Endl;
        core::string inputCode   = shadercompiler::ReadString(conn);
        core::string inputPath   = shadercompiler::ReadString(conn);
        core::string kernelName = shadercompiler::ReadString(conn);

        const int macroCount = shadercompiler::ReadInt(conn);
        dynamic_array<std::pair<core::string, core::string> > macros(kMemTempAlloc);
        macros.reserve(macroCount);

        for (int i = 0; i < macroCount; i++)
        {
            std::pair<core::string, core::string> m;
            m.first = shadercompiler::ReadString(conn);
            m.second = shadercompiler::ReadString(conn);
            macros.push_back(m);
        }

        ShaderCompilerPlatform platform = (ShaderCompilerPlatform)shadercompiler::ReadInt(conn);
        ShaderCompilationFlags compileFlags = (ShaderCompilationFlags)shadercompiler::ReadUInt32(conn);
        tReturnedVector outputBuffer;
        bool ok = CgBatchCompileComputeKernel(
            inputCode,
            inputPath,
            kernelName,
            macros,
            outputBuffer,
            platform,
            compileFlags,
            ShaderCompilerErrorReportSend,
            &conn);
        shadercompiler::WriteLine(conn, core::string("computeData: ") + (ok ? "1" : "0"));
        shadercompiler::WriteBuffer(conn, &outputBuffer[0], outputBuffer.size());
        log.Info() << "  api=" << platform << " insize=" << inputCode.size() << " outsize=" << outputBuffer.size() << " ok=" << ok << extprocess::Endl;
        return true;
    }
    if (cmd == "translateComputeKernel")
    {
        log.Info() << "Cmd: translateComputeKernel" << extprocess::Endl;
        dynamic_array<UInt8> inputBuffer(kMemTempAlloc);
        dynamic_array<UInt8> outputBuffer(kMemTempAlloc);
        ShaderCompilerPlatform platform = (ShaderCompilerPlatform)shadercompiler::ReadInt(conn);
        ShaderCompilationFlags compileFlags = (ShaderCompilationFlags)shadercompiler::ReadUInt32(conn);
        shadercompiler::ReadBuffer(conn, inputBuffer);

        bool ok = CgBatchTranslateComputeKernel(
            platform,
            compileFlags,
            inputBuffer,
            outputBuffer,
            ShaderCompilerErrorReportSend,
            &conn);
        shadercompiler::WriteLine(conn, core::string("computeData: ") + (ok ? "1" : "0"));
        shadercompiler::WriteBuffer(conn, &outputBuffer[0], outputBuffer.size());
        log.Info() << "  api=" << platform << " insize=" << inputBuffer.size() << " outsize=" << outputBuffer.size() << " ok=" << ok << extprocess::Endl;
        return true;
    }
    if (cmd == "clearIncludesCache")
    {
        CgBatchClearIncludesCache();
        return true;
    }

    log.Notice() << "Unknown command: " << cmd << extprocess::Endl;
    throw std::runtime_error("Cannot dispatch invalid command");
}

static int MainLoop(const core::string& basePath, int port, const core::string& modulesLocation)
{
    TSocketHandle sockHandle = Socket::Connect("127.0.0.1", port, 1000);

    // Try for up to 15 seconds to connect to the editor
    int connectTimeout = 0;
    for (; sockHandle == kInvalidSocketHandle && connectTimeout < 15; connectTimeout++)
    {
        sockHandle = Socket::Connect("127.0.0.1", port, 1000);
    }

    if (sockHandle == kInvalidSocketHandle)
    {
        s_Log->Fatal() << "Couldn't connect to port " << port << "\n";
        s_Log->Flush();
        return 1;
    }

    ShaderCompilerConnection conn;
    conn.sock = new SocketStream(sockHandle, true);

    // set no timeouts
    conn.readTimeoutMS = 0;
    conn.writeTimeoutMS = 0;

    if (!conn.sock->IsConnected())
    {
        s_Log->Fatal() << "Couldn't create connection on " << port << "\n";
        s_Log->Flush();
        delete conn.sock;
        return 1;
    }

    extprocess::LogStream &log = *s_Log;

    int retVal = 1;

    try
    {
        log.Info() << "Base path: " << basePath << extprocess::Endl;
        core::string initMsg = CgBatchInitialize(basePath, modulesLocation);
        if (!initMsg.empty())
        {
            log.Fatal() << "Initialization error: " << initMsg << extprocess::Endl;
            retVal = 1;
        }
        else
        {
            core::string cmd;

            for (;;)
            {
                bool cmdOk = shadercompiler::ReadCommand(conn, cmd);

                if (!cmdOk)
                {
                    log.Fatal() << "Error reading command from socket" << extprocess::Endl;
                    retVal = 1; // error
                    break;
                }

                if (cmd.empty())
                {
                    log.Info() << "Socket closed" << extprocess::Endl;
                    retVal = 0; // ok, clean shutdown
                    break;
                }

                if (!DispatchCommand(conn, cmd))
                {
                    retVal = 0; // ok
                    break;
                }
            }
        }
    }
    catch (shadercompiler::SocketException& e)
    {
        log.Fatal() << "Unhandled exception: " << e.what() << extprocess::Endl;
    }

    conn.sock->Shutdown();
    delete conn.sock;

    return retVal;
}

static void RunNativeTestsIfNeededAndExit()
{
#   if ENABLE_UNIT_TESTS
    if (HasARGV("runNativeTests"))
    {
        // We need folder where the executable is placed; for finding
        // standard includes relative to it.
        core::string shaderCompilerPath;
        #if PLATFORM_WIN
        HMODULE moduleHandle = GetModuleHandle(NULL);
        wchar_t pathBuffer[MAX_PATH + 2];
        GetModuleFileNameW(moduleHandle, pathBuffer, MAX_PATH + 1);
        shaderCompilerPath = WideToUtf8(pathBuffer);
        ConvertSeparatorsToUnity(shaderCompilerPath);
        #elif PLATFORM_OSX
        char fn[PATH_MAX + 1];
        fn[0] = 0;
        uint32_t pathlen = PATH_MAX;
        _NSGetExecutablePath(fn, &pathlen);
        shaderCompilerPath = fn;
        #elif PLATFORM_LINUX
        char buf[PATH_MAX + 1];
        ssize_t length = readlink("/proc/self/exe", buf, PATH_MAX);
        if (0 >= length)
        {
            printf_console("Error getting executable path: %s\n", strerror(errno));
            exit(1);
        }
        buf[length + 1] = '\0';
        shaderCompilerPath = buf;
        #else
            #error Unknown platform, implement getting executable path
        #endif
        const std::vector<core::string> testFilters = GetValuesForARGV("runNativeTests");
        const core::string includesPath = AppendPathName(DeleteLastPathNameComponent(shaderCompilerPath), "..");
        int failures = RunCgBatchUnitTests(includesPath.c_str(), testFilters) + RunPluginUnitTests(includesPath.c_str());

#       if WINDOWS
        // Attempt to avoid crashes when exiting on the build agents
        TerminateProcess(GetCurrentProcess(), failures);
#       endif
        exit(failures);
    }
#   endif // if ENABLE_UNIT_TESTS
}

struct ShaderCompilerPreprocessCallbacksCmdline : public ShaderCompilerPreprocessCallbacks
{
public:
    core::string m_Result;

    virtual void OnError(CgBatchErrorType type, int platform, int line, const char* fileStr, const char* messageStr)
    {
        if (fileStr == NULL)
            fileStr = "";
        core::string msg = Format("*** %s: type=%i plat=%i at %s:%i : %s\n",
            type == kCgBatchErrorInfo ? "Info" : "Error",
            type, platform, fileStr, line, messageStr);
        m_Result += msg;
        printf_console("%s", msg.c_str());
    }

    virtual void OnAddSnippet(const int id, const char* snippet, int startLine, UInt32 platformMask, UInt32 hardwareTierVariantsMask, UInt32 typesMask, ShaderSourceLanguage language, UInt32 includesHash[4])
    {
        m_Result += Format("*** Snippet: id=%i platforms=%x qualityVariants=%x types=%x language=%i hash=%08x %08x %08x %08x startLine=%i\n",
            id, platformMask, hardwareTierVariantsMask, typesMask, language, includesHash[0], includesHash[1], includesHash[2], includesHash[3], startLine);
        m_Result += "\n";
        m_Result += snippet;
        m_Result += "\n\n";
    }

    virtual void OnIncludeDependencies(const std::vector<core::string> &includedFiles)
    {
        m_Result += "Includes: ";
        std::vector<core::string>::const_iterator it;
        for (it = includedFiles.begin(); it != includedFiles.end(); it++)
        {
            m_Result += *it;
            m_Result += " ";
        }
        m_Result += "\n\n";
    }

    virtual void OnUserKeywordVariants(const int snippetID, const ShaderCompilerProgram programType, const std::vector<std::vector<core::string> >& keywords)
    {
        m_Result += Format("*** User variants: program=%i id=%i\n", programType, snippetID);
        for (size_t i = 0; i < keywords.size(); ++i)
        {
            for (size_t j = 0; j < keywords[i].size(); ++j)
            {
                m_Result += ' ';
                m_Result += keywords[i][j];
            }
            m_Result += '\n';
        }
    }

    virtual void OnBuiltinKeywordVariants(const int snippetID, const ShaderCompilerProgram programType, const std::vector<std::vector<core::string> >& keywords)
    {
        m_Result += Format("*** Builtin variants: program=%i id=%i\n", programType, snippetID);
        for (size_t i = 0; i < keywords.size(); ++i)
        {
            for (size_t j = 0; j < keywords[i].size(); ++j)
            {
                m_Result += ' ';
                m_Result += keywords[i][j];
            }
            m_Result += '\n';
        }
    }

    virtual void OnEndKeywordCombinations(const int snippetID, const std::vector<core::string>& alwaysIncludedUserKeywords, const std::vector<core::string>& builtinKeywords, const ShaderCompileRequirements& targetInfo)
    {
    }
};


// This isn't used by Unity, but helps to debug & investigate performance of surface shader generation
// while developing - just run from command line using:
//
// UnityShaderCompiler -preprocess <shader file> <includes path> <output file>
static void RunShaderPreprocessIfNeededAndExit()
{
    if (GetArgc() != 5)
        return;
    if (!HasARGV("preprocess"))
        return;

    const char** argv = GetArgv();
    printf_console("Preprocessing shader: %s path %s output %s\n", argv[2], argv[3], argv[4]);
    core::string inputShader;
    if (!ReadTextFile(argv[2], inputShader))
    {
        printf_console("Failed to read input file!\n");
        exit(1);
    }

    ShaderCompilerPreprocessCallbacksCmdline callbacks;
    bool hadSurface = false;
    core::string outputShader;
    core::string inputFolder = DeleteLastPathNameComponent(argv[2]);
    IncludeSearchContext includeContext;
    includeContext.includePaths.push_back(argv[3]);
    CgBatchSetIncludeSearchContext(includeContext);
    bool ok = CgBatchPreprocessShader(inputShader, inputFolder, outputShader, &callbacks, false, &hadSurface);

    outputShader += "\n\n" + callbacks.m_Result;
    if (!WriteTextToFile(argv[4], outputShader.c_str()))
    {
        printf_console("Failed to write output file!\n");
        exit(1);
    }

    if (!ok)
        printf_console("Failed preprocessing shader!\n");

    exit(ok ? 0 : 1);
}

#if PLATFORM_WIN
int wmain(int argc, wchar_t* argvWide[], wchar_t* envp[])
#else
int main(int argc, char* const argv[])
#endif
{
    std::ios::sync_with_stdio(false);

    // Convert command line arguments to UTF8
#   if PLATFORM_WIN
    char** argv = new char*[argc];
    for (int i = 0; i < argc; ++i)
    {
        core::string a = WideToUtf8(argvWide[i]);
        argv[i] = new char[a.size() + 1];
        memcpy(argv[i], a.c_str(), a.size() + 1);
    }
#   endif // if PLATFORM_WIN

    SetupArgv(argc, (const char**)argv);
    SetupExceptionHandler();

    RunNativeTestsIfNeededAndExit();
    RunShaderPreprocessIfNeededAndExit();

    if (argc < 4)
    {
        printf_console("Usage: UnityShaderCompiler <base folder> <log path> <port number> [modulePath] \n");
        // Cleanup UTF8 arguments
#   if PLATFORM_WIN
        for (int i = 0; i < argc; ++i)
            delete[] argv[i];
        delete[] argv;
#   endif // if PLATFORM_WIN
        return 1;
    }
    core::string basePath = argv[1];
    core::string logPath  = argv[2];
    int port = StringToInt((char const*)argv[3]);
    core::string modulesLocation = "";
    if (argc > 4)
    {
        modulesLocation = argv[4];
    }

    s_Log = new extprocess::LogStream(logPath, extprocess::LOG_INFO);

#   if PLATFORM_WIN
    WSADATA WsaData;
    int err = WSAStartup(MAKEWORD(2, 2), &WsaData);
    if (err != NO_ERROR)
    {
        printf_console("Network initialization failed; error={%d}", err);
        // Cleanup UTF8 arguments
#   if PLATFORM_WIN
        for (int i = 0; i < argc; ++i)
            delete[] argv[i];
        delete[] argv;
#   endif // if PLATFORM_WIN
        return 1;
    }
#   endif

    int res = MainLoop(basePath, port, modulesLocation);

#   if PLATFORM_WIN
    WSACleanup();
#   endif

    s_Log->Info() << "\nQuitting shader compiler process\n";
    CgBatchShutdown();

    delete s_Log;
    s_Log = NULL;

    // Cleanup UTF8 arguments
#   if PLATFORM_WIN
    for (int i = 0; i < argc; ++i)
        delete[] argv[i];
    delete[] argv;
#   endif // if PLATFORM_WIN

    return res;
}
