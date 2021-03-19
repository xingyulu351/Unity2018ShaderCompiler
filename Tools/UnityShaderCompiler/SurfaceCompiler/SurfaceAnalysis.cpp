#include "UnityPrefix.h"
#include "SurfaceAnalysis.h"
#include "MojoshaderUtils.h"
#include "../PlatformPlugin.h"
#include <string>
#include <set>
#include <cstring>
#include "Runtime/Core/Containers/hash_map.h"
#include "Runtime/Utilities/Word.h"
#include "../Utilities/ProcessIncludes.h"
#include "../Utilities/ShaderImportUtils.h"
#include "../Utilities/ShaderImportErrors.h"
#include "External/ShaderCompilers/mojoshader/mojoshader.h"
#include "artifacts/generated/Configuration/UnityConfigureVersion.gen.h"

#include "Editor/Src/Utility/d3d11/D3D11ReflectionAPI.h"
#include "Tools/UnityShaderCompiler/Utilities/D3DCompiler.h"
#include "Runtime/Threads/Mutex.h"


using core::string;

// We use a cache for saving work on doing redundant analysis of very similar shader variants.
// Right now this works by fully preprocessing the passed source (taking into account all macros passed etc.),
// and using that as a key to "what the results are for this exact input".
//
// Currently this typically saves about 2x of work, mostly because instancing off/on for surface shader analysis
// ends up not influencing the result at all. Shader code could manually do special/simpler work if it knows that
// does not affect inputs/outputs (i.e. final code generation), by checking for SHADER_TARGET_SURFACE_ANALYSIS
// being defined.
struct SurfaceAnalysisCache
{
    SurfaceAnalysisCache(const IncludeSearchContext& includeContext)
        : compileCacheRequests(0)
        , compileCacheHits(0)
        , astCacheRequests(0)
        , astCacheHits(0)
        , handler(includeContext)
    {}

    struct CacheKey
    {
        core::string source;
        core::string entryPoint;
        bool operator==(const CacheKey& o) const
        {
            return entryPoint == o.entryPoint && source == o.source;
        }

        bool operator<(const CacheKey& o) const
        {
            if (entryPoint != o.entryPoint)
                return entryPoint < o.entryPoint;
            return source < o.source;
        }
    };

    struct CacheKeyHash
    {
        UInt32 operator()(const CacheKey& s) const
        {
            core::hash<core::string> sh;
            return sh(s.source) ^ sh(s.entryPoint);
        }
    };

    core::string PreprocessSource(const core::string& source, const ShaderArgs& args, const IncludePaths& includePaths)
    {
        size_t numArgs = args.size();
        D3D10_SHADER_MACRO* macros = new D3D10_SHADER_MACRO[numArgs + 1];
        size_t argIdx = 0;
        for (size_t i = 0; i < numArgs; ++i, ++argIdx)
        {
            macros[argIdx].Name = args[i].name.c_str();
            macros[argIdx].Definition = args[i].value.c_str();
        }
        macros[argIdx].Name = NULL;
        macros[argIdx].Definition = NULL;
        ++argIdx;

        D3D10Blob* text = NULL;
        D3D10Blob* errors = NULL;
        handler.SetIncludePaths(includePaths);
        HRESULT hr = gPluginDispatcher->GetD3DCompiler()->D3DPreprocess(source.c_str(), source.size(), "", macros, &handler, &text, &errors);
        delete[] macros;

        core::string res;
        if (SUCCEEDED_IMPL(hr) && text)
        {
            const char* buffer = (const char*)text->GetBufferPointer();
            size_t bufferSize = text->GetBufferSize();
            res.assign(buffer, bufferSize);
        }
        else
        {
            res = source;
        }
        if (text)
            text->Release();
        if (errors)
            errors->Release();
        return res;
    }

    struct ASTResult
    {
        SurfaceFunctionDesc desc;
        LightingModels lightModels;
        ShaderImportErrors errors;
    };

    typedef core::hash_map<CacheKey, ShaderDesc, CacheKeyHash> CompileCacheMap;
    typedef std::map<CacheKey, ASTResult> ASTCacheMap; // can't use our hash_map since ASTResult has std::set which has internal pointers in some impls
    CompileCacheMap compileCache;
    ASTCacheMap astCache;
    HLSLIncludeHandler handler;
    size_t compileCacheRequests, compileCacheHits;
    size_t astCacheRequests, astCacheHits;
    Mutex compileMutex, astMutex;
};


