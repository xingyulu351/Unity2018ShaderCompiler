#include "Projects/PrecompiledHeaders/UnityPrefix.h"
#include "ShaderCompilerClient.h"
#include "ShaderCompilerUtility.h"
#include "ShaderCompilerPlatformCaps.h"
#include "Editor/Platform/Interface/ExternalProcess.h"
#include "Editor/Src/AssetPipeline/ShaderImporting/ShaderCompileRequirements.h"
#include "Editor/Src/AssetPipeline/ShaderImporting/ShaderVariantEnumeration.h"
#include "Editor/Src/EditorModules.h"
#include "Editor/Src/BuildPipeline/BuildTargetPlatformSpecific.h"
#include "Editor/Src/Utility/LocaleHelper.h"
#include "Runtime/Misc/PlayerSettings.h"
#include "Runtime/Network/SocketStreams.h"
#include "Runtime/Network/ServerSocket.h"
#include "Runtime/Shaders/ShaderKeywords.h"
#include "Runtime/Utilities/FileUtilities.h"
#include "Runtime/Utilities/ArrayUtility.h"
#include "Runtime/Threads/AtomicOps.h"
#include "Runtime/Threads/Mutex.h"
#include "Runtime/Threads/Thread.h"
#include "Runtime/Threads/ThreadSpecificValue.h"
#include "Runtime/Serialize/BuildTargetVerification.h"
#include "Runtime/Graphics/ScriptableRenderLoop/ScriptableRenderContext.h"


#if PLATFORM_WIN
const char* kShaderCompilerPath = "Tools/UnityShaderCompiler.exe";
#elif PLATFORM_OSX || PLATFORM_LINUX
const char* kShaderCompilerPath = "Tools/UnityShaderCompiler";
#else
#error "Unknown platform"
#endif

// compiling shaders really can take a long time on some platforms
const UInt32 kDefaultShaderPreprocessTimeoutMS = 1000 * 60 * 10; // 10 minutes
const UInt32 kDefaultShaderCompileTimeoutMS = 1000 * 60 * 10; // 10 minutes
const UInt32 kDefaultShaderDisassembleTimeoutMS = 1000 * 60 * 1; // 1 minute

static UInt32 s_ShaderPreprocessTimeoutMS;
static UInt32 s_ShaderCompileTimeoutMS;
static UInt32 s_ShaderDisassembleTimeoutMS;

// By default do not retry on timeout. Use UNITY_SHADER_COMPILER_RETRY_COUNT env var to override
const UInt32 kDefaultTimeoutRetries = 0;

const ShaderCompilerProgram kShaderTypeToCompilerType[] = // Expected size is kShaderTypeCount
{
    kProgramCount, // none
    kProgramVertex,
    kProgramFragment,
    kProgramGeometry,
    kProgramHull,
    kProgramDomain,
};
CompileTimeAssertArraySize(kShaderTypeToCompilerType, kShaderTypeCount);

struct ShaderCompilerPerThread
{
    UInt32                      availableCompilerPlatforms;
    ShaderRequirements          compilerPlatformSupportedFeatures[kShaderCompPlatformCount];
    UInt32                      compilerShaderCompilerVersions[kShaderCompPlatformCount];
    ShaderCompilerConnection    connection;
};

static ServerSocket* s_ListenSocket = NULL;
static UNITY_TLS_VALUE(ShaderCompilerPerThread*) s_ShaderCompilerPerThread;

static Mutex                            s_AllShaderCompilersMutex;
dynamic_array<ShaderCompilerPerThread*> s_AllShaderCompilers;

static IncludeSearchContext s_ShaderIncludeContext;

static core::string s_AdditionalShaderCompilerConfiguration = "";

void SetAdditionalShaderCompilerConfiguration(const core::string &configstring)
{
    s_AdditionalShaderCompilerConfiguration = configstring; // see "initializeCompiler" in DispatchCommand() in ShaderCompilerProcess.cpp to see processing
}

static bool Is64BitOS();
static void ShaderCompilerSendShutdownMessage(ShaderCompilerConnection* p);
static void ShaderCompilerWaitForExitAndDestroy(ShaderCompilerConnection* p);
// returns bitmask of available shader compiler platforms
static UInt32 ShaderCompilerProcessInitialize(ShaderCompilerConnection& conn, ShaderRequirements outPlatformFeatures[kShaderCompPlatformCount], UInt32 outPlatformShaderCompilerVersion[kShaderCompPlatformCount]);

static UInt32 GetNumRetries()
{
    UInt32 result = kDefaultTimeoutRetries;

    char *envstring = getenv("UNITY_SHADER_COMPILER_RETRY_COUNT");
    if (envstring != NULL)
        result = atoi(envstring);

    // Clamp retries to max 10 times
    result = (result < 10) ? result : 10;

    return result;
}

static void InitTaskTimeouts()
{
    char *envstring = getenv("UNITY_SHADER_COMPILER_TASK_TIMEOUT_MINUTES");
    if (envstring != NULL)
    {
        UInt32 timeout = atoi(envstring) * 60 * 1000;
        if (timeout > 0)
        {
            s_ShaderPreprocessTimeoutMS = timeout;
            s_ShaderCompileTimeoutMS = timeout;
            s_ShaderDisassembleTimeoutMS = timeout;
            return;
        }
    }

    // Use defaults if no env var found or if set to zero
    s_ShaderPreprocessTimeoutMS = kDefaultShaderPreprocessTimeoutMS;
    s_ShaderCompileTimeoutMS = kDefaultShaderCompileTimeoutMS;
    s_ShaderDisassembleTimeoutMS = kDefaultShaderDisassembleTimeoutMS;
}

static UInt32 GetConnectTimeoutMS(UInt32 defaultTimeout)
{
    UInt32 result = defaultTimeout;

    char *envstring = getenv("UNITY_SHADER_COMPILER_CONNECT_TIMEOUT_SECONDS");
    if (envstring != NULL)
        result = atoi(envstring) * 1000;

    return result;
}

static core::string GetShaderCompilerBaseFolder()
{
    return GetApplicationContentsPath();
}

static core::string GetShaderCompilerPath()
{
    return AppendPathName(GetApplicationContentsPath(), kShaderCompilerPath);
}

void InitIncludePaths(const IncludeSearchContext& includeContext)
{
    s_ShaderIncludeContext = includeContext;
    ReinitialiseShaderCompilerProcesses();
}

/*
    Reinitialise all the current shader compiler processes. This can be used if s_AdditionalShaderCompilerConfiguration or s_ShaderIncludeFolders changes
*/
void ReinitialiseShaderCompilerProcesses()
{
    shadercompiler::Initialize();
    Mutex::AutoLock lock(s_AllShaderCompilersMutex);
    for (size_t i = 0; i < s_AllShaderCompilers.size(); i++)
    {
        if (s_AllShaderCompilers[i])
            ShaderCompilerProcessInitialize(s_AllShaderCompilers[i]->connection, s_AllShaderCompilers[i]->compilerPlatformSupportedFeatures, s_AllShaderCompilers[i]->compilerShaderCompilerVersions);
    }
}

void InitShaderCompilerClient()
{
    Assert(s_ListenSocket == NULL);
    s_ListenSocket = UNITY_NEW(ServerSocket, kMemAssetImporter)();
    s_ListenSocket->StartListening("127.0.0.1", 0, false);
}

