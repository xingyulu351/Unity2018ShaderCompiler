#pragma once
#include <string>
#include <vector>
#include "../CgBatch.h"
#include "Editor/Src/AssetPipeline/ShaderImporting/ShaderCompileRequirements.h"
#include "Editor/Src/AssetPipeline/ShaderImporting/ShaderVariantEnumeration.h"

class ShaderImportErrors;


enum RenderPath
{
    kPathForward,
    kPathPrePass,
    kPathDeferred,
    kPathDirLightmap,
};

enum
{
    kShaderPlatformsHaveGLSLBlocks = (1 << kShaderCompPlatformGLES20) | (1 << kShaderCompPlatformGLES3Plus) | (1 << kShaderCompPlatformOpenGLCore),
    kShaderPlatformNoMRT = (1 << kShaderCompPlatformPSP2_Obsolete),
    kShaderPlatformFramebufferFetch = (1 << kShaderCompPlatformGLES20) | (1 << kShaderCompPlatformGLES3Plus) | (1 << kShaderCompPlatformMetal),
};

int FindShaderPlatforms(const core::string& name);
const core::string GetShaderPlatformName(ShaderCompilerPlatform api);

enum { kNumBuiltinCompileVariants = 9 };
extern const char* const kBuiltinCompileVariants[kNumBuiltinCompileVariants];

class ShaderCompileVariants
{
public:
    void FindVariantDirectives(const core::string& codeString, bool proceduralInstancing, ShaderImportErrors& outErrors);

    size_t PrepareEnumeration(ShaderCompilerProgram type)
    {
        return m_Enum.PrepareEnumeration(m_Data, type);
    }

    void Enumerate(UnityStrArray& outKeywords)
    {
        m_Enum.Enumerate(outKeywords);
    }

    void GetKeywordLists(UnityStrArray& outAlwaysIncludedUserKeywords, UnityStrArray& outBuiltinKeywords) const
    {
        m_Data.GetKeywordLists(outAlwaysIncludedUserKeywords, outBuiltinKeywords);
    }

    const ShaderVariantData& GetData() const { return m_Data; }

private:
    void FindUserCompileVariantDirectives(bool oldMultiCompileSyntax, const core::string& codeString, ShaderCompilerProgram type, ShaderImportErrors& outErrors);
    void FindUserCompileVariantShortcuts(const core::string& codeString, ShaderImportErrors& outErrors, const core::string& pragmaName, const core::string& variants);
    void AddSingleUserMultiCompile(bool oldMultiCompileSyntax, ShaderCompilerProgram type, const core::string& line, ShaderImportErrors& outErrors, bool& inoutOk);
    void SkipUserVariants(const UnityStrArray& skipVariants);

protected:
    ShaderVariantData m_Data;
    ShaderVariantEnumeration m_Enum;
};

void ExtractIncludePortions(core::string& inputSource, const core::string& tokenStart, const core::string& tokenEnd, core::string& output, ShaderImportErrors& outErrors);
void HoistGLSLVersionDirective(core::string& source);

ShaderGpuProgramType GetGLSLGpuProgramType(ShaderRequirements requirements, ShaderCompilerPlatform platform);
