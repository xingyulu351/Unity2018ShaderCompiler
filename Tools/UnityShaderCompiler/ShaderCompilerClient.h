#pragma once

#include "Runtime/Allocator/STLAllocator.h"
#include "Runtime/GfxDevice/GfxDeviceTypes.h"
#include <string>
#include <vector>
#include "Runtime/Utilities/dynamic_array.h"
#include "Editor/Src/BuildPipeline/BuildTargetPlatformSpecific.h"
#include "Runtime/Shaders/ShaderKeywordSet.h"
#include "Runtime/Core/Containers/hash_map.h"
#include "ShaderCompilerTypes.h"

#if UNITY_EDITOR
    #include "Runtime/Graphics/PlatformCapsKeywords.h"
#endif

struct ShaderCompilerConnection;
struct ShaderCompileRequirements;


/*
    Editor-side interface into shader compilation.

    All actual compilation is done by external process. Get that process with GetShaderCompilerPerThread - when called
    from multiple threads, this will use/spawn a separate process for each calling thread. This does mean the API
    is thread-safe and shader compilation can go in parallel.

    The actual API calls will do all communication with the external process.
*/

// we keep it here because we need synced names both in editor and in shader compiler process
#define SHADER_COMPILER_PLATFORM_NAMES_IMPL \
    "opengl", \
    "d3d9", \
    "xbox360", \
    "ps3", \
    "d3d11", \
    "gles", \
    "glesdesktop", \
    "flash", \
    "d3d11_9x", \
    "gles3", \
    "psp2", \
    "ps4", \
    "xboxone", \
    "psm", \
    "metal", \
    "glcore", \
    "n3ds", \
    "wiiu", \
    "vulkan", \
    "switch", \
    "xboxone_d3d12"

extern const char* kShaderCompPlatformNames[];

enum CgBatchErrorType
{
    kCgBatchErrorInfo,
    kCgBatchErrorWarning,
    kCgBatchErrorError,
};

extern const ShaderCompilerProgram kShaderTypeToCompilerType[kShaderTypeCount];

ShaderCompilerConnection* GetShaderCompilerPerThread();

int GetShaderCompilerVersion(ShaderCompilerPlatform api);   // returns version of platform shader compiler, used to trigger rebuilding of shaders after sdk change

typedef dynamic_array<core::string> IncludePaths;
typedef core::hash_map<core::string, core::string> IncludeFolderMap;

struct IncludeSearchContext
{
    void InsertPriorityPath(const core::string& priorityPath)
    {
        // Set priorityPath as the first path to search, over the working directory "."
        // This lets includes next to the shader have priority over includes in the project root.
        if (!priorityPath.empty())
        {
            IncludePaths newPaths;
            includePaths.reserve(includePaths.size() + 1);
            newPaths.push_back(priorityPath);
            newPaths.insert(newPaths.end(), includePaths.begin(), includePaths.end());
            includePaths = newPaths;
        }
    }

    IncludePaths        includePaths;
    IncludeFolderMap    redirectedFolders;
};

void InitShaderCompilerClient();
void InitIncludePaths(const IncludeSearchContext& includeContext);
void ReinitialiseShaderCompilerProcesses();
void SetAdditionalShaderCompilerConfiguration(const core::string &configstring);
void CleanupShaderCompilerClient();

typedef void (*CgBatchErrorCallback)(
    void* userData,
    CgBatchErrorType type,
    int platform,
    int line,
    const char* file,
    const char* message
);


// -------------------------------------------------------------------
// Shader preprocessing:
//
// When shader is imported, a one-time preprocessing is done. This is surface shader generation,
// extracting CGPROGRAM snippets with their parameters and keyword combinations. All the produced data is stored
// in Shader object, serialized as editor-only data.
//
// Later on, actually needed shader variants are compiled on demand (and cached to save on redundant compilations).
// See ShaderCompilerCompileSnippet.

class ShaderCompilerPreprocessCallbacks
{
public:
    virtual ~ShaderCompilerPreprocessCallbacks() {}
    virtual void OnError(CgBatchErrorType type, int platform, int line, const char* fileStr, const char* messageStr) = 0;
    virtual void OnAddSnippet(const int id, const char* snippet, int startLine, UInt32 platformMask, UInt32 hardwareTierVariantsMask, UInt32 typesMask, ShaderSourceLanguage language, UInt32 includesHash[4]) = 0;
    virtual void OnIncludeDependencies(const std::vector<core::string> &includedFiles) = 0;
    virtual void OnUserKeywordVariants(const int snippetID, const ShaderCompilerProgram programType, const std::vector<std::vector<core::string> >& keywords) = 0;
    virtual void OnBuiltinKeywordVariants(const int snippetID, const ShaderCompilerProgram programType, const std::vector<std::vector<core::string> >& keywords) = 0;
    virtual void OnEndKeywordCombinations(const int snippetID, const std::vector<core::string>& alwaysIncludedUserKeywords, const std::vector<core::string>& builtinKeywords, const ShaderCompileRequirements& targetInfo) = 0;
};

bool ShaderCompilerPreprocess(
    ShaderCompilerConnection *&conn,
    const core::string& inputShaderStr,
    const core::string& inputPathStr,
    core::string& outputShader,
    bool& outHadSurfaceShaders,
    ShaderCompilerPreprocessCallbacks* callbacks,
    bool surfaceGenerationOnly
);

enum ConstantType
{
    kConstantTypeDefault = 0, // scale or vector
    kConstantTypeMatrix,
    kConstantTypeStruct,
};