void CleanupShaderCompilerClient()
{
    Mutex::AutoLock lock(s_AllShaderCompilersMutex);

    // The shutdown process is separated into two loops so that we don't wait for a compiler
    // process to shutdown immediately after sending a shutdown message
    for (size_t i = 0; i < s_AllShaderCompilers.size(); i++)
    {
        ShaderCompilerPerThread* perThread = s_AllShaderCompilers[i];
        ShaderCompilerSendShutdownMessage(&perThread->connection);
    }

    for (size_t i = 0; i < s_AllShaderCompilers.size(); i++)
    {
        ShaderCompilerPerThread* perThread = s_AllShaderCompilers[i];
        ShaderCompilerWaitForExitAndDestroy(&perThread->connection);
        UNITY_DELETE(perThread, kMemAssetImporter);
    }

    s_AllShaderCompilers.clear_dealloc();

    s_ShaderIncludeContext.includePaths.clear_dealloc();
    s_ShaderIncludeContext.redirectedFolders.clear_dealloc();

    shadercompiler::Cleanup();
}

static ShaderCompilerPerThread* GetShaderCompilerPerThreadInternal()
{
    bool didReboot = false;
    ShaderCompilerPerThread* perThread = s_ShaderCompilerPerThread;

    // Figure out if we already have a compiler running for this thread, if so just use that and return
    if (perThread != NULL)
    {
        if (perThread->connection.proc->IsRunning())
            return perThread;

        // Its no longer running, might have crashed.
        // Kill it and below we take the usual codepath and start a new one.

        Mutex::AutoLock lock(s_AllShaderCompilersMutex);

        for (size_t i = 0; i < s_AllShaderCompilers.size(); i++)
        {
            if (s_AllShaderCompilers[i] == perThread)
            {
                s_AllShaderCompilers.erase(s_AllShaderCompilers.begin() + i);
                break;
            }
        }

        didReboot = true;

        UNITY_DELETE(perThread, kMemAssetImporter);
        perThread = NULL;
        s_ShaderCompilerPerThread = NULL;
    }

    core::string compilerPath = GetShaderCompilerPath();
    core::string compName = GetLastPathNameComponent(compilerPath);

    if (didReboot)
        printf_console("Shader compiler: %s compiler executable disappeared on thread %i, restarting\n", compName.c_str(), (int)(size_t)Thread::GetCurrentThreadID());

    // Make sure to give decent error early on instead of failure in external process...
    if (!IsFileCreated(compilerPath))
    {
        ErrorString("Shader compiler can't be found: " + compilerPath);
        return NULL;
    }

    Mutex::AutoLock lock(s_AllShaderCompilersMutex);

    // Always emit numbers as 0.0 and never 0,0: case 991968
    AutoLocale invariantLocale(AutoLocale::kInvariantLocale);

    // Make each compiler process use a different log file, starting with 1 and going up as more are spawned.
    core::string logPath = Format("./Library/shadercompiler-%s%i.log", compName.c_str(), s_AllShaderCompilers.size());

    std::vector<core::string> compilerArgs;
    compilerArgs.push_back(GetShaderCompilerBaseFolder());
    compilerArgs.push_back(logPath);
    compilerArgs.push_back(Format("%i", s_ListenSocket->GetPort()));

    compilerArgs.push_back(GetModulesLocation());
    // Allocation owner will probably be some serialization code up in the stack. But we're creating
    // a long-lived ExternalProcess object here, and don't want to tie our allocations to that.
    CLEAR_ALLOC_OWNER;

    int launchCounter = 0;
_retryLaunch:

    ExternalProcess * proc = UNITY_NEW(ExternalProcess, kMemAssetImporter)(compilerPath, compilerArgs, "");
    if (proc == NULL)
    {
        ErrorString("Failed to launch " + compName + " shader compiler!");
        return NULL;
    }

    if (!proc->Launch())
    {
        UNITY_DELETE(proc, kMemAssetImporter);
        ErrorString("Failed to launch " + compName + " shader compiler! " + compilerPath);
        return NULL;
    }

    TSocketHandle sock = s_ListenSocket->Accept();

    // wait for up to 15 second for the newly spawned process to connect to our
    // listen socket
    const int connectTimeout = GetConnectTimeoutMS(15000) / 10; // sleeping 10ms at time
    int connectTime = 0;
    for (; sock == kInvalidSocketHandle && connectTime < connectTimeout; connectTime++)
    {
        Thread::Sleep(0.01);
        sock = s_ListenSocket->Accept();
    }

    if (sock == kInvalidSocketHandle)
    {
        UNITY_DELETE(proc, kMemAssetImporter);
        ErrorString("Failed to get socket connection from " + compName + " shader compiler! " + compilerPath);
        return NULL;
    }

    printf_console("Launched and connected shader compiler %s after %.2f seconds\n", compName.c_str(), connectTime * 0.01f);

    InitTaskTimeouts();
    perThread = UNITY_NEW(ShaderCompilerPerThread, kMemAssetImporter);
    perThread->connection.proc = proc;
    perThread->connection.sock = UNITY_NEW(SocketStream, kMemAssetImporter)(sock, true);
    perThread->connection.readTimeoutMS = 30 * 1000;
    perThread->connection.writeTimeoutMS = 10 * 1000;
    perThread->connection.timeoutRetryCount = GetNumRetries();

    // Initialize: pass include folders, query available compile platforms and their flags
    perThread->availableCompilerPlatforms = ShaderCompilerProcessInitialize(perThread->connection, perThread->compilerPlatformSupportedFeatures, perThread->compilerShaderCompilerVersions);
    if (perThread->availableCompilerPlatforms == 0)
    {
        // Shader compiler executable failed to start up properly (happens on Macs sometimes, rarely).
        // Reasons not known yet, but for now just retry a few times. Only log an error if fails after
        // several attempts.
        UNITY_DELETE(proc, kMemAssetImporter);
        UNITY_DELETE(perThread, kMemAssetImporter);
        ++launchCounter;
        if (launchCounter > 10)
        {
            ErrorStringMsg("Shader compiler: failed to launch and initialize compiler executable, even after %i retries", launchCounter);
            return NULL;
        }
        goto _retryLaunch;
    }

    s_AllShaderCompilers.push_back(perThread);
    s_ShaderCompilerPerThread = perThread;

    return perThread;
}

ShaderCompilerConnection* GetShaderCompilerPerThread()
{
    ShaderCompilerPerThread* perThread = GetShaderCompilerPerThreadInternal();
    return perThread != NULL ? &perThread->connection : NULL;
}

// Returns true if a retry is required
static bool HandleShaderSocketException(ShaderCompilerConnection*& conn, const shadercompiler::SocketException& ex, UInt32& retryCount)
{
    if (conn->proc)
    {
        // Terminate the shader compiler process now. This prevents receiving data in the future
        // on this socket from this shader compiler process which would be out of sync
        if (retryCount <= 0)
            AssertFormatMsg(false, "Shader Compiler Socket Exception: Terminating shader compiler process, %i retries left", retryCount);
        else
            printf_console("Shader Compiler Socket Exception: Terminating shader compiler process, %i retries left\n", retryCount);
        conn->proc->Shutdown();
    }

    bool needsRetry = false;

    if (retryCount > 0)
    {
        // Try starting a new one and retry with that
        ShaderCompilerConnection* newConn = GetShaderCompilerPerThread();
        if (newConn != NULL)
        {
            conn = newConn;
            needsRetry = true;
            retryCount--;
        }
    }

    return needsRetry;
}

/*
    Returns version of platform shader compiler, used to trigger rebuilding of shaders after sdk change
    Will return 0 if not implemented for this platform.
*/
int GetShaderCompilerVersion(ShaderCompilerPlatform api)
{
    int version = 0;

    Mutex::AutoLock lock(s_AllShaderCompilersMutex);

    for (size_t i = 0; i < s_AllShaderCompilers.size(); i++)
    {
        ShaderCompilerPerThread* perThread = s_AllShaderCompilers[i];
        version = perThread->compilerShaderCompilerVersions[api];   // retreived from UnityShaderCompiler.exe via "initializeCompiler" command
        if (version != 0)
            return version;
    }

    return version;
}

