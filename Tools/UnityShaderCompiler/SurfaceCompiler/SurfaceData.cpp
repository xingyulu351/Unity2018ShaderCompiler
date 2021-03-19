#include "UnityPrefix.h"
#include "SurfaceData.h"
#include "SurfaceCompiler.h"
#include "Runtime/Utilities/Word.h"
#include "SurfaceAnalysis.h"
#include "../Utilities/ShaderImportUtils.h"
#include "../Utilities/ShaderImportErrors.h"
#include "artifacts/generated/Configuration/UnityConfigureVersion.gen.h"
#include "Runtime/Core/Format/Format.h"

using core::string;

ShaderCompilerPlatform SurfaceData::AnalysisShaderCompilerPlatform = kShaderCompPlatformD3D9_Obsolete;

int GetMemberTypeDimension(MemberType type)
{
    switch (type)
    {
        case kTypeFloat:
        case kTypeHalf:
        case kTypeFixed:
            return 1;
        case kTypeFloat2:
        case kTypeHalf2:
        case kTypeFixed2:
            return 2;
        case kTypeFloat3:
        case kTypeHalf3:
        case kTypeFixed3:
            return 3;
        case kTypeFloat4:
        case kTypeHalf4:
        case kTypeFixed4:
            return 4;
        default: return 2;
    }
}

static TypePrec GetMemberTypePrecision(MemberType type)
{
    switch (type)
    {
        case kTypeFixed:
        case kTypeFixed2:
        case kTypeFixed3:
        case kTypeFixed4:
            return kPrecFixed;
        case kTypeHalf:
        case kTypeHalf2:
        case kTypeHalf3:
        case kTypeHalf4:
            return kPrecHalf;
        case kTypeFloat:
        case kTypeFloat2:
        case kTypeFloat3:
        case kTypeFloat4:
            return kPrecFloat;
        default:
            return kPrecFloat;
    }
}

void PackedInterpolators::Pack(size_t count, const MemberDecl* texCoords)
{
    offsets.resize(count);
    for (size_t i = 0; i < count; ++i)
    {
        const MemberDecl& tc = texCoords[i];
        const int tcDim = GetMemberTypeDimension(tc.type);
        const TypePrec tcPrec = GetMemberTypePrecision(tc.type);

        // try to fit into packed ones
        bool found = false;
        for (size_t j = 0; j < packed.size(); ++j)
        {
            Packed& pack = packed[j];
            if (pack.usedComps + tcDim > 4)
                continue; // would not fit

            if (pack.precision != tcPrec)
                continue; // different precision

            found = true;
            offsets[i].index = static_cast<int>(j);
            offsets[i].offset = pack.usedComps;
            pack.usedComps += tcDim;
            if (!pack.names.empty())
                pack.names += ' ';
            pack.names += tc.name;
            break;
        }

        // not found, create new packed texcoord
        if (found)
            continue;
        Packed ptc;
        ptc.usedComps = tcDim;
        ptc.precision = tcPrec;
        ptc.names = tc.name;
        offsets[i].index = static_cast<int>(packed.size());
        offsets[i].offset = 0;
        packed.push_back(ptc);
    }
}

