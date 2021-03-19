#include "UnityPrefix.h"
#include "CgBatch.h"
#include <cstdio>
#include <cstdlib>
#include <string>
#include <vector>
#include <algorithm>
#include "APIPlugin.h"
#include "ShaderCompiler.h"
#include "SurfaceCompiler/SurfaceCompiler.h"
#include "artifacts/generated/Configuration/UnityConfigureVersion.gen.h"
#include "Runtime/GfxDevice/GfxUtilities.h"
#include "Runtime/Utilities/Word.h"
#include "Runtime/Utilities/BitUtility.h"
#include "Runtime/Utilities/PathNameUtility.h"
#include "Runtime/Jobs/Internal/JobQueue.h"
#include "Runtime/Threads/Thread.h"
#include "Runtime/Threads/Mutex.h"
#include "Runtime/Testing/ParametricTest.h"
#include "Runtime/Testing/Testing.h"
#include "Runtime/Utilities/HashStringFunctions.h"
#include "Runtime/Utilities/PathNameUtility.h"
#include "Runtime/Utilities/File.h"
#include "Utilities/ProcessIncludes.h"
#include "Utilities/ShaderImportErrors.h"
#include "Editor/Src/Utility/TextUtilities.h"
#include "External/yaml/include/yaml.h"
#include <time.h>
#if PLATFORM_OSX
#include <sys/sysctl.h>
#include <sys/time.h>
#include <sys/types.h>
#include <mach-o/dyld.h>
#endif
#if PLATFORM_LINUX
#include <sys/time.h>
#include <unistd.h>
#endif
#include <map>

#if PLATFORM_WIN
#include "PlatformDependent/Win/WinUnicode.h"
#include <sysinfoapi.h>
#include <processenv.h>
#include <winerror.h>
#include <errhandlingapi.h>
#endif

#include "GLES20Compiler/GLESCompilerUtils.h"
#include "MetalCompiler/MetalCompilerUtils.h"
#include "Tools/UnityShaderCompiler/ComputeShaderCompiler.h"


static IncludeSearchContext g_IncludeSearchContext;

core::string g_IncludesPath;

// deliberately no new line on the end of this, since it in effect replaces the original GLSLPROGRAM or CGPROGRAM
// which mostly have a newline. We want this to be on precisely the same line as the original token.
const char *kShaderCompilerOriginalStartText = "/* UNITY: Original start of shader */";

// JobQueue needs systeminfo::GetProcessorCount symbol, but compiling in SystemInfo.cpp would pull in a ton
// of other unneeded stuff (registry, link with network libs etc. etc.). So instead just re-implement it here.
namespace systeminfo
{
    int GetProcessorCount()
    {
        #if PLATFORM_WIN
        SYSTEM_INFO sysInfo;
        sysInfo.dwNumberOfProcessors = 1;
        GetSystemInfo(&sysInfo);
        return sysInfo.dwNumberOfProcessors;

        #elif PLATFORM_OSX
        int cores = 1;
        size_t len = sizeof(cores);
        int err = ::sysctlbyname("hw.ncpu", &cores, &len, 0, 0);
        return err == 0 ? cores : 1;

        #elif PLATFORM_LINUX
        return std::max(1L, sysconf(_SC_NPROCESSORS_ONLN));

        #else
        #error Unknown platform for shader compiler
        #endif
    }
}


bool SetEnv(const char *envvar, const char *value)
{
#if PLATFORM_WIN

    if (*value == 0)    // if we pass an empty string, then actualy delete the variable
        value = NULL;

    DWORD success = SetEnvironmentVariableA(envvar, value);
    if (!success)
        return false;
#else
    if (*value == 0)
    {
        unsetenv(envvar);
        return true;
    }

    int result = setenv(envvar, value, 1);
    if (result == 0)
        return false;
#endif
    return true;
}

// crossplatform environment variable retreival. We could use getenv() in windows, however it fails to detect deleted variables
bool GetEnvVar(const char* var, core::string& value)
{
#if PLATFORM_WIN
    char buffer[MAX_PATH];
    int ret = GetEnvironmentVariableA(var, buffer, MAX_PATH);
    if (ret == 0)
    {
        if (GetLastError() == ERROR_ENVVAR_NOT_FOUND)
        {
            return false;
        }
        value = core::string();
        return true;
    }
    else if (ret >= MAX_PATH)
    {
        return false;
    }

    value = core::string(buffer);
    return true;
#else
    char *envval = getenv(var);
    if (envval == NULL)
        return false;
    value = core::string(envval);
    return true;
#endif
}

// ----------------------------------------------------------------------------------------------

struct SnippetParameters
{
    SnippetParameters()
        : shaderAPIs(~0) // "all of them"
        , hardwareTierVariants(0) // "none of them"
    {
    }

    bool HasAnyProgram() const
    {
        for (int i = 0; i < kProgramCount; ++i)
            if (!p.entryName[i].empty())
                return true;
        return false;
    }

    bool HasProgram(ShaderCompilerProgram t) const { return !p.entryName[t].empty(); }
    UInt32 HasProgramsMask() const
    {
        UInt32 mask = 0;
        for (int i = 0; i < kProgramCount; ++i)
            if (!p.entryName[i].empty())
                mask |= (1 << i);
        return mask;
    }

    ShaderCompileParams p;
    SurfaceCompileOptions o;
    ShaderCompileRequirements t;

    unsigned int shaderAPIs;
    unsigned int hardwareTierVariants;
};

static void FindSurfaceParameters(const core::string& line, size_t linePos, const core::string& pragmaLine, int lineNumber, SnippetParameters& out, ShaderImportErrors& outErrors)
{
    if (out.HasProgram(kProgramSurface))
    {
        outErrors.AddImportError("Duplicate #pragma surface found, ignoring: " + pragmaLine, lineNumber, true);
        return;
    }

    // read all tokens following the pragma
    std::vector<core::string> tokens;
    tokens.reserve(4);
    while (true)
    {
        linePos = SkipWhiteSpace(line, linePos);
        core::string t = ReadNonWhiteSpace(line, linePos);
        if (t.empty())
            break;
        tokens.push_back(t);
    }

    // #pragma surface surfaceFunction lightModel [optionalparams]
    // optional params:
    //  alpha:auto|fade|premul - alpha blending mode
    //  alpha - same as alpha:auto
    //  keepalpha - output alpha, but without alpha blending mode
    //  alphatest:VariableName - alpha testing mode, cutoff value in VariableName
    //  vertex:VertexFunction - custom vertex modification function
    //  finalcolor:ColorFunction - custom final color modification function
    //  finalprepass:PrepassFunction - custom prepass (base) modification function
    //  finalgbuffer:GBufferFunction - custom final gbuffer modification function
    //  tessellate:TessFactorsFunction - custom tessellation factors function
    //  tessphong:VariableName - use Phong tessellation, with alpha from given variable
    //  decal:add|blend - additive or alpha blending decal shader (e.g. terrain AddPass)
    //  exclude_path:prepass|forward - exclude code generation for given rendering path
    //  addshadow - add shadow caster & collector passes
    //  noshadow - disable support for receiving shadows
    //  softvegetation - require soft vegetation for the generated passes
    //  nolightmap - do not support lightmaps in the generated shader
    //  noshadowmask - no shadow mask
    //  noambient - do not add scene ambient lighting
    //  novertexlights - skip calculating vertex lights, do only ambient lighting
    //  noforwardadd - do not generate forward additive pass
    //  fullforwardshadows - full set of shadow variants for forward rendering
    //  dualforward - use dual lightmaps in forward rendering
    //  approxview - normalize view direction per vertex instead of per pixel
    //  halfasview - use half-vector everywhere instead of view direction
    //  nofog - do not generate fog support code
    //  noinstancing - do not generate instancing variants
    //  dithercrossfade - add LOD_FADE_CROSSFADE variant and implement dithering fade
    //  nolppv - do not use Light Probe Proxy Volume feature. Fallback to normal probe blending
    //  nocolormask - prevent code generation from adding ColorMask so user can provide their own
    if (tokens.size() < 2)
    {
        outErrors.AddImportError("#pragma surface needs function name and lighting model: " + pragmaLine, lineNumber, false);
        return;
    }

    // read surface entry name and lighting model
    out.p.entryName[kProgramSurface] = tokens[0];
    out.o.lighting = tokens[1];
    core::string tokenValue;
    for (size_t i = 2; i < tokens.size(); ++i)
    {
        const core::string& t = tokens[i];
        if (t == "alpha" || t == "alpha:auto")
        {
            out.o.mode = kSurfaceModeAlpha;
            out.o.noShadow = true; // Alpha currently implies noshadow
        }
        else if (t == "alpha:blend" || t == "alpha:fade")
        {
            out.o.mode = kSurfaceModeAlpha;
            out.o.blend = kSurfaceBlendAlpha;
            out.o.noShadow = true; // Alpha currently implies noshadow
        }
        else if (t == "alpha:premul")
        {
            out.o.mode = kSurfaceModeAlpha;
            out.o.blend = kSurfaceBlendPremultiply;
            out.o.noShadow = true; // Alpha currently implies noshadow
        }
        else if (GetValueAfterPrefix(t, "alphatest:", out.o.alphaTestVariable))
        {
            out.o.mode = kSurfaceModeAlphaTest;
        }
        else if (GetValueAfterPrefix(t, "finalcolor:", out.o.finalColorModifier)
                 || GetValueAfterPrefix(t, "finalprepass:", out.o.finalPrepassModifier)
                 || GetValueAfterPrefix(t, "finalgbuffer:", out.o.finalGBufferModifier))
        {
            // nothing extra
        }
        else if (GetValueAfterPrefix(t, "tessellate:", out.o.tessModifier))
        {
            out.t.m_BaseRequirements |= kShaderRequireTessellation | kShaderRequireTessHW;
        }
        else if (GetValueAfterPrefix(t, "tessphong:", out.o.tessPhongAlpha))
        {
            // nothing extra
        }
        else if (t == "decal:add")
        {
            out.o.mode = kSurfaceModeDecalAdd;
        }
        else if (t == "decal:blend")
        {
            out.o.mode = kSurfaceModeDecalBlend;
        }
        else if (GetValueAfterPrefix(t, "vertex:", out.p.entryName[kProgramVertex]))
        {
            // nothing extra
        }
        else if (GetValueAfterPrefix(t, "exclude_path:", tokenValue))
        {
            if (tokenValue == "forward")
                out.o.renderPaths &= ~(1 << kPathForward);
            else if (tokenValue == "prepass")
                out.o.renderPaths &= ~(1 << kPathPrePass);
            else if (tokenValue == "deferred")
                out.o.renderPaths &= ~(1 << kPathDeferred);
            else
            {
                outErrors.AddImportError("Unrecognized #pragma surface exclude_path: '" + tokenValue + "'", lineNumber, true);
            }
        }
        else if (t == "addshadow")
        {
            out.o.addShadow = true;
        }
        else if (t == "noshadow")
        {
            out.o.noShadow = true;
        }
        else if (t == "softvegetation")
        {
            out.o.softVegetation = true;
        }
        else if (t == "keepalpha")
        {
            out.o.keepAlpha = true;
        }
        else if (t == "nolightmap")
        {
            out.o.noLightmap = true;
            // 'nolightmap' implies no dynamic lightmaps nor shadowmask
            out.o.noDynLightmap = true;
            out.o.noShadowMask = true;
        }
        else if (t == "nodirlightmap")
        {
            out.o.noDirLightmap = true;
        }
        else if (t == "noshadowmask")
        {
            out.o.noShadowMask = true;
        }
        else if (t == "nodynlightmap")
        {
            out.o.noDynLightmap = true;
        }
        else if (t == "noambient")
        {
            out.o.noAmbient = true;
        }
        else if (t == "novertexlights")
        {
            out.o.noVertexLights = true;
        }
        else if (t == "noforwardadd")
        {
            out.o.noForwardAdd = true;
        }
        else if (t == "nofog")
        {
            out.o.noFog = true;
        }
        else if (t == "nometa")
        {
            out.o.noMeta = true;
        }
        else if (t == "noinstancing")
        {
            out.o.noInstancing = true;
        }
        else if (t == "dithercrossfade")
        {
            out.o.ditherCrossFade = true;
            // automatic bump the shader requirement of LOD_FADE_CROSSFADE keyword to SM3.0 as before that we might not have SV_Position in fragment shader.
            // TODO: Enable GLES2 despite it having SHADER_TARGET less than 30 since gl_FragCoord is available in fragment shader since the very beginning.
            out.o.pragmaDirectives += "#pragma target 3.0 LOD_FADE_CROSSFADE\n"; // AddKeywordRequirements won't work in surface shader pass; pass through to shader snippet pass.
            outErrors.AddImportError("Automatically setting target of LOD_FADE_CROSSFADE to 3.0", lineNumber, true);
        }
        else if (t == "approxview")
        {
            // obsolete
        }
        else if (t == "halfasview")
        {
            out.o.halfAsViewDir = true;
        }
        else if (t == "interpolateview")
        {
            out.o.interpolateView = true;
        }
        else if (t == "fullforwardshadows")
        {
            out.o.fullForwardShadows = true;
        }
        else if (t == "dualforward")
        {
            out.o.dualForward = true;
        }
        else if (t == "nolppv")
        {
            out.o.noLPPV = true;
        }
        else if (t == "nocolormask")
        {
            out.o.noColorMask = true;
        }
        else
        {
            outErrors.AddImportError("Unrecognized #pragma surface directive: " + t, lineNumber, true);
        }
    }
}

static bool ParseMaxCountOption(const core::string& token, const char* optionName, size_t optionNameLen, int& outValue, int lineNumber, ShaderImportErrors& outErrors)
{
    if (!BeginsWith(token.c_str(), optionName))
        return false;

    core::string tokenValue = token.substr(optionNameLen + 1);
    if (sscanf(tokenValue.c_str(), "%d", &outValue) != 1)
    {
        outErrors.AddImportError(Format("Can't convert '%s' to an integer for '%s' option.", tokenValue.c_str(), optionName), lineNumber, true);
    }
    else if (outValue < 3)
    {
        // Using arr[1] will make compiler optimize off the array indexing.
        // Using arr[2] will trigger the code path for flexible array size (default).
        outErrors.AddImportError(Format("Option '%s' must be greater or equal to 3.", optionName), lineNumber, true);
        outValue = 3;
    }
    return true;
}

static void FindInstancingOptions(const core::string& line, size_t linePos, int lineNumber, SnippetParameters& out, ShaderImportErrors& outErrors)
{
    // read all tokens following the pragma
    std::vector<core::string> tokens;
    while (true)
    {
        linePos = SkipWhiteSpace(line, linePos);
        core::string t = ReadNonWhiteSpace(line, linePos);
        if (t.empty())
            break;
        tokens.push_back(t);
    }

    // #pragma instancing_options [optionalparams]
    // optional params:
    //  maxcount:number             - specifies max instance count, which defines the instanced property array sizes. Ignored on platforms that don't support flexible-array-size feature yet.
    //  forcemaxcount:number        - specifies max instance count for all platforms.
    //  assumeuniformscaling        - assume scaling is uniform. Effectively turns UnityObjectToWorldNormal into UnityObjectToWorldDir, eliminating the use of WorldToObject matrix in most cases.
    //  nomatrices                  - Object matrices are not instanced.
    //  nolodfade                   - don't instance LOD fade values.
    //  norenderinglayer            - don't instance Rendering layer values.
    //  nolightprobe                - don't instance SH coefficients and probe occlusion values.
    //  nolightmap                  - don't instance LightmapST values.
    //  forwardadd                  - generate instancing code in ForwardAdd pass for surface shaders
    //  procedural:func             - specifies the function get called in UNITY_SETUP_INSTANCE_ID

    core::string tokenValue;
    for (size_t i = 0; i < tokens.size(); ++i)
    {
        const core::string& t = tokens[i];
        if (ParseMaxCountOption(t, "maxcount", ARRAY_SIZE("maxcount") - 1, out.p.maxInstanceCount, lineNumber, outErrors)
            || ParseMaxCountOption(t, "forcemaxcount", ARRAY_SIZE("forcemaxcount") - 1, out.p.forcedMaxInstanceCount, lineNumber, outErrors))
        {
        }
        // TODO: move this to a standalone pragma directive as this is not only for instancing
        else if (t == "assumeuniformscaling")
        {
            out.p.compileFlags |= kShaderCompFlags_AssumeUniformScaling;
        }
        else if (t == "nomatrices")
        {
            out.p.compileFlags |= kShaderCompFlags_DontInstanceObjectMatrices;
        }
        else if (t == "lodfade")
        {
            // obsolete. do nothing (lod fade is by default instanced).
        }
        else if (t == "nolodfade")
        {
            out.p.compileFlags |= kShaderCompFlags_DontInstanceLODFade;
        }
        else if (t == "renderinglayer")
        {
            out.p.compileFlags |= kShaderCompFlags_InstanceRenderingLayer;
        }
        else if (t == "nolightprobe")
        {
            out.p.compileFlags |= kShaderCompFlags_DontInstanceSH;
        }
        else if (t == "nolightmap")
        {
            out.p.compileFlags |= kShaderCompFlags_DontInstanceLightmapST;
        }
        else if (t == "forwardadd")
        {
            out.o.instancingInForwardAdd = true;
        }
        else if (t == "force_same_maxcount_for_gl")
        {
            // obsolete. do nothing.
        }
        else if (GetValueAfterPrefix(t, "procedural:", tokenValue))
        {
            out.p.proceduralFuncName = tokenValue;
        }
        else
        {
            outErrors.AddImportError("Unrecognized #pragma instancing_options directive: " + t, lineNumber, true);
        }
    }
}