static void AdjustErrorFilePath(core::string& file, const core::string& inputPath)
{
    if (file.empty())
        return;

    if (IsFileCreated(file))
        return;

    core::string testPath = AppendPathName(inputPath, file);
    if (IsFileCreated(testPath))
    {
        testPath = ResolveSymlinks(testPath);
        file = testPath;
    }
}

static void ReadKeywordsLine(ShaderCompilerConnection& conn, std::vector<core::string>& outKeywords)
{
    core::string msgStr;
    shadercompiler::ReadLine(conn, msgStr);
    std::vector<core::string> keywordLine;
    Split(msgStr, ' ', keywordLine);
    outKeywords.resize(keywordLine.size());
    for (size_t i = 0; i < keywordLine.size(); ++i)
    {
        outKeywords[i] = keywordLine[i];
    }
}

bool ShaderCompilerPreprocess(
    ShaderCompilerConnection*& conn,
    const core::string& inputShaderStr,
    const core::string& inputPathStr,
    core::string& outputShader,
    bool& outHadSurfaceShaders,
    ShaderCompilerPreprocessCallbacks* callbacks,
    bool surfaceGenerationOnly
)
{
    UInt32 retryCount = conn->timeoutRetryCount;
    bool needsRetry;
    bool ok;

    do
    {
        needsRetry = false;
        ok = true;

        outputShader.clear();
        outHadSurfaceShaders = false;

        UInt32 origReadTimeout = conn->readTimeoutMS;
        conn->readTimeoutMS = s_ShaderPreprocessTimeoutMS;

        try
        {
            shadercompiler::SendCommand(*conn, kShaderCompilerPreprocess);
            shadercompiler::WriteLine(*conn, inputShaderStr);
            shadercompiler::WriteLine(*conn, inputPathStr);
            shadercompiler::WriteBool(*conn, surfaceGenerationOnly);
            core::string lineStr, msgStr;
            std::vector<core::string> tokens;

            while (ok)
            {
                shadercompiler::ReadLine(*conn, lineStr);
                tokens.clear();
                Split(lineStr, ' ', tokens);
                if (tokens.empty())
                {
                    AssertFormatMsg(false, "Shader compiler: unrecognized message from the compiler process: %s", lineStr.c_str());
                    ok = false;
                    break;
                }

                if (tokens[0] == "err:" && tokens.size() == 4)
                {
                    CgBatchErrorType type = (CgBatchErrorType)StringToInt(tokens[1]);
                    int plat = StringToInt(tokens[2]);
                    int line = StringToInt(tokens[3]);
                    core::string fileStr;
                    shadercompiler::ReadLine(*conn, fileStr);
                    AdjustErrorFilePath(fileStr, inputPathStr);
                    shadercompiler::ReadLine(*conn, msgStr);
                    callbacks->OnError(type, plat, line, fileStr.c_str(), msgStr.c_str());
                }
                else if (tokens[0] == "snip:" && tokens.size() == 11)
                {
                    int id = StringToInt(tokens[1]);
                    int platformsMask = StringToInt(tokens[2]);
                    int hardwareTierVariantsMask = StringToInt(tokens[3]);
                    int typesMask = StringToInt(tokens[4]);
                    ShaderSourceLanguage language = (ShaderSourceLanguage)StringToInt(tokens[5]);
                    UInt32 includeHash[4];
                    includeHash[0] = StringToInt(tokens[6]);
                    includeHash[1] = StringToInt(tokens[7]);
                    includeHash[2] = StringToInt(tokens[8]);
                    includeHash[3] = StringToInt(tokens[9]);
                    int startLine = StringToInt(tokens[10]);
                    shadercompiler::ReadLine(*conn, msgStr);
                    callbacks->OnAddSnippet(id, msgStr.c_str(), startLine, platformsMask, hardwareTierVariantsMask, typesMask, language, includeHash);
                }
                else if (tokens[0] == "includes:" && tokens.size() == 2)
                {
                    int numIncludes = StringToInt(tokens[1]);
                    std::vector<core::string> includedFiles;
                    core::string includeFile;
                    for (int i = 0; i < numIncludes; i++)
                    {
                        shadercompiler::ReadLine(*conn, includeFile);
                        includedFiles.push_back(includeFile);
                    }
                    callbacks->OnIncludeDependencies(includedFiles);
                }
                else if (tokens[0] == "keywordsUser:" && tokens.size() == 4)
                {
                    int id = StringToInt(tokens[1]);
                    ShaderCompilerProgram type = (ShaderCompilerProgram)StringToInt(tokens[2]);
                    int count = StringToInt(tokens[3]);
                    std::vector<std::vector<core::string> > keywords;
                    keywords.reserve(count);
                    for (int i = 0; i < count; ++i)
                    {
                        std::vector<core::string> keywordLine;
                        ReadKeywordsLine(*conn, keywordLine);
                        keywords.push_back(keywordLine);
                    }
                    callbacks->OnUserKeywordVariants(id, type, keywords);
                }
                else if (tokens[0] == "keywordsBuiltin:" && tokens.size() == 4)
                {
                    int id = StringToInt(tokens[1]);
                    ShaderCompilerProgram type = (ShaderCompilerProgram)StringToInt(tokens[2]);
                    int count = StringToInt(tokens[3]);
                    std::vector<std::vector<core::string> > keywords;
                    keywords.reserve(count);
                    for (int i = 0; i < count; ++i)
                    {
                        std::vector<core::string> keywordLine;
                        ReadKeywordsLine(*conn, keywordLine);
                        keywords.push_back(keywordLine);
                    }
                    callbacks->OnBuiltinKeywordVariants(id, type, keywords);
                }
                else if (tokens[0] == "keywordsEnd:" && tokens.size() == 2)
                {
                    ShaderCompileRequirements targetInfo;
                    int id = StringToInt(tokens[1]);
                    // user and built-in keywords
                    std::vector<core::string> keywords1, keywords2;
                    ReadKeywordsLine(*conn, keywords1);
                    ReadKeywordsLine(*conn, keywords2);
                    // target info
                    targetInfo.m_BaseRequirements = (ShaderRequirements)shadercompiler::ReadUInt32(*conn);
                    int kwCount = shadercompiler::ReadInt(*conn);
                    targetInfo.m_KeywordTargetInfo.reserve(kwCount);
                    for (int i = 0; i < kwCount; ++i)
                    {
                        core::string kwName = shadercompiler::ReadString(*conn);
                        ShaderRequirements kwMask = (ShaderRequirements)shadercompiler::ReadUInt32(*conn);
                        CompileTimeAssert(sizeof(kwMask) == 4, "ShaderCompileRequirements serialization needs to be updated if size is not a UInt32");
                        targetInfo.AddKeywordRequirements(kwName, kwMask);
                    }
                    callbacks->OnEndKeywordCombinations(id, keywords1, keywords2, targetInfo);
                }
                else if (tokens[0] == "shader:" && tokens.size() == 3)
                {
                    ok = StringToInt(tokens[1]) != 0;
                    outHadSurfaceShaders = StringToInt(tokens[2]) != 0;
                    shadercompiler::ReadLine(*conn, outputShader);
                    break;
                }
                else
                {
                    AssertFormatMsg(false, "Shader compiler: unrecognized message from the compiler process: %s", lineStr.c_str());
                    ok = false;
                    break;
                }
            }
        }
        catch (shadercompiler::SocketException& ex)
        {
            printf_console("Shader compiler: internal error preprocessing shader: %s", ex.what());
            needsRetry = HandleShaderSocketException(conn, ex, retryCount);
            if (!needsRetry)
            {
                AssertFormatMsg(false, "Shader compiler: internal error preprocessing shader: %s", ex.what());
            }

            ok = false;
        }

        conn->readTimeoutMS = origReadTimeout;
    }
    while (needsRetry);

    return ok;
}