static core::string GetMojoShaderTypeName(const MOJOSHADER_astDataType* dt)
{
    if (dt == NULL)
        return "";

    switch (dt->type)
    {
        case MOJOSHADER_AST_DATATYPE_BOOL:      return "bool";
        case MOJOSHADER_AST_DATATYPE_INT:       return "int";
        case MOJOSHADER_AST_DATATYPE_UINT:      return "uint";
        case MOJOSHADER_AST_DATATYPE_FLOAT:     return "float";
        case MOJOSHADER_AST_DATATYPE_FLOAT_SNORM: return "snorm float";
        case MOJOSHADER_AST_DATATYPE_FLOAT_UNORM: return "unorm float";
        case MOJOSHADER_AST_DATATYPE_HALF:      return "half";
        case MOJOSHADER_AST_DATATYPE_DOUBLE:    return "double";
        case MOJOSHADER_AST_DATATYPE_STRING:    return "string";
        case MOJOSHADER_AST_DATATYPE_SAMPLER_1D: return "sampler1D";
        case MOJOSHADER_AST_DATATYPE_SAMPLER_2D: return "sampler2D";
        case MOJOSHADER_AST_DATATYPE_SAMPLER_3D: return "sampler3D";
        case MOJOSHADER_AST_DATATYPE_SAMPLER_CUBE: return "samplerCUBE";
        case MOJOSHADER_AST_DATATYPE_SAMPLER_STATE: return "sampler_state";
        case MOJOSHADER_AST_DATATYPE_SAMPLER_COMPARISON_STATE: return "SamplerComparisonState";
        case MOJOSHADER_AST_DATATYPE_STRUCT: return "structType";
        case MOJOSHADER_AST_DATATYPE_ARRAY: return "arrayType";
        case MOJOSHADER_AST_DATATYPE_VECTOR:
            return GetMojoShaderTypeName(dt->vector.base) + IntToString(dt->vector.elements);
        case MOJOSHADER_AST_DATATYPE_MATRIX:
            return GetMojoShaderTypeName(dt->matrix.base) + IntToString(dt->matrix.rows) + "x" + IntToString(dt->matrix.columns);
        case MOJOSHADER_AST_DATATYPE_BUFFER:    return "bufferType";
        case MOJOSHADER_AST_DATATYPE_USER:      return dt->user.name;

        default:
            assert(0 && "Unexpected datatype.");
            return "";
    }
}

static MemberType GetTypeFromMojoShader(const core::string& n)
{
    if (n == "float" || n == "float1")
        return kTypeFloat;
    if (n == "float2")
        return kTypeFloat2;
    if (n == "float3")
        return kTypeFloat3;
    if (n == "float4")
        return kTypeFloat4;
    if (n == "half" || n == "half1")
        return kTypeHalf;
    if (n == "half2")
        return kTypeHalf2;
    if (n == "half3")
        return kTypeHalf3;
    if (n == "half4")
        return kTypeHalf4;
    if (n == "")
        return kTypeVoid;
    return kTypeOther;
}

static bool FindStruct(const MOJOSHADER_astCompilationUnit* ast, const core::string& name, MemberDeclarations& outMembers)
{
    outMembers.clear();

    while (ast)
    {
        if (ast->ast.type == MOJOSHADER_AST_COMPUNIT_STRUCT)
        {
            const MOJOSHADER_astCompilationUnitStruct* p = (const MOJOSHADER_astCompilationUnitStruct*)ast;
            if (p->struct_info->name == name)
            {
                const MOJOSHADER_astStructMembers* member = p->struct_info->members;
                while (member)
                {
                    core::string typeName = GetMojoShaderTypeName(member->datatype);
                    MemberDecl m = MemberDecl(typeName, member->details->identifier ? member->details->identifier : "", member->semantic ? member->semantic : "", GetTypeFromMojoShader(typeName));
                    if (member->details->isarray)
                    {
                        EvalASTData constData;
                        if (EvalASTExpression((const MOJOSHADER_astNode*)member->details->dimension, constData))
                            m.arraySize = constData.isFloat ? (int)constData.value.f : constData.value.i;
                    }
                    outMembers.push_back(m);
                    member = member->next;
                }
                return true;
            }
        }
        ast = ast->next;
    }

    return false;
}

static void GetFunctionArgs(const MOJOSHADER_astFunctionSignature* decl, MemberDeclarations& outArgs)
{
    const MOJOSHADER_astFunctionParameters* arg = decl->params;
    while (arg)
    {
        core::string typeName = GetMojoShaderTypeName(arg->datatype);
        MemberDecl m(typeName, arg->identifier ? arg->identifier : "", arg->semantic ? arg->semantic : "", GetTypeFromMojoShader(typeName));
        static MemberModifier kModTable[] = { kInputModNone, kInputModIn, kInputModOut, kInputModInOut, kInputModUniform };
        m.mod = kModTable[arg->input_modifier];
        outArgs.push_back(m);
        arg = arg->next;
    }
}

static const bool FindFunction(const MOJOSHADER_astCompilationUnit* ast, const core::string& name, MemberDeclarations& outArgs)
{
    outArgs.clear();
    while (ast)
    {
        if (ast->ast.type == MOJOSHADER_AST_COMPUNIT_FUNCTION)
        {
            const MOJOSHADER_astCompilationUnitFunction* p = (const MOJOSHADER_astCompilationUnitFunction*)ast;
            if (p->declaration->identifier == name)
            {
                GetFunctionArgs(p->declaration, outArgs);
                return true;
            }
        }
        ast = ast->next;
    }
    return false;
}

static LightingModel* GetLightingModel(
    const MOJOSHADER_astCompilationUnitFunction* func,
    LightingModels& outLightModels,
    const char* LightingTypeName)
{
    core::string name = func->declaration->identifier;
    const core::string lightingFunctionPrefix = "Lighting";
    const size_t lightingFunctionPrefixLength = lightingFunctionPrefix.size();
    name = name.substr(lightingFunctionPrefixLength, name.size() - lightingFunctionPrefixLength - strlen(LightingTypeName));

    LightingModel& lm = outLightModels[name];
    lm.name = name;
    return &lm;
}