void EmitProlog(core::string& gen, const SurfaceParams& params, bool includeAutoLight, bool pbs, PerPixelReflectMode perPixelReflect)
{
    // PBS defines
    if (pbs)
    {
        if (params.o.mode == kSurfaceModeAlpha)
        {
            if (params.o.blend == kSurfaceBlendAuto || params.o.blend == kSurfaceBlendPremultiply)
                gen += "#define _ALPHAPREMULTIPLY_ON 1\n";
            else
                gen += "#define _ALPHABLEND_ON 1\n";
        }
    }

    if (params.o.noLPPV)
    {
        gen += "// Stripping Light Probe Proxy Volume code because nolppv pragma is used. Using normal probe blending as fallback.\n";
        gen += "#ifdef UNITY_LIGHT_PROBE_PROXY_VOLUME\n";
        gen += "#undef UNITY_LIGHT_PROBE_PROXY_VOLUME\n";
        gen += "#endif\n";
    }

    // includes
    gen += "#include \"UnityCG.cginc\"\n";
    if (params.o.noLightmap)
    {
        gen += "//Shader does not support lightmap thus we always want to fallback to SH.\n";
        gen += "#undef UNITY_SHOULD_SAMPLE_SH\n";
        gen += "#define UNITY_SHOULD_SAMPLE_SH (!defined(UNITY_PASS_FORWARDADD) && !defined(UNITY_PASS_PREPASSBASE) && !defined(UNITY_PASS_SHADOWCASTER) && !defined(UNITY_PASS_META))\n";
    }
    if (params.o.needsLightingInclude)
        gen += "#include \"Lighting.cginc\"\n";
    if (params.o.needsPBSLightingInclude)
        gen += "#include \"UnityPBSLighting.cginc\"\n";
    if (includeAutoLight)
        gen += "#include \"AutoLight.cginc\"\n";
    gen += "\n";

    // internal data
    switch (perPixelReflect)
    {
        case kPerPixelReflectYes:
            gen += "#define INTERNAL_DATA half3 internalSurfaceTtoW0; half3 internalSurfaceTtoW1; half3 internalSurfaceTtoW2;\n";
            gen += "#define WorldReflectionVector(data,normal) reflect (data.worldRefl, half3(dot(data.internalSurfaceTtoW0,normal), dot(data.internalSurfaceTtoW1,normal), dot(data.internalSurfaceTtoW2,normal)))\n";
            gen += "#define WorldNormalVector(data,normal) fixed3(dot(data.internalSurfaceTtoW0,normal), dot(data.internalSurfaceTtoW1,normal), dot(data.internalSurfaceTtoW2,normal))\n";
            break;
        case kPerPixelReflectNo:
            gen += "#define INTERNAL_DATA\n";
            gen += "#define WorldReflectionVector(data,normal) data.worldRefl\n";
            gen += "#define WorldNormalVector(data,normal) normal\n";
            break;
        case kPerPixelReflectAnalysis:
            gen += "#define INTERNAL_DATA\n";
            gen += "#define WorldReflectionVector(data,normal) data.worldRefl+normal\n";
            gen += "#define WorldNormalVector(data,normal) data.worldNormal+normal\n";
            break;
    }

    // original surface shader source
    gen += "\n// Original surface shader snippet:\n";
    gen += params.source;
    gen += "\n";
}

static core::string GetMaskFromType(MemberType t)
{
    switch (t)
    {
        case kTypeFloat4:
        case kTypeHalf4:
        case kTypeFixed4:
            return "rgba";
        case kTypeFloat3:
        case kTypeHalf3:
        case kTypeFixed3:
            return "rgb";
        case kTypeFloat2:
        case kTypeHalf2:
        case kTypeFixed2:
            return "rg";
        case kTypeFloat:
        case kTypeHalf:
        case kTypeFixed:
            return "r";
        default:
            return "";
    }
}

#define kInternalFuncName "SurfShaderInternalFunc"


// When doing analysis with HLSL compiler, we need to encode original member variable semantic into
// the name, since we are stuffing everything into a constant buffer. Append semantic with a special
// delimiter; when doing compiled shader reflection this will be recognized and parsed back into semantic.
static core::string GetHLSLVarName(const MemberDecl& m)
{
    if (m.semantic.empty())
        return m.name;
    return Format("%s_Semantic_%s", m.name.c_str(), m.semantic.c_str());
}

static core::string GetHLSLMemberDecl(const MemberDecl& m, const char* kind)
{
    core::string s = Format("  %s %s%s", m.typeName.c_str(), kind, GetHLSLVarName(m).c_str());
    if (m.arraySize >= 0)
        s += Format("[%i]", m.arraySize);
    s += ";\n";
    return s;
}