bool ShaderCompilerPreprocessCompute(
    ShaderCompilerConnection *&conn,
    const core::string& inputShaderStr,
    const core::string& inputPathStr,
    core::string& outputShader,
    dynamic_array<core::string>& outKernelNames,
    dynamic_array<MacroArray>& outMacros,
    ShaderCompilationFlags &outCompileFlags,
    UInt32& supportedAPIs,
    UInt32* outIncludeHash,
    std::vector<core::string>& outIncludeFiles,
    CgBatchErrorCallback errorCallback,
    void* errorCallbackData
)
{
    UInt32 retryCount = conn->timeoutRetryCount;
    bool needsRetry;
    bool ok;

    do
    {
        needsRetry = false;
        ok = true;

        UInt32 origReadTimeout = conn->readTimeoutMS;
        conn->readTimeoutMS = s_ShaderCompileTimeoutMS;

        try
        {
            shadercompiler::SendCommand(*conn, kShaderCompilerPreprocessCompute);
            shadercompiler::WriteLine(*conn, inputShaderStr);
            shadercompiler::WriteLine(*conn, inputPathStr);

            core::string lineStr, msgStr;
            std::vector<core::string> tokens;

            while (ok)
            {
                shadercompiler::ReadLine(*conn, lineStr);
                tokens.clear();
                Split(lineStr, ' ', tokens);
                if (tokens.empty())
                {
                    AssertFormatMsg(false, "Compute Shader compiler: unrecognized message from the compiler process: %s", lineStr.c_str());
                    msgStr.assign("Internal error, empty message from the shader compiler process while preprocessing compute shader ");
                    msgStr.append(inputPathStr);
                    errorCallback(errorCallbackData, kCgBatchErrorError, -1, -1, "", msgStr.c_str());
                    ok = false;
                    break;
                }

                if (tokens[0] == "err:" && tokens.size() == 4)
                {
                    CgBatchErrorType type = (CgBatchErrorType)StringToInt(tokens[1]);
                    int plat = StringToInt(tokens[2]);
                    int line = StringToInt(tokens[3]);
                    core::string fileStr;
                    shadercompiler::ReadLine(*conn, fileStr);
                    AdjustErrorFilePath(fileStr, inputPathStr);
                    shadercompiler::ReadLine(*conn, msgStr);
                    errorCallback(errorCallbackData, type, plat, line, fileStr.c_str(), msgStr.c_str());
                }
                else if (tokens[0] == "kernel:" && tokens.size() == 3)
                {
                    const core::string& kernelName = tokens[1];
                    outKernelNames.push_back(kernelName);
                    int numMacros = StringToInt(tokens[2]);
                    MacroArray& macros = outMacros.emplace_back();
                    for (int i = 0; i < numMacros; i++)
                    {
                        core::string macroName, macroValue;
                        shadercompiler::ReadLine(*conn, macroName);
                        shadercompiler::ReadLine(*conn, macroValue);
                        macros.push_back(MacroPair(macroName, macroValue));
                    }
                }
                else if (tokens[0] == "endKernels:" && tokens.size() == 7)
                {
                    outCompileFlags = static_cast<ShaderCompilationFlags>(StringToInt(tokens[1]));
                    outIncludeHash[0] = StringToInt(tokens[2]);
                    outIncludeHash[1] = StringToInt(tokens[3]);
                    outIncludeHash[2] = StringToInt(tokens[4]);
                    outIncludeHash[3] = StringToInt(tokens[5]);
                    int numIncludes = StringToInt(tokens[6]);
                    core::string includeFile;
                    for (int i = 0; i < numIncludes; i++)
                    {
                        shadercompiler::ReadLine(*conn, includeFile);
                        outIncludeFiles.push_back(includeFile);
                    }
                    shadercompiler::ReadLine(*conn, outputShader);
                    supportedAPIs = static_cast<unsigned int>(shadercompiler::ReadInt(*conn));
                    break;
                }
                else
                {
                    printf_console("Compute Shader compiler: unrecognized output while preprocessing shader. Thread=%i shader=%s.\n\n", (int)(size_t)Thread::GetCurrentThreadID(), inputPathStr.c_str());
                    AssertFormatMsg(false, "Compute Shader compiler: unrecognized message from the compiler process: %s", lineStr.c_str());
                    msgStr.assign("Internal error, unrecognized message from the shader compiler process while preprocessing compute shader ");
                    msgStr.append(inputPathStr);
                    errorCallback(errorCallbackData, kCgBatchErrorError, -1, -1, "", msgStr.c_str());
                    ok = false;
                    break;
                }
            }
        }
        catch (shadercompiler::SocketException& ex)
        {
            printf_console("Compute Shader compiler: exception while preprocessing shader. Thread=%i shader=%s.\n\n", (int)(size_t)Thread::GetCurrentThreadID(), inputPathStr.c_str());
            needsRetry = HandleShaderSocketException(conn, ex, retryCount);
            if (!needsRetry)
            {
                AssertFormatMsg(false, "Compute Shader compiler: internal error preprocessing shader: %s", ex.what());
                core::string msgStr("Internal error communicating with the compute shader compiler process while preprocessing compute shader ");
                msgStr.append(inputPathStr);
                errorCallback(errorCallbackData, kCgBatchErrorError, -1, -1, "", msgStr.c_str());
            }
            ok = false;
        }

        conn->readTimeoutMS = origReadTimeout;
    }
    while (needsRetry);

    return ok;
}

bool ShaderCompilerCompileComputeKernel(
    ShaderCompilerConnection *& conn,
    const char* inputCodeStr,
    const char* inputPathStr,
    const char* kernelName,
    const dynamic_array<std::pair<core::string, core::string> >& macros,
    ShaderCompilerPlatform platform,
    ShaderCompilationFlags compileFlags,
    dynamic_array<UInt8> & outputBuffer,
    CgBatchErrorCallback errorCallback,
    void* errorCallbackData
)
{
    UInt32 retryCount = conn->timeoutRetryCount;
    bool needsRetry;
    bool ok;

    do
    {
        needsRetry = false;
        ok = true;

        outputBuffer.clear_dealloc();

        UInt32 origReadTimeout = conn->readTimeoutMS;
        conn->readTimeoutMS = s_ShaderCompileTimeoutMS;

        try
        {
            shadercompiler::SendCommand(*conn, kShaderCompilerCompileComputeKernel);
            shadercompiler::WriteLine(*conn, inputCodeStr);
            shadercompiler::WriteLine(*conn, inputPathStr);
            shadercompiler::WriteLine(*conn, kernelName);

            shadercompiler::WriteInt(*conn, macros.size());
            for (int i = 0, n = macros.size(); i < n; ++i)
            {
                shadercompiler::WriteLine(*conn, macros[i].first);
                if (macros[i].second.empty())
                    shadercompiler::WriteLine(*conn, "1");
                else
                    shadercompiler::WriteLine(*conn, macros[i].second);
            }

            shadercompiler::WriteInt(*conn, platform);
            shadercompiler::WriteUInt32(*conn, (UInt32)compileFlags);

            core::string lineStr, msgStr;
            std::vector<core::string> tokens;

            while (ok)
            {
                shadercompiler::ReadLine(*conn, lineStr);
                tokens.clear();
                Split(lineStr, ' ', tokens);
                if (tokens.empty())
                {
                    AssertFormatMsg(false, "Compute Shader compiler: unrecognized message from the compiler process: %s", lineStr.c_str());
                    msgStr.assign("Internal error, empty message from the shader compiler process while compiling kernel ");
                    msgStr.append(kernelName);
                    errorCallback(errorCallbackData, kCgBatchErrorError, -1, -1, "", msgStr.c_str());
                    ok = false;
                    break;
                }

                if (tokens[0] == "err:" && tokens.size() == 4)
                {
                    CgBatchErrorType type = (CgBatchErrorType)StringToInt(tokens[1]);
                    int plat = StringToInt(tokens[2]);
                    int line = StringToInt(tokens[3]);
                    core::string fileStr;
                    shadercompiler::ReadLine(*conn, fileStr);
                    AdjustErrorFilePath(fileStr, inputPathStr);
                    shadercompiler::ReadLine(*conn, msgStr);
                    msgStr.append(" at kernel ");
                    msgStr.append(kernelName);
                    errorCallback(errorCallbackData, type, plat, line, fileStr.c_str(), msgStr.c_str());
                }
                else if (tokens[0] == "computeData:" && tokens.size() == 2)
                {
                    ok = StringToInt(tokens[1]) != 0;
                    shadercompiler::ReadBuffer(*conn, outputBuffer);
                    break;
                }
                else
                {
                    printf_console("Compute Shader compiler: unrecognized output while compiling shader. Thread=%i platform=%i kernel=%s. Source:\n%s\n\n", (int)(size_t)Thread::GetCurrentThreadID(), platform, kernelName, inputCodeStr);
                    AssertFormatMsg(false, "Compute Shader compiler: unrecognized message from the compiler process: %s", lineStr.c_str());
                    msgStr.assign("Internal error, unrecognized message from the shader compiler process while compiling kernel ");
                    msgStr.append(kernelName);
                    errorCallback(errorCallbackData, kCgBatchErrorError, -1, -1, "", msgStr.c_str());
                    ok = false;
                    break;
                }
            }
        }
        catch (shadercompiler::SocketException& ex)
        {
            printf_console("Compute Shader compiler: exception while compiling shader. Thread=%i platform=%i kernel=%s. Source:\n%s\n\n", (int)(size_t)Thread::GetCurrentThreadID(), platform, kernelName, inputCodeStr);
            needsRetry = HandleShaderSocketException(conn, ex, retryCount);
            if (!needsRetry)
            {
                AssertFormatMsg(false, "Compute Shader compiler: internal error compiling shader platform=%i: %s", platform, ex.what());
                core::string msgStr("Internal error communicating with the compute shader compiler process while compiling kernel ");
                msgStr.append(kernelName);
                errorCallback(errorCallbackData, kCgBatchErrorError, -1, -1, "", msgStr.c_str());
            }
            ok = false;
        }

        conn->readTimeoutMS = origReadTimeout;
    }
    while (needsRetry);

    return ok;
}

