#pragma once

#if PLATFORM_WIN
#define ENABLE_PSSL_COMPILER 1
#define ENABLE_XBOXONE_COMPILER 1  // 32 bit version of editor will shell out to 64 bit version of shader compiler
#define ENABLE_SWITCH_COMPILER 1
#else
#define ENABLE_PSSL_COMPILER 0
#define ENABLE_XBOXONE_COMPILER 0
#define ENABLE_SWITCH_COMPILER 0
#endif


typedef std::vector<unsigned char> tReturnedVector;

#include "Utilities/ShaderImportUtils.h"
#include "Utilities/ShaderImportErrors.h"
#include "Runtime/Utilities/Word.h"

// --------------------------------------------------------------------------
// Macro/define parameter for shader compilation

struct ShaderCompileArg
{
    ShaderCompileArg(const core::string& n, const core::string& v) : name(n), value(v) {}
    core::string name;
    core::string value;
};

typedef std::vector<ShaderCompileArg> ShaderArgs;


// --------------------------------------------------------------------------
// Compilation info for single shader snippet: entry point names,
// target levels, etc.

struct ShaderCompileParams
{
    ShaderCompileParams()
        : compileFlags(kShaderCompFlags_None)
        , sourceLanguage(kShaderSourceLanguageCg)
        , maxInstanceCount(-1), forcedMaxInstanceCount(-1)
    {
    }

    core::string entryName[kProgramCount];
    ShaderCompilationFlags compileFlags;
    ShaderSourceLanguage sourceLanguage;
    int maxInstanceCount;               // for instancing: see FindInstancingOptions in CgBatch.cpp
    int forcedMaxInstanceCount;         // for instancing: see FindInstancingOptions in CgBatch.cpp
    core::string proceduralFuncName;    // setup function for procedural instancing
};

struct ShaderCompileInputInterface;
struct ShaderCompileOutputInterface;

void InitializeDispatcher();
// returns non-empty string on error, with error message
core::string InitializeCompiler(const core::string& appPath);
void ShutdownCompiler();

ShaderSourceLanguage StripComputeShaderSourceVariants(core::string& src, ShaderCompilerPlatform platform);

// utility function for adding to a ShaderImportErrors through ShaderCompileOutputData
void AddErrorsFrom(ShaderImportErrors& errors, const ShaderCompileInputInterface* input, const ShaderCompileOutputInterface* output);
void AddErrorsFrom(std::vector<ShaderImportError>& errors, const ShaderCompileInputInterface* input, const ShaderCompileOutputInterface* output);


// Features that are *always* supported by each compiler backend platform; i.e. no matter what the target shader compilation
// level is, these features are always there (e.g. on d3d11, SM4.0 is the minspec; a lower target just does not exist)
ShaderRequirements GetCompilePlatformMinSpecFeatures(ShaderCompilerPlatform api);
// All possibly supported features by each compiler backend platform (if a feature is not there, that means said platform can not do it at all)
ShaderRequirements GetCompilePlatformSupportedFeatures(ShaderCompilerPlatform api);


void ParseErrorMessages(const core::string& listing, bool warnings, const core::string& token, const core::string& entry, int startLine, ShaderCompilerPlatform api, ShaderImportErrors& outErrors);
void ParseErrorMessagesPostfix(const core::string& listing, bool warnings, const core::string& token, const core::string& entry, int startLine, ShaderCompilerPlatform api, ShaderImportErrors& outErrors, int adjustLinesBy = 0);
void ParseErrorMessagesStartLine(const core::string& listing, const core::string& token, const core::string& entry, int startLine, ShaderCompilerPlatform api, ShaderImportErrors& outErrors);
void ParseErrorMessagesStartLine(const core::string& listing, bool warnings, const core::string& token, const core::string& entry, int startLine, ShaderCompilerPlatform api, ShaderImportErrors& outErrors);

// Turn some of "known" typical shader compiler errors into more readable/actionable ones.
void MassageShaderCompilerErrors(ShaderImportErrors& errors, ShaderCompilerPlatform platform, ShaderRequirements requirements);

bool IsValidChannel(const char* name);

