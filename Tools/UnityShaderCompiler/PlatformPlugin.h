#pragma once

// data that is shared with a CgBatchPlugin

#include "Runtime/Threads/Mutex.h"
#include "Tools/UnityShaderCompiler/Utilities/D3DCompiler.h"
#include "Utilities/ShaderImportErrors.h"

typedef std::map<core::string, core::string> OpenedIncludeFileMap;

// implementation defined in common file so it can be included by CgBatch as well as CgBatch plugins

struct ShaderCompileInputInterface;

// handler for D3DCompiler based shader compilers
struct HLSLIncludeHandler : D3D10Include
{
    HLSLIncludeHandler(const IncludeSearchContext& includeContext);

    // forwards to above, querying includePaths from input
    HLSLIncludeHandler(const ShaderCompileInputInterface* input);

    HRESULT WINAPI_IMPL Open(D3D10_INCLUDE_TYPE IncludeType, const char* pFileName, const void* pParentData, const void* *ppData, UINT *pBytes);
    HRESULT WINAPI_IMPL Close(const void* pData);

    void SetIncludePaths(const IncludePaths& paths) { Mutex::AutoLock lock(m_Mutex); m_IncludeContext.includePaths = paths; }
    void UpdateErrorsWithFullPathnames(ShaderImportErrors &errors);
    std::vector<core::string> GetOpenedIncludeFiles() const;

private:
    // utility function that does the grunt work of trying to open and read a file, and adds
    // to m_PathMap if successful
    HRESULT Open(const core::string& filename, const void* *ppData, UINT* pBytes);

    // map from const void * to path.
    // the key is the allocated pointer we assign into *ppData in Open(),
    // and is passed as pParentData for nested includes
    typedef std::map<const void*, core::string> ParentPathMap;

    ParentPathMap m_PathMap;

    OpenedIncludeFileMap m_OpenedFiles;

    // global search directories
    IncludeSearchContext m_IncludeContext;

    Mutex m_Mutex;
};

struct Hlsl2Glsl_ParseCallbacks;

// handler for HLSL2GLSL based shader compilers
struct GLSLIncludeHandler
{
    GLSLIncludeHandler(const IncludeSearchContext& includeContext, ShaderImportErrors& outErrors, ShaderCompilerPlatform api);
    // forwards to above, querying includePaths from input
    GLSLIncludeHandler(const ShaderCompileInputInterface* input, ShaderImportErrors& outErrors, ShaderCompilerPlatform api);

    // leaves includes paths, errors and API to be initialized later
    GLSLIncludeHandler();

    // sets up a struct with the callback function and this object
    void SetupHlsl2GlslCallbacks(Hlsl2Glsl_ParseCallbacks &callbacks);

    void UpdateErrorsWithFullPathnames(ShaderImportErrors &errors);

    void SuppressMissingIncludeErrors() { m_SuppressErrors = true; }

    // map from string to path.
    // the key is the xxHash of the string we return in the callback,
    // and is passed as parent for nested includes
    typedef std::map<UInt32, core::string> ParentPathMap;

    ParentPathMap m_PathMap;
    OpenedIncludeFileMap m_OpenedFiles;

    // compile data
    IncludeSearchContext m_IncludeContext;
    ShaderImportErrors* m_Errors;
    ShaderCompilerPlatform m_API;
    bool m_SuppressErrors;
};

// Interface and data provided to a plugin to perform a compile job
struct ShaderCompileInputInterface
{
    ShaderCompileInputInterface()
        : startLine(0)
        , programType(kProgramCount)
        , gpuProgramType(kShaderGpuProgramUnknown)
        , api(kShaderCompPlatformOpenGLCore)
        , requirements(kShaderRequireShaderModel25_93)
        , sourceLanguage(kShaderSourceLanguageCg)
        , compileFlags(kShaderCompFlags_None)
    {
    }

    // accessors for complex/STL data
    virtual const char* GetSource() const = 0;
    virtual size_t GetSourceLength() const = 0;
    virtual const char* GetSourceFilename() const = 0;
    virtual size_t GetSourceFilenameLength() const = 0;

    virtual const char* GetEntryName(ShaderCompilerProgram programType) const = 0;
    virtual size_t GetEntryNameLength(ShaderCompilerProgram programType) const = 0;
    virtual const char* GetEntryName() const = 0;

