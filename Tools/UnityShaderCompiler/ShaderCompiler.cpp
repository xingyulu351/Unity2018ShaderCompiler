#include "UnityPrefix.h"
#include "ShaderCompiler.h"
#include "APIPlugin.h"
#include "Utilities/ProcessIncludes.h"
#include "Utilities/D3DCompilerWrapper.h"
#include "Runtime/GfxDevice/GfxUtilities.h"
#include "Runtime/Utilities/Word.h"
#include "Editor/Src/Utility/TextUtilities.h"
#include "Runtime/Utilities/ArrayUtility.h"
#include "Runtime/Utilities/PathNameUtility.h"
#include "Runtime/Utilities/StaticAssert.h"
#include <string>


using core::string;

// ----------------------------------------------------------------------


const char* kProgramTypeNames[kProgramCount] =
{
    "Vertex",
    "Fragment",
    "Hull",
    "Domain",
    "Geometry",
    "Surface",
};
const char* kProgramTypeShortNames[kProgramCount] = { "vp", "fp", "hp", "dp", "gp", "sp", };


// ----------------------------------------------------------------------


core::string MakeShaderSourcePrefix(const ShaderCompileParams& params)
{
    core::string result;

    // For CGPROGRAM snippets, always include HLSLSupport include file (shader snippets can have no
    // includes at all, and at the very least they should get built-in macros/variables).
    // Do not do this for other snippet languages; they are expected to not have automagic includes.
    if (params.sourceLanguage == kShaderSourceLanguageCg)
    {
        result += "#include \"HLSLSupport.cginc\"\n";
    }

    // Add various compile macros based on snippet compilation directives.
    if (HasFlag(params.compileFlags, kShaderCompFlags_AssumeUniformScaling))
    {
        result += "#define UNITY_ASSUME_UNIFORM_SCALING\n";
    }
    if (params.maxInstanceCount != -1)
    {
        result += Format("#define UNITY_MAX_INSTANCE_COUNT %d\n", params.maxInstanceCount);
    }
    if (params.forcedMaxInstanceCount != -1)
    {
        result += Format("#define UNITY_FORCE_MAX_INSTANCE_COUNT %d\n", params.forcedMaxInstanceCount);
    }
    if (HasFlag(params.compileFlags, kShaderCompFlags_DontInstanceObjectMatrices))
    {
        result += "#define UNITY_DONT_INSTANCE_OBJECT_MATRICES\n";
    }
    if (!HasFlag(params.compileFlags, kShaderCompFlags_DontInstanceLODFade))
    {
        result += "#define UNITY_INSTANCED_LOD_FADE\n";
    }
    if (HasFlag(params.compileFlags, kShaderCompFlags_InstanceRenderingLayer))
    {
        result += "#define UNITY_INSTANCED_RENDERING_LAYER\n";
    }
    if (!HasFlag(params.compileFlags, kShaderCompFlags_DontInstanceSH))
    {
        result += "#define UNITY_INSTANCED_SH\n";
    }
    if (!HasFlag(params.compileFlags, kShaderCompFlags_DontInstanceLightmapST))
    {
        result += "#define UNITY_INSTANCED_LIGHTMAPSTS\n";
    }
    if (!params.proceduralFuncName.empty())
    {
        result += Format("#define UNITY_INSTANCING_PROCEDURAL_FUNC %s\n", params.proceduralFuncName.c_str());
    }

    // Similar to HLSLSuport above, include built-in variables (need to do this after defines above,
    // since those defines can affect how variables are declared).
    if (params.sourceLanguage == kShaderSourceLanguageCg)
    {
        result += "#include \"UnityShaderVariables.cginc\"\n";
        // This file defines UnityObjectToClipPos(), which we decide to make it always included to make sure shader auto-upgrader works.
        result += "#include \"UnityShaderUtilities.cginc\"\n";
    }
    return result;
}

// ----------------------------------------------------------------------

static PlatformPluginInterface *kShaderCompPlatformInterfaces[] =
{
    NULL, // OpenGL (removed)
    NULL, // D3D9 (removed)
    NULL, // Xbox360 (removed)
    NULL, // PS3 (removed)
    NULL, // D3D11
    NULL, // GLES via HLSL2GLSL
    NULL, // GLESDesktop/NaCl (removed)
    NULL, // Flash (removed)
    NULL, // D3D11 9.x
    NULL, // GLES3.0 and later, via HLSLcc
    NULL, // PSP2 (removed)
    NULL, // PS4 - requires a compiler plugin
    NULL, // XB1 - requires a compiler plugin
    NULL, // PSM (removed)
    NULL, // Metal
    NULL, // OpenGL 4.3 (really 420+ARB_compute)
    NULL, // N3DS (removed)
    NULL, // Wii U (removed)
    NULL, // Vulkan
    NULL, // Switch - requires a compiler plugin
    NULL, // Xbox One D3D12 - requires a compiler plugin
};
CompileTimeAssertArraySize(kShaderCompPlatformInterfaces, kShaderCompPlatformCount);