static bool ProcessProgramName(
    const core::string& pragmaName, const core::string& programName, ShaderCompilerProgram programType,
    const core::string& pragmaLine, const core::string& line, size_t& inoutPos, int lineNumber,
    ShaderCompileParams& out, ShaderImportErrors& outErrors)
{
    if (pragmaName != programName)
        return false;

    // read vertex entry name
    if (!out.entryName[programType].empty())
    {
        outErrors.AddImportError("Duplicate #pragma " + pragmaName + " found, ignoring: " + pragmaLine, lineNumber, true);
    }
    else
    {
        out.entryName[programType] = ReadNonWhiteSpace(line, inoutPos);
        if (out.entryName[programType].empty())
        {
            outErrors.AddImportError("#pragma " + pragmaName + " needs a function name: " + pragmaLine, lineNumber, true);
        }
    }
    return true;
}

// Some specific shader requirements should be straightforward to strip out from shader requirements
// if no entry point function is provided. For some shader targets, like Metal, this will make a big
// difference on existing content compatibility that may have been written using #pragma target 4.0
// or #pragma target 5.0 for example. Obviously a large part of shaders out there don't actually
// use geometry shaders at all, that Metal lacks.

static void DowngradeShaderRequirements(ShaderSourceLanguage language, int startLine, SnippetParameters& params, ShaderRequirements& requirements)
{
    // Downgrade tessellation requirements if no hull and domain shaders used
    if (!params.HasProgram(kProgramHull) && !params.HasProgram(kProgramDomain) && HasFlag(requirements, kShaderRequireTessellation))
    {
        requirements &= ~(kShaderRequireTessellation | kShaderRequireTessHW);
    }

    // Downgrade geometry requirement if no geometry shader used
    if (!params.HasProgram(kProgramGeometry) && HasFlag(requirements, kShaderRequireGeometry))
    {
        requirements &= ~kShaderRequireGeometry;
    }
}

static void AdjustShaderAPIs(ShaderSourceLanguage language, int startLine, SnippetParameters& params, ShaderImportErrors& outErrors)
{
    // If the shader APIs include XboxOne then allow XBoxOneD3D12
    // On Xbox, unlike PC, we distinguish between 11 and 12 due to the root signature/precompilation process
    // But all shader source code is the same. Adding this prevents users having to add xboxone_d3d12 into pragma *_renderers statements
    if (params.shaderAPIs & (1 << kShaderCompPlatformXboxOne))
    {
        params.shaderAPIs |= (1 << kShaderCompPlatformXboxOneD3D12);
    }

    DowngradeShaderRequirements(language, startLine, params, params.t.m_BaseRequirements);

    // If we have tessellation shaders, automatically set requirements
    if (params.HasProgram(kProgramHull) || params.HasProgram(kProgramDomain))
    {
        // Technically this is DX-style tessellation, and shader writer should have specified both "tessellation" and "tessHW" requirements,
        // since there are platforms (Metal) where only one could be present.
        // In practice as long as they have written just one of these flags, or a corresponding #pragma target, we treat that
        // as "enough effort on their part", setup both of the flags, and move on -- much less confusion for everyone.
        // Worst case they will lose Metal support for this shader.
        if (!HasAnyFlags(params.t.m_BaseRequirements, kShaderRequireTessellation | kShaderRequireTessHW) && params.t.m_BaseRequirements != kShaderRequireShaderModel25_93)
            outErrors.AddImportError("Program uses tessellation shaders, treating as if '#pragma require tessellation tessHW' was present", startLine, true);
        params.t.m_BaseRequirements |= kShaderRequireTessellation | kShaderRequireTessHW;
    }
    // If we have geometry shaders, automatically set requirements
    if (params.HasProgram(kProgramGeometry) && !HasFlag(params.t.m_BaseRequirements, kShaderRequireGeometry))
    {
        if (params.t.m_BaseRequirements != kShaderRequireShaderModel25_93)
            outErrors.AddImportError("Program uses geometry shaders, treating as if '#pragma require geometry' was present", startLine, true);
        params.t.m_BaseRequirements |= kShaderRequireGeometry;
    }

    // If we're not building surface shader, and aren't GLSLPROGRAM snippet:
    // check if we have both vertex&fragment. If we don't, emit warning and exclude from all platforms.
    // Don't produce warning if we have excluded all platforms from compilation anyway.
    if (!params.HasProgram(kProgramSurface) && language != kShaderSourceLanguageGLSL)
    {
        const bool hasBothShaders = params.HasProgram(kProgramVertex) && params.HasProgram(kProgramFragment);
        const bool excludedAllPlatforms = (params.shaderAPIs & kValidShaderCompPlatformMask) == 0;
        if (!hasBothShaders && !excludedAllPlatforms)
        {
            outErrors.AddImportError("Both vertex and fragment programs must be present in a shader snippet. Excluding it from compilation.", startLine, true);
            params.shaderAPIs &= ~kValidShaderCompPlatformMask;
        }
    }
}

// We know these built-in variants are complex enough to need particular shader model
// at minimum.
static void AddBuiltinKeywordTargetRequirements(SnippetParameters& out)
{
    // require SM3.0+ (long instructions, 10 interpolators etc.)
    out.t.AddKeywordRequirements("SHADOWS_SOFT", kShaderRequireShaderModel30);
    out.t.AddKeywordRequirements("DIRLIGHTMAP_COMBINED", kShaderRequireShaderModel30);
    out.t.AddKeywordRequirements("DIRLIGHTMAP_SEPARATE", kShaderRequireShaderModel30);
    out.t.AddKeywordRequirements("DYNAMICLIGHTMAP_ON", kShaderRequireShaderModel30);
    out.t.AddKeywordRequirements("SHADOWS_SCREEN", kShaderRequireShaderModel30);
    // require instancing
    out.t.AddKeywordRequirements("INSTANCING_ON", kShaderRequireInstancing);
    // require compute
    out.t.AddKeywordRequirements("PROCEDURAL_INSTANCING_ON", kShaderRequireCompute);

    out.t.AddKeywordRequirements("STEREO_MULTIVIEW_ON", kShaderRequireShaderModel35_ES3);
    out.t.AddKeywordRequirements("STEREO_INSTANCING_ON", kShaderRequireShaderModel35_ES3);
}

// Parse a "#pragma target" directive.
// This is a alternative/shortcut syntax for "#pragma require" -- the "require" one is more granular.
//
// "#pragma target 3.0" - sets up base shader model target
// "#pragma target 4.0 FOO BAR BAZ" -- tells that any variants with any of FOO or BAR or BAZ keywords should use SM4.0 model
static void ParsePragmaTargetLine(const core::string& wholeLine, const core::string& line, size_t& linePos, int lineNumber, SnippetParameters& out, ShaderImportErrors& outErrors)
{
    // read compile target
    core::string targetName = ReadNonWhiteSpace(line, linePos);
    ShaderRequirements requirements = GetShaderRequirementsFromPragmaTargetString(targetName);
    if (requirements == kShaderRequireNothing)
    {
        outErrors.AddImportError("#pragma target with unknown target: " + wholeLine, lineNumber, true);
        return;
    }

    // now read space separated list of keywords for this target (might be empty)
    UnityStrArray keywords;
    while (linePos < line.size())
    {
        linePos = SkipWhiteSpace(line, linePos);

        core::string str = ReadNonWhiteSpace(line, linePos);
        if (str.empty())
            break;

        keywords.push_back(str);
    }

    // if we had any keywords, mark them as needing these features
    // otherwise, this is a base requirement
    if (!keywords.empty())
    {
        for (size_t i = 0, n = keywords.size(); i != n; ++i)
            out.t.AddKeywordRequirements(keywords[i], requirements);
    }
    else
    {
        // special case for SM2.0 requirement: replace previous requirements with it, instead of union (default is 2.5, so union would not be able to go below that)
        if (requirements == kShaderRequireShaderModel20)
            out.t.m_BaseRequirements = requirements;
        else
            out.t.m_BaseRequirements |= requirements;
    }
}

// Parse a "#pragma require" directive
//
// "#pragma require FeatureA FeatureB ..." - adds feature requirements.
// "#pragma require FeatureA FeatureB : FOO BAR BAZ" -- tells that any variants with any of FOO or BAR or BAZ keywords should require FeatureA & Feature B
static void ParsePragmaRequireLine(const core::string& wholeLine, const core::string& line, size_t& linePos, int lineNumber, SnippetParameters& out, ShaderImportErrors& outErrors)
{
    // read feature names up until ":"
    ShaderRequirements requirements = kShaderRequireNothing;
    bool hadFeatures = false;
    while (linePos < line.size())
    {
        linePos = SkipWhiteSpace(line, linePos);
        core::string featureName = ReadNonWhiteSpace(line, linePos);
        if (featureName == ":")
            break;
        ShaderRequirements feature = GetShaderRequirementsFromFeatureString(featureName);
        if (feature == kShaderRequireNothing)
        {
            outErrors.AddImportError(Format("#pragma require with unknown feature '%s': %s", featureName.c_str(), wholeLine.c_str()), lineNumber, true);
        }
        else
        {
            requirements |= feature;
            hadFeatures = true;
        }
    }

    if (!hadFeatures)
    {
        outErrors.AddImportError("#pragma require without a feature list: " + wholeLine, lineNumber, true);
        return;
    }

    // now read space separated list of keywords for this requirement (might be empty)
    UnityStrArray keywords;
    while (linePos < line.size())
    {
        linePos = SkipWhiteSpace(line, linePos);
        core::string str = ReadNonWhiteSpace(line, linePos);
        if (str.empty())
            break;
        keywords.push_back(str);
    }

    // if we had any keywords, mark them as needing these features
    // otherwise, this is a base requirement
    if (!keywords.empty())
    {
        for (size_t i = 0, n = keywords.size(); i != n; ++i)
            out.t.AddKeywordRequirements(keywords[i], requirements);
    }
    else
    {
        out.t.m_BaseRequirements |= requirements;
    }
}

static void FindParameters(const core::string& codeString, int startLine, ShaderSourceLanguage language, SnippetParameters& out, ShaderImportErrors& outErrors)
{
    size_t pos;
    size_t prevPos = 0;

    int currentLineNumber = startLine;

    size_t originalStart = codeString.find(kShaderCompilerOriginalStartText);
    int lineOffset = 0;
    if (originalStart != core::string::npos)
        lineOffset = (int)CountLines(codeString, 0, originalStart, false, 0);

    // go over all pragmas
    const core::string kPragmaToken("#pragma");
    pos = FindTokenInText(codeString, kPragmaToken, 0);
    while (pos != core::string::npos)
    {
        currentLineNumber = (int)CountLines(codeString, prevPos, pos, false, currentLineNumber);
        prevPos = pos;
        pos += kPragmaToken.size();

        // extract line
        core::string line = ExtractRestOfLine(codeString, pos, true);
        core::string pragmaLine = kPragmaToken + line;

        int lineNumber = std::max(startLine, currentLineNumber - lineOffset);

        // remove pragma line from the code string
        //codeString.erase( pos-token.size(), token.size()+line.size() );

        // find next pragma already, we don't use 'pos' anymore further
        pos = FindTokenInText(codeString, kPragmaToken, pos);

        // only match #pragma exactly
        if (!IsTabSpace(line[0]))
            continue;

        size_t linePos = 0;
        // skip whitespace
        linePos = SkipWhiteSpace(line, linePos);

        // read pragma name
        core::string pragmaName = ReadNonWhiteSpace(line, linePos);
        // skip whitespace after pragma name
        linePos = SkipWhiteSpace(line, linePos);

        bool recordPragma = true;
        bool pragmaUnknown = false;

        // detect known pragmas
        if (pragmaName == "profiles")
        {
            outErrors.AddImportError("Note: #pragma profiles is ignored (use #pragma target instead): " + pragmaLine, lineNumber, true);
            recordPragma = false;
        }
        else if (pragmaName == "surface")
        {
            FindSurfaceParameters(line, linePos, pragmaLine, lineNumber, out, outErrors);
            recordPragma = false;
        }
        else if (ProcessProgramName(pragmaName, "vertex", kProgramVertex, pragmaLine, line, linePos, lineNumber, out.p, outErrors))
        {
            recordPragma = false;
        }
        else if (ProcessProgramName(pragmaName, "fragment", kProgramFragment, pragmaLine, line, linePos, lineNumber, out.p, outErrors))
        {
            recordPragma = false;
        }
        else if (ProcessProgramName(pragmaName, "hull", kProgramHull, pragmaLine, line, linePos, lineNumber, out.p, outErrors))
        {
            recordPragma = false;
        }
        else if (ProcessProgramName(pragmaName, "domain", kProgramDomain, pragmaLine, line, linePos, lineNumber, out.p, outErrors))
        {
            recordPragma = false;
        }
        else if (ProcessProgramName(pragmaName, "geometry", kProgramGeometry, pragmaLine, line, linePos, lineNumber, out.p, outErrors))
        {
            recordPragma = false;
        }
        else if (pragmaName == "target")
        {
            ParsePragmaTargetLine(pragmaLine, line, linePos, lineNumber, out, outErrors);
        }
        else if (pragmaName == "require")
        {
            ParsePragmaRequireLine(pragmaLine, line, linePos, lineNumber, out, outErrors);
        }
        else if (
            pragmaName == "glsl" ||
            pragmaName == "no_auto_normalization" ||
            pragmaName == "glsl_no_auto_normalization" ||
            pragmaName == "profileoption" ||
            pragmaName == "fragmentoption" ||
            pragmaName == "metal_use_hlslcc" ||
            pragmaName == "metal_use_hlsl2glsl")
        {
            // silently ignore - does nothing these days (still recognize it as valid pragma
            // to avoid compile error)
        }
        else if (pragmaName == "glsl_no_optimize")
        {
            out.p.compileFlags |= kShaderCompFlags_SkipGLSLOptimize;
        }
        else if (pragmaName == "only_renderers")
        {
            // only given renderers
            unsigned int newShaderAPIs = 0;
            while (true)
            {
                core::string api = ReadNonWhiteSpace(line, linePos);
                if (api.empty())
                    break;
                linePos = SkipWhiteSpace(line, linePos);
                int mask = FindShaderPlatforms(api);
                if (mask == -1)
                {
                    outErrors.AddImportError("Unrecognized renderer for #pragma only_renderers: " + api, lineNumber, true);
                    recordPragma = false;
                }
                else
                {
                    newShaderAPIs |= mask;
                }
            }
            if (newShaderAPIs != 0)
            {
                out.shaderAPIs = newShaderAPIs;
            }
            else
            {
                outErrors.AddImportError("#pragma only_renderers did not contain any renderers, ignoring", lineNumber, true);
                recordPragma = false;
            }
        }
        else if (pragmaName == "exclude_renderers")
        {
            // exclude given renderers
            while (true)
            {
                core::string api = ReadNonWhiteSpace(line, linePos);
                if (api.empty())
                    break;
                linePos = SkipWhiteSpace(line, linePos);
                int mask = FindShaderPlatforms(api);
                if (mask == -1)
                {
                    outErrors.AddImportError("Unrecognized renderer for #pragma exclude_renderers: " + api, lineNumber, true);
                    recordPragma = false;
                }
                else
                {
                    out.shaderAPIs &= ~mask;
                }
            }
        }
        else if (pragmaName == "hardware_tier_variants")
        {
            core::string api = ReadNonWhiteSpace(line, linePos);

            int mask = FindShaderPlatforms(api);
            if (mask == -1)
            {
                outErrors.AddImportError("Unrecognized renderer for #pragma hardware_tier_variants: " + api, lineNumber, true);
                recordPragma = false;
            }
            else
            {
                out.hardwareTierVariants |= mask;
            }
        }
        else if (pragmaName == "debug")
        {
            // enable debug info (surface shader extra printing)
            out.p.compileFlags |= kShaderCompFlags_LegacyEnableDebugInfo;
        }
        else if (pragmaName == "enable_d3d11_debug_symbols")
        {
            // enable DX11 debug info
            out.p.compileFlags |= kShaderCompFlags_EnableDebugInfoD3D11;
        }
        else if (pragmaName == "hlslcc_bytecode_disassembly")
        {
            // embed HLSLcc bytecode disassembly to generated GLSL/Metal shader source
            out.p.compileFlags |= kShaderCompFlags_EmbedHLSLccDisassembly;
        }
        else if (pragmaName == "disable_fastmath")
        {
            // disable fastmath
            out.p.compileFlags |= kShaderCompFlags_DisableFastMath;
        }
        else if (pragmaName == "prefer_hlslcc")
        {
            // prefer HLSLcc backend for given renderers
            while (true)
            {
                core::string api = ReadNonWhiteSpace(line, linePos);
                if (api.empty())
                    break;
                linePos = SkipWhiteSpace(line, linePos);
                int mask = FindShaderPlatforms(api);
                if (mask == -1)
                {
                    outErrors.AddImportError("Unrecognized renderer for #pragma prefer_hlslcc: " + api, lineNumber, true);
                    recordPragma = false;
                }
                else if (out.shaderAPIs == mask)
                {
                    out.p.compileFlags |= kShaderCompFlags_PreferHLSLcc;
                    out.p.compileFlags &= ~kShaderCompFlags_PreferHLSL2GLSL;
                }
            }
        }
        else if (pragmaName == "prefer_hlsl2glsl")
        {
            // prefer HLSL2GLSL + glsl-optimizer backend for given renderers
            while (true)
            {
                core::string api = ReadNonWhiteSpace(line, linePos);
                if (api.empty())
                    break;
                linePos = SkipWhiteSpace(line, linePos);
                int mask = FindShaderPlatforms(api);
                if (mask == -1)
                {
                    outErrors.AddImportError("Unrecognized renderer for #pragma prefer_hlsl2glsl: " + api, lineNumber, true);
                    recordPragma = false;
                }
                else if (out.shaderAPIs == mask)
                {
                    out.p.compileFlags |= kShaderCompFlags_PreferHLSL2GLSL;
                    out.p.compileFlags &= ~kShaderCompFlags_PreferHLSLcc;
                }
            }
        }
        else if (
            BeginsWith(pragmaName, "multi_compile") ||
            BeginsWith(pragmaName, "shader_feature") ||
            BeginsWith(pragmaName, "skip_variants"))
        {
            // Nothing to do otherwise, just avoid the warning.
        }
        else if (pragmaName == "force_concat_matrices")
        {
            // Nothing to do, just avoid the warning.
        }
        else if (pragmaName == "lighting")
        {
            // Nothing to do, just avoid the warning.
        }
        else if (pragmaName == "warning")
        {
            // Ignore any #pragma warning
        }
        else if (BeginsWith(pragmaName, "argument"))
        {
            // Ignore any #pragma argument
        }
        else if (pragmaName == "instancing_options")
        {
            FindInstancingOptions(line, linePos, lineNumber, out, outErrors);
        }
        else if (BeginsWith(pragmaName, "argument"))    // allows passing native pssl pragma options to ps4
        {
            // Ignore any #pragma argument
        }
        else if (pragmaName == "glsl_es2")
        {
            out.p.compileFlags |= kShaderCompFlags_GLSL_ES2;
        }
        else if (pragmaName == "pssl2")
        {
            out.p.compileFlags |= kShaderCompFlags_PSSL2;   // use pssl2 rather that pssl. ps4 only
        }
        else
        {
            outErrors.AddImportError("Unrecognized #pragma directive: " + pragmaName, lineNumber, true);
            pragmaUnknown = true;
        }

        if (recordPragma)
        {
            if (pragmaUnknown)
                out.o.pragmaDirectives += "// ";
            out.o.pragmaDirectives += pragmaLine;
            if (pragmaUnknown)
                out.o.pragmaDirectives += " // Unity: Unrecognised #pragma directive";
            out.o.pragmaDirectives += '\n';
        }
    }

    out.p.sourceLanguage = language;
    AddBuiltinKeywordTargetRequirements(out);
    AdjustShaderAPIs(language, startLine, out, outErrors);
}