    virtual const IncludeSearchContext* GetIncludeSearchContext() const = 0;

    virtual const UInt8* GetCompiledDataPtr() const = 0;
    virtual size_t GetCompiledDataSize() const = 0;

    virtual size_t GetNumShaderArgs() const = 0;
    virtual const char* GetShaderArgName(size_t index) const = 0;
    virtual size_t GetShaderArgNameLength(size_t index) const = 0;
    virtual const char* GetShaderArgValue(size_t index) const = 0;
    virtual size_t GetShaderArgValueLength(size_t index) const = 0;

    // reflection interface
    ShaderCompilerReflectionReport* reflectionReport;

    // basic typed data
    int startLine;
    ShaderCompilerProgram programType;
    UInt32 programMask;
    ShaderGpuProgramType gpuProgramType;
    ShaderCompilerPlatform api;
    ShaderRequirements requirements;
    ShaderSourceLanguage sourceLanguage;
    ShaderCompilationFlags compileFlags;
};

// Interface for a compile job to return with output data
struct ShaderCompileOutputInterface
{
    ShaderCompileOutputInterface(ShaderCompilerPlatform a)
        : api(a)
        , compileFlags(kShaderCompFlags_None)
    {
    }

    virtual ~ShaderCompileOutputInterface() {}

    virtual const UInt8* GetCompiledDataPtr() const = 0;
    virtual size_t GetCompiledDataSize() const = 0;

    virtual const char* GetDisassembly() const = 0;

    virtual ShaderGpuProgramType GetGpuProgramType() const = 0;

    virtual bool HasErrors() const = 0;
    virtual size_t GetNumErrorsAndWarnings() const = 0;
    virtual void GetErrorOrWarning(size_t index,
        const char*& message, const char*& file,
        int& line, bool& warning,
        ShaderCompilerPlatform& api) const = 0;

    // used to locate the right plugin when releasing this data
    ShaderCompilerPlatform api;

    // Right now, only used as a migrational step to know if we ended compiling kShaderCompPlatformGLES20 with HLSLcc or hlsl2glsl
    ShaderCompilationFlags compileFlags;
};

// simple implementation that each plugin can use if it doesn't need
// anything special
struct ShaderCompileOutputData : public ShaderCompileOutputInterface
{
    enum OutputDataType { kStringOutputData, kBinaryOutputData };
    ShaderCompileOutputData(ShaderCompilerPlatform api, OutputDataType type)
        : ShaderCompileOutputInterface(api)
        , dataType(type)
    {
    }

    virtual const UInt8* GetCompiledDataPtr() const { return dataType == kStringOutputData ? (const UInt8*)stringOutput.c_str() : binaryOutput.data(); }
    virtual size_t GetCompiledDataSize() const { return dataType == kStringOutputData ? stringOutput.size() : binaryOutput.size(); }

    virtual const char* GetDisassembly() const { return disassembly.c_str(); }

    virtual ShaderGpuProgramType GetGpuProgramType() const { return gpuProgramType; }

    virtual bool HasErrors() const { return errors.HasErrors(); }
    virtual size_t GetNumErrorsAndWarnings() const { return errors.GetErrors().size(); }
    virtual void GetErrorOrWarning(size_t index,
        const char*& message, const char*& file,
        int& line, bool& warning,
        ShaderCompilerPlatform& api) const
    {
        if (index >= GetNumErrorsAndWarnings())
            return;

        ShaderImportErrors::ErrorContainer::iterator it = errors.GetErrors().begin();
        for (size_t i = 0; i < index; i++) it++;

        message = it->message.c_str();
        file = it->file.c_str();
        line = it->line;
        warning = it->warning;
        api = it->api;
    }

    // set in constructor, used to choose between returning
    // stringOutput or binaryOutput from GetCompiledData*
    OutputDataType dataType;

    core::string stringOutput;
    dynamic_array<UInt8> binaryOutput;
    ShaderGpuProgramType gpuProgramType;

    core::string disassembly;

    ShaderImportErrors errors;

    // utility function for appending data to binaryOutput
    void AppendBinaryData(const void* data, size_t size)
    {
        size_t offset = binaryOutput.size();
        binaryOutput.resize_uninitialized(offset + size);
        memcpy(binaryOutput.data() + offset, data, size);
    }
};

