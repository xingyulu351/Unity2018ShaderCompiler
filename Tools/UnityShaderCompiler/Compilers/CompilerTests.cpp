#include "UnityPrefix.h"

#if ENABLE_UNIT_TESTS

#include "Runtime/Testing/ParametricTest.h"
#include "Runtime/Testing/Testing.h"
#include "Tools/UnityShaderCompiler/APIPlugin.h"
#include "Tools/UnityShaderCompiler/PlatformPlugin.h"
#include "Tools/UnityShaderCompiler/ShaderCompiler.h"


ShaderCompileOutputData* InvokeCompilerHLSL(const ShaderCompileInputInterface* input);
ShaderCompileOutputInterface* InvokeCompilerHLSLcc(const ShaderCompileInputInterface* input);
ShaderCompileOutputInterface* InvokeCompilerGlslang(const ShaderCompileInputInterface* input);

INTEGRATION_TEST_SUITE(Compiler)
{
    struct TestFixture
    {
        TestFixture()
        {
            refl.statsAlu = refl.statsTex = refl.statsFlow = refl.statsTempRegister = -1;
            input.source =
                "struct appdata { float4 vertex:POSITION; };\n"
                "float4 vert (appdata v) : POSITION { return v.vertex; }\n"
                "half4 frag () : COLOR { return 1; }";
            input.p.entryName[kProgramVertex] = "vert";
            input.p.entryName[kProgramFragment] = "frag";
        }

        bool Test(ShaderCompilerProgram type, ShaderCompilerPlatform api)
        {
            input.api = api;
            input.programType = type;
            input.reflectionReport = &refl;

            ShaderCompileOutputInterface* output;
            if (IsD3D11ShaderCompiler(api))
                output = InvokeCompilerHLSL(&input);
            else if (api == kShaderCompPlatformVulkan)
                output = InvokeCompilerGlslang(&input);
            else
                output = InvokeCompilerHLSLcc(&input);
            bool hasErrors = output->GetNumErrorsAndWarnings() > 0;
            delete output;
            return !hasErrors;
        }

        size_t TestSize(ShaderCompilerProgram type, ShaderCompilerPlatform api)
        {
            input.api = api;
            input.programType = type;
            input.reflectionReport = &refl;

            ShaderCompileOutputInterface* output;
            if (api == kShaderCompPlatformVulkan)
                output = InvokeCompilerGlslang(&input);
            else if ((api == kShaderCompPlatformPS4)
                     || (api == kShaderCompPlatformXboxOne)
                     || (api == kShaderCompPlatformXboxOneD3D12)
                     || (api == kShaderCompPlatformSwitch))
                output = (ShaderCompileOutputInterface*)gPluginDispatcher->CompileShader(&input);
            else if (IsD3D11ShaderCompiler(api)) // This includes also xbox platforms, therefore the correct invoke for them is handled above
                output = InvokeCompilerHLSL(&input);
            else
                output = InvokeCompilerHLSLcc(&input);

            if (output == NULL) // check for no generated output, e.g. no compiler
                return 0;

            size_t binarysize = output->GetCompiledDataSize();
            delete output;
            return binarysize;
        }

        ShaderCompileInputData input;
        CgTestReflectionImpl refl;
    };

    PARAMETRIC_TEST_SOURCE(AllHLSLCompilerPlatforms, (ShaderCompilerPlatform platform))
    {
        PARAMETRIC_TEST_CASE(kShaderCompPlatformD3D11);
    }

    PARAMETRIC_TEST_SOURCE(AllDebugInfoCompilerPlatforms, (ShaderCompilerPlatform platform))
    {
        PARAMETRIC_TEST_CASE(kShaderCompPlatformD3D11);
#if ENABLE_XBOXONE_COMPILER
        PARAMETRIC_TEST_CASE(kShaderCompPlatformXboxOneD3D12);
#endif
#if ENABLE_PSSL_COMPILER
        PARAMETRIC_TEST_CASE(kShaderCompPlatformPS4);
#endif
    }

    PARAMETRIC_TEST_FIXTURE(TestFixture, BasicVertexShader_Works, (ShaderCompilerPlatform platform), AllHLSLCompilerPlatforms)
    {
        CHECK(Test(kProgramVertex, platform));
        CHECK_EQUAL("Bind \"vertex\" Vertex\n", refl.bindings);
        CHECK_EQUAL(0, refl.statsAlu);
        CHECK_EQUAL(0, refl.statsTex);
        CHECK_EQUAL(0, refl.statsFlow);
    }
    PARAMETRIC_TEST_FIXTURE(TestFixture, BasicPixelShader_Works, (ShaderCompilerPlatform platform), AllHLSLCompilerPlatforms)
    {
        CHECK(Test(kProgramFragment, platform));
        CHECK_EQUAL(0, refl.statsAlu);
        CHECK_EQUAL(0, refl.statsTex);
        CHECK_EQUAL(0, refl.statsFlow);
    }

    PARAMETRIC_TEST_FIXTURE(TestFixture, ShaderInstructionStats_CountedProperly, (ShaderCompilerPlatform platform), AllHLSLCompilerPlatforms)
    {
        input.source =
            "float4 vert (float3 pos : POSITION, float3 normal : NORMAL) : SV_Position {\n"
            "\tfloat4 res = float4(pos + normal, exp(pos.x));\n"
            "\t[branch]\n"
            "\tif (pos.x < normal.x)\n"
            "\t\tres.x += sin(normal.y);\n"
            "\treturn res;\n"
            "}\n"
            "sampler2D _tex;\n"
            "half4 frag (float2 uv : TEXCOORD0) : SV_Target {\n"
            "\treturn tex2D(_tex, uv) * 0.5 + 0.5;\n"
            "}";

        CHECK(Test(kProgramVertex, platform));
        CHECK_EQUAL(6, refl.statsAlu);
        CHECK_EQUAL(0, refl.statsTex);
        CHECK_EQUAL(1, refl.statsFlow);
        CHECK_EQUAL(2, refl.statsTempRegister);

        CHECK(Test(kProgramFragment, platform));
        CHECK_EQUAL(1, refl.statsAlu);
        CHECK_EQUAL(1, refl.statsTex);
        CHECK_EQUAL(0, refl.statsFlow);
        CHECK_EQUAL(1, refl.statsTempRegister);
    }

    PARAMETRIC_TEST_FIXTURE(TestFixture, ShaderInstructionStats_DebugCountedProperly, (ShaderCompilerPlatform platform), AllDebugInfoCompilerPlatforms)
    {
        input.source =
            "float4 vert (float3 pos : POSITION, float3 normal : NORMAL) : SV_Position {\n"
            "\tfloat4 res = float4(pos + normal, exp(pos.x));\n"
            "\t[branch]\n"
            "\tif (pos.x < normal.x)\n"
            "\t\tres.x += sin(normal.y);\n"
            "\treturn res;\n"
            "}\n"
            "sampler2D _tex;\n"
            "half4 frag (float2 uv : TEXCOORD0) : SV_Target {\n"
            "\treturn tex2D(_tex, uv) * 0.5 + 0.5;\n"
            "}";

        input.compileFlags = kShaderCompFlags_EnableDebugInfoD3D11;
        size_t vertexDbgBinarySize = TestSize(kProgramVertex, platform);

        input.compileFlags = kShaderCompFlags_None;
        size_t vertexBinarySize = TestSize(kProgramVertex, platform);

        CHECK((vertexBinarySize < vertexDbgBinarySize) || (vertexBinarySize == 0));
    }


    PARAMETRIC_TEST_SOURCE(AllStructRecognizableCompilerPlatforms, (ShaderCompilerPlatform platform))
    {
        PARAMETRIC_TEST_CASE(kShaderCompPlatformD3D11);
        PARAMETRIC_TEST_CASE(kShaderCompPlatformMetal);
        PARAMETRIC_TEST_CASE(kShaderCompPlatformVulkan);
        //PARAMETRIC_TEST_CASE(kShaderCompPlatformOpenGLCore); // opengl doesn't produce reflection immediately
    }

    PARAMETRIC_TEST_FIXTURE(TestFixture, RecognizeStructs, (ShaderCompilerPlatform platform), AllStructRecognizableCompilerPlatforms)
    {
        input.source =
            "cbuffer UnityInstancingCB {\n"
            "\tstruct InstanceData {\n"
            "\t\tfloat4x4 world;\n"
            "\t\tfloat3 offset;\n"
            "\t} _instanceData[16];\n"
            "}\n"
            "float4 vert (float3 pos : POSITION, uint instanceID : SV_InstanceID) : SV_Position {\n"
            "\tfloat4 res = mul(_instanceData[instanceID].world, float4(pos, 1));\n"
            "\tres.xyz += _instanceData[instanceID].offset;\n"
            "\treturn res;\n"
            "}"
            "float4 frag () : SV_Target {\n"
            "\treturn float4(1,1,1,1);\n"
            "}";
        CHECK(Test(kProgramVertex, platform));

#define STRUCT_REFL         "_instanceData: 0 dtype0 ktype2 80x1[16]\n"         // offset=0, dataType=float, constantType=struct, structSize=80, arraySize=16
#define STRUCT_WORLD_REFL   "_instanceData.world: 0 dtype0 ktype1 4x4[0]\n"     // offset=0, dataType=float, constantType=matrix, rows=4, cols=4, arraySize=0
#define STRUCT_OFFSET_REFL  "_instanceData.offset: 64 dtype0 ktype0 1x3[0]\n"   // offset=64, dataType=float, constantType=default, rows=1, cols=3, arraySize=0

        const core::string expectedConstantsReport = platform == kShaderCompPlatformVulkan
            ? STRUCT_REFL STRUCT_OFFSET_REFL STRUCT_WORLD_REFL
            : STRUCT_REFL STRUCT_WORLD_REFL STRUCT_OFFSET_REFL;
        CHECK_EQUAL(expectedConstantsReport, refl.constants);
    }
}

#endif // #if ENABLE_UNIT_TESTS
