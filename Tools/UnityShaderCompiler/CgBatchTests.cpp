#include "UnityPrefix.h"
#include "CgBatch.h"
#include "PlatformPlugin.h"
#include "Runtime/Testing/Testing.h"
#include "Runtime/Testing/ParametricTest.h"
#include "Utilities/ProcessIncludes.h"

#if ENABLE_UNIT_TESTS

INTEGRATION_TEST_SUITE(CgBatch)
{
    struct ComputeCompileFixture
    {
        static void CompileErrorCallback(void* userData, CgBatchErrorType type, int platform, int line, const char* file, const char* message)
        {
            ComputeCompileFixture* fixture = static_cast<ComputeCompileFixture*>(userData);
            if (!fixture->errorText.empty())
                fixture->errorText += "\n";
            fixture->errorText += Format("%s (line %d): %s", file ? file : "<Unknown file>", line, message);
        }

        void CheckComputeCompile(const char* src, ShaderCompilerPlatform platform)
        {
            if (gPluginDispatcher->HasCompilerForAPI(platform))
            {
                IncludeSearchContext oldContext = CgBatchGetIncludeSearchContext();
                IncludeSearchContext includeContext;
                includeContext.includePaths.push_back(".");
                includeContext.includePaths.push_back(g_IncludesPath + "/CGIncludes");
                CgBatchSetIncludeSearchContext(includeContext);

                dynamic_array<std::pair<core::string, core::string> > macros;
                // The shader stage and unity version macros come from importer. Mock set them here and see they go
                // through to the compiler. Platform identifiers are still set at the compiler end.
                macros.push_back(std::make_pair("SHADER_STAGE_COMPUTE", "1"));
                macros.push_back(std::make_pair("UNITY_VERSION", "1000"));
                CHECK(CgBatchCompileComputeKernel(src, "", "CSMain", macros, result, platform, kShaderCompFlags_None, CompileErrorCallback, this));

                CgBatchSetIncludeSearchContext(oldContext);
            }

            CHECK(errorText.empty());
        }

        core::string errorText;
        tReturnedVector result;
    };

    PARAMETRIC_TEST_SOURCE(AllComputeShaderPlatforms, (ShaderCompilerPlatform platform))
    {
        PARAMETRIC_TEST_CASE(kShaderCompPlatformD3D11);
        PARAMETRIC_TEST_CASE(kShaderCompPlatformOpenGLCore);
        PARAMETRIC_TEST_CASE(kShaderCompPlatformVulkan);
        PARAMETRIC_TEST_CASE(kShaderCompPlatformGLES3Plus);
        PARAMETRIC_TEST_CASE(kShaderCompPlatformMetal);
#if ENABLE_XBOXONE_COMPILER
        PARAMETRIC_TEST_CASE(kShaderCompPlatformXboxOne);
#endif
#if ENABLE_PSSL_COMPILER
        PARAMETRIC_TEST_CASE(kShaderCompPlatformPS4);
#endif
#if ENABLE_SWITCH_COMPILER
        PARAMETRIC_TEST_CASE(kShaderCompPlatformSwitch);
#endif
    }

    PARAMETRIC_TEST_FIXTURE(ComputeCompileFixture, CompileComputeShader_ShaderAPIMacro_IsPassed, (ShaderCompilerPlatform platform), AllComputeShaderPlatforms)
    {
        const char* src =
            "RWByteAddressBuffer bufferOut;"
            "[numthreads(1,1,1)] void CSMain(uint3 id : SV_DispatchThreadID) {\n"
            "\t#if defined(SHADER_API_D3D11) || defined(SHADER_API_GLCORE) || defined(SHADER_API_METAL) || defined(SHADER_API_VULKAN) || defined(SHADER_API_PS4) || defined(SHADER_API_GLES3) || defined(SHADER_API_SWITCH) || defined(SHADER_API_XBOXONE)\n"
            "\t\tbufferOut.Store(id.x*4, id.x);\n"
            "\t#else\n"
            "\t\terror_SHADER_API_xyz_was_not_defined!;\n"
            "\t#endif\n"
            "}\n";
        CheckComputeCompile(src, platform);
    }

    PARAMETRIC_TEST_FIXTURE(ComputeCompileFixture, CompileComputeShader_UnityVersionMacro_IsPassed, (ShaderCompilerPlatform platform), AllComputeShaderPlatforms)
    {
        const char* src =
            "RWByteAddressBuffer bufferOut;"
            "[numthreads(1,1,1)] void CSMain(uint3 id : SV_DispatchThreadID) {\n"
            "\t#if UNITY_VERSION > 500\n"
            "\t\tbufferOut.Store(id.x*4, id.x);\n"
            "\t#else\n"
            "\t\terror_UNITY_VERSION_was_not_defined!;\n"
            "\t#endif\n"
            "}\n";
        CheckComputeCompile(src, platform);
    }

    PARAMETRIC_TEST_FIXTURE(ComputeCompileFixture, CompileComputeShader_ShaderStageComputeMacro_IsPassed, (ShaderCompilerPlatform platform), AllComputeShaderPlatforms)
    {
        const char* src =
            "RWByteAddressBuffer bufferOut;"
            "[numthreads(1,1,1)] void CSMain(uint3 id : SV_DispatchThreadID) {\n"
            "\t#if SHADER_STAGE_COMPUTE\n"
            "\t\tbufferOut.Store(id.x*4, id.x);\n"
            "\t#else\n"
            "\t\terror_SHADER_STAGE_COMPUTE_was_not_defined!;\n"
            "\t#endif\n"
            "}\n";
        CheckComputeCompile(src, platform);
    }

    PARAMETRIC_TEST_FIXTURE(ComputeCompileFixture, ComputeShader_Includes_RelativePath_WithBackslash, (ShaderCompilerPlatform platform), AllComputeShaderPlatforms)
    {
        const char* fnRoot = "../common.cginc";
        CHECK(WriteTextToFile(fnRoot, "float funcRoot() { return 3; }\n"));

        const char* src =
            "#include \"..\\common.cginc\"\n"
            "RWStructuredBuffer<float> buf;\n"
            "[numthreads(1,1,1)]\n"
            "void CSMain (uint id : SV_DispatchThreadID)\n"
            "{buf[id] = funcRoot();}\n";
        CheckComputeCompile(src, platform);

        DeleteFileAtPath(fnRoot);
    }

    PARAMETRIC_TEST_FIXTURE(ComputeCompileFixture, ComputeShader_Includes_MultiLevel_RelativePaths_WithBackslash, (ShaderCompilerPlatform platform), AllComputeShaderPlatforms)
    {
        CreateDirectoryAtPath("subdir");
        CreateDirectoryAtPath("subdir/nested");

        const char* fnFirst = "subdir/first.cginc";
        CHECK(WriteTextToFile(fnFirst, "#include \"nested\\second.cginc\"\n"));
        const char* fnSecond = "subdir/nested/second.cginc";
        CHECK(WriteTextToFile(fnSecond, "float funcRoot() { return 3; }\n"));

        const char* src =
            "#include \"subdir\\first.cginc\"\n"
            "RWStructuredBuffer<float> buf;"
            "[numthreads(1,1,1)]\n"
            "void CSMain (uint id : SV_DispatchThreadID)\n"
            "{buf[id] = funcRoot();}\n";
        CheckComputeCompile(src, platform);

        DeleteFileAtPath(fnFirst);
        DeleteFileAtPath(fnSecond);
        DeleteDirectoryAtPath("subdir/nested");
        DeleteDirectoryAtPath("subdir");
    }
}

#endif // ENABLE_UNIT_TESTS