static void ParseLightingFunction(const MemberDeclarations& args, LightingModel& outLightModel, const LightingFunctionPostfix postfix)
{
    switch (postfix)
    {
        case kPrePass:
            // prepass lighting function
            // args should be:
            // (SurfaceOutput s, float4/half4 light)
            if (args.size() != 2)
                return;

            outLightModel.renderPaths |= (1 << kPathPrePass);
            break;
        case kDeferred:
            // deferred shading function
            // args should be:
            // (SurfaceOutput s, out half4 outDiffuseOcclusion, out half4 outSpecSmoothness, out half4 outNormal)
            // (SurfaceOutput s, UnityGI gi, out half4 outDiffuseOcclusion, out half4 outSpecSmoothness, out half4 outNormal)
            // (SurfaceOutput s, half3 viewDir, UnityGI gi, out half4 outDiffuseOcclusion, out half4 outSpecSmoothness, out half4 outNormal)
            if (args.size() < 4)
                return;

            if (args[2].typeName == "UnityGI")
            {
                outLightModel.useGIDeferred = true;
                outLightModel.useViewDir = true;
            }
            else if (args[1].typeName == "UnityGI")
            {
                outLightModel.useGIDeferred = true;
            }
            outLightModel.renderPaths |= (1 << kPathDeferred);
            break;
        case kSingleLightmap:
            // single lightmap function
            // args should be:
            // (SurfaceOutput s, fixed4 color)
            // (SurfaceOutput s, fixed4 color, half3 viewDir)
            if (args.size() != 2 && args.size() != 3)
                return;

            outLightModel.hasCustomSingleLightmap = true;
            if (args.size() == 3)
                outLightModel.useViewDirForSingleLightmap = true;
            break;
        case  kDualLightmap:
            // single lightmap function
            // args should be:
            // (SurfaceOutput s, fixed4 indirect, fixed4 total, half blendFactor)
            // (SurfaceOutput s, fixed4 indirect, fixed4 total, half blendFactor, half3 viewDir)
            if (args.size() != 4 && args.size() != 5)
                return;

            outLightModel.hasCustomDualLightmaps = true;
            if (args.size() == 5)
                outLightModel.useViewDirForDualLightmap = true;
            break;
        case kDirLightmap:
            // directional lightmaps lighting function
            // args should be:
            // (SurfaceOutput s, fixed4 color, fixed4 scale, bool surfFuncWritesNormal) or
            // (SurfaceOutput s, fixed4 color, fixed4 scale, half3 viewDir, bool surfFuncWritesNormal, out half3 specColor)
            if (args.size() != 4 && args.size() != 6)
                return;

            outLightModel.renderPaths |= (1 << kPathDirLightmap);
            outLightModel.hasCustomDirLightmaps = true;
            if (args.size() == 6)
                outLightModel.useViewDirForDirLightmap = true;
            break;
        case kGI:
            // GI function
            // args should be:
            // (SurfaceOutput s, UnityGIInput data, inout UnityGI gi)
            if (args.size() != 3)
                return;

            outLightModel.hasGI = true;
            break;
        default:
            // regular lighting function
            // args should be:
            // (SurfaceOutput s, UnityGI gi)
            // (SurfaceOutput s, half3 viewDir, UnityGI gi)
            // (SurfaceOutput s, half3 lightDir, half atten) or
            // (SurfaceOutput s, half3 lightDir, half3 viewDir, half atten) or
            if (args.size() != 2 && args.size() != 3 && args.size() != 4)
                return;

            outLightModel.renderPaths |= (1 << kPathForward);
            if (args.size() == 3 && args[2].typeName == "UnityGI")
            {
                outLightModel.useGIForward = true;
                outLightModel.useViewDir = true;
            }
            else if (args.size() == 2 && args[1].typeName == "UnityGI")
            {
                outLightModel.useGIForward = true;
            }

            if (args.size() == 4)
                outLightModel.useViewDir = true;
            break;
    }

    if (!args.empty() && (args[0].typeName == "SurfaceOutputStandard" || args[0].typeName == "SurfaceOutputStandardSpecular"))
        outLightModel.isPBS = true;
}

static LightingFunctionPostfix GetLightingFunctionPostfix(const MOJOSHADER_astCompilationUnitFunction* func)
{
    for (int i = ARRAY_SIZE(LightingFunctionPostfixNames) - 1; i > 0; --i)
    {
        if (EndsWith(func->declaration->identifier, LightingFunctionPostfixNames[i]))
            return (LightingFunctionPostfix)i;
    }
    return kNone;
}

static bool HasValidReturnType(const MOJOSHADER_astCompilationUnitFunction* func,  const LightingFunctionPostfix postfix)
{
    // return type should be float4/half4, or void for GI
    MemberType retType = GetTypeFromMojoShader(GetMojoShaderTypeName(func->declaration->datatype));
    if (postfix == kGI)
    {
        if (retType != kTypeVoid)
            return false;
    }
    else
    {
        if (retType != kTypeFloat4 && retType != kTypeHalf4)
            return false;
    }
    return true;
}

static bool HasConsistentOutputStruct(const MemberDeclarations& args, LightingModel& outLightModel, ShaderImportErrors& outErrors)
{
    const core::string& outputStructName = args[0].typeName;
    if (!outLightModel.outputStructName.empty() && outLightModel.outputStructName != outputStructName)
    {
        outErrors.AddImportError(Format("Surface shader lighting model '%s' has inconsistent output struct types ('%s' vs '%s')", outLightModel.name.c_str(), outputStructName.c_str(), outLightModel.outputStructName.c_str()), 0, false);
        return false;
    }

    outLightModel.outputStructName = outputStructName;
    return true;
}

