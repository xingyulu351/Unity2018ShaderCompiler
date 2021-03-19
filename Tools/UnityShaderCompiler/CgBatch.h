#pragma once

#include "ShaderCompilerClient.h"
typedef std::vector<unsigned char>           tReturnedVector;
typedef std::vector<unsigned char>::iterator tReturnedVectorIterator;

bool CgBatchPreprocessShader(
    const core::string& inputShaderStr,
    const core::string& inputPathStr,
    core::string& outputShader,
    ShaderCompilerPreprocessCallbacks* callbacks,
    bool surfaceGenerationOnly,
    bool* outHadSurfaceShaders
);

bool CgBatchDisassembleShader(
    ShaderCompilerPlatform platform,
    ShaderGpuProgramType programType,
    UInt32 programMask,
    const dynamic_array<UInt8> &inputShader,
    core::string &outputDisassembly,
    CgBatchErrorCallback errorCallback,
    void* errorCallbackData
);

bool CgBatchCompileSnippet(
    const core::string& inputCode,
    int startLine,
    const core::string& inputPathStr,
    const core::string* keywords,
    int platformKeywordCount,
    int userKeywordCount,
    CompileSnippetOptions options,
    tReturnedVector& outputCode,
    ShaderGpuProgramType& outGpuProgramType,
    ShaderSourceLanguage language,
    ShaderCompilerProgram programType,
    UInt32 programMask,
    ShaderCompilerPlatform platform,
    ShaderRequirements requirements,
    ShaderCompilerReflectionReport* reflectionReport,
    CgBatchErrorCallback errorCallback,
    void* errorCallbackData
);

bool CgBatchPreprocessComputeShader(
    core::string& inoutShaderStr,
    const core::string& inputPathStr,
    dynamic_array<core::string>& outKernelNames,
    dynamic_array<MacroArray>& outMacros,
    ShaderCompilationFlags& outCompileFlags,
    UInt32& supportedAPIs,
    UInt32* outIncludesHash,
    std::vector<core::string>& outIncludeFiles,
    CgBatchErrorCallback errorCallback,
    void* errorCallbackData
);

bool CgBatchCompileComputeKernel(
    const core::string& inputCode,
    const core::string& inputPathStr,
    const core::string& kernelName,
    const dynamic_array<std::pair<core::string, core::string> >& macros,
    tReturnedVector & result,
    ShaderCompilerPlatform platform,
    ShaderCompilationFlags compileFlags,
    CgBatchErrorCallback errorCallback,
    void* errorCallbackData
);

bool CgBatchTranslateComputeKernel(
    ShaderCompilerPlatform platform,
    ShaderCompilationFlags compileFlags,
    const dynamic_array<UInt8> &inputShader,
    dynamic_array<UInt8> &outputShader,
    CgBatchErrorCallback errorCallback,
    void* errorCallbackData
);

void CgBatchSetIncludeSearchContext(const IncludeSearchContext& includeContext);
const IncludeSearchContext& CgBatchGetIncludeSearchContext();
void CgBatchClearIncludesCache();
void CgBatchConfigureSystemFromJson(core::string configurationData);


core::string CgBatchInitialize(const core::string& appPath);
core::string CgBatchInitialize(const core::string& appPath, const core::string& modulesLocation);
void CgBatchShutdown();
int RunCgBatchUnitTests(const char* includesPath, const std::vector<core::string>& testFilters);

extern core::string g_IncludesPath;

bool SetEnv(const char *envvar, const char *value);
bool GetEnvVar(const char* var, core::string& value);
