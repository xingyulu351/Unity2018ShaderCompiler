#include "UnityPrefix.h"
#if ENABLE_UNIT_TESTS

#include "MetalCompilerUtils.h"
#include "ShaderCompiler.h"
#include "Tools/UnityShaderCompiler/Utilities/ShaderImportErrors.h"
#include "Runtime/Testing/Testing.h"

#include <string>

// TODO: what tests are still needed:
// error handling

UNIT_TEST_SUITE(MetalCompiler)
{
    TEST(ProcessMetalUniforms_Simple)
    {
        const core::string input[] =
        {
            // test vector component count
            "METAL_CONST_VECTOR(float, 1, test);",
            "METAL_CONST_VECTOR(float, 2, test);",
            "METAL_CONST_VECTOR(float, 3, test);",
            "METAL_CONST_VECTOR(float, 4, test);",

            // test vector types
            "METAL_CONST_VECTOR(int, 1, test);",
            "METAL_CONST_VECTOR(bool, 2, test);",
            "METAL_CONST_VECTOR(half, 3, test);",
            "METAL_CONST_VECTOR(short, 4, test);",
            "METAL_CONST_VECTOR(uint, 1, test);",

            // test matrix component count
            "METAL_CONST_MATRIX(float, 2, 2, test);",
            "METAL_CONST_MATRIX(float, 2, 3, test);",
            "METAL_CONST_MATRIX(float, 2, 4, test);",

            "METAL_CONST_MATRIX(float, 3, 2, test);",
            "METAL_CONST_MATRIX(float, 3, 3, test);",
            "METAL_CONST_MATRIX(float, 3, 4, test);",

            "METAL_CONST_MATRIX(float, 4, 2, test);",
            "METAL_CONST_MATRIX(float, 4, 3, test);",
            "METAL_CONST_MATRIX(float, 4, 4, test);",

            // test matrix types
            "METAL_CONST_MATRIX(half, 4, 4, test);",
        };
        const core::string checkOutput[] =
        {
            // test vector component count
            "float test;",
            "float2 test;",
            "float3 test;",
            "float4 test;",

            // test vector types
            "int test;",
            "bool2 test;",
            "half3 test;",
            "short4 test;",
            "uint test;",

            // test matrix component count
            "float2x2 test;",
            "float3x2 test;",
            "float4x2 test;",

            "float2x3 test;",
            "float3x3 test;",
            "float4x3 test;",

            "float2x4 test;",
            "float3x4 test;",
            "float4x4 test;",

            // test matrix types
            "half4x4 test;",
        };
        const core::string checkBind[] =
        {
            // check CgTestReflectionImpl::OnConstant for format

            // test vector component count
            "test: 0 dtype0 ktype0 0x1[0]\n",
            "test: 0 dtype0 ktype0 0x2[0]\n",
            "test: 0 dtype0 ktype0 0x3[0]\n",
            "test: 0 dtype0 ktype0 0x4[0]\n",

            // test vector types
            "test: 0 dtype1 ktype0 0x1[0]\n",
            "test: 0 dtype2 ktype0 0x2[0]\n",
            "test: 0 dtype3 ktype0 0x3[0]\n",
            "test: 0 dtype4 ktype0 0x4[0]\n",
            "test: 0 dtype5 ktype0 0x1[0]\n",

            // test matrix component count
            "test: 0 dtype0 ktype1 2x2[0]\n",
            "test: 0 dtype0 ktype1 2x3[0]\n",
            "test: 0 dtype0 ktype1 2x4[0]\n",

            "test: 0 dtype0 ktype1 3x2[0]\n",
            "test: 0 dtype0 ktype1 3x3[0]\n",
            "test: 0 dtype0 ktype1 3x4[0]\n",

            "test: 0 dtype0 ktype1 4x2[0]\n",
            "test: 0 dtype0 ktype1 4x3[0]\n",
            "test: 0 dtype0 ktype1 4x4[0]\n",

            // test matrix types
            "test: 0 dtype3 ktype1 4x4[0]\n",
        };
        CompileTimeAssert(ARRAY_SIZE(input) == ARRAY_SIZE(checkOutput) && ARRAY_SIZE(input) == ARRAY_SIZE(checkBind), "test cases out of sync");

        ShaderImportErrors errors;
        for (unsigned i = 0, n = ARRAY_SIZE(input); i < n; ++i)
        {
            CgTestReflectionImpl refl;

            core::string code = input[i];
            ProcessMetalUniforms(code, "Globals", &refl, errors);

            CHECK_EQUAL(false, errors.HasErrors());
            CHECK_EQUAL(checkOutput[i], code);
            CHECK_EQUAL(checkBind[i], refl.constants);
        }
    }

    TEST(ProcessMetalUniforms_Align)
    {
        // TODO: maybe we also want similar for const buffer size (tests proper size of last member)
        // TODO: more cases (i am a bit lax because all vectors/matrices of all types go through same codepath)
        const core::string input[] =
        {
            "METAL_CONST_VECTOR(float, 1, test1);METAL_CONST_VECTOR(float, 1, test2);",
            "METAL_CONST_VECTOR(float, 1, test1);METAL_CONST_VECTOR(float, 2, test2);",
            "METAL_CONST_VECTOR(float, 1, test1);METAL_CONST_VECTOR(float, 3, test2);",
            "METAL_CONST_VECTOR(float, 1, test1);METAL_CONST_VECTOR(float, 4, test2);",

            "METAL_CONST_VECTOR(float, 1, test1);METAL_CONST_VECTOR(half, 1, test2);",
            "METAL_CONST_VECTOR(float, 1, test1);METAL_CONST_VECTOR(half, 2, test2);",
            "METAL_CONST_VECTOR(float, 1, test1);METAL_CONST_VECTOR(half, 3, test2);",
            "METAL_CONST_VECTOR(float, 1, test1);METAL_CONST_VECTOR(half, 4, test2);",

            "METAL_CONST_VECTOR(bool, 1, test1);METAL_CONST_VECTOR(int, 1, test2);",
            "METAL_CONST_VECTOR(bool, 3, test1);METAL_CONST_VECTOR(int, 1, test2);",
            "METAL_CONST_VECTOR(bool, 4, test1);METAL_CONST_VECTOR(int, 1, test2);",
            "METAL_CONST_VECTOR(bool, 1, test1);METAL_CONST_VECTOR(short, 1, test2);",
            "METAL_CONST_VECTOR(bool, 2, test1);METAL_CONST_VECTOR(short, 1, test2);",
            "METAL_CONST_VECTOR(bool, 1, test1);METAL_CONST_VECTOR(bool, 1, test2);",

            "METAL_CONST_VECTOR(bool, 1, test1);METAL_CONST_MATRIX(float, 4, 4, test2);",
        };
        const core::string checkOutput[] =
        {
            "float test1;float test2;",
            "float test1;float2 test2;",
            "float test1;float3 test2;",
            "float test1;float4 test2;",

            "float test1;half test2;",
            "float test1;half2 test2;",
            "float test1;half3 test2;",
            "float test1;half4 test2;",

            "bool test1;int test2;",
            "bool3 test1;int test2;",
            "bool4 test1;int test2;",
            "bool test1;short test2;",
            "bool2 test1;short test2;",
            "bool test1;bool test2;",

            "bool test1;float4x4 test2;",
        };
        const core::string checkBind[] =
        {
            // check CgTestReflectionImpl::OnConstant for format

            "test1: 0 dtype0 ktype0 0x1[0]\ntest2: 4 dtype0 ktype0 0x1[0]\n",
            "test1: 0 dtype0 ktype0 0x1[0]\ntest2: 8 dtype0 ktype0 0x2[0]\n",
            "test1: 0 dtype0 ktype0 0x1[0]\ntest2: 16 dtype0 ktype0 0x3[0]\n",
            "test1: 0 dtype0 ktype0 0x1[0]\ntest2: 16 dtype0 ktype0 0x4[0]\n",

            "test1: 0 dtype0 ktype0 0x1[0]\ntest2: 4 dtype3 ktype0 0x1[0]\n",
            "test1: 0 dtype0 ktype0 0x1[0]\ntest2: 4 dtype3 ktype0 0x2[0]\n",
            "test1: 0 dtype0 ktype0 0x1[0]\ntest2: 8 dtype3 ktype0 0x3[0]\n",
            "test1: 0 dtype0 ktype0 0x1[0]\ntest2: 8 dtype3 ktype0 0x4[0]\n",

            "test1: 0 dtype2 ktype0 0x1[0]\ntest2: 4 dtype1 ktype0 0x1[0]\n",
            "test1: 0 dtype2 ktype0 0x3[0]\ntest2: 4 dtype1 ktype0 0x1[0]\n",
            "test1: 0 dtype2 ktype0 0x4[0]\ntest2: 4 dtype1 ktype0 0x1[0]\n",
            "test1: 0 dtype2 ktype0 0x1[0]\ntest2: 2 dtype4 ktype0 0x1[0]\n",
            "test1: 0 dtype2 ktype0 0x2[0]\ntest2: 2 dtype4 ktype0 0x1[0]\n",
            "test1: 0 dtype2 ktype0 0x1[0]\ntest2: 1 dtype2 ktype0 0x1[0]\n",

            "test1: 0 dtype2 ktype0 0x1[0]\ntest2: 16 dtype0 ktype1 4x4[0]\n",
        };
        CompileTimeAssert(ARRAY_SIZE(input) == ARRAY_SIZE(checkOutput) && ARRAY_SIZE(input) == ARRAY_SIZE(checkBind), "test cases out of sync");

        ShaderImportErrors errors;
        for (unsigned i = 0, n = ARRAY_SIZE(input); i < n; ++i)
        {
            CgTestReflectionImpl refl;

            core::string code = input[i];
            ProcessMetalUniforms(code, "Globals", &refl, errors);

            CHECK_EQUAL(false, errors.HasErrors());
            CHECK_EQUAL(checkOutput[i], code);
            CHECK_EQUAL(checkBind[i], refl.constants);
        }
    }

    TEST(ProcessMetalTextures_Simple)
    {
        const core::string input[] =
        {
            "METAL_TEX_INPUT(texture2d<half, access::sample>, 0, test),",
            "METAL_TEX_INPUT(texture2d<float>, 1, test),",
            "METAL_TEX_INPUT(texture3d<half>, 2, test),",
            "METAL_TEX_INPUT(texturecube<half>, 3, test),",
            "METAL_TEX_INPUT(depth2d<float>, 4, test),",
        };
        const core::string checkOutput[] =
        {
            "texture2d<half, access::sample> test [[texture(0)]], sampler sampler_test [[sampler(0)]],",
            "texture2d<float> test [[texture(1)]], sampler sampler_test [[sampler(1)]],",
            "texture3d<half> test [[texture(2)]], sampler sampler_test [[sampler(2)]],",
            "texturecube<half> test [[texture(3)]], sampler sampler_test [[sampler(3)]],",
            "depth2d<float> test [[texture(4)]], sampler sampler_test [[sampler(4)]],",
        };
        const core::string checkBind[] =
        {
            // check CgTestReflectionImpl::OnTextureBinding for format
            "SetTexture 0 [test] 2D\n",
            "SetTexture 1 [test] 2D\n",
            "SetTexture 2 [test] 3D\n",
            "SetTexture 3 [test] CUBE\n",
            "SetTexture 4 [test] 2D\n",
        };
        CompileTimeAssert(ARRAY_SIZE(input) == ARRAY_SIZE(checkOutput) && ARRAY_SIZE(input) == ARRAY_SIZE(checkBind), "test cases out of sync");

        ShaderImportErrors errors;
        for (unsigned i = 0, n = ARRAY_SIZE(input); i < n; ++i)
        {
            CgTestReflectionImpl refl;

            core::string code = input[i];
            ProcessMetalTextures(code, &refl, errors);

            CHECK_EQUAL(false, errors.HasErrors());
            CHECK_EQUAL(checkOutput[i], code);
            CHECK_EQUAL(checkBind[i], refl.bindings);
        }
    }

    TEST(ProcessMetalBuffers_Simple)
    {
        const core::string input[] =
        {
            "METAL_BUFFER_INPUT(TestStruct, 1, test)",
            "METAL_BUFFER_INPUT_DATA(TestStruct, test)",
        };
        const core::string checkOutput[] =
        {
            "const device TestStruct* test_inbuf [[buffer(1)]]",
            "const device TestStruct* test = reinterpret_cast<const device TestStruct*>(reinterpret_cast<const device atomic_uint*>(test_inbuf) + 1);\n",
        };
        const core::string checkBind[] =
        {
            // check CgTestReflectionImpl::OnBufferBinding for format
            "SetBuffer 1 test\n",
            "",
        };
        CompileTimeAssert(ARRAY_SIZE(input) == ARRAY_SIZE(checkOutput) && ARRAY_SIZE(input) == ARRAY_SIZE(checkBind), "test cases out of sync");

        ShaderImportErrors errors;
        for (unsigned i = 0, n = ARRAY_SIZE(input); i < n; ++i)
        {
            CgTestReflectionImpl refl;

            core::string code = input[i];
            ProcessMetalBuffers(code, &refl, errors);

            CHECK_EQUAL(false, errors.HasErrors());
            CHECK_EQUAL(checkOutput[i], code);
            CHECK_EQUAL(checkBind[i], refl.bindings);
        }
    }


    TEST(CorrectExtractionOfTags)
    {
        // we do uniforms only because internally code that we test here is fully shared.
        // if you care enough (or change the "shared" fact)
        // you are welcome to or create separate test, or just array with function pointer (the signatures are compatible) or whatever rubs you the right way
        const core::string input[] =
        {
            "METAL_CONST_VECTOR(float, 1, test);\n",
            "METAL_CONST_VECTOR                            (float, 1, test);\n",
            "METAL_CONST_VECTOR\t\t \t\t \t(float, 1, test);\n",
            "METAL_CONST_VECTOR(   float\t\t\t, 1, test);\n",
            "METAL_CONST_VECTOR(float, 1,  \t\t  test\t\t  );\n",
            "METAL_CONST_VECTOR(float,\n1,\ntest\n);",
            "METAL_CONST_VECTOR(float, 1, test)\n",
            "METAL_CONST_VECTOR(float, 1, test), METAL_CONST_VECTOR(float, 2, test2)\n",
        };
        const core::string checkOutput[] =
        {
            "float test;\n",
            "float test;\n",
            "float test;\n",
            "float test;\n",
            "float test;\n",
            "float test;",
            "float test\n",
            "float test, float2 test2\n",
        };
        CompileTimeAssert(ARRAY_SIZE(input) == ARRAY_SIZE(checkOutput), "test cases out of sync");

        ShaderImportErrors errors;
        for (unsigned i = 0, n = ARRAY_SIZE(input); i < n; ++i)
        {
            CgTestReflectionImpl refl;

            core::string code = input[i];
            ProcessMetalUniforms(code, "Globals", &refl, errors);

            CHECK_EQUAL(false, errors.HasErrors());
            CHECK_EQUAL(checkOutput[i], code);
        }
    }
}

#endif // ENABLE_UNIT_TESTS