static void FindLightingModels(const MOJOSHADER_astCompilationUnit* ast, LightingModels& outLightModels, ShaderImportErrors& outErrors)
{
    // go over all functions that start with "Lighting"
    outLightModels.clear();
    while (ast)
    {
        if (ast->ast.type == MOJOSHADER_AST_COMPUNIT_FUNCTION)
        {
            const MOJOSHADER_astCompilationUnitFunction* p = (const MOJOSHADER_astCompilationUnitFunction*)ast;

            if (BeginsWith(p->declaration->identifier, "Lighting"))
            {
                MemberDeclarations args;
                GetFunctionArgs(p->declaration, args);
                LightingFunctionPostfix postfix = GetLightingFunctionPostfix(p);

                if (HasValidReturnType(p, postfix) && !args.empty())
                {
                    LightingModel& lightingModel = *GetLightingModel(p, outLightModels, LightingFunctionPostfixNames[postfix]);
                    if (HasConsistentOutputStruct(args, lightingModel, outErrors))
                    {
                        ParseLightingFunction(args, lightingModel, postfix);
                    }
                }
            }
        }
        ast = ast->next;
    }

    // check consistency of the lighting models
    for (LightingModels::const_iterator it = outLightModels.begin(), itEnd = outLightModels.end(); it != itEnd; ++it)
    {
        const LightingModel& lm = it->second;
        // PBS light models need to have both a GI & forward functions
        if (lm.hasGI && !(lm.useGIForward || lm.useGIDeferred))
        {
            outErrors.AddImportError("Surface shader lighting model '" + lm.name + "' is missing a forward or deferred function (" + lm.name + ")", 0, false);
        }
        if ((lm.useGIForward || lm.useGIDeferred) && !lm.hasGI)
        {
            outErrors.AddImportError("Surface shader lighting model '" + lm.name + "' is missing a GI function (" + lm.name + "_GI)", 0, false);
        }
    }
}

struct MojoShaderCompiler
{
    MojoShaderCompiler() : data(NULL) {}
    ~MojoShaderCompiler()
    {
        if (data)
            MOJOSHADER_freeAstData(data);
    }

    const MOJOSHADER_astData* data;
    MojoShaderAllocator alloc;
};


static bool IsBuiltinInputMember(const core::string& n, const core::string& semantic)
{
    if (BeginsWith(semantic, "COLOR"))
        return true;
    if (semantic == "VFACE" || semantic == "FACE")
        return true;
    if (BeginsWith(n, "uv"))
        return true;
    if (n == "worldRefl" ||
        n == "worldNormal" ||
        n == "screenPos" ||
        n == "viewDir" ||
        n == "worldViewDir" ||
        n == "worldPos")
        return true;
    return false;
}

static bool CheckArgCount(const char* desc, const core::string& name, const MemberDeclarations& args, size_t minArgs, size_t maxArgs, ShaderImportErrors& outErrors)
{
    size_t count = args.size();
    if (count < minArgs || count > maxArgs)
    {
        core::string msg;
        if (minArgs == maxArgs)
            msg = Format("Surface shader %sfunction '%s' has %i parameters; needs to have %i", desc, name.c_str(), count, minArgs);
        else
            msg = Format("Surface shader %sfunction '%s' has %i parameters; needs to have %i to %i", desc, name.c_str(), count, minArgs, maxArgs);
        outErrors.AddImportError(msg, 0, false);
        return false;
    }
    return true;
}

static const char* kModNames[] = {"no modifier", "'in'", "'out'", "'inout'", "'uniform'"};


static bool CheckArgModifier(const char* desc, const core::string& name, const MemberDeclarations& args, int index, MemberModifier mod, ShaderImportErrors& outErrors)
{
    MemberModifier membermod = args[index].mod;
    if (mod == kInputModIn && membermod == kInputModNone)
        mod = kInputModNone;
    if (membermod != mod)
    {
        core::string msg = Format("Surface shader %sfunction '%s' #%i parameter has wrong modifier; has %s but needs %s at '%s %s'", desc, name.c_str(), index, kModNames[args[index].mod], kModNames[mod], args[index].typeName.c_str(), args[index].name.c_str());
        outErrors.AddImportError(msg, 0, false);
        return false;
    }
    return true;
}

static bool CheckArgType(const char* desc, const core::string& name, const MemberDeclarations& args, int index, const core::string& typeName, MemberModifier mod, ShaderImportErrors& outErrors)
{
    if (args[index].typeName != typeName)
    {
        core::string msg = Format("Surface shader %sfunction '%s' #%i parameter of wrong type; has '%s' but needs '%s'", desc, name.c_str(), index, args[index].typeName.c_str(), typeName.c_str());
        outErrors.AddImportError(msg, 0, false);
        return false;
    }
    return CheckArgModifier(desc, name, args, index, mod, outErrors);
}

static bool CheckArgVec4Type(const char* desc, const core::string& name, const MemberDeclarations& args, int index, MemberModifier mod, ShaderImportErrors& outErrors)
{
    MemberType t = GetTypeFromMojoShader(args[index].typeName);
    if (t != kTypeFloat4 && t != kTypeHalf4)
    {
        core::string msg = Format("Surface shader %sfunction '%s' #%i parameter of wrong type; has '%s' but needs a 4-vector", desc, name.c_str(), index, args[index].typeName.c_str());
        outErrors.AddImportError(msg, 0, false);
        return false;
    }
    MemberModifier membermod = args[index].mod;
    if (mod == kInputModIn && membermod == kInputModNone)
        mod = kInputModNone;
    if (membermod != mod)
    {
        core::string msg = Format("Surface shader %sfunction '%s' #%i parameter has wrong modifier; has %s but needs %s at '%s %s'", desc, name.c_str(), index, kModNames[args[index].mod], kModNames[mod], args[index].typeName.c_str(), args[index].name.c_str());
        outErrors.AddImportError(msg, 0, false);
        return false;
    }
    return true;
}

