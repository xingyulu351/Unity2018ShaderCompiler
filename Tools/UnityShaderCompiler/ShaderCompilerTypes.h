#pragma once

#include <stddef.h>
#include "Runtime/Utilities/EnumFlags.h"


enum ShaderCompilerProgram
{
    kProgramVertex = 0,
    kProgramFragment,
    kProgramHull,
    kProgramDomain,
    kProgramGeometry,
    kProgramSurface,
    kProgramCount
};

// Language that a shader snippet was written in
enum ShaderSourceLanguage
{
    kShaderSourceLanguageCg = 0,// HLSL/Cg ("CGPROGRAM") - translated into other languages if needed on various platforms
    kShaderSourceLanguageGLSL,  // GLSL - almost no processing, works only on OpenGL/ES
    kShaderSourceLanguageMetal, // Metal - almost no processing, works only on Metal
    kShaderSourceLanguageHLSL,  // HLSL ("HLSLPROGRAM") - same as Cg one, except some automatic #include files aren't added
    kShaderSourceLanguageCount,

    kShaderSourceLanguageNone = kShaderSourceLanguageCount
};

extern const char*  kShaderLangBeginTag[];
extern const size_t kShaderLangBeginTagLength[];

extern const char*  kShaderLangEndTag[];
extern const size_t kShaderLangEndTagLength[];

extern const char*  kShaderLangIncludeTag[];
extern const size_t kShaderLangIncludeTagLength[];


enum ShaderCompilationFlags
{
    kShaderCompFlags_None = 0,
    // Skip glsl-optimizer part. Only affects GLES2.0 backend now.
    kShaderCompFlags_SkipGLSLOptimize           = 1 << 0,
    // Legacy, used to be set by "#pragma debug" back when that was used for surface shader debugging.
    // A bunch of shaders in the wild still have them, do not overload to mean something else.
    kShaderCompFlags_LegacyEnableDebugInfo      = 1 << 1,
    // Enable output of DX11 shader debug info, and disable stripping. Note that this increases
    // compiled shader size massively; do not use by default.
    kShaderCompFlags_EnableDebugInfoD3D11       = 1 << 2,
    // Embed disassembled HLSLcc bytecode into translated source, for debugging compiler issues.
    // Only affects HLSLcc backends.
    kShaderCompFlags_EmbedHLSLccDisassembly     = 1 << 3,

    // Preferred shader compiler backend: HLSLcc
    kShaderCompFlags_PreferHLSLcc            = 1 << 4,
    // Preferred shader compiler backend: HLSL2GLSL + glsl-optimizer
    kShaderCompFlags_PreferHLSL2GLSL         = 1 << 5,

    // When set, UnityObjectToWorldNormal and UnityObjectToWorldDir do the same thing (not correct result in presence of non-uniform scale).
    kShaderCompFlags_AssumeUniformScaling       = 1 << 6,
    // Don't instance object matrices.
    kShaderCompFlags_DontInstanceObjectMatrices = 1 << 7,
    // LOD fade is not packed into per-instance data, therefore break batches.
    kShaderCompFlags_DontInstanceLODFade        = 1 << 8,
    // SH is not packed into per-instance data, therefore break batches
    kShaderCompFlags_DontInstanceSH             = 1 << 9,
    // LightmapST is not packed into per-instance data, therefore break batches
    kShaderCompFlags_DontInstanceLightmapST     = 1 << 10,
    // When set in a GLSL shader then generate a GLSL ES 1.0 (OpenGL ES 2.0) even when the shader target is OpenGL ES 3
    kShaderCompFlags_GLSL_ES2                   = 1 << 11,
    // Disables stripping. Used for compute shaders atm to retain same constant offsets for all kernels in a single shader file
    kShaderCompFlags_DisableStripping           = 1 << 12,
    // Do not translate to target lang on the compile command but use separate pass instead. Used for Metal compute shaders.
    kShaderCompFlags_UseSeparateTranslatePass   = 1 << 13,
    // Use nearest/point filtering for shadowmap lookups, instead of default linear/PCF
    kShaderCompFlags_MetalUsePointFilterForShadows = 1 << 14,
    // Do not add AddAvailableShaderTargetRequirementsMacros macros; they are already added by the caller plugin/backend
    kShaderCompFlags_DoNotAddShaderAvailableMacros = 1 << 15,
    // Compile for Nintendo Switch platform
    kShaderCompFlags_SwitchPlatform = 1 << 16,
    // Use PSSL2 over PSSL, PS4 only, set by #pragma pssl2
    kShaderCompFlags_PSSL2                      = 1 << 17,

    // Enable framebuffer fetch translation
    kShaderCompFlags_FramebufferFetch = 1 << 18,

    // Disable fastmath
    kShaderCompFlags_DisableFastMath = 1 << 19,
    kShaderCompFlags_InstanceRenderingLayer = 1 << 20,

    // Embed preprocessed HLSL shader source into translated source, for debugging of shader issues.
    // Only affects HLSLcc backends.
    kShaderCompFlags_EmbedHLSLccPreprocessedSource = 1 << 21,
};
ENUM_FLAGS(ShaderCompilationFlags);
