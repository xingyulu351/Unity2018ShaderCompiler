#include "UnityPrefix.h"

#include "SurfaceCompiler.h"
#include "SurfaceData.h"
#include "../ShaderCompiler.h"
#include "../ShaderCompilerPlatformCaps.h"
#include "../Utilities/ProcessIncludes.h"
#include "../Utilities/ShaderImportErrors.h"
#include "Editor/Src/Utility/TextUtilities.h"
#include "Runtime/Jobs/Jobs.h"
#include "Runtime/GfxDevice/GfxDeviceTypes.h"
#include "Runtime/Utilities/Word.h"
#include "Runtime/Utilities/BitUtility.h"
#include <string>
#include <cstring>

using core::string;


static const char* kTexCoordTypeNames[kPrecCount][5] =
{
    {"", "fixed", "fixed2", "fixed3", "fixed4"},
    {"", "half", "half2", "half3", "half4"},
    {"", "float", "float2", "float3", "float4"},
};
static const char* kPackedUVMasks[4][4] =
{
    { "x", "y", "z", "w" },
    { "xy", "yz", "zw", "?" },
    { "xyz", "yzw", "?", "?" },
    { "xyzw", "?", "?", "?" },
};

#define kInstancingOnKeyword        "INSTANCING_ON"
#define kSkipVariantsInstancing     "#pragma skip_variants " kInstancingOnKeyword "\n"

#define kGenerateVariantsFog        "#pragma multi_compile_fog\n"
#define kSkipVariantsFog            "#pragma skip_variants FOG_LINEAR FOG_EXP FOG_EXP2\n"

#define kMetaPassEditorVizKeyword   "EDITOR_VISUALIZATION"

static bool IsUsingCustomVertexData(const SurfaceParams& params, const SurfaceFunctionData& funcData)
{
    if (!params.p.entryName[kProgramVertex].empty() && funcData.desc.vertexModCustomData)
        return true;
    return false;
}

static bool SkipEmittingComboForInstancing(const SurfaceData& data, size_t index)
{
    const core::string strInstancingOn(kInstancingOnKeyword);
    const StringArray& keywords = data.m_Datas[index].keywords;
    return std::find(keywords.begin(), keywords.end(), strInstancingOn) != keywords.end();
}

enum PassFlags
{
    kPassFlagDefault = 0,
    kPassFlagShadowCaster = 1 << 0,
    kPassFlagForwardBase = 1 << 1,
    kPassFlagLighting = 1 << 2,
    kPassFlagLightmaps = 1 << 3,
    kPassFlagNeedsNormal = 1 << 4,
    kPassFlagNeedsViewDir = 1 << 5,
    kPassFlagNeedsWorldViewDir = 1 << 6,
    kPassFlagNeedsWorldPos = 1 << 7,
    kPassFlagNeedsTangentSpace = 1 << 8,
    kPassFlagDeferred = 1 << 9,
    kPassFlagPBS = 1 << 10,
    kPassFlagSeparateWorldPosTexcoord = 1 << 11, // use separate texcoord for world position, instead of packing into tangent-to-world
    kPassFlagPrePassBase = 1 << 12,
    kPassFlagPrePassFinal = 1 << 13,
    kPassFlagMeta = 1 << 14, // meta information pass
    kPassFlagPutFogInTSpace = 1 << 15,      // put fog coordinate into TBN and reconstruct TBN in fragment shader
    kPassFlagPutFogInWorldPos = 1 << 16,    // put fog coordinate into WorldPos.w
};
ENUM_FLAGS(PassFlags);

static int EmitTSpaceV2F(core::string& gen, const int startTexCoord, const bool combinedWorldPos, const bool putFog)
{
    const char* elementType = (combinedWorldPos || putFog) ? "float4" : "float3";
    int ntc = startTexCoord;
    gen += Format("  %s tSpace0 : TEXCOORD%i;\n", elementType, ntc++);
    gen += Format("  %s tSpace1 : TEXCOORD%i;\n", elementType, ntc++);
    gen += Format("  %s tSpace2 : TEXCOORD%i;\n", elementType, ntc++);

    return ntc;
}

static int EmitSurfaceV2F(core::string& gen, const SurfaceParams& params, const SurfaceFunctionData& funcData, const SurfacePassData& data, PassFlags passFlags, int startTexCoord)
{
    const bool needsWorldPos = (passFlags & kPassFlagNeedsWorldPos) != 0;
    const bool separateWorldPos = (passFlags & kPassFlagSeparateWorldPosTexcoord) != 0;
    const bool needsTangentSpace = (passFlags & kPassFlagNeedsTangentSpace) != 0;
    const bool putFogInTBN = (passFlags & kPassFlagPutFogInTSpace) != 0;
    const bool putFogInWorldPos = (passFlags & kPassFlagPutFogInWorldPos) != 0;

    if (needsTangentSpace && putFogInTBN)
        gen += "#define FOG_COMBINED_WITH_TSPACE\n";
    else if (needsWorldPos && putFogInWorldPos)
        gen += "#define FOG_COMBINED_WITH_WORLD_POS\n";

    gen += "struct v2f_" + params.p.entryName[kProgramSurface] + " {\n";
    if (passFlags & kPassFlagShadowCaster)
        gen += "  V2F_SHADOW_CASTER;\n";
    else
        gen += "  UNITY_POSITION(pos);\n";

    int ntc = startTexCoord;
    int n = (int)data.packedTC.packed.size();
    for (int i = 0; i < n; ++i)
    {
        const PackedInterpolators::Packed& pack = data.packedTC.packed[i];
        gen += Format("  %s pack%i : TEXCOORD%i; // %s\n", kTexCoordTypeNames[pack.precision][pack.usedComps], i, ntc++, pack.names.c_str());
    }

    if (needsTangentSpace)
    {
        ntc = EmitTSpaceV2F(gen, ntc, (needsWorldPos && !separateWorldPos), putFogInTBN);
        if (needsWorldPos && separateWorldPos)
            gen += Format("  float3 worldPos : TEXCOORD%i;\n", ntc++);
    }
    else
    {
        if (data.needsWorldReflection)
            gen += Format("  half3 worldRefl : TEXCOORD%i;\n", ntc++);
        if (passFlags & kPassFlagNeedsNormal)
            gen += Format("  float3 worldNormal : TEXCOORD%i;\n", ntc++);
        if (needsWorldPos)
        {
            // We will derive viewDir from the worldPos
            if (putFogInWorldPos)
                gen += Format("  float4 worldPos : TEXCOORD%i;\n", ntc++);
            else
                gen += Format("  float3 worldPos : TEXCOORD%i;\n", ntc++);
        }
    }
    if (data.needsScreenPosition)
        gen += Format("  float4 screenPos : TEXCOORD%i;\n", ntc++);
    if (data.needsVertexColor)
        gen += "  fixed4 color : COLOR0;\n";
    if (IsUsingCustomVertexData(params, funcData))
    {
        // Packed custom input data members
        const PackedInterpolators& packedCustom = funcData.desc.packedCustomInputMembers;
        for (size_t i = 0; i < packedCustom.packed.size(); ++i)
        {
            const PackedInterpolators::Packed& pack = packedCustom.packed[i];
            gen += Format("  %s custompack%i : TEXCOORD%i; // %s\n", kTexCoordTypeNames[pack.precision][pack.usedComps], i, ntc++, pack.names.c_str());
        }
    }
    if (passFlags & kPassFlagNeedsViewDir)
    {
        // World or tangent space viewDir for dir lightmaps and/or surface input
        if (!data.needsViewDir)
            gen += "#ifndef DIRLIGHTMAP_OFF\n";
        gen += Format("  half3 viewDir : TEXCOORD%i;\n", ntc++);
        if (!data.needsViewDir)
            gen += "#endif\n";
    }
    if (passFlags & kPassFlagNeedsWorldViewDir)
    {
        // Interpolated worldViewDir for lighting and/or surface input
        gen += Format("  float3 worldViewDir : TEXCOORD%i;\n", ntc++);
    }
    if (passFlags & kPassFlagMeta)
    {
        gen += "#ifdef " kMetaPassEditorVizKeyword "\n";
        gen += Format("  float2 vizUV : TEXCOORD%i;\n", ntc++);
        gen += Format("  float4 lightCoord : TEXCOORD%i;\n", ntc++);
        gen += "#endif\n";
    }
    return ntc;
}

static bool NeedsWorldViewDir(const SurfacePassData& data, PassFlags passFlags)
{
    // Reflection is calculated from worldViewDir when using tangent space
    if (data.needsWorldReflection && data.useTangentSpace)
        return true;

    // Check if lighting is enabled and needs worldViewDir
    if ((passFlags & kPassFlagLighting) && data.lightNeedsWorldViewDir)
        return true;

    return data.needsWorldViewDir;
}

static bool NeedsViewDir(const SurfacePassData& data, PassFlags passFlags)
{
    // Check if lightmaps are enabled and need viewDir
    if ((passFlags & kPassFlagLightmaps) && data.lightmapNeedsViewDir)
        return true;

    return data.needsViewDir;
}

static bool NeedsInterpolatedWorldViewDir(const SurfaceParams& params, const SurfacePassData& data, PassFlags passFlags)
{
    return params.o.interpolateView && NeedsWorldViewDir(data, passFlags);
}

static bool IsLPPVAvailableForAnyTargetPlatform(const SurfaceParams& params)
{
    // Keep this list in synch with the UNITY_LIGHT_PROBE_PROXY_VOLUME define from UnityShaderVariables.cginc
    int LPPVSupportedMask = 0;
    LPPVSupportedMask |= (1 << kShaderCompPlatformD3D11);
    LPPVSupportedMask |= (1 << kShaderCompPlatformOpenGLCore);
    LPPVSupportedMask |= (1 << kShaderCompPlatformXboxOne);
    LPPVSupportedMask |= (1 << kShaderCompPlatformXboxOneD3D12);
    LPPVSupportedMask |= (1 << kShaderCompPlatformPS4);
    LPPVSupportedMask |= (1 << kShaderCompPlatformVulkan);
    LPPVSupportedMask |= (1 << kShaderCompPlatformMetal);
    LPPVSupportedMask |= (1 << kShaderCompPlatformSwitch);

    if (params.shaderAPIs & LPPVSupportedMask) // at least one target platform support LPPV.
        return true;

    return false;
}

static bool NeedsWorldPos(const SurfaceParams& params, const SurfacePassData& data, PassFlags passFlags)
{
    // Reflection is calculated from worldPos when using tangent space
    if (data.needsWorldReflection && data.useTangentSpace)
        return true;

    if (data.lightNeedsWorldPos)
        return true;

    // View direction must be derived from the position
    if (NeedsViewDir(data, passFlags) || NeedsWorldViewDir(data, passFlags))
        return true;

    return data.needsWorldPos;
}

static bool IsOpaque(const SurfaceParams& params)
{
    // these need actual alpha channel output for blending
    if (params.o.mode == kSurfaceModeAlpha || params.o.mode == kSurfaceModeAlphaTest || params.o.mode == kSurfaceModeDecalBlend)
        return false;
    // explicitly requested to keep alpha
    if (params.o.keepAlpha)
        return false;

    return true;
}

static void EmitWorldPos(core::string& gen, const SurfacePassData& data, PassFlags passFlags)
{
    if ((passFlags & kPassFlagNeedsTangentSpace) && !(passFlags & kPassFlagSeparateWorldPosTexcoord))
        gen += "  float3 worldPos = float3(IN.tSpace0.w, IN.tSpace1.w, IN.tSpace2.w);\n";
    else if (passFlags & kPassFlagNeedsWorldPos)
    {
        gen += "  float3 worldPos = IN.worldPos.xyz;\n";
    }
    else
        gen += "  float3 worldPos = 0.0;\n";
}

static void EmitLightDir(core::string& gen)
{
    gen += "  #ifndef USING_DIRECTIONAL_LIGHT\n"
        "    fixed3 lightDir = normalize(UnityWorldSpaceLightDir(worldPos));\n"
        "  #else\n"
        "    fixed3 lightDir = _WorldSpaceLightPos0.xyz;\n"
        "  #endif\n";
}

static void EmitWorldViewDir(core::string& gen, const SurfaceParams& params, PassFlags passFlags)
{
    if (passFlags & kPassFlagNeedsWorldViewDir)
    {
        gen += "  float3 worldViewDir = normalize(IN.worldViewDir);\n";
    }
    else
    {
        gen += "  float3 worldViewDir = normalize(UnityWorldSpaceViewDir(worldPos));\n";

        if (params.o.halfAsViewDir)
            gen += "  worldViewDir = normalize(worldViewDir + lightDir);\n";
    }
}

static void EmitViewDirDeriveFromWorldPos(core::string& gen, const SurfaceParams& params, const SurfacePassData& data, PassFlags passFlags)
{
    if (passFlags & kPassFlagNeedsTangentSpace)
    {
        // Get tangent-space view direction by transforming worldViewDir by transposed tSpace.
        // It should really be the inverse, but neither way is correct for non-uniform scale.
        // That's because lighting should be calculated in an orthonormal coordinate system.
        // Uniformly scaled orthonormal is OK since it is cancelled out by normalize().
        // We have to take a shortcut since e.g. transforming directional light map basis
        // vectors into world space would be too expensive.
        gen += "  float3 viewDir = _unity_tbn_0 * worldViewDir.x"
            " + _unity_tbn_1 * worldViewDir.y "
            " + _unity_tbn_2 * worldViewDir.z;\n";
    }
    else
        gen += "  fixed3 viewDir = worldViewDir;\n";
}

static void EmitViewDirInterpolateFromVertex(core::string& gen, const SurfaceParams& params, const SurfacePassData& data, PassFlags passFlags)
{
    // Tangent space viewDir for dir lightmaps and/or surface input
    if (!data.needsViewDir)
        gen += "#ifndef DIRLIGHTMAP_OFF\n";
    gen += "  fixed3 viewDir = normalize(IN.viewDir);\n";
    if (!data.needsViewDir)
        gen += "#endif\n";
}

static void EmitViewDir(core::string& gen, const SurfaceParams& params, const SurfacePassData& data, PassFlags passFlags)
{
    if (!NeedsWorldViewDir(data, passFlags) && !NeedsViewDir(data, passFlags))
        return;

    // Derive worldViewDir from worldPos
    if (passFlags & kPassFlagNeedsWorldPos)
        EmitWorldViewDir(gen, params, passFlags);

    if (!NeedsViewDir(data, passFlags))
        return;

    // Either use interpolated viewDir or derive it from worldViewDir.
    if (passFlags & kPassFlagNeedsViewDir)
        EmitViewDirInterpolateFromVertex(gen, params, data, passFlags);
    else if (passFlags & kPassFlagNeedsWorldPos)
        EmitViewDirDeriveFromWorldPos(gen, params, data, passFlags);
}

static void EmitSurfaceNormalToWorldSpace(core::string& gen, const SurfacePassData& data)
{
    if (data.useTangentSpace)
    {
        // transform normal to world space
        gen += "  float3 worldN;\n";
        gen += "  worldN.x = dot(_unity_tbn_0, o.Normal);\n";
        gen += "  worldN.y = dot(_unity_tbn_1, o.Normal);\n";
        gen += "  worldN.z = dot(_unity_tbn_2, o.Normal);\n";
        gen += "  worldN = normalize(worldN);\n";
        gen += "  o.Normal = worldN;\n";
    }
}