// For doing the surface shader analysis, this one generates entry point that calls into user-written
// surface function, in order to find which things are *read* from. Basically
// given a typical surface function "surf (Input i, inout SurfaceOutput o)",
// it generates an entry point that would take both "Input" and "SurfaceOutput" as incoming data,
// call the surface function, and use part of the result based on surface pass type:
//
// Forward base:    lighting + Albedo, Emission, Alpha (only for alpha&alpha test modes)
// Forward add:     lighting + Alpha (only for alpha&alpha test modes)
// Prepass base:    Normal, Specular
// Prepass final:   lightingPrepass + Emission
// Deferred:        lightingDeferred, Alpha (only for alpha&alpha test modes)
// Shadow:          Alpha (only for alpha test mode)
// Meta:            Albedo, Emission
static core::string GenerateShaderForInputs(
    const core::string& initial,
    SurfaceMode mode,
    SurfacePass pass,
    const core::string& entryName,
    const core::string& finalColorModifier,
    const core::string& finalPrepassModifier,
    const core::string& finalGBufferModifier,
    const LightingModel& lighting,
    bool lightNeedsViewDir,
    const SurfaceFunctionDesc& desc,
    bool includeLighting)
{
    const bool hasLightingDirLightmap = ((lighting.renderPaths & (1 << kPathDirLightmap)) != 0);
    const bool dirLightmapNeedsViewDir = lighting.UseViewDirLightmap();

    core::string dirLightmap;
    dirLightmap += "  half3 specColor;\n";
    dirLightmap += "  res += Lighting" + lighting.name + "_DirLightmap (o, half4(1,1,1,1), half4(1,1,1,1), ";
    dirLightmap += dirLightmapNeedsViewDir ? "half3(1,1,1), true, specColor);\n" : "true);\n";

    core::string viewDir = "";
    if (lighting.useViewDir)
        viewDir = ", half3(7,7,7)";

    core::string unityGIDeclare;
    // HLSL compiler needs various structures to be fully initialized.
    // Do not just initialize to zero, since dead code elimination is smart enough to fold
    // some expressions into a "yep, not really used" result; initialize to something
    // that is extremely unlikely to exactly match literal constants used by shader code.
    unityGIDeclare += "  UnityGI gi = (UnityGI)1234;\n";
    unityGIDeclare += "  UnityGIInput giInput = (UnityGIInput)1234;\n";
    unityGIDeclare += "  giInput.light = gi.light;\n";
    unityGIDeclare += "  Lighting" + lighting.name + "_GI(o, giInput, gi);\n";

    core::string gen;

    // To figure out what is read by the shader, we put both input & output members into a constant
    // buffer, fill Input & Output structs from them, and then run the surface shader function.

    gen += "\ncbuffer " kSurfaceAnalysisHLSLConstantBuffer " {\n";
    for (size_t i = 0; i < desc.inputMembers.size(); ++i)
    {
        gen += GetHLSLMemberDecl(desc.inputMembers[i], "surfInput");
    }
    for (size_t i = 0; i < desc.outputMembers.size(); ++i)
    {
        gen += GetHLSLMemberDecl(desc.outputMembers[i], "surfInputOut"); // put output members as "input data" too, hence surfInputOut prefix
    }
    gen += "};\n";
    gen += "\nfloat4 " kInternalFuncName " (float _surfInternalAdditionalDummyInput : TEXCOORD0) : SV_Target {\n";
    gen += "  Input surfIN = (Input)1234;\n"; // same initialization reason as above
    for (size_t i = 0; i < desc.inputMembers.size(); ++i)
    {
        const MemberDecl& m = desc.inputMembers[i];
        gen += Format("  surfIN.%s = surfInput%s", m.name.c_str(), GetHLSLVarName(m).c_str());
        // We put "inputs" into a constant buffer, to figure out which ones got used later on.
        // However depending on how a shader actually uses said, inputs, the HLSL compiler can optimize
        // them away, e.g. a derivative instruction on an input, when it's placed into a constant buffer,
        // will get completely optimized out (derivative of constant value is zero).
        // So for all "regular" (non-struct/void) input values, modify their initial value with
        // a fake dummy input that the compiler can't optimize away.
        if ((m.type != kTypeOther) && (m.type != kTypeVoid) && (m.arraySize == -1))
        {
            gen += " * _surfInternalAdditionalDummyInput + _surfInternalAdditionalDummyInput";
        }
        gen += ";\n";
    }
    gen += Format("  %s o = (%s)0;\n", desc.outputType.c_str(), desc.outputType.c_str());
    for (size_t i = 0; i < desc.outputMembers.size(); ++i)
    {
        gen += Format("  o.%s = surfInputOut%s;\n", desc.outputMembers[i].name.c_str(), GetHLSLVarName(desc.outputMembers[i]).c_str());
    }

    // Call user-written surface shader function
    gen += "  " + entryName + " (surfIN, o);\n";

    // Use appropriate part of the result based on what is the pass type that we are analyzing.
    gen += "  half4 res = 0;\n";
    switch (pass)
    {
        case kSurfFwdBase:
            gen += "  half4 light = 1;\n";
            if (includeLighting)
            {
                if (lighting.useGIForward)
                {
                    gen += unityGIDeclare;
                    gen += "  res += Lighting" + lighting.name + " (o" + viewDir + ", gi);\n";
                    //@TODO: GI function?
                }
                else
                {
                    gen += "  res += Lighting" + lighting.name + " (o, half3(1,1,1), 1";
                    if (lightNeedsViewDir)
                        gen += ", half3(7,7,7)";
                    gen += ");\n";
                    if (hasLightingDirLightmap)
                        gen += dirLightmap;
                }
            }
            gen += "  res.rgb += o.Albedo;\n";
            gen += "  res.rgb += o.Emission;\n";
            if (mode == kSurfaceModeAlpha || mode == kSurfaceModeAlphaTest || mode == kSurfaceModeDecalBlend)
                gen += "  res.a += o.Alpha;\n";
            break;

        case kSurfFwdAdd:
            gen += "  half4 light = 1;\n";
            if (includeLighting)
            {
                if (lighting.useGIForward)
                {
                    gen += unityGIDeclare;
                    gen += "  res += Lighting" + lighting.name + " (o" + viewDir + ", gi);\n";
                }
                else
                {
                    gen += "  res += Lighting" + lighting.name + " (o, half3(1,1,1), 1";
                    if (lightNeedsViewDir)
                        gen += ", half3(7,7,7)";
                    gen += ");\n";
                }
            }
            if (mode == kSurfaceModeAlpha || mode == kSurfaceModeAlphaTest || mode == kSurfaceModeDecalBlend)
                gen += "  res.a += o.Alpha;\n";
            else
                gen += "  res.a = 0;\n";
            break;
        case kSurfPrepassFinal:
            gen += "  half4 light = 1;\n";
            if (includeLighting)
            {
                gen += "  res += Lighting" + lighting.name + "_PrePass (o, light);\n";
                if (hasLightingDirLightmap)
                    gen += dirLightmap;
            }
            gen += "  res.rgb += o.Emission;\n";
            if (mode == kSurfaceModeAlpha || mode == kSurfaceModeAlphaTest || mode == kSurfaceModeDecalBlend)
                gen += "  res.a += o.Alpha;\n";
            break;
        case kSurfPrepassBase:
            gen += "  res.rgb = o.Normal;\n";
            gen += "  res.a = o.Specular;\n";
            if (mode == kSurfaceModeAlphaTest || mode == kSurfaceModeDecalBlend)
                gen += "  res.a += o.Alpha;\n";
            break;
        case kSurfDeferred:
            gen += "  half4 ddiff, dspec, dnormal;\n";
            if (includeLighting)
            {
                if (lighting.useGIDeferred)
                {
                    gen += unityGIDeclare;
                    gen += "  res += Lighting" + lighting.name + "_Deferred (o" + viewDir + ", gi, ddiff, dspec, dnormal);\n";
                }
                else
                {
                    gen += "  res += Lighting" + lighting.name + "_Deferred (o, ddiff, dspec, dnormal);\n";
                }
                gen += "  res += ddiff + dspec + dnormal;\n";
                if (hasLightingDirLightmap)
                    gen += dirLightmap;
            }
            if (mode == kSurfaceModeAlpha || mode == kSurfaceModeAlphaTest || mode == kSurfaceModeDecalBlend)
                gen += "  res.a += o.Alpha;\n";
            break;
        case kSurfShadow:
            if (mode == kSurfaceModeAlphaTest)
                gen += "  res.a += o.Alpha;\n";
            break;
        case kSurfMeta:
            gen += "  res.rgb += o.Albedo;\n";
            gen += "  res.rgb += o.Emission;\n";
            break;
        default:
            AssertString("unknown mode");
    }

    // has finalcolor/finalprepass/finalgbuffer modifier, make sure this is called in order to generate the correct input dependencies
    // for that code too, it may reference inputs not touched by the 'entryName' function
    if (pass == kSurfPrepassBase && !finalPrepassModifier.empty())
        gen += "  " + finalPrepassModifier + " (surfIN, o, res);\n";
    else if (pass == kSurfDeferred && !finalGBufferModifier.empty())
        gen += "  " + finalGBufferModifier + " (surfIN, o, ddiff, dspec, dnormal, res);\n";
    else if (!finalColorModifier.empty())
        gen += "  " + finalColorModifier + " (surfIN, o, res);\n";
    gen += "  return res;\n";
    gen += "}\n";
    return gen;
}

