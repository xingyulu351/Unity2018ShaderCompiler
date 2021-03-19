#pragma once

#include "../ShaderCompiler.h"
#include "../Utilities/ShaderImportUtils.h"
#include <string>
#include <vector>
#include <map>
#include <set>

struct SurfaceParams;
class ShaderImportErrors;


enum MemberType
{
    kTypeFloat,
    kTypeFloat2,
    kTypeFloat3,
    kTypeFloat4,
    kTypeHalf,
    kTypeHalf2,
    kTypeHalf3,
    kTypeHalf4,
    kTypeFixed,
    kTypeFixed2,
    kTypeFixed3,
    kTypeFixed4,
    kTypeVoid,
    kTypeOther,
};

enum TypePrec
{
    kPrecFixed,
    kPrecHalf,
    kPrecFloat,
    kPrecCount
};

enum MemberModifier
{
    kInputModNone,
    kInputModIn,
    kInputModOut,
    kInputModInOut,
    kInputModUniform,
};


int GetMemberTypeDimension(MemberType type);


struct MemberDecl
{
    MemberDecl(core::string_ref typeName_, core::string_ref name_, core::string_ref semantic_, MemberType type_, int semIndex_ = 0, MemLabelId memLabel = kMemDefault)
        :   typeName(typeName_, memLabel)
        ,   name(name_, memLabel)
        ,   semantic(semantic_, memLabel)
        ,   type(type_)
        ,   mod(kInputModNone)
        ,   arraySize(-1)
        ,   semanticIndex(semIndex_)
    {
    }

    core::string typeName;
    core::string name;
    core::string semantic;
    MemberType type;
    MemberModifier  mod;
    int arraySize; // -1 if not array
    int semanticIndex;
};
typedef std::vector<MemberDecl> MemberDeclarations;


// Arbitrary interpolators packed into 4-component "registers".
struct PackedInterpolators
{
    void Pack(size_t count, const MemberDecl* texCoords);

    // For each input interpolator, records which packed one it lands into, and where in it.
    struct OffsetInPacked
    {
        int index;
        int offset;
    };
    std::vector<OffsetInPacked> offsets;

    // For each packed register, tracks number of components used, and type precision.
    struct Packed
    {
        int usedComps;
        TypePrec precision;
        core::string names;
    };
    std::vector<Packed> packed;
};


struct TessellationModel
{
    TessellationModel()
        : useAppdataInput(false)
    {
    }

    bool useAppdataInput;
};


struct SurfaceFunctionDesc
{
    SurfaceFunctionDesc() : appdataType("appdata_full"), vertexModCustomData(false) {}

    const MemberDecl* FindOutputMember(const core::string& name) const
    {
        for (MemberDeclarations::const_iterator it = outputMembers.begin(), itEnd = outputMembers.end(); it != itEnd; ++it)
        {
            if (it->name == name)
                return &*it;
        }
        return NULL;
    }

    core::string inputType;
    core::string inputName;
    core::string outputType;
    core::string outputName;
    core::string appdataType;
    MemberDeclarations inputMembers;
    MemberDeclarations customInputMembers;
    MemberDeclarations outputMembers;
    MemberDeclarations appdataMembers;
    TessellationModel tessModel;
    PackedInterpolators packedCustomInputMembers;
    bool vertexModCustomData;
};


// Surface shader lighting model - a set of functions starting with "Lighting"
// that define BRDF and its parts. Some are built-in (e.g. Lambert), but they can also
// be user-written.
struct LightingModel
{
    LightingModel()
        : useViewDir(false)
        , useViewDirForSingleLightmap(false)
        , useViewDirForDualLightmap(false)
        , useViewDirForDirLightmap(false)
        , hasCustomSingleLightmap(false)
        , hasCustomDualLightmaps(false)
        , hasCustomDirLightmaps(false)
        , hasGI(false)
        , isPBS(false)
        , useGIForward(false)
        , useGIDeferred(false)
        , renderPaths(0)
    {
    }

    bool UseViewDirLightmap() const { return useViewDirForSingleLightmap | useViewDirForDualLightmap | useViewDirForDirLightmap; }

    core::string name;
    core::string outputStructName;
    bool useViewDir;
    bool useViewDirForSingleLightmap;
    bool useViewDirForDualLightmap;
    bool useViewDirForDirLightmap;
    bool hasCustomSingleLightmap;
    bool hasCustomDualLightmaps;
    bool hasCustomDirLightmaps;
    bool hasGI;
    bool isPBS;
    bool useGIForward;
    bool useGIDeferred;
    unsigned int renderPaths;
};

typedef std::map<core::string, LightingModel> LightingModels;


struct SurfaceFunctionData
{
public:
    SurfaceFunctionData()
        : writesNormal(false)
        , writesEmission(false)
        , writesSpecular(false)
        , writesOcclusion(false)
    {
    }

public:
    SurfaceFunctionDesc desc;
    LightingModels lightModels;
    bool writesNormal;
    bool writesEmission;
    bool writesSpecular;
    bool writesOcclusion;
};