static void EmitPSProlog(core::string& gen, const SurfaceParams& params, const SurfaceFunctionData& funcData, const SurfacePassData& data, PassFlags passFlags)
{
    // alpha test variable if needed
    if (params.o.mode == kSurfaceModeAlphaTest)
        gen += "fixed " + params.o.alphaTestVariable + ";\n";

    // start of fragment shader
    gen += "\n// fragment shader\n";
    gen += (passFlags & kPassFlagDeferred) ? "void " : "fixed4 ";
    gen += "frag_" + params.p.entryName[kProgramSurface] + " (v2f_" + params.p.entryName[kProgramSurface] + " IN";
    if (data.needsVFace)
    {
        gen += ", float vface : VFACE";
    }
    if (passFlags & kPassFlagDeferred)
    {
        gen += ",\n";
        gen += "    out half4 outGBuffer0 : SV_Target0,\n";
        gen += "    out half4 outGBuffer1 : SV_Target1,\n";
        gen += "    out half4 outGBuffer2 : SV_Target2,\n";
        gen += "    out half4 outEmission : SV_Target3\n";
        gen += "#if defined(SHADOWS_SHADOWMASK) && (UNITY_ALLOWED_MRT_COUNT > 4)\n";
        gen += "    , out half4 outShadowMask : SV_Target4\n";
        gen += "#endif\n";
        gen += ") {\n";
    }
    else
    {
        gen += ") : SV_Target {\n";
    }
    gen += "  UNITY_SETUP_INSTANCE_ID(IN);\n";
    gen += "  // prepare and unpack data\n";
    gen += "  Input surfIN;\n";
    if (params.o.HasFog())
    {
        gen += "  #ifdef FOG_COMBINED_WITH_TSPACE\n";
        gen += "    UNITY_EXTRACT_FOG_FROM_TSPACE(IN);\n";
        gen += "  #elif defined FOG_COMBINED_WITH_WORLD_POS\n";
        gen += "    UNITY_EXTRACT_FOG_FROM_WORLD_POS(IN);\n";
        gen += "  #else\n";
        gen += "    UNITY_EXTRACT_FOG(IN);\n";
        gen += "  #endif\n";
    }

    if (passFlags & kPassFlagNeedsTangentSpace)
    {
        gen += "  #ifdef FOG_COMBINED_WITH_TSPACE\n";
        gen += "    UNITY_RECONSTRUCT_TBN(IN);\n";
        gen += "  #else\n";
        gen += "    UNITY_EXTRACT_TBN(IN);\n";
        gen += "  #endif\n";
    }

    gen += "  UNITY_INITIALIZE_OUTPUT(Input,surfIN);\n";

    // case 716161 FPE warning during HLSL shader compilation
    // emit safeguard input member initialization code, so
    // that if they are declared but not really used then we don't
    // get compiler warnings about normalizes on zero vectors etc.
    for (size_t i = 0; i < funcData.desc.inputMembers.size(); ++i)
    {
        const MemberDecl& m = funcData.desc.inputMembers[i];
        // case 759336 do not initialize invalid types i.e. struct or array member variables in Input
        if ((m.type != kTypeOther) && (m.type != kTypeVoid) && (m.arraySize == -1))
        {
            gen += "  surfIN." + m.name + ".x = 1.0;\n";
        }
    }

    // unpack texcoords into Input struct
    Assert(data.texcoords.size() == data.packedTC.offsets.size());
    for (size_t i = 0; i < data.texcoords.size(); ++i)
    {
        const MemberDecl& tc = data.texcoords[i];
        core::string semanticIndexStr = tc.semanticIndex > 0 ? IntToString(tc.semanticIndex + 1) : "";
        gen += Format("  surfIN.uv%s%s = IN.pack%i.%s;\n",
            semanticIndexStr.c_str(),
            tc.name.c_str(),
            data.packedTC.offsets[i].index,
            kPackedUVMasks[GetMemberTypeDimension(tc.type) - 1][data.packedTC.offsets[i].offset]);
    }
    // copy any custom vertex data into Input struct
    if (IsUsingCustomVertexData(params, funcData))
    {
        const PackedInterpolators& packed = funcData.desc.packedCustomInputMembers;
        Assert(funcData.desc.customInputMembers.size() == packed.offsets.size());
        for (size_t i = 0; i < funcData.desc.customInputMembers.size(); ++i)
        {
            const MemberDecl& m = funcData.desc.customInputMembers[i];
            gen += Format("  surfIN.%s = IN.custompack%i.%s;\n",
                m.name.c_str(),
                packed.offsets[i].index,
                kPackedUVMasks[GetMemberTypeDimension(m.type) - 1][packed.offsets[i].offset]);
        }
    }

    if (passFlags & kPassFlagNeedsWorldPos)
    {
        // Unpack worldPos if needed
        EmitWorldPos(gen, data, passFlags);

        // Compute light direction on the fly
        EmitLightDir(gen);
    }
    else if (passFlags & kPassFlagForwardBase)
    {
        gen += "  float3 lightDir = _WorldSpaceLightPos0.xyz;\n";
    }

    // Computed or interpolated view direction
    EmitViewDir(gen, params, data, passFlags);

    bool surfNeedsTangentSpace = false;
    const bool useTangentSpace = ((data.useTangentSpace) && (passFlags & kPassFlagNeedsTangentSpace));
    if (data.needsWorldReflection)
    {
        if (useTangentSpace)
        {
            // Per pixel world reflection is calculated from worldViewDir and tangent space
            gen += "  surfIN.worldRefl = -worldViewDir;\n";
            surfNeedsTangentSpace = true;
        }
        else
            gen += "  surfIN.worldRefl = IN.worldRefl;\n";
    }
    if (data.needsWorldNormal)
    {
        if (useTangentSpace)
        {
            gen += "  surfIN.worldNormal = 0.0;\n";
            surfNeedsTangentSpace = true;
        }
        else
            gen += "  surfIN.worldNormal = IN.worldNormal;\n";
    }
    if (surfNeedsTangentSpace)
    {
        gen += "  surfIN.internalSurfaceTtoW0 = _unity_tbn_0;\n";
        gen += "  surfIN.internalSurfaceTtoW1 = _unity_tbn_1;\n";
        gen += "  surfIN.internalSurfaceTtoW2 = _unity_tbn_2;\n";
    }
    if (data.needsScreenPosition)
    {
        gen += "  surfIN.screenPos = IN.screenPos;\n";
    }
    if (data.needsWorldPos)
        gen += "  surfIN.worldPos = worldPos;\n";

    if (data.needsViewDir)
        gen += "  surfIN.viewDir = viewDir;\n";

    if (data.needsWorldViewDir)
        gen += "  surfIN.worldViewDir = worldViewDir;\n";

    if (data.needsVertexColor)
        gen += Format("  surfIN.%s = IN.color;\n", data.vertexColorField.c_str());

    if (data.needsVFace)
    {
        gen += "  #if UNITY_VFACE_FLIPPED\n";
        gen += "     vface = -vface;\n";
        gen += "  #endif\n";
        gen += Format("  surfIN.%s = vface;\n", data.vfaceField.c_str());
    }

    // surface output; fill in defaults

    // MS' HLSL compiler really doesn't like uninitialized members. But we need some of them
    // to be uninitialized otherwise, to detect if shader actually writes to them!
    gen += "  #ifdef UNITY_COMPILER_HLSL\n";
    gen += "  " + funcData.desc.outputType + " o = (" + funcData.desc.outputType + ")0;\n";
    gen += "  #else\n";
    gen += "  " + funcData.desc.outputType + " o;\n";
    gen += "  #endif\n";
    gen += "  o.Albedo = 0.0;\n";
    gen += "  o.Emission = 0.0;\n";
    if (funcData.desc.FindOutputMember("Specular") != NULL)  // Specular can be optional
        gen += "  o.Specular = 0.0;\n";
    gen += "  o.Alpha = 0.0;\n";
    if (funcData.desc.outputType == "SurfaceOutput")
        gen += "  o.Gloss = 0.0;\n"; // Gloss is present in default struct but otherwise optional
    // default occlusion is 1.0
    const MemberDecl* occlusionOutput = funcData.desc.FindOutputMember("Occlusion");
    if (occlusionOutput != NULL)
    {
        gen += "  o.Occlusion = 1.0;\n";
    }

    gen += "  fixed3 normalWorldVertex = fixed3(0,0,1);\n";

    // pass / fill in default normal
    if (passFlags & kPassFlagNeedsNormal)
    {
        if (!(passFlags & kPassFlagNeedsTangentSpace))
        {
            gen += "  o.Normal = IN.worldNormal;\n";
            gen += "  normalWorldVertex = IN.worldNormal;\n";
        }
        else
        {
            gen += "  o.Normal = fixed3(0,0,1);\n";
        }
    }

    gen += "\n  // call surface function\n";
    gen += "  " + params.p.entryName[kProgramSurface] + " (surfIN, o);\n";

    if (params.o.AddDitherCrossFade())
    {
        // Apply dithering after calling the surface shader so that we won't mess up with UV gradient.
        gen += "  UNITY_APPLY_DITHER_CROSSFADE(IN.pos.xy);\n";
    }

    // do alpha test if needed
    if (params.o.mode == kSurfaceModeAlphaTest)
    {
        gen += "\n  // alpha test\n";
        gen += "  clip (o.Alpha - " + params.o.alphaTestVariable + ");\n";
    }
}

static void EmitPSEpilog(core::string& gen, const SurfaceParams& params)
{
    if (!params.o.finalColorModifier.empty())
    {
        gen += "  " + params.o.finalColorModifier + " (surfIN, o, c);\n";
    }
    if (params.o.HasFog())
    {
        if (params.o.mode == kSurfaceModeDecalAdd)
            gen += "  UNITY_APPLY_FOG_COLOR(_unity_fogCoord, c, fixed4(0,0,0,0)); // fog towards black for decal:add\n";
        else
            gen += "  UNITY_APPLY_FOG(_unity_fogCoord, c); // apply fog\n";
    }
    if (IsOpaque(params))
    {
        gen += "  UNITY_OPAQUE_ALPHA(c.a);\n";
    }
    gen += "  return c;\n";
    gen += "}\n";
}

static void EmitPassBlending(core::string& result, const SurfaceParams& params, bool additivePixel, bool autoMeansPremultiply)
{
    if (additivePixel)
        result += "\t\tZWrite Off Blend One One\n";

    switch (params.o.mode)
    {
        case kSurfaceModeDefault:
            break; // nothing extra
        case kSurfaceModeAlpha:
            result += "\t\tBlend";
            if (params.o.blend == kSurfaceBlendPremultiply || (autoMeansPremultiply && params.o.blend == kSurfaceBlendAuto))
                result += " One";
            else
                result += " SrcAlpha";
            if (additivePixel)
                result += " One\n";
            else
                result += " OneMinusSrcAlpha\n";
            break;
        case kSurfaceModeAlphaTest: // no fixed function AlphaTest; it's done in the shader
            if (!params.o.noColorMask)
                result += "\t\tColorMask RGB\n";
            break;
        case kSurfaceModeDecalAdd:
            result += "\t\tBlend One One ZWrite Off\n";
            break;
        case kSurfaceModeDecalBlend:
            result += "\t\t";
            if (!params.o.noColorMask)
                result += "ColorMask RGB ";
            if (additivePixel)
                result += "ZWrite Off Blend SrcAlpha One\n";
            else
                result += "ZWrite Off Blend SrcAlpha OneMinusSrcAlpha\n";
            break;
        default:
            AssertString("unknown surface mode");
    }
}

static void OutputBoolParam(core::string& result, const char* desc, bool val)
{
    result += Format("// %s: %s\n", desc, val ? "YES" : "no");
}

static void OutputGeneratedCodeDebugInfo(core::string& result, SurfaceParams& params, const SurfaceFunctionData& funcData, const SurfacePassData& data)
{
    result.reserve(200);
    result += "// Surface shader code generated based on:\n";
    if (!params.p.entryName[kProgramVertex].empty())
        result += "// vertex modifier: '" + params.p.entryName[kProgramVertex] + "'\n";

    OutputBoolParam(result, "writes to per-pixel normal", funcData.writesNormal);
    OutputBoolParam(result, "writes to emission", funcData.writesEmission);
    OutputBoolParam(result, "writes to occlusion", funcData.writesOcclusion);
    OutputBoolParam(result, "needs world space reflection vector", data.needsWorldReflection);
    OutputBoolParam(result, "needs world space normal vector", data.needsWorldNormal);
    OutputBoolParam(result, "needs screen space position", data.needsScreenPosition);
    OutputBoolParam(result, "needs world space position", data.needsWorldPos);
    OutputBoolParam(result, "needs view direction", data.needsViewDir);
    OutputBoolParam(result, "needs world space view direction", data.needsWorldViewDir);
    OutputBoolParam(result, "needs world space position for lighting", data.lightNeedsWorldPos);
    OutputBoolParam(result, "needs world space view direction for lighting", data.lightNeedsWorldViewDir);
    OutputBoolParam(result, "needs world space view direction for lightmaps", data.lightmapNeedsViewDir);
    OutputBoolParam(result, "needs vertex color", data.needsVertexColor);
    OutputBoolParam(result, "needs VFACE", data.needsVFace);
    OutputBoolParam(result, "passes tangent-to-world matrix to pixel shader", data.useTangentSpace);
    OutputBoolParam(result, "reads from normal", data.readsNormal);

    result += Format("// %i texcoords actually used\n", (int)data.texcoords.size());
    for (size_t i = 0; i < data.texcoords.size(); ++i)
    {
        const MemberDecl& tc = data.texcoords[i];
        result += Format("//   %s %s\n", tc.typeName.c_str(), tc.name.c_str());
    }
}

core::string AddNoVariantsDirectivesToLastMulticompile(const core::string& input, const SurfaceParams& params)
{
    core::string s = input;
    // find last built-in multi_compile statement, add exclusion options like "nolightmap" to end of it, if needed
    // built-in multi_compiles are in form of "multi_compile_*" (note last underscore)
    for (size_t pos = s.rfind("multi_compile_"); pos != core::string::npos; pos = pos == 0 ? core::string::npos : s.rfind("multi_compile_", pos - 1))
    {
        // skip to end of line
        core::string directive = ExtractRestOfLine(s, pos);

        for (int i = 0; i < kNumBuiltinCompileVariants; ++i)
        {
            if (directive == kBuiltinCompileVariants[i])
            {
                // make sure the directive is actually one of the builtin multi_compiles (because e.g. multi_compile_vertex)
                pos += directive.size();
                // insert directives
                if (params.o.noShadow)
                    s.insert(pos, " noshadow");
                if (params.o.noLightmap)
                    s.insert(pos, " nolightmap");
                if (params.o.noDirLightmap)
                    s.insert(pos, " nodirlightmap");
                if (params.o.noDynLightmap)
                    s.insert(pos, " nodynlightmap");
                if (params.o.noShadowMask)
                    s.insert(pos, " noshadowmask");
                if (params.o.noVertexLights)
                    s.insert(pos, " novertexlight");
                return s;
            }
        }
    }
    return s;
}

void OutputGeneratedSnippet(const core::string& variantsStringIn, core::string& result, const core::string& gen, const SurfaceParams& params, unsigned shaderAPIs, ShaderImportErrors& outErrors)
{
    core::string variantsString = AddNoVariantsDirectivesToLastMulticompile(params.o.pragmaDirectives + variantsStringIn, params);

    result += "\nCGPROGRAM\n";
    result += "// compile directives\n";
    if (params.o.HasTessellation())
    {
        result += "#pragma vertex tessvert_" + params.p.entryName[kProgramSurface] + "\n";
        result += "#pragma fragment frag_" + params.p.entryName[kProgramSurface] + "\n";
        result += "#pragma hull hs_" + params.p.entryName[kProgramSurface] + "\n";
        result += "#pragma domain ds_" + params.p.entryName[kProgramSurface] + "\n";
    }
    else
    {
        result += "#pragma vertex vert_" + params.p.entryName[kProgramSurface] + "\n";
        result += "#pragma fragment frag_" + params.p.entryName[kProgramSurface] + "\n";
    }
    result += variantsString + "\n";
    UInt32 moreExcludes = params.shaderAPIs & ~shaderAPIs;
    if (moreExcludes != 0)
    {
        // add extra exclude_renderers
        result += "#pragma exclude_renderers";
        while (moreExcludes)
        {
            result += ' ';
            int rendererIdx = LowestBit(moreExcludes);
            result += GetShaderPlatformName((ShaderCompilerPlatform)rendererIdx);
            moreExcludes &= ~(1 << rendererIdx);
        }
        result += "\n";
    }
    core::string src = MakeShaderSourcePrefix(params.p) + gen;

    // comment out all pragmas in original code; we already parsed
    // them and emitted everything that was needed above
    replace_string(src, "#pragma argument", "#pragma_argument");        // change all the PSSL passthrough pragmas so we don't comment them out
    replace_string(src, "#pragma ", "//#pragma ");
    replace_string(src, "#pragma\t", "//#pragma\t");
    replace_string(src, "#pragma_argument", "#pragma argument");        // restore all the PSSL passthrough pragmas

    result += src;
    result += "\nENDCG\n";
    result += "\n}\n";
}

