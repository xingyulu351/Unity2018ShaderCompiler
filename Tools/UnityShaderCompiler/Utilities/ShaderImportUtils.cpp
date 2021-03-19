#include "UnityPrefix.h"
#include "ShaderImportUtils.h"
#include "ShaderImportErrors.h"
#include "Runtime/Utilities/Word.h"
#include "Runtime/Utilities/ArrayUtility.h"
#include "Editor/Src/Utility/TextUtilities.h"

using core::string;
using std::vector;

#include "APIPlugin.h"

// ----------------------------------------------------------------------------------------------

const char * kShaderCompPlatformNames[] = {SHADER_COMPILER_PLATFORM_NAMES_IMPL};
CompileTimeAssert(ARRAY_SIZE(kShaderCompPlatformNames) == kShaderCompPlatformCount, "SHADER COMPILER ERROR: 'kShaderCompPlatformNames' table needs to be updated.");

int FindShaderPlatforms(const core::string& name)
{
    for (int i = 0; i < kShaderCompPlatformCount; ++i)
    {
        if (StrIEquals(kShaderCompPlatformNames[i], name))
            return (1 << i);
    }

    if (name == "shaderonly")
        return kValidShaderCompPlatformMask; // used to do something, not anymore - means "all platforms" now
    if (name == "noshadows")
        return 0; // used to do something, not anymore
    if (name == "noprepass")
        return 0; // used to do something, not anymore
    if (name == "nomrt")
        return kShaderPlatformNoMRT;
    if (name == "fbfetch" || name == "framebufferfetch")
        return kShaderPlatformFramebufferFetch;

    return -1;
}

const core::string GetShaderPlatformName(ShaderCompilerPlatform api)
{
    Assert(api >= 0);
    if (api < kShaderCompPlatformCount)
    {
        return kShaderCompPlatformNames[api];
    }
    return core::string("");
}

// ----------------------------------------------------------------------------------------------

const char* const kBuiltinCompileVariants[] =
{
    "multi_compile_fwdadd_fullshadows",
    "multi_compile_fwdadd",
    "multi_compile_fwdbasealpha",
    "multi_compile_fwdbase",
    "multi_compile_lightpass",
    "multi_compile_shadowcaster",
    "multi_compile_shadowcollector",
    "multi_compile_prepassfinal",
    "multi_compile_particles",
};
CompileTimeAssertArraySize(kBuiltinCompileVariants, kNumBuiltinCompileVariants);


static const char* kNoKeyword = "__";

static const char* litSpot = "SPOT";
static const char* litDir = "DIRECTIONAL";
static const char* litDirCk = "DIRECTIONAL_COOKIE";
static const char* litPoint = "POINT";
static const char* litPointCk = "POINT_COOKIE";

static const char* shPrefix = "SHADOWS_";
static const char* shDepth = "SHADOWS_DEPTH";
static const char* shScreen = "SHADOWS_SCREEN";
static const char* shCube = "SHADOWS_CUBE";
static const char* shSoft = "SHADOWS_SOFT";
static const char* shSplitSpheres = "SHADOWS_SPLIT_SPHERES";
static const char* shSingleCascade = "SHADOWS_SINGLE_CASCADE";
static const char* shShadowMask = "SHADOWS_SHADOWMASK";

static const char* lmOn = "LIGHTMAP_ON";
static const char* lmShadowMixing = "LIGHTMAP_SHADOW_MIXING";

static const char* dirLmCom = "DIRLIGHTMAP_COMBINED";

static const char* dynLmOn = "DYNAMICLIGHTMAP_ON";

static const char* lightProbeSH = "LIGHTPROBE_SH";

static const char* vertexOn = "VERTEXLIGHT_ON";

static const char* softPartsOn = "SOFTPARTICLES_ON";

static const char* kHDROn = "UNITY_HDR_ON";

static const char* fogKeywords = "__ FOG_LINEAR FOG_EXP FOG_EXP2";
static const char* instancingKeywords = "__ INSTANCING_ON";
static const char* proceduralInstancingKeywords = "__ INSTANCING_ON PROCEDURAL_INSTANCING_ON";


const int kLightPassCategories = 5;
static const char* kBuiltinLightPass[] =
{
    // no shadows, all light types
    litPoint    , 0       , 0     , 0           , 0,
    litDir      , 0       , 0     , 0           , 0,
    litSpot     , 0       , 0     , 0           , 0,
    litPointCk  , 0       , 0     , 0           , 0,
    litDirCk    , 0       , 0     , 0           , 0,

    // only shadow mask
    litPoint    , 0       , 0     , shShadowMask, 0,
    litDir      , 0       , 0     , shShadowMask, 0,
    litSpot     , 0       , 0     , shShadowMask, 0,
    litPointCk  , 0       , 0     , shShadowMask, 0,
    litDirCk    , 0       , 0     , shShadowMask, 0,
    litPoint    , 0       , 0     , shShadowMask, lmShadowMixing,
    litDir      , 0       , 0     , shShadowMask, lmShadowMixing,
    litSpot     , 0       , 0     , shShadowMask, lmShadowMixing,
    litPointCk  , 0       , 0     , shShadowMask, lmShadowMixing,
    litDirCk    , 0       , 0     , shShadowMask, lmShadowMixing,

    // spot + shadows
    litSpot     , shDepth , 0     , 0           , 0,
    litSpot     , shDepth , shSoft, 0           , 0,
    litSpot     , shDepth , 0     , 0           , lmShadowMixing,
    litSpot     , shDepth , shSoft, 0           , lmShadowMixing,
    litSpot     , shDepth , 0     , shShadowMask, 0,
    litSpot     , shDepth , shSoft, shShadowMask, 0,
    litSpot     , shDepth , 0     , shShadowMask, lmShadowMixing,
    litSpot     , shDepth , shSoft, shShadowMask, lmShadowMixing,
    // directional + shadows
    litDir      , shScreen, 0     , 0           , 0,
    litDirCk    , shScreen, 0     , 0           , 0,
    litDir      , shScreen, 0     , 0           , lmShadowMixing,
    litDirCk    , shScreen, 0     , 0           , lmShadowMixing,
    litDir      , shScreen, 0     , shShadowMask, 0,
    litDirCk    , shScreen, 0     , shShadowMask, 0,
    litDir      , shScreen, 0     , shShadowMask, lmShadowMixing,
    litDirCk    , shScreen, 0     , shShadowMask, lmShadowMixing,
    // point + shadows
    litPoint    , shCube  , 0     , 0           , 0,
    litPoint    , shCube  , shSoft, 0           , 0,
    litPoint    , shCube  , 0     , 0           , lmShadowMixing,
    litPoint    , shCube  , shSoft, 0           , lmShadowMixing,
    litPoint    , shCube  , 0     , shShadowMask, 0,
    litPoint    , shCube  , shSoft, shShadowMask, 0,
    litPoint    , shCube  , 0     , shShadowMask, lmShadowMixing,
    litPoint    , shCube  , shSoft, shShadowMask, lmShadowMixing,
    litPointCk  , shCube  , 0     , 0           , 0,
    litPointCk  , shCube  , shSoft, 0           , 0,
    litPointCk  , shCube  , 0     , 0           , lmShadowMixing,
    litPointCk  , shCube  , shSoft, 0           , lmShadowMixing,
    litPointCk  , shCube  , 0     , shShadowMask, 0,
    litPointCk  , shCube  , shSoft, shShadowMask, 0,
    litPointCk  , shCube  , 0     , shShadowMask, lmShadowMixing,
    litPointCk  , shCube  , shSoft, shShadowMask, lmShadowMixing,

    0, 0, 0, 0, 0
};


static const char* kBuiltinForwardAdd[] =
{
    litPoint,
    litDir,
    litSpot,
    litPointCk,
    litDirCk,
    0
};

const int kFwdBaseCategories = 8;
static const char* kBuiltinForwardBaseVertex[] =
{
    // no shadows
    litDir      , 0        , 0         , 0         , 0         , 0                 , 0                 , 0             ,
    litDir      , 0        , 0         , 0         , 0         , 0                 , 0                 , lightProbeSH  ,
    litDir      , 0        , 0         , 0         , 0         , shShadowMask      , 0                 , lightProbeSH  ,
    litDir      , 0        , 0         , dynLmOn   , 0         , 0                 , 0                 , 0             ,
    litDir      , lmOn     , 0         , 0         , 0         , 0                 , 0                 , 0             ,
    litDir      , lmOn     , 0         , dynLmOn   , 0         , 0                 , 0                 , 0             ,
    litDir      , lmOn     , 0         , 0         , 0         , 0                 , 0                 , lightProbeSH  ,
    litDir      , lmOn     , 0         , 0         , 0         , lmShadowMixing    , 0                 , 0             ,
    litDir      , lmOn     , 0         , dynLmOn   , 0         , lmShadowMixing    , 0                 , 0             ,
    litDir      , lmOn     , 0         , 0         , 0         , lmShadowMixing    , 0                 , lightProbeSH  ,

    litDir      , 0        , dirLmCom  , dynLmOn   , 0         , 0                 , 0                 , 0             ,
    litDir      , lmOn     , dirLmCom  , 0         , 0         , 0                 , 0                 , 0             ,
    litDir      , lmOn     , dirLmCom  , 0         , 0         , 0                 , 0                 , lightProbeSH  ,
    litDir      , lmOn     , dirLmCom  , dynLmOn   , 0         , 0                 , 0                 , 0             ,
    litDir      , lmOn     , dirLmCom  , 0         , 0         , lmShadowMixing    , 0                 , 0             ,
    litDir      , lmOn     , dirLmCom  , 0         , 0         , lmShadowMixing    , 0                 , lightProbeSH  ,
    litDir      , lmOn     , dirLmCom  , dynLmOn   , 0         , lmShadowMixing    , 0                 , 0             ,

    // only shadow mask
    litDir      , lmOn     , 0         , 0         , 0         , shShadowMask      , 0                 , 0             ,
    litDir      , lmOn     , 0         , dynLmOn   , 0         , shShadowMask      , 0                 , 0             ,
    litDir      , lmOn     , 0         , 0         , 0         , shShadowMask      , 0                 , lightProbeSH  ,
    litDir      , lmOn     , dirLmCom  , 0         , 0         , shShadowMask      , 0                 , 0             ,
    litDir      , lmOn     , dirLmCom  , dynLmOn   , 0         , shShadowMask      , 0                 , 0             ,
    litDir      , lmOn     , dirLmCom  , 0         , 0         , shShadowMask      , 0                 , lightProbeSH  ,
    litDir      , lmOn     , 0         , 0         , 0         , shShadowMask      , lmShadowMixing    , 0             ,
    litDir      , lmOn     , 0         , dynLmOn   , 0         , shShadowMask      , lmShadowMixing    , 0             ,
    litDir      , lmOn     , 0         , 0         , 0         , shShadowMask      , lmShadowMixing    , lightProbeSH  ,
    litDir      , lmOn     , dirLmCom  , 0         , 0         , shShadowMask      , lmShadowMixing    , 0             ,
    litDir      , lmOn     , dirLmCom  , dynLmOn   , 0         , shShadowMask      , lmShadowMixing    , 0             ,
    litDir      , lmOn     , dirLmCom  , 0         , 0         , shShadowMask      , lmShadowMixing    , lightProbeSH  ,

    // shadows
    litDir      , 0        , 0         , 0         , shScreen  , 0                 , 0                 , 0             ,
    litDir      , 0        , 0         , 0         , shScreen  , 0                 , 0                 , lightProbeSH  ,
    litDir      , 0        , 0         , dynLmOn   , shScreen  , 0                 , 0                 , 0             ,
    litDir      , 0        , 0         , 0         , shScreen  , 0                 , lmShadowMixing    , lightProbeSH  ,
    litDir      , 0        , 0         , dynLmOn   , shScreen  , 0                 , lmShadowMixing    , 0             ,
    litDir      , 0        , 0         , 0         , shScreen  , shShadowMask      , 0                 , lightProbeSH  ,
    litDir      , 0        , 0         , dynLmOn   , shScreen  , shShadowMask      , 0                 , 0             ,
    litDir      , 0        , 0         , 0         , shScreen  , shShadowMask      , lmShadowMixing    , lightProbeSH  ,
    litDir      , 0        , 0         , dynLmOn   , shScreen  , shShadowMask      , lmShadowMixing    , 0             ,
    litDir      , lmOn     , 0         , 0         , shScreen  , 0                 , 0                 , 0             ,
    litDir      , lmOn     , 0         , dynLmOn   , shScreen  , 0                 , 0                 , 0             ,
    litDir      , lmOn     , 0         , 0         , shScreen  , 0                 , 0                 , lightProbeSH  ,
    litDir      , lmOn     , 0         , 0         , shScreen  , lmShadowMixing    , 0                 , 0             ,
    litDir      , lmOn     , 0         , dynLmOn   , shScreen  , lmShadowMixing    , 0                 , 0             ,
    litDir      , lmOn     , 0         , 0         , shScreen  , lmShadowMixing    , 0                 , lightProbeSH  ,
    litDir      , lmOn     , 0         , 0         , shScreen  , shShadowMask      , 0                 , 0             ,
    litDir      , lmOn     , 0         , dynLmOn   , shScreen  , shShadowMask      , 0                 , 0             ,
    litDir      , lmOn     , 0         , 0         , shScreen  , shShadowMask      , 0                 , lightProbeSH  ,
    litDir      , lmOn     , 0         , 0         , shScreen  , shShadowMask      , lmShadowMixing    , 0             ,
    litDir      , lmOn     , 0         , dynLmOn   , shScreen  , shShadowMask      , lmShadowMixing    , 0             ,
    litDir      , lmOn     , 0         , 0         , shScreen  , shShadowMask      , lmShadowMixing    , lightProbeSH  ,

    litDir      , 0        , dirLmCom  , dynLmOn   , shScreen  , 0                 , 0                 , 0             ,
    litDir      , lmOn     , dirLmCom  , 0         , shScreen  , 0                 , 0                 , 0             ,
    litDir      , lmOn     , dirLmCom  , 0         , shScreen  , 0                 , 0                 , lightProbeSH  ,
    litDir      , lmOn     , dirLmCom  , dynLmOn   , shScreen  , 0                 , 0                 , 0             ,
    litDir      , lmOn     , dirLmCom  , 0         , shScreen  , lmShadowMixing    , 0                 , 0             ,
    litDir      , lmOn     , dirLmCom  , 0         , shScreen  , lmShadowMixing    , 0                 , lightProbeSH  ,
    litDir      , lmOn     , dirLmCom  , dynLmOn   , shScreen  , lmShadowMixing    , 0                 , 0             ,
    litDir      , lmOn     , dirLmCom  , 0         , shScreen  , shShadowMask      , 0                 , 0             ,
    litDir      , lmOn     , dirLmCom  , 0         , shScreen  , shShadowMask      , 0                 , lightProbeSH  ,
    litDir      , lmOn     , dirLmCom  , dynLmOn   , shScreen  , shShadowMask      , 0                 , 0             ,
    litDir      , lmOn     , dirLmCom  , 0         , shScreen  , shShadowMask      , lmShadowMixing    , 0             ,
    litDir      , lmOn     , dirLmCom  , 0         , shScreen  , shShadowMask      , lmShadowMixing    , lightProbeSH  ,
    litDir      , lmOn     , dirLmCom  , dynLmOn   , shScreen  , shShadowMask      , lmShadowMixing    , 0             ,

    // no shadows, +4 vertex lights
    litDir      , vertexOn , 0         , 0         , 0         , 0                 , 0                 , 0             ,
    litDir      , vertexOn , 0         , 0         , 0         , 0                 , 0                 , lightProbeSH  ,
    litDir      , vertexOn , 0         , dynLmOn   , 0         , 0                 , 0                 , 0             ,
    litDir      , vertexOn , dirLmCom  , dynLmOn   , 0         , 0                 , 0                 , 0             ,

    // shadows, +4 vertex lights
    litDir      , vertexOn , 0         , 0         , shScreen  , 0                 , 0                 , 0             ,
    litDir      , vertexOn , 0         , 0         , shScreen  , 0                 , 0                 , lightProbeSH  ,
    litDir      , vertexOn , 0         , dynLmOn   , shScreen  , 0                 , 0                 , 0             ,
    litDir      , vertexOn , dirLmCom  , dynLmOn   , shScreen  , 0                 , 0                 , 0             ,
    litDir      , vertexOn , 0         , 0         , shScreen  , 0                 , lmShadowMixing    , 0             ,
    litDir      , vertexOn , 0         , 0         , shScreen  , 0                 , lmShadowMixing    , lightProbeSH  ,
    litDir      , vertexOn , 0         , dynLmOn   , shScreen  , 0                 , lmShadowMixing    , 0             ,
    litDir      , vertexOn , dirLmCom  , dynLmOn   , shScreen  , 0                 , lmShadowMixing    , 0             ,
    litDir      , vertexOn , 0         , 0         , shScreen  , shShadowMask      , 0                 , 0             ,
    litDir      , vertexOn , 0         , 0         , shScreen  , shShadowMask      , 0                 , lightProbeSH  ,
    litDir      , vertexOn , 0         , dynLmOn   , shScreen  , shShadowMask      , 0                 , 0             ,
    litDir      , vertexOn , dirLmCom  , dynLmOn   , shScreen  , shShadowMask      , 0                 , 0             ,
    litDir      , vertexOn , 0         , 0         , shScreen  , shShadowMask      , lmShadowMixing    , 0             ,
    litDir      , vertexOn , 0         , 0         , shScreen  , shShadowMask      , lmShadowMixing    , lightProbeSH  ,
    litDir      , vertexOn , 0         , dynLmOn   , shScreen  , shShadowMask      , lmShadowMixing    , 0             ,
    litDir      , vertexOn , dirLmCom  , dynLmOn   , shScreen  , shShadowMask      , lmShadowMixing    , 0             ,

    0           , 0        , 0         , 0         , 0         , 0                 , 0                 , 0             ,
};