// -------------------------------------------------------------------
// Shader snippet compilation:
//
// Compiles a single piece of shader (e.g. a single pixel shader) for a single platform, for one particular set
// of keyword defines. Reports back the resulting shader code/bytecode, as well as reflection information
// (vertex bindings, uniforms, textures etc.).

class ShaderCompilerReflectionReport
{
public:
    virtual ~ShaderCompilerReflectionReport() {}

    // For vertex shader: records which mesh data component should go into which attribute/input slot.
    virtual void OnInputBinding(ShaderChannel dataChannel, VertexComponent inputSlot) = 0;
    virtual void OnConstantBuffer(const char* name, int size, int varCount) = 0;
    virtual void OnConstant(const char* name, int bindIndex, ShaderParamType dataType, ConstantType constantType, int rows, int cols, int arraySize) = 0;
    virtual void OnCBBinding(const char* name, int bindIndex) = 0;

    // For each used texture. Often a texture is sampled with it's own sampler (i.e. sampling state comes from texture itself).
    // However a platform shader compiler does not necessarily assign the same binding slots for texture & sampler; so they are separate here.
    // samplerIndex can be -1 if this texture is not used with it's own sampler in the shader.
    virtual void OnTextureBinding(const char* name, int bindIndex, int samplerIndex, bool multisampled, TextureDimension dim) = 0;

    // Some sampler states ("inline samplers") can be used without a texture that the settings come from. For every such sampler used by the shader,
    // it is reported here.
    virtual void OnInlineSampler(InlineSamplerType sampler, int bindIndex) = 0;

    virtual void OnBufferBinding(const char* name, int bindIndex) = 0;

    // Called on UAV Binding used by the shader. OriginalBindIndex contains the original bind index given in the source shader. On most platforms it's equal to bindIndex.
    // This function is not pure virtual as it's not needed on all platforms.
    virtual void OnUAVBinding(const char* name, int bindIndex, int originalBindIndex) {}

    virtual void OnStatsInfo(int alu, int tex, int flow, int tempRegister) = 0;
};


enum CompileSnippetOptions
{
    kCompileSnippetNone = 0,
    kCompileSnippetMetalUsePointFilterForShadows = 1 << 0,
};
ENUM_FLAGS(CompileSnippetOptions);


bool ShaderCompilerCompileSnippet(
    ShaderCompilerConnection *&conn,
    const core::string& inputCodeStr,
    int inputCodeStartLine,
    const core::string& inputPathStr,
    const core::string* keywords,   /* first platformKeywordCount "platform caps" keywords, then userKeywordCount "user" keywords */
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
);

typedef std::pair<core::string, core::string> MacroPair;
typedef dynamic_array<MacroPair> MacroArray;

bool ShaderCompilerPreprocessCompute(
    ShaderCompilerConnection *&conn,
    const core::string& inputShaderStr,
    const core::string& inputPathStr,
    core::string& outputShader,
    dynamic_array<core::string>& outKernelNames,
    dynamic_array<MacroArray>& outMacros,
    ShaderCompilationFlags &outCompilationFlags,
    UInt32& supportedAPIs,
    UInt32* outIncludeHash,
    std::vector<core::string>& outIncludeFiles,
    CgBatchErrorCallback errorCallback,
    void* errorCallbackData);

bool ShaderCompilerCompileComputeKernel(
    ShaderCompilerConnection *&conn,
    const char* inputCodeStr,
    const char* inputPathStr,
    const char* kernelName,
    const dynamic_array<std::pair<core::string, core::string> >& macros,
    ShaderCompilerPlatform platform,
    ShaderCompilationFlags compileFlags,
    dynamic_array<UInt8> & outputBuffer,
    CgBatchErrorCallback errorCallback,
    void* errorCallbackData
);

bool ShaderCompilerTranslateComputeKernel(
    ShaderCompilerConnection *&conn,
    const char* inputPathStr,
    const char* kernelName,
    ShaderCompilerPlatform platform,
    ShaderCompilationFlags compileFlags,
    dynamic_array<UInt8> inputBuffer,
    dynamic_array<UInt8> & outputBuffer,
    CgBatchErrorCallback errorCallback,
    void* errorCallbackData
);

bool ShaderCompilerDisassemble(
    ShaderCompilerConnection *&conn,
    ShaderCompilerPlatform platform,
    const ShaderGpuProgramType shaderProgramType,
    const UInt32 shaderProgramMask,
    const dynamic_array<UInt8>& shaderCode,
    core::string& outputDisassembly,
    CgBatchErrorCallback errorCallback,
    void* errorCallbackData
);

core::string ShaderCompilerGetPlatformName(ShaderCompilerPlatform api);

void ShaderCompilerGetPlatformSpecificVariants(BuildTargetPlatformGroup platformGroup, ShaderCompilerPlatform api, const StereoRenderingPath stereoRenderingPath, const bool enable360StereoCapture, dynamic_array<ShaderKeywordSet>& outPlatformVariants);

bool ShaderCompilerSupportsRequirements(ShaderRequirements requirements, ShaderCompilerPlatform api);
#if UNITY_EDITOR
bool ShaderCompilerShouldSkipVariant(const ShaderKeywordSet& keywords, const PlatformCapsKeywords& platformKeywords, ShaderCompilerPlatform api, BuildTargetSelection buildTarget);
bool ShaderCompilerIsPlatformAvailable(ShaderCompilerPlatform platform);
#endif

void ShaderCompilerClearIncludesCache();