static void EmitVSProlog(core::string& gen, const SurfaceParams& params, const SurfaceFunctionData& funcData, const SurfacePassData& data, PassFlags passFlags)
{
    // texcoord scale/offset properties
    for (size_t i = 0; i < data.texcoords.size(); ++i)
        gen += Format("float4 %s_ST;\n", data.texcoords[i].name.c_str());

    // start of vertex shader
    gen += "\n// vertex shader\n";
    gen += "v2f_" + params.p.entryName[kProgramSurface] + " vert_" + params.p.entryName[kProgramSurface] + " (" + funcData.desc.appdataType + " v) {\n";
    gen += "  UNITY_SETUP_INSTANCE_ID(v);\n";
    gen += "  v2f_" + params.p.entryName[kProgramSurface] + " o;\n";
    gen += "  UNITY_INITIALIZE_OUTPUT(v2f_" + params.p.entryName[kProgramSurface] + ",o);\n";
    gen += "  UNITY_TRANSFER_INSTANCE_ID(v,o);\n";
    gen += "  UNITY_INITIALIZE_VERTEX_OUTPUT_STEREO(o);\n";
    if (!params.o.HasTessellation() && !params.p.entryName[kProgramVertex].empty())
    {
        if (IsUsingCustomVertexData(params, funcData))
        {
            gen += "  " + funcData.desc.inputType + " customInputData;\n";
            gen += "  " + params.p.entryName[kProgramVertex] + " (v, customInputData);\n";

            const PackedInterpolators& packed = funcData.desc.packedCustomInputMembers;
            Assert(funcData.desc.customInputMembers.size() == packed.offsets.size());
            for (size_t i = 0; i < funcData.desc.customInputMembers.size(); ++i)
            {
                const MemberDecl& m = funcData.desc.customInputMembers[i];
                gen += Format("  o.custompack%i.%s = customInputData.%s;\n",
                    packed.offsets[i].index,
                    kPackedUVMasks[GetMemberTypeDimension(m.type) - 1][packed.offsets[i].offset],
                    m.name.c_str());
            }
        }
        else
        {
            gen += "  " + params.p.entryName[kProgramVertex] + " (v);\n";
        }
    }

    // position
    if (passFlags & kPassFlagMeta)
    {
        gen += "  o.pos = UnityMetaVertexPosition(v.vertex, v.texcoord1.xy, v.texcoord2.xy, unity_LightmapST, unity_DynamicLightmapST);\n"
            "#ifdef " kMetaPassEditorVizKeyword "\n"
            "  o.vizUV = 0;\n"
            "  o.lightCoord = 0;\n"
            "  if (unity_VisualizationMode == EDITORVIZ_TEXTURE)\n"
            "    o.vizUV = UnityMetaVizUV(unity_EditorViz_UVIndex, v.texcoord.xy, v.texcoord1.xy, v.texcoord2.xy, unity_EditorViz_Texture_ST);\n"
            "  else if (unity_VisualizationMode == EDITORVIZ_SHOWLIGHTMASK)\n"
            "  {\n"
            "    o.vizUV = v.texcoord1.xy * unity_LightmapST.xy + unity_LightmapST.zw;\n"
            "    o.lightCoord = mul(unity_EditorViz_WorldToLight, mul(unity_ObjectToWorld, float4(v.vertex.xyz, 1)));\n"
            "  }\n"
            "#endif\n";
    }
    else if (!(passFlags & kPassFlagShadowCaster))
    {
        gen += "  o.pos = UnityObjectToClipPos(v.vertex);\n";
    }

    // transform & pass texcoords
    Assert(data.texcoords.size() == data.packedTC.offsets.size());
    for (size_t i = 0; i < data.texcoords.size(); ++i)
    {
        int semanticIndex = data.texcoords[i].semanticIndex;
        core::string semanticIndexStr = semanticIndex > 0 ? IntToString(semanticIndex) : "";
        gen += Format("  o.pack%i.%s = TRANSFORM_TEX(v.texcoord%s, %s);\n",
            data.packedTC.offsets[i].index,
            kPackedUVMasks[GetMemberTypeDimension(data.texcoords[i].type) - 1][data.packedTC.offsets[i].offset],
            semanticIndexStr.c_str(),
            data.texcoords[i].name.c_str());
    }

    gen += "  float3 worldPos = mul(unity_ObjectToWorld, v.vertex).xyz;\n";
    gen += "  float3 worldNormal = UnityObjectToWorldNormal(v.normal);\n";

    const bool needsViewDir = (passFlags & kPassFlagNeedsViewDir) != 0;
    const bool needsWorldViewDir = (passFlags & kPassFlagNeedsWorldViewDir) != 0;
    const bool needsWorldPos = (passFlags & kPassFlagNeedsWorldPos) != 0;
    const bool needsTangentSpace = (passFlags & kPassFlagNeedsTangentSpace) != 0;
    const bool worldPosSeparate = (passFlags & kPassFlagSeparateWorldPosTexcoord) != 0;
    const bool needsTangentViewDir = needsViewDir && data.useTangentSpace;
    const bool needsTangentSpaceOrViewDir = needsTangentSpace || needsTangentViewDir;

    // These two passes need to handle lightmaps for legacy surface shaders - pre-5.0 surface shaders with custom lighting functions.
    // For that, they need to convert from tangent to world space. If the rest of the shader doesn't need that conversion (i.e. !needsTangentSpace),
    // we can surround that code with #ifdef DIRLIGHTMAP_COMBINED.
    const bool forwardBaseOrPrepassFinal = (passFlags & kPassFlagForwardBase) != 0 || (passFlags & kPassFlagPrePassFinal) != 0;

    if (needsTangentSpaceOrViewDir || forwardBaseOrPrepassFinal)
    {
        if (!needsTangentSpaceOrViewDir)
            gen += Format("  #if defined(LIGHTMAP_ON) && defined(DIRLIGHTMAP_COMBINED)\n");
        gen += "  fixed3 worldTangent = UnityObjectToWorldDir(v.tangent.xyz);\n"
            "  fixed tangentSign = v.tangent.w * unity_WorldTransformParams.w;\n"
            "  fixed3 worldBinormal = cross(worldNormal, worldTangent) * tangentSign;\n";
        if (!needsTangentSpaceOrViewDir)
            gen += Format("  #endif\n");
    }

    if (needsTangentSpace || forwardBaseOrPrepassFinal)
    {
        if (!needsTangentSpace)
            gen += Format("  #if defined(LIGHTMAP_ON) && defined(DIRLIGHTMAP_COMBINED) && !defined(UNITY_HALF_PRECISION_FRAGMENT_SHADER_REGISTERS)\n");
        if (needsWorldPos && !worldPosSeparate)
            gen += "  o.tSpace0 = float4(worldTangent.x, worldBinormal.x, worldNormal.x, worldPos.x);\n"
                "  o.tSpace1 = float4(worldTangent.y, worldBinormal.y, worldNormal.y, worldPos.y);\n"
                "  o.tSpace2 = float4(worldTangent.z, worldBinormal.z, worldNormal.z, worldPos.z);\n";
        else
            gen += "  o.tSpace0 = float3(worldTangent.x, worldBinormal.x, worldNormal.x);\n"
                "  o.tSpace1 = float3(worldTangent.y, worldBinormal.y, worldNormal.y);\n"
                "  o.tSpace2 = float3(worldTangent.z, worldBinormal.z, worldNormal.z);\n";

        if (!needsTangentSpace)
            gen += Format("  #endif\n");
    }

    if (needsTangentSpace)
    {
        if (needsWorldPos && worldPosSeparate)
            gen += "  o.worldPos.xyz = worldPos;\n";
    }
    else
    {
        // Output worldPos separately
        if (needsWorldPos)
            gen += "  o.worldPos.xyz = worldPos;\n";

        // World normal is implicit in the tangent space.
        // We can also output just the normal if tangents aren't needed.
        if (passFlags & kPassFlagNeedsNormal)
            gen += "  o.worldNormal = worldNormal;\n";

        // Using tangent space world reflection can be computed per pixel.
        // Otherwise just compute it per vertex and interpolate.
        if (data.needsWorldReflection)
        {
            gen += "  float3 worldViewDir = UnityWorldSpaceViewDir(worldPos);\n";
            gen += "  o.worldRefl = reflect(-worldViewDir, worldNormal);\n";
        }
    }

    // Interpolated view direction
    if (needsViewDir || needsWorldViewDir)
    {
        // Should not normalize here if we want interpolation to be correct.
        // Unnormalized the world to eye vector can be correctly linearly interpolated.
        gen += "  float3 viewDirForLight = UnityWorldSpaceViewDir(worldPos);\n";
        if (params.o.halfAsViewDir)
        {
            gen += "  float3 worldLightDir = UnityWorldSpaceLightDir(worldPos);\n";
            gen += "  viewDirForLight = normalize(normalize(viewDirForLight) + normalize(worldLightDir));\n";
        }
        // Output viewDir for dir lightmaps and/or surface input
        if (needsViewDir)
        {
            if (!data.needsViewDir)
                gen += "  #ifndef DIRLIGHTMAP_OFF\n";
            // Transform into tangent space if needed
            if (data.useTangentSpace)
            {
                gen += "  o.viewDir.x = dot(viewDirForLight, worldTangent);\n";
                gen += "  o.viewDir.y = dot(viewDirForLight, worldBinormal);\n";
                gen += "  o.viewDir.z = dot(viewDirForLight, worldNormal);\n";
            }
            else
                gen += "  o.viewDir = viewDirForLight;\n";
            if (!data.needsViewDir)
                gen += "  #endif\n";
        }
        // Output worldViewDir per vertex for lighting
        if (needsWorldViewDir)
            gen += "  o.worldViewDir = viewDirForLight;\n";
    }

    // screen space position
    if (data.needsScreenPosition)
    {
        gen += "  o.screenPos = ComputeScreenPos (o.pos);\n";
    }

    // pass color
    if (data.needsVertexColor)
        gen += "  o.color = v.color;\n";
}

static void EmitPassProlog(core::string& result, const SurfaceParams& params, const core::string& name, const core::string& lightMode)
{
    result += "\tPass {\n";
    result += "\t\tName \"" + name + "\"\n";
    result += "\t\tTags { ";
    result += "\"LightMode\" = \"" + lightMode + "\" ";
    if (params.o.softVegetation)
        result += "\"RequireOptions\" = \"SoftVegetation\" ";
    result += "}\n";
}

static void EmitPSLightingFunc(core::string& gen, const SurfaceParams& params, const SurfacePassData& data, core::string resultColorName, core::string attenuationName)
{
    core::string viewDir = "";
    if (data.lightNeedsWorldViewDir)
        viewDir = ", worldViewDir";

    gen += "  " + resultColorName + " += Lighting" + params.o.lighting + " (o, lightDir" + viewDir + ", " + attenuationName + ");\n";
}

static void EmitPSLightingGIFunc(core::string& gen, const SurfaceParams& params, const SurfacePassData& data, const core::string& resultColorName)
{
    core::string viewDir = "";
    if (data.lightNeedsWorldViewDir)
        viewDir = ", worldViewDir";

    gen += "  " + resultColorName + " += Lighting" + params.o.lighting + " (o" + viewDir + ", gi);\n";
}

static bool IsUsingGILightingFunction(const SurfaceParams& params, const SurfaceFunctionData& funcData)
{
    // do we have GI lighting function?
    LightingModels::const_iterator lmit = funcData.lightModels.find(params.o.lighting);
    if (lmit == funcData.lightModels.end())
        return false;
    const bool gi = lmit->second.hasGI;
    return gi;
}

static bool IsUsingGILightingFunction(const SurfaceParams& params, const SurfaceData& data)
{
    // All variants do use the same lighting function, enough to check just the 1st variant
    return IsUsingGILightingFunction(params, data.m_Datas[0].func);
}

static bool IsUsingPBSLightingFunction(const SurfaceParams& params, const SurfaceFunctionData& funcData)
{
    // do we have PBS lighting function?
    LightingModels::const_iterator lmit = funcData.lightModels.find(params.o.lighting);
    if (lmit == funcData.lightModels.end())
        return false;
    const bool pbs = lmit->second.isPBS;
    return pbs;
}

static bool IsUsingPBSLightingFunction(const SurfaceParams& params, const SurfaceData& data)
{
    // All variants do use the same lighting function, enough to check just the 1st variant
    return IsUsingPBSLightingFunction(params, data.m_Datas[0].func);
}

static void EmitGISetup(core::string& gen, const SurfaceParams& params, SurfacePass passType, const bool lightNeedsWorldViewDir)
{
    const bool passDoesGI = (passType == kSurfFwdBase || passType == kSurfDeferred);

    gen += "\n";
    gen += "  // Setup lighting environment\n";
    gen += "  UnityGI gi;\n";
    gen += "  UNITY_INITIALIZE_OUTPUT(UnityGI, gi);\n";
    gen += "  gi.indirect.diffuse = 0;\n";
    gen += "  gi.indirect.specular = 0;\n";

    if (passType == kSurfDeferred)
    {
        gen += "  gi.light.color = 0;\n";
        gen += "  gi.light.dir = half3(0,1,0);\n";
    }
    else
    {
        gen += "  gi.light.color = _LightColor0.rgb;\n";
        gen += "  gi.light.dir = lightDir;\n";
    }

    if (passDoesGI)
    {
        gen += "  // Call GI (lightmaps/SH/reflections) lighting function\n";
        gen += "  UnityGIInput giInput;\n";
        gen += "  UNITY_INITIALIZE_OUTPUT(UnityGIInput, giInput);\n";
        gen += "  giInput.light = gi.light;\n";
        gen += "  giInput.worldPos = worldPos;\n";
        if (lightNeedsWorldViewDir)
            gen += "  giInput.worldViewDir = worldViewDir;\n";
        gen += "  giInput.atten = atten;\n";
        gen += "  #if defined(LIGHTMAP_ON) || defined(DYNAMICLIGHTMAP_ON)\n";
        gen += "    giInput.lightmapUV = IN.lmap;\n";
        gen += "  #else\n";
        gen += "    giInput.lightmapUV = 0.0;\n";
        gen += "  #endif\n";
        gen += "  #if UNITY_SHOULD_SAMPLE_SH && !UNITY_SAMPLE_FULL_SH_PER_PIXEL\n";
        gen += "    giInput.ambient = IN.sh;\n";
        gen += "  #else\n";
        gen += "    giInput.ambient.rgb = 0.0;\n";
        gen += "  #endif\n";

        gen += "  giInput.probeHDR[0] = unity_SpecCube0_HDR;\n";
        gen += "  giInput.probeHDR[1] = unity_SpecCube1_HDR;\n";
        gen += "  #if defined(UNITY_SPECCUBE_BLENDING) || defined(UNITY_SPECCUBE_BOX_PROJECTION)\n";
        gen += "    giInput.boxMin[0] = unity_SpecCube0_BoxMin; // .w holds lerp value for blending\n";
        gen += "  #endif\n";
        gen += "  #ifdef UNITY_SPECCUBE_BOX_PROJECTION\n";
        gen += "    giInput.boxMax[0] = unity_SpecCube0_BoxMax;\n";
        gen += "    giInput.probePosition[0] = unity_SpecCube0_ProbePosition;\n";
        gen += "    giInput.boxMax[1] = unity_SpecCube1_BoxMax;\n";
        gen += "    giInput.boxMin[1] = unity_SpecCube1_BoxMin;\n";
        gen += "    giInput.probePosition[1] = unity_SpecCube1_ProbePosition;\n";
        gen += "  #endif\n";

        core::string lightFunc = "Lighting" + params.o.lighting + "_GI";
        gen += "  " + lightFunc + "(o, giInput, gi);\n";

        if (params.o.noAmbient)
        {
            // Sampling of SH in the _GI() function should get compiled out.
            // UNITY_SHOULD_SAMPLE_SH is true only when lightmaps are off, so this will not affect them.
            gen += "  \n";
            gen += "  #if UNITY_SHOULD_SAMPLE_SH && !defined(LIGHTMAP_ON)\n";
            gen += "    gi.indirect.diffuse = 0;\n";
            gen += "  #endif\n";
        }
    }
}

static core::string EmitPSSingleLightmap(const SurfaceParams& params, const SurfaceFunctionData& funcData, const SurfacePassData& data, core::string resultColorName, bool forward)
{
    core::string result;

    core::string lmType = "fixed3";
    core::string lmSwizzle = ".rgb";

    result += "      // single lightmap\n";
    result += "      fixed4 lmtex = UNITY_SAMPLE_TEX2D(unity_Lightmap, IN.lmap.xy);\n";

    LightingModels::const_iterator lmit = funcData.lightModels.find(params.o.lighting);
    if (lmit != funcData.lightModels.end() && lmit->second.hasCustomSingleLightmap)
    {
        core::string viewDir = "";
        if (lmit->second.useViewDirForSingleLightmap)
            viewDir = ", viewDir";

        core::string lightmapFunc = "Lighting" + params.o.lighting + "_SingleLightmap";
        result += "      " + lmType + " lm = " + lightmapFunc + "(o, lmtex" + viewDir + ")" + lmSwizzle + ";\n";
    }
    else
    {
        result += "      " + lmType + " lm = DecodeLightmap (lmtex);\n";
    }

    if (!forward)
        result += "      " + resultColorName + ".rgb += lm;\n";

    return result;
}

static core::string EmitPSDualLightmaps(const SurfaceParams& params, const SurfaceFunctionData& funcData, const SurfacePassData& data, core::string resultColorName, bool forward)
{
    core::string result;

    core::string lmType = "half3";
    core::string lmSwizzle = ".rgb";

    result += "      // dual lightmaps\n";
    result += "      fixed4 lmtex = UNITY_SAMPLE_TEX2D(unity_Lightmap, IN.lmap.xy);\n";
    result += "      fixed4 lmtex2 = UNITY_SAMPLE_TEX2D_SAMPLER(unity_LightmapInd, unity_Lightmap, IN.lmap.xy);\n";
    result += "      half lmFade = length (IN.lmapFadePos) * unity_LightmapFade.z + unity_LightmapFade.w;\n";

    LightingModels::const_iterator lmit = funcData.lightModels.find(params.o.lighting);
    if (lmit != funcData.lightModels.end() && lmit->second.hasCustomDualLightmaps)
    {
        core::string viewDir = "";
        if (lmit->second.useViewDirForDualLightmap)
            viewDir = ", viewDir";

        core::string lightmapFunc = "Lighting" + params.o.lighting + "_DualLightmap";
        result += "      " + lmType + " lm = " + lightmapFunc + "(o, lmtex, lmtex2, saturate(lmFade)" + viewDir + ")" + lmSwizzle + ";\n";
    }
    else
    {
        result += "      half3 lmFull = DecodeLightmap (lmtex);\n";
        result += "      half3 lmIndirect = DecodeLightmap (lmtex2);\n";
        result += "      " + lmType + " lm = lerp (lmIndirect, lmFull, saturate(lmFade));\n";
    }

    if (!forward)
        result += "      " + resultColorName + ".rgb += lm;\n";

    return result;
}