static const char* kBuiltinForwardBaseFragment[] =
{
    // no shadows
    litDir      , 0     , 0         , 0         , 0         , 0                 , 0                 , 0             ,
    litDir      , 0     , 0         , 0         , 0         , 0                 , 0                 , lightProbeSH  ,
    litDir      , 0     , 0         , 0         , 0         , shShadowMask      , 0                 , lightProbeSH  ,
    litDir      , 0     , 0         , dynLmOn   , 0         , 0                 , 0                 , 0             ,
    litDir      , lmOn  , 0         , 0         , 0         , 0                 , 0                 , 0             ,
    litDir      , lmOn  , 0         , dynLmOn   , 0         , 0                 , 0                 , 0             ,
    litDir      , lmOn  , 0         , 0         , 0         , 0                 , 0                 , lightProbeSH  ,
    litDir      , lmOn  , 0         , 0         , 0         , lmShadowMixing    , 0                 , 0             ,
    litDir      , lmOn  , 0         , dynLmOn   , 0         , lmShadowMixing    , 0                 , 0             ,
    litDir      , lmOn  , 0         , 0         , 0         , lmShadowMixing    , 0                 , lightProbeSH  ,

    litDir      , 0     , dirLmCom  , dynLmOn   , 0         , 0                 , 0                 , 0             ,
    litDir      , lmOn  , dirLmCom  , 0         , 0         , 0                 , 0                 , 0             ,
    litDir      , lmOn  , dirLmCom  , 0         , 0         , 0                 , 0                 , lightProbeSH  ,
    litDir      , lmOn  , dirLmCom  , dynLmOn   , 0         , 0                 , 0                 , 0             ,
    litDir      , lmOn  , dirLmCom  , 0         , 0         , lmShadowMixing    , 0                 , 0             ,
    litDir      , lmOn  , dirLmCom  , 0         , 0         , lmShadowMixing    , 0                 , lightProbeSH  ,
    litDir      , lmOn  , dirLmCom  , dynLmOn   , 0         , lmShadowMixing    , 0                 , 0             ,

    // only shadow mask
    litDir      , lmOn  , 0         , 0         , 0         , shShadowMask      , 0                 , 0             ,
    litDir      , lmOn  , 0         , dynLmOn   , 0         , shShadowMask      , 0                 , 0             ,
    litDir      , lmOn  , 0         , 0         , 0         , shShadowMask      , 0                 , lightProbeSH  ,
    litDir      , lmOn  , dirLmCom  , 0         , 0         , shShadowMask      , 0                 , 0             ,
    litDir      , lmOn  , dirLmCom  , dynLmOn   , 0         , shShadowMask      , 0                 , 0             ,
    litDir      , lmOn  , dirLmCom  , 0         , 0         , shShadowMask      , 0                 , lightProbeSH  ,
    litDir      , lmOn  , 0         , 0         , 0         , shShadowMask      , lmShadowMixing    , 0             ,
    litDir      , lmOn  , 0         , dynLmOn   , 0         , shShadowMask      , lmShadowMixing    , 0             ,
    litDir      , lmOn  , 0         , 0         , 0         , shShadowMask      , lmShadowMixing    , lightProbeSH  ,
    litDir      , lmOn  , dirLmCom  , 0         , 0         , shShadowMask      , lmShadowMixing    , 0             ,
    litDir      , lmOn  , dirLmCom  , dynLmOn   , 0         , shShadowMask      , lmShadowMixing    , 0             ,
    litDir      , lmOn  , dirLmCom  , 0         , 0         , shShadowMask      , lmShadowMixing    , lightProbeSH  ,

    // shadows
    litDir      , 0     , 0         , 0         , shScreen  , 0                 , 0                 , 0             ,
    litDir      , 0     , 0         , 0         , shScreen  , 0                 , 0                 , lightProbeSH  ,
    litDir      , 0     , 0         , dynLmOn   , shScreen  , 0                 , 0                 , 0             ,
    litDir      , 0     , 0         , 0         , shScreen  , 0                 , lmShadowMixing    , lightProbeSH  ,
    litDir      , 0     , 0         , dynLmOn   , shScreen  , 0                 , lmShadowMixing    , 0             ,
    litDir      , 0     , 0         , 0         , shScreen  , shShadowMask      , 0                 , lightProbeSH  ,
    litDir      , 0     , 0         , dynLmOn   , shScreen  , shShadowMask      , 0                 , 0             ,
    litDir      , 0     , 0         , 0         , shScreen  , shShadowMask      , lmShadowMixing    , lightProbeSH  ,
    litDir      , 0     , 0         , dynLmOn   , shScreen  , shShadowMask      , lmShadowMixing    , 0             ,
    litDir      , lmOn  , 0         , 0         , shScreen  , 0                 , 0                 , 0             ,
    litDir      , lmOn  , 0         , dynLmOn   , shScreen  , 0                 , 0                 , 0             ,
    litDir      , lmOn  , 0         , 0         , shScreen  , 0                 , 0                 , lightProbeSH  ,
    litDir      , lmOn  , 0         , 0         , shScreen  , lmShadowMixing    , 0                 , 0             ,
    litDir      , lmOn  , 0         , dynLmOn   , shScreen  , lmShadowMixing    , 0                 , 0             ,
    litDir      , lmOn  , 0         , 0         , shScreen  , lmShadowMixing    , 0                 , lightProbeSH  ,
    litDir      , lmOn  , 0         , 0         , shScreen  , shShadowMask      , 0                 , 0             ,
    litDir      , lmOn  , 0         , dynLmOn   , shScreen  , shShadowMask      , 0                 , 0             ,
    litDir      , lmOn  , 0         , 0         , shScreen  , shShadowMask      , 0                 , lightProbeSH  ,
    litDir      , lmOn  , 0         , 0         , shScreen  , shShadowMask      , lmShadowMixing    , 0             ,
    litDir      , lmOn  , 0         , dynLmOn   , shScreen  , shShadowMask      , lmShadowMixing    , 0             ,
    litDir      , lmOn  , 0         , 0         , shScreen  , shShadowMask      , lmShadowMixing    , lightProbeSH  ,

    litDir      , 0     , dirLmCom  , dynLmOn   , shScreen  , 0                 , 0                 , 0             ,
    litDir      , lmOn  , dirLmCom  , 0         , shScreen  , 0                 , 0                 , 0             ,
    litDir      , lmOn  , dirLmCom  , 0         , shScreen  , 0                 , 0                 , lightProbeSH  ,
    litDir      , lmOn  , dirLmCom  , dynLmOn   , shScreen  , 0                 , 0                 , 0             ,
    litDir      , lmOn  , dirLmCom  , 0         , shScreen  , lmShadowMixing    , 0                 , 0             ,
    litDir      , lmOn  , dirLmCom  , 0         , shScreen  , lmShadowMixing    , 0                 , lightProbeSH  ,
    litDir      , lmOn  , dirLmCom  , dynLmOn   , shScreen  , lmShadowMixing    , 0                 , 0             ,
    litDir      , lmOn  , dirLmCom  , 0         , shScreen  , shShadowMask      , 0                 , 0             ,
    litDir      , lmOn  , dirLmCom  , 0         , shScreen  , shShadowMask      , 0                 , lightProbeSH  ,
    litDir      , lmOn  , dirLmCom  , dynLmOn   , shScreen  , shShadowMask      , 0                 , 0             ,
    litDir      , lmOn  , dirLmCom  , 0         , shScreen  , shShadowMask      , lmShadowMixing    , 0             ,
    litDir      , lmOn  , dirLmCom  , 0         , shScreen  , shShadowMask      , lmShadowMixing    , lightProbeSH  ,
    litDir      , lmOn  , dirLmCom  , dynLmOn   , shScreen  , shShadowMask      , lmShadowMixing    , 0             ,

    0           , 0     , 0         , 0         , 0         , 0                 , 0                 , 0             ,
};