// For doing the surface shader analysis, this one generates entry point that calls into user-written
// surface function, in order to find which things are *written into*.
//
// We are only interested in a few special inputs that drive the code generation
// (Normal, Emission etc.), and the detection works by (ab)using the optimizing compiler we are doing
// the analysis with:
//
// thing = MagicValue
// callUserSurfaceFunction()
// if (thing != MagicValue) // means it is written into
//     useSomeInputThatWouldShowUpInReflection
//
// Since shader compilers inline everything & code serious dead code removal, algebraic optimizations
// etc., if a "thing" is not actually written into then the compiler removes
// useSomeInputThatWouldShowUpInReflection part.
static core::string GenerateShaderForOutputs(const core::string& initial, const core::string& entryName, const SurfaceFunctionDesc& desc)
{
    bool hasScalarSpecular = false;
    const MemberDecl* spec = desc.FindOutputMember("Specular");
    if (spec && (spec->type == kTypeFloat || spec->type == kTypeHalf || spec->type == kTypeFixed))
        hasScalarSpecular = true;

    bool hasScalarOcclusion = false;
    const MemberDecl* occlusion = desc.FindOutputMember("Occlusion");
    if (occlusion && (occlusion->type == kTypeFloat || occlusion->type == kTypeHalf || occlusion->type == kTypeFixed))
        hasScalarOcclusion = true;

    core::string gen;

    // Put inputs into a constant buffer and use them to pre-fill Input structure before calling user
    // code. Put output members into a constant buffer too, with a different prefix,
    // and we'll make "useSomeInputThatWouldShowUpInReflection" read from these.

    gen += "\ncbuffer " kSurfaceAnalysisHLSLConstantBuffer " {\n";
    for (size_t i = 0; i < desc.inputMembers.size(); ++i)
    {
        gen += GetHLSLMemberDecl(desc.inputMembers[i], "surfInput");
    }
    for (size_t i = 0; i < desc.outputMembers.size(); ++i)
    {
        gen += GetHLSLMemberDecl(desc.outputMembers[i], "surfOutput");
    }
    gen += "};\n";
    gen += "\nfloat4 " kInternalFuncName " (float _surfInternalAdditionalDummyInput : TEXCOORD0) : SV_Target {\n";
    gen += "  Input i = (Input)0;\n";
    for (size_t i = 0; i < desc.inputMembers.size(); ++i)
    {
        const MemberDecl& m = desc.inputMembers[i];
        gen += Format("  i.%s = surfInput%s", m.name.c_str(), GetHLSLVarName(m).c_str());
        // Same case as in GenerateShaderForInputs; need additional input to prevent compiler from optimizing some inputs away
        if ((m.type != kTypeOther) && (m.type != kTypeVoid) && (m.arraySize == -1))
        {
            gen += " * _surfInternalAdditionalDummyInput + _surfInternalAdditionalDummyInput";
        }
        gen += ";\n";
    }
    gen += Format("  %s o = (%s)0;\n", desc.outputType.c_str(), desc.outputType.c_str());
    // Initialize to magic values that are extremely unlikely to exactly match some literal constants
    // used in user code.
    gen += "  o.Normal = -1337;\n";
    gen += "  o.Emission = -1338;\n";
    if (hasScalarSpecular)
        gen += "  o.Specular = 1339;\n";
    if (hasScalarOcclusion)
        gen += "  o.Occlusion = 1340;\n";
    // Call surface function
    gen += "  " + entryName + " (i, o);\n";
    gen += "  float r = 0;\n";
    // If the variables are modified in any way ("any" call checks all their components),
    // make their corresponding sentinel variables in the constant buffer be used. Since HLSL
    // compiler is really good at dead code removal, we have to make sure the actual variables
    // are used for final result too; a dot of both achieves that nicely.
    gen += "  if (any(o.Normal-(-1337))) r += dot(o.Normal,surfOutputNormal);\n";
    gen += "  if (any(o.Emission-(-1338))) r += dot(o.Emission,surfOutputEmission);\n";
    if (hasScalarSpecular)
        gen += "  if (o.Specular-1339) r += o.Specular+surfOutputSpecular;\n";
    if (hasScalarOcclusion)
        gen += "  if (o.Occlusion-1340) r += o.Occlusion+surfOutputOcclusion;\n";
    gen += "  return r;\n";
    gen += "}\n";

    return gen;
}

