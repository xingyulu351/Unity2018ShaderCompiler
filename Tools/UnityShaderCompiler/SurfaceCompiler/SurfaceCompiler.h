#pragma once

// This is main functionality of surface shader (http://docs.unity3d.com/Manual/SL-SurfaceShaders.html) code generation.
// Given surface shader source snippet, this generates actual vertex/pixel shaders for forward/deferred passes etc.,
// ready to be pasted into shader instead of the original surface snippet. Later on that is compiled just like any regular shader.

#include "ShaderCompiler.h"

class ShaderImportErrors;


enum SurfaceMode
{
    kSurfaceModeDefault = 0,
    kSurfaceModeAlpha,
    kSurfaceModeAlphaTest,
    kSurfaceModeDecalAdd,
    kSurfaceModeDecalBlend,
};

enum SurfaceBlendMode
{
    kSurfaceBlendAuto = 0,
    kSurfaceBlendAlpha,
    kSurfaceBlendPremultiply,
};

struct SurfaceCompileOptions
{
    SurfaceCompileOptions()
        : mode(kSurfaceModeDefault)
        , blend(kSurfaceBlendAuto)
        , renderPaths(~0)
        , addShadow(false)
        , noShadow(false)
        , noFog(false)
        , softVegetation(false)
        , keepAlpha(false)
        , noLightmap(false)
        , noDirLightmap(false)
        , noDynLightmap(false)
        , noShadowMask(false)
        , noAmbient(false)
        , noVertexLights(false)
        , noForwardBase(false)
        , noForwardAdd(false)
        , noMeta(false)
        , noInstancing(false)
        , ditherCrossFade(false)
        , noLPPV(false)
        , fullForwardShadows(false)
        , dualForward(false)
        , halfAsViewDir(false)
        , interpolateView(false)
        , needsPBSLightingInclude(false)
        , needsLightingInclude(true)
        , instancingInForwardAdd(false)
        , noColorMask(false)
    {
    }

    bool HasTessellation() const { return !tessModifier.empty(); }
    // "final color modifier" is meant for custom fog; so when we have it
    // then don't add built-in fog.
    bool HasFog() const { return !noFog && finalColorModifier.empty(); }
    bool HasInstancing() const { return !noInstancing; }
    bool AddDitherCrossFade() const { return ditherCrossFade; }

    core::string pragmaDirectives;
    core::string lighting;
    SurfaceMode mode;
    SurfaceBlendMode blend;
    core::string alphaTestVariable;
    core::string finalColorModifier;
    core::string finalPrepassModifier;
    core::string finalGBufferModifier;
    core::string tessModifier;
    core::string tessPhongAlpha;
    unsigned int renderPaths;
    bool addShadow;
    bool noShadow;
    bool noFog;
    bool softVegetation;
    bool keepAlpha;
    bool noLightmap;
    bool noDirLightmap;
    bool noDynLightmap;
    bool noShadowMask;
    bool noAmbient;
    bool noVertexLights;
    bool noForwardBase; // only used for tests
    bool noForwardAdd;
    bool noMeta; // don't generate meta (albedo/emission extraction etc.) pass
    bool noInstancing; // don't generate instancing variant
    bool ditherCrossFade;
    bool noLPPV;
    bool fullForwardShadows;
    bool dualForward;
    bool halfAsViewDir;
    bool interpolateView;
    bool needsPBSLightingInclude;
    bool needsLightingInclude;
    bool instancingInForwardAdd;
    bool noColorMask;
};


struct SurfaceParams
{
    SurfaceParams()
        : shaderAPIs(0)
        , startLine(0)
        , analysisCacheRequests(0)
        , astAnalysisCacheRequests(0)
        , analysisCacheHits(0)
        , astAnalysisCacheHits(0)
    {
    }

    ShaderCompileParams p;
    SurfaceCompileOptions o;
    core::string source;
    IncludeSearchContext includeContext;
    unsigned int shaderAPIs;
    ShaderCompileRequirements target;
    int startLine;

    // stats for variant analysis cache; exposed for tests
    size_t analysisCacheRequests, astAnalysisCacheRequests;
    size_t analysisCacheHits, astAnalysisCacheHits;
};


void ProcessSurfaceSnippet(SurfaceParams& params, core::string& result, ShaderImportErrors& outErrors, ShaderCompilerPreprocessCallbacks* callbacks);
void ProcessSurfaceParams(SurfaceParams& params);
void OutputGeneratedSnippet(const core::string& variantsStringIn, core::string& result, const core::string& gen, const SurfaceParams& params, unsigned shaderAPIs, ShaderImportErrors& outErrors);
core::string AddNoVariantsDirectivesToLastMulticompile(const core::string& input, const SurfaceParams& params);