const int kFwdBaseAlphaCategories = 6;
static const char* kBuiltinForwardBaseAlphaVertex[] =
{
    litDir      , 0     , 0         , 0         , 0                 ,   0               ,
    litDir      , 0     , 0         , 0         , 0                 ,   lightProbeSH    ,
    litDir      , 0     , 0         , dynLmOn   , 0                 ,   0               ,
    litDir      , lmOn  , 0         , 0         , 0                 ,   0               ,
    litDir      , lmOn  , 0         , 0         , 0                 ,   lightProbeSH    ,
    litDir      , lmOn  , 0         , dynLmOn   , 0                 ,   0               ,
    litDir      , lmOn  , 0         , 0         , lmShadowMixing    ,   0               ,
    litDir      , lmOn  , 0         , 0         , lmShadowMixing    ,   lightProbeSH    ,
    litDir      , lmOn  , 0         , dynLmOn   , lmShadowMixing    ,   0               ,

    litDir      , 0     , dirLmCom  , dynLmOn   , 0                 ,   0               ,
    litDir      , lmOn  , dirLmCom  , 0         , 0                 ,   0               ,
    litDir      , lmOn  , dirLmCom  , 0         , 0                 ,   lightProbeSH    ,
    litDir      , lmOn  , dirLmCom  , dynLmOn   , 0                 ,   0               ,
    litDir      , lmOn  , dirLmCom  , 0         , lmShadowMixing    ,   0               ,
    litDir      , lmOn  , dirLmCom  , 0         , lmShadowMixing    ,   lightProbeSH    ,
    litDir      , lmOn  , dirLmCom  , dynLmOn   , lmShadowMixing    ,   0               ,

    litDir      , 0     , 0         , 0         , vertexOn          ,   lightProbeSH    ,
    litDir      , 0     , 0         , dynLmOn   , vertexOn          ,   0               ,
    litDir      , 0     , dirLmCom  , dynLmOn   , vertexOn          ,   0               ,

    0           , 0     , 0         , 0         , 0                 ,   0               ,
};
static const char* kBuiltinForwardBaseAlphaFragment[] =
{
    litDir      , 0     , 0         , 0         , 0                 ,   0               ,
    litDir      , 0     , 0         , 0         , 0                 ,   lightProbeSH    ,
    litDir      , 0     , 0         , dynLmOn   , 0                 ,   0               ,
    litDir      , lmOn  , 0         , 0         , 0                 ,   0               ,
    litDir      , lmOn  , 0         , 0         , 0                 ,   lightProbeSH    ,
    litDir      , lmOn  , 0         , dynLmOn   , 0                 ,   0               ,
    litDir      , lmOn  , 0         , 0         , lmShadowMixing    ,   0               ,
    litDir      , lmOn  , 0         , 0         , lmShadowMixing    ,   lightProbeSH    ,
    litDir      , lmOn  , 0         , dynLmOn   , lmShadowMixing    ,   0               ,

    litDir      , 0     , dirLmCom  , dynLmOn   , 0                 ,   0               ,
    litDir      , lmOn  , dirLmCom  , 0         , 0                 ,   0               ,
    litDir      , lmOn  , dirLmCom  , 0         , 0                 ,   lightProbeSH    ,
    litDir      , lmOn  , dirLmCom  , dynLmOn   , 0                 ,   0               ,
    litDir      , lmOn  , dirLmCom  , 0         , lmShadowMixing    ,   0               ,
    litDir      , lmOn  , dirLmCom  , 0         , lmShadowMixing    ,   lightProbeSH    ,
    litDir      , lmOn  , dirLmCom  , dynLmOn   , lmShadowMixing    ,   0               ,

    0           , 0     , 0         , 0         , 0                 ,   0               ,
};

static const char* kBuiltinShadowCaster[] =
{
    shDepth,
    shCube,
    0
};

const int kShadowCollectorCategories = 2;
static const char* kBuiltinShadowCollector[] =
{
    kNoKeyword,     kNoKeyword, // just our code would interpret zeros here as "end of table"
    shSplitSpheres, 0,
    0,              shSingleCascade,
    shSplitSpheres, shSingleCascade,
    0,              0
};

const int kPrepassFinalCategories = 6;
static const char* kBuiltinPrepassFinal[] =
{
    kNoKeyword  , 0         , 0         , 0         , 0             , 0             , // just our code would interpret zeros here as "end of table"
    0           , 0         , 0         , 0         , 0             , lightProbeSH  ,
    0           , 0         , dynLmOn   , 0         , 0             , 0             ,
    0           , 0         , 0         , 0         , shShadowMask  , lightProbeSH  ,
    0           , 0         , dynLmOn   , 0         , shShadowMask  , 0             ,
    lmOn        , 0         , 0         , 0         , 0             , 0             ,
    lmOn        , 0         , 0         , 0         , 0             , lightProbeSH  ,
    lmOn        , 0         , dynLmOn   , 0         , 0             , 0             ,
    lmOn        , 0         , 0         , 0         , shShadowMask  , 0             ,
    lmOn        , 0         , 0         , 0         , shShadowMask  , lightProbeSH  ,
    lmOn        , 0         , dynLmOn   , 0         , shShadowMask  , 0             ,

    0           , dirLmCom  , dynLmOn   , 0         , 0             , 0             ,
    0           , dirLmCom  , dynLmOn   , 0         , shShadowMask  , 0             ,
    lmOn        , dirLmCom  , 0         , 0         , 0             , 0             ,
    lmOn        , dirLmCom  , 0         , 0         , 0             , lightProbeSH  ,
    lmOn        , dirLmCom  , dynLmOn   , 0         , 0             , 0             ,
    lmOn        , dirLmCom  , 0         , 0         , shShadowMask  , 0             ,
    lmOn        , dirLmCom  , 0         , 0         , shShadowMask  , lightProbeSH  ,
    lmOn        , dirLmCom  , dynLmOn   , 0         , shShadowMask  , 0             ,

    0           , 0         , 0         , kHDROn    , 0             , 0             ,
    0           , 0         , 0         , kHDROn    , 0             , lightProbeSH  ,
    0           , 0         , dynLmOn   , kHDROn    , 0             , 0             ,
    0           , 0         , 0         , kHDROn    , shShadowMask  , 0             ,
    0           , 0         , 0         , kHDROn    , shShadowMask  , lightProbeSH  ,
    lmOn        , 0         , 0         , kHDROn    , 0             , 0             ,
    lmOn        , 0         , 0         , kHDROn    , 0             , lightProbeSH  ,
    lmOn        , 0         , dynLmOn   , kHDROn    , 0             , 0             ,
    lmOn        , 0         , 0         , kHDROn    , shShadowMask  , 0             ,
    lmOn        , 0         , 0         , kHDROn    , shShadowMask  , lightProbeSH  ,
    lmOn        , 0         , dynLmOn   , kHDROn    , shShadowMask  , 0             ,

    0           , dirLmCom  , dynLmOn   , kHDROn    , 0             , 0             ,
    0           , dirLmCom  , dynLmOn   , kHDROn    , shShadowMask  , 0             ,
    lmOn        , dirLmCom  , 0         , kHDROn    , 0             , 0             ,
    lmOn        , dirLmCom  , dynLmOn   , kHDROn    , 0             , 0             ,
    lmOn        , dirLmCom  , 0         , kHDROn    , 0             , lightProbeSH  ,
    lmOn        , dirLmCom  , 0         , kHDROn    , shShadowMask  , 0             ,
    lmOn        , dirLmCom  , 0         , kHDROn    , shShadowMask  , lightProbeSH  ,
    lmOn        , dirLmCom  , dynLmOn   , kHDROn    , shShadowMask  , 0             ,

    0           , 0         , 0         , 0         , 0             , 0             ,
};

static const char* kBuiltinParticles[] =
{
    kNoKeyword,
    softPartsOn,
    0
};


static void FillBuiltinVariants(ShaderVariantData::VariantsArray& outPerms, const char** keywordSet, int categories, const UnityStrArray& skipVariants, bool skipShadow)
{
    outPerms.clear();

    while (true)
    {
        // check if all keywords are null, or whether we need to skip this variant
        bool skip = false;
        bool allNull = true;
        for (int i = 0; i < categories; ++i)
        {
            const char* str = keywordSet[i];
            if (str)
            {
                allNull = false;
                if (skipShadow && !strncmp(str, shPrefix, strlen(shPrefix)))
                    skip = true;
                if (std::find(skipVariants.begin(), skipVariants.end(), str) != skipVariants.end())
                    skip = true;
            }
        }
        if (allNull)
            break;

        if (!skip)
        {
            outPerms.push_back(UnityStrArray());
            for (int i = 0; i < categories; ++i)
            {
                if (keywordSet[i] && !IsAllUnderscores(keywordSet[i]))
                    outPerms.back().push_back(keywordSet[i]);
            }
        }
        keywordSet += categories;
    }
}

static bool FindBuiltinCompileVariants(
    const core::string& multiCompile,
    const core::string& codeString,
    UnityStrArray& skipVariants,
    ShaderVariantData::VariantsArray* outPerms,
    const char** keywordSetVertex,
    const char** keywordSetFragment,
    int categories)
{
    bool skipShadows = false;
    const core::string tokenStr = "#pragma " + multiCompile;

    size_t pos;

    pos = FindTokenInText(codeString, tokenStr + "_nolightmap", 0);
    if (pos != string::npos)
    {
        skipVariants.push_back(lmOn);
    }
    else
    {
        pos = FindTokenInText(codeString, tokenStr, 0);
        if (pos == string::npos)
            return false;
    }

    pos += tokenStr.size();

    while (pos < codeString.size())
    {
        pos = SkipWhiteSpace(codeString, pos);
        core::string nextWord = ReadNonWhiteSpace(codeString, pos);
        if (nextWord == "noshadow")
            skipShadows = true;
        else if (nextWord == "nolightmap")
            skipVariants.push_back(lmOn);
        else if (nextWord == "nodirlightmap")
            skipVariants.push_back(dirLmCom);
        else if (nextWord == "nodynlightmap")
            skipVariants.push_back(dynLmOn);
        else if (nextWord == "noshadowmask")
            skipVariants.push_back(shShadowMask);
        else if (nextWord == "novertexlight")
            skipVariants.push_back(vertexOn);
        else
            break;
    }

    FillBuiltinVariants(outPerms[kProgramVertex],      keywordSetVertex,   categories, skipVariants, skipShadows);
    FillBuiltinVariants(outPerms[kProgramFragment],    keywordSetFragment, categories, skipVariants, skipShadows);
    FillBuiltinVariants(outPerms[kProgramHull],        keywordSetFragment, categories, skipVariants, skipShadows);
    FillBuiltinVariants(outPerms[kProgramDomain],      keywordSetFragment, categories, skipVariants, skipShadows);
    FillBuiltinVariants(outPerms[kProgramGeometry],    keywordSetFragment, categories, skipVariants, skipShadows);
    return true;
}

const char* kProgramVariantSuffixes[] = {"_vertex", "_fragment", "_hull", "_domain", "_geometry", "_surface", ""};
CompileTimeAssert(ARRAY_SIZE(kProgramVariantSuffixes) == kProgramCount + 1, "SHADER COMPILER ERROR: The kProgramVariantSuffixes table for needs to be updated.");

static core::string GetMultiCompilePragmaName(bool multiCompileSyntax)
{
    return multiCompileSyntax ? "multi_compile" : "shader_feature";
}

void ShaderCompileVariants::AddSingleUserMultiCompile(bool multiCompileSyntax, ShaderCompilerProgram type, const core::string& line, ShaderImportErrors& outErrors, bool& inoutOk)
{
    UnityStrArray curKeywords;

    size_t linePos = 0;
    while (linePos < line.size())
    {
        // skip whitespace
        linePos = SkipWhiteSpace(line, linePos);

        // read word
        core::string str = ReadNonWhiteSpace(line, linePos);
        if (str.empty())
            break;

        DebugAssert(!str.empty());
        if (str.find("/*") != core::string::npos || str.find("*/") != core::string::npos)
        {
            outErrors.AddImportError(Format("Block-comments are not supported in #pragma %s, ignoring the whole line.", GetMultiCompilePragmaName(multiCompileSyntax).c_str()), -1, true);
            inoutOk = false;
        }
        else if (!IsAlpha(str[0]) && str[0] != '_')
        {
            outErrors.AddImportError(Format("%s keyword '%s' is not started with a letter or underscore, ignoring the whole line.", GetMultiCompilePragmaName(multiCompileSyntax).c_str(), str.c_str()), -1, true);
            inoutOk = false;
        }
        else
        {
            // add to the list if not there yet
            if (std::find(curKeywords.begin(), curKeywords.end(), str) == curKeywords.end())
                curKeywords.push_back(str);
            else
                outErrors.AddImportError(Format("Ignoring duplicate keyword '%s' in a single %s ", str.c_str(), GetMultiCompilePragmaName(multiCompileSyntax).c_str()), -1, true);

            // if we're multi_compile syntax, add to "always included" keyword list
            if (multiCompileSyntax && !IsAllUnderscores(str))
            {
                if (std::find(m_Data.m_AlwaysIncludedUserKeywords.begin(), m_Data.m_AlwaysIncludedUserKeywords.end(), str) == m_Data.m_AlwaysIncludedUserKeywords.end())
                    m_Data.m_AlwaysIncludedUserKeywords.push_back(str);
            }
        }
    }
    // If we had only one item provided in shader_feature, add a dummy one in front, i.e.
    //   shader_feature FOOBAR
    // is the same as
    //   shader_feature _ FOOBAR
    // Don't do this for multi_compile though, to avoid possible regressions with old existing shaders.
    if (!multiCompileSyntax && curKeywords.size() == 1)
    {
        curKeywords.insert(curKeywords.begin(), "_");
    }

    if (curKeywords.empty())
    {
        if (inoutOk)
            outErrors.AddImportError(GetMultiCompilePragmaName(multiCompileSyntax) + " requires space separated keywords list, ignoring directive", -1, true);
        inoutOk = false;
    }

    // Add to the user keyword variant lists: all shader types;
    // or just specified type only
    if (inoutOk)
    {
        bool hadDuplicate = false;
        for (int i = 0; i < kProgramCount; ++i)
        {
            if (i == (int)type || type == kProgramCount)
            {
                if (std::find(m_Data.m_User[i].begin(), m_Data.m_User[i].end(), curKeywords) == m_Data.m_User[i].end())
                    m_Data.m_User[i].push_back(curKeywords);
                else
                    hadDuplicate = true;
            }
        }

        if (hadDuplicate)
            outErrors.AddImportError(Format("Ignoring duplicate keyword line '%s %s'", GetMultiCompilePragmaName(multiCompileSyntax).c_str(), line.c_str()), -1, true);
    }
}