bool ShaderCompilerTranslateComputeKernel(
    ShaderCompilerConnection *& conn,
    const char* inputPathStr,
    const char* kernelName,
    ShaderCompilerPlatform platform,
    ShaderCompilationFlags compileFlags,
    dynamic_array<UInt8> inputBuffer,
    dynamic_array<UInt8> & outputBuffer,
    CgBatchErrorCallback errorCallback,
    void* errorCallbackData
)
{
    UInt32 retryCount = conn->timeoutRetryCount;
    bool needsRetry;
    bool ok;

    do
    {
        needsRetry = false;
        ok = true;

        outputBuffer.clear_dealloc();

        UInt32 origReadTimeout = conn->readTimeoutMS;
        conn->readTimeoutMS = s_ShaderCompileTimeoutMS;

        try
        {
            shadercompiler::SendCommand(*conn, kShaderCompilerTranslateComputeKernel);
            shadercompiler::WriteInt(*conn, platform);
            shadercompiler::WriteInt(*conn, compileFlags);
            shadercompiler::WriteBuffer(*conn, &inputBuffer[0], inputBuffer.size());

            core::string lineStr, msgStr;
            std::vector<core::string> tokens;

            while (ok)
            {
                shadercompiler::ReadLine(*conn, lineStr);
                tokens.clear();
                Split(lineStr, ' ', tokens);
                if (tokens.empty())
                {
                    AssertFormatMsg(false, "Compute Shader compiler: unrecognized message from the compiler process: %s", lineStr.c_str());
                    msgStr.assign("Internal error, empty message from the shader compiler process while translating kernel ");
                    msgStr.append(kernelName);
                    errorCallback(errorCallbackData, kCgBatchErrorError, -1, -1, "", msgStr.c_str());
                    ok = false;
                    break;
                }

                if (tokens[0] == "err:" && tokens.size() == 4)
                {
                    CgBatchErrorType type = (CgBatchErrorType)StringToInt(tokens[1]);
                    int plat = StringToInt(tokens[2]);
                    int line = StringToInt(tokens[3]);
                    core::string fileStr;
                    shadercompiler::ReadLine(*conn, fileStr);
                    AdjustErrorFilePath(fileStr, inputPathStr);
                    shadercompiler::ReadLine(*conn, msgStr);
                    msgStr.append(" at kernel ");
                    msgStr.append(kernelName);
                    errorCallback(errorCallbackData, type, plat, line, fileStr.c_str(), msgStr.c_str());
                }
                else if (tokens[0] == "computeData:" && tokens.size() == 2)
                {
                    ok = StringToInt(tokens[1]) != 0;
                    shadercompiler::ReadBuffer(*conn, outputBuffer);
                    break;
                }
                else
                {
                    printf_console("Compute Shader compiler: unrecognized output while compiling shader. Thread=%i platform=%i kernel=%s.\n\n", (int)(size_t)Thread::GetCurrentThreadID(), kernelName, platform);
                    AssertFormatMsg(false, "Compute Shader compiler: unrecognized message from the compiler process: %s", lineStr.c_str());
                    msgStr.assign("Internal error, unrecognized message from the shader compiler process while translating kernel ");
                    msgStr.append(kernelName);
                    errorCallback(errorCallbackData, kCgBatchErrorError, -1, -1, "", msgStr.c_str());
                    ok = false;
                    break;
                }
            }
        }
        catch (shadercompiler::SocketException& ex)
        {
            printf_console("Compute Shader compiler: exception while compiling shader. Thread=%i platform=%i kernel=%s.\n\n", (int)(size_t)Thread::GetCurrentThreadID(), platform, kernelName);
            needsRetry = HandleShaderSocketException(conn, ex, retryCount);
            if (!needsRetry)
            {
                AssertFormatMsg(false, "Compute Shader compiler: internal error compiling shader platform=%i: %s", platform, ex.what());
                core::string msgStr("Internal error communicating with the compute shader compiler process while translating kernel ");
                msgStr.append(kernelName);
                errorCallback(errorCallbackData, kCgBatchErrorError, -1, -1, "", msgStr.c_str());
            }
            ok = false;
        }

        conn->readTimeoutMS = origReadTimeout;
    }
    while (needsRetry);

    return ok;
}