static core::string EmitPSDirLightmaps(const SurfaceParams& params, const SurfaceFunctionData& funcData, const SurfacePassData& data, core::string resultColorName, RenderPath renderPath)
{
    core::string result;

    result += "      // directional lightmaps\n";
    result += "      fixed4 lmtex = UNITY_SAMPLE_TEX2D(unity_Lightmap, IN.lmap.xy);\n";

    core::string dirlmfunc;

    if (funcData.writesNormal)
    {
        result += "      fixed4 lmIndTex = UNITY_SAMPLE_TEX2D_SAMPLER(unity_LightmapInd, unity_Lightmap, IN.lmap.xy);\n";

        dirlmfunc = "DecodeDirectionalLightmap (DecodeLightmap(lmtex), lmIndTex, o.Normal)";
    }
    else
    {
        // Surface function doesn't write to o.Normal. Providing a sensible world normal is not for
        // free and would result in exactly the same output as non-directional lightmaps. Instead
        // just use the non-directional lightmaps decoding and don't sample the second texture.
        dirlmfunc = "DecodeLightmap(lmtex)";
    }

    if (renderPath == kPathForward)
        result += "      half3 lm = " + dirlmfunc + ";\n";
    else
        result += "      half4 lm = half4(" + dirlmfunc + ", 0);\n";

    if (renderPath != kPathForward)
        result += "      " + resultColorName + " += lm;\n";

    return result;
}

static void EmitTessellationHull(core::string& gen, const SurfaceParams& params, const SurfaceFunctionData& funcData)
{
    if (!params.o.HasTessellation())
        return; // no tessellation, nothing to do

    const core::string tessInterpType = "InternalTessInterp_" + funcData.desc.appdataType;
    gen += "\n#ifdef UNITY_CAN_COMPILE_TESSELLATION\n";
    gen += "\n// tessellation vertex shader\n";
    gen += "struct " + tessInterpType + " {\n";
    for (size_t i = 0; i < funcData.desc.appdataMembers.size(); ++i)
    {
        const MemberDecl& m = funcData.desc.appdataMembers[i];
        gen += Format("  %s %s : %s;\n",
            m.typeName.c_str(),
            m.name.c_str(),
            (m.semantic == "POSITION" || m.semantic == "SV_POSITION") ? "INTERNALTESSPOS" : m.semantic.c_str());
    }
    gen += "};\n";
    gen += tessInterpType + " tessvert_" + params.p.entryName[kProgramSurface] + " (" + funcData.desc.appdataType + " v) {\n";
    gen += "  " + tessInterpType + " o;\n";
    for (size_t i = 0; i < funcData.desc.appdataMembers.size(); ++i)
    {
        const MemberDecl& m = funcData.desc.appdataMembers[i];
        gen += Format("  o.%s = v.%s;\n", m.name.c_str(), m.name.c_str());
    }
    gen += "  return o;\n";
    gen += "}\n";

    gen += "\n// tessellation hull constant shader\n";
    gen += "UnityTessellationFactors hsconst_" + params.p.entryName[kProgramSurface] + " (InputPatch<" + tessInterpType + ",3> v) {\n";
    gen += "  UnityTessellationFactors o;\n";
    gen += "  float4 tf;\n";
    if (!funcData.desc.tessModel.useAppdataInput)
    {
        // simple tessellation, no inputs
        gen += "  tf = " + params.o.tessModifier + "();\n";
    }
    else
    {
        // takes input patch as input
        gen += "  " + funcData.desc.appdataType + " vi[3];\n";
        for (int pt = 0; pt < 3; ++pt)
        {
            for (size_t i = 0; i < funcData.desc.appdataMembers.size(); ++i)
            {
                const MemberDecl& m = funcData.desc.appdataMembers[i];
                gen += Format("  vi[%i].%s = v[%i].%s;\n", pt, m.name.c_str(), pt, m.name.c_str());
            }
        }
        gen += "  tf = " + params.o.tessModifier + "(vi[0], vi[1], vi[2]);\n";
    }
    gen += "  o.edge[0] = tf.x; o.edge[1] = tf.y; o.edge[2] = tf.z; o.inside = tf.w;\n";
    gen += "  return o;\n";
    gen += "}\n";

    gen += "\n// tessellation hull shader\n";
    gen += "[UNITY_domain(\"tri\")]\n";
    gen += "[UNITY_partitioning(\"fractional_odd\")]\n";
    gen += "[UNITY_outputtopology(\"triangle_cw\")]\n";
    gen += "[UNITY_patchconstantfunc(\"hsconst_" + params.p.entryName[kProgramSurface] + "\")]\n";
    gen += "[UNITY_outputcontrolpoints(3)]\n";
    gen += "" + tessInterpType + " hs_" + params.p.entryName[kProgramSurface] + " (InputPatch<" + tessInterpType + ",3> v, uint id : SV_OutputControlPointID) {\n";
    gen += "  return v[id];\n";
    gen += "}\n";
    gen += "\n#endif // UNITY_CAN_COMPILE_TESSELLATION\n\n";
}

static void EmitTessellationDomain(core::string& gen, const SurfaceParams& params, const SurfaceFunctionData& funcData)
{
    if (!params.o.HasTessellation())
        return; // no tessellation, nothing to do

    const core::string tessInterpType = "InternalTessInterp_" + funcData.desc.appdataType;
    gen += "\n#ifdef UNITY_CAN_COMPILE_TESSELLATION\n";
    gen += "\n// tessellation domain shader\n";
    gen += "[UNITY_domain(\"tri\")]\n";
    gen += "v2f_" + params.p.entryName[kProgramSurface] + " ds_" + params.p.entryName[kProgramSurface] + " (UnityTessellationFactors tessFactors, const OutputPatch<" + tessInterpType + ",3> vi, float3 bary : SV_DomainLocation) {\n";
    gen += "  " + funcData.desc.appdataType + " v;UNITY_INITIALIZE_OUTPUT(" + funcData.desc.appdataType + ",v);\n";
    for (size_t i = 0; i < funcData.desc.appdataMembers.size(); ++i)
    {
        const MemberDecl& m = funcData.desc.appdataMembers[i];
        gen += Format("  v.%s = vi[0].%s*bary.x + vi[1].%s*bary.y + vi[2].%s*bary.z;\n",
            m.name.c_str(), m.name.c_str(), m.name.c_str(), m.name.c_str());

        // phong tessellation for position, if needed
        if (!params.o.tessPhongAlpha.empty() && (m.semantic == "POSITION" || m.semantic == "SV_POSITION"))
        {
            gen += "  float3 pp[3];\n";
            gen += "  for (int i = 0; i < 3; ++i)\n";
            gen += Format("    pp[i] = v.%s.xyz - vi[i].normal * (dot(v.%s.xyz, vi[i].normal) - dot(vi[i].%s.xyz, vi[i].normal));\n", m.name.c_str(), m.name.c_str(), m.name.c_str());
            gen += Format("  v.%s.xyz = %s * (pp[0]*bary.x + pp[1]*bary.y + pp[2]*bary.z) + (1.0f-%s) * v.%s.xyz;\n", m.name.c_str(), params.o.tessPhongAlpha.c_str(), params.o.tessPhongAlpha.c_str(), m.name.c_str());
        }
    }

    if (!params.p.entryName[kProgramVertex].empty())
    {
        //@TODO: custom vertex data?
        gen += "  " + params.p.entryName[kProgramVertex] + " (v);\n";
    }

    gen += "  v2f_" + params.p.entryName[kProgramSurface] + " o = vert_" + params.p.entryName[kProgramSurface] + " (v);\n";
    gen += "  return o;\n";
    gen += "}\n";
    gen += "\n#endif // UNITY_CAN_COMPILE_TESSELLATION\n\n";
}

static int GetMaximumInterpolatorsFromRequirements(const ShaderRequirements mask)
{
    if (HasFlag(mask, kShaderRequireInterpolators32))
        return 32;
    // If full "#pragma target 4.0" or "5.0" was specified, treat as if full 32 varyings are supported.
    // Often people write that in surface shaders to get a lot of varyings, and they don't care about
    // mobile platfomrs (which might only provide 15 in some cases).
    if (HasAllFlags(mask, kShaderRequireShaderModel40))
        return 32;

    if (HasFlag(mask, kShaderRequireInterpolators15Integers))
        return 15;
    if (HasFlag(mask, kShaderRequireInterpolators10))
        return 10;
    return 8; // SM2.0 / DX11 FL9.x: 8
}