typedef bool (*ProcessSnippetFunction)(const IncludeSearchContext& includeContext, const core::string& codeString, int startLine, core::string& output, ShaderImportErrors& outErrors, int& id, ShaderSourceLanguage language, ShaderCompilerPreprocessCallbacks* callbacks);


static bool ProcessPrograms(
    const IncludeSearchContext& includeContext, core::string& source, ShaderSourceLanguage language,
    ProcessSnippetFunction func,
    int& snippetSequence,
    ShaderImportErrors& outErrors,
    ShaderCompilerPreprocessCallbacks* callbacks)
{
    const core::string includeTag   = kShaderLangIncludeTag[language];
    const core::string programTag   = kShaderLangBeginTag[language];
    const core::string endTag       = kShaderLangEndTag[language];

    const core::string origsource = source;

    core::string includePortions;
    ExtractIncludePortions(source, includeTag, endTag, includePortions, outErrors);

    const core::string kTokenStart = programTag;
    const core::string kTokenEnd = endTag;

    size_t begin = FindTokenInText(source, kTokenStart, 0);
    size_t beginInOrigSource = FindTokenInText(origsource, kTokenStart, 0);
    bool res = false;
    size_t lastEndInOrigSource = 0;
    size_t lastEndInOrigSourceLineNumber = 0;

    while (begin != core::string::npos && beginInOrigSource != core::string::npos)
    {
        size_t end = FindTokenInText(source, kTokenEnd, begin);
        size_t endInOrigSource = FindTokenInText(origsource, kTokenEnd, beginInOrigSource);
        if (end == core::string::npos || endInOrigSource == core::string::npos)
        {
            outErrors.AddImportError(Format("No %s tag found after %s tag", endTag.c_str(), programTag.c_str()), -1, false);
            return res;
        }
        // Find original line number for the end of this snippet - we'll insert it after
        size_t originalEndLineNumber = CountLines(origsource, lastEndInOrigSource, endInOrigSource, true, lastEndInOrigSourceLineNumber) + 1;

        // Cut out the snippet between start and end tags
        core::string codeString = source.substr(begin + kTokenStart.size(), end - (begin + kTokenStart.size()));
        source = source.erase(begin, (end + kTokenEnd.size()) - begin);

        // Find line number for the start of this snippet and insert a #line directive
        int startLine = (int)CountLines(origsource, lastEndInOrigSource, beginInOrigSource, true, lastEndInOrigSourceLineNumber) + 1;

        // DX11 HLSL compiler treats #line directives funky: any empty, whitespace-only or comment-only
        // lines immediately after it are just skipped and do not increase the line number counter.
        // To work around it, we insert a dummy preprocessor directive just after #line.
        core::string curLineText = "#line " + IntToString(startLine - 2);
        if (language == kShaderSourceLanguageCg || language == kShaderSourceLanguageHLSL)
            curLineText += " \"\"";
        curLineText += "\n";
        curLineText += "#ifdef DUMMY_PREPROCESSOR_TO_WORK_AROUND_HLSL_COMPILER_LINE_HANDLING\n";
        curLineText += "#endif\n";

        bool containsOriginalStart = (codeString.find(kShaderCompilerOriginalStartText) != core::string::npos);

        codeString = includePortions + curLineText + (containsOriginalStart ? "" : kShaderCompilerOriginalStartText) + codeString;

        // Do actual processing of this snippet
        core::string output;
        res |= func(includeContext, codeString, startLine, output, outErrors, snippetSequence, language, callbacks);

        core::string insertSource = output + "\n#LINE " + IntToString((int)originalEndLineNumber) + '\n';
        source.insert(begin, insertSource);

        begin = FindTokenInText(source, kTokenStart, begin + insertSource.size());
        beginInOrigSource = FindTokenInText(origsource, kTokenStart, endInOrigSource + kTokenEnd.size());
        lastEndInOrigSource = endInOrigSource;
        lastEndInOrigSourceLineNumber = originalEndLineNumber - 1;
    }

    return res;
}

// Generates code for surface shaders, if they are present in the snippet.
// Otherwise just adds a common source prefix (automatic includes etc.) if needed and returns false.
static bool GenerateSurfaceShaderCode(const IncludeSearchContext& includeContext, const core::string& codeString, int startLine, core::string& output, ShaderImportErrors& outErrors, int& snippetSequence, ShaderSourceLanguage language, ShaderCompilerPreprocessCallbacks* callbacks)
{
    ShaderImportErrors errors;
    SnippetParameters params;
    FindParameters(codeString, startLine, language, params, errors);

    if (!params.HasProgram(kProgramSurface))
    {
        output =
            core::string(kShaderLangBeginTag[language]) + '\n' +
            MakeShaderSourcePrefix(params.p) +
            codeString +
            kShaderLangEndTag[language] + '\n';
        return false;
    }

    // If surface is present - can't have anything else except vertex (which is a modifier function then)
    bool hasNonSurfaceShaders = false;
    for (int pt = kProgramVertex + 1; pt < kProgramSurface; ++pt)
    {
        if (params.HasProgram((ShaderCompilerProgram)pt))
        {
            hasNonSurfaceShaders = true;
            break;
        }
    }

    if (hasNonSurfaceShaders)
        errors.AddImportError("CGPROGRAM cannot contain #pragma surface as well as other programs", startLine, false);

    // Generate surface program
    core::string result;
    SurfaceParams spar;
    spar.source = codeString;
    spar.p = params.p;
    spar.o = params.o;
    spar.target = params.t;
    spar.shaderAPIs = params.shaderAPIs;
    spar.startLine = startLine;
    spar.includeContext = includeContext;
    ProcessSurfaceSnippet(spar, result, errors, callbacks);
    if (!errors.HasErrors())
    {
        output = result;
    }
    else
    {
        output = "// surface shader with errors was here\nPass { }\n/*" + result + "*/";
    }

    outErrors.AddErrorsFrom(errors);
    return true;
}

static bool PreprocessProgram(const IncludeSearchContext& includeContext, const core::string& codeString, int startLine, core::string& output, ShaderImportErrors& outErrors, int& snippetSequence, ShaderSourceLanguage language, ShaderCompilerPreprocessCallbacks* callbacks)
{
    bool res = false;

    ShaderImportErrors errors;

    // Generate code from surface functions
    if (language == kShaderSourceLanguageCg || language == kShaderSourceLanguageHLSL)
    {
        res = GenerateSurfaceShaderCode(includeContext, codeString, startLine, output, outErrors, snippetSequence, language, callbacks);
    }
    else
    {
        output.reserve(output.size() + kShaderLangBeginTagLength[language] + kShaderLangEndTagLength[language] + 3 /* for newlines */);

        const core::string endl("\n");
        output  = kShaderLangBeginTag[language]; output += endl;
        output += codeString; output += endl;
        output += kShaderLangEndTag[language]; output += endl;
    }

    if (errors.HasErrors())
    {
        outErrors.AddErrorsFrom(errors);
        output = "// Shader  with errors was here\nPass { }\n";
    }
    return res;
}

static bool ExtractShaderSnippets(const IncludeSearchContext& includeContext, const core::string& inCodeString, int startLine, core::string& output, ShaderImportErrors& outErrors, int& snippetSequence, ShaderSourceLanguage language, ShaderCompilerPreprocessCallbacks* callbacks)
{
    if (!callbacks)
        return false;

    core::string codeString = inCodeString;

    if (language == kShaderSourceLanguageGLSL)
    {
        // Hoist version directive that has most likely been buried below injected line directives and dummy macros.
        // (hoisting here also allows promoting GLSLPROGRAM directives above GLSLINCLUDE sections, which is a very convenient bonus feature)
        HoistGLSLVersionDirective(codeString);
    }

    // Compute include hashes
    UInt32 includesHash[4];
    std::vector<core::string> includedFiles;
    ComputeIncludesHash(codeString, includeContext, includesHash, includedFiles);

    SnippetParameters params;
    FindParameters(codeString, startLine, language, params, outErrors);
    ShaderCompileVariants variants;
    variants.FindVariantDirectives(codeString, !params.p.proceduralFuncName.empty(), outErrors);

    // If the snippet is one one "raw, no translation" languages, then mark it as only relevant for platforms that have them
    if (language == kShaderSourceLanguageGLSL)
    {
        // GLSL source, exclude all non-GLSL platforms
        params.shaderAPIs &= kShaderPlatformsHaveGLSLBlocks;
    }
    else if (language == kShaderSourceLanguageMetal)
    {
        params.shaderAPIs &= (1 << kShaderCompPlatformMetal);
    }

    // Add snippet to the Shader; snippet ID is: sequential number, and hash of the source string in 2 lowest bytes.
    // We use sequential ID part so that the same source text from same shader will result in different snippets,
    // and a hash so that UsePasses & fallbacks will work - a UsePass that pulls in snippets from another shader doesn't
    // want to clash with its own snippets.
    // We only use 24 bits since number constants are parsed as floats in ShaderLab, and these only map to 24 bit integers
    // exactly.
    unsigned snippetHash = ComputeStringHash32(codeString);
    int snippetID = (snippetSequence << 16) | (snippetHash & 0xFFFF);

    // GLSLPROGRAM snippets are special: they always have vertex+fragment parts
    const bool isGLSL = language == kShaderSourceLanguageGLSL;

    UInt32 programsMask = params.HasProgramsMask();
    if (isGLSL)
        programsMask = (1 << kProgramVertex) | (1 << kProgramFragment);

    callbacks->OnAddSnippet(snippetID, codeString.c_str(), startLine, params.shaderAPIs, params.hardwareTierVariants, programsMask, language, includesHash);
    callbacks->OnIncludeDependencies(includedFiles);
    output = "GpuProgramID " + IntToString(snippetID) + "\n";

    // Send keyword variant data
    for (int programType = 0; programType < kProgramCount; ++programType)
    {
        if (programsMask & (1 << programType))
        {
            callbacks->OnUserKeywordVariants(snippetID, (ShaderCompilerProgram)programType, variants.GetData().m_User[programType]);
            callbacks->OnBuiltinKeywordVariants(snippetID, (ShaderCompilerProgram)programType, variants.GetData().m_Builtin[programType]);
        }
    }
    UnityStrArray alwaysIncludedUserKeywords, builtinKeywords;
    variants.GetKeywordLists(alwaysIncludedUserKeywords, builtinKeywords);
    callbacks->OnEndKeywordCombinations(snippetID, alwaysIncludedUserKeywords, builtinKeywords, params.t);

    // have shadow keywords?
    bool hasShadows = false;
    if (std::find(builtinKeywords.begin(), builtinKeywords.end(), "SHADOWS_DEPTH") != builtinKeywords.end() ||
        std::find(builtinKeywords.begin(), builtinKeywords.end(), "SHADOWS_CUBE") != builtinKeywords.end() ||
        std::find(builtinKeywords.begin(), builtinKeywords.end(), "SHADOWS_SCREEN") != builtinKeywords.end())
    {
        hasShadows = true;
    }
    if (hasShadows)
        output += "Tags { \"ShadowSupport\" = \"True\" }\n";

    snippetSequence++;

    return false;
}

static bool CheckSnippetParameters(SnippetParameters& params, int startLine, ShaderImportErrors& errors)
{
    bool skipCompilation = false;

    Assert(!params.HasProgram(kProgramSurface));

    if (!params.HasAnyProgram())
    {
        errors.AddImportError("CGPROGRAM block does not contain #pragma vertex, #pragma fragment or #pragma surface directive, skipping", startLine, true);
        skipCompilation = true;
    }

    return skipCompilation;
}

int RunCgBatchUnitTests(const char* includesPath, const std::vector<core::string>& testFilters)
{
    g_IncludesPath = core::string(includesPath);

    CgBatchInitialize(g_IncludesPath);
    #if ENABLE_UNIT_TESTS
    // Now that we are initialized, do generate of dynamic test cases.
    Testing::ParametricTestBase::CreateAllTestInstances();
    #endif

    return RunNativeTests(testFilters);
}

void CgBatchSetIncludeSearchContext(const IncludeSearchContext& includeContext)
{
    g_IncludeSearchContext = includeContext;
}

const IncludeSearchContext& CgBatchGetIncludeSearchContext()
{
    return g_IncludeSearchContext;
}

void CgBatchClearIncludesCache()
{
    // Clear this process include cache.
    ClearReadTextFilesCache();
    // Clear the plugins includes cache (for compilers accessed through DLLs).
    gPluginDispatcher->ClearIncludesCache();
}

#if PLATFORM_WIN
#define CGBATCH_TIME DWORD
#else
#define CGBATCH_TIME timeval
#endif

void StartTimer(CGBATCH_TIME& startTime)
{
#if PLATFORM_WIN
    startTime = GetTickCount();
#else
    gettimeofday(&startTime, NULL);
#endif
}

float StopTimer(const CGBATCH_TIME startTime)
{
    CGBATCH_TIME endTime;
#if PLATFORM_WIN
    endTime = GetTickCount();
    float timeTaken = (endTime - startTime) * 0.001f;
#else
    gettimeofday(&endTime, NULL);
    CGBATCH_TIME diffTime;
    timersub(&endTime, &startTime, &diffTime);
    float timeTaken = diffTime.tv_sec + diffTime.tv_usec * 1.0e-6f;
#endif
    return timeTaken;
}

bool CgBatchPreprocessShader(
    const core::string& inputShaderStr,
    const core::string& inputPathStr,
    core::string& outputShader,
    ShaderCompilerPreprocessCallbacks* callbacks,
    bool surfaceGenerationOnly,
    bool* outHadSurfaceShaders
)
{
    outputShader.clear();
    *outHadSurfaceShaders = false;
    ShaderImportErrors errors;
    IncludeSearchContext includeContext = g_IncludeSearchContext;
    includeContext.InsertPriorityPath(inputPathStr);
    core::string inputShader = inputShaderStr;
    RemoveUTF8BOM(inputShader);

    CGBATCH_TIME startTime;
    StartTimer(startTime);

    int snippetSequence = 0;

    *outHadSurfaceShaders = ProcessPrograms(includeContext, inputShader, kShaderSourceLanguageCg, surfaceGenerationOnly ? GenerateSurfaceShaderCode : PreprocessProgram, snippetSequence, errors, callbacks);
    if (!surfaceGenerationOnly)
    {
        ProcessPrograms(includeContext, inputShader, kShaderSourceLanguageGLSL, PreprocessProgram, snippetSequence, errors, callbacks);
        ProcessPrograms(includeContext, inputShader, kShaderSourceLanguageMetal, PreprocessProgram, snippetSequence, errors, callbacks);
        ProcessPrograms(includeContext, inputShader, kShaderSourceLanguageHLSL, PreprocessProgram, snippetSequence, errors, callbacks);

        ProcessPrograms(includeContext, inputShader, kShaderSourceLanguageCg, ExtractShaderSnippets, snippetSequence, errors, callbacks);
        ProcessPrograms(includeContext, inputShader, kShaderSourceLanguageGLSL, ExtractShaderSnippets, snippetSequence, errors, callbacks);
        ProcessPrograms(includeContext, inputShader, kShaderSourceLanguageMetal, ExtractShaderSnippets, snippetSequence, errors, callbacks);
        ProcessPrograms(includeContext, inputShader, kShaderSourceLanguageHLSL, ExtractShaderSnippets, snippetSequence, errors, callbacks);
    }

    float timeTaken = StopTimer(startTime);
    core::string msg = Format("preprocessed shader in %.2fs", timeTaken);
    if (callbacks)
        callbacks->OnError(kCgBatchErrorInfo, -1, 0, "", msg.c_str());

    // copy to output
    outputShader = inputShader;

    errors.LogErrors(callbacks);

    return !errors.HasErrors();
}

const char* kShaderCompProgramTypeDefines[] =
{
    "SHADER_STAGE_VERTEX",
    "SHADER_STAGE_FRAGMENT",
    "SHADER_STAGE_HULL",
    "SHADER_STAGE_DOMAIN",
    "SHADER_STAGE_GEOMETRY",
    "", // surface shader, won't happen
};
CompileTimeAssertArraySize(kShaderCompProgramTypeDefines, kProgramCount);

static const core::string kPragmaWithSpace("#pragma ");
static const core::string kPragmaWithTab("#pragma\t");
static const core::string kPragmaWithSpaceCommented("//#prag ");
static const core::string kPragmaWithTabCommented("//#prag\t");
static const core::string kPragmaWithArgument("#pragma argument");
static const core::string kPragmaWithArgumentTemporary("#pragma_argument");