bool ShaderCompilerCompileSnippet(
    ShaderCompilerConnection *&conn,
    const core::string& inputCodeStr,
    int inputCodeStartLine,
    const core::string& inputPathStr,
    const core::string* keywords,
    int platformKeywordCount,
    int userKeywordCount,
    CompileSnippetOptions options,
    dynamic_array<UInt8>& outputCode,
    ShaderGpuProgramType& outGpuProgramType,
    ShaderSourceLanguage language,
    ShaderCompilerProgram programType,
    UInt32 programMask,
    ShaderCompilerPlatform platform,
    ShaderRequirements requirements,
    ShaderCompilerReflectionReport* reflectionReport,
    CgBatchErrorCallback errorCallback,
    void* errorCallbackData
)
{
    UInt32 retryCount = conn->timeoutRetryCount;
    bool needsRetry;
    bool ok;

    do
    {
        needsRetry = false;
        ok = true;

        outputCode.clear_dealloc();
        outGpuProgramType = kShaderGpuProgramUnknown;

        UInt32 origReadTimeout = conn->readTimeoutMS;
        conn->readTimeoutMS = s_ShaderCompileTimeoutMS;

        try
        {
            shadercompiler::SendCommand(*conn, kShaderCompilerCompileSnippet);
            shadercompiler::WriteLine(*conn, inputCodeStr);
            shadercompiler::WriteLine(*conn, inputPathStr);
            shadercompiler::WriteInt(*conn, platformKeywordCount);
            shadercompiler::WriteInt(*conn, userKeywordCount);
            for (int i = 0, n = platformKeywordCount + userKeywordCount; i < n; ++i)
                shadercompiler::WriteLine(*conn, keywords[i]);
            shadercompiler::WriteUInt32(*conn, options);
            shadercompiler::WriteInt(*conn, language);
            shadercompiler::WriteInt(*conn, programType);
            shadercompiler::WriteInt(*conn, platform);
            shadercompiler::WriteUInt32(*conn, requirements);
            shadercompiler::WriteInt(*conn, programMask);
            shadercompiler::WriteInt(*conn, inputCodeStartLine);

            core::string lineStr, msgStr;
            std::vector<core::string> tokens;

            while (ok)
            {
                shadercompiler::ReadLine(*conn, lineStr);
                tokens.clear();
                Split(lineStr, ' ', tokens);
                if (tokens.empty())
                {
                    AssertFormatMsg(false, "Shader compiler: unrecognized message from the compiler process: %s", lineStr.c_str());
                    errorCallback(errorCallbackData, kCgBatchErrorError, -1, -1, "", "Internal error, empty message from the shader compiler process");
                    ok = false;
                    break;
                }

                if (tokens[0] == "err:" && tokens.size() == 4)
                {
                    CgBatchErrorType type = (CgBatchErrorType)StringToInt(tokens[1]);
                    int plat = StringToInt(tokens[2]);
                    int line = StringToInt(tokens[3]);
                    core::string fileStr;
                    shadercompiler::ReadLine(*conn, fileStr);
                    AdjustErrorFilePath(fileStr, inputPathStr);
                    shadercompiler::ReadLine(*conn, msgStr);
                    errorCallback(errorCallbackData, type, plat, line, fileStr.c_str(), msgStr.c_str());
                }
                else if (tokens[0] == "input:" && tokens.size() == 3)
                {
                    ShaderChannel src = (ShaderChannel)StringToInt(tokens[1]);
                    VertexComponent dst = (VertexComponent)StringToInt(tokens[2]);
                    reflectionReport->OnInputBinding(src, dst);
                }
                else if (tokens[0] == "cb:" && tokens.size() == 4)
                {
                    const core::string& name = tokens[1];
                    int size = StringToInt(tokens[2]);
                    int varCount = StringToInt(tokens[3]);
                    reflectionReport->OnConstantBuffer(name.c_str(), size, varCount);
                }
                else if (tokens[0] == "const:" && tokens.size() == 8)
                {
                    const core::string& name = tokens[1];
                    int idx = StringToInt(tokens[2]);
                    ShaderParamType dataType = (ShaderParamType)StringToInt(tokens[3]);
                    ConstantType constantType = (ConstantType)StringToInt(tokens[4]);
                    int rows = StringToInt(tokens[5]);
                    int cols = StringToInt(tokens[6]);
                    int arraySize = StringToInt(tokens[7]);
                    reflectionReport->OnConstant(name.c_str(), idx, dataType, constantType, rows, cols, arraySize);
                }
                else if (tokens[0] == "cbbind:" && tokens.size() == 3)
                {
                    const core::string& name = tokens[1];
                    int idx = StringToInt(tokens[2]);
                    reflectionReport->OnCBBinding(name.c_str(), idx);
                }
                else if (tokens[0] == "texbind:" && tokens.size() == 6)
                {
                    const core::string& name = tokens[1];
                    int idx = StringToInt(tokens[2]);
                    int sampler = StringToInt(tokens[3]);
                    int ms = StringToInt(tokens[4]);
                    TextureDimension dim = (TextureDimension)StringToInt(tokens[5]);
                    reflectionReport->OnTextureBinding(name.c_str(), idx, sampler, (ms != 0), dim);
                }
                else if (tokens[0] == "sampler:" && tokens.size() == 3)
                {
                    InlineSamplerType sampler;
                    sampler.bits = StringToInt(tokens[1]);
                    int idx = StringToInt(tokens[2]);
                    reflectionReport->OnInlineSampler(sampler, idx);
                }
                else if (tokens[0] == "uavbind:" && tokens.size() == 4)
                {
                    const core::string& name = tokens[1];
                    int idx = StringToInt(tokens[2]);
                    int origIdx = StringToInt(tokens[3]);
                    reflectionReport->OnUAVBinding(name.c_str(), idx, origIdx);
                }
                else if (tokens[0] == "bufferbind:" && tokens.size() == 3)
                {
                    const core::string& name = tokens[1];
                    int idx = StringToInt(tokens[2]);
                    reflectionReport->OnBufferBinding(name.c_str(), idx);
                }
                else if (tokens[0] == "stats:" && tokens.size() == 5)
                {
                    int alu = StringToInt(tokens[1]);
                    int tex = StringToInt(tokens[2]);
                    int flow = StringToInt(tokens[3]);
                    int tempRegister = StringToInt(tokens[4]);
                    reflectionReport->OnStatsInfo(alu, tex, flow, tempRegister);
                }
                else if (tokens[0] == "shader:" && tokens.size() == 2)
                {
                    ok = StringToInt(tokens[1]) != 0;
                    outGpuProgramType = (ShaderGpuProgramType)shadercompiler::ReadInt(*conn);
                    shadercompiler::ReadBuffer(*conn, outputCode);
                    break;
                }
                else
                {
                    printf_console("Shader compiler: unrecognized output while compiling shader snippet. Thread=%i Type=%i platform=%i. Source:\n%s\n\n", (int)(size_t)Thread::GetCurrentThreadID(), programType, platform, inputCodeStr.c_str());
                    AssertFormatMsg(false, "Shader compiler: unrecognized message from the compiler process: %s", lineStr.c_str());
                    errorCallback(errorCallbackData, kCgBatchErrorError, -1, -1, "", "Internal error, unrecognized message from the shader compiler process");
                    ok = false;
                    break;
                }
            }
        }
        catch (shadercompiler::SocketException& ex)
        {
            printf_console("Shader compiler: exception while compiling shader snippet. Thread=%i Type=%i platform=%i. Source:\n%s\n\n", (int)(size_t)Thread::GetCurrentThreadID(), programType, platform, inputCodeStr.c_str());
            needsRetry = HandleShaderSocketException(conn, ex, retryCount);
            if (!needsRetry)
            {
                AssertFormatMsg(false, "Shader compiler: internal error compiling shader snippet type=%i platform=%i: %s", programType, platform, ex.what());
                errorCallback(errorCallbackData, kCgBatchErrorError, -1, -1, "", "Internal error communicating with the shader compiler process");
            }
            ok = false;
        }

        conn->readTimeoutMS = origReadTimeout;
    }
    while (needsRetry);

    if (outputCode.empty())
    {
        switch (platform)
        {
            case kShaderCompPlatformGLES20:
            case kShaderCompPlatformGLES3Plus:
            case kShaderCompPlatformOpenGLCore:
            case kShaderCompPlatformVulkan:
            case kShaderCompPlatformMetal:
                // Some platforms merge either all or partially different shader output stages
                break;
            default:
                ok = false;
                break;
        }
    }

    return ok;
}