static bool GenerateForwardBasePassCode(core::string& gen, SurfaceParams& params, const SurfaceFunctionData& funcData, const SurfacePassData& data, const StringArray& keywords, ShaderImportErrors& outErrors)
{
    OutputGeneratedCodeDebugInfo(gen, params, funcData, data);
    core::string v2fdef;

    bool needsWorldPos = false;
    PassFlags passFlags = kPassFlagForwardBase | kPassFlagLighting | kPassFlagLightmaps | kPassFlagNeedsNormal;
    // Does anything need to be derived from world position (shadow need it to fade in the distance)?
    if (NeedsWorldPos(params, data, kPassFlagDefault) || data.lightNeedsWorldViewDir || !params.o.noShadow || IsLPPVAvailableForAnyTargetPlatform(params))
    {
        passFlags |= kPassFlagNeedsWorldPos;
        needsWorldPos = true;
    }
    if (NeedsInterpolatedWorldViewDir(params, data, passFlags))
        passFlags |= kPassFlagNeedsWorldViewDir;
    if (data.useTangentSpace)
        passFlags |= kPassFlagNeedsTangentSpace;

    const bool gi = IsUsingGILightingFunction(params, funcData);
    const bool pbs = IsUsingPBSLightingFunction(params, funcData);

    // generate vertex-to-fragment definition
    for (int lightmaps = 0; lightmaps < 2; lightmaps++)
    {
        if (!lightmaps)
            v2fdef += "// no lightmaps:\n#ifndef LIGHTMAP_ON\n";
        else
            v2fdef += "// with lightmaps:\n#ifdef LIGHTMAP_ON\n";

        for (int highPrecisionFragmentShaderRegisters = 0; highPrecisionFragmentShaderRegisters < 2; ++highPrecisionFragmentShaderRegisters)
        {
            if (!highPrecisionFragmentShaderRegisters)
                v2fdef += "// half-precision fragment shader registers:\n#ifdef UNITY_HALF_PRECISION_FRAGMENT_SHADER_REGISTERS\n";
            else
                v2fdef += "// high-precision fragment shader registers:\n#ifndef UNITY_HALF_PRECISION_FRAGMENT_SHADER_REGISTERS\n";

            PassFlags adjustedPassFlags = passFlags;
            bool fogIsPacked = false;
            if (!highPrecisionFragmentShaderRegisters && params.o.HasFog())
            {
                if (!data.useTangentSpace && needsWorldPos)
                {
                    adjustedPassFlags |= kPassFlagPutFogInWorldPos;
                    fogIsPacked = true;
                }
                else if (data.useTangentSpace)
                {
                    adjustedPassFlags |= kPassFlagPutFogInTSpace;
                    fogIsPacked = true;
                }
            }

            int ntc = EmitSurfaceV2F(v2fdef, params, funcData, data, adjustedPassFlags, 0);

            if (lightmaps)
            {
                if (params.o.dualForward)
                    v2fdef += Format("  float4 lmapFadePos : TEXCOORD%i;\n", ntc++);
                v2fdef += Format("  float4 lmap : TEXCOORD%i;\n", ntc++);
            }

            if (!lightmaps)
            {
                if (gi)
                {
                    v2fdef += "  #if UNITY_SHOULD_SAMPLE_SH\n";
                    v2fdef += Format("  half3 sh : TEXCOORD%i; // SH\n", ntc++);
                    v2fdef += "  #endif\n";
                }
                else if (!params.o.noVertexLights)
                {
                    v2fdef += Format("  fixed3 vlight : TEXCOORD%i; // ambient/SH/vertexlights\n", ntc++);
                }
            }

            if (params.o.HasFog() && !fogIsPacked)
                v2fdef += Format("  UNITY_FOG_COORDS(%i)\n", ntc++);

            if (!params.o.noShadow)
            {
                if (highPrecisionFragmentShaderRegisters)   // AutoLight.cginc will skip light coordinates if UNITY_HALF_PRECISION_FRAGMENT_SHADER_REGISTERS is not defined, so we can save one TC in calculation.
                    v2fdef += Format("  UNITY_SHADOW_COORDS(%i)\n", ntc++);
                else
                {
                    int first = ntc++;
                    int second = ntc++;
                    v2fdef += Format("  UNITY_LIGHTING_COORDS(%i,%i)\n", first, second);
                }
            }
            else
                v2fdef += Format("  DECLARE_LIGHT_COORDS(%i)\n", ntc++);

            // add texcoord for dynamic lightmaps if we need it at the end,
            // and do not count it towards the limit if we're on pre-SM3.0
            if (!lightmaps && !params.o.noDynLightmap)
            {
                v2fdef += Format("  #if SHADER_TARGET >= 30\n  float4 lmap : TEXCOORD%i;\n  #endif\n", ntc);
                if (HasFlag(params.target.m_BaseRequirements, kShaderRequireInterpolators10))
                    ntc++;
            }

            int extraInterpolators = 0;
            // DIRLIGHTMAP_COMBINED requires SM3.0, where we have highp in FS anyway
            if (highPrecisionFragmentShaderRegisters && lightmaps && !params.o.noLightmap && !params.o.noDirLightmap && (adjustedPassFlags & kPassFlagNeedsTangentSpace) == 0)
            {
                v2fdef += Format("  #ifdef DIRLIGHTMAP_COMBINED\n");
                extraInterpolators = EmitTSpaceV2F(v2fdef, ntc, false, false) - ntc;
                v2fdef += Format("  #endif\n");
            }

            v2fdef += "  UNITY_VERTEX_INPUT_INSTANCE_ID\n";
            v2fdef += "  UNITY_VERTEX_OUTPUT_STEREO\n";
            v2fdef += "};\n";

            const int maxTC = GetMaximumInterpolatorsFromRequirements(params.target.m_BaseRequirements);
            const int maxTCForDirLightmapTarget = std::max(GetMaximumInterpolatorsFromRequirements(kShaderRequireInterpolators10), maxTC);
            const bool overflowDueToDirLm = ntc + extraInterpolators > maxTCForDirLightmapTarget;
            if (ntc > maxTC || overflowDueToDirLm)
            {
                gen = "// Too many texture interpolators would be used for ForwardBase pass:\n";
                if (!keywords.empty())
                {
                    gen += "// For a keyword variant:";
                    for (size_t ik = 0; ik < keywords.size(); ++ik)
                        gen += ' ' + keywords[ik];
                    gen += '\n';
                }
                gen += v2fdef;
                const int ntcInfo = overflowDueToDirLm ? ntc + extraInterpolators : ntc;
                const int maxTCInfo = overflowDueToDirLm ? maxTCForDirLightmapTarget : maxTC;
                const bool addPragma30 = !HasFlag(params.target.m_BaseRequirements, kShaderRequireInterpolators10) && !overflowDueToDirLm;
                outErrors.AddImportError(
                    Format("Too many texture interpolators would be used for ForwardBase pass (%d out of max %d)%s", ntcInfo, maxTCInfo, addPragma30 ? ", try adding #pragma target 3.0" : ""),
                    params.startLine, false);
                return false;
            }
            v2fdef += "#endif\n";   // UNITY_HALF_PRECISION_FRAGMENT_SHADER_REGISTERS
        }
        v2fdef += "#endif\n";   // LIGHTMAP_ON
    }

    // structs, variables, hull tessellation
    EmitProlog(gen, params, true, pbs, (passFlags & kPassFlagNeedsTangentSpace) ? kPerPixelReflectYes : kPerPixelReflectNo);
    EmitTessellationHull(gen, params, funcData);

    gen += "\n// vertex-to-fragment interpolation data\n";
    gen += v2fdef;

    // start vertex shader
    EmitVSProlog(gen, params, funcData, data, passFlags);

    if (!params.o.noDynLightmap)
    {
        gen += "  #ifdef DYNAMICLIGHTMAP_ON\n";
        gen += "  o.lmap.zw = v.texcoord2.xy * unity_DynamicLightmapST.xy + unity_DynamicLightmapST.zw;\n";
        gen += "  #endif\n";
    }
    gen += "  #ifdef LIGHTMAP_ON\n";
    gen += "  o.lmap.xy = v.texcoord1.xy * unity_LightmapST.xy + unity_LightmapST.zw;\n";
    if (params.o.dualForward)
    {
        gen += "  o.lmapFadePos.xyz = (mul(unity_ObjectToWorld, v.vertex).xyz - unity_ShadowFadeCenterAndType.xyz) * unity_ShadowFadeCenterAndType.w;\n";
        gen += "  o.lmapFadePos.w = (-UnityObjectToViewPos(v.vertex).z) * (1.0 - unity_ShadowFadeCenterAndType.w);\n";
    }
    gen += "  #endif\n";
    gen += "\n  // SH/ambient and vertex lights\n";
    gen += "  #ifndef LIGHTMAP_ON\n";

    if (gi)
    {
        gen += "    #if UNITY_SHOULD_SAMPLE_SH && !UNITY_SAMPLE_FULL_SH_PER_PIXEL\n";
        gen += "      o.sh = 0;\n";
        gen += "      // Approximated illumination from non-important point lights\n";
        gen += "      #ifdef VERTEXLIGHT_ON\n";
        gen += "        o.sh += Shade4PointLights (\n";
        gen += "          unity_4LightPosX0, unity_4LightPosY0, unity_4LightPosZ0,\n";
        gen += "          unity_LightColor[0].rgb, unity_LightColor[1].rgb, unity_LightColor[2].rgb, unity_LightColor[3].rgb,\n";
        gen += "          unity_4LightAtten0, worldPos, worldNormal);\n";
        gen += "      #endif\n";
        gen += "      o.sh = ShadeSHPerVertex (worldNormal, o.sh);\n";
        gen += "    #endif\n";
    }
    else if (!params.o.noVertexLights)
    {
        if (!params.o.noAmbient)
        {
            gen += "  #if UNITY_SHOULD_SAMPLE_SH && !UNITY_SAMPLE_FULL_SH_PER_PIXEL\n";
            gen += "  float3 shlight = ShadeSH9 (float4(worldNormal,1.0));\n";
            gen += "  o.vlight = shlight;\n";
            gen += "  #else\n";
            gen += "  o.vlight = 0.0;\n";
            gen += "  #endif\n";
        }
        else
            gen += "  o.vlight = 0.0;\n";

        gen += "  #ifdef VERTEXLIGHT_ON\n";
        gen += "  o.vlight += Shade4PointLights (\n";
        gen += "    unity_4LightPosX0, unity_4LightPosY0, unity_4LightPosZ0,\n";
        gen += "    unity_LightColor[0].rgb, unity_LightColor[1].rgb, unity_LightColor[2].rgb, unity_LightColor[3].rgb,\n";
        gen += "    unity_4LightAtten0, worldPos, worldNormal );\n";
        gen += "  #endif // VERTEXLIGHT_ON\n";
    }
    gen += "  #endif // !LIGHTMAP_ON\n\n";

    if (!params.o.noShadow)
        gen += "  UNITY_TRANSFER_LIGHTING(o,v.texcoord1.xy); // pass shadow and, possibly, light cookie coordinates to pixel shader\n";
    else
        gen += "  COMPUTE_LIGHT_COORDS(o); // pass light cookie coordinates to pixel shader\n";

    if (params.o.HasFog())
    {
        gen += "  #ifdef FOG_COMBINED_WITH_TSPACE\n";
        gen += "    UNITY_TRANSFER_FOG_COMBINED_WITH_TSPACE(o,o.pos); // pass fog coordinates to pixel shader\n";
        gen += "  #elif defined FOG_COMBINED_WITH_WORLD_POS\n";
        gen += "    UNITY_TRANSFER_FOG_COMBINED_WITH_WORLD_POS(o,o.pos); // pass fog coordinates to pixel shader\n";
        gen += "  #else\n";
        gen += "    UNITY_TRANSFER_FOG(o,o.pos); // pass fog coordinates to pixel shader\n";
        gen += "  #endif\n";
    }
    gen += "  return o;\n";
    gen += "}\n";

    EmitTessellationDomain(gen, params, funcData);

    if (params.o.dualForward)
    {
        gen += "#ifdef LIGHTMAP_ON\n";
        gen += "float4 unity_LightmapFade;\n";
        gen += "#endif\n";
    }

    EmitPSProlog(gen, params, funcData, data, passFlags);
    gen += "\n  // compute lighting & shadowing factor\n";
    gen += "  UNITY_LIGHT_ATTENUATION(atten, IN, worldPos)\n";
    gen += "  fixed4 c = 0;\n";

    // from now on surface normal is in world space!
    EmitSurfaceNormalToWorldSpace(gen, data);

    if (gi)
    {
        // handle ambient/lightmaps/... with GI lighting function
        EmitGISetup(gen, params, kSurfFwdBase, data.lightNeedsWorldViewDir);
    }
    else
    {
        // otherwise, Unity 4.x way: add code to sample lightmaps

        // add vertex lighting
        if (!params.o.noVertexLights)
        {
            gen += "  #ifndef LIGHTMAP_ON\n";
            gen += "  c.rgb += o.Albedo * IN.vlight;\n";
            gen += "  #endif // !LIGHTMAP_ON\n";
        }

        // lightmaps
        gen += "\n  // lightmaps\n";
        gen += "  #ifdef LIGHTMAP_ON\n";
        {
            gen += "    #if DIRLIGHTMAP_COMBINED\n";
            gen += EmitPSDirLightmaps(params, funcData, data, "c", kPathForward);
            gen += "    #else\n";
            gen += EmitPSSingleLightmap(params, funcData, data, "c", true);
            gen += "    #endif\n\n";
        }
        gen += "  #endif // LIGHTMAP_ON\n\n";
    }

    // realtime lighting
    gen += "\n  // realtime lighting: call lighting function\n";
    if (gi)
    {
        // GI way: call GI lighting function
        EmitPSLightingGIFunc(gen, params, data, "c");
    }
    else
    {
        // Unity 4.x way: if (lightmaps are off) OR (we use dual lightmaps)
        if (params.o.dualForward)
            gen += "  #ifdef DIRLIGHTMAP_OFF\n";
        else
            gen += "  #ifndef LIGHTMAP_ON\n";
        EmitPSLightingFunc(gen, params, data, "c", "atten");

        // if lighting function was not called, we need to initialize alpha from surface
        gen += "  #else\n";
        gen += "    c.a = o.Alpha;\n";
        gen += "  #endif\n\n";
    }


    // combine shadows with lightmaps (when GI is not defined nor used)
    if (!gi)
    {
        gen += "  #ifdef LIGHTMAP_ON\n";
        if (params.o.dualForward)
        {
            gen += "    #ifndef DIRLIGHTMAP_OFF\n";
        }

        if (!params.o.noLightmap)
        {
            // combine shadow and (non-dual) lightmaps
            gen += "    // combine lightmaps with realtime shadows\n";
            gen += "    #ifdef SHADOWS_SCREEN\n";

            // Let's try to make realtime shadows work on a surface, which already contains
            // baked lighting and shadowing from the current light.
            // On gles min(lm, atten*2) will do just fine.
            // Wherever we use rgbm lightmaps this is insufficient though, as lm intensity goes up to 8,
            // and will be clamped to 2 (max value of atten*2) even if there's no rt shadow.
            // max(min(lm,(atten*2)*lmtex.rgb), (lm*atten)) works around this issue nicely.
            // It implicitly assumes lightmap intensity of >2 is not a shadow anymore, so there would be
            // double shadowing there, but that's very bright already and I haven't seen that to be a problem.
            // '*lmtex.rgb' makes sure there's still tint from the lightmap in the baked shadow penumbra;
            // it can be skipped to save 1 alu on some platforms, but methinks it's not worth it.

            gen += "      #if defined(UNITY_NO_RGBM)\n";
            gen += "      c.rgb += o.Albedo * min(lm, atten*2);\n";
            gen += "      #else\n";
            gen += "      c.rgb += o.Albedo * max(min(lm,(atten*2)*lmtex.rgb), lm*atten);\n";
            gen += "      #endif\n";

            gen += "    #else // SHADOWS_SCREEN\n";
            gen += "      c.rgb += o.Albedo * lm;\n";
            gen += "    #endif // SHADOWS_SCREEN\n";
        }

        if (params.o.dualForward)
        {
            gen += "  #else // !DIRLIGHTMAP_OFF\n";
            gen += "  half lmFade2 = length (IN.lmapFadePos) * unity_LightmapFade.z + unity_LightmapFade.w;\n";
            gen += "  c.rgb *= saturate(1.0 - lmFade2);\n";
            gen += "  c.rgb += o.Albedo * lm;\n";
            gen += "  #endif // !DIRLIGHTMAP_OFF\n";
        }
        gen += "  #endif // LIGHTMAP_ON\n\n";
    }

    // add dynamic GI lightmaps (when GI is not defined nor used)
    if (!gi)
    {
        if (!params.o.noDynLightmap)
        {
            gen += "  #ifdef DYNAMICLIGHTMAP_ON\n";
            gen += "  fixed4 dynlmtex = UNITY_SAMPLE_TEX2D(unity_DynamicLightmap, IN.lmap.zw);\n";
            gen += "  c.rgb += o.Albedo * DecodeRealtimeLightmap (dynlmtex);\n";
            gen += "  #endif\n\n";
        }
    }

    if (funcData.writesEmission)
        gen += "  c.rgb += o.Emission;\n";
    EmitPSEpilog(gen, params);

    return true;
}

static void EmitComboCheckClause(const StringArray& keywords, const std::set<core::string>& allKeywords, core::string& gen)
{
    bool first = true;
    // check for keywords that should be defined
    for (size_t i = 0; i < keywords.size(); ++i)
    {
        if (!first)
            gen += " && ";
        first = false;
        gen += "defined(" + keywords[i] + ")";
    }
    // emit checks that all other possible keywords are not defined
    for (std::set<core::string>::const_iterator it = allKeywords.begin(), itEnd = allKeywords.end(); it != itEnd; ++it)
    {
        const core::string& s = *it;
        // is this keyword in keywords array?
        if (std::find(keywords.begin(), keywords.end(), s) != keywords.end())
            continue;
        // it is not in keywords array, so emit check that it is not defined
        if (!first)
            gen += " && ";
        first = false;
        gen += "!defined(" + s + ")";
    }
}

static void EmitComboGuardStart(const SurfaceData& data, size_t index, core::string& gen)
{
    // just one variant in total, no need for guards
    if (data.m_Datas.size() <= 1)
        return;

    const StringArray& keywords = data.m_Datas[index].keywords;

    // add comment for variant start
    gen += "// -------- variant for: ";
    if (!keywords.empty())
    {
        for (size_t i = 0; i < keywords.size(); ++i)
            gen += keywords[i] + ' ';
    }
    else
        gen += "<when no other keywords are defined>";
    gen += '\n';

    // add preprocessor check
    gen += "#if ";
    EmitComboCheckClause(keywords, data.m_AllKeywords, gen);
    gen += '\n';
}

static void EmitComboGuardEnd(const SurfaceData& data, size_t index, core::string& gen)
{
    // just one variant in total, no need for guards
    if (data.m_Datas.size() <= 1)
        return;

    gen += "\n\n#endif\n\n";
}

static void GenerateForwardBasePass(core::string& result, SurfaceParams& params, const SurfaceData& data, ShaderImportErrors& outErrors)
{
    // generate code
    core::string gen;
    for (size_t i = 0; i < data.m_Datas.size(); ++i)
    {
        EmitComboGuardStart(data, i, gen);
        if (!GenerateForwardBasePassCode(gen, params, data.m_Datas[i].func, data.m_Datas[i].passes[kSurfFwdBase], data.m_Datas[i].keywords, outErrors))
        {
            result += gen;
            return;
        }
        EmitComboGuardEnd(data, i, gen);
    }

    const bool pbs = IsUsingPBSLightingFunction(params, data);

    result += "\n\t// ---- forward rendering base pass:\n";
    EmitPassProlog(result, params, "FORWARD", "ForwardBase");
    EmitPassBlending(result, params, false, pbs);

    // output final code
    core::string variants;
    if (params.o.HasFog())
        variants += kGenerateVariantsFog;
    variants += (params.o.mode == kSurfaceModeAlpha ? "#pragma multi_compile_fwdbasealpha" : "#pragma multi_compile_fwdbase");
    OutputGeneratedSnippet(variants, result, gen, params, params.shaderAPIs, outErrors);
}

static int GenerateForwardAddV2F(core::string& gen, const SurfaceParams& params, const SurfaceFunctionData& funcData, const SurfacePassData& data, PassFlags passFlags)
{
    gen.clear();
    int ntc = EmitSurfaceV2F(gen, params, funcData, data, passFlags, 0);
    if (!params.o.noShadow)
    {
        // It's possible to save one texture coordinate set in case UNITY_HALF_PRECISION_FRAGMENT_SHADER_REGISTERS is not defined, but it seems that we have spare anyway and the code would get even messier...
        int first = ntc++;
        int second = ntc++;
        gen += Format("  UNITY_LIGHTING_COORDS(%i,%i)\n", first, second);
    }
    else
        gen += Format("  DECLARE_LIGHT_COORDS(%i)\n", ntc++);

    if (params.o.HasFog())
        gen += Format("  UNITY_FOG_COORDS(%i)\n", ntc++);

    gen += "  UNITY_VERTEX_INPUT_INSTANCE_ID\n";
    gen += "  UNITY_VERTEX_OUTPUT_STEREO\n";
    return ntc;
}

static void GenerateForwardAddPassCode(core::string& gen, SurfaceParams& params, const SurfaceFunctionData& funcData, const SurfacePassData& data, ShaderImportErrors& outErrors)
{
    OutputGeneratedCodeDebugInfo(gen, params, funcData, data);

    PassFlags passFlags =  kPassFlagLighting | kPassFlagNeedsNormal | kPassFlagNeedsWorldPos;
    if (NeedsInterpolatedWorldViewDir(params, data, passFlags))
        passFlags |= kPassFlagNeedsWorldViewDir;
    if (data.useTangentSpace)
        passFlags |= kPassFlagNeedsTangentSpace;

    const bool pbs = IsUsingPBSLightingFunction(params, funcData);
    const bool gi = IsUsingGILightingFunction(params, funcData);

    // structs, variables, hull tessellation
    EmitProlog(gen, params, true, pbs, (passFlags & kPassFlagNeedsTangentSpace) ? kPerPixelReflectYes : kPerPixelReflectNo);
    EmitTessellationHull(gen, params, funcData);

    gen += "\n// vertex-to-fragment interpolation data\n";

    // generate vertex-to-fragment structure definition
    core::string v2fgen;
    int v2fTexcoordCount = GenerateForwardAddV2F(v2fgen, params, funcData, data, passFlags);
    if ((passFlags & kPassFlagNeedsTangentSpace) && (passFlags & kPassFlagNeedsWorldPos))
    {
        // if we do have spare texcoords, then put worldPos into a separate one instead of packing into .w components
        // of others; saves ~5 ALU on ps_2_0
        int maxTC = GetMaximumInterpolatorsFromRequirements(params.target.m_BaseRequirements);
        if (v2fTexcoordCount < maxTC)
        {
            passFlags |= kPassFlagSeparateWorldPosTexcoord;
            GenerateForwardAddV2F(v2fgen, params, funcData, data, passFlags);
        }
    }
    gen += v2fgen;

    gen += "};\n";
    EmitVSProlog(gen, params, funcData, data, passFlags);
    gen += "\n";

    if (!params.o.noShadow)
        gen += "  UNITY_TRANSFER_LIGHTING(o,v.texcoord1.xy); // pass shadow and, possibly, light cookie coordinates to pixel shader\n";
    else
        gen += "  COMPUTE_LIGHT_COORDS(o); // pass light cookie coordinates to pixel shader\n";

    if (params.o.HasFog())
        gen += "  UNITY_TRANSFER_FOG(o,o.pos); // pass fog coordinates to pixel shader\n";

    gen += "  return o;\n";
    gen += "}\n";

    EmitTessellationDomain(gen, params, funcData);

    EmitPSProlog(gen, params, funcData, data, passFlags);
    gen += "  UNITY_LIGHT_ATTENUATION(atten, IN, worldPos)\n";
    gen += "  fixed4 c = 0;\n";

    EmitSurfaceNormalToWorldSpace(gen, data);


    if (gi)
    {
        EmitGISetup(gen, params, kSurfFwdAdd, data.lightNeedsWorldViewDir);
        gen += "  gi.light.color *= atten;\n";
        EmitPSLightingGIFunc(gen, params, data, "c");
    }
    else
    {
        EmitPSLightingFunc(gen, params, data, "c", "atten");
    }

    if (IsOpaque(params))
        gen += "  c.a = 0.0;\n";
    EmitPSEpilog(gen, params);
}