D3DCompilerWrapper g_D3DCompiler;
static PlatformPluginInterface* g_HLSL2GLSLPlugin = 0;

PlatformPluginDispatcher* gPluginDispatcher = NULL;


struct Dispatcher : public PlatformPluginDispatcher
{
    D3DCompiler *GetD3DCompiler()
    {
        return &g_D3DCompiler;
    }

    virtual PlatformPluginInterface* GetHLSL2GLSLPlugin()
    {
        return g_HLSL2GLSLPlugin;
    }

    bool HasCompilerForAPI(ShaderCompilerPlatform api)
    {
        return api > kShaderCompPlatformNone && api < kShaderCompPlatformCount && kShaderCompPlatformInterfaces[api] != NULL;
    }

    PlatformPluginInterface* Get(ShaderCompilerPlatform api)
    {
        if (HasCompilerForAPI(api))
            return kShaderCompPlatformInterfaces[api];

        return NULL;
    }

    const ShaderCompileOutputInterface* CompileShader(const ShaderCompileInputInterface *input)
    {
        ShaderCompilerPlatform api = input->api;

        if (HasCompilerForAPI(api))
            return kShaderCompPlatformInterfaces[api]->CompileShader(input);

        return NULL;
    }

    const ShaderCompileOutputInterface* DisassembleShader(const ShaderCompileInputInterface *input)
    {
        ShaderCompilerPlatform api = input->api;

        if (HasCompilerForAPI(api))
            return kShaderCompPlatformInterfaces[api]->DisassembleShader(input);

        return NULL;
    }

    const ShaderCompileOutputInterface* CompileComputeShader(const ShaderCompileInputInterface *input)
    {
        ShaderCompilerPlatform api = input->api;

        if (HasCompilerForAPI(api))
            return kShaderCompPlatformInterfaces[api]->CompileComputeShader(input);

        return NULL;
    }

    const ShaderCompileOutputInterface* TranslateComputeKernel(const ShaderCompileInputInterface *input)
    {
        ShaderCompilerPlatform api = input->api;

        if (HasCompilerForAPI(api))
            return kShaderCompPlatformInterfaces[api]->TranslateComputeKernel(input);

        return NULL;
    }

    void ReleaseShader(const ShaderCompileOutputInterface *output)
    {
        ShaderCompilerPlatform api = output->api;

        if (HasCompilerForAPI(api))
            kShaderCompPlatformInterfaces[api]->ReleaseShader(output);
    }

    void ClearIncludesCache()
    {
        for (int api = kShaderCompPlatformNone; api < kShaderCompPlatformCount; ++api)
        {
            if (HasCompilerForAPI((ShaderCompilerPlatform)api))
                kShaderCompPlatformInterfaces[api]->ClearIncludesCache();
        }
    }

    int GetShaderCompilerVersion(ShaderCompilerPlatform api)
    {
        if (HasCompilerForAPI(api))
            return kShaderCompPlatformInterfaces[api]->GetShaderCompilerVersion();

        return 0;
    }

    void InitializeCompiler(ShaderCompilerPlatform api)
    {
        if (HasCompilerForAPI(api))
            kShaderCompPlatformInterfaces[api]->InitializeCompiler();
    }
} sDispatcher;

static ShaderSourceLanguage kShaderCompPlatformComputeShaderLanguages[] =
{
    kShaderSourceLanguageNone, // OpenGL2 (removed)
    kShaderSourceLanguageNone, // D3D9 (removed)
    kShaderSourceLanguageNone, // Xbox360 (removed)
    kShaderSourceLanguageNone, // PS3 (removed)
    kShaderSourceLanguageCg,   // D3D11
    kShaderSourceLanguageNone, // GLES 2.0
    kShaderSourceLanguageNone, // GLES/NaCl (removed)
    kShaderSourceLanguageNone, // Flash (removed)
    kShaderSourceLanguageNone, // D3D11 9.x
    kShaderSourceLanguageGLSL, // GLES3.0 and later
    kShaderSourceLanguageNone, // PSP2 (removed)
    kShaderSourceLanguageCg, // PS4 TODO: check if PSSL needs own source variant
    kShaderSourceLanguageCg, // XB1
    kShaderSourceLanguageNone, // PSM (removed)
    kShaderSourceLanguageNone, // Metal
    kShaderSourceLanguageGLSL, // OGL 4.3
    kShaderSourceLanguageNone, // N3DS (removed)
    kShaderSourceLanguageNone, // Wii U (removed)
    kShaderSourceLanguageCg,   // Vulkan
    kShaderSourceLanguageGLSL, // Switch
    kShaderSourceLanguageCg, // XB1 D3D12
};
CompileTimeAssertArraySize(kShaderCompPlatformComputeShaderLanguages, kShaderCompPlatformCount);