// Interface to the platform plugin, used by the main shader compiler
struct PlatformPluginInterface
{
    virtual unsigned int GetShaderPlatforms() = 0;
    virtual const char* GetPlatformName() = 0;
    virtual unsigned int GetShaderCompilerVersion() { return 0; }   // base class will return a compiler version of 0 if not implemented in per-platform class

    virtual int  RunPluginUnitTests(const char* includesPath) = 0;
    virtual void Shutdown() = 0;
    virtual const ShaderCompileOutputInterface* CompileShader(const ShaderCompileInputInterface *) = 0;
    virtual const ShaderCompileOutputInterface* DisassembleShader(const ShaderCompileInputInterface *) = 0;
    virtual const ShaderCompileOutputInterface* CompileComputeShader(const ShaderCompileInputInterface *) = 0;
    virtual const ShaderCompileOutputInterface* TranslateComputeKernel(const ShaderCompileInputInterface *) { return NULL; }
    virtual void ReleaseShader(const ShaderCompileOutputInterface *) = 0;
    virtual void InitializeCompiler() {}    // base class will do nothing if not implemented in per-platform class
    // Override this method only if your platform compiler is accessed through a DLL. It will internally use its own includes cache in that case.
    virtual void ClearIncludesCache() {}
};

struct PlatformPluginDispatcher
{
    // The D3D compiler is a shared resource for the process that can be used by any plugin
    virtual D3DCompiler *GetD3DCompiler() = 0;

    // while we are in the process of migrating from hlsl2glsl to hlslcc we want to support both for gles2
    //   but at the same time we want to have hlslcc as default for everything
    // due to the way code is structured we can only query plugins by "shader platform"
    // we want *all* glsl platform to use hlslcc by default
    //   but we still need a way to provide hlsl2glsl plugin (we may allow opt-out from hlslcc in some cases)
    // when we full switch all glsl platforms to be backed by hlslcc this can be removed safely
    //   along witn hlsl2glsl and glsloptimizer
    virtual PlatformPluginInterface* GetHLSL2GLSLPlugin() = 0;

    virtual bool HasCompilerForAPI(ShaderCompilerPlatform api) = 0;

    virtual PlatformPluginInterface* Get(ShaderCompilerPlatform api) = 0;

    // for convenience, calling directly into PlatformPluginInterface using the
    // platform specified in ShaderCompileInputInterface/ShaderCompileOutputInterface
    virtual const ShaderCompileOutputInterface* CompileShader(const ShaderCompileInputInterface *) = 0;
    virtual const ShaderCompileOutputInterface* DisassembleShader(const ShaderCompileInputInterface *) = 0;
    virtual const ShaderCompileOutputInterface* CompileComputeShader(const ShaderCompileInputInterface *) = 0;
    virtual const ShaderCompileOutputInterface* TranslateComputeKernel(const ShaderCompileInputInterface *) = 0;
    virtual void ReleaseShader(const ShaderCompileOutputInterface *) = 0;
    virtual void ClearIncludesCache() = 0;
    virtual void InitializeCompiler(ShaderCompilerPlatform api) = 0;

    virtual int GetShaderCompilerVersion(ShaderCompilerPlatform api) = 0;
};

// declared in the shader compiler and filled with the interfaces to each plugin.
// Passed at declaration time to each plugin to let them invoke each other to chain things together
extern PlatformPluginDispatcher* gPluginDispatcher;

// these helper functions are included in all plugins and declared here

void ParseErrorMessages(const core::string& listing, bool warnings, const core::string& token, const core::string& entry, int startLine, ShaderCompilerPlatform api, ShaderImportErrors& outErrors);
void ParseErrorMessagesStartLine(const core::string& listing, const core::string& token, const core::string& entry, int startLine, ShaderCompilerPlatform api, ShaderImportErrors& outErrors);

void ChannelFromSemantic(const char* semantic, ShaderChannel* outShaderChannel, VertexComponent* outVertexComponent);
bool IsValidChannel(const char* name);
void MassageConstantName(char* name);

const char* GetCompilerPlatformDefine(ShaderCompilerPlatform api);
core::string GetGLSLDefinesString(const ShaderCompileInputInterface* input, ShaderCompilerPlatform api);

#define MAX_PLATFORM_PLUGINS (16)       // we need to make sure we don't enable more that 32 shaders (api bit mask)