static void GenerateForwardAddPass(core::string& result, SurfaceParams& params, const SurfaceData& data, ShaderImportErrors& outErrors)
{
    // generate code
    core::string gen;

    for (size_t i = 0; i < data.m_Datas.size(); ++i)
    {
        // just an optimisation: skip INSTANCING_ON here since we have kSkipVariantsInstancing
        if (params.o.HasInstancing() && !params.o.instancingInForwardAdd && SkipEmittingComboForInstancing(data, i))
            continue;

        EmitComboGuardStart(data, i, gen);
        core::string gen2;
        GenerateForwardAddPassCode(gen2, params, data.m_Datas[i].func, data.m_Datas[i].passes[kSurfFwdAdd], outErrors);
        gen += gen2;
        EmitComboGuardEnd(data, i, gen);
    }

    const bool pbs = IsUsingPBSLightingFunction(params, data);

    result += "\n\t// ---- forward rendering additive lights pass:\n";
    EmitPassProlog(result, params, "FORWARD", "ForwardAdd");
    EmitPassBlending(result, params, true, pbs);


    // output final code
    core::string variants;
    if (params.o.HasFog())
        variants += kGenerateVariantsFog;
    if (!params.o.instancingInForwardAdd)
        variants += kSkipVariantsInstancing;
    variants += params.o.fullForwardShadows ? "#pragma multi_compile_fwdadd_fullshadows" : "#pragma multi_compile_fwdadd";
    OutputGeneratedSnippet(variants, result, gen, params, params.shaderAPIs, outErrors);
}

static void GeneratePrePassBasePassCode(core::string& gen, SurfaceParams& params, const SurfaceFunctionData& funcData, const SurfacePassData& data, ShaderImportErrors& outErrors)
{
    OutputGeneratedCodeDebugInfo(gen, params, funcData, data);

    PassFlags passFlags = kPassFlagPrePassBase | kPassFlagNeedsNormal;
    if (data.useTangentSpace)
        passFlags |= kPassFlagNeedsTangentSpace;
    if (NeedsWorldPos(params, data, passFlags))
        passFlags |= kPassFlagNeedsWorldPos;

    // structs, variables, hull tessellation
    EmitProlog(gen, params, false, false, (passFlags & kPassFlagNeedsTangentSpace) ? kPerPixelReflectYes : kPerPixelReflectNo);
    EmitTessellationHull(gen, params, funcData);

    gen += "\n// vertex-to-fragment interpolation data\n";
    EmitSurfaceV2F(gen, params, funcData, data, passFlags, 0);
    gen += "  UNITY_VERTEX_INPUT_INSTANCE_ID\n";
    gen += "  UNITY_VERTEX_OUTPUT_STEREO\n";

    gen += "};\n";
    EmitVSProlog(gen, params, funcData, data, passFlags);
    gen += "  return o;\n";
    gen += "}\n";

    EmitTessellationDomain(gen, params, funcData);

    EmitPSProlog(gen, params, funcData, data, passFlags);

    EmitSurfaceNormalToWorldSpace(gen, data);

    gen += "\n  // output normal and specular\n";
    gen += "  fixed4 res;\n";
    gen += "  res.rgb = o.Normal * 0.5 + 0.5;\n";
    gen += "  res.a = o.Specular;\n";
    if (params.o.mode == kSurfaceModeDecalBlend)
        gen += "  res.a = o.Alpha;\n";
    if (!params.o.finalPrepassModifier.empty())
        gen += "  " + params.o.finalPrepassModifier + " (surfIN, o, res);\n";
    gen += "  return res;\n";
    gen += "}\n";
}

static void GeneratePrePassBasePass(core::string& result, SurfaceParams& params, const SurfaceData& data, ShaderImportErrors& outErrors)
{
    // semitransparent geometry is not in pre-pass renderer
    if (params.o.mode == kSurfaceModeAlpha)
        return;

    if ((params.o.mode == kSurfaceModeDecalAdd || params.o.mode == kSurfaceModeDecalBlend)
        && params.o.finalPrepassModifier.empty())
    {
        // decals can be just skipped from base pass if they don't use per-pixel normal
        // nor writes to specular and no finalprepass modifiers specified
        bool allSkip = true;
        for (size_t i = 0; i < data.m_Datas.size(); ++i)
        {
            if (data.m_Datas[i].func.writesNormal || data.m_Datas[i].func.writesSpecular)
            {
                allSkip = false;
                break;
            }
        }
        if (allSkip)
            return;
    }

    // generate code
    core::string gen;
    for (size_t i = 0; i < data.m_Datas.size(); ++i)
    {
        EmitComboGuardStart(data, i, gen);
        GeneratePrePassBasePassCode(gen, params, data.m_Datas[i].func, data.m_Datas[i].passes[kSurfPrepassBase], outErrors);
        EmitComboGuardEnd(data, i, gen);
    }

    result += "\n\t// ---- deferred lighting base geometry pass:\n";
    EmitPassProlog(result, params, "PREPASS", "PrePassBase");

    if (params.o.mode == kSurfaceModeDecalAdd)
    {
        // In additive decal mode, no good way to do additive blending of
        // normals (neutral normal is 0.5 value, so we'd need signed addition; A+B-0.5.
        // Approximate that by doing multiply-double blending; A*B*2.
        if (params.o.finalPrepassModifier.empty())
            result += "\t\tBlend DstColor SrcColor, One One ZWrite Off\n";
        else
            // Terrain shader specifies finalprepass modifier to achieve correct blending of normals thus we use additive blending still.
            result += "\t\tBlend One One ZWrite Off\n";
    }

    // In blended decals mode, need to put blend factor into alpha channel. This means
    // we can't also write the specular factor (since that goes into alpha as well).
    // So intead, choose to not modify specular at all with RGB color mask; alpha decals
    // in deferred will get specular of whatever was underneath.
    if (params.o.mode == kSurfaceModeDecalBlend)
    {
        result += "\t\tBlend SrcAlpha OneMinusSrcAlpha ZWrite Off";
        if (!params.o.noColorMask)
            result += " ColorMask RGB\n";
        else
            result += "\n";
    }

    // output final code
    core::string variants = kSkipVariantsFog;
    OutputGeneratedSnippet(variants, result, gen, params, params.shaderAPIs, outErrors);
}

static void GeneratePrePassFinalPassCode(core::string& gen, SurfaceParams& params, const SurfaceFunctionData& funcData, const SurfacePassData& data, ShaderImportErrors& outErrors)
{
    OutputGeneratedCodeDebugInfo(gen, params, funcData, data);

    const bool sampleNormalsSpecForDirLightmaps = true;

    PassFlags passFlags = kPassFlagPrePassFinal | kPassFlagLightmaps;
    // Does anything need to be derived from world position?
    // Pass default flags since we don't need the position for dir lightmaps.
    if (NeedsWorldPos(params, data, kPassFlagDefault))
        passFlags |= kPassFlagNeedsWorldPos;
    // Do we really need the vertex normal?
    if (data.readsNormal || data.needsWorldNormal)
        passFlags |= kPassFlagNeedsNormal;
    // Do we really need tangent space?
    // Generally not, so we only require it for per-pixel worldRefl and worldNormal.
    // Without tangent space we handle viewDir separately for dir lightmaps and surface input.
    if (data.useTangentSpace && (data.needsWorldReflection || data.needsWorldNormal))
        passFlags |= kPassFlagNeedsTangentSpace;
    else if (data.lightmapNeedsViewDir || data.needsViewDir)
        passFlags |= kPassFlagNeedsViewDir;

    // structs, variables, hull tessellation
    EmitProlog(gen, params, false, false, (passFlags & kPassFlagNeedsTangentSpace) ? kPerPixelReflectYes : kPerPixelReflectNo);
    EmitTessellationHull(gen, params, funcData);

    gen += "\n// vertex-to-fragment interpolation data\n";
    int ntc = EmitSurfaceV2F(gen, params, funcData, data, passFlags, 0);
    gen += Format("  float4 screen : TEXCOORD%i;\n", ntc++);

    gen += Format("  float4 lmap : TEXCOORD%i;\n", ntc++);
    gen += "#ifndef LIGHTMAP_ON\n";
    if (!params.o.noAmbient && !params.o.noVertexLights)
        gen += Format("  float3 vlight : TEXCOORD%i;\n", ntc);
    gen += "#else\n";
    gen += "#ifdef DIRLIGHTMAP_OFF\n";
    gen += Format("  float4 lmapFadePos : TEXCOORD%i;\n", ntc++);
    gen += "#endif\n";

    gen += "#endif\n";
    if (params.o.HasFog())
        gen += Format("  UNITY_FOG_COORDS(%i)\n", ntc++);
    if ((passFlags & kPassFlagNeedsTangentSpace) == 0)
    {
        gen += Format("  #if defined(LIGHTMAP_ON) && defined(DIRLIGHTMAP_COMBINED)\n");
        ntc = EmitTSpaceV2F(gen, ntc, false, false);
        gen += Format("  #endif\n");
    }
    gen += "  UNITY_VERTEX_INPUT_INSTANCE_ID\n";
    gen += "  UNITY_VERTEX_OUTPUT_STEREO\n";
    gen += "};\n";

    EmitVSProlog(gen, params, funcData, data, passFlags);
    gen += "  o.screen = ComputeScreenPos (o.pos);\n";

    if (!params.o.noDynLightmap)
    {
        gen += "#ifdef DYNAMICLIGHTMAP_ON\n";
        gen += "  o.lmap.zw = v.texcoord2.xy * unity_DynamicLightmapST.xy + unity_DynamicLightmapST.zw;\n";
        gen += "#else\n";
        gen += "  o.lmap.zw = 0;\n";
        gen += "#endif\n";
    }
    else
    {
        gen += "  o.lmap.zw = 0;\n";
    }

    gen += "#ifdef LIGHTMAP_ON\n";
    gen += "  o.lmap.xy = v.texcoord1.xy * unity_LightmapST.xy + unity_LightmapST.zw;\n";
    gen += "  #ifdef DIRLIGHTMAP_OFF\n";
    gen += "    o.lmapFadePos.xyz = (mul(unity_ObjectToWorld, v.vertex).xyz - unity_ShadowFadeCenterAndType.xyz) * unity_ShadowFadeCenterAndType.w;\n";
    gen += "    o.lmapFadePos.w = (-UnityObjectToViewPos(v.vertex).z) * (1.0 - unity_ShadowFadeCenterAndType.w);\n";
    gen += "  #endif\n";
    gen += "#else\n";
    gen += "  o.lmap.xy = 0;\n";
    if (!params.o.noAmbient && !params.o.noVertexLights)
    {
        // re-use world normal if it was already needed AND not only in the fragment shader
        if (data.needsWorldNormal && !(passFlags & kPassFlagNeedsTangentSpace))
        {
            gen += "  o.vlight = ShadeSH9 (float4(o.worldNormal,1.0));\n";
        }
        else
        {
            gen += "  float3 worldN = UnityObjectToWorldNormal(v.normal);\n";
            gen += "  o.vlight = ShadeSH9 (float4(worldN,1.0));\n";
        }
    }
    gen += "#endif\n";

    if (params.o.HasFog())
    {
        gen += "  UNITY_TRANSFER_FOG(o,o.pos); // pass fog coordinates to pixel shader\n";
    }
    gen += "  return o;\n";
    gen += "}\n";

    EmitTessellationDomain(gen, params, funcData);

    gen += "sampler2D _LightBuffer;\n";
    gen += "sampler2D _CameraNormalsTexture;\n";
    gen += "#ifdef LIGHTMAP_ON\n";
    gen += "float4 unity_LightmapFade;\n";
    gen += "#endif\n";
    gen += "fixed4 unity_Ambient;\n";
    EmitPSProlog(gen, params, funcData, data, passFlags);
    gen += "  half4 light = tex2Dproj (_LightBuffer, UNITY_PROJ_COORD(IN.screen));\n";
    gen += "#if defined (SHADER_API_MOBILE)\n";
    gen += "  light = max(light, half4(0.001, 0.001, 0.001, 0.001));\n";
    gen += "#endif\n";
    gen += "#ifndef UNITY_HDR_ON\n";
    gen += "  light = -log2(light);\n";
    gen += "#endif\n";

    gen += "  #ifdef LIGHTMAP_ON\n";
    gen += "    #ifdef DIRLIGHTMAP_OFF\n";
    gen += EmitPSSingleLightmap(params, funcData, data, "light", false);

    gen += "    #elif DIRLIGHTMAP_COMBINED\n";
    if (sampleNormalsSpecForDirLightmaps)
    {
        gen += "      half4 nspec = tex2Dproj (_CameraNormalsTexture, UNITY_PROJ_COORD(IN.screen));\n";
        gen += "      half3 normal = nspec.rgb * 2 - 1;\n";
        gen += "      o.Normal = normalize(normal);\n";
        // this normal is already in world space
    }
    else
    {
        // From now on surface normal is in world space, but nothing else uses it.
        EmitSurfaceNormalToWorldSpace(gen, data);
    }
    gen += EmitPSDirLightmaps(params, funcData, data, "light", kPathPrePass);
    gen += "    #endif // DIRLIGHTMAP_OFF\n";
    gen += "  #else\n";

    if (!params.o.noAmbient)
    {
        if (!params.o.noVertexLights)
            gen += "    light.rgb += IN.vlight;\n";
        else
            gen += "    light.rgb += unity_Ambient.rgb;\n";
    }
    gen += "  #endif // LIGHTMAP_ON\n\n";

    if (!params.o.noDynLightmap)
    {
        gen += "  #ifdef DYNAMICLIGHTMAP_ON\n";
        gen += "  fixed4 dynlmtex = UNITY_SAMPLE_TEX2D(unity_DynamicLightmap, IN.lmap.zw);\n";
        gen += "  light.rgb += DecodeRealtimeLightmap (dynlmtex);\n";
        gen += "  #endif\n\n";
    }

    gen += "  half4 c = Lighting" + params.o.lighting + "_PrePass (o, light);\n";
    if (funcData.writesEmission)
        gen += "  c.rgb += o.Emission;\n";
    EmitPSEpilog(gen, params);
}

static void GeneratePrePassFinalPass(core::string& result, SurfaceParams& params, const SurfaceData& data, ShaderImportErrors& outErrors)
{
    // semitransparent geometry is not in pre-pass renderer
    if (params.o.mode == kSurfaceModeAlpha)
        return;

    // generate code
    core::string gen;
    for (size_t i = 0; i < data.m_Datas.size(); ++i)
    {
        EmitComboGuardStart(data, i, gen);
        GeneratePrePassFinalPassCode(gen, params, data.m_Datas[i].func, data.m_Datas[i].passes[kSurfPrepassFinal], outErrors);
        EmitComboGuardEnd(data, i, gen);
    }

    result += "\n\t// ---- deferred lighting final pass:\n";
    EmitPassProlog(result, params, "PREPASS", "PrePassFinal");
    result += "\t\tZWrite Off\n";
    if (params.o.mode == kSurfaceModeDecalAdd)
        result += "\t\tBlend One One\n";
    if (params.o.mode == kSurfaceModeDecalBlend)
        result += "\t\tBlend SrcAlpha OneMinusSrcAlpha\n";

    // output final code
    core::string variants;
    if (params.o.HasFog())
        variants += kGenerateVariantsFog;
    variants += "#pragma multi_compile_prepassfinal";
    OutputGeneratedSnippet(variants, result, gen, params, params.shaderAPIs, outErrors);
}