static bool FindTessellationModel(const MOJOSHADER_astCompilationUnit* ast, const core::string& tessModifier, SurfaceFunctionDesc& outDesc, ShaderImportErrors& outErrors)
{
    if (tessModifier.empty())
        return true;

    MemberDeclarations args;
    bool func = FindFunction(ast, tessModifier, args);
    if (!func)
    {
        outErrors.AddImportError("Surface shader tessellation function '" + tessModifier + "' not found", 0, false);
        return false;
    }
    const size_t argCount = args.size();

    if (argCount == 0)
    {
        // simple tessellation, no inputs at all
        outDesc.tessModel.useAppdataInput = false;
    }
    else if (argCount == 3)
    {
        outDesc.tessModel.useAppdataInput = true;
        for (int i = 0; i < 3; ++i)
        {
            if (!CheckArgType("tessellation ", tessModifier, args, i, outDesc.appdataType, kInputModIn, outErrors))
                return false;
        }
    }
    else
    {
        core::string msg = Format("Surface shader tessellation function '%s' has %i parameters; needs to have 0 or 3", tessModifier.c_str(), (int)argCount);
        outErrors.AddImportError(msg, 0, false);
        return false;
    }

    return true;
}

void AnalyseSurfaceShaderAST(
    const core::string& gen, const ShaderArgs& macroArgs,
    const core::string& entryPoint, const core::string& vertexEntryPoint,
    const core::string& finalColorEntryPoint, const core::string& finalPrepassEntryPoint, const core::string& finalGBufferEntryPoint,
    const core::string& tessModifier,
    const IncludePaths& includePaths,
    SurfaceFunctionDesc& outDesc, LightingModels& outLightModels, ShaderImportErrors& outErrors,
    SurfaceAnalysisCache& analysisCache)
{
    // have we analyzed an identical (after preprocessing) surface function already?
    SurfaceAnalysisCache::CacheKey key;
    {
        key.source = analysisCache.PreprocessSource(gen, macroArgs, includePaths);
        key.entryPoint = entryPoint;
        Mutex::AutoLock lock(analysisCache.astMutex);
        SurfaceAnalysisCache::ASTCacheMap::const_iterator it = analysisCache.astCache.find(key);
        ++analysisCache.astCacheRequests;
        if (it != analysisCache.astCache.end())
        {
            ++analysisCache.astCacheHits;
            outDesc = it->second.desc;
            outLightModels = it->second.lightModels;
            outErrors.AddErrorsFrom(it->second.errors);
            return;
        }
    }

    MojoShaderCompiler compiler;
    compiler.data = MOJOSHADER_parseAst(
        "",
        key.source.c_str(),
        (unsigned)strlen(key.source.c_str()),     // use strlen, since HLSL preprocessor sometimes puts \0 character at end of string itself
        NULL, 0,
        NULL, NULL, NULL,
        MojoShaderAllocator::Alloc, MojoShaderAllocator::Free, &compiler.alloc);

    if (compiler.data->error_count > 0)
    {
        for (int i = 0; i < compiler.data->error_count; ++i)
        {
            outErrors.AddImportError(compiler.data->errors[i].error, compiler.data->errors[i].filename ? compiler.data->errors[i].filename : "", compiler.data->errors[i].error_position, false);
        }
        return;
    }

    // Find surface function
    MemberDeclarations args;
    bool func = FindFunction(&compiler.data->ast->compunit, entryPoint, args);
    if (!func)
    {
        outErrors.AddImportError("Surface shader function '" + entryPoint + "' not found", 0, false);
        return;
    }
    if (!CheckArgCount("", entryPoint, args, 2, 2, outErrors))
        return;

    // Input struct
    if (args[0].mod != kInputModNone && args[0].mod != kInputModIn)
    {
        outErrors.AddImportError(Format("Surface shader's input parameter needs to have no or 'in' modifier; found %s at '%s %s'",
            kModNames[args[0].mod], args[0].typeName.c_str(), args[0].name.c_str()), 0, false);
        return;
    }
    outDesc.inputType = args[0].typeName;
    outDesc.inputName = args[0].name;
    if (!FindStruct(&compiler.data->ast->compunit, args[0].typeName, outDesc.inputMembers))
    {
        outErrors.AddImportError("Can't find surface shader input struct '" + args[0].typeName + "'", 0, false);
        return;
    }
    // Calculate custom input members
    for (size_t i = 0; i < outDesc.inputMembers.size(); ++i)
    {
        const MemberDecl& m = outDesc.inputMembers[i];
        if (!IsBuiltinInputMember(m.name, m.semantic))
            outDesc.customInputMembers.push_back(m);
    }
    outDesc.packedCustomInputMembers.Pack(outDesc.customInputMembers.size(), outDesc.customInputMembers.empty() ? NULL : &outDesc.customInputMembers[0]);

    // Output struct
    if (args[1].mod != kInputModInOut)
    {
        outErrors.AddImportError(Format("Surface shader's output parameter needs to have 'inout' modifier; found %s at '%s %s'",
            kModNames[args[1].mod], args[1].typeName.c_str(), args[1].name.c_str()), 0, false);
        return;
    }
    outDesc.outputType = args[1].typeName;
    outDesc.outputName = args[1].name;
    if (!FindStruct(&compiler.data->ast->compunit, args[1].typeName, outDesc.outputMembers))
    {
        outErrors.AddImportError("Can't find surface shader output struct '" + args[1].typeName + "'", 0, false);
        return;
    }

    // If we need vertex entry point, try to find it
    if (!vertexEntryPoint.empty())
    {
        MemberDeclarations vertexArgs;
        bool func = FindFunction(&compiler.data->ast->compunit, vertexEntryPoint, vertexArgs);
        if (!func)
        {
            outErrors.AddImportError("Surface shader vertex function '" + vertexEntryPoint + "' not found", 0, false);
            return;
        }
        if (!CheckArgCount("vertex ", vertexEntryPoint, vertexArgs, 1, 2, outErrors))
            return;

        outDesc.appdataType = vertexArgs[0].typeName;

        // inout appdata
        if (!CheckArgModifier("vertex ", vertexEntryPoint, vertexArgs, 0, kInputModInOut, outErrors))
            return;
        // (if present) out Input
        if (vertexArgs.size() == 2)
        {
            outDesc.vertexModCustomData = true;
            if (!CheckArgType("vertex ", vertexEntryPoint, vertexArgs, 1, outDesc.inputType, kInputModOut, outErrors))
                return;
        }
    }

    // Vertex data struct members
    if (!FindStruct(&compiler.data->ast->compunit, outDesc.appdataType, outDesc.appdataMembers))
    {
        outErrors.AddImportError("Can't find surface shader vertex data struct '" + outDesc.appdataType + "'", 0, false);
        return;
    }

    // If we need final color entry point, try to find it
    if (!finalColorEntryPoint.empty())
    {
        MemberDeclarations modArgs;
        bool func = FindFunction(&compiler.data->ast->compunit, finalColorEntryPoint, modArgs);
        if (!func)
        {
            outErrors.AddImportError("Surface shader final color function '" + finalColorEntryPoint + "' not found", 0, false);
            return;
        }

        // needs 3 args: in Input, in SurfaceOutput, inout float4 color
        if (!CheckArgCount("final color ", finalColorEntryPoint, modArgs, 3, 3, outErrors))
            return;
        // in Input
        if (!CheckArgType("final color ", finalColorEntryPoint, modArgs, 0, outDesc.inputType, kInputModIn, outErrors))
            return;
        // in SurfaceOutput
        if (!CheckArgType("final color ", finalColorEntryPoint, modArgs, 1, outDesc.outputType, kInputModIn, outErrors))
            return;
        // inout float4
        if (!CheckArgVec4Type("final color ", finalColorEntryPoint, modArgs, 2, kInputModInOut, outErrors))
            return;
    }

    // If we need final prepass entry point, try to find it
    if (!finalPrepassEntryPoint.empty())
    {
        MemberDeclarations modArgs;
        bool func = FindFunction(&compiler.data->ast->compunit, finalPrepassEntryPoint, modArgs);
        if (!func)
        {
            outErrors.AddImportError("Surface shader final prepass function '" + finalPrepassEntryPoint + "' not found", 0, false);
            return;
        }

        // needs 3 args: in Input, in SurfaceOutput, inout float4 normalspec
        if (!CheckArgCount("final prepass ", finalPrepassEntryPoint, modArgs, 3, 3, outErrors))
            return;
        // in Input
        if (!CheckArgType("final prepass ", finalPrepassEntryPoint, modArgs, 0, outDesc.inputType, kInputModIn, outErrors))
            return;
        // in SurfaceOutput
        if (!CheckArgType("final prepass ", finalPrepassEntryPoint, modArgs, 1, outDesc.outputType, kInputModIn, outErrors))
            return;
        // inout float4
        if (!CheckArgVec4Type("final prepass ", finalPrepassEntryPoint, modArgs, 2, kInputModInOut, outErrors))
            return;
    }

    // If we need final gbuffer entry point, try to find it
    if (!finalGBufferEntryPoint.empty())
    {
        MemberDeclarations modArgs;
        bool func = FindFunction(&compiler.data->ast->compunit, finalGBufferEntryPoint, modArgs);
        if (!func)
        {
            outErrors.AddImportError("Surface shader final gbuffer function '" + finalGBufferEntryPoint + "' not found", 0, false);
            return;
        }

        // needs 6 args: in Input, in SurfaceOutput, inout float4 diffuse, inout float4 specSmoothness, inout float4 normal, inout float4 emission
        if (!CheckArgCount("final gbuffer ", finalGBufferEntryPoint, modArgs, 6, 6, outErrors))
            return;
        // in Input
        if (!CheckArgType("final gbuffer ", finalGBufferEntryPoint, modArgs, 0, outDesc.inputType, kInputModIn, outErrors))
            return;
        // in SurfaceOutput
        if (!CheckArgType("final gbuffer ", finalGBufferEntryPoint, modArgs, 1, outDesc.outputType, kInputModIn, outErrors))
            return;
        // 4 inout float4s for 4 RTs
        for (int i = 0; i < 4; ++i)
        {
            if (!CheckArgVec4Type("final gbuffer ", finalGBufferEntryPoint, modArgs, 2 + i, kInputModInOut, outErrors))
                return;
        }
    }

    // Find lighting models
    FindLightingModels(&compiler.data->ast->compunit, outLightModels, outErrors);

    // Find tessellation model
    if (!FindTessellationModel(&compiler.data->ast->compunit, tessModifier, outDesc, outErrors))
        return;

    // Put into cache
    {
        Mutex::AutoLock lock(analysisCache.astMutex);
        SurfaceAnalysisCache::ASTResult cacheValue;
        cacheValue.desc = outDesc;
        cacheValue.lightModels = outLightModels;
        cacheValue.errors = outErrors;
        if (!analysisCache.astCache.insert(std::make_pair(key, cacheValue)).second)
        {
            // Some other thread already calculated same result and put into cache
            // in the meantime; count as "cache hit" for stats/tests -- as in single threaded
            // case it would have been.
            ++analysisCache.astCacheHits;
        }
    }
}