void SetSurfaceShaderArgs(const ShaderCompilerPlatform api, const ShaderCompileRequirements& requirements, const StringArray& keywords, ShaderArgs& args)
{
    ShaderRequirements reqs = requirements.m_BaseRequirements;
    for (size_t i = 0; i < keywords.size(); ++i)
    {
        args.push_back(ShaderCompileArg(keywords[i], "1"));

        // Instancing keyword normally implies bumping up shader requirements to SM3.5; however for surface shader analysis we treat instancing as not really
        // supported -- it does not affect what is being read/written, and making both instancing & no-instancing variants pass the same SHADER_TARGET
        // value means more analysis variants can skip doing the actual work, since after full preprocessing they end up already having the result in cache.
        if (keywords[i] == "INSTANCING_ON")
            continue;
        reqs |= requirements.GetRequirementsForKeyword(keywords[i]);
    }
    AddShaderTargetRequirementsMacros(reqs, args);
    args.push_back(ShaderCompileArg("SHADER_TARGET_SURFACE_ANALYSIS", "1"));
    args.push_back(ShaderCompileArg("UNITY_VERSION", IntToString(CreateNumericUnityVersion(UNITY_VERSION_VER, UNITY_VERSION_MAJ, UNITY_VERSION_MIN))));
    args.push_back(ShaderCompileArg(kShaderCompPlatformDefines[api], "1"));
}