// Adds SHADER_TARGET and SHADER_REQUIRE_* macros, based on what #pragma target or #pragma require in the shader source indicates.
void AddShaderTargetRequirementsMacros(ShaderRequirements requirements, ShaderArgs& inoutArgs);
// Adds SHADER_TARGET_AVAILABLE and SHADER_AVAILABLE_* macros, based on what is effective target level for current platform.
// e.g. even if shader source says only #pragma target 3.0 (SHADER_TARGET thus being 30), when compiled for DX11 it effectively turns
// into SM4.0 (since DX11 does not have anything below that), and SHADER_TARGET_AVAILABLE would be 40.
void AddAvailableShaderTargetRequirementsMacros(ShaderRequirements requirements, ShaderArgs& inoutArgs);


InlineSamplerType ParseInlineSamplerName(core::string samplerName, core::string& errorMsg);

// Returns true if given sampler name is matching the texture name. e.g. "sampler_Foo" matches texture "_Foo",
// and that means the sampler state will be taken from that texture.
bool CheckSamplerAndTextureNameMatch(const char* textureName, const char* samplerName);
inline bool CheckSamplerAndTextureNameMatch(const core::string& textureName, const core::string& samplerName)
{
    return CheckSamplerAndTextureNameMatch(textureName.c_str(), samplerName.c_str());
}

// Prefix source code that is added to all compiled shader snippets (automatic includes, some defines etc.)
core::string MakeShaderSourcePrefix(const ShaderCompileParams& params);

extern const char* kProgramTypeNames[kProgramCount];
extern const char* kProgramTypeShortNames[kProgramCount];
extern const char* kSCVertexCompNames[kVertexCompCount];
extern const char* kSCShaderChannelNames[kShaderChannelCount];
extern const char* kSCShaderChannelNames[kShaderChannelCount];
extern const char* kSCTextureDimensionNames[];

extern const char * kShaderCompPlatformDefines[];
#define kShaderCompPlatformDefineMobile "SHADER_API_MOBILE"
#define kShaderCompPlatformDefineDesktop "SHADER_API_DESKTOP"

struct CgTestReflectionImpl : public ShaderCompilerReflectionReport
{
    core::string bindings;
    core::string constants;
    core::string uavs;
    int statsAlu, statsTex, statsFlow, statsTempRegister;
    CgTestReflectionImpl() : statsAlu(0), statsTex(0), statsFlow(0), statsTempRegister(0) {}

    virtual void OnInputBinding(ShaderChannel dataChannel, VertexComponent inputSlot)
    {
        bindings += Format("Bind \"%s\" %s\n", kSCShaderChannelNames[dataChannel], kSCVertexCompNames[inputSlot]);
    }

    virtual void OnConstantBuffer(const char* name, int size, int varCount) {}
    virtual void OnConstant(const char* name, int bindIndex, ShaderParamType dataType, ConstantType constantType, int rows, int cols, int arraySize)
    {
        constants += Format("%s: %i dtype%i ktype%i %ix%i[%i]\n", name, bindIndex, dataType, constantType, rows, cols, arraySize);
    }

    virtual void OnCBBinding(const char* name, int bindIndex) {}
    virtual void OnTextureBinding(const char* name, int bindIndex, int samplerIndex, bool multisampled, TextureDimension dim)
    {
        bindings += Format("SetTexture %i [%s] %s\n", bindIndex, name, kSCTextureDimensionNames[dim]);
    }

    virtual void OnInlineSampler(InlineSamplerType sampler, int bindIndex) {}
    virtual void OnUAVBinding(const char* name, int bindIndex, int originalBindIndex)
    {
        uavs += Format("UAV %i [%s] orig: %i\n", bindIndex, name, originalBindIndex);
    }

    virtual void OnBufferBinding(const char* name, int bindIndex)
    {
        bindings += Format("SetBuffer %i %s\n", bindIndex, name);
    }

    virtual void OnStatsInfo(int alu, int tex, int flow, int tempRegister)
    {
        statsAlu = alu;
        statsTex = tex;
        statsFlow = flow;
        statsTempRegister = tempRegister;
    }
};

inline int CreateNumericUnityVersion(int version, int major, int minor)
{
    int unityVersionMajModifier = std::min(major, 9);
    int unityVersionMinModifier = std::min(minor, 9);
    int combinedVersion = version * 100 + unityVersionMajModifier * 10 + unityVersionMinModifier;
    return combinedVersion;
}