bool ShaderCompilerDisassemble(
    ShaderCompilerConnection *&conn,
    ShaderCompilerPlatform platform,
    const ShaderGpuProgramType shaderProgramType,
    const UInt32 shaderProgramMask,
    const dynamic_array<UInt8>& shaderCode,
    core::string& outputDisassembly,
    CgBatchErrorCallback errorCallback,
    void* errorCallbackData
)
{
    UInt32 retryCount = conn->timeoutRetryCount;
    bool needsRetry;
    bool ok;

    do
    {
        needsRetry = false;
        ok = true;

        outputDisassembly = "";

        UInt32 origReadTimeout = conn->readTimeoutMS;
        conn->readTimeoutMS = s_ShaderDisassembleTimeoutMS;

        try
        {
            shadercompiler::SendCommand(*conn, kShaderCompilerDisassembleShader);
            shadercompiler::WriteInt(*conn, platform);
            shadercompiler::WriteInt(*conn, shaderProgramType);
            shadercompiler::WriteInt(*conn, shaderProgramMask);
            shadercompiler::WriteBuffer(*conn, shaderCode.data(), shaderCode.size());

            core::string lineStr, msgStr;
            std::vector<core::string> tokens;

            while (ok)
            {
                shadercompiler::ReadLine(*conn, lineStr);
                tokens.clear();
                Split(lineStr, ' ', tokens);
                if (tokens.empty())
                {
                    AssertFormatMsg(false, "Shader compiler: unrecognized message from the compiler process: %s", lineStr.c_str());
                    errorCallback(errorCallbackData, kCgBatchErrorError, -1, -1, "", "Internal error, empty message from the shader compiler process");
                    ok = false;
                    break;
                }

                if (tokens[0] == "err:" && tokens.size() == 4)
                {
                    CgBatchErrorType type = (CgBatchErrorType)StringToInt(tokens[1]);
                    int plat = StringToInt(tokens[2]);
                    int line = StringToInt(tokens[3]);
                    core::string fileStr;
                    shadercompiler::ReadLine(*conn, fileStr);
                    shadercompiler::ReadLine(*conn, msgStr);
                    errorCallback(errorCallbackData, type, plat, line, fileStr.c_str(), msgStr.c_str());
                }
                else if (tokens[0] == "disasm:" && tokens.size() == 2)
                {
                    ok = StringToInt(tokens[1]) != 0;
                    shadercompiler::ReadLine(*conn, outputDisassembly);
                    break;
                }
                else
                {
                    printf_console("Shader compiler: unrecognized output while disassembling shader snippet. Thread=%i Type=%i platform=%i.\n\n", (int)(size_t)Thread::GetCurrentThreadID(), shaderProgramType, platform);
                    AssertFormatMsg(false, "Shader compiler: unrecognized message from the compiler process: %s", lineStr.c_str());
                    errorCallback(errorCallbackData, kCgBatchErrorError, -1, -1, "", "Internal error, unrecognized message from the shader compiler process");
                    ok = false;
                    break;
                }
            }
        }
        catch (shadercompiler::SocketException& ex)
        {
            printf_console("Shader compiler: exception while disassembling shader snippet. Thread=%i Type=%i platform=%i.\n\n", (int)(size_t)Thread::GetCurrentThreadID(), shaderProgramType, platform);
            needsRetry = HandleShaderSocketException(conn, ex, retryCount);
            if (!needsRetry)
            {
                AssertFormatMsg(false, "Shader compiler: internal error disassembling shader snippet type=%i platform=%i: %s", shaderProgramType, platform, ex.what());
                errorCallback(errorCallbackData, kCgBatchErrorError, -1, -1, "", "Internal error communicating with the shader compiler process");
            }
            ok = false;
        }

        conn->readTimeoutMS = origReadTimeout;
    }
    while (needsRetry);

    return ok;
}

static void ShaderCompilerSendShutdownMessage(ShaderCompilerConnection* p)
{
    if (!p->proc->IsRunning())
        return;
    try
    {
        shadercompiler::SendCommand(*p, kShaderCompilerShutdown);
    }
    catch (shadercompiler::SocketException&)
    {
        // ignore
    }
}

static void ShaderCompilerWaitForExitAndDestroy(ShaderCompilerConnection* p)
{
    if (!p->proc->IsRunning())
        return;

    if (p->proc)
        UNITY_DELETE(p->proc, kMemAssetImporter);

    if (p->sock)
        UNITY_DELETE(p->sock, kMemAssetImporter);
}

const char * kShaderCompPlatformNames[] = {SHADER_COMPILER_PLATFORM_NAMES_IMPL};
CompileTimeAssert(ARRAY_SIZE(kShaderCompPlatformNames) == kShaderCompPlatformCount, "SHADER COMPILER ERROR: 'kShaderCompPlatformNames' table needs to be updated.");

core::string ShaderCompilerGetPlatformName(ShaderCompilerPlatform api)
{
    if ((int)api < 0 || (int)api >= kShaderCompPlatformCount)
        return core::string();

    return kShaderCompPlatformNames[api];
}

static bool ShaderCompilerLicenseCheck(ShaderCompilerPlatform api)
{
    //TODO: Remove? no platforms need a license check for shader compilation.
    //      Will we ever need to do so again?
    return true;
}

static UInt32 ShaderCompilerProcessInitialize(ShaderCompilerConnection& conn, ShaderRequirements outPlatformFeatures[kShaderCompPlatformCount], UInt32 outPlatformShaderCompilerVersion[kShaderCompPlatformCount])
{
    UInt32 platforms = ~0;
    for (int i = 0; i < kShaderCompPlatformCount; ++i)
    {
        // always exclude platforms we don't have license for
        if (!ShaderCompilerLicenseCheck((ShaderCompilerPlatform)i))
            platforms &= ~(1 << i);
        outPlatformFeatures[i] = kShaderRequireShaderModel30; // default to "support SM3.0"
    }

    try
    {
        shadercompiler::SendCommand(conn, kShaderCompilerInitialize);   // uses command "initializeCompiler"

        // Fixed shader include search paths
        shadercompiler::WriteInt(conn, s_ShaderIncludeContext.includePaths.size());
        for (size_t i = 0, n = s_ShaderIncludeContext.includePaths.size(); i < n; ++i)
            shadercompiler::WriteLine(conn, s_ShaderIncludeContext.includePaths[i]);

        // Redirected shader include paths, maps e.g. package name to absolute path
        shadercompiler::WriteInt(conn, s_ShaderIncludeContext.redirectedFolders.size());
        for (IncludeFolderMap::iterator itr = s_ShaderIncludeContext.redirectedFolders.begin(); itr != s_ShaderIncludeContext.redirectedFolders.end(); ++itr)
        {
            shadercompiler::WriteLine(conn, itr->first);
            shadercompiler::WriteLine(conn, itr->second);
        }

        // Send custom string to shader compiler to allow per-platform specific configuration, e.g. set custom folder for shader compiler
        shadercompiler::WriteLine(conn, s_AdditionalShaderCompilerConfiguration);


        UInt32 res = shadercompiler::ReadInt(conn);
        for (int i = 0; i < kShaderCompPlatformCount; ++i)
        {
            outPlatformFeatures[i] = (ShaderRequirements)shadercompiler::ReadUInt32(conn);
            outPlatformShaderCompilerVersion[i] = shadercompiler::ReadInt(conn);
        }
        platforms &= res;
    }
    catch (shadercompiler::SocketException& ex)
    {
        // Do not log an error; once in a while the compiler crashes on startup (on Macs, rarely).
        // Reasons not known yet, but just log to console for now, and calling code will try to
        // launch the compiler a few times.
        printf_console("Shader compiler: failed to get available platforms. Likely the shader compiler crashed at initialization time. Will retry. Error: %s\n", ex.what());
        platforms = 0;
    }
    return platforms;
}