void CalculateSurfaceFunctionData(const SurfaceParams& params, const StringArray& keywords, SurfaceFunctionData& data, core::string& result, ShaderImportErrors& outErrors, SurfaceAnalysisCache& analysisCache)
{
    result.clear();

    const ShaderCompilerPlatform api = SurfaceData::AnalysisShaderCompilerPlatform;
    ShaderArgs args;
    SetSurfaceShaderArgs(api, params.target, keywords, args);

    core::string gen;
    gen += MakeShaderSourcePrefix(params.p);
    if (params.o.needsLightingInclude)
        gen += "#include \"Lighting.cginc\"\n";
    if (params.o.needsPBSLightingInclude)
        gen += "#include \"UnityPBSLighting.cginc\"\n";
    EmitProlog(gen, params, false, false, kPerPixelReflectNo);

    // Analyze surface function's argument names, input/output struct members
    ShaderImportErrors analysisErrors;
    ShaderArgs argsAST = args;
    argsAST.push_back(ShaderCompileArg("SHADER_TARGET_SURFACE_ANALYSIS_MOJOSHADER", "1"));
    AnalyseSurfaceShaderAST(
        gen, argsAST,
        params.p.entryName[kProgramSurface], params.p.entryName[kProgramVertex],
        params.o.finalColorModifier, params.o.finalPrepassModifier, params.o.finalGBufferModifier,
        params.o.tessModifier,
        params.includeContext.includePaths,
        data.desc, data.lightModels,
        analysisErrors, analysisCache);

    // Check if surface function and lighting model agree on output type
    LightingModels::const_iterator lit = data.lightModels.find(params.o.lighting);
    if (lit != data.lightModels.end())
    {
        if (data.desc.outputType != lit->second.outputStructName)
        {
            core::string msg = Format("Surface function '%s' and lighting model '%s' have mismatching output types ('%s' vs '%s')", params.p.entryName[kProgramSurface].c_str(), lit->second.name.c_str(), data.desc.outputType.c_str(), lit->second.outputStructName.c_str());
            analysisErrors.AddImportError(msg, 0, false);
        }
    }

    // massage line numbers of errors
    bool hadErrors = false;
    for (ShaderImportErrors::ErrorContainer::iterator it = analysisErrors.GetErrors().begin(); it != analysisErrors.GetErrors().end(); ++it)
    {
        int line = (it->line > 0) ? it->line : params.startLine;
        outErrors.AddImportError(it->message, it->file, line, it->warning, it->api);
        if (!it->warning)
        {
            hadErrors = true;
            result += Format("// %s:%d %s\n", it->file.c_str(), line, it->message.c_str());
        }
    }
    if (hadErrors)
    {
        result = Format("// errors analysing surface function '%s':\n", params.p.entryName[kProgramSurface].c_str()) + result;
        return;
    }

    // Compile it as dummy fragment program
    ShaderImportErrors cgErrors;
    ShaderDesc desc;
    core::string gen2 = gen + GenerateShaderForOutputs(gen, params.p.entryName[kProgramSurface], data.desc);
    AnalyzeSurfaceShaderViaCompile(gen2, kInternalFuncName, params.startLine, params.includeContext.includePaths, api, args, desc, cgErrors, analysisCache);
    outErrors.AddErrorsFrom(cgErrors);
    if (cgErrors.HasErrors())
    {
        result = Format("// error compiling initial surface function '%s':\n", params.p.entryName[kProgramSurface].c_str()) + gen + "\n";
        return;
    }

    // See which outputs surface functions writes into
    for (size_t i = 0; i < desc.outputs.size(); ++i)
    {
        const MemberDecl& m = desc.outputs[i];
        if (EndsWith(m.name, ".Normal"))
            data.writesNormal = true;
        if (EndsWith(m.name, ".Specular"))
            data.writesSpecular = true;
        if (EndsWith(m.name, ".Emission"))
            data.writesEmission = true;
        if (EndsWith(m.name, ".Occlusion"))
            data.writesOcclusion = true;
    }
}