SurfaceAnalysisCache* CreateSurfaceAnalysisCache(const IncludeSearchContext& includeContext)
{
    SurfaceAnalysisCache* cache = new SurfaceAnalysisCache(includeContext);
    return cache;
}

void DestroySurfaceAnalysisCache(SurfaceAnalysisCache* cache)
{
    delete cache;
}

void GetSurfaceAnalysisCacheStats(const SurfaceAnalysisCache* cache, size_t& outRequests, size_t& outHits, size_t& outASTRequests, size_t& outASTHits)
{
    outRequests = cache->compileCacheRequests;
    outHits = cache->compileCacheHits;
    outASTRequests = cache->astCacheRequests;
    outASTHits = cache->astCacheHits;
}

const std::vector<core::string> GetSurfaceAnalysisOpenedIncludes(const SurfaceAnalysisCache* cache)
{
    return cache->handler.GetOpenedIncludeFiles();
}

static MemberType GetTypeFromHLSL(const D3D11_SHADER_TYPE_DESC& t, core::string& outName)
{
    if (t.Type == D3D10_SVT_FLOAT && (t.Class == D3D10_SVC_VECTOR || t.Class == D3D10_SVC_SCALAR) && t.Rows == 1)
    {
        if (t.Name && BeginsWith(t.Name, "half"))
        {
            if (t.Columns == 1)
            {
                outName = "half"; return kTypeHalf;
            }
            if (t.Columns == 2)
            {
                outName = "half2"; return kTypeHalf2;
            }
            if (t.Columns == 3)
            {
                outName = "half3"; return kTypeHalf3;
            }
            if (t.Columns == 4)
            {
                outName = "half4"; return kTypeHalf4;
            }
        }
        else
        {
            if (t.Columns == 1)
            {
                outName = "float"; return kTypeFloat;
            }
            if (t.Columns == 2)
            {
                outName = "float2"; return kTypeFloat2;
            }
            if (t.Columns == 3)
            {
                outName = "float3"; return kTypeFloat3;
            }
            if (t.Columns == 4)
            {
                outName = "float4"; return kTypeFloat4;
            }
        }
    }
    outName = t.Name ? t.Name : "<other>";
    return kTypeOther;
}