struct SurfacePassData
{
public:
    SurfacePassData()
        : needsWorldReflection(false)
        , needsWorldNormal(false)
        , needsScreenPosition(false)
        , needsViewDir(false)
        , needsWorldPos(false)
        , needsWorldViewDir(false)
        , lightNeedsWorldPos(false)
        , lightNeedsWorldViewDir(false)
        , lightmapNeedsViewDir(false)
        , needsVertexColor(false)
        , needsVFace(false)
        , useTangentSpace(false)
        , readsNormal(false)
    {
    }

public:
    MemberDeclarations texcoords;
    PackedInterpolators packedTC;
    core::string vertexColorField;
    core::string vfaceField;

    bool needsWorldReflection;
    bool needsWorldNormal;
    bool needsScreenPosition;
    bool needsViewDir;
    bool needsWorldPos;
    bool needsWorldViewDir;
    bool lightNeedsWorldPos;
    bool lightNeedsWorldViewDir;
    bool lightmapNeedsViewDir;
    bool needsVertexColor;
    bool needsVFace;
    bool useTangentSpace;

    bool readsNormal;
};


enum SurfacePass
{
    kSurfFwdBase = 0,
    kSurfFwdAdd,
    kSurfPrepassBase,
    kSurfPrepassFinal,
    kSurfDeferred,
    kSurfShadow,
    kSurfMeta,
    kSurfPassCount
};


typedef std::vector<core::string> StringArray;

struct SurfaceData
{
    struct Entry
    {
        SurfaceFunctionData func;
        SurfacePassData passes[kSurfPassCount];
        StringArray keywords;
    };

    // A "what data is needed" entry per compile variant, in enumeration order.
    // Always has at least one entry.
    std::vector<Entry> m_Datas;

    std::set<core::string> m_AllKeywords;

    static ShaderCompilerPlatform AnalysisShaderCompilerPlatform;
};

enum PerPixelReflectMode
{
    kPerPixelReflectNo = 0,
    kPerPixelReflectYes,
    kPerPixelReflectAnalysis,
};

void EmitProlog(core::string& gen, const SurfaceParams& params, bool includeAutoLight, bool pbs, PerPixelReflectMode perPixelReflect);

// For surface shaders with a lot of variants, we have a cache that tries to avoid doing redundant analysis for identical (after preprocessing) variants.
struct SurfaceAnalysisCache;
SurfaceAnalysisCache* CreateSurfaceAnalysisCache(const IncludeSearchContext& includeContext);
void DestroySurfaceAnalysisCache(SurfaceAnalysisCache* cache);
void GetSurfaceAnalysisCacheStats(const SurfaceAnalysisCache* cache, size_t& outRequests, size_t& outHits, size_t& outASTRequests, size_t& outASTHits);
const std::vector<core::string> GetSurfaceAnalysisOpenedIncludes(const SurfaceAnalysisCache* cache);


// Surface shader code generation works by first analyzing the user-written surface shader code.
//
// For example, it needs to find whether per-pixel Normal is written to or not; and if it's not written
// into then it can save on interpolators by not passing the tangent space matrix from vertex to pixel shader; and so on.
//
// Analysis is done by doing a small "fake" compilation of a small specially crafted shader snippet that calls into user-written
// surface function; in two ways:
// 1. Calculate "function" data; this does parsing of user written code at AST level to figure out names of input/output structures etc.,
//    as well as compilation to find whether for example writes to Normal per-pixel are done.
// 2. Calculate "pass" data; additional separate analyses are done for each possible pass type (forward, deferred, shadow etc.)
//    to find which inputs/outputs are needed strictly for that pass. For example, UV input that ends up being used for fetching Albedo
//    texture is typically "used" by Forward/Deferred passes, but not a Shadow pass. Unless that UV is also used for doing alpha-testing,
//    in which case Shadow pass needs it too! That's why this per-pass analysis step is needed.
//
// Note: both CalculateSurfaceFunctionData and CalculateSurfacePassData need to be thread-safe; they are called
// from jobs.
void CalculateSurfaceFunctionData(const SurfaceParams& params, const StringArray& keywords, SurfaceFunctionData& data, core::string& result, ShaderImportErrors& outErrors, SurfaceAnalysisCache& analysisCache);
void CalculateSurfacePassData(const SurfaceParams& params, const StringArray& keywords, SurfacePass pass, const SurfaceFunctionData& funcData, SurfacePassData& data, core::string& result, bool lightNeedsViewDir, ShaderImportErrors& outErrors, SurfaceAnalysisCache& analysisCache);

bool CheckVertexInputRequirements(SurfaceParams& params, const SurfaceFunctionData& funcData, const SurfacePassData& passData, ShaderImportErrors& outErrors);


void SetSurfaceShaderArgs(const ShaderCompilerPlatform api, const ShaderCompileRequirements& requirements, const StringArray& keywords, ShaderArgs& args);