static void CompileHLSLCgShaderSnippet(
    const core::string& codeString,
    int startLine,
    const core::string& inputPathStr,
    const core::string* keywords,
    int keywordCount,
    CompileSnippetOptions options,
    ShaderSourceLanguage sourceLanguage,
    ShaderCompilerProgram programType,
    UInt32 programMask,
    ShaderCompilerPlatform platform,
    ShaderRequirements requirements,
    ShaderCompilerReflectionReport* reflectionReport,
    tReturnedVector& result,
    ShaderGpuProgramType& outGpuProgramType,
    ShaderImportErrors& outErrors)
{
    Assert(programType != kProgramSurface);

    result.clear();

    SnippetParameters params;
    params.shaderAPIs &= (1 << platform);
    FindParameters(codeString, startLine, sourceLanguage, params, outErrors);

    bool skipCompilation = CheckSnippetParameters(params, startLine, outErrors);
    if (skipCompilation || outErrors.HasErrors())
        return;

    ShaderCompileInputData inputData;
    inputData.includeContext = g_IncludeSearchContext;
    inputData.includeContext.InsertPriorityPath(inputPathStr);
    inputData.source = codeString;
    ConvertIncludePathSlashes(inputData.source);

    if (platform == kShaderCompPlatformPS4)
    {
        // for PSSL shaders we allow '#pragma argument' parameters to be passed through so we can affect shader compiling
        replace_string(inputData.source, kPragmaWithArgument, kPragmaWithArgumentTemporary); // replace #pragma argument with #pragma_argument so that it doesnt match in the next line
        replace_string(inputData.source, kPragmaWithSpace, kPragmaWithSpaceCommented); // remove pragmas
        replace_string(inputData.source, kPragmaWithTab, kPragmaWithTabCommented); // remove pragmas
        replace_string(inputData.source, kPragmaWithArgumentTemporary, kPragmaWithArgument); // replace back #pragma_argument with #pragma argument
    }
    else
    {
        replace_string(inputData.source, kPragmaWithSpace, kPragmaWithSpaceCommented); // remove pragmas
        replace_string(inputData.source, kPragmaWithTab, kPragmaWithTabCommented); // remove pragmas
    }

    inputData.programType = programType;
    inputData.programMask = programMask;
    inputData.p = params.p;
    inputData.startLine = 0; //@TODO: start line of the snippet, we should have it somewhere

    ShaderArgs args;
    AddShaderTargetRequirementsMacros(requirements, args);

    // For compatibility, downgrade requirements after the overall shader target define is set
    DowngradeShaderRequirements(sourceLanguage, startLine, params, requirements);

    for (int k = 0; k < keywordCount; ++k)
        args.push_back(ShaderCompileArg(keywords[k], "1"));
    // Add a UNITY_VERSION macro, that is for example 451 for 4.5.1
    args.push_back(ShaderCompileArg("UNITY_VERSION", IntToString(CreateNumericUnityVersion(UNITY_VERSION_VER, UNITY_VERSION_MAJ, UNITY_VERSION_MIN))));
    // Add a macro indicating the type of the program, or stage being compiled.
    args.push_back(ShaderCompileArg(kShaderCompProgramTypeDefines[programType], "1"));

    inputData.args = args;
    inputData.api = platform;
    inputData.requirements = requirements;
    inputData.compileFlags = params.p.compileFlags;
    if (HasFlag(options, kCompileSnippetMetalUsePointFilterForShadows))
        inputData.compileFlags |= kShaderCompFlags_MetalUsePointFilterForShadows;
    inputData.reflectionReport = reflectionReport;
    const ShaderCompileOutputInterface* output = gPluginDispatcher->CompileShader(&inputData);

    //@TODO: do something with stats


    AddErrorsFrom(outErrors, &inputData, output);
    if (!outErrors.HasErrors())
    {
        const UInt8* outputData = output->GetCompiledDataPtr();
        if (outputData != NULL)
            result.assign(outputData, outputData + output->GetCompiledDataSize());
        outGpuProgramType = output->GetGpuProgramType();
    }


    gPluginDispatcher->ReleaseShader(output);
}

/*
 This function extracts extensions from codeText and saves it as codeStringWithoutExtensions.
 Extraced extensions saved in extensionsString.
*/
static void ExtractExtensionsFromCodeString(
    const core::string& codeString,
    size_t positionOffset,
    /* out */ core::string& codeStringWithoutExtensions,
    /* out */ core::string& extensionsString,
    /* out */ size_t& extensionCount,
    /* out */ ShaderImportErrors& outErrors,
    ShaderCompilerPlatform platform)
{
    codeStringWithoutExtensions.reserve(codeString.size()); // codeString >= codeStringWithoutExtensions in sense of size
    extensionCount = 0;
    size_t position = positionOffset;

    while (true)
    {
        size_t lastPosition = position;

        size_t startPositionOfExtensionString = FindTokenInText(codeString, "#extension ", lastPosition);
        if (startPositionOfExtensionString == codeString.npos) // Theres is not extensions left
            break;

        size_t endPositionOfExtensionString = codeString.find('\n', startPositionOfExtensionString);
        if (endPositionOfExtensionString == codeString.npos)
        {
            // If we reach this part, means someone forgot to add newline after extension
            outErrors.AddImportError("#extension directive missing the line ending.", 0, false, platform);
            return;
        }

        // Append the code until the extension starts
        codeStringWithoutExtensions.append(&codeString[lastPosition], startPositionOfExtensionString - lastPosition);

        // Add extension to stream
        size_t sizeOfExtensionString = endPositionOfExtensionString - startPositionOfExtensionString;
        const char* pointerToExtensionStringWithNoTermination = &codeString[startPositionOfExtensionString]; // This char array has no null terminator, don't use it for text
        extensionsString.append(pointerToExtensionStringWithNoTermination, sizeOfExtensionString);
        extensionsString += "\n";

        extensionCount++;
        position = endPositionOfExtensionString;
    }

    // If there is no extensions, lets not waste the cpu power for appending the code end
    if (extensionCount != 0)
    {
        // Append the code ending
        codeStringWithoutExtensions.append(&codeString[position], codeString.size() - position); // position == endPositionOfExtensionString, in order to skip new line add one
    }
}

static void MoveExtensionsToPosition(
    const core::string& codeString,
    /* inout */ size_t& position,
    /* out */ core::string& resultString,
    /* out */ ShaderImportErrors& outErrors,
    ShaderCompilerPlatform platform)
{
    core::string codeStringWithoutExtensions;
    core::string extensionsString;
    size_t extensionCount;
    ExtractExtensionsFromCodeString(codeString, position,
        codeStringWithoutExtensions, extensionsString, extensionCount, outErrors, platform);

    if (outErrors.HasErrors())
        return;

    if (extensionCount != 0)
    {
        resultString = extensionsString + codeStringWithoutExtensions;
        position = extensionsString.size();
    }
}

static void ReplaceExternalTextureSamplerQualifiersToTexture2D(core::string& source)
{
    // Some extensions require to keep old style (Because they don't support the new one).
    // So we leave ability for user to specify the functions that shouldn't be changed.
    // For example: GL_OES_EGL_image_external (https://www.khronos.org/registry/gles/extensions/OES/OES_EGL_image_external.txt)
    // Case: https://fogbugz.unity3d.com/f/cases/816627/
    replace_string(source, "textureExternal", "texture2D");
    replace_string(source, "textureProjExternal", "texture2DProj");
}

static void CompileGLSLShaderSnippet(
    const core::string& codeString,
    int startLine,
    const core::string& inputPathStr,
    const core::string* keywords,
    int keywordCount,
    CompileSnippetOptions /*options*/,
    ShaderSourceLanguage sourceLanguage,
    ShaderCompilerProgram programType,
    UInt32 /*programMask*/,
    ShaderCompilerPlatform platform,
    ShaderRequirements requirements,
    ShaderCompilerReflectionReport* reflectionReport,
    tReturnedVector& result,
    ShaderGpuProgramType& outProgramType,
    ShaderImportErrors& outErrors)
{
    Assert(programType != kProgramSurface);

    SnippetParameters params; params.shaderAPIs = 1 << platform;
    FindParameters(codeString, startLine, sourceLanguage, params, outErrors);
    if (params.shaderAPIs == 0 || outErrors.HasErrors())
        return;

    core::string defineKeywords;
    for (int k = 0; k < keywordCount; ++k)
        defineKeywords += core::string("#define ") + keywords[k] + " 1\n";

    // Find the #version line
    size_t posForInserts = codeString.find("#version ");
    if (posForInserts == core::string::npos)
        posForInserts = 0;
    else
        posForInserts = codeString.find("\n", posForInserts) + 1;

    core::string resultString = codeString;

    // Find all extensions in codeString and move them to selected position
    MoveExtensionsToPosition(codeString, posForInserts, resultString, outErrors, platform);

    ShaderCompileInputData inputData;
    core::string inserts = defineKeywords;
    AddShaderTargetRequirementsMacros(requirements, inputData.args);
    inserts += GetGLSLDefinesString(&inputData, platform);

    static const core::string stereoMacro("UNITY_SETUP_STEREO_RENDERING");
    bool stereoMacroFound = resultString.find(stereoMacro) != core::string::npos;

    if (stereoMacroFound)
    {
        replace_string(resultString, stereoMacro, "");  // remove stereo setup macro once found
        inserts += "#include \"UnityStereoExtensions.glslinc\"\n";  // include stereo rendering extensions before GLSL support methods
    }

    inserts += "#include \"GLSLSupport.glslinc\"\n";

    if (stereoMacroFound)
    {
        inserts += "#include \"UnityStereoSupport.glslinc\"\n";  // include stereo rendering methods after GLSL support methods
    }

    // For compatibility, downgrade requirements after the overall shader target define is set
    DowngradeShaderRequirements(sourceLanguage, startLine, params, requirements);

    FixupGLSLFixedFunctionVariables(resultString, inserts);

    resultString.insert(posForInserts, inserts);

    IncludeSearchContext includeContext = g_IncludeSearchContext;
    includeContext.InsertPriorityPath(inputPathStr);
    ProcessIncludeDirectives(resultString, includeContext, outErrors, platform);
    if (outErrors.HasErrors())
        return;

    replace_string(resultString, "#pragma ", "// #pragma ");  // remove pragmas
    replace_string(resultString, "\"", "'");  // replace double quotes with single quotes, so we can put the string into ShaderLab's string later

    switch (platform)
    {
        default:
        case kShaderCompPlatformOpenGLCore:
            resultString = TranslateGLSLToGLUnifiedShader(resultString);
            break;
        case kShaderCompPlatformGLES20:
            resultString = TranslateGLSLToGLES20(resultString);
            break;
        case kShaderCompPlatformGLES3Plus:
            if (params.p.compileFlags & kShaderCompFlags_GLSL_ES2)
                resultString = TranslateGLSLToGLES20(resultString);
            else
                resultString = TranslateGLSLToGLES30Plus(resultString);
            break;
    }

    // Replace all textureExternal/textureProjExternal to texture2D/textureProj2D
    ReplaceExternalTextureSamplerQualifiersToTexture2D(resultString);
    outProgramType = GetGLSLGpuProgramType(requirements, platform);

    result.assign(resultString.begin(), resultString.end());
}

static void CompileMetalShaderSnippet(
    const core::string& codeString,
    int startLine,
    const core::string& inputPathStr,
    const core::string* keywords,
    int keywordCount,
    CompileSnippetOptions /*options*/,
    ShaderSourceLanguage sourceLanguage,
    ShaderCompilerProgram programType,
    UInt32 /*programMask*/,
    ShaderCompilerPlatform platform,
    ShaderRequirements requirements,
    ShaderCompilerReflectionReport* reflectionReport,
    tReturnedVector& result,
    ShaderGpuProgramType& outProgramType,
    ShaderImportErrors& outErrors)
{
    Assert(programType != kProgramSurface);

    SnippetParameters params;
    params.shaderAPIs = (1 << kShaderCompPlatformMetal);
    FindParameters(codeString, startLine, sourceLanguage, params, outErrors);

    bool skipCompilation = CheckSnippetParameters(params, startLine, outErrors);
    if (skipCompilation || outErrors.HasErrors())
        return;

    DebugAssert(programType == kProgramVertex || programType == kProgramFragment);
    outProgramType = programType == kProgramVertex ? kShaderGpuProgramMetalVS : kShaderGpuProgramMetalFS;

    core::string defineKeywords;
    for (int k = 0; k < keywordCount; ++k)
        defineKeywords += core::string("#define ") + keywords[k] + " 1\n";

    // Add a macro indicating the current type of program, or stage being compiled.
    defineKeywords += core::string("#define ") + kShaderCompProgramTypeDefines[programType] + " 1\n";

    core::string resultString = defineKeywords + codeString;

    replace_string(resultString, "#pragma ", "// #pragma ");  // remove pragmas
    replace_string(resultString, "\"", "'");  // replace double quotes with single quotes, so we can put the string into ShaderLab's string later

    const char* const globalCbName = programType == kProgramVertex ? "VGlobals" : "FGlobals";

    if (ProcessMetalUniforms(resultString, globalCbName, reflectionReport, outErrors)
        &&  ProcessMetalVertexInputs(resultString, reflectionReport, outErrors)
        &&  ProcessMetalTextures(resultString, reflectionReport, outErrors)
        &&  ProcessMetalBuffers(resultString, reflectionReport, outErrors)
    )
    {
        WriteShaderSnippetBlob(result, params.p.entryName[programType].c_str(), resultString);
    }
}

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
)
{
    ShaderImportErrors outErrors;
    IncludeSearchContext includeContext = g_IncludeSearchContext;
    includeContext.InsertPriorityPath(DeleteLastPathNameComponent(inputPathStr));
    RemoveUTF8BOM(inoutShaderStr);

    ComputeImportDirectives importDirectives;
    // Finds all the kernel pragmas and their macro definitions, commenting them out from the source
    bool ok = ParseComputeImportDirectives(inoutShaderStr, importDirectives, supportedAPIs);

    //TODO: remove the whole ComputeImportDirectives struct together with all the serialization stuff from CgBatch
    for (int i = 0; i < importDirectives.kernels.size(); i++)
    {
        outKernelNames.push_back(importDirectives.kernels[i].name);
        MacroArray& macroArray = outMacros.emplace_back();

        for (int j = 0; j < importDirectives.kernels[i].macros.size(); j++)
        {
            macroArray.push_back(importDirectives.kernels[i].macros[j]);
        }
    }

    outCompileFlags = importDirectives.compileFlags;

    ComputeIncludesHash(inoutShaderStr, includeContext, outIncludesHash, outIncludeFiles);

    if (errorCallback)
        outErrors.LogErrors(errorCallback, errorCallbackData);

    return ok && !outErrors.HasErrors();
}

bool CgBatchCompileComputeKernel(
    const core::string& inputCode,
    const core::string& inputPathStr,
    const core::string& kernelName,
    const dynamic_array<std::pair<core::string, core::string> >& macros,
    tReturnedVector & result,
    ShaderCompilerPlatform platform,
    ShaderCompilationFlags compileFlags,
    CgBatchErrorCallback errorCallback,
    void* errorCallbackData)
{
    ShaderCompileInputData   inputData;

    inputData.source = inputCode;
    ConvertIncludePathSlashes(inputData.source);
    inputData.sourceFilename = inputPathStr;
    inputData.includeContext = g_IncludeSearchContext;
    inputData.includeContext.InsertPriorityPath(DeleteLastPathNameComponent(inputPathStr));
    inputData.sourceLanguage = StripComputeShaderSourceVariants(inputData.source, platform);
    inputData.api = platform;
    inputData.compileFlags = compileFlags;

    inputData.p.entryName[0] = kernelName;

    for (int m = 0; m < macros.size(); m++)
        inputData.args.push_back(ShaderCompileArg(macros[m].first, macros[m].second));

    const ShaderCompileOutputInterface* output = gPluginDispatcher->CompileComputeShader(&inputData);

    ShaderImportErrors errors;
    AddErrorsFrom(errors, &inputData, output);

    if (errorCallback)
        errors.LogErrors(errorCallback, errorCallbackData);

    bool hasErrors = errors.HasErrors();

    if (!hasErrors)
    {
        const UInt8* outputData = output->GetCompiledDataPtr();
        if (outputData != NULL)
            result.assign(outputData, outputData + output->GetCompiledDataSize());
    }

    gPluginDispatcher->ReleaseShader(output);

    return !hasErrors;
}

bool CgBatchTranslateComputeKernel(
    ShaderCompilerPlatform platform,
    ShaderCompilationFlags compileFlags,
    const dynamic_array<UInt8> &inputShader,
    dynamic_array<UInt8> &outputShader,
    CgBatchErrorCallback errorCallback,
    void* errorCallbackData)
{
    ShaderCompileInputData inputData;
    inputData.gpuProgramData = inputShader;
    inputData.api = platform;
    inputData.compileFlags = compileFlags;

    const ShaderCompileOutputInterface* output = gPluginDispatcher->TranslateComputeKernel(&inputData);

    ShaderImportErrors errors;
    AddErrorsFrom(errors, &inputData, output);

    if (errorCallback)
        errors.LogErrors(errorCallback, errorCallbackData);

    bool hasErrors = errors.HasErrors();

    if (!hasErrors)
    {
        const UInt8* outputData = output->GetCompiledDataPtr();
        if (outputData != NULL)
            outputShader.assign(outputData, outputData + output->GetCompiledDataSize());
    }

    gPluginDispatcher->ReleaseShader(output);

    return !hasErrors;
}