static core::string InputSemanticName(const core::string& semantic)
{
    // d3d9 sematics should be cosidered case insensitive
    const core::string semanticUpper = ToUpper(semantic);

    if (BeginsWith(semanticUpper, "POSITION"))
        return semanticUpper;
    if (BeginsWith(semanticUpper, "NORMAL"))
        return semanticUpper;
    if (BeginsWith(semanticUpper, "TANGENT"))
        return semanticUpper;
    if (BeginsWith(semanticUpper, "COLOR"))
        return semanticUpper;
    if (BeginsWith(semanticUpper, "TEXCOORD"))
        return semanticUpper;

    // if it was NOT known d3d9 semantic, return unchanged
    return semantic;
}

static void ReflectHLSLShader(const void* shaderCode, size_t shaderSize, const core::string& programName, int startLine, ShaderDesc& out, ShaderImportErrors& outErrors)
{
    D3D11ShaderReflection* reflector = NULL;
    HRESULT hr = gPluginDispatcher->GetD3DCompiler()->D3DReflect(shaderCode, shaderSize, (void**)&reflector);
    if (FAILED_IMPL(hr))
    {
        outErrors.AddImportError("Failed to get HLSL Reflection interface", startLine, false, kShaderCompPlatformD3D11);
        return;
    }

    D3D11_SHADER_DESC shaderDesc;
    reflector->GetDesc(&shaderDesc);

    // For surface shader analysis, we are only interested in that one special constant buffer
    // that we put things into.
    D3D11ShaderReflectionConstantBuffer* cb = reflector->GetConstantBufferByName(kSurfaceAnalysisHLSLConstantBuffer);
    if (cb != NULL)
    {
        D3D11_SHADER_BUFFER_DESC desc;
        hr = cb->GetDesc(&desc);
        // Seems that if CB by name does not exist, then HLSL can still return an actual pointer to
        // a CB interface; however that one will fail any GetDesc calls on it.
        if (FAILED_IMPL(hr))
        {
            goto _cleanup;
        }

        if (desc.Type == D3D11_CT_CBUFFER)
        {
            for (UINT j = 0; j < desc.Variables; ++j)
            {
                D3D11ShaderReflectionVariable* var = cb->GetVariableByIndex(j);
                D3D11_SHADER_VARIABLE_DESC vdesc;
                var->GetDesc(&vdesc);
                if (!(vdesc.uFlags & D3D10_SVF_USED))
                    continue;
                D3D11_SHADER_TYPE_DESC tdesc;
                var->GetType()->GetDesc(&tdesc);

                core::string typeName;
                MemberType type = GetTypeFromHLSL(tdesc, typeName);
                bool isInput = BeginsWith(vdesc.Name, "surfInput");
                bool isInputOut = BeginsWith(vdesc.Name, "surfInputOut");
                if (isInput || isInputOut)
                {
                    core::string mname = Format("_hlslin_.%s", vdesc.Name + (isInputOut ? 12 : 9));
                    core::string msemantic;
                    size_t splitPos = mname.find("_Semantic_");
                    if (splitPos != core::string::npos)
                    {
                        msemantic = InputSemanticName(mname.substr(splitPos + 10));
                        mname = mname.substr(0, splitPos);
                    }
                    out.inputs.push_back(MemberDecl(typeName, mname, msemantic, type));
                }
                if (BeginsWith(vdesc.Name, "surfOutput"))
                {
                    core::string mname = Format("_hlslout_.%s", vdesc.Name + 10);
                    core::string msemantic;
                    size_t splitPos = mname.find("_Semantic_");
                    if (splitPos != core::string::npos)
                    {
                        msemantic = mname.substr(splitPos + 10);
                        mname = mname.substr(0, splitPos);
                    }
                    out.outputs.push_back(MemberDecl(typeName, mname, msemantic, type));
                }
            }
        }
    }

_cleanup:
    reflector->Release();
}

