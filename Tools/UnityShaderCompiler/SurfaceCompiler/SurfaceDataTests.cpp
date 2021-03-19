#include "UnityPrefix.h"
#include "SurfaceData.h"
#include "SurfaceCompiler.h"
#include "Runtime/Utilities/Word.h"
#include "SurfaceAnalysis.h"
#include "Runtime/Utilities/ArrayUtility.h"
#include "../Utilities/ShaderImportUtils.h"
#include "../Utilities/ShaderImportErrors.h"
#include "../Utilities/ProcessIncludes.h"

using core::string;


#if ENABLE_UNIT_TESTS
#include "Runtime/Testing/Testing.h"

UNIT_TEST_SUITE(SurfaceData)
{
    struct SurfaceDataFixture
    {
        SurfaceDataFixture()
        {
            params.p.entryName[kProgramSurface] = "surf";
            params.o.lighting = "Lambert";
            params.shaderAPIs = (1 << kShaderCompPlatformD3D11);
            params.includeContext.includePaths.push_back(g_IncludesPath + "/CGIncludes");
            analysisCache = CreateSurfaceAnalysisCache(params.includeContext);
        }

        ~SurfaceDataFixture()
        {
            DestroySurfaceAnalysisCache(analysisCache);
        }

        void CalculateSurfaceFunctionData()
        {
            ::CalculateSurfaceFunctionData(params, keywords, outF, res, errors, *analysisCache);
        }

        void CalculateSurfacePassData(SurfacePass passType, bool lightNeedsViewDir = false)
        {
            ::CalculateSurfacePassData(params, keywords, passType, outF, out, res, lightNeedsViewDir, errors, *analysisCache);
            ::CheckVertexInputRequirements(params, outF, out, errors);
        }

        SurfaceParams params;
        SurfaceFunctionData outF;
        SurfacePassData out;
        core::string res;
        ShaderImportErrors errors;
        StringArray keywords;
        SurfaceAnalysisCache* analysisCache;
    };


    TEST_FIXTURE(SurfaceDataFixture, ExtraRightCurlyBraceInSource_ParsesWithCorrectError)
    {
        params.source =
            "void surf () {}}}}}}}}}\n";
        CalculateSurfaceFunctionData();
        CHECK(errors.HasErrorsOrWarnings());
        const ShaderImportErrors::ErrorContainer& errorList = errors.GetErrors();
        CHECK_EQUAL("Unexpected token '}'. Expected one of: typedef const void inline uniform nointerpolation extern shared static volatile row_major column_major struct or a user-defined type", errorList.begin()->message);
    }

    TEST_FIXTURE(SurfaceDataFixture, JustAlbedoOutput_Works)
    {
        params.source =
            "struct Input { float2 uv_MainTex; };\n"
            "void surf (Input IN, inout SurfaceOutput o) {\n"
            "\to.Albedo = 1;\n"
            "}";
        CalculateSurfaceFunctionData();
        CHECK_EQUAL(true, outF.desc.customInputMembers.empty());
        CalculateSurfacePassData(kSurfFwdBase);
        CHECK(!errors.HasErrorsOrWarnings());
        CHECK_EQUAL(true, out.texcoords.empty());

        CHECK_EQUAL(false, out.needsWorldReflection);
        CHECK_EQUAL(false, out.needsWorldNormal);
        CHECK_EQUAL(false, out.needsScreenPosition);
        CHECK_EQUAL(false, out.needsViewDir);
        CHECK_EQUAL(false, out.needsWorldPos);
        CHECK_EQUAL(false, out.needsWorldViewDir);
        CHECK_EQUAL(false, out.lightNeedsWorldViewDir);
        CHECK_EQUAL(false, out.needsVertexColor);
        CHECK_EQUAL(false, out.needsVFace);
        CHECK_EQUAL(false, outF.writesNormal);
        CHECK_EQUAL(false, outF.writesSpecular);
        CHECK_EQUAL(false, outF.writesOcclusion);
        CHECK_EQUAL(false, out.useTangentSpace);
        CHECK_EQUAL(false, outF.writesEmission);
        CHECK_EQUAL(false, out.readsNormal);
    }

    TEST_FIXTURE(SurfaceDataFixture, WritesNormalPerPixel_Works)
    {
        params.source =
            "struct Input { float2 uv_MainTex; };\n"
            "void surf (Input IN, inout SurfaceOutput o) {\n"
            "\to.Normal = half3(1,2,3);\n"
            "}";
        CalculateSurfaceFunctionData();
        CHECK_EQUAL(true, outF.desc.customInputMembers.empty());
        CalculateSurfacePassData(kSurfFwdBase);
        CHECK(!errors.HasErrorsOrWarnings());
        CHECK_EQUAL(false, out.needsWorldReflection);
        CHECK_EQUAL(false, out.needsScreenPosition);
        CHECK_EQUAL(false, out.needsViewDir);
        CHECK_EQUAL(false, out.needsWorldViewDir);
        CHECK_EQUAL(false, out.needsWorldPos);
        CHECK_EQUAL(false, out.lightNeedsWorldViewDir);
        CHECK_EQUAL(false, out.needsVertexColor);
        CHECK_EQUAL(true, outF.writesNormal); // yes
        CHECK_EQUAL(false, outF.writesSpecular);
        CHECK_EQUAL(false, outF.writesOcclusion);
        CHECK_EQUAL(true, out.useTangentSpace); // yes
        CHECK_EQUAL(false, outF.writesEmission);
        CHECK_EQUAL(false, out.readsNormal);
    }

    TEST_FIXTURE(SurfaceDataFixture, WritesNormalPerPixel_WithSmallFloats_Works)
    {
        params.source =
            "struct Input { float2 uv_MainTex; };\n"
            "void surf (Input IN, inout SurfaceOutput o) {\n"
            "  o.Normal.z += 0.001;\n" // back when Cg was used for surface shader analysis, the number here was too small for it to detect
            "}";
        CalculateSurfaceFunctionData();
        CalculateSurfacePassData(kSurfFwdBase);
        CHECK(!errors.HasErrorsOrWarnings());
        CHECK_EQUAL(true, outF.writesNormal);
        CHECK_EQUAL(true, out.useTangentSpace);
    }

    TEST_FIXTURE(SurfaceDataFixture, WritesNormalPerPixel_FromAnotherFunction_Works)
    {
        params.source =
            "struct Input { float2 uv_MainTex; };\n"
            "void myfunc (inout SurfaceOutput o) { o.Normal = half3(1,2,3); o.Albedo += o.Normal; }\n"
            "void surf (Input IN, inout SurfaceOutput o) {\n"
            "  myfunc(o);\n"
            "}";
        CalculateSurfaceFunctionData();
        CalculateSurfacePassData(kSurfFwdBase);
        CHECK(!errors.HasErrorsOrWarnings());
        CHECK_EQUAL(true, outF.writesNormal);
        CHECK_EQUAL(true, out.useTangentSpace);
        CHECK_EQUAL(false, out.readsNormal);
    }

    TEST_FIXTURE(SurfaceDataFixture, WritesNormalPerPixel_FromAnotherFunction_AsOutParam_Works)
    {
        params.source =
            "struct Input { float2 uv_MainTex; };\n"
            "void myfunc (out fixed3 n) { n = half3(1,2,3); }\n"
            "void surf (Input IN, inout SurfaceOutput o) {\n"
            "  myfunc(o.Normal);\n"
            "}";
        CalculateSurfaceFunctionData();
        CalculateSurfacePassData(kSurfFwdBase);
        CHECK(!errors.HasErrorsOrWarnings());
        CHECK_EQUAL(true, outF.writesNormal);
        CHECK_EQUAL(true, out.useTangentSpace);
        CHECK_EQUAL(false, out.readsNormal);
    }

    // Does not *actually* read or write normal, since it does math operations that are effectively
    // a no-op (here, multiplies by 1).
    TEST_FIXTURE(SurfaceDataFixture, WritesNormalPerPixel_NoopMath_OptimizedAway)
    {
        params.source =
            "struct Input { float2 uv_MainTex; };\n"
            "void surf (Input IN, inout SurfaceOutput o) {\n"
            "  o.Normal *= 1.0;\n"
            "}";
        CalculateSurfaceFunctionData();
        CalculateSurfacePassData(kSurfFwdBase);
        CHECK(!errors.HasErrorsOrWarnings());
        CHECK_EQUAL(false, outF.writesNormal);
        CHECK_EQUAL(false, out.readsNormal);
    }

    TEST_FIXTURE(SurfaceDataFixture, ReadsNormalPerPixel_FromAnotherFunction_Works)
    {
        params.source =
            "struct Input { float2 uv_MainTex; };\n"
            "void myfunc (inout SurfaceOutput o) { o.Albedo = o.Normal; }\n"
            "void surf (Input IN, inout SurfaceOutput o) {\n"
            "  myfunc(o);\n"
            "}";
        CalculateSurfaceFunctionData();
        CalculateSurfacePassData(kSurfFwdBase);
        CHECK(!errors.HasErrorsOrWarnings());
        CHECK_EQUAL(false, outF.writesNormal);
        CHECK_EQUAL(false, out.useTangentSpace);
        CHECK_EQUAL(true, out.readsNormal);
    }

    TEST_FIXTURE(SurfaceDataFixture, WritesNormalAndSpec_IntoCustomOutputVarName)
    {
        params.source =
            "struct Input { float2 uv_MainTex; };\n"
            "void surf (Input IN, inout SurfaceOutput customOutputVar) {\n"
            "\tcustomOutputVar.Normal = half3(1,2,3);\n"
            "\tcustomOutputVar.Specular = 0.5;\n"
            "}";
        CalculateSurfaceFunctionData();
        CHECK_EQUAL(true, outF.desc.customInputMembers.empty());
        CalculateSurfacePassData(kSurfFwdBase);
        CHECK(!errors.HasErrorsOrWarnings());
        CHECK_EQUAL(false, out.needsWorldReflection);
        CHECK_EQUAL(false, out.needsScreenPosition);
        CHECK_EQUAL(false, out.needsViewDir);
        CHECK_EQUAL(false, out.needsWorldPos);
        CHECK_EQUAL(false, out.needsWorldViewDir);
        CHECK_EQUAL(false, out.lightNeedsWorldViewDir);
        CHECK_EQUAL(false, out.needsVertexColor);
        CHECK_EQUAL(true, outF.writesNormal); // yes
        CHECK_EQUAL(true, outF.writesSpecular); // yes
        CHECK_EQUAL(false, outF.writesOcclusion);
        CHECK_EQUAL(true, out.useTangentSpace); // yes
        CHECK_EQUAL(false, outF.writesEmission);
        CHECK_EQUAL(false, out.readsNormal);
    }

    TEST_FIXTURE(SurfaceDataFixture, WritesNormal_WithCustomOutputStruct)
    {
        params.source =
            "struct MyOutput { half3 Albedo; half3 Normal; half3 Emission; half Specular; half Alpha; };\n"
            "inline half4 LightingMyLambert (MyOutput s, half3 lightDir, half atten) { return dot (s.Normal, lightDir); }\n"
            "struct Input { float2 uv_MainTex; };\n"
            "void surf (Input IN, inout MyOutput o) {\n"
            "\to.Normal = half3(1,2,3);\n"
            "}";
        params.o.lighting = "MyLambert";
        CalculateSurfaceFunctionData();
        CHECK_EQUAL(true, outF.desc.customInputMembers.empty());
        CalculateSurfacePassData(kSurfFwdBase);
        CHECK(!errors.HasErrorsOrWarnings());
        CHECK_EQUAL(outF.desc.outputType, "MyOutput");
        CHECK_EQUAL(false, out.needsWorldReflection);
        CHECK_EQUAL(false, out.needsScreenPosition);
        CHECK_EQUAL(false, out.needsViewDir);
        CHECK_EQUAL(false, out.needsWorldPos);
        CHECK_EQUAL(false, out.needsWorldViewDir);
        CHECK_EQUAL(false, out.lightNeedsWorldViewDir);
        CHECK_EQUAL(false, out.needsVertexColor);
        CHECK_EQUAL(true, outF.writesNormal); // yes
        CHECK_EQUAL(false, outF.writesOcclusion);
        CHECK_EQUAL(true, out.useTangentSpace); // yes
        CHECK_EQUAL(false, outF.writesEmission);
        CHECK_EQUAL(false, out.readsNormal);
    }

    TEST_FIXTURE(SurfaceDataFixture, NeedsWorldPosInput_ViaDerivativeInstructions_ForNormal_Works)
    {
        params.source =
            "struct Input { float3 worldPos; };\n"
            "void surf (Input i, inout SurfaceOutput o) {\n"
            "  o.Normal = ddx(i.worldPos);\n"
            "}";
        CalculateSurfaceFunctionData();
        CalculateSurfacePassData(kSurfFwdBase);
        CHECK(!errors.HasErrorsOrWarnings());
        CHECK_EQUAL(true, out.needsWorldPos);
        CHECK_EQUAL(true, outF.writesNormal);
        CHECK_EQUAL(true, out.useTangentSpace);
    }

    TEST_FIXTURE(SurfaceDataFixture, NeedsWorldPosInput_ViaDerivativeInstructions_ForAlbedo_Works)
    {
        params.source =
            "struct Input { float3 worldPos; };\n"
            "void surf (Input i, inout SurfaceOutput o) {\n"
            "  o.Albedo = ddx(i.worldPos);\n"
            "}";
        CalculateSurfaceFunctionData();
        CalculateSurfacePassData(kSurfFwdBase);
        CHECK(!errors.HasErrorsOrWarnings());
        CHECK_EQUAL(true, out.needsWorldPos);
        CHECK_EQUAL(false, outF.writesNormal);
        CHECK_EQUAL(false, out.useTangentSpace);
    }

    TEST_FIXTURE(SurfaceDataFixture, NeedsScreenPosAndVertexColor_Works)
    {
        params.source =
            "struct Input { float2 uv_MainTex; float3 screenPos; float4 myColor : COLOR; };\n"
            "void surf (Input IN, inout SurfaceOutput o) {\n"
            "\to.Albedo = IN.screenPos.x + IN.myColor.x;\n"
            "}";
        CalculateSurfaceFunctionData();
        CHECK_EQUAL(true, outF.desc.customInputMembers.empty());
        CalculateSurfacePassData(kSurfFwdBase);
        CHECK(!errors.HasErrorsOrWarnings());
        CHECK_EQUAL(false, out.needsWorldReflection);
        CHECK_EQUAL(true, out.needsScreenPosition); // yes
        CHECK_EQUAL(false, out.needsViewDir);
        CHECK_EQUAL(false, out.needsWorldPos);
        CHECK_EQUAL(false, out.needsWorldViewDir);
        CHECK_EQUAL(false, out.lightNeedsWorldViewDir);
        CHECK_EQUAL(true, out.needsVertexColor); // yes
        CHECK_EQUAL("myColor", out.vertexColorField);
        CHECK_EQUAL(false, outF.writesNormal);
        CHECK_EQUAL(false, out.useTangentSpace);
        CHECK_EQUAL(false, outF.writesEmission);
        CHECK_EQUAL(false, outF.writesOcclusion);
        CHECK_EQUAL(false, out.readsNormal);
        out = SurfacePassData();
        CalculateSurfacePassData(kSurfFwdAdd);
        CHECK(!errors.HasErrorsOrWarnings());
        CHECK_EQUAL(false, out.needsWorldReflection);
        CHECK_EQUAL(true, out.needsScreenPosition); // yes
    }

    TEST_FIXTURE(SurfaceDataFixture, VFaceInputSemantic_Works)
    {
        params.target.m_BaseRequirements = kShaderRequireShaderModel30;
        params.source =
            "struct Input { float myface : VFACE; };\n"
            "void surf (Input IN, inout SurfaceOutput o) {\n"
            "\to.Albedo = IN.myface;\n"
            "}";
        CalculateSurfaceFunctionData();
        CHECK_EQUAL(true, outF.desc.customInputMembers.empty());
        CalculateSurfacePassData(kSurfFwdBase);
        CHECK(!errors.HasErrorsOrWarnings());
        CHECK_EQUAL(true, out.needsVFace);
        CHECK_EQUAL("myface", out.vfaceField);
    }

    TEST_FIXTURE(SurfaceDataFixture, VFaceInputSemantic_WithoutTarget30_ReportsError)
    {
        params.target.m_BaseRequirements = kShaderRequireShaderModel20;
        params.source =
            "struct Input { float myface : VFACE; };\n"
            "void surf (Input IN, inout SurfaceOutput o) {\n"
            "\to.Albedo = IN.myface;\n"
            "}";
        CalculateSurfaceFunctionData();
        CHECK_EQUAL(true, outF.desc.customInputMembers.empty());
        CalculateSurfacePassData(kSurfFwdBase);
        CHECK(errors.HasErrorsOrWarnings());
        CHECK_EQUAL(1, errors.GetErrors().size());
    }

    TEST_FIXTURE(SurfaceDataFixture, NeedsWorldReflection_Works)
    {
        params.source =
            "struct Input { float2 uv_MainTex; float3 worldRefl; };\n"
            "void surf (Input IN, inout SurfaceOutput o) {\n"
            "\to.Albedo = IN.worldRefl;\n"
            "}";
        CalculateSurfaceFunctionData();
        CHECK_EQUAL(true, outF.desc.customInputMembers.empty());
        CalculateSurfacePassData(kSurfFwdBase);
        CHECK(!errors.HasErrorsOrWarnings());
        CHECK_EQUAL(true, out.needsWorldReflection); // yes
        CHECK_EQUAL(false, out.needsScreenPosition);
        CHECK_EQUAL(false, out.needsViewDir);
        CHECK_EQUAL(false, out.needsWorldPos);
        CHECK_EQUAL(false, out.needsWorldViewDir);
        CHECK_EQUAL(false, out.lightNeedsWorldViewDir);
        CHECK_EQUAL(false, out.needsVertexColor);
        CHECK_EQUAL(false, outF.writesNormal);
        CHECK_EQUAL(false, outF.writesOcclusion);
        CHECK_EQUAL(false, out.useTangentSpace);
        CHECK_EQUAL(false, outF.writesEmission);
        CHECK_EQUAL(false, out.readsNormal);
    }

    TEST_FIXTURE(SurfaceDataFixture, NeedsWorldNormal_Works)
    {
        params.source =
            "struct Input { float3 worldNormal; };\n"
            "void surf (Input IN, inout SurfaceOutput o) {\n"
            "\to.Albedo = IN.worldNormal;\n"
            "}";
        CalculateSurfaceFunctionData();
        CHECK_EQUAL(true, outF.desc.customInputMembers.empty());
        CalculateSurfacePassData(kSurfFwdBase);
        CHECK(!errors.HasErrorsOrWarnings());
        CHECK_EQUAL(false, out.needsWorldReflection);
        CHECK_EQUAL(true, out.needsWorldNormal); // yes
        CHECK_EQUAL(false, out.useTangentSpace);
    }

    TEST_FIXTURE(SurfaceDataFixture, NeedsViewDirection_Works)
    {
        params.source =
            "struct Input { float3 viewDir; };\n"
            "void surf (Input IN, inout SurfaceOutput o) {\n"
            "\to.Albedo = IN.viewDir;\n"
            "}";
        CalculateSurfaceFunctionData();
        CHECK_EQUAL(true, outF.desc.customInputMembers.empty());
        CalculateSurfacePassData(kSurfFwdBase);
        CHECK(!errors.HasErrorsOrWarnings());
        CHECK_EQUAL(false, out.needsWorldReflection);
        CHECK_EQUAL(true, out.needsViewDir); // yes
        CHECK_EQUAL(false, out.needsWorldViewDir);
        CHECK_EQUAL(false, out.useTangentSpace);
    }

    TEST_FIXTURE(SurfaceDataFixture, NeedsWorldViewDirection_Works)
    {
        params.source =
            "struct Input { float3 worldViewDir; };\n"
            "void surf (Input IN, inout SurfaceOutput o) {\n"
            "\to.Albedo = IN.worldViewDir;\n"
            "}";
        CalculateSurfaceFunctionData();
        CHECK_EQUAL(true, outF.desc.customInputMembers.empty());
        CalculateSurfacePassData(kSurfFwdBase);
        CHECK(!errors.HasErrorsOrWarnings());
        CHECK_EQUAL(false, out.needsWorldReflection);
        CHECK_EQUAL(false, out.needsViewDir);
        CHECK_EQUAL(true, out.needsWorldViewDir); // yes
        CHECK_EQUAL(false, out.useTangentSpace);
    }

    TEST_FIXTURE(SurfaceDataFixture, NeedsPerPixelWorldReflection_Works)
    {
        params.source =
            "struct Input { float2 uv_MainTex; float3 worldRefl; INTERNAL_DATA };\n"
            "void surf (Input IN, inout SurfaceOutput o) {\n"
            "\to.Normal = float3(1,2,3);\n"
            "\tfloat3 worldRefl = WorldReflectionVector (IN, o.Normal);\n"
            "\to.Albedo = worldRefl;\n"
            "}";
        CalculateSurfaceFunctionData();
        CHECK_EQUAL(true, outF.desc.customInputMembers.empty());
        CalculateSurfacePassData(kSurfFwdBase);
        CHECK(!errors.HasErrorsOrWarnings());
        CHECK_EQUAL(true, out.needsWorldReflection); // yes
        CHECK_EQUAL(false, out.needsScreenPosition);
        CHECK_EQUAL(false, out.needsViewDir);
        CHECK_EQUAL(false, out.needsWorldPos);
        CHECK_EQUAL(false, out.needsWorldViewDir);
        CHECK_EQUAL(false, out.lightNeedsWorldViewDir);
        CHECK_EQUAL(false, out.needsVertexColor);
        CHECK_EQUAL(true, outF.writesNormal); // yes
        CHECK_EQUAL(true, out.useTangentSpace); // yes
        CHECK_EQUAL(false, outF.writesEmission);
        CHECK_EQUAL(false, outF.writesOcclusion);
        CHECK_EQUAL(false, out.readsNormal);
    }

    // when we were using CG for analysis, it was (rightfully so, as by spec) considering d3d9 semantics as case insensitive
    // on the other hand HLSL bytecode has all semantics case-sensitive
    // make sure that data analysis is backwards compatible in that regard
    // NB: it might make sense to add more checks, like for texcoord

    TEST_FIXTURE(SurfaceDataFixture, NeedsVertexColorNotUppercase_Works)
    {
        params.source =
            "struct Input { float2 uv_MainTex; float4 myColor : Color; };\n"
            "void surf (Input IN, inout SurfaceOutput o) {\n"
            "\to.Albedo = IN.myColor.x;\n"
            "}";
        CalculateSurfaceFunctionData();
        CalculateSurfacePassData(kSurfFwdBase);
        CHECK(!errors.HasErrorsOrWarnings());
        CHECK_EQUAL(true, out.needsVertexColor); // yes
        CHECK_EQUAL("myColor", out.vertexColorField);
    }

    TEST_FIXTURE(SurfaceDataFixture, NeedsPerPixelWorldReflection_ForEmission_Works)
    {
        params.source =
            "struct Input { float2 uv_MainTex; float3 worldRefl; INTERNAL_DATA };\n"
            "void surf (Input IN, inout SurfaceOutput o) {\n"
            "\tfloat3 worldRefl = WorldReflectionVector (IN, o.Normal);\n"
            "\to.Emission = worldRefl.x;\n"
            "}";
        CalculateSurfaceFunctionData();
        CHECK_EQUAL(true, outF.desc.customInputMembers.empty());
        CHECK_EQUAL(true, outF.writesEmission); // yes
        CHECK(!errors.HasErrorsOrWarnings());

        out = SurfacePassData();
        CalculateSurfacePassData(kSurfFwdBase);
        CHECK(!errors.HasErrorsOrWarnings());
        CHECK_EQUAL(true, out.needsWorldReflection); // yes
        CHECK_EQUAL(false, out.useTangentSpace);

        out = SurfacePassData();
        CalculateSurfacePassData(kSurfFwdAdd);
        CHECK(!errors.HasErrorsOrWarnings());
        CHECK_EQUAL(false, out.needsWorldReflection);
        CHECK_EQUAL(false, out.useTangentSpace);

        out = SurfacePassData();
        CalculateSurfacePassData(kSurfPrepassBase);
        CHECK(!errors.HasErrorsOrWarnings());
        CHECK_EQUAL(false, out.needsWorldReflection);
        CHECK_EQUAL(false, out.useTangentSpace);

        out = SurfacePassData();
        CalculateSurfacePassData(kSurfPrepassFinal);
        CHECK(!errors.HasErrorsOrWarnings());
        CHECK_EQUAL(true, out.needsWorldReflection); // yes
        CHECK_EQUAL(false, out.useTangentSpace);
    }

    TEST_FIXTURE(SurfaceDataFixture, NeedsPerPixelWorldReflection_ForEmissionAlpha_Works)
    {
        params.source =
            "struct Input { float2 uv_MainTex; float3 worldRefl; INTERNAL_DATA };\n"
            "void surf (Input IN, inout SurfaceOutput o) {\n"
            "\tfloat3 worldRefl = WorldReflectionVector (IN, o.Normal);\n"
            "\to.Emission = worldRefl.x;\n"
            "\to.Alpha = worldRefl.y;\n"
            "}";
        CalculateSurfaceFunctionData();
        CHECK_EQUAL(true, outF.desc.customInputMembers.empty());
        CHECK_EQUAL(true, outF.writesEmission); // yes

        out = SurfacePassData();
        CalculateSurfacePassData(kSurfFwdBase);
        CHECK(!errors.HasErrorsOrWarnings());
        CHECK_EQUAL(true, out.needsWorldReflection); // yes

        out = SurfacePassData();
        CalculateSurfacePassData(kSurfFwdAdd);
        CHECK(!errors.HasErrorsOrWarnings());
        CHECK_EQUAL(false, out.needsWorldReflection);

        out = SurfacePassData();
        CalculateSurfacePassData(kSurfPrepassBase);
        CHECK(!errors.HasErrorsOrWarnings());
        CHECK_EQUAL(false, out.needsWorldReflection);

        out = SurfacePassData();
        CalculateSurfacePassData(kSurfPrepassFinal);
        CHECK(!errors.HasErrorsOrWarnings());
        CHECK_EQUAL(true, out.needsWorldReflection); // yes
    }

    TEST_FIXTURE(SurfaceDataFixture, TwoTexCoords_ArePackedIntoOne)
    {
        params.source =
            "struct Input { float2 uv_MainTex; float2 uv_BumpMap; };\n"
            "void surf (Input IN, inout SurfaceOutput o) {\n"
            "\to.Albedo = IN.uv_MainTex.x + IN.uv_BumpMap.x;\n"
            "}";
        CalculateSurfaceFunctionData();
        CalculateSurfacePassData(kSurfFwdBase);
        CHECK(!errors.HasErrorsOrWarnings());
        CHECK_EQUAL(2, out.texcoords.size());
        CHECK_EQUAL(1, out.packedTC.packed.size());
        CHECK_EQUAL(kPrecFloat, out.packedTC.packed[0].precision);
    }

    TEST_FIXTURE(SurfaceDataFixture, TexCoords_WithDifferentPrecision_AreNotPacked)
    {
        // this tests uses float2, half2, fixed2, but Cg in analysis mode does not distinguish between
        // fixed2 and half2; so those get packed together
        params.source =
            "struct Input { float2 uv_MainTex; half2 uv_BumpMap; fixed2 uv_Foo; };\n"
            "void surf (Input IN, inout SurfaceOutput o) {\n"
            "\to.Albedo = IN.uv_MainTex.x + IN.uv_BumpMap.x + IN.uv_Foo.x;\n"
            "}";
        CalculateSurfaceFunctionData();
        CalculateSurfacePassData(kSurfFwdBase);
        CHECK(!errors.HasErrorsOrWarnings());
        CHECK_EQUAL(3, out.texcoords.size());
        CHECK_EQUAL(2, out.packedTC.packed.size());
        CHECK_EQUAL(2, out.packedTC.packed[0].usedComps); CHECK_EQUAL(kPrecFloat, out.packedTC.packed[0].precision);
        CHECK_EQUAL(4, out.packedTC.packed[1].usedComps); CHECK_EQUAL(kPrecHalf, out.packedTC.packed[1].precision);
    }

    TEST_FIXTURE(SurfaceDataFixture, CanFigureOutWhichTexcoordsAreUsed)
    {
        params.source =
            "struct Input { float2 uv_MainTex; float2 uv_BumpMap; };\n"
            "void surf (Input IN, inout SurfaceOutput o) {\n"
            "\to.Albedo = IN.uv_MainTex.x;\n"
            "}";
        CalculateSurfaceFunctionData();
        CHECK_EQUAL(true, outF.desc.customInputMembers.empty());
        CalculateSurfacePassData(kSurfFwdBase);
        CHECK(!errors.HasErrorsOrWarnings());
        CHECK_EQUAL(1, out.texcoords.size());
        CHECK_EQUAL("_MainTex", out.texcoords[0].name);
        CHECK_EQUAL(2, GetMemberTypeDimension(out.texcoords[0].type));
    }

    TEST_FIXTURE(SurfaceDataFixture, CanFigureOutWhichTexcoordsAreUsed_WhenOnlyOneComponentUsed)
    {
        params.source =
            "struct Input { float2 uv_MainTex; float2 uv_BumpMap; };\n"
            "void surf (Input IN, inout SurfaceOutput o) {\n"
            "\to.Albedo = IN.uv_BumpMap.x;\n"
            "}";
        CalculateSurfaceFunctionData();
        CHECK_EQUAL(true, outF.desc.customInputMembers.empty());
        CalculateSurfacePassData(kSurfFwdBase);
        CHECK(!errors.HasErrorsOrWarnings());
        CHECK_EQUAL(1, out.texcoords.size());
        CHECK_EQUAL("_BumpMap", out.texcoords[0].name);
        CHECK_EQUAL(2, GetMemberTypeDimension(out.texcoords[0].type));
    }

    TEST_FIXTURE(SurfaceDataFixture, CanFigureOutWhichTexcoordsAreUsed_AlbedoAndNormalCase)
    {
        params.source =
            "struct Input { float2 uv_MainTex; float2 uv_BumpMap; };\n"
            "void surf (Input IN, inout SurfaceOutput o) {\n"
            "\to.Albedo = IN.uv_MainTex.x;\n"
            "\to.Normal = IN.uv_BumpMap.x;\n"
            "}";
        CalculateSurfaceFunctionData();
        CHECK_EQUAL(true, outF.desc.customInputMembers.empty());
        CalculateSurfacePassData(kSurfFwdBase);
        CHECK(!errors.HasErrorsOrWarnings());
        CHECK_EQUAL(2, out.texcoords.size());
    }

    TEST_FIXTURE(SurfaceDataFixture, CanFigureOutWhichTexcoordsAreUsed_AlbedoAndEmissionCase)
    {
        params.source =
            "struct Input { float2 uv_MainTex; float2 uv_BumpMap; };\n"
            "void surf (Input IN, inout SurfaceOutput o) {\n"
            "\to.Albedo = IN.uv_MainTex.x;\n"
            "\to.Emission = IN.uv_BumpMap.x;\n"
            "}";
        CalculateSurfaceFunctionData();
        CHECK_EQUAL(true, outF.desc.customInputMembers.empty());
        CalculateSurfacePassData(kSurfFwdBase);
        CHECK(!errors.HasErrorsOrWarnings());
        CHECK_EQUAL(2, out.texcoords.size());
    }

    TEST_FIXTURE(SurfaceDataFixture, CanFigureOutWhichTexcoordsAreUsed_UV2Case)
    {
        params.source =
            "struct Input { float2 uv2_MainTex; };\n"
            "void surf (Input IN, inout SurfaceOutput o) {\n"
            "\to.Albedo = IN.uv2_MainTex.x;\n"
            "}";
        CalculateSurfaceFunctionData();
        CHECK_EQUAL(true, outF.desc.customInputMembers.empty());
        CalculateSurfacePassData(kSurfFwdBase);
        CHECK(!errors.HasErrorsOrWarnings());
        CHECK_EQUAL(1, out.texcoords.size());
        CHECK_EQUAL("_MainTex", out.texcoords[0].name);
        CHECK_EQUAL(2, GetMemberTypeDimension(out.texcoords[0].type));
        CHECK_EQUAL(1, out.texcoords[0].semanticIndex);
    }

    TEST_FIXTURE(SurfaceDataFixture, CanFigureOutWhichTexcoordsAreUsed_UV3UV4Case)
    {
        params.source =
            "struct Input { float2 uv3_MainTex; float2 uv4_BumpMap; };\n"
            "void surf (Input IN, inout SurfaceOutput o) {\n"
            "\to.Albedo = IN.uv3_MainTex.x;\n"
            "\to.Normal = IN.uv4_BumpMap.y;\n"
            "}";
        CalculateSurfaceFunctionData();
        CHECK_EQUAL(true, outF.desc.customInputMembers.empty());
        CalculateSurfacePassData(kSurfFwdBase);
        CHECK(!errors.HasErrorsOrWarnings());
        CHECK_EQUAL(2, out.texcoords.size());
        CHECK_EQUAL("_MainTex", out.texcoords[0].name);
        CHECK_EQUAL("_BumpMap", out.texcoords[1].name);
        CHECK_EQUAL(2, GetMemberTypeDimension(out.texcoords[0].type));
        CHECK_EQUAL(2, out.texcoords[0].semanticIndex);
        CHECK_EQUAL(2, GetMemberTypeDimension(out.texcoords[1].type));
        CHECK_EQUAL(3, out.texcoords[1].semanticIndex);
    }

    TEST_FIXTURE(SurfaceDataFixture, CanFigureOutWhichTexcoordsAreUsed_WhenShaderKeywordsAffectIt)
    {
        // shader only uses the UV when FOOBAR is on
        params.source =
            "struct Input { float2 uv_MainTex; };\n"
            "void surf (Input IN, inout SurfaceOutput o) {\n"
            "\t#if FOOBAR\n"
            "\to.Albedo = IN.uv_MainTex.x;\n"
            "\t#endif\n"
            "}";
        // no FOOBAR -> no UVs used
        CalculateSurfaceFunctionData();
        CalculateSurfacePassData(kSurfFwdBase);
        CHECK(!errors.HasErrorsOrWarnings());
        CHECK_EQUAL(0, out.texcoords.size());

        // with FOOBAR -> _MainTex UV used
        keywords.push_back("FOOBAR");
        CalculateSurfaceFunctionData();
        CalculateSurfacePassData(kSurfFwdBase);
        CHECK_EQUAL(1, out.texcoords.size());
        CHECK_EQUAL("_MainTex", out.texcoords[0].name);
        CHECK_EQUAL(2, GetMemberTypeDimension(out.texcoords[0].type));
    }

    TEST_FIXTURE(SurfaceDataFixture, CanFigureOutWhichTexcoordsAreUsed_WithCustomOutputStruct)
    {
        params.source =
            "struct MyOutput { half3 Albedo; half3 Normal; half3 Emission; half Specular; half Alpha; half MyGloss; };\n"
            "inline half4 LightingMy (MyOutput s, half3 lightDir, half atten) { return s.MyGloss; }\n"
            "struct Input { float2 uv_MainTex; };\n"
            "void surf (Input IN, inout MyOutput o) {\n"
            "\to.MyGloss = IN.uv_MainTex.x;\n"
            "}";
        params.o.lighting = "My";
        CalculateSurfaceFunctionData();
        CHECK_EQUAL(true, outF.desc.customInputMembers.empty());
        CalculateSurfacePassData(kSurfFwdBase);
        CHECK(!errors.HasErrorsOrWarnings());
        CHECK_EQUAL(outF.desc.outputType, "MyOutput");
        CHECK_EQUAL(1, out.texcoords.size());
    }

    TEST_FIXTURE(SurfaceDataFixture, CanFigureOutWhichTexcoordsAreUsed_ForPrepassBasePass)
    {
        params.source =
            "struct Input { float2 uv_MainTex; float2 uv_BumpMap; float2 uv_Other; };\n"
            "void surf (Input IN, inout SurfaceOutput o) {\n"
            "\to.Albedo = IN.uv_MainTex.x;\n"
            "\to.Normal = IN.uv_BumpMap.x;\n" // only this will be used
            "\to.Alpha = IN.uv_Other.x;\n"
            "}";
        CalculateSurfaceFunctionData();
        CHECK_EQUAL(true, outF.desc.customInputMembers.empty());
        CalculateSurfacePassData(kSurfPrepassBase);
        CHECK(!errors.HasErrorsOrWarnings());
        CHECK_EQUAL(1, out.texcoords.size());
    }

    TEST_FIXTURE(SurfaceDataFixture, CanFigureOutWhichTexcoordsAreUsed_ForPrepassBasePassWithAlphaTest)
    {
        params.source =
            "struct Input { float2 uv_MainTex; float2 uv_BumpMap; float2 uv_Other; };\n"
            "void surf (Input IN, inout SurfaceOutput o) {\n"
            "\to.Albedo = IN.uv_MainTex.x;\n"
            "\to.Normal = IN.uv_BumpMap.x;\n" // this will be used
            "\to.Alpha = IN.uv_Other.x;\n" // this will be used
            "}";
        params.o.mode = kSurfaceModeAlphaTest;
        CalculateSurfaceFunctionData();
        CHECK_EQUAL(true, outF.desc.customInputMembers.empty());
        CalculateSurfacePassData(kSurfPrepassBase);
        CHECK(!errors.HasErrorsOrWarnings());
        CHECK_EQUAL(2, out.texcoords.size());
    }

    TEST_FIXTURE(SurfaceDataFixture, ReadsButNotWritesNormal_Works)
    {
        core::string res;
        params.source =
            "struct Input { float2 uv_MainTex; };\n"
            "void surf (Input IN, inout SurfaceOutput o) {\n"
            "\to.Albedo = o.Normal;\n"
            "}";
        CalculateSurfaceFunctionData();
        CHECK_EQUAL(true, outF.desc.customInputMembers.empty());
        CalculateSurfacePassData(kSurfFwdBase);
        CHECK(!errors.HasErrorsOrWarnings());
        CHECK_EQUAL(0, out.texcoords.size());
        CHECK_EQUAL(true, out.readsNormal);
    }

    TEST_FIXTURE(SurfaceDataFixture, ErrorReporting_EntryPointNotFound)
    {
        core::string res;
        params.source = "float foo() { return 42; }";
        CalculateSurfaceFunctionData();
        CHECK(errors.GetErrors().size() == 1 && errors.GetErrors().begin()->message == "Surface shader function 'surf' not found" && !errors.GetErrors().begin()->warning);
    }

    TEST_FIXTURE(SurfaceDataFixture, ErrorReporting_EntryPointWrongArgs)
    {
        core::string res;
        params.source = "void surf (float a, float b, float c) { }";
        CalculateSurfaceFunctionData();
        CHECK(errors.GetErrors().size() == 1 && errors.GetErrors().begin()->message == "Surface shader function 'surf' has 3 parameters; needs to have 2" && !errors.GetErrors().begin()->warning);
    }

    TEST_FIXTURE(SurfaceDataFixture, ErrorReporting_EntryPointWrongInputModifiers)
    {
        core::string res;
        params.source =
            "struct Input { float a; };\n"
            "void surf (out Input IN, inout SurfaceOutput o) { }";
        CalculateSurfaceFunctionData();
        CHECK(errors.GetErrors().size() == 1 && errors.GetErrors().begin()->message == "Surface shader's input parameter needs to have no or 'in' modifier; found 'out' at 'Input IN'" && !errors.GetErrors().begin()->warning);
    }

    TEST_FIXTURE(SurfaceDataFixture, ErrorReporting_EntryPointWrongOutputModifiers)
    {
        core::string res;
        params.source =
            "struct Input { float a; };\n"
            "void surf (Input IN, SurfaceOutput o) { }";
        CalculateSurfaceFunctionData();
        CHECK(errors.GetErrors().size() == 1 && errors.GetErrors().begin()->message == "Surface shader's output parameter needs to have 'inout' modifier; found no modifier at 'SurfaceOutput o'" && !errors.GetErrors().begin()->warning);
    }

    TEST_FIXTURE(SurfaceDataFixture, ErrorReporting_OnCorrectLineNumbers)
    {
        core::string res;
        params.source =
            "#line 1 \"\"\n"
            "sampler2D _MainTex\n" // missing semicolon, error will be attributed to next line
            "struct Input { float a; };\n"
            "void surf (Input IN, inout SurfaceOutput o) { }";
        CalculateSurfaceFunctionData();
        CHECK(errors.GetErrors().size() >= 1);
        CHECK(!errors.GetErrors().begin()->warning);
        CHECK_EQUAL(2, errors.GetErrors().begin()->line);
        CHECK_EQUAL("Unexpected token struct. Expected one of: ',' ';'", errors.GetErrors().begin()->message);
    }

    TEST_FIXTURE(SurfaceDataFixture, VertexModifier_Works)
    {
        core::string res;
        params.source =
            "void myvert (inout appdata_full v) { }\n"
            "struct Input { float a; };\n"
            "void surf (Input IN, inout SurfaceOutput o) { }";
        params.p.entryName[kProgramVertex] = "myvert";
        CalculateSurfaceFunctionData();
        CHECK_EQUAL(1, outF.desc.customInputMembers.size());
        CHECK(!errors.HasErrorsOrWarnings());
        CHECK_EQUAL(false, outF.desc.vertexModCustomData);
    }

    TEST_FIXTURE(SurfaceDataFixture, CustomInputMembers_ArePacked)
    {
        core::string res;
        params.source =
            "struct Input { float a; float2 b; float3 c; };\n"
            "void myvert (inout appdata_full v, out Input o) { }\n"
            "void surf (Input IN, inout SurfaceOutput o) { }";
        params.p.entryName[kProgramVertex] = "myvert";
        CalculateSurfaceFunctionData();
        CHECK_EQUAL(true, outF.desc.vertexModCustomData);
        CHECK_EQUAL(3, outF.desc.customInputMembers.size());
        const PackedInterpolators& p = outF.desc.packedCustomInputMembers;
        CHECK_EQUAL(2, p.packed.size());
        CHECK_EQUAL("a b", p.packed[0].names);
        CHECK_EQUAL("c", p.packed[1].names);
        CHECK(!errors.HasErrorsOrWarnings());
    }

    TEST_FIXTURE(SurfaceDataFixture, VertexModifier_WithCustomData_Works)
    {
        core::string res;
        params.source =
            "struct Input { float a; };\n"
            "void myvert (inout appdata_full v, out Input o) { }\n"
            "void surf (Input IN, inout SurfaceOutput o) { }";
        params.p.entryName[kProgramVertex] = "myvert";
        CalculateSurfaceFunctionData();
        CHECK(!errors.HasErrorsOrWarnings());
        CHECK_EQUAL(true, outF.desc.vertexModCustomData);
    }

    TEST_FIXTURE(SurfaceDataFixture, VertexModifier_WithCustomVertexStruct_NoTexCoord12_WarnsAndDisablesFlags)
    {
        core::string res;
        // no texcoord1 nor 2 in our vertex struct
        params.source =
            "struct appdata_custom { float4 vertex : POSITION; float3 normal : NORMAL; }; \n"
            "void myvert (inout appdata_custom foo) { }\n"
            "struct Input { float a; };\n"
            "void surf (Input IN, inout SurfaceOutput o) { }";
        params.p.entryName[kProgramVertex] = "myvert";
        CalculateSurfaceFunctionData();
        CHECK(!errors.HasErrorsOrWarnings());
        CHECK_EQUAL("appdata_custom", outF.desc.appdataType);
        CHECK_EQUAL(2, outF.desc.appdataMembers.size());

        CalculateSurfacePassData(kSurfFwdBase);
        // should get one warning about missing texcoord1/texcoord2, and should get noMeta and noDynLightmap set
        CHECK(!errors.HasErrors());
        CHECK(errors.HasErrorsOrWarnings());
        CHECK_EQUAL(2, errors.GetErrors().size());
        CHECK_EQUAL(true, params.o.noLightmap);
        CHECK_EQUAL(true, params.o.noMeta);
        CHECK_EQUAL(true, params.o.noDynLightmap);
    }

    TEST_FIXTURE(SurfaceDataFixture, ErrorReporting_VertexStructIsMissingNormal)
    {
        core::string res;
        params.source =
            "struct appdata_custom { float4 vertex : POSITION; float2 texcoord1 : TEXCOORD1; float2 texcoord2 : TEXCOORD2; }; \n"
            "void myvert (inout appdata_custom foo) { }\n"
            "struct Input { float a; };\n"
            "void surf (Input IN, inout SurfaceOutput o) { }";
        params.p.entryName[kProgramVertex] = "myvert";
        CalculateSurfaceFunctionData();
        CHECK(!errors.HasErrorsOrWarnings());
        CHECK_EQUAL("appdata_custom", outF.desc.appdataType);
        CHECK_EQUAL(3, outF.desc.appdataMembers.size());

        CalculateSurfacePassData(kSurfFwdBase);
        CHECK_EQUAL(1, errors.GetErrors().size());
        CHECK_EQUAL("Surface shader's vertex input struct (appdata_custom) needs to have a 'float3 normal' member", errors.GetErrors().begin()->message);
    }

    TEST_FIXTURE(SurfaceDataFixture, VertexModifier_WithCustomDataAndVertexStruct_Works)
    {
        core::string res;
        params.source =
            "struct appdata_custom { float4 vertex : POSITION; }; \n"
            "struct Input { float a; };\n"
            "void myvert (inout appdata_custom foo, out Input o) { }\n"
            "void surf (Input IN, inout SurfaceOutput o) { }";
        params.p.entryName[kProgramVertex] = "myvert";
        CalculateSurfaceFunctionData();
        CHECK(!errors.HasErrorsOrWarnings());
        CHECK_EQUAL("appdata_custom", outF.desc.appdataType);
        CHECK_EQUAL(true, outF.desc.vertexModCustomData);
    }

    TEST_FIXTURE(SurfaceDataFixture, ErrorReporting_VertexModifierNotFound)
    {
        core::string res;
        params.source =
            "struct Input { float a; };\n"
            "void surf (Input IN, inout SurfaceOutput o) { }";
        params.p.entryName[kProgramVertex] = "myvert";
        CalculateSurfaceFunctionData();
        CHECK(errors.GetErrors().size() == 1 && errors.GetErrors().begin()->message == "Surface shader vertex function 'myvert' not found" && !errors.GetErrors().begin()->warning);
    }

    TEST_FIXTURE(SurfaceDataFixture, ErrorReporting_VertexModifierWrongArgs)
    {
        core::string res;
        params.source =
            "void myvert () { }\n"
            "struct Input { float a; };\n"
            "void surf (Input IN, inout SurfaceOutput o) { }";
        params.p.entryName[kProgramVertex] = "myvert";
        CalculateSurfaceFunctionData();
        CHECK(errors.GetErrors().size() == 1 && errors.GetErrors().begin()->message == "Surface shader vertex function 'myvert' has 0 parameters; needs to have 1 to 2" && !errors.GetErrors().begin()->warning);
    }

    TEST_FIXTURE(SurfaceDataFixture, ErrorReporting_VertexModifierFirstArgWrong)
    {
        core::string res;
        params.source =
            "void myvert (appdata_full v) { }\n"
            "struct Input { float a; };\n"
            "void surf (Input IN, inout SurfaceOutput o) { }";
        params.p.entryName[kProgramVertex] = "myvert";
        CalculateSurfaceFunctionData();
        CHECK(errors.GetErrors().size() == 1 && errors.GetErrors().begin()->message == "Surface shader vertex function 'myvert' #0 parameter has wrong modifier; has no modifier but needs 'inout' at 'appdata_full v'" && !errors.GetErrors().begin()->warning);
    }

    TEST_FIXTURE(SurfaceDataFixture, FinalColorModifier_Works)
    {
        core::string res;
        params.source =
            "struct Input { float a; };\n"
            "void mycolor (Input IN, SurfaceOutput o, inout fixed4 color) { }\n"
            "void surf (Input IN, inout SurfaceOutput o) { }";
        params.o.finalColorModifier = "mycolor";
        CalculateSurfaceFunctionData();
        CHECK_EQUAL(1, outF.desc.customInputMembers.size());
        CHECK(!errors.HasErrorsOrWarnings());
        CHECK_EQUAL(false, outF.desc.vertexModCustomData);
    }

    TEST_FIXTURE(SurfaceDataFixture, ErrorReporting_FinalColorModifierNotFound)
    {
        core::string res;
        params.source =
            "struct Input { float a; };\n"
            "void surf (Input IN, inout SurfaceOutput o) { }";
        params.o.finalColorModifier = "mycolor";
        CalculateSurfaceFunctionData();
        CHECK(errors.GetErrors().size() == 1 && errors.GetErrors().begin()->message == "Surface shader final color function 'mycolor' not found" && !errors.GetErrors().begin()->warning);
    }

    TEST_FIXTURE(SurfaceDataFixture, ErrorReporting_FinalColorModifierWrongArgs)
    {
        core::string res;
        params.source =
            "struct Input { float a; };\n"
            "void mycolor (float a) { }\n"
            "void surf (Input IN, inout SurfaceOutput o) { }";
        params.o.finalColorModifier = "mycolor";
        CalculateSurfaceFunctionData();
        CHECK(errors.GetErrors().size() == 1 && errors.GetErrors().begin()->message == "Surface shader final color function 'mycolor' has 1 parameters; needs to have 3" && !errors.GetErrors().begin()->warning);
    }

    TEST_FIXTURE(SurfaceDataFixture, ErrorReporting_FinalColorModifierWrong1stArgType)
    {
        core::string res;
        params.source =
            "struct Input { float a; };\n"
            "void mycolor (float IN, SurfaceOutput o, inout fixed4 color) { }\n"
            "void surf (Input IN, inout SurfaceOutput o) { }";
        params.o.finalColorModifier = "mycolor";
        CalculateSurfaceFunctionData();
        CHECK(errors.GetErrors().size() == 1 && errors.GetErrors().begin()->message == "Surface shader final color function 'mycolor' #0 parameter of wrong type; has 'float' but needs 'Input'" && !errors.GetErrors().begin()->warning);
    }

    TEST_FIXTURE(SurfaceDataFixture, ErrorReporting_FinalColorModifierWrong1stArgMod)
    {
        core::string res;
        params.source =
            "struct Input { float a; };\n"
            "void mycolor (out Input IN, SurfaceOutput o, inout fixed4 color) { }\n"
            "void surf (Input IN, inout SurfaceOutput o) { }";
        params.o.finalColorModifier = "mycolor";
        CalculateSurfaceFunctionData();
        CHECK(errors.GetErrors().size() == 1 && errors.GetErrors().begin()->message == "Surface shader final color function 'mycolor' #0 parameter has wrong modifier; has 'out' but needs 'in' at 'Input IN'" && !errors.GetErrors().begin()->warning);
    }

    TEST_FIXTURE(SurfaceDataFixture, ErrorReporting_FinalColorModifierWrong3rdArgType)
    {
        core::string res;
        params.source =
            "struct Input { float a; };\n"
            "void mycolor (Input IN, SurfaceOutput o, inout fixed color) { }\n"
            "void surf (Input IN, inout SurfaceOutput o) { }";
        params.o.finalColorModifier = "mycolor";
        CalculateSurfaceFunctionData();
        CHECK(errors.GetErrors().size() == 1 && errors.GetErrors().begin()->message == "Surface shader final color function 'mycolor' #2 parameter of wrong type; has 'half' but needs a 4-vector" && !errors.GetErrors().begin()->warning);
    }

    TEST_FIXTURE(SurfaceDataFixture, ErrorReporting_FinalColorModifierWrong3rdArgMod)
    {
        core::string res;
        params.source =
            "struct Input { float a; };\n"
            "void mycolor (Input IN, SurfaceOutput o, out fixed4 color) { }\n"
            "void surf (Input IN, inout SurfaceOutput o) { }";
        params.o.finalColorModifier = "mycolor";
        CalculateSurfaceFunctionData();
        CHECK(errors.GetErrors().size() == 1 && errors.GetErrors().begin()->message == "Surface shader final color function 'mycolor' #2 parameter has wrong modifier; has 'out' but needs 'inout' at 'half4 color'" && !errors.GetErrors().begin()->warning);
    }

    TEST_FIXTURE(SurfaceDataFixture, FinalPrepassModifier_Works)
    {
        core::string res;
        params.source =
            "struct Input { float a; };\n"
            "void myprepass (Input IN, SurfaceOutput o, inout fixed4 normalSpec) { }\n"
            "void surf (Input IN, inout SurfaceOutput o) { }";
        params.o.finalPrepassModifier = "myprepass";
        CalculateSurfaceFunctionData();
        CHECK_EQUAL(1, outF.desc.customInputMembers.size());
        CHECK(!errors.HasErrorsOrWarnings());
        CHECK_EQUAL(false, outF.desc.vertexModCustomData);
    }

    TEST_FIXTURE(SurfaceDataFixture, ErrorReporting_FinalPrepassModifierNotFound)
    {
        core::string res;
        params.source =
            "struct Input { float a; };\n"
            "void surf (Input IN, inout SurfaceOutput o) { }";
        params.o.finalPrepassModifier = "myprepass";
        CalculateSurfaceFunctionData();
        CHECK(errors.GetErrors().size() == 1 && errors.GetErrors().begin()->message == "Surface shader final prepass function 'myprepass' not found" && !errors.GetErrors().begin()->warning);
    }

    TEST_FIXTURE(SurfaceDataFixture, ErrorReporting_FinalPrepassModifierWrongArgs)
    {
        core::string res;
        params.source =
            "struct Input { float a; };\n"
            "void myprepass (float a) { }\n"
            "void surf (Input IN, inout SurfaceOutput o) { }";
        params.o.finalPrepassModifier = "myprepass";
        CalculateSurfaceFunctionData();
        CHECK(errors.GetErrors().size() == 1 && errors.GetErrors().begin()->message == "Surface shader final prepass function 'myprepass' has 1 parameters; needs to have 3" && !errors.GetErrors().begin()->warning);
    }

    TEST_FIXTURE(SurfaceDataFixture, FinalGBufferModifier_Works)
    {
        core::string res;
        params.source =
            "struct Input { float a; };\n"
            "void mygbuffer (Input IN, SurfaceOutput o, inout half4 diffuse, inout half4 specSmoothness, inout half4 normal, inout half4 emission) { }\n"
            "void surf (Input IN, inout SurfaceOutput o) { }";
        params.o.finalGBufferModifier = "mygbuffer";
        CalculateSurfaceFunctionData();
        CHECK_EQUAL(1, outF.desc.customInputMembers.size());
        CHECK(!errors.HasErrorsOrWarnings());
        CHECK_EQUAL(false, outF.desc.vertexModCustomData);
    }

    TEST_FIXTURE(SurfaceDataFixture, ErrorReporting_FinalGBufferModifierNotFound)
    {
        core::string res;
        params.source =
            "struct Input { float a; };\n"
            "void surf (Input IN, inout SurfaceOutput o) { }";
        params.o.finalGBufferModifier = "mygbuffer";
        CalculateSurfaceFunctionData();
        CHECK(errors.GetErrors().size() == 1 && errors.GetErrors().begin()->message == "Surface shader final gbuffer function 'mygbuffer' not found" && !errors.GetErrors().begin()->warning);
    }

    TEST_FIXTURE(SurfaceDataFixture, ErrorReporting_FinalGBufferModifierWrongArgs)
    {
        core::string res;
        params.source =
            "struct Input { float a; };\n"
            "void mygbuffer (float a) { }\n"
            "void surf (Input IN, inout SurfaceOutput o) { }";
        params.o.finalGBufferModifier = "mygbuffer";
        CalculateSurfaceFunctionData();
        CHECK(errors.GetErrors().size() == 1 && errors.GetErrors().begin()->message == "Surface shader final gbuffer function 'mygbuffer' has 1 parameters; needs to have 6" && !errors.GetErrors().begin()->warning);
    }

    TEST_FIXTURE(SurfaceDataFixture, ErrorReporting_EmptyInputStruct)
    {
        core::string res;
        params.source =
            "struct Input { };\n"
            "void surf (Input IN, inout SurfaceOutput o) { }";
        CalculateSurfaceFunctionData();
        CHECK(errors.GetErrors().size() == 1 && errors.GetErrors().begin()->message == "struct 'Input' has no members" && !errors.GetErrors().begin()->warning);
    }

    TEST_FIXTURE(SurfaceDataFixture, ErrorReporting_SyntaxError_DoesNotReportParserHopelesslyLost)
    {
        core::string res;
        // This used to report two errors: one for proper syntax error at the correct line,
        // and one more unspecified "parser hopelessly lost", with no line number.
        // Make sure only one error is reported with correct location.
        params.source =
            "#line 1\n"
            "struct Input { float a; };\n"
            "void surf (Input IN, inout SurfaceOutput o) {\n"
            " a }\n";
        CalculateSurfaceFunctionData();
        CHECK_EQUAL(1, errors.GetErrors().size());
        CHECK_EQUAL("Unexpected token '}'. Expected one of: ',' ';'", errors.GetErrors().begin()->message);
        CHECK_EQUAL(3, errors.GetErrors().begin()->line);
    }

    TEST(PackedInterpolators_Pack_1_Works)
    {
        PackedInterpolators d;
        MemberDecl tc[] =
        {
            MemberDecl("", "a", "", kTypeFloat2)
        };
        d.Pack(ARRAY_SIZE(tc), tc);
        CHECK(d.packed.size() == 1 && d.packed[0].usedComps == 2);
        CHECK(d.offsets[0].index == 0 && d.offsets[0].offset == 0);
    }


    TEST(PackedInterpolators_Pack_2_Works)
    {
        PackedInterpolators d;
        MemberDecl tc[] =
        {
            MemberDecl("", "a", "", kTypeFloat2),
            MemberDecl("", "b", "", kTypeFloat2),
        };
        d.Pack(ARRAY_SIZE(tc), tc);
        CHECK_EQUAL(1, d.packed.size());
        CHECK_EQUAL(4, d.packed[0].usedComps); CHECK_EQUAL("a b", d.packed[0].names);
        CHECK(d.offsets[0].index == 0 && d.offsets[0].offset == 0);
        CHECK(d.offsets[1].index == 0 && d.offsets[1].offset == 2);
    }


    TEST(PackedInterpolators_Pack_2_DifferentPrecision_AreNotPacked)
    {
        PackedInterpolators d;
        MemberDecl tc[] =
        {
            MemberDecl("", "a", "", kTypeFloat2),
            MemberDecl("", "b", "", kTypeHalf2),
        };
        d.Pack(ARRAY_SIZE(tc), tc);
        CHECK_EQUAL(2, d.packed.size());
        CHECK_EQUAL(2, d.packed[0].usedComps); CHECK_EQUAL("a", d.packed[0].names);
        CHECK_EQUAL(2, d.packed[1].usedComps); CHECK_EQUAL("b", d.packed[1].names);
        CHECK(d.offsets[0].index == 0 && d.offsets[0].offset == 0);
        CHECK(d.offsets[1].index == 1 && d.offsets[1].offset == 0);
    }


    TEST(PackedInterpolators_Pack_3_Works)
    {
        PackedInterpolators d;
        MemberDecl tc[] =
        {
            MemberDecl("", "a", "", kTypeFloat3),
            MemberDecl("", "b", "", kTypeFloat2),
            MemberDecl("", "c", "", kTypeFloat),
        };
        d.Pack(ARRAY_SIZE(tc), tc);
        CHECK_EQUAL(2, d.packed.size());
        CHECK_EQUAL(4, d.packed[0].usedComps); CHECK_EQUAL("a c", d.packed[0].names);
        CHECK_EQUAL(2, d.packed[1].usedComps); CHECK_EQUAL("b", d.packed[1].names);
        CHECK(d.offsets[0].index == 0 && d.offsets[0].offset == 0);
        CHECK(d.offsets[1].index == 1 && d.offsets[1].offset == 0);
        CHECK(d.offsets[2].index == 0 && d.offsets[2].offset == 3);
    }


    TEST(PackedInterpolators_Pack_3_DifferentPrecisionA)
    {
        PackedInterpolators d;
        MemberDecl tc[] =
        {
            MemberDecl("", "a", "", kTypeFloat3),
            MemberDecl("", "b", "", kTypeHalf2),
            MemberDecl("", "c", "", kTypeFloat),
        };
        d.Pack(ARRAY_SIZE(tc), tc);
        CHECK_EQUAL(2, d.packed.size());
        CHECK_EQUAL(4, d.packed[0].usedComps); CHECK_EQUAL("a c", d.packed[0].names);
        CHECK_EQUAL(2, d.packed[1].usedComps); CHECK_EQUAL("b", d.packed[1].names);
        CHECK(d.offsets[0].index == 0 && d.offsets[0].offset == 0);
        CHECK(d.offsets[1].index == 1 && d.offsets[1].offset == 0);
        CHECK(d.offsets[2].index == 0 && d.offsets[2].offset == 3);
    }


    TEST(PackedInterpolators_Pack_3_DifferentPrecisionB)
    {
        PackedInterpolators d;
        MemberDecl tc[] =
        {
            MemberDecl("", "a", "", kTypeHalf3),
            MemberDecl("", "b", "", kTypeFloat2),
            MemberDecl("", "c", "", kTypeFloat),
        };
        d.Pack(ARRAY_SIZE(tc), tc);
        CHECK_EQUAL(2, d.packed.size());
        CHECK_EQUAL(3, d.packed[0].usedComps); CHECK_EQUAL("a", d.packed[0].names);
        CHECK_EQUAL(3, d.packed[1].usedComps); CHECK_EQUAL("b c", d.packed[1].names);
        CHECK(d.offsets[0].index == 0 && d.offsets[0].offset == 0);
        CHECK(d.offsets[1].index == 1 && d.offsets[1].offset == 0);
        CHECK(d.offsets[2].index == 1 && d.offsets[2].offset == 2);
    }

    TEST_FIXTURE(SurfaceDataFixture, BuiltinLightingModels_AreFound)
    {
        core::string res;
        params.source =
            "struct Input { float a; };\n"
            "void surf (Input IN, inout SurfaceOutput o) { }";
        CalculateSurfaceFunctionData();
        CHECK(!errors.HasErrorsOrWarnings());
        CHECK_EQUAL(2, outF.lightModels.size());
        // This should find two built-in lighting models (non PBS ones from Lighting.cginc)
        // Lambert
        const LightingModel& lmL = outF.lightModels["Lambert"];
        CHECK_EQUAL("Lambert", lmL.name);
        CHECK_EQUAL("SurfaceOutput", lmL.outputStructName);
        CHECK_EQUAL((1 << kPathForward) | (1 << kPathPrePass) | (1 << kPathDeferred), lmL.renderPaths);
        CHECK_EQUAL(false, lmL.useViewDir);
        // Blinn Phong
        const LightingModel& lmBP = outF.lightModels["BlinnPhong"];
        CHECK_EQUAL("BlinnPhong", lmBP.name);
        CHECK_EQUAL("SurfaceOutput", lmBP.outputStructName);
        CHECK_EQUAL((1 << kPathForward) | (1 << kPathPrePass) | (1 << kPathDeferred), lmBP.renderPaths);
        CHECK_EQUAL(true, lmBP.useViewDir);
    }

    TEST_FIXTURE(SurfaceDataFixture, BuiltinLightingModels_AreFound_IncludingPBSOnes)
    {
        core::string res;
        // when we use Standard lighting, code should get UnityPBSLighting included automatically,
        // and find Standard & StandardSpecular too.
        params.o.needsPBSLightingInclude = true;
        params.o.lighting = "Standard";

        params.source =
            "struct Input { float a; };\n"
            "void surf (Input IN, inout SurfaceOutputStandard o) { }";
        CalculateSurfaceFunctionData();
        CHECK(!errors.HasErrorsOrWarnings());
        CHECK_EQUAL(4, outF.lightModels.size());
        // Lambert
        const LightingModel& lmL = outF.lightModels["Lambert"];
        CHECK_EQUAL("Lambert", lmL.name);
        CHECK_EQUAL("SurfaceOutput", lmL.outputStructName);
        CHECK_EQUAL((1 << kPathForward) | (1 << kPathPrePass) | (1 << kPathDeferred), lmL.renderPaths);
        CHECK_EQUAL(false, lmL.useViewDir);
        CHECK_EQUAL(false, lmL.isPBS);
        // Blinn Phong
        const LightingModel& lmBP = outF.lightModels["BlinnPhong"];
        CHECK_EQUAL("BlinnPhong", lmBP.name);
        CHECK_EQUAL("SurfaceOutput", lmBP.outputStructName);
        CHECK_EQUAL((1 << kPathForward) | (1 << kPathPrePass) | (1 << kPathDeferred), lmBP.renderPaths);
        CHECK_EQUAL(true, lmBP.useViewDir);
        CHECK_EQUAL(false, lmBP.isPBS);
        // Standard
        const LightingModel& lmStd = outF.lightModels["Standard"];
        CHECK_EQUAL("Standard", lmStd.name);
        CHECK_EQUAL("SurfaceOutputStandard", lmStd.outputStructName);
        CHECK_EQUAL((1 << kPathForward) | (1 << kPathDeferred), lmStd.renderPaths);
        CHECK_EQUAL(true, lmStd.useViewDir);
        CHECK_EQUAL(true, lmStd.hasGI);
        CHECK_EQUAL(true, lmStd.useGIForward);
        CHECK_EQUAL(true, lmStd.useGIDeferred);
        CHECK_EQUAL(true, lmStd.isPBS);
        // Standard Specular
        const LightingModel& lmStdSpec = outF.lightModels["StandardSpecular"];
        CHECK_EQUAL("StandardSpecular", lmStdSpec.name);
        CHECK_EQUAL("SurfaceOutputStandardSpecular", lmStdSpec.outputStructName);
        CHECK_EQUAL((1 << kPathForward) | (1 << kPathDeferred), lmStdSpec.renderPaths);
        CHECK_EQUAL(true, lmStdSpec.useViewDir);
        CHECK_EQUAL(true, lmStdSpec.hasGI);
        CHECK_EQUAL(true, lmStdSpec.useGIForward);
        CHECK_EQUAL(true, lmStdSpec.useGIDeferred);
        CHECK_EQUAL(true, lmStdSpec.isPBS);
    }

    TEST_FIXTURE(SurfaceDataFixture, CustomLightingModel_IsFound)
    {
        core::string res;
        params.source =
            "half4 LightingFoo (SurfaceOutput s, half3 lightDir, half atten) { return 0; }\n"
            "half4 LightingFoo_PrePass (SurfaceOutput s, half4 light) { return 0; }\n"
            "struct Input { float a; };\n"
            "void surf (Input IN, inout SurfaceOutput o) { }";
        CalculateSurfaceFunctionData();
        CHECK(!errors.HasErrorsOrWarnings());
        // Foo
        const LightingModel& lm = outF.lightModels["Foo"];
        CHECK_EQUAL("Foo", lm.name);
        CHECK_EQUAL("SurfaceOutput", lm.outputStructName);
        CHECK_EQUAL((1 << kPathForward) | (1 << kPathPrePass), lm.renderPaths);
        CHECK_EQUAL(false, lm.useViewDir);
        CHECK_EQUAL(false, lm.UseViewDirLightmap());
        CHECK_EQUAL(false, lm.hasCustomSingleLightmap || lm.hasCustomDualLightmaps || lm.hasCustomDirLightmaps);
    }

    TEST_FIXTURE(SurfaceDataFixture, CustomLightingModel_NoPrepass_IsFound)
    {
        core::string res;
        params.source =
            "half4 LightingFoo (SurfaceOutput s, half3 lightDir, half atten) { return 0; }\n"
            "struct Input { float a; };\n"
            "void surf (Input IN, inout SurfaceOutput o) { }";
        CalculateSurfaceFunctionData();
        CHECK(!errors.HasErrorsOrWarnings());
        CHECK_EQUAL((1 << kPathForward), outF.lightModels["Foo"].renderPaths);
    }

    TEST_FIXTURE(SurfaceDataFixture, CustomLightingModel_NoForward_IsFound)
    {
        core::string res;
        params.source =
            "half4 LightingFoo_PrePass (SurfaceOutput s, half4 light) { return 0; }\n"
            "struct Input { float a; };\n"
            "void surf (Input IN, inout SurfaceOutput o) { }";
        CalculateSurfaceFunctionData();
        CHECK(!errors.HasErrorsOrWarnings());
        CHECK_EQUAL((1 << kPathPrePass), outF.lightModels["Foo"].renderPaths);
    }

    TEST_FIXTURE(SurfaceDataFixture, CustomLightmapModel_IsFound)
    {
        core::string res;
        params.source =
            "half4 LightingFoo (SurfaceOutput s, half3 lightDir, half3 viewDir, half atten) { return 0; }\n"
            "half4 LightingFoo_PrePass (SurfaceOutput s, half4 light) { return 0; }\n"
            "half4 LightingFoo_SingleLightmap (SurfaceOutput s, fixed4 color, half3 viewDir) { return 0; }\n"
            "half4 LightingFoo_DualLightmap (SurfaceOutput s, fixed4 nearColor, fixed4 farColor, half nearFarFade, half3 viewDir) { return 0; }\n"
            "half4 LightingFoo_DirLightmap (SurfaceOutput s, fixed4 color, fixed4 scale, half3 viewDir, bool surfFuncWritesNormal, out half3 specColor) { specColor = 0; return 0; }\n"
            "struct Input { float a; };\n"
            "void surf (Input IN, inout SurfaceOutput o) { }";
        CalculateSurfaceFunctionData();
        CHECK(!errors.HasErrorsOrWarnings());
        // Foo
        const LightingModel& lm = outF.lightModels["Foo"];
        CHECK_EQUAL("Foo", lm.name);
        CHECK_EQUAL((1 << kPathForward) | (1 << kPathPrePass) | (1 << kPathDirLightmap), lm.renderPaths);
        CHECK_EQUAL(true, lm.useViewDir);
        CHECK_EQUAL(true, lm.UseViewDirLightmap());
        CHECK_EQUAL(true, lm.hasCustomSingleLightmap && lm.hasCustomDualLightmaps && lm.hasCustomDirLightmaps);
    }

    TEST_FIXTURE(SurfaceDataFixture, CustomLightmapModel_NoViewDirection_IsFound)
    {
        core::string res;
        params.source =
            "half4 LightingFoo (SurfaceOutput s, half3 lightDir, half atten) { return 0; }\n"
            "half4 LightingFoo_PrePass (SurfaceOutput s, half4 light) { return 0; }\n"
            "half4 LightingFoo_SingleLightmap (SurfaceOutput s, fixed4 color) { return 0; }\n"
            "half4 LightingFoo_DualLightmap (SurfaceOutput s, fixed4 nearColor, fixed4 farColor, half nearFarFade) { return 0; }\n"
            "half4 LightingFoo_DirLightmap (SurfaceOutput s, fixed4 color, fixed4 scale, bool surfFuncWritesNormal) { return 0; }\n"
            "struct Input { float a; };\n"
            "void surf (Input IN, inout SurfaceOutput o) { }";
        CalculateSurfaceFunctionData();
        CHECK(!errors.HasErrorsOrWarnings());
        // Foo
        const LightingModel& lm = outF.lightModels["Foo"];
        CHECK_EQUAL("Foo", lm.name);
        CHECK_EQUAL((1 << kPathForward) | (1 << kPathPrePass) | (1 << kPathDirLightmap), lm.renderPaths);
        CHECK_EQUAL(false, lm.useViewDir);
        CHECK_EQUAL(false, lm.UseViewDirLightmap());
        CHECK_EQUAL(true, lm.hasCustomSingleLightmap && lm.hasCustomDualLightmaps && lm.hasCustomDirLightmaps);
    }

    TEST_FIXTURE(SurfaceDataFixture, LargeOutputStruct_Works)
    {
        // large output struct; used to not work a while ago
        params.source =
            "struct MyOutput { half3 Albedo; half3 Normal; half3 Emission; half Specular; half Alpha; half Occlusion; half4 Data1; half4 Data2; };\n"
            "inline half4 LightingMy (MyOutput s, half3 lightDir, half atten) { return dot (s.Normal, lightDir); }\n"
            "struct Input { float2 uv_MainTex; };\n"
            "void surf (Input IN, inout MyOutput o) {\n"
            "\to.Albedo = 0.5; o.Normal = 0.3; o.Emission = 0.1; o.Specular = 0.0; o.Alpha = 0.0; o.Occlusion = 0.0;\n"
            "\to.Data1 = 0.0; o.Data2 = 0.0;\n"
            "}";
        params.o.lighting = "My";
        CalculateSurfaceFunctionData();
        CHECK_EQUAL(true, outF.desc.customInputMembers.empty());
        CalculateSurfacePassData(kSurfFwdBase);
        CHECK(!errors.HasErrorsOrWarnings());
        CHECK_EQUAL(outF.desc.outputType, "MyOutput");
        CHECK_EQUAL(true, outF.writesNormal); // yes
        CHECK_EQUAL(true, out.useTangentSpace); // yes
        CHECK_EQUAL(true, outF.writesEmission); // yes
        CHECK_EQUAL(true, outF.writesOcclusion); // yes
    }

    TEST_FIXTURE(SurfaceDataFixture, CustomLightingModel_MismatchingOutputTypes_ErrorIsReported)
    {
        core::string res;
        params.source =
            // custom output struct
            "struct MyOutput { half3 Albedo; half3 Normal; half3 Emission; half Alpha; };\n"
            // lighting model with mismatching output structs (SurfaceOutput in one, MyOutput in another)
            // should result in error
            "half4 LightingFoo (SurfaceOutput s, half3 lightDir, half atten) { return 0; }\n"
            "half4 LightingFoo_PrePass (MyOutput s, half4 light) { return 0; }\n"
            "struct Input { float a; };\n"
            "void surf (Input IN, inout SurfaceOutput o) { }";
        CalculateSurfaceFunctionData();
        CHECK_EQUAL(1, errors.GetErrors().size());
        CHECK_EQUAL("Surface shader lighting model 'Foo' has inconsistent output struct types ('MyOutput' vs 'SurfaceOutput')", errors.GetErrors().begin()->message);
    }

    TEST_FIXTURE(SurfaceDataFixture, CustomPBSLightingModel_IsFound)
    {
        core::string res;
        params.o.lighting = "MyStandard";
        params.source =
            // custom PBS lighting model
            "#include \"UnityPBSLighting.cginc\"\n"
            "half4 LightingMyStandard (SurfaceOutputStandard s, half3 viewDir, UnityGI gi) { return 0; }\n"
            "void  LightingMyStandard_GI (SurfaceOutputStandard s, UnityGIInput data, inout UnityGI gi) { }\n"
            "struct Input { float a; };\n"
            "void surf (Input IN, inout SurfaceOutputStandard o) { }";
        CalculateSurfaceFunctionData();
        CHECK(!errors.HasErrorsOrWarnings());
        // check lighting model
        const LightingModel& lm = outF.lightModels["MyStandard"];
        CHECK_EQUAL("MyStandard", lm.name);
        CHECK_EQUAL((1 << kPathForward), lm.renderPaths);
        CHECK_EQUAL(true, lm.useGIForward);
        CHECK_EQUAL(true, lm.hasGI);
        CHECK_EQUAL(true, lm.isPBS);
        CHECK_EQUAL(true, lm.useViewDir); // PBS lighting models always use view direction
    }

    TEST_FIXTURE(SurfaceDataFixture, CustomPBSLightingModel_MissingAmbient_ErrorIsReported)
    {
        core::string res;
        params.o.lighting = "MyStandard";
        params.source =
            // custom PBS lighting model
            "#include \"UnityPBSLighting.cginc\"\n"
            // has forward but no GI function -> error
            "half4 LightingMyStandard (SurfaceOutputStandard s, half3 viewDir, UnityGI gi) { return 0; }\n"
            //"void  LightingMyStandard_GI (SurfaceOutputStandard s, UnityGIInput data, inout UnityGI gi) { }\n"
            "struct Input { float a; };\n"
            "void surf (Input IN, inout SurfaceOutputStandard o) { }";
        CalculateSurfaceFunctionData();
        CHECK_EQUAL(1, errors.GetErrors().size());
        CHECK_EQUAL("Surface shader lighting model 'MyStandard' is missing a GI function (MyStandard_GI)", errors.GetErrors().begin()->message);
    }

    TEST_FIXTURE(SurfaceDataFixture, CustomPBSLightingModel_MissingForward_ErrorIsReported)
    {
        core::string res;
        params.o.lighting = "MyStandard";
        params.source =
            // custom PBS lighting model
            "#include \"UnityPBSLighting.cginc\"\n"
            // has GI function but no forward function -> error
            //"half4 LightingMyStandard (SurfaceOutputStandard s, half3 viewDir, UnityGI gi) { return 0; }\n"
            "void  LightingMyStandard_GI (SurfaceOutputStandard s, UnityGIInput data, inout UnityGI gi) { }\n"
            "struct Input { float a; };\n"
            "void surf (Input IN, inout SurfaceOutputStandard o) { }";
        CalculateSurfaceFunctionData();
        CHECK_EQUAL(1, errors.GetErrors().size());
        CHECK_EQUAL("Surface shader lighting model 'MyStandard' is missing a forward or deferred function (MyStandard)", errors.GetErrors().begin()->message);
    }

    TEST_FIXTURE(SurfaceDataFixture, OutputStructWithNoSpecularField_Works)
    {
        params.source =
            "struct MyOutput { half3 Albedo; half3 Normal; half3 Emission; half Alpha; };\n"
            "inline half4 LightingMy (MyOutput s, half3 lightDir, half atten) { return dot (s.Normal, lightDir); }\n"
            "struct Input { float2 uv_MainTex; };\n"
            "void surf (Input IN, inout MyOutput o) {\n"
            "\to.Albedo = 0.3; o.Emission = 0.2; o.Alpha = 0.0;\n"
            "}";
        params.o.lighting = "My";
        CalculateSurfaceFunctionData();
        CHECK_EQUAL(true, outF.desc.customInputMembers.empty());
        CalculateSurfacePassData(kSurfFwdBase);
        CHECK(!errors.HasErrorsOrWarnings());
        CHECK_EQUAL(outF.desc.outputType, "MyOutput");
        CHECK_EQUAL(true, outF.writesEmission); // yes
        CHECK_EQUAL(false, outF.writesNormal);
        CHECK_EQUAL(false, outF.writesOcclusion);
    }

    TEST_FIXTURE(SurfaceDataFixture, OutputStructWithNonRGBSpecularField_Works)
    {
        params.source =
            "struct MyOutput { half3 Albedo; half3 Normal; half3 Emission; half Alpha; half4 Specular; };\n"
            "inline half4 LightingMy (MyOutput s, half3 lightDir, half atten) { return dot (s.Normal, lightDir); }\n"
            "struct Input { float2 uv_MainTex; };\n"
            "void surf (Input IN, inout MyOutput o) {\n"
            "\to.Albedo = 0.0; o.Specular = 0.0;\n"
            "}";
        params.o.lighting = "My";
        CalculateSurfaceFunctionData();
        CHECK_EQUAL(true, outF.desc.customInputMembers.empty());
        CalculateSurfacePassData(kSurfFwdBase);
        CHECK(!errors.HasErrorsOrWarnings());
        CHECK_EQUAL(outF.desc.outputType, "MyOutput");
        CHECK_EQUAL(false, outF.writesSpecular);
    }

    TEST_FIXTURE(SurfaceDataFixture, WriteToOcclusionOnly_BringsInUsedTexcoord)
    {
        // Used to have a bug where if some texture coordinate (e.g. uv2) was *only* used for Occlusion,
        // then it was not accounted properly in the surface analysis pass since it was not calling
        // the lighting GI function in the analysis shader.
        params.source =
            "#include \"UnityPBSLighting.cginc\"\n"
            "struct Input { float2 uv_MainTex; float2 uv2_Occlusion; };\n"
            "void surf (Input IN, inout SurfaceOutputStandard o) {\n"
            "\to.Occlusion = IN.uv2_Occlusion.x;\n"
            "}";
        params.o.lighting = "Standard";
        CalculateSurfaceFunctionData();
        CalculateSurfacePassData(kSurfFwdBase);
        CHECK(!errors.HasErrorsOrWarnings());

        CHECK_EQUAL(true, outF.writesOcclusion);
        CHECK_EQUAL(1, out.texcoords.size());
        CHECK_EQUAL("_Occlusion", out.texcoords[0].name);
    }

    TEST_FIXTURE(SurfaceDataFixture, WorldReflection_IsProperlyInitialized)
    {
        // Checks if worldRefl is properly initialized even when only DirLightmap lighting is using it and not the forward lighting function.
        // The cubemap sample is there, because with o.Custom = IN.worldRefl.r the compiler would complain about o not being fully initialized in the forward add pass,
        // even though o.Custom isn't used there. I guess that's something we should fix as well.
        params.source =
            "struct MyOutput { half3 Albedo; half3 Normal; half3 Emission; half Alpha; half Specular; float Custom; };\n"
            "inline half4 LightingMy (MyOutput s, half3 lightDir, half atten) { return 0; }\n"
            "inline half4 LightingMy_DirLightmap (MyOutput s, fixed4 color, fixed4 scale, half3 viewDir, bool surfFuncWritesNormal, out half3 specColor) { specColor = 0; return s.Custom.rrrr; }\n"
            "struct Input { float3 worldRefl; };\n"
            "samplerCUBE _Cubemap;\n"
            "void surf (Input IN, inout MyOutput o) {\n"
            "\to.Custom = texCUBE(_Cubemap,IN.worldRefl).r;\n"
            "}";
        params.o.lighting = "My";
        CalculateSurfaceFunctionData();
        CalculateSurfacePassData(kSurfFwdBase);
        CHECK(!errors.HasErrorsOrWarnings());
        CHECK_EQUAL(true, out.needsWorldReflection);
    }

    TEST_FIXTURE(SurfaceDataFixture, LargeInputPlusOutputCounts_Work)
    {
        params.source =
            "struct MyOutput { half3 Albedo; half3 Normal; half3 Emission; half Specular; half4 Gloss; half Alpha; };\n"
            "inline half4 LightingMy (MyOutput s, half3 lightDir, half3 viewDir, half atten) {\n"
            "\treturn s.Albedo.r + s.Normal.r + s.Specular + s.Gloss.r + lightDir.r + viewDir.r + atten;\n"
            "}\n"
            "struct Input { float2 uv_Control; float2 uv_Splat0; float2 uv_Splat1; float2 uv_Splat2; float2 uv_Splat3; float3 worldNormal; };\n"
            "sampler2D _Control; sampler2D _Splat0,_Splat1,_Splat2,_Splat3;\n"
            "void surf (Input IN, inout MyOutput o) {\n"
            "\thalf4 tc = tex2D(_Control,IN.uv_Control);\n"
            "\thalf4 t0 = tex2D(_Splat0,IN.uv_Splat0);\n"
            "\thalf4 t1 = tex2D(_Splat1,IN.uv_Splat1);\n"
            "\thalf4 t2 = tex2D(_Splat2,IN.uv_Splat2);\n"
            "\thalf4 t3 = tex2D(_Splat3,IN.uv_Splat3);\n"
            "\to.Emission = IN.worldNormal;\n"
            "}";
        params.o.lighting = "My";
        CalculateSurfaceFunctionData();
        CHECK_EQUAL(true, outF.desc.customInputMembers.empty());
        CalculateSurfacePassData(kSurfFwdBase, true);
        CHECK(!errors.HasErrorsOrWarnings());
    }

    // Had an issue where during surface analysis step, the shader code was thinking it's using
    // HLSL compiler, instead of Cg that's in fact used.
    TEST_FIXTURE(SurfaceDataFixture, SurfaceAnalysis_AssumesCorrectCompiler)
    {
        params.source =
            "struct Input { float2 uv_MainTex; };\n"
            "void surf (Input IN, inout SurfaceOutput o) {\n"
            "\to.Albedo = 1;\n"
            // update check if surface analysis is switched to another compiler
            "\t#ifndef UNITY_COMPILER_CG\n"
            "\t\terror_should_not_get_here;\n"
            "\t#endif\n"
            "}";
        CalculateSurfaceFunctionData();
        CalculateSurfacePassData(kSurfFwdBase);
        CHECK(!errors.HasErrorsOrWarnings());
    }
}


#endif // ENABLE_UNIT_TESTS