static void GenerateDeferredPassCode(core::string& gen, SurfaceParams& params, const SurfaceFunctionData& funcData, const SurfacePassData& data, ShaderImportErrors& outErrors)
{
    OutputGeneratedCodeDebugInfo(gen, params, funcData, data);

    const bool gi = IsUsingGILightingFunction(params, funcData);
    const bool pbs = IsUsingPBSLightingFunction(params, funcData);

    PassFlags passFlags = kPassFlagDeferred | kPassFlagLightmaps | kPassFlagNeedsNormal;
    passFlags |= kPassFlagLighting; // Temporarily we call Deferred function for reflection probes TODO: Split the reflection probe functionality
    // Does anything need to be derived from world position?
    // Pass default flags since we don't need the position for dir lightmaps.
    if (NeedsWorldPos(params, data, kPassFlagDefault))
        passFlags |= kPassFlagNeedsWorldPos;
    // Do we really need the vertex normal?
    if (data.readsNormal || data.needsWorldNormal)
        passFlags |= kPassFlagNeedsNormal;
    if (data.useTangentSpace)
        passFlags |= kPassFlagNeedsTangentSpace;
    if (data.lightmapNeedsViewDir || data.needsViewDir || pbs)
        passFlags |= kPassFlagNeedsViewDir;
    if (pbs)
        passFlags |= kPassFlagPBS;


    // structs, variables, hull tessellation
    EmitProlog(gen, params, false, pbs, (passFlags & kPassFlagNeedsTangentSpace) ? kPerPixelReflectYes : kPerPixelReflectNo);
    EmitTessellationHull(gen, params, funcData);

    gen += "\n// vertex-to-fragment interpolation data\n";
    int ntc = EmitSurfaceV2F(gen, params, funcData, data, passFlags, 0);
    gen += Format("  float4 lmap : TEXCOORD%i;\n", ntc++);
    gen += "#ifndef LIGHTMAP_ON\n";

    // Not incrementing the tex coord counter in the block below, because every usage of it is guarded by ifdefs and thus the same index can be used.
    if (gi)
    {
        gen += "  #if UNITY_SHOULD_SAMPLE_SH && !UNITY_SAMPLE_FULL_SH_PER_PIXEL\n";
        gen += Format("    half3 sh : TEXCOORD%i; // SH\n", ntc);
        gen += "  #endif\n";
    }
    else
    {
        if (!params.o.noAmbient && !params.o.noVertexLights)
            gen += Format("  float3 vlight : TEXCOORD%i; // ambient/SH\n", ntc);
    }
    gen += "#else\n";
    gen += "  #ifdef DIRLIGHTMAP_OFF\n";
    gen += Format("    float4 lmapFadePos : TEXCOORD%i;\n", ntc);
    gen += "  #endif\n";
    gen += "#endif\n";
    ntc++;
    gen += "  UNITY_VERTEX_INPUT_INSTANCE_ID\n";
    gen += "  UNITY_VERTEX_OUTPUT_STEREO\n";
    gen += "};\n";


    EmitVSProlog(gen, params, funcData, data, passFlags);

    if (!params.o.noDynLightmap)
    {
        gen += "#ifdef DYNAMICLIGHTMAP_ON\n";
        gen += "  o.lmap.zw = v.texcoord2.xy * unity_DynamicLightmapST.xy + unity_DynamicLightmapST.zw;\n";
        gen += "#else\n";
        gen += "  o.lmap.zw = 0;\n";
        gen += "#endif\n";
    }
    else
    {
        gen += "  o.lmap.zw = 0;\n";
    }


    gen += "#ifdef LIGHTMAP_ON\n";
    gen += "  o.lmap.xy = v.texcoord1.xy * unity_LightmapST.xy + unity_LightmapST.zw;\n";
    gen += "  #ifdef DIRLIGHTMAP_OFF\n";
    gen += "    o.lmapFadePos.xyz = (mul(unity_ObjectToWorld, v.vertex).xyz - unity_ShadowFadeCenterAndType.xyz) * unity_ShadowFadeCenterAndType.w;\n";
    gen += "    o.lmapFadePos.w = (-UnityObjectToViewPos(v.vertex).z) * (1.0 - unity_ShadowFadeCenterAndType.w);\n";
    gen += "  #endif\n";
    gen += "#else\n";
    gen += "  o.lmap.xy = 0;\n";

    if (gi)
    {
        gen += "    #if UNITY_SHOULD_SAMPLE_SH && !UNITY_SAMPLE_FULL_SH_PER_PIXEL\n";
        gen += "      o.sh = 0;\n";
        gen += "      o.sh = ShadeSHPerVertex (worldNormal, o.sh);\n";
        gen += "    #endif\n";
    }
    else if (!params.o.noAmbient && !params.o.noVertexLights)
    {
        // re-use world normal if it was already needed AND not only in the fragment shader
        if (data.needsWorldNormal && !(passFlags & kPassFlagNeedsTangentSpace))
        {
            gen += "  o.vlight = ShadeSH9 (float4(worldNormal,1.0));\n";
        }
        else
        {
            gen += "  fixed3 worldN = UnityObjectToWorldNormal(v.normal);\n";
            gen += "  o.vlight = ShadeSH9 (float4(worldN,1.0));\n";
        }
    }
    gen += "#endif\n";
    gen += "  return o;\n";
    gen += "}\n";

    EmitTessellationDomain(gen, params, funcData);

    gen += "#ifdef LIGHTMAP_ON\n";
    gen += "float4 unity_LightmapFade;\n";
    gen += "#endif\n";
    gen += "fixed4 unity_Ambient;\n";

    EmitPSProlog(gen, params, funcData, data, passFlags);

    gen += "fixed3 originalNormal = o.Normal;\n";
    EmitSurfaceNormalToWorldSpace(gen, data);

    if (gi)
    {
        // handle ambient/lightmaps/... with GI lighting function
        gen += "  half atten = 1;\n";
        EmitGISetup(gen, params, kSurfDeferred, data.lightNeedsWorldViewDir);
    }

    gen += "\n  // call lighting function to output g-buffer\n";
    //@TODO: g-buffer layout from lighting function
    core::string extraArgs = "";
    if (gi && data.lightNeedsWorldViewDir)
        extraArgs += ", worldViewDir";
    if (gi)
        extraArgs += ", gi";

    gen += "  outEmission = Lighting" + params.o.lighting + "_Deferred (o" + extraArgs + ", outGBuffer0, outGBuffer1, outGBuffer2);\n";
    gen += "  #if defined(SHADOWS_SHADOWMASK) && (UNITY_ALLOWED_MRT_COUNT > 4)\n";
    if (passFlags & kPassFlagNeedsWorldPos)
        gen += "    outShadowMask = UnityGetRawBakedOcclusions (IN.lmap.xy, worldPos);\n";
    else
        gen += "    outShadowMask = UnityGetRawBakedOcclusions (IN.lmap.xy, float3(0, 0, 0));\n";
    gen += "  #endif\n";

    if (!gi)
    {
        // Unity 4.x way: add lightmaps/vertex/ambient
        gen += "\n  // add lighting from lightmaps / vertex / ambient:\n";
        gen += "  o.Normal = originalNormal;\n";
        gen += "  half3 light = 0;\n";
        gen += "  #ifdef LIGHTMAP_ON\n";
        gen += "    #ifdef DIRLIGHTMAP_OFF\n";
        gen += EmitPSDualLightmaps(params, funcData, data, "light", false);
        gen += "    #else\n";
        gen += EmitPSDirLightmaps(params, funcData, data, "light", kPathDeferred);
        gen += "    #endif\n";
        gen += "  #else\n";
        if (!params.o.noAmbient)
        {
            if (!params.o.noVertexLights)
                gen += "    light.rgb += IN.vlight;\n";
            else
                gen += "    light.rgb += unity_Ambient.rgb;\n";
        }
        gen += "  #endif\n\n";

        if (!params.o.noDynLightmap)
        {
            gen += "  #ifdef DYNAMICLIGHTMAP_ON\n";
            gen += "  fixed4 dynlmtex = UNITY_SAMPLE_TEX2D(unity_DynamicLightmap, IN.lmap.zw);\n";
            gen += "  light.rgb += DecodeRealtimeLightmap (dynlmtex);\n";
            gen += "  #endif\n\n";
        }
        gen += "  outEmission.rgb += light * o.Albedo;\n";
    }

    gen += "  #ifndef UNITY_HDR_ON\n";
    gen += "  outEmission.rgb = exp2(-outEmission.rgb);\n";
    gen += "  #endif\n";

    if (!(passFlags & kPassFlagDeferred) && IsOpaque(params) && !funcData.writesOcclusion)
    {
        gen += "  UNITY_OPAQUE_ALPHA(outGBuffer0.a);\n"; // This code is used in forward only, and in this case the outGBuffer0.a is not the Gbuffer but simply the alpha channel of the output rendertarget
    }

    if (!params.o.finalGBufferModifier.empty())
    {
        gen += "  " + params.o.finalGBufferModifier + " (surfIN, o, outGBuffer0, outGBuffer1, outGBuffer2, outEmission);\n";
    }

    gen += "}\n";
}

static void GenerateDeferredPass(core::string& result, SurfaceParams& params, const SurfaceData& data, ShaderImportErrors& outErrors)
{
    // deferred can not do alpha blended or decal blend mode
    if (params.o.mode == kSurfaceModeAlpha || params.o.mode == kSurfaceModeDecalBlend)
        return;

    // decal add mode is not deferred either, unless there is a gbuffer modifier (e.g. terrain)
    if (params.o.mode == kSurfaceModeDecalAdd && params.o.finalGBufferModifier.empty())
        return;

    // generate code
    core::string gen;
    for (size_t i = 0; i < data.m_Datas.size(); ++i)
    {
        EmitComboGuardStart(data, i, gen);
        GenerateDeferredPassCode(gen, params, data.m_Datas[i].func, data.m_Datas[i].passes[kSurfDeferred], outErrors);
        EmitComboGuardEnd(data, i, gen);
    }

    result += "\n\t// ---- deferred shading pass:\n";
    EmitPassProlog(result, params, "DEFERRED", "Deferred");
    if (params.o.mode == kSurfaceModeDecalAdd)
        result += "\t\tZWrite Off\n\t\tBlend One One\n";

    // output final code
    core::string variants;
    variants = "#pragma exclude_renderers nomrt\n";
    ShaderRequirements oldReqs = params.target.m_BaseRequirements;
    params.target.m_BaseRequirements |= kShaderRequireMRT4;
    variants += kSkipVariantsFog;
    variants += "#pragma multi_compile_prepassfinal";
    OutputGeneratedSnippet(variants, result, gen, params, params.shaderAPIs, outErrors);
    params.target.m_BaseRequirements = oldReqs;
}

static void GenerateShadowCasterPassCode(core::string& gen, SurfaceParams& params, const SurfaceFunctionData& funcData, const SurfacePassData& data, ShaderImportErrors& outErrors)
{
    OutputGeneratedCodeDebugInfo(gen, params, funcData, data);

    PassFlags passFlags = kPassFlagShadowCaster;
    if (NeedsWorldPos(params, data, passFlags))
        passFlags |= kPassFlagNeedsWorldPos;
    // Do we really need the vertex normal?
    if (data.readsNormal || data.needsWorldNormal)
        passFlags |= kPassFlagNeedsNormal;

    // structs, variables, hull tessellation
    EmitProlog(gen, params, false, false, (passFlags & kPassFlagNeedsTangentSpace) ? kPerPixelReflectYes : kPerPixelReflectNo);
    EmitTessellationHull(gen, params, funcData);

    gen += "\n// vertex-to-fragment interpolation data\n";
    EmitSurfaceV2F(gen, params, funcData, data, passFlags, 1);  // 1 TexCoord used by V2F_SHADOW_CASTER
    gen += "  UNITY_VERTEX_INPUT_INSTANCE_ID\n";
    gen += "  UNITY_VERTEX_OUTPUT_STEREO\n";
    gen += "};\n";
    EmitVSProlog(gen, params, funcData, data, passFlags);
    gen += "  TRANSFER_SHADOW_CASTER_NORMALOFFSET(o)\n";
    gen += "  return o;\n";
    gen += "}\n";

    EmitTessellationDomain(gen, params, funcData);

    EmitPSProlog(gen, params, funcData, data, passFlags);
    gen += "  SHADOW_CASTER_FRAGMENT(IN)\n";
    gen += "}\n";
}

static void GenerateShadowCasterPass(core::string& result, SurfaceParams& params, const SurfaceData& data, ShaderImportErrors& outErrors)
{
    // semitransparent geometry does not cast shadows
    if (params.o.mode == kSurfaceModeAlpha)
        return;

    // generate code
    core::string gen;
    for (size_t i = 0; i < data.m_Datas.size(); ++i)
    {
        EmitComboGuardStart(data, i, gen);
        GenerateShadowCasterPassCode(gen, params, data.m_Datas[i].func, data.m_Datas[i].passes[kSurfShadow], outErrors);
        EmitComboGuardEnd(data, i, gen);
    }

    result += "\n\t// ---- shadow caster pass:\n";
    EmitPassProlog(result, params, "ShadowCaster", "ShadowCaster");
    result += "\t\tZWrite On ZTest LEqual\n";

    // output final code
    core::string variants = kSkipVariantsFog;
    variants += "#pragma multi_compile_shadowcaster";
    OutputGeneratedSnippet(variants, result, gen, params, params.shaderAPIs, outErrors);
}

static void GenerateMetaPassCode(core::string& gen, SurfaceParams& params, const SurfaceFunctionData& funcData, const SurfacePassData& data, ShaderImportErrors& outErrors)
{
    OutputGeneratedCodeDebugInfo(gen, params, funcData, data);

    PassFlags passFlags = kPassFlagMeta;
    if (NeedsWorldPos(params, data, passFlags))
        passFlags |= kPassFlagNeedsWorldPos;
    if (data.useTangentSpace)
        passFlags |= kPassFlagNeedsTangentSpace;
    if (data.readsNormal || data.needsWorldNormal)
        passFlags |= kPassFlagNeedsNormal;

    // structs, variables, hull tessellation
    EmitProlog(gen, params, false, false, (passFlags & kPassFlagNeedsTangentSpace) ? kPerPixelReflectYes : kPerPixelReflectNo);
    EmitTessellationHull(gen, params, funcData);

    gen += "#include \"UnityMetaPass.cginc\"\n";

    gen += "\n// vertex-to-fragment interpolation data\n";
    EmitSurfaceV2F(gen, params, funcData, data, passFlags, 0);
    gen += "  UNITY_VERTEX_INPUT_INSTANCE_ID\n"; // used for procedural instancing
    gen += "  UNITY_VERTEX_OUTPUT_STEREO\n";
    gen += "};\n";
    EmitVSProlog(gen, params, funcData, data, passFlags);
    gen += "  return o;\n";
    gen += "}\n";

    EmitTessellationDomain(gen, params, funcData);

    EmitPSProlog(gen, params, funcData, data, passFlags);
    gen += "  UnityMetaInput metaIN;\n";
    gen += "  UNITY_INITIALIZE_OUTPUT(UnityMetaInput, metaIN);\n";
    gen += "  metaIN.Albedo = o.Albedo;\n";
    gen += "  metaIN.Emission = o.Emission;\n";
    //Specular can be optional
    if (funcData.desc.FindOutputMember("Specular") != NULL)
    {
        gen += "  metaIN.SpecularColor = o.Specular;\n";
    }
    gen += "#ifdef " kMetaPassEditorVizKeyword "\n";
    gen += "  metaIN.VizUV = IN.vizUV;\n";
    gen += "  metaIN.LightCoord = IN.lightCoord;\n";
    gen += "#endif\n";
    gen += "  return UnityMetaFragment(metaIN);\n";
    gen += "}\n";
}

static void GenerateMetaPass(core::string& result, SurfaceParams& params, const SurfaceData& data, ShaderImportErrors& outErrors)
{
    // generate code
    core::string gen;
    for (size_t i = 0; i < data.m_Datas.size(); ++i)
    {
        EmitComboGuardStart(data, i, gen);
        GenerateMetaPassCode(gen, params, data.m_Datas[i].func, data.m_Datas[i].passes[kSurfMeta], outErrors);
        EmitComboGuardEnd(data, i, gen);
    }

    result += "\n\t// ---- meta information extraction pass:\n";
    EmitPassProlog(result, params, "Meta", "Meta");
    result += "\t\tCull Off\n";

    // output final code
    core::string variants = kSkipVariantsFog;
    //variants += kSkipVariantsInstancing; // Allows instancing for debug visualization purpose
    //Force the Editor Visualization variant.
    variants += "#pragma shader_feature " kMetaPassEditorVizKeyword "\n";
    OutputGeneratedSnippet(variants, result, gen, params, params.shaderAPIs, outErrors);
}

struct CalculatePassJobData
{
    CalculatePassJobData(const SurfaceParams& params, SurfaceData::Entry* datas, SurfaceAnalysisCache& analysisCache, SurfacePass pass, bool lightUsesViewDir, size_t jobCount)
        : params(&params)
        , analysisCache(&analysisCache)
        , pass(pass)
        , lightUsesViewDir(lightUsesViewDir)
        , hadAnyErrors(false)
        , jobDatas(datas)
    {
        jobErrors = new ShaderImportErrors[jobCount];
        jobResult = new core::string[jobCount];
    }

    ~CalculatePassJobData()
    {
        delete[] jobErrors;
        delete[] jobResult;
    }

    const SurfaceParams* params;
    SurfaceAnalysisCache* analysisCache;
    SurfacePass pass;
    bool lightUsesViewDir;
    bool hadAnyErrors;

    SurfaceData::Entry* jobDatas; // one per job
    ShaderImportErrors* jobErrors; // one per job
    core::string* jobResult; // one per job
};

static void CalculatePassJob(CalculatePassJobData* jobData, unsigned index)
{
    if (jobData->hadAnyErrors)
        return; // some other job already had errors; no point in continuing
    SurfacePass pass = jobData->pass;
    SurfaceData::Entry& data = jobData->jobDatas[index];
    CalculateSurfacePassData(*jobData->params, data.keywords, jobData->pass, data.func, data.passes[pass], jobData->jobResult[index], jobData->lightUsesViewDir, jobData->jobErrors[index], *jobData->analysisCache);
    if (jobData->jobErrors[index].HasErrors())
        jobData->hadAnyErrors = true;
}