bool CheckVertexInputRequirements(SurfaceParams& params, const SurfaceFunctionData& funcData, const SurfacePassData& passData, ShaderImportErrors& outErrors)
{
    // We need:
    // float4 position
    // float3 normal
    // if using tangent space:
    //      float4 tangent
    // if using vertex color:
    //      float4 color
    //
    // Also emit warnings and disable things if we don't have:
    // texcoord1: disable lightmaps & meta pass
    // texcoord2: disable dynamic lightmaps & meta pass

    bool havePosition = false;
    bool haveNormal = false;
    bool haveTangent = false;
    bool haveColor = false;
    bool haveTexcoord1 = false;
    bool haveTexcoord2 = false;
    for (size_t i = 0; i < funcData.desc.appdataMembers.size(); ++i)
    {
        const MemberDecl& m = funcData.desc.appdataMembers[i];
        if (m.type == kTypeFixed4 || m.type == kTypeHalf4 || m.type == kTypeFloat4)
        {
            if (m.name == "vertex")
                havePosition = true;
            if (m.name == "tangent")
                haveTangent = true;
            if (m.name == "color")
                haveColor = true;
        }
        if (m.type == kTypeFixed3 || m.type == kTypeHalf3 || m.type == kTypeFloat3)
        {
            if (m.name == "normal")
                haveNormal = true;
        }
        if (m.name == "texcoord1")
            haveTexcoord1 = true;
        if (m.name == "texcoord2")
            haveTexcoord2 = true;
    }

    bool haveErrors = false;
    if (!havePosition)
    {
        outErrors.AddImportError(Format("Surface shader's vertex input struct (%s) needs to have a 'float4 vertex' member",
            funcData.desc.appdataType.c_str()), 0, false);
        haveErrors = true;
    }
    if (!haveNormal)
    {
        outErrors.AddImportError(Format("Surface shader's vertex input struct (%s) needs to have a 'float3 normal' member",
            funcData.desc.appdataType.c_str()), 0, false);
        haveErrors = true;
    }
    if (passData.useTangentSpace)
    {
        if (!haveTangent)
        {
            outErrors.AddImportError(Format("Surface shader's vertex input struct (%s) needs to have a 'float4 tangent' member",
                funcData.desc.appdataType.c_str()), 0, false);
            haveErrors = true;
        }
    }
    if (passData.needsVertexColor)
    {
        if (!haveColor)
        {
            outErrors.AddImportError(Format("Surface shader's vertex input struct (%s) needs to have a 'float4 color' member",
                funcData.desc.appdataType.c_str()), 0, false);
            haveErrors = true;
        }
    }
    if (passData.needsVFace)
    {
        if (!HasAllFlags(params.target.m_BaseRequirements, kShaderRequireShaderModel30))
        {
            outErrors.AddImportError("Surface shader input uses VFACE; this requires #pragma target 3.0 or higher", 0, false);
            haveErrors = true;
        }
    }

    // when lightmaps are turned off, all the below (meta/lightmaps/...) is turned off too, so no need
    // for warnings
    if (!params.o.noLightmap)
    {
        if (!haveTexcoord1 && (!params.o.noLightmap || !params.o.noMeta))
        {
            outErrors.AddImportError(Format("texcoord1 missing from surface shader's vertex input struct (%s), disabled lightmaps and meta pass generation",
                funcData.desc.appdataType.c_str()), 0, true);
            params.o.noLightmap = true;
            params.o.noMeta = true;
        }

        if (!haveTexcoord2 && (!params.o.noDynLightmap || !params.o.noMeta))
        {
            outErrors.AddImportError(Format("texcoord2 missing from surface shader's vertex input struct (%s), disabled dynamic GI and meta pass generation",
                funcData.desc.appdataType.c_str()), 0, true);
            params.o.noDynLightmap = true;
            params.o.noMeta = true;
        }
    }

    return haveErrors;
}