static void ShaderCompilerAddPlatformSpecificVariants(ShaderKeywordSet& keywords, BuildTargetPlatformGroup platformGroup, ShaderCompilerPlatform api, const StereoRenderingPath stereoRenderingPath, const bool enableStereo360Capture, dynamic_array<ShaderKeywordSet>& outPlatformVariants)
{
    int variantPasses = enableStereo360Capture && stereoRenderingPath != kStereoRenderingMultiPass ? 2 : 1;

    if (enableStereo360Capture)
    {
        keywords.Enable(keywords::kStereoCubemapRenderOn);
        outPlatformVariants.push_back(keywords);
        keywords.Disable(keywords::kStereoCubemapRenderOn);
    }

    for (int variant = 0; variant < variantPasses; variant++)
    {
        if (variant == 1 && enableStereo360Capture)
        {
            keywords.Enable(keywords::kStereoCubemapRenderOn);
        }

        if (stereoRenderingPath == kStereoRenderingInstancing &&
            DoesBuildTargetSupportStereoInstancingRendering(platformGroup) &&
            SupportsStereoInstancing(api))
        {
            keywords.Enable(keywords::kStereoInstancingOn);
            outPlatformVariants.push_back(keywords);
            keywords.Disable(keywords::kStereoInstancingOn);
        }
        else if (stereoRenderingPath == kStereoRenderingSinglePass &&
                 DoesBuildTargetSupportSinglePassStereoRendering(platformGroup) &&
                 SupportsSinglePassStereo(api))
        {
            const bool supportsMultiview = DoesBuildTargetSupportStereoMultiviewRendering(platformGroup) && SupportsStereoMultiview(api);

            // VR platforms may blacklist devices that support multi-view but have a number of gpu driver bugs supporting multi-view.
            // Therefore we must always include the single-pass double wide shader variants so that we can fall back to single-pass double wide
            // when multi-view is blacklisted.
            const bool usingScriptableRenderPipeline = ScriptableRenderContext::ShouldUseRenderPipeline();
            if (usingScriptableRenderPipeline || !supportsMultiview)
            {
                keywords.Enable(keywords::kSinglePassStereo);
                outPlatformVariants.push_back(keywords);
                keywords.Disable(keywords::kSinglePassStereo);
            }

            if (supportsMultiview)
            {
                keywords.Enable(keywords::kStereoMultiviewOn);
                outPlatformVariants.push_back(keywords);
                keywords.Disable(keywords::kStereoMultiviewOn);
            }

            // Avoid too many variants for other typical platforms like DX11
            // With SP rendering, only enable SPI when some Multiview supporting target platforms might need it
            // TODO: Add new DoesBuildTargetSupport* helper if this is necessary for multiple platforms

            if (supportsMultiview && SupportsStereoInstancing(api))
            {
                keywords.Enable(keywords::kStereoInstancingOn);
                outPlatformVariants.push_back(keywords);
                keywords.Disable(keywords::kStereoInstancingOn);
            }
        }
        keywords.Disable(keywords::kStereoCubemapRenderOn);
    }
}

void ShaderCompilerGetPlatformSpecificVariants(BuildTargetPlatformGroup platformGroup, ShaderCompilerPlatform api, const StereoRenderingPath stereoRenderingPath, const bool enable360StereoCapture, dynamic_array<ShaderKeywordSet>& outPlatformVariants)
{
    outPlatformVariants.resize_uninitialized(0);

    ShaderKeywordSet keywords;
    outPlatformVariants.push_back(keywords);
    ShaderCompilerAddPlatformSpecificVariants(keywords, platformGroup, api, stereoRenderingPath, enable360StereoCapture, outPlatformVariants);

#if GFX_SUPPORTS_SHADERCOMPILER_EXT_PLUGIN
    const ShaderKeywordSet& UnityShaderExtGetCustomKeywordsFromPlugin();
    const ShaderKeywordSet& pluginKeywords = UnityShaderExtGetCustomKeywordsFromPlugin();
    if (pluginKeywords.CountEnabled() > 0)
    {
        keywords.Reset();
        keywords::AddKeywords(keywords, pluginKeywords);
        outPlatformVariants.push_back(keywords);
        ShaderCompilerAddPlatformSpecificVariants(keywords, platformGroup, api, stereoRenderingPath, enable360StereoCapture, outPlatformVariants);
    }
#endif
}

#if UNITY_EDITOR
static bool ShaderCompilerShouldSkipVariant(ShaderCompilerPerThread& perThread, const ShaderKeywordSet& keywords, const PlatformCapsKeywords& platformKeywords, ShaderCompilerPlatform api, BuildTargetSelection buildTarget)
{
    // Is this platform not available at all?
    if ((perThread.availableCompilerPlatforms & (1 << api)) == 0)
        return true;

    if ((keywords.IsEnabled(keywords::kInstancingOn) || keywords.IsEnabled(keywords::kProceduralInstancingOn))
        && !SupportsInstancing(api))
        return true;

    bool platformNeedsETC1Variant = IsBuildTargetETC(buildTarget.platform) ||
        IsPCStandaloneTargetPlatform(buildTarget.platform) ||                             /*Builtin resources are built using Windows player. Don't want to throw away the ETC1 variant from builtin resources*/
        buildTarget.platform == kBuildNoTargetPlatform;                             /*Editor compiles project local shaders with this target. Don't throw away ETC1 variant from editor compiled shaders*/
    if (keywords.IsEnabled(keywords::kETC1ExternalAlpha) && !platformNeedsETC1Variant)
        return true;

    return false;
}

bool ShaderCompilerShouldSkipVariant(const ShaderKeywordSet& keywords, const PlatformCapsKeywords& platformKeywords, ShaderCompilerPlatform api, BuildTargetSelection buildTarget)
{
    ShaderCompilerPerThread* perThread = GetShaderCompilerPerThreadInternal();
    if (perThread)
        return ShaderCompilerShouldSkipVariant(*perThread, keywords, platformKeywords, api, buildTarget);
    else
        return true;
}

bool ShaderCompilerIsPlatformAvailable(ShaderCompilerPlatform platform)
{
    ShaderCompilerPerThread* perThread = GetShaderCompilerPerThreadInternal();
    if (perThread)
    {
        return ((perThread->availableCompilerPlatforms & (1 << platform)) != 0);
    }
    else
    {
        return false;
    }
}

#endif

static bool ShaderCompilerSupportsRequirements(ShaderCompilerPerThread& perThread, ShaderRequirements requirements, ShaderCompilerPlatform api)
{
    // Is this platform not available at all?
    if ((perThread.availableCompilerPlatforms & (1 << api)) == 0)
        return false;

    // All the required features have to be supported
    return (perThread.compilerPlatformSupportedFeatures[api] & requirements) == requirements;
}

bool ShaderCompilerSupportsRequirements(ShaderRequirements requirements, ShaderCompilerPlatform api)
{
    ShaderCompilerPerThread* perThread = GetShaderCompilerPerThreadInternal();
    if (perThread)
        return ShaderCompilerSupportsRequirements(*perThread, requirements, api);
    else
        return false;
}

static bool Is64BitOS()
{
#if PLATFORM_WIN && !PLATFORM_ARCH_64
    // 32-bit programs run on both 32-bit and 64-bit Windows so must sniff
    BOOL isWow64 = FALSE;

    typedef BOOL (WINAPI *FuncIsWow64Process) (HANDLE, PBOOL);
    HMODULE kernel = GetModuleHandleW(L"kernel32");
    if (kernel)
    {
        FuncIsWow64Process funcWow64 = (FuncIsWow64Process)GetProcAddress(kernel, "IsWow64Process");
        if (NULL != funcWow64)
            funcWow64(GetCurrentProcess(), &isWow64);
    }
    return isWow64 ? true : false;
#else
    return true;
#endif
}

void ShaderCompilerClearIncludesCache()
{
    Mutex::AutoLock lock(s_AllShaderCompilersMutex);

    for (size_t i = 0; i < s_AllShaderCompilers.size(); i++)
    {
        ShaderCompilerPerThread* perThread = s_AllShaderCompilers[i];
        try
        {
            // Each shader compiler process holds its own cache for the include files
            // to avoid redundant disk reads.
            shadercompiler::SendCommand(perThread->connection, kShaderCompilerClearIncludesCache);
        }
        catch (shadercompiler::SocketException&)
        {
            // ignore
        }
    }
}