// Strip all snippet language tags and any non-target snippets.
// Any code outside snippet tags are left untouched.
//TODO: leave CG code if native tags are not found
ShaderSourceLanguage StripComputeShaderSourceVariants(core::string& src, ShaderCompilerPlatform platform)
{
    ShaderSourceLanguage retval = kShaderSourceLanguageCg;
    ShaderSourceLanguage nativeLanguage = kShaderCompPlatformComputeShaderLanguages[platform];

    for (unsigned i = 0; i < kShaderSourceLanguageCount; ++i)
    {
        const char* beginTag = kShaderLangBeginTag[i];
        const char* endTag = kShaderLangEndTag[i];

        size_t pos = src.find(beginTag);
        while (pos != core::string::npos)
        {
            if (i == nativeLanguage)
                src.erase(pos, kShaderLangBeginTagLength[i]);

            size_t endPos = src.find(endTag, pos);
            if (endPos != core::string::npos)
            {
                if (i == nativeLanguage)
                {
                    // For target language only the tags are removed. Search continues from the end tag position.
                    src.erase(endPos, kShaderLangEndTagLength[i]);
                    pos = endPos;
                    retval = nativeLanguage;
                }
                else
                {
                    // For non-target languages the whole snippet is removed. Search continues from where the snippet begun.
                    src.erase(pos, endPos - pos + kShaderLangEndTagLength[i]);
                }

                pos = src.find(beginTag, pos);
            }
            else
            {
                pos = core::string::npos; // no end tag found -> broken syntax
            }
        }
    }
    return retval;
}

// these may be moved to separate dlls in future
PlatformPluginInterface* GetHLSL2GLSLPluginInterface(int cgBatchVersion, PlatformPluginDispatcher* dispatcher);
PlatformPluginInterface* GetHLSLPluginInterface(int cgBatchVersion, PlatformPluginDispatcher* dispatcher);
PlatformPluginInterface* GetHLSLccPluginInterface(int cgBatchVersion, PlatformPluginDispatcher* dispatcher);
PlatformPluginInterface* GetGlslangPluginInterface(int cgBatchVersion, PlatformPluginDispatcher* dispatcher);

void InitializeDispatcher()
{
    gPluginDispatcher = &sDispatcher;
}

core::string InitializeCompiler(const core::string& appPath)
{
    core::string compilerPath = AppendPathName(appPath, "Tools/" kD3DCompilerDLL);
    const char* d3dError = g_D3DCompiler.Initialize(compilerPath.c_str());
    if (d3dError != NULL)
        return d3dError;

    const int cgBatchVersion = 0x0002; // current version of CgBatch ... can be incremented to stop old plugins working

    sPlatformPlugins[sPlatformPluginCount++] = g_HLSL2GLSLPlugin = GetHLSL2GLSLPluginInterface(cgBatchVersion, gPluginDispatcher);
    sPlatformPlugins[sPlatformPluginCount++] = GetHLSLPluginInterface(cgBatchVersion, gPluginDispatcher);
    sPlatformPlugins[sPlatformPluginCount++] = GetHLSLccPluginInterface(cgBatchVersion, gPluginDispatcher);
    sPlatformPlugins[sPlatformPluginCount++] = GetGlslangPluginInterface(cgBatchVersion, gPluginDispatcher);

    // Assign compiler invoke functions from our loaded plugins
    for (int i = 0; i < sPlatformPluginCount; ++i)
    {
        if (sPlatformPlugins[i] == NULL)
            continue;

        const unsigned int platforms = sPlatformPlugins[i]->GetShaderPlatforms();
        for (int p = 0; p < kShaderCompPlatformCount; ++p)
        {
            if (platforms & (1 << p))
            {
                // allow overwriting plugin platforms since plugin might be found in multiple locations during development,
                // but have a debug assert fire to alert the developer to the problem
                DebugAssertFormatMsg(kShaderCompPlatformInterfaces[p] == NULL, "Found two CgBatch plugins providing the same platform '%d'", p);
                kShaderCompPlatformInterfaces[p] = sPlatformPlugins[i];
            }
        }
    }

    return "";
}

void ShutdownCompiler()
{
    g_D3DCompiler.Shutdown();

    // shutdown plugin compilers
    for (int j = 0; j < sPlatformPluginCount; j++)
    {
        if (sPlatformPlugins[j])
        {
            sPlatformPlugins[j]->Shutdown();
            sPlatformPlugins[j] = NULL;
        }
    }
}