static void CalculateDataForPass(SurfaceParams& params, SurfacePass pass, SurfaceData& datas, core::string& outResult, const LightingModel& lighting, ShaderImportErrors& outErrors, SurfaceAnalysisCache& analysisCache)
{
    const bool lightUsesViewDir = lighting.useViewDir;
    const bool lightmapUsesViewDir = lighting.UseViewDirLightmap();

    const size_t jobCount = datas.m_Datas.size();
    CalculatePassJobData jobData(params, datas.m_Datas.data(), analysisCache, pass, lightUsesViewDir, jobCount);
    for (size_t ic = 0; ic < jobCount; ++ic)
    {
        datas.m_Datas[ic].passes[pass].lightNeedsWorldViewDir = lightUsesViewDir;
        // TODO: Needs worldPos only for box projection reflection probes. We could save on the worldPos interpolator by e.g. adding a noreflection
        // surface option and then set it on all legacy surface shaders, which right now disable reflections anyway.
        datas.m_Datas[ic].passes[pass].lightNeedsWorldPos = lighting.hasGI;
        datas.m_Datas[ic].passes[pass].lightmapNeedsViewDir = lightmapUsesViewDir;
    }

    JobFence fence;
    ScheduleJobForEach(fence, CalculatePassJob, &jobData, jobCount);
    SyncFence(fence);

    for (size_t ic = 0; ic < jobCount; ++ic)
    {
        CheckVertexInputRequirements(params, datas.m_Datas[ic].func, datas.m_Datas[ic].passes[pass], jobData.jobErrors[ic]);
        outErrors.AddErrorsFrom(jobData.jobErrors[ic]);
        if (jobData.jobErrors[ic].HasErrors())
        {
            outResult = jobData.jobResult[ic];
            break;
        }
        outResult += jobData.jobResult[ic];
    }
}

struct CalculateFunctionJobData
{
    CalculateFunctionJobData(const SurfaceParams& params, SurfaceData& outData, SurfaceAnalysisCache& analysisCache, size_t jobCount)
        : params(&params)
        , outData(&outData)
        , analysisCache(&analysisCache)
        , hadAnyErrors(false)
    {
        jobResults = new core::string[jobCount];
        jobErrors = new ShaderImportErrors[jobCount];
    }

    ~CalculateFunctionJobData()
    {
        delete[] jobResults;
        delete[] jobErrors;
    }

    const SurfaceParams* params;
    SurfaceData* outData;
    SurfaceAnalysisCache* analysisCache;
    core::string* jobResults; // one per job
    ShaderImportErrors* jobErrors; // one per job
    bool hadAnyErrors;
};

static void CalculateFunctionJob(CalculateFunctionJobData* jobData, unsigned index)
{
    if (jobData->hadAnyErrors)
        return; // some other job already had errors; no point in continuing
    CalculateSurfaceFunctionData(*jobData->params, jobData->outData->m_Datas[index].keywords, jobData->outData->m_Datas[index].func, jobData->jobResults[index], jobData->jobErrors[index], *jobData->analysisCache);
    if (jobData->jobErrors[index].HasErrors())
        jobData->hadAnyErrors = true;
}

static void CalculateSurfaceFunctionDatas(ShaderCompileVariants& variants, const SurfaceParams& params, core::string& result, SurfaceData& outData, ShaderImportErrors& outErrors, SurfaceAnalysisCache& analysisCache)
{
    size_t combos = variants.PrepareEnumeration(kProgramFragment);
    outData.m_AllKeywords.clear();
    outData.m_Datas.clear();
    outData.m_Datas.resize(combos);

    CalculateFunctionJobData jobData(params, outData, analysisCache, combos);
    for (size_t i = 0; i < combos; ++i)
    {
        variants.Enumerate(outData.m_Datas[i].keywords);
        for (size_t j = 0; j < outData.m_Datas[i].keywords.size(); ++j)
            outData.m_AllKeywords.insert(outData.m_Datas[i].keywords[j]);
    }
    JobFence fence;
    ScheduleJobForEach(fence, CalculateFunctionJob, &jobData, combos);
    SyncFence(fence);
    for (size_t i = 0; i < combos; ++i)
    {
        outErrors.AddErrorsFrom(jobData.jobErrors[i]);
        if (jobData.jobErrors[i].HasErrors())
        {
            result = jobData.jobResults[i];
            return;
        }
    }
}

static bool SupportsInstancing(unsigned int shaderAPIs)
{
    for (int i = 0; i < kShaderCompPlatformCount; ++i)
    {
        if ((shaderAPIs & (1 << i)) != 0
            && SupportsInstancing(static_cast<ShaderCompilerPlatform>(i)))
            return true;
    }
    return false;
}

// TODO: we could combine ShaderCompileVariants parsing together
void ProcessSurfaceParams(SurfaceParams& params)
{
    // don't generate instancing variants if the selected targets don't support instancing.
    params.o.noInstancing |= !SupportsInstancing(params.shaderAPIs);
    bool multiCompileInstancingFound = false;

    const bool addDitherCrossFade = params.o.AddDitherCrossFade();
    bool crossFadeAdded = false;

    // loop through all "#pragma multi_compile" occurances
    core::string& pragmaDirectives = params.o.pragmaDirectives;
    const core::string kPragma("#pragma");
    const char kMultiCompile[] = "multi_compile";
    for (size_t pos = FindTokenInText(pragmaDirectives, kPragma, 0); pos != string::npos; pos = FindTokenInText(pragmaDirectives, kPragma, pos + kPragma.size()))
    {
        size_t lineStart = pos;
        size_t curPos = lineStart + kPragma.size();

        // only match #pragma exactly
        if (!IsTabSpace(pragmaDirectives[curPos]))
            continue;

        size_t multiCompileStart = SkipWhiteSpace(pragmaDirectives, curPos);
        core::string multiCompileLine = ExtractRestOfLine(pragmaDirectives, multiCompileStart, true);
        if (multiCompileLine.compare(0, sizeof(kMultiCompile) - 1, kMultiCompile) != 0) // starts with multi_compile
            continue;
        size_t lineSize = multiCompileStart + multiCompileLine.size() - lineStart;

        if (multiCompileLine == "multi_compile_instancing")
        {
            // instancing: remove multi_compile_instancing if not supported; add if supported
            multiCompileInstancingFound = true;
            if (!params.o.HasInstancing())
            {
                pragmaDirectives.erase(lineStart, lineSize);
                pos -= lineSize;
                continue;
            }
        }

        if (addDitherCrossFade)
        {
            // dither crossfade:
            // - "multi_compile* LOD_FADE_CROSSFADE *"      : do nothing
            // - "multi_compile LOD_FADE_*"                 : append "LOD_FADE_CROSSFADE"
            // - "multi_compile_vertex LOD_FADE_*"          : append "LOD_FADE_CROSSFADE" and "multi_compile_fragment __ LOD_FADE_CROSSFADE"
            // - "multi_compile_fragment LOD_FADE_*"        : (won't work on GL/Vulkan anyway) append "LOD_FADE_CROSSFADE"
            // - "multi_compile_{other types} LOD_FADE_*"   : (won't work on GL/Vulkan anyway) append "multi_compile_fragment __ LOD_FADE_CROSSFADE"
            if (multiCompileLine.find("LOD_FADE_CROSSFADE") != string::npos)
            {
                crossFadeAdded = true;
                continue;
            }
            if (multiCompileLine.find("LOD_FADE_") == string::npos)
                continue;

            size_t multiCompileStart2 = sizeof(kMultiCompile) - 1;
            if (IsTabSpace(multiCompileLine[multiCompileStart2]))
            {
                const char kAppend[] = " LOD_FADE_CROSSFADE";
                pragmaDirectives.insert(lineStart + lineSize, kAppend);
                pos += sizeof(kAppend) - 1;
                crossFadeAdded = true;
                continue;
            }
            else if (multiCompileLine[multiCompileStart2] == '_')
            {
                const char* append;
                if (multiCompileLine.compare(multiCompileStart2 + 1, sizeof("vertex") - 1, "vertex") == 0)
                    append = " LOD_FADE_CROSSFADE\n#pragma multi_compile_fragment __ LOD_FADE_CROSSFADE";
                // TODO: Won't work for GL/Vulkan until we correctly hanlde non-vertex stage-only variants.
                else if (multiCompileLine.compare(multiCompileStart2 + 1, sizeof("fragment") - 1, "fragment") == 0)
                    append = " LOD_FADE_CROSSFADE";
                // TODO: Won't work for GL/Vulkan until we correctly hanlde non-vertex stage-only variants.
                else
                    append = "\n#pragma multi_compile_fragment __ LOD_FADE_CROSSFADE";
                pragmaDirectives.insert(lineStart + lineSize, append);
                pos += std::strlen(append);
                crossFadeAdded = true;
                continue;
            }
        }
    }

    if (!multiCompileInstancingFound && params.o.HasInstancing())
        pragmaDirectives += "#pragma multi_compile_instancing\n";

    if (addDitherCrossFade && !crossFadeAdded)
        pragmaDirectives += "#pragma multi_compile __ LOD_FADE_CROSSFADE\n";
}

void ProcessSurfaceSnippet(SurfaceParams& params, core::string& result, ShaderImportErrors& outErrors, ShaderCompilerPreprocessCallbacks* callbacks)
{
    result.clear();

    ShaderImportErrors errors;

    ProcessSurfaceParams(params);

    // find user-specified compile variants we need to use
    ShaderCompileVariants variants;
    variants.FindVariantDirectives(params.o.pragmaDirectives, !params.p.proceduralFuncName.empty(), errors);
    if (errors.HasErrors())
    {
        outErrors.AddErrorsFrom(errors);
        result += "// invalid multi_compile or shader_feature directives for surface shader\n";
        return;
    }

    // If we're using one of built-in PBS lighting models, we might need to automatically
    // include UnityPBSLighting header. Don't automatically add it if source
    // already includes it (it might want to define BRDF quality before including, etc.)
    const bool usingBuiltinPBSLighting = (params.o.lighting == "Standard" || params.o.lighting == "StandardSpecular");
    const bool hasPBSLightingInclude = params.source.find("#include \"UnityPBSLighting.cginc\"") != core::string::npos;
    if (usingBuiltinPBSLighting && !hasPBSLightingInclude)
    {
        params.o.needsPBSLightingInclude = true;
    }

    const bool hasLightingInclude = params.source.find("#include \"Lighting.cginc\"") != core::string::npos;
    params.o.needsLightingInclude = !hasLightingInclude;

    SurfaceData data;
    core::string dataResult;
    SurfaceAnalysisCache* analysisCache = CreateSurfaceAnalysisCache(params.includeContext);

    // Figure out surface data common to all passes
    CalculateSurfaceFunctionDatas(variants, params, dataResult, data, errors, *analysisCache);
    outErrors.AddErrorsFrom(errors);
    if (errors.HasErrors())
    {
        result += dataResult;
        // In normal case the include dependencies would be handled at compile time. However, a surface shader
        // processing error will prevent that. Therefore we report the encountered include files already here.
        if (callbacks)
            callbacks->OnIncludeDependencies(GetSurfaceAnalysisOpenedIncludes(analysisCache));
        DestroySurfaceAnalysisCache(analysisCache);
        return;
    }
    Assert(!data.m_Datas.empty());

    // Check if we have the needed light model.
    // Look only in the first variant - if your variants are using different lighting models
    // and one of them is missing, you'll just get a not-so-nice compile error later on.
    LightingModels::const_iterator lmit = data.m_Datas[0].func.lightModels.find(params.o.lighting);
    if (lmit == data.m_Datas[0].func.lightModels.end())
    {
        core::string modelnames;
        for (lmit = data.m_Datas[0].func.lightModels.begin(); lmit != data.m_Datas[0].func.lightModels.end(); ++lmit)
        {
            if (!modelnames.empty())
                modelnames += ", ";
            modelnames += lmit->first;
        }
        core::string err = Format("Surface lighting model '%s' not found. Available models: %s", params.o.lighting.c_str(), modelnames.c_str());
        outErrors.AddImportError(err, params.startLine, false);
        result += "// " + err + "\n";
        DestroySurfaceAnalysisCache(analysisCache);
        return;
    }

    const LightingModel& lightModel = lmit->second;

    // Generate shader
    result += "\n\t// ------------------------------------------------------------\n";
    result += "\t// Surface shader code generated out of a CGPROGRAM block:\n";
    if (params.o.mode == kSurfaceModeAlpha)
    {
        result += "\tZWrite Off";
        if (!params.o.noColorMask)
            result += " ColorMask RGB\n";
        else
            result += "\n";
    }
    result += "\t\n";

    // Forward passes
    if ((params.o.renderPaths & lightModel.renderPaths) & (1 << kPathForward))
    {
        if (!params.o.noForwardBase)
            CalculateDataForPass(params, kSurfFwdBase, data, dataResult, lightModel, errors, *analysisCache);
        if (!params.o.noForwardAdd)
            CalculateDataForPass(params, kSurfFwdAdd, data, dataResult, lightModel, errors, *analysisCache);
        if (errors.HasErrors())
        {
            outErrors.AddErrorsFrom(errors);
            result += dataResult;
            if (callbacks)
                callbacks->OnIncludeDependencies(GetSurfaceAnalysisOpenedIncludes(analysisCache));
            DestroySurfaceAnalysisCache(analysisCache);
            return;
        }
        result += dataResult;
        if (!errors.HasErrors() && !params.o.noForwardBase)
        {
            GenerateForwardBasePass(result, params, data, errors);
        }
        if (!errors.HasErrors() && !params.o.noForwardAdd)
        {
            GenerateForwardAddPass(result, params, data, errors);
        }
    }

    // Deferred lighting passes
    if ((params.o.renderPaths & lightModel.renderPaths) & (1 << kPathPrePass))
    {
        CalculateDataForPass(params, kSurfPrepassBase, data, dataResult, lightModel, errors, *analysisCache);
        CalculateDataForPass(params, kSurfPrepassFinal, data, dataResult, lightModel, errors, *analysisCache);
        if (errors.HasErrors())
        {
            outErrors.AddErrorsFrom(errors);
            result += dataResult;
            if (callbacks)
                callbacks->OnIncludeDependencies(GetSurfaceAnalysisOpenedIncludes(analysisCache));
            DestroySurfaceAnalysisCache(analysisCache);
            return;
        }
        result += dataResult;
        if (!errors.HasErrors())
            GeneratePrePassBasePass(result, params, data, errors);
        if (!errors.HasErrors())
            GeneratePrePassFinalPass(result, params, data, errors);
    }

    // Deferred shading pass
    if ((params.o.renderPaths & lightModel.renderPaths) & (1 << kPathDeferred))
    {
        CalculateDataForPass(params, kSurfDeferred, data, dataResult, lightModel, errors, *analysisCache);
        if (errors.HasErrors())
        {
            outErrors.AddErrorsFrom(errors);
            result += dataResult;
            if (callbacks)
                callbacks->OnIncludeDependencies(GetSurfaceAnalysisOpenedIncludes(analysisCache));
            DestroySurfaceAnalysisCache(analysisCache);
            return;
        }
        result += dataResult;
        if (!errors.HasErrors())
            GenerateDeferredPass(result, params, data, errors);
    }

    // shader uses custom vertex modifications or alpha test; add shadow caster/collector passes
    if (params.o.addShadow)
    {
        CalculateDataForPass(params, kSurfShadow, data, dataResult, lightModel, errors, *analysisCache);
        if (errors.HasErrors())
        {
            outErrors.AddErrorsFrom(errors);
            result += dataResult;
            if (callbacks)
                callbacks->OnIncludeDependencies(GetSurfaceAnalysisOpenedIncludes(analysisCache));
            DestroySurfaceAnalysisCache(analysisCache);
            return;
        }
        result += dataResult;
        if (!errors.HasErrors())
            GenerateShadowCasterPass(result, params, data, errors);
    }

    // generate albedo/emission extraction pass (no point in doing it if we don't do lightmaps)
    if (!params.o.noMeta && !params.o.noLightmap)
    {
        CalculateDataForPass(params, kSurfMeta, data, dataResult, lightModel, errors, *analysisCache);
        if (errors.HasErrors())
        {
            outErrors.AddErrorsFrom(errors);
            result += dataResult;
            if (callbacks)
                callbacks->OnIncludeDependencies(GetSurfaceAnalysisOpenedIncludes(analysisCache));
            DestroySurfaceAnalysisCache(analysisCache);
            return;
        }
        result += dataResult;
        if (!errors.HasErrors())
        {
            GenerateMetaPass(result, params, data, errors);
        }
    }

    result += "\n\t// ---- end of surface shader generated code\n";

    outErrors.AddErrorsFrom(errors);
    GetSurfaceAnalysisCacheStats(analysisCache, params.analysisCacheRequests, params.analysisCacheHits, params.astAnalysisCacheRequests, params.astAnalysisCacheHits);
    DestroySurfaceAnalysisCache(analysisCache);
}