static void AnalyzeHLSL(const core::string& source, const core::string& entryPoint, int startLine, ShaderDesc& out, ShaderImportErrors& outErrors)
{
    const ShaderCompilerPlatform api = kShaderCompPlatformD3D11;
    if (!gPluginDispatcher->GetD3DCompiler()->IsValid())
    {
        outErrors.AddImportError("D3D shader compiler could not be loaded", startLine, false, api);
        return;
    }

    // Compile
    D3D10Blob* shader = NULL;
    D3D10Blob* errors = NULL;
    // Optimization level 0 is a bit faster and does enough optimizations to power the surface
    // shader analysis step.
    DWORD flags = D3D10_SHADER_ENABLE_BACKWARDS_COMPATIBILITY | D3D10_SHADER_OPTIMIZATION_LEVEL0;
    HRESULT hr = gPluginDispatcher->GetD3DCompiler()->D3DCompile(
        source.c_str(),
        (unsigned long)source.size(),
        "",
        NULL,     // no macros
        NULL,     // no include handler
        entryPoint.c_str(),
        "ps_5_0",
        flags,
        0,
        &shader,
        &errors);

    const BYTE* shaderCode;
    DWORD shaderSize;

    // Check errors
    if (FAILED_IMPL(hr) || !shader)
    {
        bool reportUnknownErrors = false;
        core::string errorMsg;
        if (errors)
        {
            core::string msg(reinterpret_cast<const char*>(errors->GetBufferPointer()), errors->GetBufferSize());
            ParseErrorMessages(msg, false, "): fatal error ", entryPoint, startLine, api, outErrors);
            ParseErrorMessages(msg, false, "): error ", entryPoint, startLine, api, outErrors);
            ParseErrorMessages(msg, false, "): error: ", entryPoint, startLine, api, outErrors);
            ParseErrorMessagesStartLine(msg, "error ", entryPoint, startLine, api, outErrors);
            ParseErrorMessagesStartLine(msg, "internal error:", entryPoint, startLine, api, outErrors);
            errorMsg = msg;
        }
        else
            reportUnknownErrors = true;

        if (!outErrors.HasErrorsOrWarnings())
            reportUnknownErrors = true;

        if (reportUnknownErrors)
            outErrors.AddImportError(Format("Compilation failed (other error) '%s'", errorMsg.c_str()).c_str(), startLine, false, api);
        goto _cleanup;
    }

    // Get compiled shader
    Assert(shader);
    shaderCode = reinterpret_cast<const BYTE*>(shader->GetBufferPointer());
    shaderSize = shader->GetBufferSize();

    // Figure out inputs/outputs via reflection
    ReflectHLSLShader(shaderCode, shaderSize, entryPoint, startLine, out, outErrors);
    if (outErrors.HasErrors())
    {
        goto _cleanup;
    }

_cleanup:
    if (shader)
        shader->Release();
    if (errors)
        errors->Release();
}

void AnalyzeSurfaceShaderViaCompile(const core::string& gen, const core::string& entryPoint, int startLine, const IncludePaths& includePaths, const ShaderCompilerPlatform api, const ShaderArgs& args, ShaderDesc& out, ShaderImportErrors& outErrors, SurfaceAnalysisCache& analysisCache)
{
    // have we analyzed an identical (after preprocessing) surface function already?
    SurfaceAnalysisCache::CacheKey key;
    {
        key.source = analysisCache.PreprocessSource(gen, args, includePaths);
        key.entryPoint = entryPoint;
        Mutex::AutoLock lock(analysisCache.compileMutex);
        SurfaceAnalysisCache::CompileCacheMap::const_iterator it = analysisCache.compileCache.find(key);
        ++analysisCache.compileCacheRequests;
        if (it != analysisCache.compileCache.end())
        {
            ++analysisCache.compileCacheHits;
            out = it->second;
            return;
        }
    }

    ShaderImportErrors errors;

    // Run analysis via HLSL compiler; note that we have already preprocessed the source so no need to do any include/macro handling
    AnalyzeHLSL(key.source, entryPoint, startLine, out, errors);
    outErrors.AddErrorsFrom(errors);
    if (errors.HasErrors())
        return;

    // Put into cache
    {
        Mutex::AutoLock lock(analysisCache.compileMutex);
        if (!analysisCache.compileCache.insert(key, out).second)
        {
            // Some other thread already calculated same result and put into cache
            // in the meantime; count as "cache hit" for stats/tests -- as in single threaded
            // case it would have been.
            ++analysisCache.compileCacheHits;
        }
    }
}