void ShaderCompileVariants::FindUserCompileVariantDirectives(bool multiCompileSyntax, const core::string& codeString, ShaderCompilerProgram type, ShaderImportErrors& outErrors)
{
    core::string pragmaName = GetMultiCompilePragmaName(multiCompileSyntax);
    pragmaName += kProgramVariantSuffixes[type];
    const core::string token = "#pragma";

    bool ok = true;

    size_t pos = FindTokenInText(codeString, token, 0);
    while (pos != string::npos)
    {
        pos += token.size();

        size_t linePos = pos;

        pos = FindTokenInText(codeString, token, pos);

        // only match #pragma exactly
        if (!IsTabSpace(codeString[linePos]))
            continue;

        linePos = SkipWhiteSpace(codeString, linePos);

        core::string name = ReadNonWhiteSpace(codeString, linePos);

        if (name != pragmaName)
            continue;

        linePos = SkipWhiteSpace(codeString, linePos);

        core::string line = ExtractRestOfLine(codeString, linePos, true);
        AddSingleUserMultiCompile(multiCompileSyntax, type, line, outErrors, ok);
    }
}

void ShaderCompileVariants::FindUserCompileVariantShortcuts(const core::string& codeString, ShaderImportErrors& outErrors, const core::string& pragmaName, const core::string& variants)
{
    size_t pos = FindTokenInText(codeString, pragmaName, 0);
    if (pos == string::npos)
        return;

    bool ok = true;
    AddSingleUserMultiCompile(true, kProgramCount, variants, outErrors, ok);
}

void ShaderCompileVariants::SkipUserVariants(const UnityStrArray& skipVariants)
{
    for (int i = 0; i < kProgramCount; ++i)
    {
        for (size_t j = 0; j < m_Data.m_User[i].size();)
        {
            for (size_t k = 0; k < skipVariants.size(); ++k)
            {
                UnityStrArray::iterator it = std::find(m_Data.m_User[i][j].begin(), m_Data.m_User[i][j].end(), skipVariants[k]);
                if (it != m_Data.m_User[i][j].end())
                    m_Data.m_User[i][j].erase(it);
            }
            if (m_Data.m_User[i][j].empty() || (m_Data.m_User[i][j].size() == 1 && IsAllUnderscores(m_Data.m_User[i][j][0])))
                m_Data.m_User[i].erase(m_Data.m_User[i].begin() + j);
            else
                ++j;
        }
    }
    for (size_t i = 0; i < skipVariants.size(); ++i)
    {
        UnityStrArray::iterator it = std::find(m_Data.m_AlwaysIncludedUserKeywords.begin(), m_Data.m_AlwaysIncludedUserKeywords.end(), skipVariants[i]);
        if (it != m_Data.m_AlwaysIncludedUserKeywords.end())
            m_Data.m_AlwaysIncludedUserKeywords.erase(it);
    }
}

static void FindSkipVariantsDirectives(const core::string& codeString, UnityStrArray& outSkipVariants, ShaderImportErrors& outErrors)
{
    const core::string token = "#pragma skip_variants ";

    bool ok = true;

    size_t pos = FindTokenInText(codeString, token, 0);
    while (pos != string::npos)
    {
        pos += token.size();

        UnityStrArray curKeywords;

        core::string line = ExtractRestOfLine(codeString, pos);
        size_t linePos = 0;
        while (linePos < line.size())
        {
            // skip whitespace
            linePos = SkipWhiteSpace(line, linePos);

            // read word
            core::string str = ReadNonWhiteSpace(line, linePos);
            if (str.empty())
                break;

            DebugAssert(!str.empty());
            if (!IsAlpha(str[0]) && str[0] != '_')
            {
                outErrors.AddImportError(Format("skip_variants keywords must start with a letter or underscore, ignoring '%s'", str.c_str()), -1, true);
                ok = false;
            }
            else
            {
                // add to the list if not there yet
                if (std::find(curKeywords.begin(), curKeywords.end(), str) == curKeywords.end())
                {
                    curKeywords.push_back(str);
                }
            }
        }

        if (curKeywords.empty())
        {
            if (ok)
                outErrors.AddImportError("skip_variants requires space separated keywords list, ignoring directive", -1, true);
            ok = false;
        }

        if (ok)
        {
            outSkipVariants.insert(outSkipVariants.end(), curKeywords.begin(), curKeywords.end());
        }


        pos = FindTokenInText(codeString, token, pos);
    }
}

void ShaderCompileVariants::FindVariantDirectives(const core::string& codeString, bool proceduralInstancing, ShaderImportErrors& outErrors)
{
    // find user provided per-type variants, or global variants (kProgramCount case)
    for (int i = 0; i <= kProgramCount; ++i)
    {
        FindUserCompileVariantDirectives(true, codeString, (ShaderCompilerProgram)i, outErrors);
        FindUserCompileVariantDirectives(false, codeString, (ShaderCompilerProgram)i, outErrors);
    }

    // Find built-in variant shortcuts. A bit special case from other multi_compile_* directives,
    // since we want to allow combining them with others.
    FindUserCompileVariantShortcuts(codeString, outErrors, "#pragma multi_compile_fog", fogKeywords);

    if (proceduralInstancing)
        FindUserCompileVariantShortcuts(codeString, outErrors, "#pragma multi_compile_instancing", proceduralInstancingKeywords);
    else
        FindUserCompileVariantShortcuts(codeString, outErrors, "#pragma multi_compile_instancing", instancingKeywords);

    // find keyword variants to skip
    UnityStrArray skipVariants;
    FindSkipVariantsDirectives(codeString, skipVariants, outErrors);

    // Let the skip_variants directive skip all user variants, including fog variants.
    // In terrain shader we use final modifiers so we have to add #pragma multi_compile_fog ourselves in the shader. But in deferred,
    // prepass base, shadow caster and meta pass we don't want fog variants, we put #pragma skip_variants to skip them (see the use
    // of kSkipVariantsFog in SurfaceCompiler.cpp).
    SkipUserVariants(skipVariants);

    const struct
    {
        const char** keywordSetVertex;
        const char** keywordSetFragment;
        int categories;
    } s_BuiltinCompileVariants[] =
    {
        { kBuiltinLightPass, kBuiltinLightPass, kLightPassCategories }, // forward additive with full shadows is the same variants as light pass
        { kBuiltinForwardAdd, kBuiltinForwardAdd, 1 },
        { kBuiltinForwardBaseAlphaVertex, kBuiltinForwardBaseAlphaFragment, kFwdBaseAlphaCategories },
        { kBuiltinForwardBaseVertex, kBuiltinForwardBaseFragment, kFwdBaseCategories },
        { kBuiltinLightPass, kBuiltinLightPass, kLightPassCategories },
        { kBuiltinShadowCaster, kBuiltinShadowCaster, 1 },
        { kBuiltinShadowCollector, kBuiltinShadowCollector, kShadowCollectorCategories },
        { kBuiltinPrepassFinal, kBuiltinPrepassFinal, kPrepassFinalCategories },
        { kBuiltinParticles, kBuiltinParticles, 1 },
    };
    CompileTimeAssertArraySize(s_BuiltinCompileVariants, kNumBuiltinCompileVariants);

    // find one of built-in compile variant sets
    for (int i = 0; i < kNumBuiltinCompileVariants; ++i)
    {
        if (FindBuiltinCompileVariants(kBuiltinCompileVariants[i], codeString, skipVariants, m_Data.m_Builtin, s_BuiltinCompileVariants[i].keywordSetVertex, s_BuiltinCompileVariants[i].keywordSetFragment, s_BuiltinCompileVariants[i].categories))
            return;
    }
}

void HoistGLSLVersionDirective(core::string& source)
{
    size_t begin = FindTokenInText(source, "#version", 0);
    if (begin != string::npos)
    {
        core::string directive = ExtractRestOfLine(source, begin) + "\n";
        source.replace(begin, 1, "//", 2);
        source.insert(0, directive);
    }
}

void ExtractIncludePortions(core::string& inputSource, const core::string& tokenStart, const core::string& tokenEnd, core::string& output, ShaderImportErrors& outErrors)
{
    output.clear();

    size_t begin = FindTokenInText(inputSource, tokenStart, 0);
    while (begin != string::npos)
    {
        size_t end = FindTokenInText(inputSource, tokenEnd, begin);
        if (end == string::npos)
        {
            outErrors.AddImportError("No " + tokenEnd + " tag found after " + tokenStart + " tag", -1, false);
            return;
        }

        // Find original linenumber for the end of this snippet
        size_t originalEndLineNumber = CountLines(inputSource, 0, end, false);

        // Cut out the snippet
        core::string codeString = inputSource.substr(begin + tokenStart.size(), end - (begin + tokenStart.size()));
        inputSource = inputSource.erase(begin, (end + tokenEnd.size()) - begin);

        // Calc line number and insert a #line directive at start
        size_t currentLineNumber = CountLines(inputSource, 0, begin, false) + 1;

        const core::string curLineText = "#line " + IntToString((int)currentLineNumber) + '\n';
        const core::string origLineText = "#LINE " + IntToString((int)originalEndLineNumber + 1) + '\n';
        inputSource.insert(begin, origLineText);

        output += curLineText + codeString;

        begin = FindTokenInText(inputSource, tokenStart, begin);
    }
}

ShaderGpuProgramType GetGLSLGpuProgramType(ShaderRequirements requirements, ShaderCompilerPlatform platform)
{
    switch (platform)
    {
        case kShaderCompPlatformGLES20:
        case kShaderCompPlatformGLES3Plus:
            if (HasAnyFlags(requirements, kShaderRequireTessellation | kShaderRequireTessHW | kShaderRequireGeometry | kShaderRequireCubeArray))
            {
                return kShaderGpuProgramGLES31AEP;
            }
            if (HasAnyFlags(requirements, kShaderRequireCompute | kShaderRequireRandomWrite | kShaderRequireMSAATex))
            {
                return kShaderGpuProgramGLES31;
            }
            if (HasAnyFlags(requirements, kShaderRequireInstancing | kShaderRequire2DArray | kShaderRequireInterpolators15Integers))
            {
                return kShaderGpuProgramGLES3;
            }
            return platform == kShaderCompPlatformGLES20 ? kShaderGpuProgramGLES : kShaderGpuProgramGLES3;
            break;

        case kShaderCompPlatformOpenGLCore:
            if (HasAnyFlags(requirements, kShaderRequireCompute | kShaderRequireRandomWrite))
            {
                return kShaderGpuProgramGLCore43;
            }
            if (HasAnyFlags(requirements, kShaderRequireTessellation | kShaderRequireTessHW | kShaderRequireGeometry | kShaderRequireCubeArray))
            {
                return kShaderGpuProgramGLCore41;
            }
            return kShaderGpuProgramGLCore32;
            break;
        default:
            return kShaderGpuProgramGLCore32;
    }

    return kShaderGpuProgramGLCore32;
}

// --------------------------------------------------------------------------


#if ENABLE_UNIT_TESTS
#include "Runtime/Testing/Testing.h"