void CalculateSurfacePassData(const SurfaceParams& params, const StringArray& keywords, SurfacePass pass, const SurfaceFunctionData& funcData, SurfacePassData& data, core::string& result, bool lightNeedsViewDir, ShaderImportErrors& outErrors, SurfaceAnalysisCache& analysisCache)
{
    result.clear();

    const ShaderCompilerPlatform api = SurfaceData::AnalysisShaderCompilerPlatform;
    ShaderArgs args;
    SetSurfaceShaderArgs(api, params.target, keywords, args);

    core::string gen1;
    gen1 += MakeShaderSourcePrefix(params.p);
    EmitProlog(gen1, params, false, false, kPerPixelReflectNo);

    // To analyze which inputs are read by the surface function, generate two dummy shaders:
    // 1) one that does call the lighting function (also the DirLightmaps lighting function),
    // 2) one that does not call the lighting function

    core::string gen2;
    gen2 += MakeShaderSourcePrefix(params.p);
    bool emitPPLReflect = false;
    if (funcData.writesNormal)
    {
        for (size_t i = 0; i < funcData.desc.inputMembers.size(); ++i)
        {
            if (funcData.desc.inputMembers[i].name == "worldRefl" || funcData.desc.inputMembers[i].name == "worldNormal")
            {
                emitPPLReflect = true;
                break;
            }
        }
    }

    EmitProlog(gen2, params, false, false, emitPPLReflect ? kPerPixelReflectAnalysis : kPerPixelReflectNo);

    const LightingModel dummyLightModel;
    const LightingModel* lightModel = &dummyLightModel;
    LightingModels::const_iterator lmit = funcData.lightModels.find(params.o.lighting);
    if (lmit != funcData.lightModels.end())
    {
        lightModel = &lmit->second;
    }
    core::string gen3 = gen2;
    gen2 += GenerateShaderForInputs(gen2, params.o.mode, pass,
        params.p.entryName[kProgramSurface],
        params.o.finalColorModifier, params.o.finalPrepassModifier, params.o.finalGBufferModifier,
        *lightModel,
        lightNeedsViewDir,
        funcData.desc, true);

    ShaderDesc desc2;
    AnalyzeSurfaceShaderViaCompile(gen2, kInternalFuncName, params.startLine, params.includeContext.includePaths, api, args, desc2, outErrors, analysisCache);
    if (outErrors.HasErrors())
    {
        result = Format("// error compiling initial surface function '%s':\n", kInternalFuncName) + gen2 + "\n";
        return;
    }


    // Some passes don't need different shader for analysis, since they never call the lighting
    // function. Save on dummy compilation count and reuse results from the already-made
    // compilation in that case.
    ShaderDesc desc3;
    const bool needsSeparateAnalysisWithoutLighting = (pass == kSurfFwdBase || pass == kSurfFwdAdd || pass == kSurfPrepassFinal);
    if (needsSeparateAnalysisWithoutLighting)
    {
        gen3 += GenerateShaderForInputs(gen2, params.o.mode, pass,
            params.p.entryName[kProgramSurface],
            params.o.finalColorModifier, params.o.finalPrepassModifier, params.o.finalGBufferModifier,
            *lightModel,
            lightNeedsViewDir,
            funcData.desc, false);

        AnalyzeSurfaceShaderViaCompile(gen3, kInternalFuncName, params.startLine, params.includeContext.includePaths, api, args, desc3, outErrors, analysisCache);
        if (outErrors.HasErrors())
        {
            result = Format("// error compiling initial surface function '%s':\n", kInternalFuncName) + gen3 + "\n";
            return;
        }
    }

    // see which inputs are used by surface function
    for (size_t i = 0; i < desc2.inputs.size(); ++i)
    {
        const MemberDecl& m = desc2.inputs[i];

        // remove everything up to and including dot
        size_t dotIndex = m.name.find('.', 0);
        core::string_ref n = (dotIndex != string::npos) ?
            m.name.substr(dotIndex + 1, m.name.size() - dotIndex - 1) :
            core::string_ref(m.name);

        if (BeginsWith(m.semantic, "COLOR"))
        {
            data.vertexColorField = n;
            data.needsVertexColor = true;
            continue;
        }

        if (m.semantic == "VFACE" || m.semantic == "FACE")
        {
            data.vfaceField = n;
            data.needsVFace = true;
            continue;
        }

        if (n.starts_with("uv"))
        {
            core::string_ref str = n.substr(2);

            int semIndex = 0;
            if (!str.empty() && str[0] >= '1' && str[0] <= '9')
            {
                semIndex = str[0] - '1';
                str = str.substr(1);
            }

            data.texcoords.push_back(MemberDecl(m.typeName, str, core::Format("TEXCOORD{0}", semIndex), m.type, semIndex, m.name.get_memory_label()));
        }
        else if (n == "worldRefl")
        {
            data.needsWorldReflection = true;
        }
        else if (n == "worldNormal")
        {
            data.needsWorldNormal = true;
        }
        else if (n == "screenPos")
        {
            data.needsScreenPosition = true;
        }
        else if (n == "viewDir")
        {
            data.needsViewDir = true;
        }
        else if (n == "worldViewDir")
        {
            data.needsWorldViewDir = true;
        }
        else if (n == "worldPos")
        {
            data.needsWorldPos = true;
        }
    }

    const ShaderDesc& descForNormalRead = needsSeparateAnalysisWithoutLighting ? desc3 : desc2;
    for (size_t i = 0; i < descForNormalRead.inputs.size(); ++i)
    {
        const MemberDecl& m = descForNormalRead.inputs[i];
        if (m.name == funcData.desc.outputName + ".Normal" || m.name == "_hlslin_.Normal")
            data.readsNormal = true;
    }

    // Figure out more options
    if (funcData.writesNormal)
    {
        data.useTangentSpace = true;
    }
    data.packedTC.Pack(data.texcoords.size(), data.texcoords.empty() ? NULL : &data.texcoords[0]);
}
