#pragma once

#include "SurfaceData.h"
#include "../ShaderCompiler.h"
#include <string>
#include <vector>

class ShaderImportErrors;

// Parses user-written source code of a surface shader, and finds out various things from the AST of
// the source; as well as does basic sanity checking & validation (whether surface lighting function
// has expected number of arguments etc.); to produce human understandable errors. Otherwise errors
// would still be produced later on when compiling the final generated code, but at that point it's
// much harder to understand what the user-written code did wrong.
void AnalyseSurfaceShaderAST(
    const core::string& gen, const ShaderArgs& macroArgs,
    const core::string& entryPoint, const core::string& vertexEntryPoint,
    const core::string& finalColorEntryPoint, const core::string& finalPrepassEntryPoint, const core::string& finalGBufferEntryPoint,
    const core::string& tessModifier,
    const IncludePaths& includePaths,
    SurfaceFunctionDesc& outDesc, LightingModels& outLightModels, ShaderImportErrors& outErrors,
    SurfaceAnalysisCache& analysisCache);

struct ShaderDesc
{
    MemberDeclarations inputs;
    MemberDeclarations outputs;
};

enum LightingFunctionPostfix
{
    kNone = 0,
    kGI,
    kPrePass,
    kDeferred,
    kSingleLightmap,
    kDualLightmap,
    kDirLightmap
};

static const char* LightingFunctionPostfixNames[] =
{
    "",
    "_GI",
    "_PrePass",
    "_Deferred",
    "_SingleLightmap",
    "_DualLightmap",
    "_DirLightmap"
};

CompileTimeAssert(ARRAY_SIZE(LightingFunctionPostfixNames) == kDirLightmap + 1, "The translation table for 'LightingFunctionPostfixNames' needs to be updated.");

// Given a shader, figures out what input & output variables are *actually* read/written by it.
// This is used in surface shader code analysis, that drives the later codegen step from SurfaceData.
void AnalyzeSurfaceShaderViaCompile(const core::string& gen, const core::string& entryPoint, int startLine, const IncludePaths& includePaths, const ShaderCompilerPlatform api, const ShaderArgs& args, ShaderDesc& out, ShaderImportErrors& outErrors, SurfaceAnalysisCache& analysisCache);

#define kSurfaceAnalysisHLSLConstantBuffer "SurfShaderGlobals"