UNIT_TEST_SUITE(ShaderImportUtils)
{
    struct TestShaderCompileVariantsFixture : public ShaderCompileVariants
    {
        TestShaderCompileVariantsFixture()
            : m_User(m_Data.m_User)
            , m_Builtin(m_Data.m_Builtin)
        {
        }

        void FindUserDirectives(const char* s, bool shouldHaveWarnings = false)
        {
            FindVariantDirectives(s, false, errors);
            if (shouldHaveWarnings)
            {
                CHECK(!errors.HasErrors());
                CHECK(errors.HasErrorsOrWarnings());
            }
            else
                CHECK(!errors.HasErrorsOrWarnings());
        }

        static core::string GetKeywordsString(const UnityStrArray& kwnames)
        {
            core::string res;
            for (size_t i = 0, n = kwnames.size(); i != n; ++i)
            {
                if (!res.empty())
                    res += ' ';
                res += kwnames[i];
            }
            return res;
        }

        void EnumerateOne()
        {
            UnityStrArray kwnames;
            Enumerate(kwnames);
            kw = GetKeywordsString(kwnames);
        }

        static core::string GetKeywordSet(const ShaderVariantData::VariantsArray* arr, int index, int programIndex = 0)
        {
            const UnityStrArray& kwnames = arr[programIndex][index];
            return GetKeywordsString(kwnames);
        }

        ShaderImportErrors errors;
        core::string kw;
        ShaderVariantData::VariantsArray* m_User;
        ShaderVariantData::VariantsArray* m_Builtin;
    };

    TEST_FIXTURE(TestShaderCompileVariantsFixture, PragmaShaderFeature_SingleLine_Works)
    {
        FindUserDirectives("#pragma shader_feature AAA BBB");
        for (int i = 0; i < kProgramCount; ++i)
        {
            CHECK_EQUAL(1, m_User[i].size());
            CHECK_EQUAL("AAA BBB", GetKeywordSet(m_User, 0, i));
        }
        size_t comboCount = PrepareEnumeration(kProgramVertex);
        CHECK_EQUAL(2, comboCount);
        EnumerateOne(); CHECK_EQUAL("AAA", kw);
        EnumerateOne(); CHECK_EQUAL("BBB", kw);
    }

    TEST_FIXTURE(TestShaderCompileVariantsFixture, PragmaShaderFeature_SingleLine_SingleItem_Works)
    {
        // Single item provided, need to expand into two variants: one with nothing defined, another with AAA
        FindUserDirectives("#pragma shader_feature AAA");
        CHECK_EQUAL(1, m_User[0].size());
        CHECK_EQUAL("_ AAA", GetKeywordSet(m_User, 0));
        size_t comboCount = PrepareEnumeration(kProgramVertex);
        CHECK_EQUAL(2, comboCount);
        EnumerateOne(); CHECK_EQUAL("", kw);
        EnumerateOne(); CHECK_EQUAL("AAA", kw);
    }

    TEST_FIXTURE(TestShaderCompileVariantsFixture, PragmaShaderFeature_OnlyUnderscores_AreTreatedAsDummy)
    {
        FindUserDirectives("#pragma shader_feature AAA ___ BBB");
        CHECK_EQUAL(1, m_User[0].size());
        if (m_User[0].size() == 1)
        {
            CHECK_EQUAL(3, m_User[0][0].size());
        }
        size_t comboCount = PrepareEnumeration(kProgramVertex);
        CHECK_EQUAL(3, comboCount);
        EnumerateOne(); CHECK_EQUAL("AAA", kw);
        EnumerateOne(); CHECK_EQUAL("", kw);
        EnumerateOne(); CHECK_EQUAL("BBB", kw);
    }

    TEST_FIXTURE(TestShaderCompileVariantsFixture, PragmaShaderFeature_SpecificStage_Works)
    {
        FindUserDirectives(
            "#pragma shader_feature_vertex A B\n"
            "#pragma shader_feature_fragment C D\n"
        );
        // vertex shader should get A, B
        CHECK_EQUAL(1, m_User[kProgramVertex].size());
        if (m_User[kProgramVertex].size() == 1)
        {
            CHECK_EQUAL(2, m_User[kProgramVertex][0].size());
            if (m_User[kProgramVertex][0].size() == 2)
            {
                CHECK_EQUAL("A", m_User[kProgramVertex][0][0]);
                CHECK_EQUAL("B", m_User[kProgramVertex][0][1]);
            }
        }
        size_t comboCount = PrepareEnumeration(kProgramVertex);
        CHECK_EQUAL(2, comboCount);
        EnumerateOne(); CHECK_EQUAL("A", kw);
        EnumerateOne(); CHECK_EQUAL("B", kw);

        // fragment shader should get C, D
        CHECK(m_User[kProgramFragment].size() == 1);
        CHECK(m_User[kProgramFragment][0].size() == 2 && m_User[kProgramFragment][0][0] == "C" && m_User[kProgramFragment][0][1] == "D");
        comboCount = PrepareEnumeration(kProgramFragment);
        CHECK_EQUAL(2, comboCount);
        EnumerateOne(); CHECK_EQUAL("C", kw);
        EnumerateOne(); CHECK_EQUAL("D", kw);

        // geometry shader should get nothing
        CHECK(m_User[kProgramGeometry].size() == 0);
    }

    TEST_FIXTURE(TestShaderCompileVariantsFixture, FindVariant_OnlyUnderscores_Work_WhenMultiplePresent)
    {
        FindUserDirectives(
            "#pragma multi_compile ___ A\n" // two variants
            "#pragma shader_feature B\n" // * two variants
            "#pragma shader_feature _ C\n" // * two variants
        );
        CHECK_EQUAL(3, m_User[0].size());
        size_t comboCount = PrepareEnumeration(kProgramVertex);
        CHECK_EQUAL(8, comboCount);
        EnumerateOne(); CHECK_EQUAL("", kw);
        EnumerateOne(); CHECK_EQUAL("C", kw);
        EnumerateOne(); CHECK_EQUAL("B", kw);
        EnumerateOne(); CHECK_EQUAL("B C", kw);
        EnumerateOne(); CHECK_EQUAL("A", kw);
        EnumerateOne(); CHECK_EQUAL("A C", kw);
        EnumerateOne(); CHECK_EQUAL("A B", kw);
        EnumerateOne(); CHECK_EQUAL("A B C", kw);
    }

    TEST_FIXTURE(TestShaderCompileVariantsFixture, FindVariantsSingleLine)
    {
        FindUserDirectives("#pragma multi_compile AAA BBB");
        CHECK_EQUAL(1, m_User[0].size());
        if (m_User[0].size() == 1)
        {
            CHECK_EQUAL(2, m_User[0][0].size());
            if (m_User[0][0].size() == 2)
            {
                CHECK_EQUAL("AAA", m_User[0][0][0]);
                CHECK_EQUAL("BBB", m_User[0][0][1]);
            }
        }
        size_t comboCount = PrepareEnumeration(kProgramVertex);
        CHECK_EQUAL(2, comboCount);
        EnumerateOne(); CHECK_EQUAL("AAA", kw);
        EnumerateOne(); CHECK_EQUAL("BBB", kw);
    }

    TEST_FIXTURE(TestShaderCompileVariantsFixture, FindVariantsMultipleLines)
    {
        FindUserDirectives(
            "#pragma multi_compile AAA BBB\n"
            "#pragma multi_compile CCC");
        CHECK_EQUAL(2, m_User[0].size());
        if (m_User[0].size() == 2)
        {
            CHECK_EQUAL(2, m_User[0][0].size());
            if (m_User[0][0].size() == 2)
            {
                CHECK_EQUAL("AAA", m_User[0][0][0]);
                CHECK_EQUAL("BBB", m_User[0][0][1]);
            }
            CHECK_EQUAL(1, m_User[0][1].size());
            if (m_User[0][1].size() == 1)
            {
                CHECK_EQUAL("CCC", m_User[0][1][0]);
            }
        }
        size_t comboCount = PrepareEnumeration(kProgramVertex);
        CHECK_EQUAL(2, comboCount);
        EnumerateOne(); CHECK_EQUAL("AAA CCC", kw);
        EnumerateOne(); CHECK_EQUAL("BBB CCC", kw);
    }

    TEST_FIXTURE(TestShaderCompileVariantsFixture, FindVariants_SkipsDuplicates)
    {
        FindUserDirectives("#pragma multi_compile AAA BBB AAA BBB CCC", true);
        CHECK_EQUAL(1, m_User[0].size());
        if (m_User[0].size() == 1)
        {
            CHECK_EQUAL(3, m_User[0][0].size());
            if (m_User[0][0].size() == 3)
            {
                CHECK_EQUAL("AAA", m_User[0][0][0]);
                CHECK_EQUAL("BBB", m_User[0][0][1]);
                CHECK_EQUAL("CCC", m_User[0][0][2]);
            }
        }
    }

    TEST_FIXTURE(TestShaderCompileVariantsFixture, FindVariants_Enumeration_SkipsDuplicates)
    {
        FindUserDirectives(
            "#pragma multi_compile AAA BBB\n"
            "#pragma multi_compile AAA CCC\n"
            "#pragma shader_feature AAA DDD\n");
        CHECK_EQUAL(3, m_User[0].size());
        if (m_User[0].size() == 3)
        {
            CHECK_EQUAL(2, m_User[0][0].size());
            if (m_User[0][0].size() == 2)
            {
                CHECK_EQUAL("AAA", m_User[0][0][0]);
                CHECK_EQUAL("BBB", m_User[0][0][1]);
            }
            CHECK_EQUAL(2, m_User[0][1].size());
            if (m_User[0][1].size() == 2)
            {
                CHECK_EQUAL("AAA", m_User[0][1][0]);
                CHECK_EQUAL("CCC", m_User[0][1][1]);
            }
            CHECK_EQUAL(2, m_User[0][2].size());
            if (m_User[0][2].size() == 2)
            {
                CHECK_EQUAL("AAA", m_User[0][2][0]);
                CHECK_EQUAL("DDD", m_User[0][2][1]);
            }
        }
        size_t comboCount = PrepareEnumeration(kProgramVertex);
        CHECK_EQUAL(8, comboCount);
        // Enumerations should not produce duplicate keyword entries, so e.g. AAA AAA AAA, it should contain only unique: AAA
        EnumerateOne(); CHECK_EQUAL("AAA"        , kw);
        EnumerateOne(); CHECK_EQUAL("AAA DDD"    , kw);
        EnumerateOne(); CHECK_EQUAL("AAA CCC"    , kw);
        EnumerateOne(); CHECK_EQUAL("AAA CCC DDD", kw);
        EnumerateOne(); CHECK_EQUAL("BBB AAA"    , kw);
        EnumerateOne(); CHECK_EQUAL("BBB AAA DDD", kw);
        EnumerateOne(); CHECK_EQUAL("BBB CCC AAA", kw);
        EnumerateOne(); CHECK_EQUAL("BBB CCC DDD", kw);
    }

    TEST_FIXTURE(TestShaderCompileVariantsFixture, FindVariants_Enumeration_SkipsDuplicateLines)
    {
        FindUserDirectives(
            "#pragma multi_compile AAA BBB\n"
            "#pragma multi_compile AAA BBB\n"
            "#pragma shader_feature AAA DDD\n"
            "#pragma shader_feature AAA DDD\n", true); // duplicate keyword lines should be ignored with a warning
        CHECK_EQUAL(2, m_User[0].size());
        if (m_User[0].size() == 2)
        {
            CHECK_EQUAL(2, m_User[0][0].size());
            if (m_User[0][0].size() == 2)
            {
                CHECK_EQUAL("AAA", m_User[0][0][0]);
                CHECK_EQUAL("BBB", m_User[0][0][1]);
            }
            CHECK_EQUAL(2, m_User[0][1].size());
            if (m_User[0][1].size() == 2)
            {
                CHECK_EQUAL("AAA", m_User[0][1][0]);
                CHECK_EQUAL("DDD", m_User[0][1][1]);
            }
        }
        size_t comboCount = PrepareEnumeration(kProgramVertex);
        CHECK_EQUAL(4, comboCount);
        EnumerateOne(); CHECK_EQUAL("AAA", kw);
        EnumerateOne(); CHECK_EQUAL("AAA DDD", kw);
        EnumerateOne(); CHECK_EQUAL("BBB AAA", kw);
        EnumerateOne(); CHECK_EQUAL("BBB DDD", kw);
    }

    TEST_FIXTURE(TestShaderCompileVariantsFixture, FindVariants_SkipsComments)
    {
        FindUserDirectives(
            "#pragma multi_compile AAA BBB // CCC\n", false);
        CHECK_EQUAL(1, m_User[0].size());
        if (m_User[0].size() == 1)
        {
            CHECK_EQUAL(2, m_User[0][0].size());
            if (m_User[0][0].size() == 2)
            {
                CHECK_EQUAL("AAA", m_User[0][0][0]);
                CHECK_EQUAL("BBB", m_User[0][0][1]);
            }
        }
        size_t comboCount = PrepareEnumeration(kProgramVertex);
        CHECK_EQUAL(2, comboCount);
        EnumerateOne(); CHECK_EQUAL("AAA", kw);
        EnumerateOne(); CHECK_EQUAL("BBB", kw);
    }

    TEST_FIXTURE(TestShaderCompileVariantsFixture, FindVariants_SkipsLine_OnBlockCommentTokens)
    {
        FindUserDirectives(
            "#pragma multi_compile AAA BBB /*CCC\n"
            "#pragma multi_compile DD*/\n", true);
        CHECK_EQUAL(0, m_User[0].size());
        size_t comboCount = PrepareEnumeration(kProgramVertex);
        CHECK_EQUAL(1, comboCount);
        EnumerateOne(); CHECK_EQUAL("", kw);
    }

    TEST_FIXTURE(TestShaderCompileVariantsFixture, FindVariants_MultiCompileSyntaxTracking_Works)
    {
        FindUserDirectives(
            "#pragma shader_feature AAA BBB\n"
            "#pragma multi_compile CCC DDD\n"); // these should get tracked as "always included variants" list
        CHECK_EQUAL(2, m_User[0].size());
        UnityStrArray alwaysIncluded, builtin;
        GetKeywordLists(alwaysIncluded, builtin);
        core::string userList[] = {"CCC", "DDD"}; CHECK_ARRAY_EQUAL(userList, alwaysIncluded, 2);
    }

    TEST_FIXTURE(TestShaderCompileVariantsFixture, FindVariantsBuiltin)
    {
        FindUserDirectives(
            "#pragma multi_compile_fwdbasealpha\n");
        CHECK(m_User[0].empty());
        CHECK_EQUAL(19, m_Builtin[0].size());
        CHECK_EQUAL(16, m_Builtin[1].size()); // vertexlight variants only in vertex shader

        size_t comboCount = PrepareEnumeration(kProgramVertex);
        CHECK_EQUAL(19, comboCount);

        EnumerateOne(); CHECK_EQUAL("DIRECTIONAL", kw);
        EnumerateOne(); CHECK_EQUAL("DIRECTIONAL LIGHTPROBE_SH"   , kw);
        EnumerateOne(); CHECK_EQUAL("DIRECTIONAL DYNAMICLIGHTMAP_ON"    , kw);
        EnumerateOne(); CHECK_EQUAL("DIRECTIONAL LIGHTMAP_ON"   , kw);
        EnumerateOne(); CHECK_EQUAL("DIRECTIONAL LIGHTMAP_ON LIGHTPROBE_SH", kw);
        EnumerateOne(); CHECK_EQUAL("DIRECTIONAL LIGHTMAP_ON DYNAMICLIGHTMAP_ON"    , kw);
        EnumerateOne(); CHECK_EQUAL("DIRECTIONAL LIGHTMAP_ON LIGHTMAP_SHADOW_MIXING", kw);
        EnumerateOne(); CHECK_EQUAL("DIRECTIONAL LIGHTMAP_ON LIGHTMAP_SHADOW_MIXING LIGHTPROBE_SH", kw);
        EnumerateOne(); CHECK_EQUAL("DIRECTIONAL LIGHTMAP_ON DYNAMICLIGHTMAP_ON LIGHTMAP_SHADOW_MIXING", kw);

        EnumerateOne(); CHECK_EQUAL("DIRECTIONAL DIRLIGHTMAP_COMBINED DYNAMICLIGHTMAP_ON"   , kw);
        EnumerateOne(); CHECK_EQUAL("DIRECTIONAL LIGHTMAP_ON DIRLIGHTMAP_COMBINED"  , kw);
        EnumerateOne(); CHECK_EQUAL("DIRECTIONAL LIGHTMAP_ON DIRLIGHTMAP_COMBINED LIGHTPROBE_SH", kw);
        EnumerateOne(); CHECK_EQUAL("DIRECTIONAL LIGHTMAP_ON DIRLIGHTMAP_COMBINED DYNAMICLIGHTMAP_ON"   , kw);
        EnumerateOne(); CHECK_EQUAL("DIRECTIONAL LIGHTMAP_ON DIRLIGHTMAP_COMBINED LIGHTMAP_SHADOW_MIXING", kw);
        EnumerateOne(); CHECK_EQUAL("DIRECTIONAL LIGHTMAP_ON DIRLIGHTMAP_COMBINED LIGHTMAP_SHADOW_MIXING LIGHTPROBE_SH", kw);
        EnumerateOne(); CHECK_EQUAL("DIRECTIONAL LIGHTMAP_ON DIRLIGHTMAP_COMBINED DYNAMICLIGHTMAP_ON LIGHTMAP_SHADOW_MIXING", kw);

        EnumerateOne(); CHECK_EQUAL("DIRECTIONAL VERTEXLIGHT_ON LIGHTPROBE_SH"    , kw);
        EnumerateOne(); CHECK_EQUAL("DIRECTIONAL DYNAMICLIGHTMAP_ON VERTEXLIGHT_ON" , kw);
        EnumerateOne(); CHECK_EQUAL("DIRECTIONAL DIRLIGHTMAP_COMBINED DYNAMICLIGHTMAP_ON VERTEXLIGHT_ON"    , kw);
    }

    TEST_FIXTURE(TestShaderCompileVariantsFixture, FindVariantsNoVertexLight)
    {
        FindUserDirectives(
            "#pragma multi_compile_fwdbasealpha novertexlight\n");
        CHECK(m_User[0].empty());
        CHECK_EQUAL(16, m_Builtin[0].size());
        CHECK_EQUAL(16, m_Builtin[1].size());

        size_t comboCount = PrepareEnumeration(kProgramVertex);
        CHECK_EQUAL(16, comboCount);

        EnumerateOne(); CHECK_EQUAL("DIRECTIONAL", kw);
        EnumerateOne(); CHECK_EQUAL("DIRECTIONAL LIGHTPROBE_SH"   , kw);
        EnumerateOne(); CHECK_EQUAL("DIRECTIONAL DYNAMICLIGHTMAP_ON"    , kw);
        EnumerateOne(); CHECK_EQUAL("DIRECTIONAL LIGHTMAP_ON"   , kw);
        EnumerateOne(); CHECK_EQUAL("DIRECTIONAL LIGHTMAP_ON LIGHTPROBE_SH", kw);
        EnumerateOne(); CHECK_EQUAL("DIRECTIONAL LIGHTMAP_ON DYNAMICLIGHTMAP_ON"    , kw);
        EnumerateOne(); CHECK_EQUAL("DIRECTIONAL LIGHTMAP_ON LIGHTMAP_SHADOW_MIXING", kw);
        EnumerateOne(); CHECK_EQUAL("DIRECTIONAL LIGHTMAP_ON LIGHTMAP_SHADOW_MIXING LIGHTPROBE_SH", kw);
        EnumerateOne(); CHECK_EQUAL("DIRECTIONAL LIGHTMAP_ON DYNAMICLIGHTMAP_ON LIGHTMAP_SHADOW_MIXING", kw);

        EnumerateOne(); CHECK_EQUAL("DIRECTIONAL DIRLIGHTMAP_COMBINED DYNAMICLIGHTMAP_ON"   , kw);
        EnumerateOne(); CHECK_EQUAL("DIRECTIONAL LIGHTMAP_ON DIRLIGHTMAP_COMBINED"  , kw);
        EnumerateOne(); CHECK_EQUAL("DIRECTIONAL LIGHTMAP_ON DIRLIGHTMAP_COMBINED LIGHTPROBE_SH", kw);
        EnumerateOne(); CHECK_EQUAL("DIRECTIONAL LIGHTMAP_ON DIRLIGHTMAP_COMBINED DYNAMICLIGHTMAP_ON"   , kw);
        EnumerateOne(); CHECK_EQUAL("DIRECTIONAL LIGHTMAP_ON DIRLIGHTMAP_COMBINED LIGHTMAP_SHADOW_MIXING", kw);
        EnumerateOne(); CHECK_EQUAL("DIRECTIONAL LIGHTMAP_ON DIRLIGHTMAP_COMBINED LIGHTMAP_SHADOW_MIXING LIGHTPROBE_SH", kw);
        EnumerateOne(); CHECK_EQUAL("DIRECTIONAL LIGHTMAP_ON DIRLIGHTMAP_COMBINED DYNAMICLIGHTMAP_ON LIGHTMAP_SHADOW_MIXING", kw);
    }

    TEST_FIXTURE(TestShaderCompileVariantsFixture, FindVariantsNoLightmapAndNoVertexLight)
    {
        FindUserDirectives(
            "#pragma multi_compile_fwdbasealpha nolightmap novertexlight\n");
        size_t comboCount = PrepareEnumeration(kProgramVertex);
        // should strip away both lightmap & vertex-light variants, leaving
        // only this:
        CHECK_EQUAL(4, comboCount);
        EnumerateOne(); CHECK_EQUAL("DIRECTIONAL"       , kw);
        EnumerateOne(); CHECK_EQUAL("DIRECTIONAL LIGHTPROBE_SH"       , kw);
        EnumerateOne(); CHECK_EQUAL("DIRECTIONAL DYNAMICLIGHTMAP_ON"        , kw);
        EnumerateOne(); CHECK_EQUAL("DIRECTIONAL DIRLIGHTMAP_COMBINED DYNAMICLIGHTMAP_ON"   , kw);
    }

    TEST_FIXTURE(TestShaderCompileVariantsFixture, FindVariantsNoLightmapAndNoVertexLight_ViaSkipVariants)
    {
        FindUserDirectives(
            "#pragma multi_compile_fwdbasealpha\n"
            "#pragma skip_variants LIGHTMAP_ON VERTEXLIGHT_ON");
        size_t comboCount = PrepareEnumeration(kProgramVertex);
        // should strip away both lightmap & vertex-light variants, leaving
        // only this:
        CHECK_EQUAL(4, comboCount);
        EnumerateOne(); CHECK_EQUAL("DIRECTIONAL", kw);
        EnumerateOne(); CHECK_EQUAL("DIRECTIONAL LIGHTPROBE_SH"      , kw);
        EnumerateOne(); CHECK_EQUAL("DIRECTIONAL DYNAMICLIGHTMAP_ON"        , kw);
        EnumerateOne(); CHECK_EQUAL("DIRECTIONAL DIRLIGHTMAP_COMBINED DYNAMICLIGHTMAP_ON"   , kw);
    }

    TEST_FIXTURE(TestShaderCompileVariantsFixture, FindVariants_FwdAddFullShadows_SkipSoftShadowsAndShadowMask)
    {
        // Should skip soft shadows variants
        FindUserDirectives(
            "#pragma multi_compile_fwdadd_fullshadows\n"
            "#pragma skip_variants SHADOWS_SOFT\n"
            "#pragma skip_variants SHADOWS_SHADOWMASK\n");
        size_t comboCount = PrepareEnumeration(kProgramFragment);
        CHECK_EQUAL(15, comboCount);

        // "No shadow" variants are retained:
        EnumerateOne(); CHECK_EQUAL("POINT", kw);
        EnumerateOne(); CHECK_EQUAL("DIRECTIONAL", kw);
        EnumerateOne(); CHECK_EQUAL("SPOT", kw);
        EnumerateOne(); CHECK_EQUAL("POINT_COOKIE", kw);
        EnumerateOne(); CHECK_EQUAL("DIRECTIONAL_COOKIE", kw);

        // Regular shadow variants are retained:
        EnumerateOne(); CHECK_EQUAL("SPOT SHADOWS_DEPTH", kw);
        EnumerateOne(); CHECK_EQUAL("SPOT SHADOWS_DEPTH LIGHTMAP_SHADOW_MIXING", kw);
        EnumerateOne(); CHECK_EQUAL("DIRECTIONAL SHADOWS_SCREEN", kw);
        EnumerateOne(); CHECK_EQUAL("DIRECTIONAL_COOKIE SHADOWS_SCREEN", kw);
        EnumerateOne(); CHECK_EQUAL("DIRECTIONAL SHADOWS_SCREEN LIGHTMAP_SHADOW_MIXING", kw);
        EnumerateOne(); CHECK_EQUAL("DIRECTIONAL_COOKIE SHADOWS_SCREEN LIGHTMAP_SHADOW_MIXING", kw);
        EnumerateOne(); CHECK_EQUAL("POINT SHADOWS_CUBE", kw);
        EnumerateOne(); CHECK_EQUAL("POINT SHADOWS_CUBE LIGHTMAP_SHADOW_MIXING", kw);
        EnumerateOne(); CHECK_EQUAL("POINT_COOKIE SHADOWS_CUBE", kw);
        EnumerateOne(); CHECK_EQUAL("POINT_COOKIE SHADOWS_CUBE LIGHTMAP_SHADOW_MIXING", kw);

        // Soft shadow variants are skipped.
        // ShadowMask shadow variants are skipped.
    }

    TEST_FIXTURE(TestShaderCompileVariantsFixture, FindVariantsUserAndBuiltin)
    {
        FindUserDirectives(
            "#pragma multi_compile AAA BBB CCC\n"
            "#pragma multi_compile DDD\n"
            "#pragma multi_compile_fwdbasealpha\n");
        CHECK_EQUAL(2, m_User[0].size());
        CHECK_EQUAL("AAA BBB CCC", GetKeywordSet(m_User, 0));
        CHECK_EQUAL("DDD", GetKeywordSet(m_User, 1));
        CHECK(m_Builtin[0].size() == 19);

        size_t comboCount = PrepareEnumeration(kProgramVertex);
        CHECK_EQUAL(57, comboCount);

        EnumerateOne(); CHECK_EQUAL("AAA DDD DIRECTIONAL", kw);
        EnumerateOne(); CHECK_EQUAL("AAA DDD DIRECTIONAL LIGHTPROBE_SH"   , kw);
        EnumerateOne(); CHECK_EQUAL("AAA DDD DIRECTIONAL DYNAMICLIGHTMAP_ON"    , kw);
        EnumerateOne(); CHECK_EQUAL("AAA DDD DIRECTIONAL LIGHTMAP_ON"   , kw);
        EnumerateOne(); CHECK_EQUAL("AAA DDD DIRECTIONAL LIGHTMAP_ON LIGHTPROBE_SH", kw);
        EnumerateOne(); CHECK_EQUAL("AAA DDD DIRECTIONAL LIGHTMAP_ON DYNAMICLIGHTMAP_ON"    , kw);
        EnumerateOne(); CHECK_EQUAL("AAA DDD DIRECTIONAL LIGHTMAP_ON LIGHTMAP_SHADOW_MIXING", kw);
        EnumerateOne(); CHECK_EQUAL("AAA DDD DIRECTIONAL LIGHTMAP_ON LIGHTMAP_SHADOW_MIXING LIGHTPROBE_SH", kw);
        EnumerateOne(); CHECK_EQUAL("AAA DDD DIRECTIONAL LIGHTMAP_ON DYNAMICLIGHTMAP_ON LIGHTMAP_SHADOW_MIXING", kw);

        EnumerateOne(); CHECK_EQUAL("AAA DDD DIRECTIONAL DIRLIGHTMAP_COMBINED DYNAMICLIGHTMAP_ON"   , kw);
        EnumerateOne(); CHECK_EQUAL("AAA DDD DIRECTIONAL LIGHTMAP_ON DIRLIGHTMAP_COMBINED"  , kw);
        EnumerateOne(); CHECK_EQUAL("AAA DDD DIRECTIONAL LIGHTMAP_ON DIRLIGHTMAP_COMBINED LIGHTPROBE_SH", kw);
        EnumerateOne(); CHECK_EQUAL("AAA DDD DIRECTIONAL LIGHTMAP_ON DIRLIGHTMAP_COMBINED DYNAMICLIGHTMAP_ON"   , kw);
        EnumerateOne(); CHECK_EQUAL("AAA DDD DIRECTIONAL LIGHTMAP_ON DIRLIGHTMAP_COMBINED LIGHTMAP_SHADOW_MIXING", kw);
        EnumerateOne(); CHECK_EQUAL("AAA DDD DIRECTIONAL LIGHTMAP_ON DIRLIGHTMAP_COMBINED LIGHTMAP_SHADOW_MIXING LIGHTPROBE_SH", kw);
        EnumerateOne(); CHECK_EQUAL("AAA DDD DIRECTIONAL LIGHTMAP_ON DIRLIGHTMAP_COMBINED DYNAMICLIGHTMAP_ON LIGHTMAP_SHADOW_MIXING", kw);

        EnumerateOne(); CHECK_EQUAL("AAA DDD DIRECTIONAL VERTEXLIGHT_ON LIGHTPROBE_SH", kw);
        EnumerateOne(); CHECK_EQUAL("AAA DDD DIRECTIONAL DYNAMICLIGHTMAP_ON VERTEXLIGHT_ON" , kw);
        EnumerateOne(); CHECK_EQUAL("AAA DDD DIRECTIONAL DIRLIGHTMAP_COMBINED DYNAMICLIGHTMAP_ON VERTEXLIGHT_ON", kw);

        EnumerateOne(); CHECK_EQUAL("BBB DDD DIRECTIONAL", kw);
        EnumerateOne(); CHECK_EQUAL("BBB DDD DIRECTIONAL LIGHTPROBE_SH"   , kw);
        EnumerateOne(); CHECK_EQUAL("BBB DDD DIRECTIONAL DYNAMICLIGHTMAP_ON"    , kw);
        EnumerateOne(); CHECK_EQUAL("BBB DDD DIRECTIONAL LIGHTMAP_ON"   , kw);
        EnumerateOne(); CHECK_EQUAL("BBB DDD DIRECTIONAL LIGHTMAP_ON LIGHTPROBE_SH", kw);
        EnumerateOne(); CHECK_EQUAL("BBB DDD DIRECTIONAL LIGHTMAP_ON DYNAMICLIGHTMAP_ON"    , kw);
        EnumerateOne(); CHECK_EQUAL("BBB DDD DIRECTIONAL LIGHTMAP_ON LIGHTMAP_SHADOW_MIXING", kw);
        EnumerateOne(); CHECK_EQUAL("BBB DDD DIRECTIONAL LIGHTMAP_ON LIGHTMAP_SHADOW_MIXING LIGHTPROBE_SH", kw);
        EnumerateOne(); CHECK_EQUAL("BBB DDD DIRECTIONAL LIGHTMAP_ON DYNAMICLIGHTMAP_ON LIGHTMAP_SHADOW_MIXING", kw);

        EnumerateOne(); CHECK_EQUAL("BBB DDD DIRECTIONAL DIRLIGHTMAP_COMBINED DYNAMICLIGHTMAP_ON"   , kw);
        EnumerateOne(); CHECK_EQUAL("BBB DDD DIRECTIONAL LIGHTMAP_ON DIRLIGHTMAP_COMBINED"  , kw);
        EnumerateOne(); CHECK_EQUAL("BBB DDD DIRECTIONAL LIGHTMAP_ON DIRLIGHTMAP_COMBINED LIGHTPROBE_SH", kw);
        EnumerateOne(); CHECK_EQUAL("BBB DDD DIRECTIONAL LIGHTMAP_ON DIRLIGHTMAP_COMBINED DYNAMICLIGHTMAP_ON"   , kw);
        EnumerateOne(); CHECK_EQUAL("BBB DDD DIRECTIONAL LIGHTMAP_ON DIRLIGHTMAP_COMBINED LIGHTMAP_SHADOW_MIXING", kw);
        EnumerateOne(); CHECK_EQUAL("BBB DDD DIRECTIONAL LIGHTMAP_ON DIRLIGHTMAP_COMBINED LIGHTMAP_SHADOW_MIXING LIGHTPROBE_SH", kw);
        EnumerateOne(); CHECK_EQUAL("BBB DDD DIRECTIONAL LIGHTMAP_ON DIRLIGHTMAP_COMBINED DYNAMICLIGHTMAP_ON LIGHTMAP_SHADOW_MIXING", kw);

        EnumerateOne(); CHECK_EQUAL("BBB DDD DIRECTIONAL VERTEXLIGHT_ON LIGHTPROBE_SH"    , kw);
        EnumerateOne(); CHECK_EQUAL("BBB DDD DIRECTIONAL DYNAMICLIGHTMAP_ON VERTEXLIGHT_ON" , kw);
        EnumerateOne(); CHECK_EQUAL("BBB DDD DIRECTIONAL DIRLIGHTMAP_COMBINED DYNAMICLIGHTMAP_ON VERTEXLIGHT_ON"    , kw);

        EnumerateOne(); CHECK_EQUAL("CCC DDD DIRECTIONAL", kw);
        EnumerateOne(); CHECK_EQUAL("CCC DDD DIRECTIONAL LIGHTPROBE_SH"   , kw);
        EnumerateOne(); CHECK_EQUAL("CCC DDD DIRECTIONAL DYNAMICLIGHTMAP_ON"    , kw);
        EnumerateOne(); CHECK_EQUAL("CCC DDD DIRECTIONAL LIGHTMAP_ON"   , kw);
        EnumerateOne(); CHECK_EQUAL("CCC DDD DIRECTIONAL LIGHTMAP_ON LIGHTPROBE_SH", kw);
        EnumerateOne(); CHECK_EQUAL("CCC DDD DIRECTIONAL LIGHTMAP_ON DYNAMICLIGHTMAP_ON"    , kw);
        EnumerateOne(); CHECK_EQUAL("CCC DDD DIRECTIONAL LIGHTMAP_ON LIGHTMAP_SHADOW_MIXING", kw);
        EnumerateOne(); CHECK_EQUAL("CCC DDD DIRECTIONAL LIGHTMAP_ON LIGHTMAP_SHADOW_MIXING LIGHTPROBE_SH", kw);
        EnumerateOne(); CHECK_EQUAL("CCC DDD DIRECTIONAL LIGHTMAP_ON DYNAMICLIGHTMAP_ON LIGHTMAP_SHADOW_MIXING", kw);

        EnumerateOne(); CHECK_EQUAL("CCC DDD DIRECTIONAL DIRLIGHTMAP_COMBINED DYNAMICLIGHTMAP_ON"   , kw);
        EnumerateOne(); CHECK_EQUAL("CCC DDD DIRECTIONAL LIGHTMAP_ON DIRLIGHTMAP_COMBINED"  , kw);
        EnumerateOne(); CHECK_EQUAL("CCC DDD DIRECTIONAL LIGHTMAP_ON DIRLIGHTMAP_COMBINED LIGHTPROBE_SH", kw);
        EnumerateOne(); CHECK_EQUAL("CCC DDD DIRECTIONAL LIGHTMAP_ON DIRLIGHTMAP_COMBINED DYNAMICLIGHTMAP_ON"   , kw);
        EnumerateOne(); CHECK_EQUAL("CCC DDD DIRECTIONAL LIGHTMAP_ON DIRLIGHTMAP_COMBINED LIGHTMAP_SHADOW_MIXING", kw);
        EnumerateOne(); CHECK_EQUAL("CCC DDD DIRECTIONAL LIGHTMAP_ON DIRLIGHTMAP_COMBINED LIGHTMAP_SHADOW_MIXING LIGHTPROBE_SH", kw);
        EnumerateOne(); CHECK_EQUAL("CCC DDD DIRECTIONAL LIGHTMAP_ON DIRLIGHTMAP_COMBINED DYNAMICLIGHTMAP_ON LIGHTMAP_SHADOW_MIXING", kw);

        EnumerateOne(); CHECK_EQUAL("CCC DDD DIRECTIONAL VERTEXLIGHT_ON LIGHTPROBE_SH"    , kw);
        EnumerateOne(); CHECK_EQUAL("CCC DDD DIRECTIONAL DYNAMICLIGHTMAP_ON VERTEXLIGHT_ON" , kw);
        EnumerateOne(); CHECK_EQUAL("CCC DDD DIRECTIONAL DIRLIGHTMAP_COMBINED DYNAMICLIGHTMAP_ON VERTEXLIGHT_ON"    , kw);
    }

    TEST_FIXTURE(TestShaderCompileVariantsFixture, FindVariantsShadowCaster)
    {
        FindUserDirectives(
            "#pragma multi_compile_shadowcaster\n");
        CHECK(m_User[0].empty());
        CHECK_EQUAL(2, m_Builtin[0].size());
        CHECK_EQUAL("SHADOWS_DEPTH", GetKeywordSet(m_Builtin, 0));
        CHECK_EQUAL("SHADOWS_CUBE", GetKeywordSet(m_Builtin, 1));
    }

    TEST_FIXTURE(TestShaderCompileVariantsFixture, FindVariantsShadowCollector)
    {
        FindUserDirectives(
            "#pragma multi_compile_shadowcollector\n");
        CHECK(m_User[0].empty());
        CHECK_EQUAL(4, m_Builtin[0].size());
        CHECK_EQUAL("", GetKeywordSet(m_Builtin, 0));
        CHECK_EQUAL("SHADOWS_SPLIT_SPHERES", GetKeywordSet(m_Builtin, 1));
        CHECK_EQUAL("SHADOWS_SINGLE_CASCADE", GetKeywordSet(m_Builtin, 2));
        CHECK_EQUAL("SHADOWS_SPLIT_SPHERES SHADOWS_SINGLE_CASCADE", GetKeywordSet(m_Builtin, 3));
    }

    TEST_FIXTURE(TestShaderCompileVariantsFixture, FindVariantsPrepassFinal)
    {
        FindUserDirectives(
            "#pragma multi_compile_prepassfinal\n");
        CHECK(m_User[0].empty());
        CHECK_EQUAL(38, m_Builtin[0].size());

        int i = -1;
        CHECK_EQUAL("", GetKeywordSet(m_Builtin, ++i));
        CHECK_EQUAL("LIGHTPROBE_SH", GetKeywordSet(m_Builtin, ++i));
        CHECK_EQUAL("DYNAMICLIGHTMAP_ON", GetKeywordSet(m_Builtin, ++i));
        CHECK_EQUAL("SHADOWS_SHADOWMASK LIGHTPROBE_SH", GetKeywordSet(m_Builtin, ++i));
        CHECK_EQUAL("DYNAMICLIGHTMAP_ON SHADOWS_SHADOWMASK", GetKeywordSet(m_Builtin, ++i));
        CHECK_EQUAL("LIGHTMAP_ON", GetKeywordSet(m_Builtin, ++i));
        CHECK_EQUAL("LIGHTMAP_ON LIGHTPROBE_SH", GetKeywordSet(m_Builtin, ++i));
        CHECK_EQUAL("LIGHTMAP_ON DYNAMICLIGHTMAP_ON", GetKeywordSet(m_Builtin, ++i));
        CHECK_EQUAL("LIGHTMAP_ON SHADOWS_SHADOWMASK", GetKeywordSet(m_Builtin, ++i));
        CHECK_EQUAL("LIGHTMAP_ON SHADOWS_SHADOWMASK LIGHTPROBE_SH", GetKeywordSet(m_Builtin, ++i));
        CHECK_EQUAL("LIGHTMAP_ON DYNAMICLIGHTMAP_ON SHADOWS_SHADOWMASK", GetKeywordSet(m_Builtin, ++i));

        CHECK_EQUAL("DIRLIGHTMAP_COMBINED DYNAMICLIGHTMAP_ON", GetKeywordSet(m_Builtin, ++i));
        CHECK_EQUAL("DIRLIGHTMAP_COMBINED DYNAMICLIGHTMAP_ON SHADOWS_SHADOWMASK", GetKeywordSet(m_Builtin, ++i));
        CHECK_EQUAL("LIGHTMAP_ON DIRLIGHTMAP_COMBINED", GetKeywordSet(m_Builtin, ++i));
        CHECK_EQUAL("LIGHTMAP_ON DIRLIGHTMAP_COMBINED LIGHTPROBE_SH", GetKeywordSet(m_Builtin, ++i));
        CHECK_EQUAL("LIGHTMAP_ON DIRLIGHTMAP_COMBINED DYNAMICLIGHTMAP_ON", GetKeywordSet(m_Builtin, ++i));
        CHECK_EQUAL("LIGHTMAP_ON DIRLIGHTMAP_COMBINED SHADOWS_SHADOWMASK", GetKeywordSet(m_Builtin, ++i));
        CHECK_EQUAL("LIGHTMAP_ON DIRLIGHTMAP_COMBINED SHADOWS_SHADOWMASK LIGHTPROBE_SH", GetKeywordSet(m_Builtin, ++i));
        CHECK_EQUAL("LIGHTMAP_ON DIRLIGHTMAP_COMBINED DYNAMICLIGHTMAP_ON SHADOWS_SHADOWMASK", GetKeywordSet(m_Builtin, ++i));

        CHECK_EQUAL("UNITY_HDR_ON", GetKeywordSet(m_Builtin, ++i));
        CHECK_EQUAL("UNITY_HDR_ON LIGHTPROBE_SH", GetKeywordSet(m_Builtin, ++i));
        CHECK_EQUAL("DYNAMICLIGHTMAP_ON UNITY_HDR_ON", GetKeywordSet(m_Builtin, ++i));
        CHECK_EQUAL("UNITY_HDR_ON SHADOWS_SHADOWMASK", GetKeywordSet(m_Builtin, ++i));
        CHECK_EQUAL("UNITY_HDR_ON SHADOWS_SHADOWMASK LIGHTPROBE_SH", GetKeywordSet(m_Builtin, ++i));
        CHECK_EQUAL("LIGHTMAP_ON UNITY_HDR_ON", GetKeywordSet(m_Builtin, ++i));
        CHECK_EQUAL("LIGHTMAP_ON UNITY_HDR_ON LIGHTPROBE_SH", GetKeywordSet(m_Builtin, ++i));
        CHECK_EQUAL("LIGHTMAP_ON DYNAMICLIGHTMAP_ON UNITY_HDR_ON", GetKeywordSet(m_Builtin, ++i));
        CHECK_EQUAL("LIGHTMAP_ON UNITY_HDR_ON SHADOWS_SHADOWMASK", GetKeywordSet(m_Builtin, ++i));
        CHECK_EQUAL("LIGHTMAP_ON UNITY_HDR_ON SHADOWS_SHADOWMASK LIGHTPROBE_SH", GetKeywordSet(m_Builtin, ++i));
        CHECK_EQUAL("LIGHTMAP_ON DYNAMICLIGHTMAP_ON UNITY_HDR_ON SHADOWS_SHADOWMASK", GetKeywordSet(m_Builtin, ++i));

        CHECK_EQUAL("DIRLIGHTMAP_COMBINED DYNAMICLIGHTMAP_ON UNITY_HDR_ON", GetKeywordSet(m_Builtin, ++i));
        CHECK_EQUAL("DIRLIGHTMAP_COMBINED DYNAMICLIGHTMAP_ON UNITY_HDR_ON SHADOWS_SHADOWMASK", GetKeywordSet(m_Builtin, ++i));
        CHECK_EQUAL("LIGHTMAP_ON DIRLIGHTMAP_COMBINED UNITY_HDR_ON", GetKeywordSet(m_Builtin, ++i));
        CHECK_EQUAL("LIGHTMAP_ON DIRLIGHTMAP_COMBINED DYNAMICLIGHTMAP_ON UNITY_HDR_ON", GetKeywordSet(m_Builtin, ++i));
        CHECK_EQUAL("LIGHTMAP_ON DIRLIGHTMAP_COMBINED UNITY_HDR_ON LIGHTPROBE_SH", GetKeywordSet(m_Builtin, ++i));
        CHECK_EQUAL("LIGHTMAP_ON DIRLIGHTMAP_COMBINED UNITY_HDR_ON SHADOWS_SHADOWMASK", GetKeywordSet(m_Builtin, ++i));
        CHECK_EQUAL("LIGHTMAP_ON DIRLIGHTMAP_COMBINED UNITY_HDR_ON SHADOWS_SHADOWMASK LIGHTPROBE_SH", GetKeywordSet(m_Builtin, ++i));
        CHECK_EQUAL("LIGHTMAP_ON DIRLIGHTMAP_COMBINED DYNAMICLIGHTMAP_ON UNITY_HDR_ON SHADOWS_SHADOWMASK", GetKeywordSet(m_Builtin, ++i));
    }

    TEST_FIXTURE(TestShaderCompileVariantsFixture, FindVariantsPrepassFinalNoLightmap)
    {
        FindUserDirectives(
            "#pragma multi_compile_prepassfinal nolightmap nodynlightmap\n");
        CHECK(m_User[0].empty());
        CHECK_EQUAL(7, m_Builtin[0].size());
        CHECK_EQUAL("", GetKeywordSet(m_Builtin, 0));
        CHECK_EQUAL("LIGHTPROBE_SH", GetKeywordSet(m_Builtin, 1));
        CHECK_EQUAL("SHADOWS_SHADOWMASK LIGHTPROBE_SH", GetKeywordSet(m_Builtin, 2));
        CHECK_EQUAL("UNITY_HDR_ON", GetKeywordSet(m_Builtin, 3));
        CHECK_EQUAL("UNITY_HDR_ON LIGHTPROBE_SH", GetKeywordSet(m_Builtin, 4));
        CHECK_EQUAL("UNITY_HDR_ON SHADOWS_SHADOWMASK", GetKeywordSet(m_Builtin, 5));
        CHECK_EQUAL("UNITY_HDR_ON SHADOWS_SHADOWMASK LIGHTPROBE_SH", GetKeywordSet(m_Builtin, 6));
    }

    TEST_FIXTURE(TestShaderCompileVariantsFixture, FindVariantsPrepassFinalNoLightmapNoShadowMask)
    {
        FindUserDirectives(
            "#pragma multi_compile_prepassfinal nolightmap nodynlightmap noshadowmask\n");
        CHECK(!errors.HasErrorsOrWarnings());
        CHECK(m_User[0].empty());
        CHECK_EQUAL(4, m_Builtin[0].size());
        CHECK_EQUAL("", GetKeywordSet(m_Builtin, 0));
        CHECK_EQUAL("LIGHTPROBE_SH", GetKeywordSet(m_Builtin, 1));
        CHECK_EQUAL("UNITY_HDR_ON", GetKeywordSet(m_Builtin, 2));
        CHECK_EQUAL("UNITY_HDR_ON LIGHTPROBE_SH", GetKeywordSet(m_Builtin, 3));
    }

    TEST_FIXTURE(TestShaderCompileVariantsFixture, FindVariantsPrepassFinalNoLightmapMultiCompileSyntax)
    {
        FindUserDirectives(
            "#pragma multi_compile_prepassfinal_nolightmap\n");
        CHECK(m_User[0].empty());
        CHECK_EQUAL(14, m_Builtin[0].size());

        int i = -1;
        CHECK_EQUAL("", GetKeywordSet(m_Builtin, ++i));
        CHECK_EQUAL("LIGHTPROBE_SH", GetKeywordSet(m_Builtin, ++i));
        CHECK_EQUAL("DYNAMICLIGHTMAP_ON", GetKeywordSet(m_Builtin, ++i));
        CHECK_EQUAL("SHADOWS_SHADOWMASK LIGHTPROBE_SH", GetKeywordSet(m_Builtin, ++i));
        CHECK_EQUAL("DYNAMICLIGHTMAP_ON SHADOWS_SHADOWMASK", GetKeywordSet(m_Builtin, ++i));
        CHECK_EQUAL("DIRLIGHTMAP_COMBINED DYNAMICLIGHTMAP_ON", GetKeywordSet(m_Builtin, ++i));
        CHECK_EQUAL("DIRLIGHTMAP_COMBINED DYNAMICLIGHTMAP_ON SHADOWS_SHADOWMASK", GetKeywordSet(m_Builtin, ++i));

        CHECK_EQUAL("UNITY_HDR_ON", GetKeywordSet(m_Builtin, ++i));
        CHECK_EQUAL("UNITY_HDR_ON LIGHTPROBE_SH", GetKeywordSet(m_Builtin, ++i));
        CHECK_EQUAL("DYNAMICLIGHTMAP_ON UNITY_HDR_ON", GetKeywordSet(m_Builtin, ++i));
        CHECK_EQUAL("UNITY_HDR_ON SHADOWS_SHADOWMASK", GetKeywordSet(m_Builtin, ++i));
        CHECK_EQUAL("UNITY_HDR_ON SHADOWS_SHADOWMASK LIGHTPROBE_SH", GetKeywordSet(m_Builtin, ++i));
        CHECK_EQUAL("DIRLIGHTMAP_COMBINED DYNAMICLIGHTMAP_ON UNITY_HDR_ON", GetKeywordSet(m_Builtin, ++i));
        CHECK_EQUAL("DIRLIGHTMAP_COMBINED DYNAMICLIGHTMAP_ON UNITY_HDR_ON SHADOWS_SHADOWMASK", GetKeywordSet(m_Builtin, ++i));
    }

    TEST_FIXTURE(TestShaderCompileVariantsFixture, FindVariants_MultiCompileFogAndOtherBuiltins_WorkTogether)
    {
        FindUserDirectives(
            "#pragma multi_compile_fog\n"
            "#pragma multi_compile_particles\n");
        size_t comboCount = PrepareEnumeration(kProgramVertex);
        CHECK_EQUAL(8, comboCount);
        EnumerateOne(); CHECK_EQUAL("", kw);
        EnumerateOne(); CHECK_EQUAL("SOFTPARTICLES_ON", kw);
        EnumerateOne(); CHECK_EQUAL("FOG_LINEAR", kw);
        EnumerateOne(); CHECK_EQUAL("FOG_LINEAR SOFTPARTICLES_ON", kw);
        EnumerateOne(); CHECK_EQUAL("FOG_EXP", kw);
        EnumerateOne(); CHECK_EQUAL("FOG_EXP SOFTPARTICLES_ON", kw);
        EnumerateOne(); CHECK_EQUAL("FOG_EXP2", kw);
        EnumerateOne(); CHECK_EQUAL("FOG_EXP2 SOFTPARTICLES_ON", kw);
    }

    TEST_FIXTURE(TestShaderCompileVariantsFixture, FindVariants_MultiCompileFogAndInstancing_WorkTogether)
    {
        FindUserDirectives(
            "#pragma multi_compile_fog\n"
            "#pragma multi_compile_instancing\n");
        size_t comboCount = PrepareEnumeration(kProgramVertex);
        CHECK_EQUAL(8, comboCount);
        EnumerateOne(); CHECK_EQUAL("", kw);
        EnumerateOne(); CHECK_EQUAL("INSTANCING_ON", kw);
        EnumerateOne(); CHECK_EQUAL("FOG_LINEAR", kw);
        EnumerateOne(); CHECK_EQUAL("FOG_LINEAR INSTANCING_ON", kw);
        EnumerateOne(); CHECK_EQUAL("FOG_EXP", kw);
        EnumerateOne(); CHECK_EQUAL("FOG_EXP INSTANCING_ON", kw);
        EnumerateOne(); CHECK_EQUAL("FOG_EXP2", kw);
        EnumerateOne(); CHECK_EQUAL("FOG_EXP2 INSTANCING_ON", kw);
    }

    TEST_FIXTURE(TestShaderCompileVariantsFixture, FindVariants_MultiCompileFogAndInstancing_WithProcedural_WorkTogether)
    {
        FindVariantDirectives(
            "#pragma multi_compile_fog\n"
            "#pragma multi_compile_instancing\n", true, errors);
        CHECK(!errors.HasErrorsOrWarnings());
        size_t comboCount = PrepareEnumeration(kProgramVertex);
        CHECK_EQUAL(12, comboCount);
        EnumerateOne(); CHECK_EQUAL("", kw);
        EnumerateOne(); CHECK_EQUAL("INSTANCING_ON", kw);
        EnumerateOne(); CHECK_EQUAL("PROCEDURAL_INSTANCING_ON", kw);
        EnumerateOne(); CHECK_EQUAL("FOG_LINEAR", kw);
        EnumerateOne(); CHECK_EQUAL("FOG_LINEAR INSTANCING_ON", kw);
        EnumerateOne(); CHECK_EQUAL("FOG_LINEAR PROCEDURAL_INSTANCING_ON", kw);
        EnumerateOne(); CHECK_EQUAL("FOG_EXP", kw);
        EnumerateOne(); CHECK_EQUAL("FOG_EXP INSTANCING_ON", kw);
        EnumerateOne(); CHECK_EQUAL("FOG_EXP PROCEDURAL_INSTANCING_ON", kw);
        EnumerateOne(); CHECK_EQUAL("FOG_EXP2", kw);
        EnumerateOne(); CHECK_EQUAL("FOG_EXP2 INSTANCING_ON", kw);
        EnumerateOne(); CHECK_EQUAL("FOG_EXP2 PROCEDURAL_INSTANCING_ON", kw);
    }

    TEST_FIXTURE(TestShaderCompileVariantsFixture, FindVariants_MultiCompileFogAndUserDirectives_WorkTogether)
    {
        FindUserDirectives(
            "#pragma multi_compile_fog\n"
            "#pragma multi_compile A B\n");
        size_t comboCount = PrepareEnumeration(kProgramVertex);
        CHECK_EQUAL(8, comboCount);
        EnumerateOne(); CHECK_EQUAL("A", kw);
        EnumerateOne(); CHECK_EQUAL("A FOG_LINEAR", kw);
        EnumerateOne(); CHECK_EQUAL("A FOG_EXP", kw);
        EnumerateOne(); CHECK_EQUAL("A FOG_EXP2", kw);
        EnumerateOne(); CHECK_EQUAL("B", kw);
        EnumerateOne(); CHECK_EQUAL("B FOG_LINEAR", kw);
        EnumerateOne(); CHECK_EQUAL("B FOG_EXP", kw);
        EnumerateOne(); CHECK_EQUAL("B FOG_EXP2", kw);
    }

    TEST_FIXTURE(TestShaderCompileVariantsFixture, FindVariants_SkipVariants_OneLine_Works)
    {
        FindUserDirectives(
            "#pragma multi_compile_fwdadd\n"
            "#pragma skip_variants SPOT POINT DIRECTIONAL\n");
        size_t comboCount = PrepareEnumeration(kProgramVertex);
        CHECK_EQUAL(2, comboCount);
        EnumerateOne(); CHECK_EQUAL("POINT_COOKIE", kw);
        EnumerateOne(); CHECK_EQUAL("DIRECTIONAL_COOKIE", kw);
    }

    TEST_FIXTURE(TestShaderCompileVariantsFixture, FindVariants_SkipVariants_MultipleLines_Works)
    {
        FindUserDirectives(
            "#pragma multi_compile_fwdadd\n"
            "#pragma skip_variants SPOT POINT\n"
            "#pragma skip_variants DIRECTIONAL\n");
        size_t comboCount = PrepareEnumeration(kProgramVertex);
        CHECK_EQUAL(2, comboCount);
        EnumerateOne(); CHECK_EQUAL("POINT_COOKIE", kw);
        EnumerateOne(); CHECK_EQUAL("DIRECTIONAL_COOKIE", kw);
    }

    TEST(FindShaderPlatforms)
    {
        CHECK_EQUAL(1 << kShaderCompPlatformD3D11, FindShaderPlatforms("d3d11"));
        CHECK_EQUAL(1 << kShaderCompPlatformGLES20, FindShaderPlatforms("gles"));
        CHECK_EQUAL(1 << kShaderCompPlatformGLES3Plus, FindShaderPlatforms("gles3"));
        CHECK_EQUAL(1 << kShaderCompPlatformOpenGLCore, FindShaderPlatforms("glcore"));
        CHECK_EQUAL(-1, FindShaderPlatforms("foobar"));
        CHECK_EQUAL(kValidShaderCompPlatformMask, FindShaderPlatforms("shaderonly"));
    }
}


#endif // ENABLE_UNIT_TESTS