bool CgBatchDisassembleShader(
    ShaderCompilerPlatform platform,
    ShaderGpuProgramType programType,
    UInt32 programMask,
    const dynamic_array<UInt8> &inputShader,
    core::string &outputDisassembly,
    CgBatchErrorCallback errorCallback,
    void* errorCallbackData
)
{
    ShaderCompileInputData   inputData;

    inputData.gpuProgramData = inputShader;
    inputData.gpuProgramType = programType;
    inputData.programMask = programMask;

    inputData.sourceLanguage = kShaderSourceLanguageCg;
    inputData.api = platform;

    const ShaderCompileOutputInterface* output = gPluginDispatcher->DisassembleShader(&inputData);

    ShaderImportErrors errors;
    AddErrorsFrom(errors, &inputData, output);

    if (errorCallback)
        errors.LogErrors(errorCallback, errorCallbackData);

    outputDisassembly = output->GetDisassembly();

    return !errors.HasErrors();
}

typedef void (*CompileSnippetFunction)(
    const core::string& codeString, int startLine, const core::string& inputPathStr, const core::string* keywords, int keywordCount, CompileSnippetOptions options,
    ShaderSourceLanguage sourceLanguage, ShaderCompilerProgram programType, UInt32 programMask, ShaderCompilerPlatform platform, ShaderRequirements requirements,
    ShaderCompilerReflectionReport* reflectionReport, tReturnedVector& result, ShaderGpuProgramType& outGpuProgramType, ShaderImportErrors& outErrors
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
)
{
    CompileSnippetFunction compileFunc[] = { &CompileHLSLCgShaderSnippet, &CompileGLSLShaderSnippet, &CompileMetalShaderSnippet, &CompileHLSLCgShaderSnippet };
    CompileTimeAssertArraySize(compileFunc, kShaderSourceLanguageCount);

    outputCode.clear();
    outGpuProgramType = kShaderGpuProgramUnknown;

    ShaderImportErrors errors;
    compileFunc[language](
        inputCode, startLine, inputPathStr, keywords, platformKeywordCount + userKeywordCount,
        options, language, programType, programMask, platform, requirements, reflectionReport, outputCode, outGpuProgramType, errors
    );

    errors.MakeErrorSetSaner();
    errors.AddSnippetInfoToErrorMessages(kProgramTypeNames[programType], platformKeywordCount, userKeywordCount, keywords);
    if (errorCallback)
        errors.LogErrors(errorCallback, errorCallbackData);

    return !errors.HasErrors();
}

void CgBatchFreeOutputShader(char* shaderStr)
{
    if (shaderStr)
        free(shaderStr);
}

core::string CgBatchInitialize(const core::string& appPath)
{
    return CgBatchInitialize(appPath, "");
}

core::string CgBatchInitialize(const core::string& appPath, const core::string& modulesLocation)
{
#if PLATFORM_WIN
    wchar_t exepathW[MAX_PATH];
    GetModuleFileNameW(NULL, exepathW, MAX_PATH);
    core::string exepath = WideToUtf8(exepathW);
#elif PLATFORM_OSX
    char path[PATH_MAX];
    uint32_t size = sizeof(path);
    _NSGetExecutablePath(path, &size);
    core::string exepath = path;
#elif PLATFORM_LINUX
    char path[PATH_MAX];
    AssertMsg(realpath("/proc/self/exe", path) != NULL, "Error looking up executable path");
    core::string exepath = path;
#else
#error Unknown platform
#endif

    CreateJobQueue("Job", "Worker", UNITY_DEFAULT_NUM_JOB_WORKER_THREADS);

    // initialize plugin dispatcher that will be passed to platform plugins
    InitializeDispatcher();

    // replace the .exe name with the folder to search for plugins
    ConvertSeparatorsToUnity(exepath);
    core::string exefolder = DeleteLastPathNameComponent(exepath);
#if PLATFORM_ARCH_64
    core::string pluginpath = AppendPathName(exefolder, "/CgBatchPlugins64");
#else
    core::string pluginpath = AppendPathName(exefolder, "/CgBatchPlugins");
#endif
    // scan a directory and load all the CgBatchPlugins it finds ... we can call ReadPlatformPlugins() for as many directories as we want
    // looking in a folder called CgBatchPlugins from where the exe is run from ... you'll want to change this if your debugging a plugin
    ReadPlatformPlugins(pluginpath);

    // Now scan all directories at ../PlaybackEngines/PLATFORM/CgBatchPlugins/    ... this allows installed playback engines to work
    core::string datafolder = DeleteLastPathNameComponent(exefolder);
    core::string playbackEnginePath = AppendPathName(datafolder, "/PlaybackEngines");
    std::set<core::string> allFiles;
    GetFolderContentsAtPath(playbackEnginePath, allFiles);
    if (!modulesLocation.empty())
    {
        GetFolderContentsAtPath(modulesLocation, allFiles);
    }
    for (std::set<core::string>::const_iterator i = allFiles.begin(); i != allFiles.end(); ++i)
    {
#if PLATFORM_ARCH_64
        const core::string& playbackEnginePluginFolder = AppendPathName(*i, "/CgBatchPlugins64/");
#else
        const core::string& playbackEnginePluginFolder = AppendPathName(*i, "/CgBatchPlugins/");
#endif
        if (IsDirectoryCreated(playbackEnginePluginFolder))
            ReadPlatformPlugins(playbackEnginePluginFolder);
    }

#if PLATFORM_OSX
    // Do not scan the root folder when quarantined/translocated, as FileSystem.Enumerate() will fail
    // The app is isolated from other folders by definition, so there's nothing to find anyway
    bool scanRootFolder = (datafolder.find("AppTranslocation") == core::string::npos);
#else
    bool scanRootFolder = true;
#endif

    if (scanRootFolder)
    {
        // Finally scan all directories next to the root (installation) folder    ... this allows locally built playback engines to work
        core::string rootfolder = DeleteLastPathNameComponent(DeleteLastPathNameComponent(datafolder));
    #if PLATFORM_OSX    // OSX keeps an additional folder for the Unity application (Unity.app/Contents)
        rootfolder = DeleteLastPathNameComponent(rootfolder);
    #endif
        allFiles.clear();
        GetFolderContentsAtPath(rootfolder, allFiles);
        for (std::set<core::string>::const_iterator i = allFiles.begin(); i != allFiles.end(); ++i)
        {
    #if PLATFORM_ARCH_64
            const core::string& playbackEnginePluginFolder = AppendPathName(*i, "/CgBatchPlugins64");
    #else
            const core::string& playbackEnginePluginFolder = AppendPathName(*i, "/CgBatchPlugins");
    #endif
            if (IsDirectoryCreated(playbackEnginePluginFolder))
                ReadPlatformPlugins(playbackEnginePluginFolder);
        }
    }

    RegisterRuntimeInitializeAndCleanup::ExecuteInitializations();

    return InitializeCompiler(appPath);
}

void CgBatchShutdown()
{
    ShutdownCompiler();
    GetJobQueue().Shutdown(JobQueue::kShutdownImmediate);
    DestroyJobQueue();
    RegisterRuntimeInitializeAndCleanup::ExecuteCleanup();
}

typedef void(*JsonSectionCallback) (yaml_document_t *document, yaml_node_pair_t* sectionheader);

void GlobalEnvVarHandler(yaml_document_t *document, yaml_node_pair_t* sectionheader)
{
    yaml_node_t * outervaluenode = yaml_document_get_node(document, sectionheader->value);
    if (outervaluenode && outervaluenode->type == YAML_MAPPING_NODE)
    {
        for (yaml_node_pair_t *nodepair = outervaluenode->data.mapping.pairs.start; nodepair < outervaluenode->data.mapping.pairs.top; nodepair++)
        {
            yaml_node_t *keynode = yaml_document_get_node(document, nodepair->key);
            yaml_node_t *valuenode = yaml_document_get_node(document, nodepair->value);
            if (keynode && keynode->type == YAML_SCALAR_NODE && valuenode && valuenode->type == YAML_SCALAR_NODE)
            {
                core::string envVar((const char*)keynode->data.scalar.value, keynode->data.scalar.length);
                core::string value((const char*)valuenode->data.scalar.value, valuenode->data.scalar.length);

                SetEnv(envVar.c_str(), value.c_str());
            }
        }
    }
}

/*
Example of json that can be supported.
{"globalenvvar":{"SCE_ORBIS_SDK_DIR":"E:/SCE/PS4 SDKs/5.508_021","testenvkey":"yes"},"othermap":{"SCE_ORBIS_SDK_DIR":"blah","testenvkey2":"no"}}

*/
void CgBatchConfigureSystemFromJson(core::string configurationData)
{
    // create callback configurations for parsing json ... this could be moved externally to this function and used for configuring different types of handler for platform specific uses
    std::map<core::string, JsonSectionCallback> sectionmap;
    sectionmap["globalenvvar"] = GlobalEnvVarHandler;   // add callback when a named/value object matching this name is found

    yaml_parser_t parser;
    yaml_document_t document;
    yaml_parser_initialize(&parser);
    yaml_parser_set_input_string(&parser, (const unsigned char*)configurationData.c_str(), configurationData.length());

    yaml_parser_load(&parser, &document);
    if (parser.error != YAML_NO_ERROR)
    {
        yaml_parser_delete(&parser);
        return;
    }

    yaml_parser_delete(&parser);
    yaml_node_t*rootnode = yaml_document_get_root_node(&document);

    if (rootnode && rootnode->type == YAML_MAPPING_NODE)
    {
        for (yaml_node_pair_t *rootnodepair = rootnode->data.mapping.pairs.start; rootnodepair < rootnode->data.mapping.pairs.top; rootnodepair++)
        {
            yaml_node_t * outerkeynode = yaml_document_get_node(&document, rootnodepair->key);
            if (outerkeynode && outerkeynode->type == YAML_SCALAR_NODE)
            {
                auto result = sectionmap.find(core::string((const char*)outerkeynode->data.scalar.value, outerkeynode->data.scalar.length));
                if (result != sectionmap.end())
                {
                    result->second(&document, rootnodepair);    // call the callback function
                }
            }
        }
    }
}

#if ENABLE_UNIT_TESTS

