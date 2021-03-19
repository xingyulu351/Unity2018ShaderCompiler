#include "UnityPrefix.h"
#include "SurfaceCompiler.h"
#include "APIPlugin.h"

#if ENABLE_UNIT_TESTS
#include "SurfaceData.h"
#include "artifacts/generated/Configuration/UnityConfigureVersion.gen.h"
#include "Runtime/Testing/Testing.h"
#include "Editor/Src/Utility/TextUtilities.h"

size_t CountOccurrencesInString(const core::string& source, const core::string& token)
{
    size_t count = 0;
    size_t position = source.find(token);
    while (position != core::string::npos)
    {
        ++count;
        position += token.size();
        position = source.find(token, position);
    }
    return count;
}

UNIT_TEST_SUITE(SurfaceCompiler)
{
    struct SurfaceCompilerFixture
    {
        SurfaceCompilerFixture()
        {
            params.p.entryName[kProgramSurface] = "surf";
            params.o.lighting = "Lambert";
            params.includeContext.includePaths.push_back(g_IncludesPath + "/CGIncludes");
            params.shaderAPIs = ~0;
        }

        void DoTest()
        {
            rawErrors.Clear();
            ProcessSurfaceSnippet(params, res, rawErrors, NULL);
            errors.clear();
            for (ShaderImportErrors::ErrorContainer::const_iterator it = rawErrors.GetErrors().begin(); it != rawErrors.GetErrors().end(); ++it)
                errors.push_back(*it);
        }

        SurfaceParams params;
        core::string res;
        ShaderImportErrors rawErrors;
        std::vector<ShaderImportError> errors;
    };


    TEST_FIXTURE(SurfaceCompilerFixture, SurfaceAnalysis_IncludesInsideFailedIfdefs_AreSkipped)
    {
        // During surface analysis stage, #includes inside failing #ifdefs should not be processed.
        // This tries to include a non-existing file, inside a check that will never be true -
        // everything should work just fine (case 629317).
        params.source =
            "#ifdef CONDITION_THAT_WILL_NEVER_BE_TRUE\n"
            "#include \"NonExistingFile.cginc\"\n"
            "#endif\n"
            "struct Input { float a; };\n"
            "void surf (Input IN, inout SurfaceOutput o) {\n"
            "\to.Albedo = 0;\n"
            "}\n";
        DoTest();
        CHECK(!rawErrors.HasErrorsOrWarnings());
    }

    TEST_FIXTURE(SurfaceCompilerFixture, SurfaceCompiler_InputStruct_IsFullyInitialized_ForDX11)
    {
        // DX11 shader compiler really dislikes uninitialized data.
        // Shader below was getting a:
        //   output parameter 'o' not completely initialized
        // error because forward additive pass is not really using emission,
        // and thus not really initializing viewDir.
        params.source =
            "struct Input { float3 viewDir; };\n"
            "void surf (Input IN, inout SurfaceOutput o) {\n"
            "\to.Emission = IN.viewDir;\n"
            "}\n";
        params.o.noLightmap = true;
        params.shaderAPIs = (1 << kShaderCompPlatformD3D11);
        DoTest();
        CHECK(!rawErrors.HasErrorsOrWarnings());
    }

    TEST_FIXTURE(SurfaceCompilerFixture, SurfaceCompiler_SurfaceFunctionAndLightingModel_MismatchingOutputTypes_ErrorIsReported)
    {
        core::string res;
        params.o.lighting = "Foo";
        params.source =
            // custom output struct and lighting model
            "struct MyOutput { half3 Albedo; half3 Normal; half3 Emission; half Alpha; };\n"
            "half4 LightingFoo (MyOutput s, half3 lightDir, half atten) { return 0; }\n"
            "struct Input { float a; };\n"
            // surface function using a wrong output type (SurfaceOutput instead of MyOutput)
            "void surf (Input IN, inout SurfaceOutput o) { }";
        DoTest();
        CHECK_EQUAL(1, errors.size());
        CHECK_EQUAL("Surface function 'surf' and lighting model 'Foo' have mismatching output types ('SurfaceOutput' vs 'MyOutput')", errors[0].message);
    }

    TEST_FIXTURE(SurfaceCompilerFixture, SurfaceCompiler_ErrorsReportedOnCorrectLines_WhileDoingAnalysisStep)
    {
        params.source =
            "#line 10 \"\"\n"
            "struct Input { float3 viewDir; };\n" // 10
            "aaa;\n" // 11 - error here, will get caught in analysis step
            "void surf (Input IN, inout SurfaceOutput o) {\n"
            "\to.Albedo = 0.0;\n"
            "}\n";
        params.o.noLightmap = true;
        params.shaderAPIs = (1 << kShaderCompPlatformD3D11);
        DoTest();
        CHECK_EQUAL(11, errors[0].line);
        CHECK_EQUAL("", errors[0].file);
    }

    TEST_FIXTURE(SurfaceCompilerFixture, SurfaceCompiler_ErrorsReportedWithCorrectFilenames_WhileDoingAnalysisStep)
    {
        params.source =
            "#line 10 \"testfile\"\n"
            "struct Input { float3 viewDir; };\n" // 10
            "aaa;\n" // 11 - error here, will get caught in analysis step
            "void surf (Input IN, inout SurfaceOutput o) {\n"
            "\to.Albedo = 0.0;\n"
            "}\n";
        params.o.noLightmap = true;
        params.shaderAPIs = (1 << kShaderCompPlatformD3D11);
        DoTest();
        CHECK_EQUAL(11, errors[0].line);
        CHECK_EQUAL("testfile", errors[0].file);
    }

    TEST_FIXTURE(SurfaceCompilerFixture, SurfaceCompiler_ErrorsReported_WithFriendlyMessages)
    {
        params.source =
            "sampler2D _MainTex\n" // missing semicolon, error will be attributed to next line
            "struct Input { float a; };\n"
            "void surf (Input IN, inout SurfaceOutput o) { }";
        params.o.noLightmap = true;
        params.shaderAPIs = (1 << kShaderCompPlatformD3D11);
        DoTest();
        CHECK_EQUAL("Unexpected token struct. Expected one of: ',' ';'", errors[0].message);
    }

    TEST_FIXTURE(SurfaceCompilerFixture, SurfaceCompiler_ErrorsReportedOnCorrectLines_WhileCompilingDummyFunction)
    {
        params.source =
            "#line 10 \"\"\n"
            "struct Input { float3 viewDir; };\n" // 10
            "void surf (Input IN, inout SurfaceOutput o) {\n" // 11
            "\taaa = 0;\n" // 12 - error here, will get caught while compiling dummy function
            "\to.Albedo = 0.0;\n"
            "}\n";
        params.o.noLightmap = true;
        params.shaderAPIs = (1 << kShaderCompPlatformD3D11);
        DoTest();
        CHECK_EQUAL(12, errors[0].line);
        CHECK_EQUAL("", errors[0].file);
    }

    TEST_FIXTURE(SurfaceCompilerFixture, SurfaceCompiler_ShaderFeature_VariantGuards_Work)
    {
        params.shaderAPIs = 0;
        params.o.renderPaths = (1 << kPathForward);
        params.o.noForwardAdd = true;
        // single shader_feature, A
        params.o.pragmaDirectives = "#pragma shader_feature A\n";
        params.source =
            "struct Input { float a; };\n"
            "void surf (Input IN, inout SurfaceOutput o) {}\n";
        DoTest();
        // code should have two guards: (!A) and (A)
        size_t pos1 = res.find("#if !defined(A)\n// Surface shader code");
        CHECK(pos1 != core::string::npos);
        size_t pos2 = res.find("#if defined(A)\n// Surface shader code");
        CHECK(pos2 != core::string::npos);
        // Check how many surface variants were analyzed (instancing does not kick in since shaderAPIs is 0)
        // Compile: 4 unique (function, fwdbase lighting, fwdbase no lighting, meta), x2 total for the variants.
        CHECK_EQUAL(8, params.analysisCacheRequests);
        CHECK_EQUAL(4, params.analysisCacheHits);
        // AST: 1 unique, 2x total for variants.
        CHECK_EQUAL(2, params.astAnalysisCacheRequests);
        CHECK_EQUAL(1, params.astAnalysisCacheHits);
    }

    TEST_FIXTURE(SurfaceCompilerFixture, SurfaceCompiler_TwoShaderFeatures_VariantGuards_Work)
    {
        params.shaderAPIs = 0;
        params.o.renderPaths = (1 << kPathForward);
        params.o.noForwardAdd = true;
        // two separate shader_features, A and B
        params.o.pragmaDirectives =
            "#pragma shader_feature A\n"
            "#pragma shader_feature B\n";
        params.source =
            "struct Input { float a; };\n"
            "void surf (Input IN, inout SurfaceOutput o) {}\n";
        DoTest();
        // code should have four guards: (!A && !B), (A && !B), (!A && B), (A && B)
        size_t pos1 = res.find("#if !defined(A) && !defined(B)\n// Surface shader code");
        CHECK(pos1 != core::string::npos);
        size_t pos2 = res.find("#if defined(A) && !defined(B)\n// Surface shader code");
        CHECK(pos2 != core::string::npos);
        size_t pos3 = res.find("#if defined(B) && !defined(A)\n// Surface shader code");
        CHECK(pos3 != core::string::npos);
        size_t pos4 = res.find("#if defined(A) && defined(B)\n// Surface shader code");
        CHECK(pos4 != core::string::npos);
        // Check how many surface variants were analyzed (instancing does not kick in since shaderAPIs is 0)
        // Compile: 4 unique (function, fwdbase lighting, fwdbase no lighting, meta), x4 total for the variants
        CHECK_EQUAL(16, params.analysisCacheRequests);
        CHECK_EQUAL(12, params.analysisCacheHits);
        // AST: 1 unique, 4x total for variants.
        CHECK_EQUAL(4, params.astAnalysisCacheRequests);
        CHECK_EQUAL(3, params.astAnalysisCacheHits);
    }

    TEST_FIXTURE(SurfaceCompilerFixture, UnityVersionMacro_IsSetupForSurfaceShaderAnalysisStep)
    {
        params.o.renderPaths = (1 << kPathForward);
        params.o.noForwardAdd = true;
        params.source =
            "#if UNITY_VERSION > 500\n" // will result in compile error if UNITY_VERSION is not setup
            "struct Input { float a; };\n"
            "#endif\n"
            "void surf (Input IN, inout SurfaceOutput o) {}\n";
        DoTest();
        CHECK_EQUAL(0, errors.size());
    }

    TEST_FIXTURE(SurfaceCompilerFixture, SurfaceCompiler_AllowsMoreThan10Texcoords_InSM40)
    {
        params.source =
            "struct Input {\n"
            // 12 UVs, packed into 6 texcoords
            "\tfloat2 uv_0, uv_1, uv_2, uv_3, uv_4, uv_5, uv_6, uv_7, uv_8, uv_9, uv_10, uv_11;\n"
            // a bunch of builtin variables that will use up a lot of texcoords
            "\tfloat3 worldNormal; float3 worldRefl; float3 viewDir;\n"
            "\tINTERNAL_DATA };\n"
            "void surf (Input IN, inout SurfaceOutput o) {\n"
            "\to.Normal.rg = IN.uv_0;\n"
            "\to.Albedo.rg = IN.uv_0 + IN.uv_1 + IN.uv_2 + IN.uv_3 + IN.uv_4 + IN.uv_5 + IN.uv_6 + IN.uv_7 + IN.uv_8 + IN.uv_9 + IN.uv_10 + IN.uv_11;\n"
            "\to.Albedo += WorldReflectionVector(IN, o.Normal) + WorldNormalVector(IN, o.Normal) + IN.viewDir;\n"
            "}\n";

        // should fail for SM3.0 (more than 10 texcoords used)
        params.target.m_BaseRequirements = kShaderRequireShaderModel30;
        DoTest();
        CHECK_EQUAL(1, errors.size());
        CHECK(errors[0].message.find("Too many texture interpolators") != core::string::npos); // should have this error message
        CHECK(res.find("Too many texture interpolators") != core::string::npos); // result code snippet should have comment about the error

        // should not fail for SM4.0 (since it can do up to 15 texcoords in our nomenclature)
        params.target.m_BaseRequirements = kShaderRequireShaderModel40;
        DoTest();
        CHECK_EQUAL(0, errors.size());
    }

    TEST_FIXTURE(SurfaceCompilerFixture, SurfaceCompiler_AllowsMoreThan20Texcoords_InRequestsFor32TexCoords)
    {
        params.source =
            "struct Input {\n"
            // 12 float4 UVs
            "\tfloat4 uv_0, uv_1, uv_2, uv_3, uv_4, uv_5, uv_6, uv_7, uv_8, uv_9, uv_10, uv_11;\n"
            // a bunch of builtin variables that will use up a lot of texcoords
            "\tfloat3 worldNormal; float3 worldRefl; float3 viewDir;\n"
            "\tINTERNAL_DATA };\n"
            "void surf (Input IN, inout SurfaceOutput o) {\n"
            "\to.Normal.rg = IN.uv_0.xy;\n"
            "\to.Albedo.rg = (IN.uv_0 + IN.uv_1 + IN.uv_2 + IN.uv_3 + IN.uv_4 + IN.uv_5 + IN.uv_6 + IN.uv_7 + IN.uv_8 + IN.uv_9 + IN.uv_10 + IN.uv_11).xy;\n"
            "\to.Albedo += WorldReflectionVector(IN, o.Normal) + WorldNormalVector(IN, o.Normal) + IN.viewDir;\n"
            "}\n";

        // should fail for SM3.0 (more than 10 texcoords used)
        params.target.m_BaseRequirements = kShaderRequireShaderModel30;
        DoTest();
        CHECK_EQUAL(1, errors.size());
        CHECK(errors[0].message.find("Too many texture interpolators") != core::string::npos); // should have this error message

        // should not fail for SM3.5 + 32 varyings
        params.target.m_BaseRequirements = kShaderRequireShaderModel35_ES3 | kShaderRequireInterpolators32;
        DoTest();
        CHECK_EQUAL(0, errors.size());

        // now, our SM4.0 and 5.0 shader target models normally don't mandate 32 varyings (to make them work on mobile,
        // where even platforms that have compute+tessellation often only expose 15 varyings). However
        // there's a ton of surface shaders written out there that use "#pragma target 4.0" as a shorthand for
        // "I will need a ton of varyings". SurfaceCompiler.cpp GetMaximumVaryingsFromRequirements has special handling for this
        // exception.
        params.target.m_BaseRequirements = kShaderRequireShaderModel40;
        DoTest();
        CHECK_EQUAL(0, errors.size());
    }

    TEST_FIXTURE(SurfaceCompilerFixture, AllowsMoreThan16Samplers_WhenNoSamplerMacrosAreUsed)
    {
        params.source =
            "struct Input {\n"
            "    float2 uv_Texture0;\n"
            "};\n"
            // Declare and use many textures, but have them share the same sampler. Should produce no errors.
            "UNITY_DECLARE_TEX2D(_Texture0);\n"
            "UNITY_DECLARE_TEX2D_NOSAMPLER(_Texture1);\n"
            "UNITY_DECLARE_TEX2D_NOSAMPLER(_Texture2);\n"
            "UNITY_DECLARE_TEX2D_NOSAMPLER(_Texture3);\n"
            "UNITY_DECLARE_TEX2D_NOSAMPLER(_Texture4);\n"
            "UNITY_DECLARE_TEX2D_NOSAMPLER(_Texture5);\n"
            "UNITY_DECLARE_TEX2D_NOSAMPLER(_Texture6);\n"
            "UNITY_DECLARE_TEX2D_NOSAMPLER(_Texture7);\n"
            "UNITY_DECLARE_TEX2D_NOSAMPLER(_Texture8);\n"
            "UNITY_DECLARE_TEX2D_NOSAMPLER(_Texture9);\n"
            "UNITY_DECLARE_TEX2D_NOSAMPLER(_Texture10);\n"
            "UNITY_DECLARE_TEX2D_NOSAMPLER(_Texture11);\n"
            "UNITY_DECLARE_TEX2D_NOSAMPLER(_Texture12);\n"
            "UNITY_DECLARE_TEX2D_NOSAMPLER(_Texture13);\n"
            "UNITY_DECLARE_TEX2D_NOSAMPLER(_Texture14);\n"
            "UNITY_DECLARE_TEX2D_NOSAMPLER(_Texture15);\n"
            "UNITY_DECLARE_TEX2D_NOSAMPLER(_Texture16);\n"
            "UNITY_DECLARE_TEX2D_NOSAMPLER(_Texture17);\n"
            "UNITY_DECLARE_TEX2D_NOSAMPLER(_Texture18);\n"
            "UNITY_DECLARE_TEX2D_NOSAMPLER(_Texture19);\n"
            "UNITY_DECLARE_TEX2D_NOSAMPLER(_Texture20);\n"
            "UNITY_DECLARE_TEX2D_NOSAMPLER(_Texture21);\n"
            "UNITY_DECLARE_TEX2D_NOSAMPLER(_Texture22);\n"
            "UNITY_DECLARE_TEX2D_NOSAMPLER(_Texture23);\n"
            "UNITY_DECLARE_TEX2D_NOSAMPLER(_Texture24);\n"
            "UNITY_DECLARE_TEX2D_NOSAMPLER(_Texture25);\n"
            "UNITY_DECLARE_TEX2D_NOSAMPLER(_Texture26);\n"
            "UNITY_DECLARE_TEX2D_NOSAMPLER(_Texture27);\n"
            "UNITY_DECLARE_TEX2D_NOSAMPLER(_Texture28);\n"
            "UNITY_DECLARE_TEX2D_NOSAMPLER(_Texture29);\n"
            "UNITY_DECLARE_TEX2D_NOSAMPLER(_Texture30);\n"
            "UNITY_DECLARE_TEX2D_NOSAMPLER(_Texture31);\n"
            "UNITY_DECLARE_TEX2D_NOSAMPLER(_Texture32);\n"
            "UNITY_DECLARE_TEX2D_NOSAMPLER(_Texture33);\n"
            "\n"
            "UNITY_INSTANCING_BUFFER_START(Props)\n"
            "    float4 _Color;\n"
            "UNITY_INSTANCING_BUFFER_END(Props)\n"
            "\n"
            "void surf (Input IN, inout SurfaceOutput o) {\n"
            "    float2 uv = IN.uv_Texture0;\n"
            "    fixed4 col = UNITY_SAMPLE_TEX2D(_Texture0, uv);\n"
            "    col += UNITY_SAMPLE_TEX2D_SAMPLER(_Texture1, _Texture0, uv);\n"
            "    col += UNITY_SAMPLE_TEX2D_SAMPLER(_Texture2, _Texture0, uv);\n"
            "    col += UNITY_SAMPLE_TEX2D_SAMPLER(_Texture3, _Texture0, uv);\n"
            "    col += UNITY_SAMPLE_TEX2D_SAMPLER(_Texture4, _Texture0, uv);\n"
            "    col += UNITY_SAMPLE_TEX2D_SAMPLER(_Texture5, _Texture0, uv);\n"
            "    col += UNITY_SAMPLE_TEX2D_SAMPLER(_Texture6, _Texture0, uv);\n"
            "    col += UNITY_SAMPLE_TEX2D_SAMPLER(_Texture7, _Texture0, uv);\n"
            "    col += UNITY_SAMPLE_TEX2D_SAMPLER(_Texture8, _Texture0, uv);\n"
            "    col += UNITY_SAMPLE_TEX2D_SAMPLER(_Texture9, _Texture0, uv);\n"
            "    col += UNITY_SAMPLE_TEX2D_SAMPLER(_Texture10, _Texture0, uv);\n"
            "    col += UNITY_SAMPLE_TEX2D_SAMPLER(_Texture11, _Texture0, uv);\n"
            "    col += UNITY_SAMPLE_TEX2D_SAMPLER(_Texture12, _Texture0, uv);\n"
            "    col += UNITY_SAMPLE_TEX2D_SAMPLER(_Texture13, _Texture0, uv);\n"
            "    col += UNITY_SAMPLE_TEX2D_SAMPLER(_Texture14, _Texture0, uv);\n"
            "    col += UNITY_SAMPLE_TEX2D_SAMPLER(_Texture15, _Texture0, uv);\n"
            "    col += UNITY_SAMPLE_TEX2D_SAMPLER(_Texture16, _Texture0, uv);\n"
            "    col += UNITY_SAMPLE_TEX2D_SAMPLER(_Texture17, _Texture0, uv);\n"
            "    col += UNITY_SAMPLE_TEX2D_SAMPLER(_Texture18, _Texture0, uv);\n"
            "    col += UNITY_SAMPLE_TEX2D_SAMPLER(_Texture19, _Texture0, uv);\n"
            "    col += UNITY_SAMPLE_TEX2D_SAMPLER(_Texture20, _Texture0, uv);\n"
            "    col += UNITY_SAMPLE_TEX2D_SAMPLER(_Texture21, _Texture0, uv);\n"
            "    col += UNITY_SAMPLE_TEX2D_SAMPLER(_Texture22, _Texture0, uv);\n"
            "    col += UNITY_SAMPLE_TEX2D_SAMPLER(_Texture23, _Texture0, uv);\n"
            "    col += UNITY_SAMPLE_TEX2D_SAMPLER(_Texture24, _Texture0, uv);\n"
            "    col += UNITY_SAMPLE_TEX2D_SAMPLER(_Texture25, _Texture0, uv);\n"
            "    col += UNITY_SAMPLE_TEX2D_SAMPLER(_Texture26, _Texture0, uv);\n"
            "    col += UNITY_SAMPLE_TEX2D_SAMPLER(_Texture27, _Texture0, uv);\n"
            "    col += UNITY_SAMPLE_TEX2D_SAMPLER(_Texture28, _Texture0, uv);\n"
            "    col += UNITY_SAMPLE_TEX2D_SAMPLER(_Texture29, _Texture0, uv);\n"
            "    col += UNITY_SAMPLE_TEX2D_SAMPLER(_Texture30, _Texture0, uv);\n"
            "    col += UNITY_SAMPLE_TEX2D_SAMPLER(_Texture31, _Texture0, uv);\n"
            "    col += UNITY_SAMPLE_TEX2D_SAMPLER(_Texture32, _Texture0, uv);\n"
            "    col += UNITY_SAMPLE_TEX2D_SAMPLER(_Texture33, _Texture0, uv);\n"
            "    o.Albedo = col.rgb;\n"
            "    o.Alpha = col.a;\n"
            "}\n";

        DoTest();
        CHECK_EQUAL(0, errors.size());
    }

    TEST_FIXTURE(SurfaceCompilerFixture, AllowsSameMemberNames_InInputAndOutputStructs)
    {
        params.source =
            "struct Input {\n"
            "  float2 uv_Texture0;\n"
            "  float Alpha;\n"
            "};\n"
            "void vert (inout appdata_full v, out Input o) {\n"
            "  UNITY_INITIALIZE_OUTPUT(Input, o);\n"
            "  o.Alpha = 0.5;\n"
            "}\n"
            "void surf (Input IN, inout SurfaceOutput o) {\n"
            "  o.Albedo.rg = IN.uv_Texture0;\n"
            "  o.Alpha = IN.Alpha;\n"
            "}\n";
        params.p.entryName[kProgramVertex] = "vert";
        DoTest();
        CHECK_EQUAL(0, errors.size());
    }

    TEST_FIXTURE(SurfaceCompilerFixture, GeneratedForwardBasePass_DynamicGITexcoord_NotCountedTowardsLimit_InSM20)
    {
        // we need a shader that will use exactly 8 texcoords in SM2.0, and dynamic GI texcoord would go over the limit
        // 2 texcoord: user
        // 3 more: tangent-to-world
        // 3 more: shadows (1), light cookies (1), ambient/sh (1)
        // Update expectations & comments below if we start generating code differently! This test requires generated
        // things to be exactly at 8 texcoords (excluding dynamic GI one).
        params.source =
            "struct Input {\n"
            "\tfloat3 uv_0;\n" // will use 2 texcoords
            "\tfloat3 uv_1;\n"
            "};\n"
            "void surf (Input IN, inout SurfaceOutput o) {\n"
            "   o.Normal.rgb = IN.uv_0 + IN.uv_1;\n"
            "}\n";

        params.o.renderPaths = (1 << kPathForward);
        params.o.noForwardAdd = true;

        // Should work fine in SM3.0 (10 texcoords used, no problem)
        params.target.m_BaseRequirements = kShaderRequireShaderModel30;
        DoTest();
        CHECK_EQUAL(0, errors.size());
        CHECK(res.find("UNITY_FOG_COORDS(6)") != core::string::npos); // fog should get texcoord 5
        CHECK(res.find("float4 lmap : TEXCOORD8") != core::string::npos); // GI lightmap should get texcoord 8

        // Should be no error in SM2.0 (9 texcoords used, but the last one is for dynamic GI, which only kicks in in SM3.0
        // variants).
        params.target.m_BaseRequirements = kShaderRequireShaderModel20;
        DoTest();
        CHECK_EQUAL(0, errors.size());
        CHECK(res.find("UNITY_FOG_COORDS(6)") != core::string::npos); // fog should get texcoord 5
        CHECK(res.find("float4 lmap : TEXCOORD8") != core::string::npos); // GI lightmap should get texcoord 8
    }

    TEST_FIXTURE(SurfaceCompilerFixture, GeneratedForwardBasePass_DynamicGITexcoord_IsCountedTowardsLimit_InSM30)
    {
        // we need a shader that will use exactly 10 texcoords in SM3.0, and dynamic GI texcoord would go over the limit
        // 4 texcoords: user
        // 3 more: tangent-to-world
        // 3 more: shadows (1), light (1), ambient/sh (1)
        // Update expectations & comments below if we start generating code differently! This test requires generated
        // things to be exactly at 10 texcoords (excluding dynamic GI one).
        params.source =
            "struct Input {\n"
            "\tfloat3 uv_0, uv_1, uv_2, uv_3;\n" // will use 4 texcoords
            "};\n"
            "void surf (Input IN, inout SurfaceOutput o) {\n"
            "   o.Albedo.rgb = IN.uv_0 + IN.uv_1 + IN.uv_2;\n"
            "\to.Normal.rgb = IN.uv_3;\n"
            "}\n";

        params.o.renderPaths = (1 << kPathForward);
        params.o.noForwardAdd = true;

        // Should work fine in SM4.0 (11 texcoords used, no problem)
        params.target.m_BaseRequirements = kShaderRequireShaderModel40;
        DoTest();
        CHECK_EQUAL(0, errors.size());
        CHECK(res.find("UNITY_FOG_COORDS(8)") != core::string::npos); // fog should get texcoord 8
        CHECK(res.find("float4 lmap : TEXCOORD10") != core::string::npos); // GI lightmap should get texcoord 10

        // Should be an error in SM3.0: 11 texcoords used.
        params.target.m_BaseRequirements = kShaderRequireShaderModel30;
        DoTest();
        CHECK_EQUAL(1, errors.size());
        CHECK(errors[0].message.find("Too many texture interpolators") != core::string::npos); // should have this error message
        CHECK(res.find("float4 lmap : TEXCOORD10") != core::string::npos); // GI lightmap should get texcoord 10
    }

    TEST_FIXTURE(SurfaceCompilerFixture, GeneratedForwardBasePass_PassesShadowAndLightCoordsFromVertexShaderToFragmentShader_WhenHasEnoughTexcoordSpace)
    {
        params.source =
            "struct Input {\n"
            "\tfloat3 uv_0;\n" // will use 2 texcoords
            "\tfloat3 uv_1;\n"
            "};\n"
            "void surf (Input IN, inout SurfaceOutput o) {\n"
            "\to.Normal.rgb = IN.uv_0 + IN.uv_1;\n"
            "}\n";
        params.o.renderPaths = (1 << kPathForward);
        params.o.noForwardAdd = true;

        params.target.m_BaseRequirements = kShaderRequireShaderModel20;

        DoTest();
        CHECK_EQUAL(0, errors.size());
        CHECK(res.find("UNITY_LIGHTING_COORDS(6,7)") != core::string::npos);    // Should generate both lighting and shadow coords (TC6 and TC7)
    }

    // To save on ALUs in pixel shader, the ForwardAdd pass tries to pass worldPos in a separate texcoord
    // when there are texcoords to spare. Check for that.
    TEST_FIXTURE(SurfaceCompilerFixture, GeneratedForwardAddPass_UsesSeparateWorldPosTexcoord_WhenNotTooManyTexcoords)
    {
        params.source =
            "struct Input {\n"
            "\tfloat3 uv_0;\n"
            "};\n"
            "void surf (Input IN, inout SurfaceOutput o) {\n"
            "\to.Normal.rgb = IN.uv_0;\n" // per-pixel normal, will cause tangent-to-world be passed in texcoords
            "}\n";

        // only generate ForwardAdd pass
        params.o.renderPaths = (1 << kPathForward);
        params.o.noForwardBase = true;

        params.target.m_BaseRequirements = kShaderRequireShaderModel20;
        DoTest();
        CHECK_EQUAL(0, errors.size());
        CHECK(res.find("3 tSpace0 : TEXCOORD") != core::string::npos); // tangent-to-world matrix should use a 3-component vector
        CHECK(res.find("float3 worldPos : TEXCOORD") != core::string::npos); // worldPos should be passed in a separate texcoord
    }

    // worldPos is packed into tangent space texcoords (.w components) when there aren't spare texcoords.
    TEST_FIXTURE(SurfaceCompilerFixture, GeneratedForwardAddPass_PacksWorldPosInTangentSpace_WhenManyTexcoords)
    {
        // our own data will use 4 texcoords,
        // 3 more will be taken by tangent-to-world,
        // and 2 more by shadows & fog:
        // no space left for passing worldPos in a separate texcoord
        params.source =
            "struct Input {\n"
            "\tfloat3 uv_0, uv_1, uv_2, uv_3;\n"
            "};\n"
            "void surf (Input IN, inout SurfaceOutput o) {\n"
            "\to.Normal.rgb = IN.uv_0 + IN.uv_1 + IN.uv_2 + IN.uv_3;\n" // per-pixel normal, will cause tangent-to-world be passed in texcoords
            "}\n";

        // only generate ForwardAdd pass
        params.o.renderPaths = (1 << kPathForward);
        params.o.noForwardBase = true;

        params.target.m_BaseRequirements = kShaderRequireShaderModel20;
        DoTest();
        CHECK_EQUAL(0, errors.size());
        CHECK(res.find("4 tSpace0 : TEXCOORD") != core::string::npos); // tangent-to-world matrix should use a 4-component vector
        CHECK(res.find("float3 worldPos : TEXCOORD") == core::string::npos); // worldPos should *not* be passed in a separate texcoord!
    }

    TEST_FIXTURE(SurfaceCompilerFixture, GeneratedForwardBasePass_DefaultSettings_EmitsOpaqueAlpha)
    {
        params.source = "struct Input { float a; }; void surf (Input IN, inout SurfaceOutput o) { o.Albedo = 1.0; }";
        params.o.renderPaths = (1 << kPathForward);
        params.o.noForwardAdd = true;
        DoTest();
        CHECK_EQUAL(0, errors.size());
        // generated code should make alpha channel be one
        CHECK(res.find("UNITY_OPAQUE_ALPHA") != core::string::npos);
    }

    TEST_FIXTURE(SurfaceCompilerFixture, GeneratedForwardBasePass_AlphaBlendedMode_DoesNotEmitOpaqueAlpha)
    {
        params.source = "struct Input { float a; }; void surf (Input IN, inout SurfaceOutput o) { o.Albedo = 1.0; }";
        params.o.renderPaths = (1 << kPathForward);
        params.o.noForwardAdd = true;
        params.o.mode = kSurfaceModeAlpha;
        DoTest();
        CHECK_EQUAL(0, errors.size());
        // generated code should *not* make alpha channel be one
        CHECK(res.find("UNITY_OPAQUE_ALPHA") == core::string::npos);
    }

    TEST_FIXTURE(SurfaceCompilerFixture, GeneratedForwardBasePass_KeepAlphaFlag_DoesNotEmitOpaqueAlpha)
    {
        params.source = "struct Input { float a; }; void surf (Input IN, inout SurfaceOutput o) { o.Albedo = 1.0; }";
        params.o.renderPaths = (1 << kPathForward);
        params.o.noForwardAdd = true;
        params.o.keepAlpha = true;
        DoTest();
        CHECK_EQUAL(0, errors.size());
        // generated code should *not* make alpha channel be one
        CHECK(res.find("UNITY_OPAQUE_ALPHA") == core::string::npos);
    }

    TEST_FIXTURE(SurfaceCompilerFixture, GeneratedForwardBasePass_AlphaBlend_EmitsCorrectState)
    {
        params.source = "struct Input { float a; }; void surf (Input IN, inout SurfaceOutput o) { o.Albedo = 1.0; }";
        params.o.renderPaths = (1 << kPathForward);
        params.o.noForwardAdd = true;
        params.o.mode = kSurfaceModeAlpha;
        params.o.blend = kSurfaceBlendAlpha;
        DoTest();
        CHECK_EQUAL(0, errors.size());
        CHECK(res.find("Blend SrcAlpha OneMinusSrcAlpha") != core::string::npos);
    }

    TEST_FIXTURE(SurfaceCompilerFixture, GeneratedForwardBasePass_AlphaPremul_EmitsCorrectState)
    {
        params.source = "struct Input { float a; }; void surf (Input IN, inout SurfaceOutput o) { o.Albedo = 1.0; }";
        params.o.renderPaths = (1 << kPathForward);
        params.o.noForwardAdd = true;
        params.o.mode = kSurfaceModeAlpha;
        params.o.blend = kSurfaceBlendPremultiply;
        DoTest();
        CHECK_EQUAL(0, errors.size());
        CHECK(res.find("Blend One OneMinusSrcAlpha") != core::string::npos);
    }

    TEST_FIXTURE(SurfaceCompilerFixture, GeneratedForwardBasePass_NoColorMask_PreventsColorMaskEmitting)
    {
        params.source = "struct Input { float a; }; void surf (Input IN, inout SurfaceOutput o) { o.Albedo = 1.0; }";
        params.o.renderPaths = (1 << kPathForward);
        params.o.noForwardAdd = true;
        params.o.noColorMask = true;
        params.o.mode = kSurfaceModeAlpha;
        DoTest();
        CHECK_EQUAL(0, errors.size());
        // generated code should *not* have ColorMask
        CHECK(res.find("ColorMask") == core::string::npos);
    }

    TEST_FIXTURE(SurfaceCompilerFixture, GeneratedForwardBasePass_StandardWithAlphaBlend_EmitsDefineAndState)
    {
        params.o.lighting = "Standard";
        params.source =
            "#include \"UnityPBSLighting.cginc\"\n"
            "struct Input { float a; };\n"
            "void surf (Input IN, inout SurfaceOutputStandard o) { }";
        params.o.renderPaths = (1 << kPathForward);
        params.o.noForwardAdd = true;
        params.o.mode = kSurfaceModeAlpha;
        params.o.blend = kSurfaceBlendAlpha;
        DoTest();
        CHECK_EQUAL(0, errors.size());
        // generated code should define _ALPHABLEND_ON and have appropriate blending state
        CHECK(res.find("define _ALPHABLEND_ON 1") != core::string::npos);
        CHECK(res.find("Blend SrcAlpha OneMinusSrcAlpha") != core::string::npos);
        // generated code should *not* have _ALPHAPREMULTIPLY_ON defined
        CHECK(res.find("define _ALPHAPREMULTIPLY_ON 1") == core::string::npos);
    }

    TEST_FIXTURE(SurfaceCompilerFixture, GeneratedForwardBasePass_StandardWithAlphaPremul_EmitsDefineAndState)
    {
        params.o.lighting = "Standard";
        params.source =
            "#include \"UnityPBSLighting.cginc\"\n"
            "struct Input { float a; };\n"
            "void surf (Input IN, inout SurfaceOutputStandard o) { }";
        params.o.renderPaths = (1 << kPathForward);
        params.o.noForwardAdd = true;
        params.o.mode = kSurfaceModeAlpha;
        params.o.blend = kSurfaceBlendPremultiply;
        DoTest();
        CHECK_EQUAL(0, errors.size());
        // generated code should define _ALPHAPREMULTIPLY_ON and have appropriate alpha premultiply bledning state
        CHECK(res.find("define _ALPHAPREMULTIPLY_ON 1") != core::string::npos);
        CHECK(res.find("Blend One OneMinusSrcAlpha") != core::string::npos);
        // generated code should *not* have _ALPHABLEND_ON defined
        CHECK(res.find("define _ALPHABLEND_ON 1") == core::string::npos);
    }

    TEST_FIXTURE(SurfaceCompilerFixture, GeneratedForwardBasePass_StandardLighting_AddsIncludePBSLighting_Automatically)
    {
        params.o.lighting = "Standard";
        params.source =
            "struct Input { float a; };\n"
            "void surf (Input IN, inout SurfaceOutputStandard o) { }";
        params.o.renderPaths = (1 << kPathForward);
        params.o.noForwardAdd = true;
        DoTest();
        CHECK_EQUAL(0, errors.size());
        // generated code should have UnityPBSLighting include added
        CHECK(res.find("#include \"UnityPBSLighting.cginc\"\n") != core::string::npos);
    }

    TEST_FIXTURE(SurfaceCompilerFixture, GeneratedForwardBasePass_StandardLighting_WithPBSIncludePresent_DoesNotAddIt)
    {
        params.o.lighting = "Standard";
        // source already incldues UnityPBSLighting
        params.source =
            "#include \"UnityPBSLighting.cginc\" // mine\n"
            "struct Input { float a; };\n"
            "void surf (Input IN, inout SurfaceOutputStandard o) { }";
        params.o.renderPaths = (1 << kPathForward);
        params.o.noForwardAdd = true;
        DoTest();
        CHECK_EQUAL(0, errors.size());
        // generated code not have one more UnityPBSLighting include added!
        CHECK(res.find("#include \"UnityPBSLighting.cginc\"\n") == core::string::npos);
        CHECK(res.find("#include \"UnityPBSLighting.cginc\" // mine") != core::string::npos);
    }

    TEST_FIXTURE(SurfaceCompilerFixture, SurfaceCompiler_CommentsOutAllPragmas)
    {
        core::string result;
        core::string gen = "#pragma   surface   surf\n#pragma foobar\n";
        OutputGeneratedSnippet("", result, gen, params, ~0, rawErrors);
        CHECK(!rawErrors.HasErrorsOrWarnings());
        CHECK(result.find("//#pragma   surface") != core::string::npos); // comments out pragma with multiple spaces (case 610094)
        CHECK(result.find("//#pragma foobar") != core::string::npos); // comments out arbitrary pragma
    }

    TEST_FIXTURE(SurfaceCompilerFixture, SurfaceCompiler_EmitsPragmaTargetCorrectly)
    {
        core::string result;
        core::string gen =
            "#pragma target 3.0\n"
            "#pragma target 4.0 FOO\n"
            "#pragma target 5.0 BAR BAZ\n";
        params.o.pragmaDirectives = gen;
        params.target.m_BaseRequirements = kShaderRequireShaderModel30;
        params.target.AddKeywordRequirements("FOO", kShaderRequireShaderModel40);
        params.target.AddKeywordRequirements("BAR", kShaderRequireShaderModel50);
        params.target.AddKeywordRequirements("BAZ", kShaderRequireShaderModel50);

        OutputGeneratedSnippet("", result, gen, params, ~0, rawErrors);
        CHECK(!rawErrors.HasErrorsOrWarnings());
        // check if surface compiler emits un-commented target pragmas
        CHECK(result.find("\n#pragma target 3.0\n") != core::string::npos);
        CHECK(result.find("\n#pragma target 4.0 FOO\n") != core::string::npos);
        CHECK(result.find("\n#pragma target 5.0 BAR BAZ\n") != core::string::npos);
        // check if surface compiler comments out all input pragmas
        CHECK(result.find("//#pragma target 3.0\n") != core::string::npos);
        CHECK(result.find("//#pragma target 4.0 FOO\n") != core::string::npos);
        CHECK(result.find("//#pragma target 5.0 BAR BAZ\n") != core::string::npos);
    }

    TEST_FIXTURE(SurfaceCompilerFixture, SurfaceCompiler_EmitsPragmaOnlyExcludeRenderersCorrectly)
    {
        core::string result;
        core::string gen =
            "#pragma only_renderers d3d11 d3d9\n"
            "#pragma exclude_renderers d3d11\n";
        params.o.pragmaDirectives = gen;
        OutputGeneratedSnippet("", result, gen, params, ~0, rawErrors);
        CHECK(!rawErrors.HasErrorsOrWarnings());
        CHECK(result.find("\n#pragma only_renderers") != core::string::npos); // emits un-commented pragmas
        CHECK(result.find("\n#pragma exclude_renderers") != core::string::npos); // emits un-commented pragmas
        CHECK(result.find("//#pragma only_renderers") != core::string::npos); // comments out original pragmas
        CHECK(result.find("//#pragma exclude_renderers") != core::string::npos); // comments out original pragmas
    }

    TEST_FIXTURE(SurfaceCompilerFixture, Texture2DArrayMacros_CanBeParsedBySurfaceAnalysis)
    {
        params.source =
            "UNITY_DECLARE_TEX2DARRAY(_MyArray);\n"
            "struct Input { float2 uv_MyArray; };\n"
            "void surf (Input IN, inout SurfaceOutput o) {\n"
            "  o.Albedo = UNITY_SAMPLE_TEX2DARRAY(_MyArray, float3(IN.uv_MyArray,1)).rgb;\n"
            "}";
        params.target.m_BaseRequirements = kShaderRequireShaderModel35_ES3;
        DoTest();
        CHECK_EQUAL(0, errors.size());
    }

    TEST_FIXTURE(SurfaceCompilerFixture, Always_Sample_SH_If_Lightmap_Support_Is_Disabled)
    {
        params.source = "struct Input { float a; }; void surf (Input IN, inout SurfaceOutput o) { o.Albedo = 1.0; }";
        params.o.renderPaths = (1 << kPathForward);
        params.o.noForwardAdd = true;
        params.o.noLightmap = true;
        DoTest();
        CHECK_EQUAL(0, errors.size());
        // Specific test for case 978682 We should always sample SH if lightmap support is disabled
        CHECK(res.find("//Shader does not support lightmap thus we always want to fallback to SH.") != core::string::npos);
    }

    static void CheckInstancingNotGenerated(const core::string& source)
    {
        // instancing macros should always be generated
        CHECK(source.find("UNITY_VERTEX_INPUT_INSTANCE_ID\n") != core::string::npos);
        CHECK(source.find("UNITY_SETUP_INSTANCE_ID(v);\n") != core::string::npos);
        CHECK(source.find("UNITY_TRANSFER_INSTANCE_ID(v,o);\n") != core::string::npos);

        // we should not see instancing variants being added
        CHECK(source.find("#pragma multi_compile_instancing\n") == core::string::npos);
        CHECK(source.find("// -------- variant for: INSTANCING_ON \n") == core::string::npos);
    }

    TEST_FIXTURE(SurfaceCompilerFixture, DontGenerateInstancingIfDisabled)
    {
        params.source = "struct Input { float a; }; void surf (Input IN, inout SurfaceOutput o) { o.Albedo = 1.0; }";

        // don't generate instancing if noInstancing is specified
        params.shaderAPIs = (1 << kShaderCompPlatformD3D11);
        params.o.noInstancing = true;
        DoTest();
        CHECK(errors.empty());
        CheckInstancingNotGenerated(res);
        // Check how many surface variants were analyzed
        // Compile: 10 (function, FwdBase, FwdBaseNoLight, FwdAdd, FwdAddNoLight, PrepassBase, PrepassFinal, PrepassFinalNoLight, Deferred, Meta); all unique
        CHECK_EQUAL(10, params.analysisCacheRequests);
        CHECK_EQUAL(0, params.analysisCacheHits);
        // AST: 1 unique/total
        CHECK_EQUAL(1, params.astAnalysisCacheRequests);
        CHECK_EQUAL(0, params.astAnalysisCacheHits);

        // pragma multi_compile_instancing should be removed if instancing is not to be generated
        params.o.pragmaDirectives = "#pragma multi_compile_instancing";
        DoTest();
        CHECK(errors.empty());
        CheckInstancingNotGenerated(res);
    }

    static void CheckInstancingGenerated(bool hasInstancingOn, const core::string& source)
    {
        CHECK(source.find("#pragma multi_compile_instancing") != core::string::npos);
        CHECK(source.find("UNITY_VERTEX_INPUT_INSTANCE_ID\n") != core::string::npos);
        CHECK(source.find("UNITY_SETUP_INSTANCE_ID(v);\n") != core::string::npos);
        CHECK(source.find("UNITY_TRANSFER_INSTANCE_ID(v,o);\n") != core::string::npos);
        if (hasInstancingOn)
        {
            CHECK(source.find("#pragma skip_variants INSTANCING_ON") == core::string::npos);
            CHECK(source.find("// -------- variant for: INSTANCING_ON \n") != core::string::npos);
        }
        else
        {
            CHECK(source.find("#pragma skip_variants INSTANCING_ON") != core::string::npos);
            CHECK(source.find("// -------- variant for: INSTANCING_ON \n") == core::string::npos);
        }
    }

    TEST_FIXTURE(SurfaceCompilerFixture, InstancingGeneratedCorrectly)
    {
        params.source = "struct Input { float a; }; void surf (Input IN, inout SurfaceOutput o) { o.Albedo = 1.0; }";
        // generate instancing for the D3D11
        params.shaderAPIs = (1 << kShaderCompPlatformD3D11);

        // Forward base
        params.o.renderPaths = (1 << kPathForward);
        params.o.noForwardAdd = true;
        params.o.noMeta = true;
        DoTest();
        CHECK_EQUAL(0, errors.size());
        CheckInstancingGenerated(true, res);
        // Check how many surface variants were analyzed
        // Compile: 3 unique (function, FwdBase, FwdBaseNoLight); 2x requests for instancing
        CHECK_EQUAL(6, params.analysisCacheRequests);
        CHECK_EQUAL(3, params.analysisCacheHits);
        // AST: 1 unique, 2x for instancing
        CHECK_EQUAL(2, params.astAnalysisCacheRequests);
        CHECK_EQUAL(1, params.astAnalysisCacheHits);

        // Forward add
        params.o.noForwardAdd = false;
        params.o.noForwardBase = true;
        DoTest();
        CHECK_EQUAL(0, errors.size());
        CheckInstancingGenerated(false, res);

        // Prepass
        params.o.renderPaths = 1 << kPathPrePass;
        DoTest();
        CHECK_EQUAL(0, errors.size());
        CheckInstancingGenerated(true, res);

        // Deferred
        params.o.renderPaths = 1 << kPathDeferred;
        DoTest();
        CHECK_EQUAL(0, errors.size());
        CheckInstancingGenerated(true, res);

        // Shadow
        params.o.renderPaths = 0;
        params.o.addShadow = true;
        DoTest();
        CHECK_EQUAL(0, errors.size());
        CheckInstancingGenerated(true, res);

        // Meta
        params.o.renderPaths = 0;
        params.o.addShadow = false;
        params.o.noMeta = false;
        DoTest();
        CHECK_EQUAL(0, errors.size());
        CheckInstancingGenerated(true, res);
    }

    TEST_FIXTURE(SurfaceCompilerFixture, InstanceIDInSurfaceShaderInput_DoesNotFail)
    {
        params.source =
            "struct Input {\n"
            "UNITY_VERTEX_INPUT_INSTANCE_ID\n"
            "fixed4 color; \n"
            "};\n"
            "UNITY_INSTANCING_BUFFER_START(MyProperties)\n"
            "UNITY_DEFINE_INSTANCED_PROP(fixed4, _Color)\n"
            "#define _Color_arr MyProperties\n"
            "UNITY_INSTANCING_BUFFER_END(MyProperties)\n"
            "void vert(inout appdata_full v, out Input o) {\n"
            "    UNITY_INITIALIZE_OUTPUT(Input, o);\n"
            "    UNITY_SETUP_INSTANCE_ID(v); \n"
            "    UNITY_TRANSFER_INSTANCE_ID(v, o); \n"
            "    o.color = UNITY_ACCESS_INSTANCED_PROP(_Color_arr, _Color) * v.color; \n"
            "}\n"
            "void surf(Input IN, inout SurfaceOutput o) {\n"
            "UNITY_SETUP_INSTANCE_ID(IN); \n"
            "o.Albedo = IN.color.rgb;\n"
            "}\n";
        DoTest();
        CHECK(!rawErrors.HasErrorsOrWarnings());
    }

    TEST_FIXTURE(SurfaceCompilerFixture, ProceduralInstancingInSurfaceShaderInput_DoesNotFail)
    {
        params.source =
            "struct Input { fixed4 color; };\n"
            "#ifdef UNITY_PROCEDURAL_INSTANCING_ENABLED\n"
            "StructuredBuffer<float4> positionBuffer;\n"
            "#endif\n"
            "void surf (Input IN, inout SurfaceOutput o) {\n"
            "o.Albedo = IN.color.rgb;\n"
            "}\n";

        params.p.proceduralFuncName = "a";
        DoTest();
        CHECK(!rawErrors.HasErrorsOrWarnings());
    }

    struct SurfaceCompilerProcessParamsFixture
    {
        SurfaceCompilerProcessParamsFixture()
        {
            params.source = "struct Input { float a; }; void surf (Input IN, inout SurfaceOutput o) { o.Albedo = 1.0; }";
        }

        void DoTest(const char* pragmaDirectives)
        {
            params.o.pragmaDirectives = pragmaDirectives;

            errors.Clear();
            ProcessSurfaceParams(params);
            variants.FindVariantDirectives(params.o.pragmaDirectives, false, errors);
        }

        size_t PrepareEnumeration(ShaderCompilerProgram type)
        {
            return variants.PrepareEnumeration(type);
        }

        void EnumerateOne()
        {
            UnityStrArray kwnames;
            variants.Enumerate(kwnames);
            kw = "";
            for (size_t i = 0, n = kwnames.size(); i != n; ++i)
            {
                if (!kw.empty())
                    kw += ' ';
                kw += kwnames[i];
            }
        }

        SurfaceParams params;
        ShaderCompileVariants variants;
        ShaderImportErrors errors;
        core::string kw;
    };

    TEST_FIXTURE(SurfaceCompilerProcessParamsFixture, AddDithringCrossFade_Works)
    {
        params.o.ditherCrossFade = true;
        DoTest("");
        CHECK(!errors.HasErrors());

        size_t comboCount = PrepareEnumeration(kProgramVertex);
        CHECK_EQUAL(2, comboCount);
        EnumerateOne(); CHECK_EQUAL("", kw);
        EnumerateOne(); CHECK_EQUAL("LOD_FADE_CROSSFADE", kw);
    }

    TEST_FIXTURE(SurfaceCompilerProcessParamsFixture, AddDithringCrossFade_CombineWithLODFadePercentage_Works)
    {
        params.o.ditherCrossFade = true;
        DoTest("#pragma multi_compile_vertex __ LOD_FADE_PERCENTAGE\n");
        CHECK(!errors.HasErrors());

        size_t comboCount = PrepareEnumeration(kProgramVertex);
        CHECK_EQUAL(3, comboCount);
        EnumerateOne(); CHECK_EQUAL("", kw);
        EnumerateOne(); CHECK_EQUAL("LOD_FADE_PERCENTAGE", kw);
        EnumerateOne(); CHECK_EQUAL("LOD_FADE_CROSSFADE", kw);

        comboCount = PrepareEnumeration(kProgramFragment);
        CHECK_EQUAL(2, comboCount);
        EnumerateOne(); CHECK_EQUAL("", kw);
        EnumerateOne(); CHECK_EQUAL("LOD_FADE_CROSSFADE", kw);
    }

    TEST_FIXTURE(SurfaceCompilerProcessParamsFixture, AddDithringCrossFade_CombineWithLODFadePercentage2_Works)
    {
        params.o.ditherCrossFade = true;
        DoTest("#pragma multi_compile __ LOD_FADE_PERCENTAGE\n");
        CHECK(!errors.HasErrors());

        size_t comboCount = PrepareEnumeration(kProgramVertex);
        CHECK_EQUAL(3, comboCount);
        EnumerateOne(); CHECK_EQUAL("", kw);
        EnumerateOne(); CHECK_EQUAL("LOD_FADE_PERCENTAGE", kw);
        EnumerateOne(); CHECK_EQUAL("LOD_FADE_CROSSFADE", kw);

        comboCount = PrepareEnumeration(kProgramFragment);
        CHECK_EQUAL(3, comboCount);
        EnumerateOne(); CHECK_EQUAL("", kw);
        EnumerateOne(); CHECK_EQUAL("LOD_FADE_PERCENTAGE", kw);
        EnumerateOne(); CHECK_EQUAL("LOD_FADE_CROSSFADE", kw);
    }

    TEST_FIXTURE(SurfaceCompilerProcessParamsFixture, AddDithringCrossFade_Duplicate_Works)
    {
        params.o.ditherCrossFade = true;
        DoTest("#pragma multi_compile __ LOD_FADE_CROSSFADE\n");
        CHECK(!errors.HasErrors());

        size_t comboCount = PrepareEnumeration(kProgramVertex);
        CHECK_EQUAL(2, comboCount);
        EnumerateOne(); CHECK_EQUAL("", kw);
        EnumerateOne(); CHECK_EQUAL("LOD_FADE_CROSSFADE", kw);
    }

    TEST_FIXTURE(SurfaceCompilerProcessParamsFixture, AddDithringCrossFade_StringReplaceWorks_Works)
    {
        params.o.ditherCrossFade = true;
        DoTest("#pragma multi_compile __ AA\n#pragma multi_compile __ LOD_FADE_PERCENTAGE\n#pragma multi_compile_vertex __ BB\n");
        CHECK(!errors.HasErrors());

        size_t comboCount = PrepareEnumeration(kProgramVertex);
        CHECK_EQUAL(12, comboCount);
        EnumerateOne(); CHECK_EQUAL("", kw);
        EnumerateOne(); CHECK_EQUAL("LOD_FADE_PERCENTAGE", kw);
        EnumerateOne(); CHECK_EQUAL("LOD_FADE_CROSSFADE", kw);
        EnumerateOne(); CHECK_EQUAL("AA", kw);
        EnumerateOne(); CHECK_EQUAL("AA LOD_FADE_PERCENTAGE", kw);
        EnumerateOne(); CHECK_EQUAL("AA LOD_FADE_CROSSFADE", kw);
        EnumerateOne(); CHECK_EQUAL("BB", kw);
        EnumerateOne(); CHECK_EQUAL("BB LOD_FADE_PERCENTAGE", kw);
        EnumerateOne(); CHECK_EQUAL("BB LOD_FADE_CROSSFADE", kw);
        EnumerateOne(); CHECK_EQUAL("BB AA", kw);
        EnumerateOne(); CHECK_EQUAL("BB AA LOD_FADE_PERCENTAGE", kw);
        EnumerateOne(); CHECK_EQUAL("BB AA LOD_FADE_CROSSFADE", kw);

        comboCount = PrepareEnumeration(kProgramFragment);
        CHECK_EQUAL(6, comboCount);
        EnumerateOne(); CHECK_EQUAL("", kw);
        EnumerateOne(); CHECK_EQUAL("LOD_FADE_PERCENTAGE", kw);
        EnumerateOne(); CHECK_EQUAL("LOD_FADE_CROSSFADE", kw);
        EnumerateOne(); CHECK_EQUAL("AA", kw);
        EnumerateOne(); CHECK_EQUAL("AA LOD_FADE_PERCENTAGE", kw);
        EnumerateOne(); CHECK_EQUAL("AA LOD_FADE_CROSSFADE", kw);
    }

    TEST(AddNoVariantsDirectivesToLastMulticompile_DoesNotAddAnything_OnNoMultiCompileInput)
    {
        SurfaceParams p;
        core::string s;
        s = AddNoVariantsDirectivesToLastMulticompile("", p);
        CHECK_EQUAL("", s);
        s = AddNoVariantsDirectivesToLastMulticompile("#pragma foobar", p);
        CHECK_EQUAL("#pragma foobar", s);

        p.o.noLightmap = true;
        s = AddNoVariantsDirectivesToLastMulticompile("", p);
        CHECK_EQUAL("", s);
        s = AddNoVariantsDirectivesToLastMulticompile("#pragma foobar", p);
        CHECK_EQUAL("#pragma foobar", s);
    }

    TEST(AddNoVariantsDirectivesToLastMulticompile_DoesNotAddAnything_ToUserMultiCompiles)
    {
        SurfaceParams p;
        core::string s;
        p.o.noLightmap = true;
        s = AddNoVariantsDirectivesToLastMulticompile("#pragma multi_compile A B", p);
        CHECK_EQUAL("#pragma multi_compile A B", s); // nothing got added
    }

    TEST(AddNoVariantsDirectivesToLastMulticompile_AddsNoLightmap_ToLastMultiBuiltinCompile)
    {
        SurfaceParams p;
        core::string s;
        p.o.noLightmap = true;
        s = AddNoVariantsDirectivesToLastMulticompile("#pragma multi_compile_fwdbase", p);
        CHECK_EQUAL("#pragma multi_compile_fwdbase nolightmap", s); // nolightmap added to fwdbase line
        s = AddNoVariantsDirectivesToLastMulticompile("#pragma multi_compile_fwdbase\n\n// comment", p);
        CHECK_EQUAL("#pragma multi_compile_fwdbase nolightmap\n\n// comment", s); // nolightmap added to fwdbase line
        s = AddNoVariantsDirectivesToLastMulticompile("#pragma multi_compile_fog\n#pragma multi_compile_fwdbase\n", p);
        CHECK_EQUAL("#pragma multi_compile_fog\n#pragma multi_compile_fwdbase nolightmap\n", s); // nolightmap added to last line (fwdbase)
        s = AddNoVariantsDirectivesToLastMulticompile("#pragma multi_compile_fwdbase\n#pragma multi_compile A B\n", p);
        CHECK_EQUAL("#pragma multi_compile_fwdbase nolightmap\n#pragma multi_compile A B\n", s); // nolightmap added to fwdbase line, not to user multi_compile
    }

    TEST(AddNoVariantsDirectivesToLastMulticompile_DoesNotAddAnything_ToProgramMultiCompiles)
    {
        SurfaceParams p;
        core::string s;
        p.o.noLightmap = true;
        s = AddNoVariantsDirectivesToLastMulticompile("#pragma multi_compile_vertex A B", p);
        CHECK_EQUAL("#pragma multi_compile_vertex A B", s); // nothing got added
    }

    TEST(AddNoVariantsDirectivesToLastMulticompile_AddsNoLightmap_ToSecondLastMultiCompile)
    {
        SurfaceParams p;
        core::string s;
        p.o.noLightmap = true;
        s = AddNoVariantsDirectivesToLastMulticompile("#pragma multi_compile_fwdbase\n#pragma multi_compile_vertex A B", p);
        CHECK_EQUAL("#pragma multi_compile_fwdbase nolightmap\n#pragma multi_compile_vertex A B", s);
    }

    struct DummyReflectionReport : public ShaderCompilerReflectionReport
    {
        virtual void OnInputBinding(ShaderChannel dataChannel, VertexComponent inputSlot) {}
        virtual void OnConstantBuffer(const char* name, int size, int varCount) {}
        virtual void OnConstant(const char* name, int bindIndex, ShaderParamType dataType, ConstantType constantType, int rows, int cols, int arraySize) {}
        virtual void OnCBBinding(const char* name, int bindIndex) {}
        virtual void OnTextureBinding(const char* name, int bindIndex, int samplerIndex, bool multisampled, TextureDimension dim) {}
        virtual void OnBufferBinding(const char* name, int bindIndex) {}
        virtual void OnStatsInfo(int alu, int tex, int flow, int tempRegisters) {}
        virtual void OnUAVBinding(const char *name, int index, int origIndex) {}
        virtual void OnInlineSampler(InlineSamplerType sampler, int bindIndex) {}
    };

    struct SurfaceShaderPlatformCompilationFixture
    {
        SurfaceShaderPlatformCompilationFixture()
        {
            params.p.entryName[kProgramSurface] = "surf";
            params.o.lighting = "Lambert";
            params.includeContext.includePaths.push_back(g_IncludesPath + "/CGIncludes");

            input.p.entryName[kProgramVertex] = "vert_surf";
            input.p.entryName[kProgramFragment] = "frag_surf";
            input.programType = kProgramFragment;
            input.includeContext.includePaths.push_back(".");
            input.includeContext.includePaths.push_back(g_IncludesPath + "/CGIncludes");
            input.startLine = 0;
            input.reflectionReport = &refl;
        }

        void TestCompilationOk(ShaderCompilerPlatform platform, bool allowWarnings)
        {
            if (!gPluginDispatcher->HasCompilerForAPI(platform))
                return;

            // Grab the Cg shader source between the CGPROGRAM & ENDCG tokens
            core::string tokenStart = "CGPROGRAM";
            core::string tokenEnd = "ENDCG";
            size_t begin = FindTokenInText(res, tokenStart, 0);
            CHECK(begin != core::string::npos);
            size_t end = FindTokenInText(res, tokenEnd, begin);
            CHECK(end != core::string::npos);
            input.source = res.substr(begin + tokenStart.size(), end - (begin + tokenStart.size()));

            // Comment out pragmas
            replace_string(input.source, "#pragma ", "// #pragma ");
            replace_string(input.source, "#pragma\t", "// #pragma\t");

            input.api = platform;
            const ShaderCompileOutputInterface* output = gPluginDispatcher->CompileShader(&input);
            if (allowWarnings)
                CHECK_EQUAL(false, output->HasErrors());
            else
                CHECK_EQUAL(0, output->GetNumErrorsAndWarnings());

            gPluginDispatcher->ReleaseShader(output);

            if (platform == kShaderCompPlatformMetal)
            {
                input.programType = kProgramFragment;
                output = gPluginDispatcher->CompileShader(&input);
                CHECK_EQUAL(0, output->GetNumErrorsAndWarnings());

                gPluginDispatcher->ReleaseShader(output);
            }
        }

        void DoTest(ShaderCompilerPlatform platform, bool allowWarnings = false)
        {
            rawErrors.Clear();
            ProcessSurfaceSnippet(params, res, rawErrors, NULL);
            errors.clear();
            for (ShaderImportErrors::ErrorContainer::const_iterator it = rawErrors.GetErrors().begin(); it != rawErrors.GetErrors().end(); ++it)
                errors.push_back(*it);
            CHECK_EQUAL(0, errors.size());

            TestCompilationOk(platform, allowWarnings);
        }

        SurfaceParams params;
        core::string res;
        ShaderImportErrors rawErrors;

        ShaderCompileInputData input;
        DummyReflectionReport refl;
        std::vector<ShaderImportError> errors;
    };

    // Specific test for case 716161 FPE warning during HLSL shader compilation
    TEST_FIXTURE(SurfaceShaderPlatformCompilationFixture, NeedsViewDirection_False_Does_Not_Trigger_FXC_FPE_Warning)
    {
        // Very specific combination that ensures needs-view-direction = false
        params.o.lighting = "Standard";
        params.o.renderPaths = (1 << kPathForward);
        params.o.noForwardAdd = false;
        params.o.noForwardBase = true;
        params.source =
            "struct Input { float3 viewDir; };\n"
            "void surf (Input IN, inout SurfaceOutputStandard o)\n"
            "{\n"
            "half a = dot(normalize(IN.viewDir), o.Normal);\n"
            "o.Emission += a;\n"
            "}\n";
        DoTest(kShaderCompPlatformD3D11);
        CHECK(res.find("// needs view direction: no") != core::string::npos);
    }

    // Generic test based on 759336 : do not initialize invalid types i.e. struct member variables in Input
    TEST_FIXTURE(SurfaceShaderPlatformCompilationFixture, Members_Of_Surface_Input_Do_Not_Trigger_Compile_Error)
    {
        params.o.lighting = "Standard";
        params.source =
            "struct Dummy { float a; };\n"
            "struct Input {\n"
            "\tDummy dummy;\n"
            "\tbool b1;\n"
            "\tint i1;\n"
            "\tint i5[2];\n"
            "\tint1x1 i6;\n"
            "\tfixed fi1;\n"
            "\thalf h1;\n"
            "\tfloat fl1;\n"
            "\tfloat2 fl2;\n"
            "\tfloat3 fl3;\n"
            "\tfloat4 fl4;\n"
            "\tfloat3x3 fl33;\n"
            "\tfloat farr1[2+1];\n"
            "\tfloat farr2[3*2-1];\n"
            "\tfloat farr3[(int)3.5];\n"
            "};\n"
            "void surf (Input IN, inout SurfaceOutputStandard o)\n"
            "{\n"
            "o.Emission = 1;\n"
            "}\n";
        DoTest(kShaderCompPlatformD3D11);
    }

    // Test adapted from 856507 : Error when compiling surface shader when dynamic loop depending on WorldNormalVector is present
    TEST_FIXTURE(SurfaceShaderPlatformCompilationFixture, Internal_Data_TangentSpaceHelpers_IgnoredForPassesWhichIgnoreTangentSpace)
    {
        params.source =
            "struct Input { float3 worldPos; float3 worldNormal; INTERNAL_DATA };\n"
            "void surf( Input IN, inout SurfaceOutputStandard o ) {\n"
            "\to.Normal = float3( 0, 0, 10 );\n"
            "\tfloat3 worldNormal = WorldNormalVector( IN, o.Normal );\n"
            "\tint steps = (int)dot( worldNormal, IN.worldPos );\n"
            "\tfor (int i = 0; i < steps; ++i) { o.Normal += 0.1; o.Albedo += 0.2; }\n"
            "}";

        params.o.lighting = "Standard";
        params.o.renderPaths = 0;
        params.o.addShadow = true;
        DoTest(kShaderCompPlatformD3D11, true); // loop has no iterations in shadow pass; HLSL produces warning
    }

    TEST_FIXTURE(SurfaceCompilerFixture, SurfaceShaderAnalysis_Defines_SHADER_TARGET_SURFACE_ANALYSIS_To_1)
    {
        params.source =
            "#if !defined(SHADER_TARGET_SURFACE_ANALYSIS)\n"
            "#error SHADER_TARGET_SURFACE_ANALYSIS not defined\n"
            "#endif\n";

        params.o.lighting = "Standard";
        params.o.renderPaths = 0;
        params.o.addShadow = false;
        DoTest();
        CHECK(rawErrors.HasErrorsOrWarnings());
    }

    TEST_FIXTURE(SurfaceCompilerFixture, SurfaceShaderAnalysisDoesNotUndefine_PARALLAXMAP_WhenCompilingForShaderModel30)
    {
        params.source =
            "struct Input {\n"
            "#if defined(_PARALLAXMAP)\n"
            "float parallaxMapOn;\n"
            "#else\n"
            "float parallaxMapOff;\n"
            "#endif\n"
            "};\n"
            "void vert(inout appdata_full v, out Input o) {\n"
            "    UNITY_INITIALIZE_OUTPUT(Input, o);\n"
            "#if defined(_PARALLAXMAP)\n"
            "o.parallaxMapOn = 1.0;\n"
            "#else\n"
            "o.parallaxMapOff = 0.0;\n"
            "#endif\n"
            "}\n"
            "void surf(Input IN, inout SurfaceOutputStandard o) {\n"
            "#if defined(_PARALLAXMAP)\n"
            "o.Albedo.rgb *= IN.parallaxMapOn;\n"
            "#else\n"
            "o.Albedo.rgb *= IN.parallaxMapOff;\n"
            "#endif\n"
            "}\n";

        params.p.entryName[kProgramVertex] = "vert";
        params.o.lighting = "Standard";
        params.o.renderPaths = 0;
        params.o.addShadow = false;
        params.o.pragmaDirectives = "#pragma shader_feature _PARALLAXMAP";
        params.target.m_BaseRequirements = kShaderRequireShaderModel30;
        params.shaderAPIs = 0;
        DoTest();
        CHECK(!rawErrors.HasErrorsOrWarnings());
        CHECK_EQUAL(1, CountOccurrencesInString(res, "customInputData.parallaxMapOn;"));
        CHECK_EQUAL(1, CountOccurrencesInString(res, "customInputData.parallaxMapOff;"));
        CHECK_EQUAL(1, CountOccurrencesInString(res, "surfIN.parallaxMapOn.x = 1.0;"));
        CHECK_EQUAL(1, CountOccurrencesInString(res, "surfIN.parallaxMapOff.x = 1.0;"));
    }

    TEST_FIXTURE(SurfaceCompilerFixture, SurfaceShaderAnalysis_Sets_SHADER_TARGET_FromKeywordRequirement_IfKeywordIsPresent)
    {
        params.source =
            "struct Input {\n"
            "#if defined(_A) && (SHADER_TARGET == 40)\n"
            "float aOn;\n"
            "#elif (SHADER_TARGET == 30)\n"
            "float aOff;\n"
            "#endif\n"
            "};\n"
            "void vert(inout appdata_full v, out Input o) {\n"
            "    UNITY_INITIALIZE_OUTPUT(Input, o);\n"
            "#if defined(_A) && (SHADER_TARGET == 40)\n"
            "o.aOn = 1.0;\n"
            "#elif (SHADER_TARGET == 30)\n"
            "o.aOff = 0.0;\n"
            "#endif\n"
            "}\n"
            "void surf(Input IN, inout SurfaceOutputStandard o) {\n"
            "#if defined(_A) && (SHADER_TARGET == 40)\n"
            "o.Albedo.rgb *= IN.aOn;\n"
            "#elif (SHADER_TARGET == 30)\n"
            "o.Albedo.rgb *= IN.aOff;\n"
            "#endif\n"
            "}\n";

        params.p.entryName[kProgramVertex] = "vert";
        params.o.lighting = "Standard";
        params.o.renderPaths = 0;
        params.o.addShadow = false;
        params.o.pragmaDirectives = "#pragma shader_feature _A";
        params.target.m_BaseRequirements = kShaderRequireShaderModel30;
        params.target.AddKeywordRequirements("_A", kShaderRequireShaderModel40);
        params.shaderAPIs = 0;
        DoTest();
        CHECK(!rawErrors.HasErrorsOrWarnings());
        CHECK_EQUAL(1, CountOccurrencesInString(res, "customInputData.aOn;"));
        CHECK_EQUAL(1, CountOccurrencesInString(res, "customInputData.aOff;"));
        CHECK_EQUAL(1, CountOccurrencesInString(res, "surfIN.aOn.x = 1.0;"));
        CHECK_EQUAL(1, CountOccurrencesInString(res, "surfIN.aOff.x = 1.0;"));
    }

    struct SetSurfaceShaderArgsFixture
    {
        SetSurfaceShaderArgsFixture() : api(SurfaceData::AnalysisShaderCompilerPlatform) {}

        void DoTest()
        {
            SetSurfaceShaderArgs(api, requirements, keywords, args);
        }

        bool CheckArgs(const core::string& name, const core::string& value)
        {
            for (size_t i = 0; i < args.size(); ++i)
                if ((args[i].name == name) && (args[i].value == value))
                    return true;
            return false;
        }

        ShaderArgs args;
        ShaderCompilerPlatform api;
        ShaderCompileRequirements requirements;
        StringArray keywords;
    };

    TEST_FIXTURE(SetSurfaceShaderArgsFixture, SetSurfaceShaderArgs_Sets_SHADER_TARGET_SURFACE_ANALYSIS_To_1)
    {
        DoTest();
        CHECK(CheckArgs("SHADER_TARGET_SURFACE_ANALYSIS", "1"));
    }

    TEST_FIXTURE(SetSurfaceShaderArgsFixture, SetSurfaceShaderArgs_Sets_UNITY_VERSION_To_UnityVersionValue)
    {
        DoTest();
        CHECK(CheckArgs("UNITY_VERSION", IntToString(CreateNumericUnityVersion(UNITY_VERSION_VER, UNITY_VERSION_MAJ, UNITY_VERSION_MIN))));
    }

    TEST_FIXTURE(SetSurfaceShaderArgsFixture, SetSurfaceShaderArgs_Sets_SHADER_API_D3D9_REMOVED_To_1)
    {
        DoTest();
        CHECK(CheckArgs("SHADER_API_D3D9_REMOVED", "1"));
    }

    TEST_FIXTURE(SetSurfaceShaderArgsFixture, SetSurfaceShaderArgs_ByDefault_Sets_SHADER_TARGET_To_25)
    {
        DoTest();
        CHECK(CheckArgs("SHADER_TARGET", "25"));
    }

    TEST_FIXTURE(SetSurfaceShaderArgsFixture, SetSurfaceShaderArgs_Sets_SHADER_TARGET_From_BaseRequirement)
    {
        requirements.m_BaseRequirements = kShaderRequireShaderModel30;
        DoTest();
        CHECK(CheckArgs("SHADER_TARGET", "30"));
    }

    TEST_FIXTURE(SetSurfaceShaderArgsFixture, SetSurfaceShaderArgs_Sets_SHADER_TARGET_From_BaseRequirement_IfKeywordNotPresent)
    {
        requirements.m_BaseRequirements = kShaderRequireShaderModel30;
        requirements.AddKeywordRequirements("_A", kShaderRequireShaderModel40);
        DoTest();
        CHECK(CheckArgs("SHADER_TARGET", "30"));
    }

    TEST_FIXTURE(SetSurfaceShaderArgsFixture, SetSurfaceShaderArgs_Sets_SHADER_TARGET_From_KeywordRequirement_IfKeywordIsPresent)
    {
        requirements.m_BaseRequirements = kShaderRequireShaderModel30;
        requirements.AddKeywordRequirements("_A", kShaderRequireShaderModel40);
        keywords.push_back("_A");
        DoTest();
        CHECK(CheckArgs("SHADER_TARGET", "40"));
    }

    TEST_FIXTURE(SetSurfaceShaderArgsFixture, SetSurfaceShaderArgs_Sets_SHADER_TARGET_From_BaseRequirement_IfBaseRequirementIsHighest)
    {
        requirements.m_BaseRequirements = kShaderRequireShaderModel30;
        requirements.AddKeywordRequirements("_A", kShaderRequireShaderModel20);
        keywords.push_back("_A");
        DoTest();
        CHECK(CheckArgs("SHADER_TARGET", "30"));
    }

    TEST_FIXTURE(SetSurfaceShaderArgsFixture, SetSurfaceShaderArgs_Sets_SHADER_TARGET_From_HighestKeywordRequirement_IfKeywordsArePresent)
    {
        requirements.m_BaseRequirements = kShaderRequireShaderModel30;
        requirements.AddKeywordRequirements("_A", kShaderRequireShaderModel50);
        requirements.AddKeywordRequirements("_B", kShaderRequireShaderModel40);
        keywords.push_back("_A");
        keywords.push_back("_B");
        DoTest();
        CHECK(CheckArgs("SHADER_TARGET", "50"));
    }
}

#endif // ENABLE_UNIT_TESTS