INTEGRATION_TEST_SUITE(CgBatch)
{
    TEST(SurfaceParamsSimple)
    {
        SnippetParameters o; ShaderImportErrors errors;
        FindSurfaceParameters("surf Lambert", 0, "", 1, o, errors);
        CHECK(!errors.HasErrorsOrWarnings());
        CHECK_EQUAL("surf", o.p.entryName[kProgramSurface]);
        CHECK_EQUAL("Lambert", o.o.lighting);
        CHECK_EQUAL(~0, o.o.renderPaths);
        CHECK_EQUAL(false, o.o.softVegetation);
        CHECK_EQUAL(false, o.o.noLightmap);
        CHECK_EQUAL(false, o.o.noAmbient);
        CHECK_EQUAL(false, o.o.noVertexLights);
        CHECK_EQUAL(false, o.o.noForwardAdd);
        CHECK_EQUAL(false, o.o.halfAsViewDir);
        CHECK_EQUAL(false, o.o.fullForwardShadows);
        CHECK_EQUAL(false, o.o.dualForward);
        CHECK_EQUAL(false, o.o.noLPPV);
        CHECK_EQUAL(false, o.o.noDynLightmap);
        CHECK_EQUAL(false, o.o.noDirLightmap);
        CHECK_EQUAL(false, o.o.noShadow);
        CHECK_EQUAL(false, o.o.noShadowMask);
        CHECK_EQUAL(false, o.o.noColorMask);
    }
    TEST(SurfaceParamsExcludePath)
    {
        SnippetParameters o; ShaderImportErrors errors;
        FindSurfaceParameters("surf Lambert exclude_path:forward", 0, "", 1, o, errors);
        CHECK(!errors.HasErrorsOrWarnings());
        CHECK(!(o.o.renderPaths & (1 << kPathForward)));
    }
    TEST(SurfaceParamsSoftVeg)
    {
        SnippetParameters o; ShaderImportErrors errors;
        FindSurfaceParameters("surf Lambert softvegetation", 0, "", 1, o, errors);
        CHECK(!errors.HasErrorsOrWarnings());
        CHECK_EQUAL(true, o.o.softVegetation);
    }
    TEST(SurfaceParamsNoLightmap)
    {
        SnippetParameters o; ShaderImportErrors errors;
        FindSurfaceParameters("surf Lambert nolightmap", 0, "", 1, o, errors);
        CHECK(!errors.HasErrorsOrWarnings());
        CHECK_EQUAL(true, o.o.noLightmap);
    }
    TEST(SurfaceParamsNoAmbient)
    {
        SnippetParameters o; ShaderImportErrors errors;
        FindSurfaceParameters("surf Lambert noambient", 0, "", 1, o, errors);
        CHECK(!errors.HasErrorsOrWarnings());
        CHECK_EQUAL(true, o.o.noAmbient);
    }
    TEST(SurfaceParamsNoVertexLights)
    {
        SnippetParameters o; ShaderImportErrors errors;
        FindSurfaceParameters("surf Lambert novertexlights", 0, "", 1, o, errors);
        CHECK(!errors.HasErrorsOrWarnings());
        CHECK_EQUAL(true, o.o.noVertexLights);
    }
    TEST(SurfaceParamsNoLPPV)
    {
        SnippetParameters o; ShaderImportErrors errors;
        FindSurfaceParameters("surf Lambert nolppv", 0, "", 1, o, errors);
        CHECK(!errors.HasErrorsOrWarnings());
        CHECK_EQUAL(true, o.o.noLPPV);
    }
    TEST(SurfaceParamsFullFwdShadows)
    {
        SnippetParameters o; ShaderImportErrors errors;
        FindSurfaceParameters("surf Lambert fullforwardshadows", 0, "", 1, o, errors);
        CHECK(!errors.HasErrorsOrWarnings());
        CHECK_EQUAL(true, o.o.fullForwardShadows);
    }
    TEST(SurfaceParamsDualFwd)
    {
        SnippetParameters o; ShaderImportErrors errors;
        FindSurfaceParameters("surf Lambert dualforward", 0, "", 1, o, errors);
        CHECK(!errors.HasErrorsOrWarnings());
        CHECK_EQUAL(true, o.o.dualForward);
    }
    TEST(SurfaceParamsNoColorMask)
    {
        SnippetParameters o; ShaderImportErrors errors;
        FindSurfaceParameters("surf Lambert nocolormask", 0, "", 1, o, errors);
        CHECK(!errors.HasErrorsOrWarnings());
        CHECK_EQUAL(true, o.o.noColorMask);
    }

    TEST(CgBatch_FindParameters_RecordsPragmas_ButNotEntryPoints)
    {
        SnippetParameters o; ShaderImportErrors errors;
        FindParameters("#pragma surface surf Lambert\n#pragma multi_compile BLACK WHITE\n#pragma debug", 1, kShaderSourceLanguageCg, o, errors);
        CHECK(!errors.HasErrorsOrWarnings());
        CHECK_EQUAL("#pragma multi_compile BLACK WHITE\n#pragma debug\n", o.o.pragmaDirectives);
    }

    TEST(CgBatch_PragmaExcludeRenderers_ParsedCorrectly)
    {
        SnippetParameters o; ShaderImportErrors errors;
        o.p.entryName[kProgramVertex] = "vert"; o.p.entryName[kProgramFragment] = "frag";
        o.shaderAPIs = ~0;
        FindParameters("#pragma exclude_renderers gles\n", 1, kShaderSourceLanguageCg, o, errors);
        CHECK(!errors.HasErrorsOrWarnings());
        CHECK_EQUAL(~(1 << kShaderCompPlatformGLES20), o.shaderAPIs);
    }

    TEST(CgBatch_PragmaTarget_ParsedCorrectly)
    {
        const char* s =
            "#pragma vertex vert\n#pragma fragment frag\n"
            "#pragma target 3.0\n";
        SnippetParameters sp; ShaderImportErrors err;
        FindParameters(s, 1, kShaderSourceLanguageCg, sp, err);
        CHECK(!err.HasErrorsOrWarnings());
        CHECK_EQUAL(kShaderRequireShaderModel30, sp.t.m_BaseRequirements);
        CHECK_EQUAL(kShaderTarget30, GetApproximateShaderTargetLevelFromRequirementsMask(sp.t.m_BaseRequirements));
        // Count built-in keywords with special target requirements (see AddBuiltinKeywordTargetRequirements)
        CHECK_EQUAL(9, sp.t.m_KeywordTargetInfo.size());
    }

    TEST(CgBatch_PragmaTarget_ParsedCorrectly_TabSpacing)
    {
        const char* s =
            "#pragma     vertex\t\tvert\n#pragma\t   fragment    \t  frag\n"
            "#pragma target 3.0\n";
        SnippetParameters sp; ShaderImportErrors err;
        FindParameters(s, 1, kShaderSourceLanguageCg, sp, err);
        CHECK(!err.HasErrorsOrWarnings());
        CHECK_EQUAL(kShaderRequireShaderModel30, sp.t.m_BaseRequirements);
    }

    TEST(CgBatch_PragmaTarget_GeometryShader_AddsGSToRequirements_WithoutWarnings)
    {
        const char* s =
            "#pragma vertex vert\n"
            "#pragma fragment frag\n"
            "#pragma geometry geom\n";
        SnippetParameters sp; ShaderImportErrors err;
        FindParameters(s, 1, kShaderSourceLanguageCg, sp, err);
        CHECK(!err.HasErrorsOrWarnings());
        CHECK(HasFlag(sp.t.m_BaseRequirements, kShaderRequireGeometry));
    }

    TEST(CgBatch_PragmaTarget_GeometryShader_AddsGSToRequirements_IfNotSufficientPragmaTargetWasSpecified)
    {
        const char* s =
            "#pragma vertex vert\n"
            "#pragma fragment frag\n"
            "#pragma geometry geom\n"
            "#pragma target 3.0\n";
        SnippetParameters sp; ShaderImportErrors err;
        FindParameters(s, 1, kShaderSourceLanguageCg, sp, err);
        CHECK_EQUAL(1, err.GetErrors().size()); // warning reported
        CHECK(HasFlag(sp.t.m_BaseRequirements, kShaderRequireGeometry));
    }

    TEST(CgBatch_PragmaTarget_DomainShader_AddsTessToRequirements_WithoutWarnings)
    {
        const char* s =
            "#pragma vertex vert\n"
            "#pragma fragment frag\n"
            "#pragma domain dom\n";
        SnippetParameters sp; ShaderImportErrors err;
        FindParameters(s, 1, kShaderSourceLanguageCg, sp, err);
        CHECK(!err.HasErrorsOrWarnings());
        CHECK(HasFlag(sp.t.m_BaseRequirements, kShaderRequireTessellation));
    }

    TEST(CgBatch_PragmaTarget_DomainShader_AddsTessToRequirements_IfNotSufficientPragmaTargetWasSpecified)
    {
        const char* s =
            "#pragma vertex vert\n"
            "#pragma fragment frag\n"
            "#pragma domain dom\n"
            "#pragma target 4.0\n";
        SnippetParameters sp; ShaderImportErrors err;
        FindParameters(s, 1, kShaderSourceLanguageCg, sp, err);
        CHECK_EQUAL(1, err.GetErrors().size()); // warning reported
        CHECK(HasFlag(sp.t.m_BaseRequirements, kShaderRequireTessellation));
    }

    TEST(CgBatch_PragmaTarget_DomainWithoutHull_DowngradeShaderRequirements_DoesNotRemoveTessellationRequirement)
    {
        const char* s =
            "#pragma vertex vert\n"
            "#pragma fragment frag\n"
            "#pragma domain dom\n";
        SnippetParameters sp; ShaderImportErrors err;
        FindParameters(s, 1, kShaderSourceLanguageCg, sp, err);

        DowngradeShaderRequirements(kShaderSourceLanguageCg, 1, sp, sp.t.m_BaseRequirements);
        CHECK(!err.HasErrorsOrWarnings());
        CHECK(HasFlag(sp.t.m_BaseRequirements, kShaderRequireTessellation));
    }

    TEST(PragmaTarget_Target20_SetsExpectedTarget)
    {
        const char* s =
            "#pragma vertex vert\n#pragma fragment frag\n"
            "#pragma target 2.0\n"; // default is 2.5; but indicating a 2.0 should make it the target (and not union of 2.0+2.5)
        SnippetParameters sp; ShaderImportErrors err;
        FindParameters(s, 1, kShaderSourceLanguageCg, sp, err);
        CHECK(!err.HasErrorsOrWarnings());
        CHECK_EQUAL(kShaderRequireShaderModel20, sp.t.m_BaseRequirements);
    }

    TEST(PragmaTarget_MultiplePresent_UnionIsPutIntoRequirements)
    {
        const char* s =
            "#pragma vertex vert\n#pragma fragment frag\n"
            "#pragma target 4.6\n"
            "#pragma target 4.5\n";
        SnippetParameters sp; ShaderImportErrors err;
        FindParameters(s, 1, kShaderSourceLanguageCg, sp, err);
        CHECK(!err.HasErrorsOrWarnings());
        CHECK_EQUAL(kShaderRequireShaderModel45_ES31, sp.t.m_BaseRequirements);
        CHECK_EQUAL(kShaderTarget45_ES31, GetApproximateShaderTargetLevelFromRequirementsMask(sp.t.m_BaseRequirements));
    }

    TEST(PragmaRequire_UnionOfAllFeatures_IsPutIntoBaseRequirements)
    {
        const char* s =
            "#pragma vertex vert\n#pragma fragment frag\n"
            "#pragma target 2.0\n"
            "#pragma require mrt4 integers cubearray\n"
            "#pragma require randomWrite\n";
        SnippetParameters sp; ShaderImportErrors err;
        FindParameters(s, 1, kShaderSourceLanguageCg, sp, err);
        CHECK(!err.HasErrorsOrWarnings());
        CHECK_EQUAL(kShaderRequireBaseShaders | kShaderRequireMRT4 | kShaderRequireInterpolators15Integers | kShaderRequireCubeArray | kShaderRequireRandomWrite, sp.t.m_BaseRequirements);
    }

    TEST(PragmaRequire_FeaturesWithKeywordLists_ParsedCorrectly)
    {
        const char* s =
            "#pragma vertex vert\n#pragma fragment frag\n"
            "#pragma target 2.0\n"
            "#pragma require cubearray\n"
            "#pragma require randomWrite : A B\n"
            "#pragma require mrt8 : C\n";
        SnippetParameters sp; ShaderImportErrors err;
        FindParameters(s, 1, kShaderSourceLanguageCg, sp, err);
        CHECK(!err.HasErrorsOrWarnings());
        CHECK_EQUAL(kShaderRequireBaseShaders | kShaderRequireCubeArray, sp.t.m_BaseRequirements);
        CHECK_EQUAL(kShaderRequireBaseShaders | kShaderRequireCubeArray | kShaderRequireRandomWrite, sp.t.GetRequirementsForKeyword("A"));
        CHECK_EQUAL(kShaderRequireBaseShaders | kShaderRequireCubeArray | kShaderRequireRandomWrite, sp.t.GetRequirementsForKeyword("B"));
        CHECK_EQUAL(kShaderRequireBaseShaders | kShaderRequireCubeArray | kShaderRequireMRT8, sp.t.GetRequirementsForKeyword("C"));
    }

    TEST(CgBatch_PragmaTarget_WithKeywords_OnOneLine_ParsedCorrectly)
    {
        const char* s =
            "#pragma vertex vert\n#pragma fragment frag\n"
            "#pragma target 3.0\n"
            "#pragma target 4.0 A B C\n";
        SnippetParameters sp; ShaderImportErrors err;
        FindParameters(s, 1, kShaderSourceLanguageCg, sp, err);
        CHECK(!err.HasErrorsOrWarnings());
        CHECK_EQUAL(kShaderRequireShaderModel30, sp.t.m_BaseRequirements);
        CHECK_EQUAL(kShaderRequireShaderModel40, sp.t.GetRequirementsForKeyword("A"));
        CHECK_EQUAL(kShaderRequireShaderModel40, sp.t.GetRequirementsForKeyword("B"));
        CHECK_EQUAL(kShaderRequireShaderModel40, sp.t.GetRequirementsForKeyword("C"));
    }

    TEST(CgBatch_PragmaTarget_WithKeywords_OnSeparateLines_ParsedCorrectly)
    {
        const char* s =
            "#pragma vertex vert\n#pragma fragment frag\n"
            "#pragma target 3.0\n"
            "#pragma target 4.0 A\n"
            "#pragma target 4.0 B\n";
        SnippetParameters sp; ShaderImportErrors err;
        FindParameters(s, 1, kShaderSourceLanguageCg, sp, err);
        CHECK(!err.HasErrorsOrWarnings());
        CHECK_EQUAL(kShaderRequireShaderModel30, sp.t.m_BaseRequirements);
        CHECK_EQUAL(kShaderRequireShaderModel40, sp.t.GetRequirementsForKeyword("A"));
        CHECK_EQUAL(kShaderRequireShaderModel40, sp.t.GetRequirementsForKeyword("B"));
    }

    TEST(CgBatch_PragmaTarget_WithTrailingComments_ParsedCorrectly)
    {
        const char* s =
            "#pragma vertex vert\n"
            "#pragma fragment frag\n"
            "#pragma target 4.0 // a comment\n";
        SnippetParameters sp; ShaderImportErrors err;
        FindParameters(s, 1, kShaderSourceLanguageCg, sp, err);
        CHECK(!err.HasErrorsOrWarnings());
        CHECK_EQUAL(kShaderRequireShaderModel35_ES3, sp.t.m_BaseRequirements);
    }

    TEST(CgBatch_PragmaOnlyRenderers_WithTrailingComments_ParsedCorrectly)
    {
        const char* s =
            "#pragma vertex vert\n"
            "#pragma fragment frag\n"
            "#pragma only_renderers d3d11 // commented out words";
        SnippetParameters sp; ShaderImportErrors err;
        FindParameters(s, 1, kShaderSourceLanguageCg, sp, err);
        CHECK(!err.HasErrorsOrWarnings());
        CHECK_EQUAL(1 << kShaderCompPlatformD3D11, sp.shaderAPIs);
    }


    struct CgBatch_ShaderCompilerPreprocessCallbacks_TestImpl : public ShaderCompilerPreprocessCallbacks
    {
    public:
        CgBatch_ShaderCompilerPreprocessCallbacks_TestImpl()
        {
            memset(m_KeywordVariants, 0, sizeof(m_KeywordVariants));
        }

        ShaderVariantData m_VariantData;
        int m_KeywordVariants[kProgramCount];

        virtual void OnError(CgBatchErrorType type, int platform, int line, const char* fileStr, const char* messageStr)
        {
            if (type == kCgBatchErrorError)
            {
                m_Errors.push_back(ShaderImportError(messageStr, fileStr, line, false, kShaderCompPlatformCount));
            }
            if (type == kCgBatchErrorWarning)
            {
                m_Warnings.push_back(ShaderImportError(messageStr, fileStr, line, true, kShaderCompPlatformCount));
            }
        }

        virtual void OnAddSnippet(const int id, const char* snippet, int startLine, UInt32 platformMask, UInt32 hardwareTierVariantsMask, UInt32 typesMask, ShaderSourceLanguage language, UInt32 includesHash[4])
        {
            m_Snippets.push_back(snippet);
            m_SnippetLanguages.push_back(language);
        }

        virtual void OnIncludeDependencies(const std::vector<core::string> &includedFiles)
        {
        }

        virtual void OnUserKeywordVariants(const int snippetID, const ShaderCompilerProgram programType, const std::vector<std::vector<core::string> >& keywords)
        {
            m_VariantData.m_User[programType] = keywords;
        }

        virtual void OnBuiltinKeywordVariants(const int snippetID, const ShaderCompilerProgram programType, const std::vector<std::vector<core::string> >& keywords)
        {
            m_VariantData.m_Builtin[programType] = keywords;
        }

        virtual void OnEndKeywordCombinations(const int snippetID, const std::vector<core::string>& alwaysIncludedUserKeywords, const std::vector<core::string>& builtinKeywords, const ShaderCompileRequirements& targetInfo)
        {
            ShaderVariantEnumeration variantEnum;
            for (int pt = 0; pt < kProgramCount; ++pt)
            {
                int count = (int)variantEnum.PrepareEnumeration(m_VariantData, (ShaderCompilerProgram)pt);
                // Special case: when there's only one variant, and it has no keywords defined,
                // then don't report it.
                if (count == 1)
                {
                    UnityStrArray keywords;
                    variantEnum.Enumerate(keywords);
                    if (keywords.empty())
                        count = 0;
                }
                m_KeywordVariants[pt] = count;
            }
        }

        std::vector<ShaderImportError> m_Errors;
        std::vector<ShaderImportError> m_Warnings;
        std::vector<core::string> m_Snippets;
        std::vector<ShaderSourceLanguage> m_SnippetLanguages;
    };

    struct CgBatchFixture
    {
        CgBatchFixture()
        {
            includeContext.includePaths.push_back(g_IncludesPath + "/CGIncludes");
        }

        ~CgBatchFixture()
        {
            CHECK_EQUAL("", errorText.c_str());
        }

        static void CompileErrorCallback(void* userData, CgBatchErrorType type, int platform, int line, const char* file, const char* message)
        {
            CgBatchFixture* fixture = static_cast<CgBatchFixture*>(userData);
            if (!fixture->errorText.empty()) fixture->errorText += "\n";
            fixture->errorText += Format("%s (line %d): %s", file ? file : "<Unknown file>", line, message);
        }

        void TestCompileSnippet(
            const core::string& inputCode,
            const core::string& inputPathStr,
            CompileSnippetOptions options,
            tReturnedVector& outputCode,
            ShaderGpuProgramType& outGpuProgramType,
            ShaderSourceLanguage language,
            ShaderCompilerProgram programType,
            UInt32 programMask,
            ShaderCompilerPlatform platform,
            ShaderRequirements requirements)
        {
            bool ok = CgBatchCompileSnippet(
                inputCode, 0, inputPathStr,
                NULL, 0, 0,
                options, outputCode, outGpuProgramType, language, programType, programMask, platform, requirements,
                &refl, CompileErrorCallback, this);
            CHECK(ok);
        }

        IncludeSearchContext includeContext;
        CgTestReflectionImpl refl;
        core::string res;
        ShaderImportErrors errors;
        CgBatch_ShaderCompilerPreprocessCallbacks_TestImpl callbacks;
        core::string errorText;
    };

    TEST(CgBatch_FindParameters_DetectsOnlyVertexOrFragmentPresent_AndExcludesAllPlatforms)
    {
        const char* s =
            "#pragma fragment frag\n";
        SnippetParameters params;
        ShaderImportErrors errors;
        FindParameters(s, 1, kShaderSourceLanguageCg, params, errors);
        // a warning to have only vertex or fragment
        CHECK_EQUAL(1, errors.GetErrors().size());
        CHECK_EQUAL(true, errors.GetErrors().begin()->warning);
        // all platforms should be excluded from compilation
        CHECK_EQUAL(0, params.shaderAPIs & kValidShaderCompPlatformMask);
    }

    TEST(CgBatch_FindParameters_DetectsOnlyVertexOrFragment_NoErrorIfAllExcluded)
    {
        const char* s =
            "#pragma fragment frag\n"
            "#pragma exclude_renderers shaderonly\n";
        SnippetParameters params;
        ShaderImportErrors errors;
        FindParameters(s, 1, kShaderSourceLanguageCg, params, errors);
        // no errors if only fragment shader, but all renderers are
        // excluded anyway
        CHECK(!errors.HasErrorsOrWarnings());
    }

    TEST(CgBatch_FindParameters_DetectionOfOnlyVertexOrFragment_DoesNotFailForGLSLPROGRAMBlocks)
    {
        // When we have GLSLPROGRAM block and we're parsing various #pragma statements
        // in it, ensure we don't accidentally treat it as "only vertex or fragment shader
        // was present". GLSLPROGRAM blocks don't have vertex or fragment pragma statements
        // to begin with.
        const char* s =
            "#pragma multi_compile foo bar baz\n";
        SnippetParameters params;
        ShaderImportErrors errors;
        FindParameters(s, 1, kShaderSourceLanguageGLSL, params, errors);
        CHECK(!errors.HasErrorsOrWarnings());
    }

    TEST(CgBatch_PragmaInstancingOptions_ParsedCorrectly)
    {
        const char* s = "#pragma instancing_options maxcount:10 assumeuniformscaling\n";
        SnippetParameters params;
        ShaderImportErrors errors;
        FindParameters(s, 1, kShaderSourceLanguageGLSL, params, errors);
        CHECK(!errors.HasErrorsOrWarnings());
        CHECK_EQUAL(10, params.p.maxInstanceCount);
        CHECK_EQUAL(true, HasFlag(params.p.compileFlags, kShaderCompFlags_AssumeUniformScaling));
    }

    TEST_FIXTURE(CgBatchFixture, PreprocessShader_GeneralErrors_AreReportedOnCorrectLines)
    {
        const char* s =
            "Shader \"Foo\" {\n"
            " SubShader {\n"
            "  Pass {\n"
            "   CGPROGRAM\n" // line 4, fragment program missing: error should be reported for this line
            "   #pragma vertex vert\n"
            "   ENDCG\n"
            "  }\n"
            " }\n"
            "}";
        bool hadSurface;
        CgBatchPreprocessShader(s, "", res, &callbacks, false, &hadSurface);
        CHECK_EQUAL(1, callbacks.m_Warnings.size());
        CHECK_EQUAL(4, callbacks.m_Warnings[0].line);
    }

    TEST_FIXTURE(CgBatchFixture, PreprocessShader_GeneralErrors_WithMultipleCGPROGRAMSnippets_AreReportedOnCorrectLines)
    {
        const char* s =
            "Shader \"Foo\" {\n"
            " SubShader {\n"
            "  Pass {\n"
            "   CGPROGRAM\n" // line 4, fragment program missing: error should be reported for this line
            "   #pragma vertex vert\n"
            "   ENDCG\n"
            "  }\n"
            "  Pass {\n"
            "   CGPROGRAM\n" // line 9, fragment program missing: error should be reported for this line
            "   #pragma vertex vert\n"
            "   ENDCG\n"
            "  }\n"
            " }\n"
            "}";
        bool hadSurface;
        CgBatchPreprocessShader(s, "", res, &callbacks, false, &hadSurface);
        CHECK_EQUAL(2, callbacks.m_Warnings.size());
        CHECK_EQUAL(4, callbacks.m_Warnings[0].line);
        CHECK_EQUAL(9, callbacks.m_Warnings[1].line);
    }

    TEST_FIXTURE(CgBatchFixture, PreprocessShader_GeneralErrors_WithCGINCLUDE_AreReportedOnCorrectLines)
    {
        const char* s =
            "Shader \"Foo\" {\n"
            " CGINCLUDE\n"
            " \n"
            " ENDCG\n"
            " SubShader {\n"
            "  Pass {\n"
            "   CGPROGRAM\n" // line 7, fragment program missing: error should be reported for this line
            "   #pragma vertex vert\n"
            "   ENDCG\n"
            "  }\n"
            "  Pass {\n"
            "   CGPROGRAM\n" // line 12, fragment program missing: error should be reported for this line
            "   #pragma vertex vert\n"
            "   ENDCG\n"
            "  }\n"
            " }\n"
            "}";
        bool hadSurface;
        CgBatchPreprocessShader(s, "", res, &callbacks, false, &hadSurface);
        CHECK_EQUAL(2, callbacks.m_Warnings.size());
        CHECK_EQUAL(7, callbacks.m_Warnings[0].line);
        CHECK_EQUAL(12, callbacks.m_Warnings[1].line);
    }

    TEST_FIXTURE(CgBatchFixture, PreprocessShader_PragmaErrors_AreReportedOnCorrectLines)
    {
        const char* s =
            "Shader \"Foo\" {\n"
            " SubShader {\n"
            "  Pass {\n"
            "   CGPROGRAM\n"
            "   #pragma vertex vert\n"
            "   #pragma fragment frag\n"
            "   #pragma foobar\n" // line 7, unknown pragma: warning should be reported for this line
            "   ENDCG\n"
            "  }\n"
            " }\n"
            "}";
        bool hadSurface;
        CgBatchPreprocessShader(s, "", res, &callbacks, false, &hadSurface);
        CHECK_EQUAL(1, callbacks.m_Warnings.size());
        CHECK_EQUAL(7, callbacks.m_Warnings[0].line);
    }

    TEST_FIXTURE(CgBatchFixture, PreprocessShader_PragmaErrors_WithCGINCLUDE_AreReportedOnCorrectLines)
    {
        const char* s =
            "Shader \"Foo\" {\n"
            " CGINCLUDE\n"
            " \n"
            " ENDCG\n"
            " SubShader {\n"
            "  Pass {\n"
            "   CGPROGRAM\n"
            "   #pragma vertex vert\n"
            "   #pragma fragment frag\n"
            "   #pragma foobar\n" // line 10, unknown pragma: warning should be reported for this line
            "   ENDCG\n"
            "  }\n"
            " }\n"
            "}";
        bool hadSurface;
        CgBatchPreprocessShader(s, "", res, &callbacks, false, &hadSurface);
        CHECK_EQUAL(1, callbacks.m_Warnings.size());
        CHECK_EQUAL(10, callbacks.m_Warnings[0].line);
    }

    TEST_FIXTURE(CgBatchFixture, PreprocessShader_PragmaErrors_WithComments_AreReportedOnCorrectLines)
    {
        const char* s =
            "Shader \"Foo\" {\n"
            " SubShader {\n"
            "  Pass {\n"
            "   CGPROGRAM\n"
            "   #pragma vertex vert\n"
            "   #pragma fragment frag\n"
            "\n"
            "   // C++ style comment\n"
            "\n"
            "   /* C style comment\n"
            "   *\n"
            "   *\n"
            "   */\n"
            "   #pragma foobar\n" // line 14, unknown pragma: warning should be reported for this line
            "   ENDCG\n"
            "  }\n"
            " }\n"
            "}";
        bool hadSurface;
        CgBatchPreprocessShader(s, "", res, &callbacks, false, &hadSurface);
        CHECK_EQUAL(1, callbacks.m_Warnings.size());
        CHECK_EQUAL(14, callbacks.m_Warnings[0].line);
    }

    TEST_FIXTURE(CgBatchFixture, PreprocessShader_WholeCGPROGRAM_OnOneLine_ParsedSuccessfully)
    {
        const char* s =
            "Shader \"Foo\" {\n"
            " CGINCLUDE\n"
            " #pragma vertex vert\n"
            " #pragma fragment frag\n"
            " ENDCG\n"
            " SubShader {\n"
            "  Pass {\n"
            // whole CGPROGRAM is on one line, with the rest pulled via CGINCLUDE above. Should work.
            "   CGPROGRAM float4 frag() : SV_Target { return 0.5; } ENDCG\n"
            "  }\n"
            " }\n"
            "}";
        bool hadSurface;
        CHECK(CgBatchPreprocessShader(s, g_IncludesPath + "/CGIncludes", res, &callbacks, false, &hadSurface));
        CHECK_EQUAL(0, callbacks.m_Errors.size());
        CHECK_EQUAL(0, callbacks.m_Warnings.size());
    }

    TEST_FIXTURE(CgBatchFixture, CgBatch_ExtractShaderSnippets_CorrectlyReportsNoVariants)
    {
        const char* s =
            "#pragma vertex vert\n"
            "#pragma fragment frag\n";
        int snippetSequence = 0;
        ExtractShaderSnippets(includeContext, s, 1, res, errors, snippetSequence, kShaderSourceLanguageCg, &callbacks);
        CHECK(!errors.HasErrorsOrWarnings());
        CHECK_EQUAL(0, callbacks.m_KeywordVariants[kShaderVertex]);
    }

    TEST_FIXTURE(CgBatchFixture, CgBatch_ExtractShaderSnippets_MultiCompileOneToken_CorrectlyReportsOneVariant)
    {
        const char* s =
            "#pragma vertex vert\n"
            "#pragma fragment frag\n"
            "#pragma multi_compile AAA\n";
        int snippetSequence = 0;
        ExtractShaderSnippets(includeContext, s, 1, res, errors, snippetSequence, kShaderSourceLanguageCg, &callbacks);
        CHECK_EQUAL(1, callbacks.m_KeywordVariants[kShaderVertex]);
    }

    TEST_FIXTURE(CgBatchFixture, CgBatch_ExtractShaderSnippets_MultiCompileTwoTokens_CorrectlyReportsTwoVariants)
    {
        const char* s =
            "#pragma vertex vert\n"
            "#pragma fragment frag\n"
            "#pragma multi_compile AAA BBB\n";
        int snippetSequence = 0;
        ExtractShaderSnippets(includeContext, s, 1, res, errors, snippetSequence, kShaderSourceLanguageCg, &callbacks);
        CHECK_EQUAL(2, callbacks.m_KeywordVariants[kShaderVertex]);
    }

    TEST_FIXTURE(CgBatchFixture, CgBatch_ExtractShaderSnippets_MultiCompileDummyToken_CorrectlyReportsTwoVariants)
    {
        const char* s =
            "#pragma vertex vert\n"
            "#pragma fragment frag\n"
            "#pragma multi_compile _ AAA\n";
        int snippetSequence = 0;
        ExtractShaderSnippets(includeContext, s, 1, res, errors, snippetSequence, kShaderSourceLanguageCg, &callbacks);
        CHECK_EQUAL(2, callbacks.m_KeywordVariants[kShaderVertex]);
    }

    struct CgBatch_ShaderCompilerPreprocessCallbacks_InstancingOptions : public CgBatch_ShaderCompilerPreprocessCallbacks_TestImpl
    {
        virtual void OnAddSnippet(const int id, const char* snippet, int startLine, UInt32 platformMask, UInt32 hardwareTierVariantsMask, UInt32 typesMask, ShaderSourceLanguage language, UInt32 includesHash[4])
        {
            bool hasSkipInstancing = strstr(snippet, "#pragma skip_variants INSTANCING_ON") != NULL;
            bool hasOptions = strstr(snippet, "#define UNITY_MAX_INSTANCE_COUNT") != NULL;
            CHECK(hasSkipInstancing || hasOptions); // if we have instancing we must have options defined.
        }
    };
    TEST_FIXTURE(CgBatchFixture, CgBatch_PragmaInstancingOptions_CorrectlyExpands)
    {
        bool hadSurface;
        CgBatch_ShaderCompilerPreprocessCallbacks_InstancingOptions cb;
        core::string includePath = g_IncludesPath + "/CGIncludes";

        const char* surface =
            "Shader \"Foo\" {\n"
            "  SubShader {\n"
            "    CGPROGRAM\n"
            "      #pragma surface surf Lambert\n"
            "      #pragma instancing_options maxcount:10\n"
            "      struct Input { float dummy; };\n"
            "      void surf(Input IN, inout SurfaceOutput o) {\n"
            "        o.Albedo = 1;\n"
            "      }\n"
            "    ENDCG\n"
            "  }\n"
            "}";
        CgBatchPreprocessShader(surface, includePath, res, &cb, false, &hadSurface);
        CHECK(!errors.HasErrorsOrWarnings());
        CHECK(hadSurface);

        const char* custom =
            "Shader \"Foo\" {\n"
            "  SubShader {\n"
            "    CGPROGRAM\n"
            "      #pragma vertex vert\n"
            "      #pragma fragment frag\n"
            "      #pragma instancing_options maxcount:10\n"
            "    ENDCG\n"
            "  }\n"
            "}";
        CgBatchPreprocessShader(custom, includePath, res, &cb, false, &hadSurface);
        CHECK(!errors.HasErrorsOrWarnings());
        CHECK(!hadSurface);
    }

    TEST_FIXTURE(CgBatchFixture, CGPROGRAM_Snippet_GetsHLSLSupportAndVariablesIncludesAdded)
    {
        bool hadSurface;
        const char* source =
            "CGPROGRAM\n"
            "#pragma vertex vert\n"
            "#pragma fragment frag\n"
            "ENDCG\n";
        CgBatchPreprocessShader(source, "", res, &callbacks, false, &hadSurface);
        CHECK_EQUAL(1, callbacks.m_Snippets.size());
        CHECK_EQUAL(kShaderSourceLanguageCg, callbacks.m_SnippetLanguages[0]);
        CHECK(callbacks.m_Snippets[0].find("HLSLSupport.cginc") != core::string::npos);
        CHECK(callbacks.m_Snippets[0].find("UnityShaderVariables.cginc") != core::string::npos);
        CHECK(callbacks.m_Snippets[0].find("UnityShaderUtilities.cginc") != core::string::npos);
    }

    TEST_FIXTURE(CgBatchFixture, HLSLPROGRAM_Snippet_DoesNotGetIncludesAdded)
    {
        bool hadSurface;
        const char* source =
            "HLSLPROGRAM\n"
            "#pragma vertex vert\n"
            "#pragma fragment frag\n"
            "ENDHLSL\n";
        CgBatchPreprocessShader(source, "", res, &callbacks, false, &hadSurface);
        CHECK_EQUAL(1, callbacks.m_Snippets.size());
        CHECK_EQUAL(kShaderSourceLanguageHLSL, callbacks.m_SnippetLanguages[0]);
        CHECK(callbacks.m_Snippets[0].find("#include") == core::string::npos);
    }

    // Check that GLSL version directive gets hoisted to the top of the produced snippet
    struct CgBatch_ShaderCompilerPreprocessCallbacks_CaptureSnippets_TestImpl : public CgBatch_ShaderCompilerPreprocessCallbacks_TestImpl
    {
        core::string m_Snippets;
        virtual void OnAddSnippet(const int id, const char* snippet, int startLine, UInt32 platformMask, UInt32 hardwareTierVariantsMask, UInt32 typesMask, ShaderSourceLanguage language, UInt32 includesHash[4]) { m_Snippets.append(snippet); }
    };
    TEST_FIXTURE(CgBatchFixture, CgBatch_GLSL_Hoist_Version_Directive)
    {
        const char* s =
            "Shader \"Foo\" {\n"
            "GLSLINCLUDE\n"
            "  #version 130\n"
            "  #ifdef VERTEX\n"
            "    void main () { gl_Position = gl_ModelViewProjectionMatrix * gl_Vertex; }\n"
            "  #endif\n"
            "  #ifdef FRAGMENT\n"
            "    void main() { gl_FragColor = vec4(0,0,0,0); }\n"
            "  #endif\n"
            "ENDGLSL\n"
            "SubShader {\n"
            "  Pass {\n"
            "    GLSLPROGRAM\n"
            "    ENDGLSL\n"
            "  }\n"
            "}\n"
            "}";

        bool hadSurface;
        CgBatch_ShaderCompilerPreprocessCallbacks_CaptureSnippets_TestImpl cb;
        CgBatchPreprocessShader(s, "", res, &cb, false, &hadSurface);
        CHECK(!errors.HasErrorsOrWarnings());
        CHECK_EQUAL(0, cb.m_Snippets.find("#version"));
    }

    // There was a bug when TANGENT used in a raw GLSLPROGRAM block would not get translated
    // into GLES3 correctly. Check that it produces correct output for both ES2 and ES3.
    TEST_FIXTURE(CgBatchFixture, CgBatch_GlslProgramSnippet_WithTangent_IsProcessedCorrectly)
    {
        const char* source =
            "#ifdef VERTEX\n"
            "\tattribute vec4 TANGENT;\n"
            "\tvoid main () {\n"
            "\t\tgl_Position = gl_ModelViewProjectionMatrix * gl_Vertex;\n"
            "\t\tgl_TexCoord[0] = TANGENT;\n"
            "\t}\n"
            "#endif\n"
            "#ifdef FRAGMENT\n"
            "\tvoid main() {\n"
            "\t\tgl_FragColor = gl_TexCoord[0];\n"
            "\t}\n"
            "#endif\n";

        UInt32 programMask = (1 << kProgramVertex) | (1 << kProgramFragment);
        core::string includePath = g_IncludesPath + "/CGIncludes";

        // Check GLES2
        tReturnedVector res2;
        ShaderGpuProgramType gpuProgType2;
        TestCompileSnippet(source, includePath, kCompileSnippetNone, res2, gpuProgType2, kShaderSourceLanguageGLSL, kProgramVertex, programMask, kShaderCompPlatformGLES20, kShaderRequireShaderModel20);
        CHECK_EQUAL(kShaderGpuProgramGLES, gpuProgType2);
        // copy to a string so that we can search.
        core::string res2str;
        res2str.assign((const char*)&res2[0], res2.size());
        // Should find vertex and tangent attribute definitions
        CHECK(res2str.find("#define gl_Vertex _glesVertex") != core::string::npos);
        CHECK(res2str.find("attribute vec4 _glesVertex;") != core::string::npos);
        CHECK(res2str.find("#define TANGENT _glesTANGENT") != core::string::npos);
        CHECK(res2str.find("attribute vec4 _glesTANGENT;") != core::string::npos);

        // Check GLES3
        tReturnedVector res3;
        ShaderGpuProgramType gpuProgType3;
        TestCompileSnippet(source, includePath, kCompileSnippetNone, res3, gpuProgType3, kShaderSourceLanguageGLSL, kProgramVertex, programMask, kShaderCompPlatformGLES3Plus, kShaderRequireShaderModel35_ES3);
        CHECK_EQUAL(kShaderGpuProgramGLES3, gpuProgType3);
        // copy to a string so that we can search.
        core::string res3str;
        res3str.assign((const char*)&res3[0], res3.size());
        // Should find vertex and tangent input definitions
        CHECK(res3str.find("#define gl_Vertex _glesVertex") != core::string::npos);
        CHECK(res3str.find("in vec4 _glesVertex;") != core::string::npos);
        CHECK(res3str.find("#define TANGENT _glesTANGENT") != core::string::npos);
        CHECK(res3str.find("in vec4 _glesTANGENT;") != core::string::npos);

        // Check GLES3.1
        tReturnedVector res31;
        ShaderGpuProgramType gpuProgType31;
        TestCompileSnippet(source, includePath, kCompileSnippetNone, res31, gpuProgType31, kShaderSourceLanguageGLSL, kProgramVertex, programMask, kShaderCompPlatformGLES3Plus, kShaderRequireShaderModel50);
        CHECK_EQUAL(kShaderGpuProgramGLES31, gpuProgType31);
        // copy to a string so that we can search.
        core::string res31str;
        res31str.assign((const char*)&res31[0], res31.size());
        // Should find vertex and tangent input definitions
        CHECK(res31str.find("#define gl_Vertex _glesVertex") != core::string::npos);
        CHECK(res31str.find("in vec4 _glesVertex;") != core::string::npos);
        CHECK(res31str.find("#define TANGENT _glesTANGENT") != core::string::npos);
        CHECK(res31str.find("in vec4 _glesTANGENT;") != core::string::npos);
    }

    TEST_FIXTURE(CgBatchFixture, CgBatch_GLSL_Test_Function_MoveExtensionsToPosition)
    {
        const char* source =
            "#ifdef VERTEX\n"
            "attribute vec4 _glesVertex;\n"
            "attribute vec4 _glesMultiTexCoord0;\n"
            "uniform highp mat4 glstate_matrix_mvp;\n"
            "varying mediump vec2 xlv_TEXCOORD0;\n"
            "void main()\n"
            "{\n"
            "gl_Position = (glstate_matrix_mvp * _glesVertex);\n"
            "vec2 tex = _glesMultiTexCoord0.xy - 0.5;\n"
            "xlv_TEXCOORD0 = vec2(_glesMultiTexCoord0.x, _glesMultiTexCoord0.y);\n"
            "}\n"
            "#endif\n"
            "#ifdef FRAGMENT\n"
            /* Test Depends */ "#extension GL_OES_EGL_image_external : require\n"
            "uniform samplerExternalOES _MainTex;\n"
            "varying mediump vec2 xlv_TEXCOORD0;\n"
            "void main()\n"
            "{\n"
            "gl_FragData[0] = textureExternal(_MainTex, xlv_TEXCOORD0);\n"
            "}\n"
            "#endif;\n";

        core::string resultString;
        ShaderImportErrors errors;
        size_t position = 0;
        MoveExtensionsToPosition(source, position, resultString, errors, kShaderCompPlatformOpenGLCore);
        CHECK(!errors.HasErrors());

        size_t checkPosition = 0;
        // Check if extension is in top
        checkPosition = resultString.find("#extension GL_OES_EGL_image_external : require\n", checkPosition);
        CHECK(checkPosition == 0);

        // Check if extension is removed
        checkPosition++;
        checkPosition = resultString.find("#extension GL_OES_EGL_image_external : require\n", checkPosition);
        CHECK(checkPosition == core::string::npos);
    }

    TEST_FIXTURE(CgBatchFixture, CgBatch_GLSL_Test_Function_MoveExtensionsToPosition_CommentedExtensions)
    {
        const char* sourceWithSingleLineComment =
            "#ifdef VERTEX\n"
            "attribute vec4 _glesVertex;\n"
            "attribute vec4 _glesMultiTexCoord0;\n"
            "uniform highp mat4 glstate_matrix_mvp;\n"
            "varying mediump vec2 xlv_TEXCOORD0;\n"
            "void main()\n"
            "{\n"
            "gl_Position = (glstate_matrix_mvp * _glesVertex);\n"
            "vec2 tex = _glesMultiTexCoord0.xy - 0.5;\n"
            "xlv_TEXCOORD0 = vec2(_glesMultiTexCoord0.x, _glesMultiTexCoord0.y);\n"
            "}\n"
            "#endif\n"
            "#ifdef FRAGMENT\n"
            /* Test Depends */ "//#extension GL_OES_EGL_image_external : require\n"
            "uniform samplerExternalOES _MainTex;\n"
            "varying mediump vec2 xlv_TEXCOORD0;\n"
            "void main()\n"
            "{\n"
            "gl_FragData[0] = textureExternal(_MainTex, xlv_TEXCOORD0);\n"
            "}\n"
            "#endif;\n";

        core::string resultString;
        ShaderImportErrors errors;
        size_t position = 0;

        resultString = sourceWithSingleLineComment;
        MoveExtensionsToPosition(sourceWithSingleLineComment, position, resultString, errors, kShaderCompPlatformOpenGLCore);
        CHECK(!errors.HasErrors());

        // Check that extension was not touched, because its commented
        CHECK(resultString.compare(sourceWithSingleLineComment) == 0);

        const char* sourceWithMultiLineComment =
            "#ifdef VERTEX\n"
            "attribute vec4 _glesVertex;\n"
            "attribute vec4 _glesMultiTexCoord0;\n"
            "uniform highp mat4 glstate_matrix_mvp;\n"
            "varying mediump vec2 xlv_TEXCOORD0;\n"
            "void main()\n"
            "{\n"
            "gl_Position = (glstate_matrix_mvp * _glesVertex);\n"
            "vec2 tex = _glesMultiTexCoord0.xy - 0.5;\n"
            "xlv_TEXCOORD0 = vec2(_glesMultiTexCoord0.x, _glesMultiTexCoord0.y);\n"
            "}\n"
            "#endif\n"
            "#ifdef FRAGMENT\n"
            /* Test Depends */ "/* Hello I'm just random comment"
            /* Test Depends */ "#extension GL_OES_EGL_image_external : require\n"
            /* Test Depends */ "Lets end this random comment */"
            "uniform samplerExternalOES _MainTex;\n"
            "varying mediump vec2 xlv_TEXCOORD0;\n"
            "void main()\n"
            "{\n"
            "gl_FragData[0] = textureExternal(_MainTex, xlv_TEXCOORD0);\n"
            "}\n"
            "#endif;\n";

        resultString = sourceWithMultiLineComment;
        MoveExtensionsToPosition(sourceWithMultiLineComment, position, resultString, errors, kShaderCompPlatformOpenGLCore);
        CHECK(!errors.HasErrors());

        // Check that extension was not touched, because its commented
        CHECK(resultString.compare(sourceWithMultiLineComment) == 0);
    }

    TEST_FIXTURE(CgBatchFixture, CgBatch_GLSL_TextureExternal)
    {
        const char* source =
            "#ifdef VERTEX\n"
            "attribute vec4 _glesVertex;\n"
            "attribute vec4 _glesMultiTexCoord0;\n"
            "uniform highp mat4 glstate_matrix_mvp;\n"
            "varying mediump vec2 xlv_TEXCOORD0;\n"
            "void main()\n"
            "{\n"
            "gl_Position = (glstate_matrix_mvp * _glesVertex);\n"
            "vec2 tex = _glesMultiTexCoord0.xy - 0.5;\n"
            "xlv_TEXCOORD0 = vec2(_glesMultiTexCoord0.x, _glesMultiTexCoord0.y);\n"
            "}\n"
            "#endif\n"
            "#ifdef FRAGMENT\n"
            /* Test Depends */ "#extension GL_OES_EGL_image_external : require\n"
            /* Test Depends */ "uniform samplerExternalOES _MainTex;\n"
            "varying mediump vec2 xlv_TEXCOORD0;\n"
            "void main()\n"
            "{\n"
            /* Test Depends */ "gl_FragData[0] = textureExternal(_MainTex, xlv_TEXCOORD0);\n"
            "}\n"
            "#endif;\n";

        core::string resultString = source;
        resultString = TranslateGLSLToGLUnifiedShader(resultString);
        ReplaceExternalTextureSamplerQualifiersToTexture2D(resultString);

        size_t checkPosition = 0;
        // Check if textureExternal was replaced to texture
        checkPosition = resultString.find("texture2D", checkPosition);
        CHECK(checkPosition != core::string::npos);
    }

    TEST_FIXTURE(CgBatchFixture, CgBatch_Metal_ShadowsFilteringIsHandled)
    {
        const char* source =
            "#include \"HLSLSupport.cginc\"\n"
            "#include \"UnityShaderVariables.cginc\"\n"
            "#pragma vertex vert\n"
            "#pragma fragment frag\n"
            "#include \"UnityCG.cginc\"\n"
            "float4 vert() : SV_POSITION\n"
            "{\n"
            "\treturn float4(1,1,1,1);"
            "}\n"
            "UNITY_DECLARE_SHADOWMAP(myshadow);\n"
            "half4 frag(float3 uv : TEXCOORD0) : SV_Target\n"
            "{\n"
            "\treturn UNITY_SAMPLE_SHADOW(myshadow, uv);\n"
            "}\n";

        UInt32 programMask = (1 << kProgramVertex) | (1 << kProgramFragment);

        {
            CompileSnippetOptions options = kCompileSnippetMetalUsePointFilterForShadows;
            tReturnedVector res;
            ShaderGpuProgramType gpuProgType;
            TestCompileSnippet(source, includeContext.includePaths.back(), options, res, gpuProgType, kShaderSourceLanguageCg, kProgramFragment, programMask, kShaderCompPlatformMetal, kShaderRequireShaderModel30);

            core::string resstr;
            resstr.assign((const char*)&res[0], res.size());
            CHECK(resstr.find("constexpr sampler _mtl_xl_shadow_sampler(address::clamp_to_edge, filter::nearest, compare_func::greater_equal);") != core::string::npos);
        }

        {
            CompileSnippetOptions options = kCompileSnippetNone;
            tReturnedVector res;
            ShaderGpuProgramType gpuProgType;
            TestCompileSnippet(source, includeContext.includePaths.back(), options, res, gpuProgType, kShaderSourceLanguageCg, kProgramFragment, programMask, kShaderCompPlatformMetal, kShaderRequireShaderModel30);

            core::string resstr;
            resstr.assign((const char*)&res[0], res.size());
            CHECK(resstr.find("constexpr sampler _mtl_xl_shadow_sampler(address::clamp_to_edge, filter::linear, compare_func::greater_equal);") != core::string::npos);
        }
    }

    TEST_FIXTURE(CgBatchFixture, PreprocessComputeShader_KernelParsingWorks)
    {
        core::string s = core::string(
            "#pragma kernel CSMain1\n"
            "[numthreads(1, 1, 1)]\n"
            "void CSMain(uint3 id : SV_DispatchThreadID)\n"
            "{\n"
            "}\n"
            "#pragma kernel CSMain2\n"
            "[numthreads(1, 1, 1)]\n"
            "void CSMain(uint3 id : SV_DispatchThreadID)\n"
            "{\n"
            "}\n"
            "#pragma kernel CSMain3\n"
            "[numthreads(1, 1, 1)]\n"
            "void CSMain(uint3 id : SV_DispatchThreadID)\n"
            "{\n"
            "}\n");
        dynamic_array<core::string> kernelNames;
        dynamic_array<MacroArray> macros;
        ShaderCompilationFlags compileFlags;
        UInt32 supportedAPIs = 0;
        UInt32 includeHash[4];
        std::vector<core::string> includeFiles;
        CgBatchPreprocessComputeShader(s, "", kernelNames, macros, compileFlags, supportedAPIs, includeHash, includeFiles, NULL, NULL);

        CHECK_EQUAL(3, kernelNames.size());
        CHECK_EQUAL(3, macros.size());
        CHECK_EQUAL("CSMain1", kernelNames[0]);
        CHECK_EQUAL("CSMain2", kernelNames[1]);
        CHECK_EQUAL("CSMain3", kernelNames[2]);
    }

    TEST_FIXTURE(CgBatchFixture, PreprocessComputeShader_MacroParsingWorks)
    {
        core::string s = core::string(
            "#pragma kernel CSMain FOO=1 BAR XYZ=2\n"
            "[numthreads(1, 1, 1)]\n"
            "void CSMain(uint3 id : SV_DispatchThreadID)\n"
            "{\n"
            "}\n");
        dynamic_array<core::string> kernelNames;
        dynamic_array<MacroArray> macros;
        ShaderCompilationFlags compileFlags;
        UInt32 supportedAPIs = 0;
        UInt32 includeHash[4];
        std::vector<core::string> includeFiles;
        CgBatchPreprocessComputeShader(s, "", kernelNames, macros, compileFlags, supportedAPIs, includeHash, includeFiles, NULL, NULL);

        CHECK_EQUAL(1, macros.size());
        CHECK_EQUAL(3, macros[0].size());
        CHECK_EQUAL(MacroPair("FOO", "1"), macros[0][0]);
        CHECK_EQUAL(MacroPair("BAR", ""), macros[0][1]);
        CHECK_EQUAL(MacroPair("XYZ", "2"), macros[0][2]);
    }

    TEST_FIXTURE(CgBatchFixture, PreprocessComputeShader_ExcludeRenderersWorks)
    {
        core::string s = core::string(
            "#pragma exclude_renderers gles3 glcore\n"
            "#pragma kernel CSMain\n"
            "[numthreads(1, 1, 1)]\n"
            "void CSMain(uint3 id : SV_DispatchThreadID) {}\n");
        dynamic_array<core::string> kernelNames;
        dynamic_array<MacroArray> macros;
        ShaderCompilationFlags compileFlags;
        UInt32 supportedAPIs = 0;
        UInt32 includeHash[4];
        std::vector<core::string> includeFiles;
        CgBatchPreprocessComputeShader(s, "", kernelNames, macros, compileFlags, supportedAPIs, includeHash, includeFiles, NULL, NULL);

        CHECK_EQUAL(0, (supportedAPIs & (1 << kShaderCompPlatformOpenGLCore)));
        CHECK_EQUAL(0, (supportedAPIs & (1 << kShaderCompPlatformGLES3Plus)));
        CHECK_NOT_EQUAL(0, (supportedAPIs & (1 << kShaderCompPlatformMetal)));
        CHECK_NOT_EQUAL(0, (supportedAPIs & (1 << kShaderCompPlatformD3D11)));
    }

    TEST_FIXTURE(CgBatchFixture, PreprocessComputeShader_OnlyRenderersWorks)
    {
        core::string s = core::string(
            "#pragma only_renderers d3d11\n"
            "#pragma kernel CSMain\n"
            "[numthreads(1, 1, 1)]\n"
            "void CSMain(uint3 id : SV_DispatchThreadID) {}\n");
        dynamic_array<core::string> kernelNames;
        dynamic_array<MacroArray> macros;
        ShaderCompilationFlags compileFlags;
        UInt32 supportedAPIs = 0;
        UInt32 includeHash[4];
        std::vector<core::string> includeFiles;
        CgBatchPreprocessComputeShader(s, "", kernelNames, macros, compileFlags, supportedAPIs, includeHash, includeFiles, NULL, NULL);
        CHECK_EQUAL((1 << kShaderCompPlatformD3D11), supportedAPIs);
    }

    TEST_FIXTURE(CgBatchFixture, ShaderConfigurationWorks)
    {
        core::string testEnvVal = "testvalue";
        core::string testEnvVar = "testvariable";
        core::string testConfigString = "{\"globalenvvar\":{\"testvar2\":\"error\",\"" + testEnvVar + "\":\"" + testEnvVal + "\"},\"othermap\":{\"testvariable\":\"error\",\"testvar2\":\"error\"}}";
        core::string testResetConfigString = "{\"globalenvvar\":{\"testvar2\":\"\",\"" + testEnvVar + "\":\"\"}}";
        CgBatchConfigureSystemFromJson(testConfigString);
        core::string envVarResult = "";
        bool res = GetEnvVar(testEnvVar.c_str(), envVarResult);
        CHECK(res);
        CHECK(envVarResult == testEnvVal);

        // make sure we can remove variables
        CgBatchConfigureSystemFromJson(testResetConfigString);
        res = GetEnvVar(testEnvVar.c_str(), envVarResult);
        CHECK(!res);
    }

    TEST_FIXTURE(CgBatchFixture, Shader_Includes_RelativePath_WithBackslash)
    {
        const char* fnRoot = "../common.cginc";
        CHECK(WriteTextToFile(fnRoot, "float funcRoot() { return 3; }\n"));

        const char* src =
            "Shader \"Foo\" {\n"
            "    SubShader {\n"
            "        Pass {\n"
            "            CGPROGRAM\n"
            "            #pragma vertex vert\n"
            "            #pragma fragment frag\n"
            "            #include \"..\\common.cginc\"\n"
            "            struct v2f{float4 vertex : SV_POSITION;};\n"
            "            v2f vert (){v2f o;o.vertex = float4(0,0,0,1);return o;}\n"
            "            fixed4 frag (v2f i) : SV_Target{return fixed4(0,0,0,1);}\n"
            "            ENDCG\n"
            "       }\n"
            "   }\n"
            "}\n";

        bool hadSurface;
        CgBatchPreprocessShader(src, "", res, &callbacks, false, &hadSurface);
        CHECK_EQUAL(0, callbacks.m_Errors.size());
        CHECK_EQUAL(0, callbacks.m_Warnings.size());
        CHECK_EQUAL(1, callbacks.m_Snippets.size());

        IncludeSearchContext oldContext = CgBatchGetIncludeSearchContext();
        IncludeSearchContext includeContext;
        includeContext.includePaths.push_back(".");
        includeContext.includePaths.push_back(g_IncludesPath + "/CGIncludes");
        CgBatchSetIncludeSearchContext(includeContext);

        tReturnedVector res;
        ShaderGpuProgramType gpuProgType;
        UInt32 programMask = (1 << kProgramVertex) | (1 << kProgramFragment);

        ShaderCompilerPlatform platforms[] =
        {
            kShaderCompPlatformD3D11,
            kShaderCompPlatformGLES20,
            kShaderCompPlatformGLES3Plus,
            kShaderCompPlatformOpenGLCore,
            kShaderCompPlatformMetal,
            kShaderCompPlatformVulkan,
#if ENABLE_XBOXONE_COMPILER
            kShaderCompPlatformXboxOne,
#endif
#if ENABLE_PSSL_COMPILER
            kShaderCompPlatformPS4,
#endif
#if ENABLE_SWITCH_COMPILER
            kShaderCompPlatformSwitch,
#endif
        };

        for (size_t i = 0; i < ARRAY_SIZE(platforms); ++i)
        {
            if (gPluginDispatcher->HasCompilerForAPI(platforms[i]))
                TestCompileSnippet(callbacks.m_Snippets[0], "", kCompileSnippetNone, res, gpuProgType, kShaderSourceLanguageCg, kProgramVertex, programMask, platforms[i], kShaderRequireBaseShaders);
        }

        CgBatchSetIncludeSearchContext(oldContext);

        DeleteFileAtPath(fnRoot);
    }

    TEST_FIXTURE(CgBatchFixture, Shader_Includes_MultiLevel_RelativePaths_WithBackslash)
    {
        CreateDirectoryAtPath("subdir");
        CreateDirectoryAtPath("subdir/nested");

        const char* fnFirst = "subdir/first.cginc";
        CHECK(WriteTextToFile(fnFirst, "#include \"nested\\second.cginc\"\n"));
        const char* fnSecond = "subdir/nested/second.cginc";
        CHECK(WriteTextToFile(fnSecond, "float funcRoot() { return 3; }\n"));

        const char* src =
            "Shader \"Foo\" {\n"
            "    SubShader {\n"
            "        Pass {\n"
            "            CGPROGRAM\n"
            "            #pragma vertex vert\n"
            "            #pragma fragment frag\n"
            "            #include \"subdir\\first.cginc\"\n"
            "            struct v2f{float4 vertex : SV_POSITION;};\n"
            "            v2f vert (){v2f o;o.vertex = float4(0,0,0,1);return o;}\n"
            "            fixed4 frag (v2f i) : SV_Target{return fixed4(0,0,0,1);}\n"
            "            ENDCG\n"
            "       }\n"
            "   }\n"
            "}\n";

        bool hadSurface;
        CgBatchPreprocessShader(src, "", res, &callbacks, false, &hadSurface);
        CHECK_EQUAL(0, callbacks.m_Errors.size());
        CHECK_EQUAL(0, callbacks.m_Warnings.size());
        CHECK_EQUAL(1, callbacks.m_Snippets.size());

        IncludeSearchContext oldContext = CgBatchGetIncludeSearchContext();
        IncludeSearchContext includeContext;
        includeContext.includePaths.push_back(".");
        includeContext.includePaths.push_back(g_IncludesPath + "/CGIncludes");
        CgBatchSetIncludeSearchContext(includeContext);

        tReturnedVector res;
        ShaderGpuProgramType gpuProgType;
        UInt32 programMask = (1 << kProgramVertex) | (1 << kProgramFragment);

        ShaderCompilerPlatform platforms[] =
        {
            kShaderCompPlatformD3D11,
            kShaderCompPlatformGLES20,
            kShaderCompPlatformGLES3Plus,
            kShaderCompPlatformOpenGLCore,
            kShaderCompPlatformMetal,
            kShaderCompPlatformVulkan,
#if ENABLE_XBOXONE_COMPILER
            kShaderCompPlatformXboxOne,
#endif
#if ENABLE_PSSL_COMPILER
            kShaderCompPlatformPS4,
#endif
#if ENABLE_SWITCH_COMPILER
            kShaderCompPlatformSwitch,
#endif
        };

        for (size_t i = 0; i < ARRAY_SIZE(platforms); ++i)
        {
            if (gPluginDispatcher->HasCompilerForAPI(platforms[i]))
                TestCompileSnippet(callbacks.m_Snippets[0], "", kCompileSnippetNone, res, gpuProgType, kShaderSourceLanguageCg, kProgramVertex, programMask, platforms[i], kShaderRequireBaseShaders);
        }

        CgBatchSetIncludeSearchContext(oldContext);

        DeleteFileAtPath(fnFirst);
        DeleteFileAtPath(fnSecond);
        DeleteDirectoryAtPath("subdir/nested");
        DeleteDirectoryAtPath("subdir");
    }
}


#endif // ENABLE_UNIT_TESTS
