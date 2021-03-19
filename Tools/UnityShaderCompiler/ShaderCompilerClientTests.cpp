#include "UnityPrefix.h"

#if ENABLE_UNIT_TESTS

#include "ShaderCompilerClient.h"
#include "Runtime/Shaders/ShaderKeywords.h"
#include "Runtime/Testing/Testing.h"
#include "Runtime/Testing/ParametricTest.h"

INTEGRATION_TEST_SUITE(ShaderCompilerClient)
{
    struct PlatformsOptionsAndKeywordsData
    {
        BuildTargetPlatformGroup platformGroup;
        ShaderCompilerPlatform shaderCompiler;
        StereoRenderingPath stereoRenderPath;
        bool enableStereo360Capture;
        std::vector<core::string> expectedPlatformVariants;
    };

    core::string GetStereoRenderPathNameForType(StereoRenderingPath stereoRenderingPathType)
    {
        switch (stereoRenderingPathType)
        {
            case kStereoRenderingSinglePass:
                return "stereoRenderingSinglePass";
                break;
            case kStereoRenderingInstancing:
                return "stereoRenderingInstancing";
                break;
            default:
            {
                AssertString("StereoRenderingPathTypeUndefined");
                return "";
            }
        }
    }

    core::string BuildTestNameFromTestParams(PlatformsOptionsAndKeywordsData platformOptionsAndKeywords)
    {
        core::string testName;
        testName += GetBuildTargetGroupDisplayName(platformOptionsAndKeywords.platformGroup);
        testName.append("_");
        testName += ShaderCompilerGetPlatformName(platformOptionsAndKeywords.shaderCompiler);
        testName.append("_");
        testName += GetStereoRenderPathNameForType(platformOptionsAndKeywords.stereoRenderPath);
        testName.append("_");

        if (platformOptionsAndKeywords.enableStereo360Capture)
            testName += "enableStereo360Capture";
        else
            testName += "disableStereo360Capture";

        return testName;
    }

    void TestCases_Standalone_BuildGroupPlatform(Testing::TestCaseEmitter<PlatformsOptionsAndKeywordsData>& testCase)
    {
        bool enableStereo360Capture = true;

        PlatformsOptionsAndKeywordsData testData[] =
        {
            { kPlatformStandalone, kShaderCompPlatformD3D11, kStereoRenderingInstancing, enableStereo360Capture, { "", "STEREO_CUBEMAP_RENDER_ON", "STEREO_INSTANCING_ON", "STEREO_CUBEMAP_RENDER_ON STEREO_INSTANCING_ON" } },
            { kPlatformStandalone, kShaderCompPlatformD3D11, kStereoRenderingInstancing, !enableStereo360Capture, { "", "STEREO_INSTANCING_ON"} },
            { kPlatformStandalone, kShaderCompPlatformOpenGLCore, kStereoRenderingInstancing, enableStereo360Capture, { "", "STEREO_CUBEMAP_RENDER_ON", "STEREO_INSTANCING_ON", "STEREO_CUBEMAP_RENDER_ON STEREO_INSTANCING_ON" } },
            { kPlatformStandalone, kShaderCompPlatformOpenGLCore, kStereoRenderingInstancing, !enableStereo360Capture, { "", "STEREO_INSTANCING_ON" } },
            { kPlatformStandalone, kShaderCompPlatformVulkan, kStereoRenderingInstancing, enableStereo360Capture, { "", "STEREO_CUBEMAP_RENDER_ON" } },
            { kPlatformStandalone, kShaderCompPlatformVulkan, kStereoRenderingInstancing, !enableStereo360Capture, { "" } },
            { kPlatformStandalone, kShaderCompPlatformD3D11, kStereoRenderingSinglePass, enableStereo360Capture, { "", "STEREO_CUBEMAP_RENDER_ON", "UNITY_SINGLE_PASS_STEREO", "STEREO_CUBEMAP_RENDER_ON UNITY_SINGLE_PASS_STEREO" } },
            { kPlatformStandalone, kShaderCompPlatformD3D11, kStereoRenderingSinglePass, !enableStereo360Capture, { "", "UNITY_SINGLE_PASS_STEREO" } },
            { kPlatformStandalone, kShaderCompPlatformOpenGLCore, kStereoRenderingSinglePass, enableStereo360Capture, { "", "STEREO_CUBEMAP_RENDER_ON", "UNITY_SINGLE_PASS_STEREO", "STEREO_CUBEMAP_RENDER_ON UNITY_SINGLE_PASS_STEREO" } },
            { kPlatformStandalone, kShaderCompPlatformOpenGLCore, kStereoRenderingSinglePass, !enableStereo360Capture, { "", "UNITY_SINGLE_PASS_STEREO" } },
            { kPlatformStandalone, kShaderCompPlatformVulkan, kStereoRenderingSinglePass, enableStereo360Capture, { "", "STEREO_CUBEMAP_RENDER_ON", "UNITY_SINGLE_PASS_STEREO", "STEREO_CUBEMAP_RENDER_ON UNITY_SINGLE_PASS_STEREO" } },
            { kPlatformStandalone, kShaderCompPlatformVulkan, kStereoRenderingSinglePass, !enableStereo360Capture, { "" , "UNITY_SINGLE_PASS_STEREO" } },
        };

        for (int i = 0; i < ARRAY_SIZE(testData); i++)
            testCase.WithName(BuildTestNameFromTestParams(testData[i])).WithValues(testData[i]);
    }

    void TestCases_Metro_BuildGroupPlatform(Testing::TestCaseEmitter<PlatformsOptionsAndKeywordsData>& testCase)
    {
        bool enableStereo360Capture = true;

        PlatformsOptionsAndKeywordsData testData[] =
        {
            { kPlatformMetro, kShaderCompPlatformD3D11, kStereoRenderingInstancing, enableStereo360Capture, { "", "STEREO_CUBEMAP_RENDER_ON", "STEREO_INSTANCING_ON", "STEREO_CUBEMAP_RENDER_ON STEREO_INSTANCING_ON" } },
            { kPlatformMetro, kShaderCompPlatformD3D11, kStereoRenderingInstancing, !enableStereo360Capture, { "", "STEREO_INSTANCING_ON" } },
            { kPlatformMetro, kShaderCompPlatformOpenGLCore, kStereoRenderingInstancing, enableStereo360Capture, { "", "STEREO_CUBEMAP_RENDER_ON", "STEREO_INSTANCING_ON", "STEREO_CUBEMAP_RENDER_ON STEREO_INSTANCING_ON" } },
            { kPlatformMetro, kShaderCompPlatformOpenGLCore, kStereoRenderingInstancing, !enableStereo360Capture, { "", "STEREO_INSTANCING_ON" } },
            { kPlatformMetro, kShaderCompPlatformVulkan, kStereoRenderingInstancing, enableStereo360Capture, { "", "STEREO_CUBEMAP_RENDER_ON" } },
            { kPlatformMetro, kShaderCompPlatformVulkan, kStereoRenderingInstancing, !enableStereo360Capture, { "" } },
            { kPlatformMetro, kShaderCompPlatformD3D11, kStereoRenderingSinglePass, enableStereo360Capture, { "", "STEREO_CUBEMAP_RENDER_ON", "UNITY_SINGLE_PASS_STEREO", "STEREO_CUBEMAP_RENDER_ON UNITY_SINGLE_PASS_STEREO" } },
            { kPlatformMetro, kShaderCompPlatformD3D11, kStereoRenderingSinglePass, !enableStereo360Capture, { "", "UNITY_SINGLE_PASS_STEREO" } },
            { kPlatformMetro, kShaderCompPlatformOpenGLCore, kStereoRenderingSinglePass, enableStereo360Capture, { "", "STEREO_CUBEMAP_RENDER_ON", "UNITY_SINGLE_PASS_STEREO", "STEREO_CUBEMAP_RENDER_ON UNITY_SINGLE_PASS_STEREO" } },
            { kPlatformMetro, kShaderCompPlatformOpenGLCore, kStereoRenderingSinglePass, !enableStereo360Capture, { "", "UNITY_SINGLE_PASS_STEREO" } },
            { kPlatformMetro, kShaderCompPlatformVulkan, kStereoRenderingSinglePass, enableStereo360Capture, { "", "UNITY_SINGLE_PASS_STEREO", "STEREO_CUBEMAP_RENDER_ON", "STEREO_CUBEMAP_RENDER_ON UNITY_SINGLE_PASS_STEREO" } },
            { kPlatformMetro, kShaderCompPlatformVulkan, kStereoRenderingSinglePass, !enableStereo360Capture, { "", "UNITY_SINGLE_PASS_STEREO" } },
        };

        for (int i = 0; i < ARRAY_SIZE(testData); i++)
            testCase.WithName(BuildTestNameFromTestParams(testData[i])).WithValues(testData[i]);
    }

    void TestCases_Android_BuildGroupPlatform(Testing::TestCaseEmitter<PlatformsOptionsAndKeywordsData>& testCase)
    {
        bool enableStereo360Capture = true;

        PlatformsOptionsAndKeywordsData testData[] =
        {
            { kPlatformAndroid, kShaderCompPlatformGLES20, kStereoRenderingInstancing, enableStereo360Capture, { "", "STEREO_CUBEMAP_RENDER_ON" } },
            { kPlatformAndroid, kShaderCompPlatformGLES20, kStereoRenderingInstancing, !enableStereo360Capture, { "" } },
            { kPlatformAndroid, kShaderCompPlatformGLES3Plus, kStereoRenderingInstancing, enableStereo360Capture, { "", "STEREO_CUBEMAP_RENDER_ON" } },
            { kPlatformAndroid, kShaderCompPlatformGLES3Plus, kStereoRenderingInstancing, !enableStereo360Capture, { "", } },
            { kPlatformAndroid, kShaderCompPlatformVulkan, kStereoRenderingInstancing, enableStereo360Capture, { "", "STEREO_CUBEMAP_RENDER_ON" } },
            { kPlatformAndroid, kShaderCompPlatformVulkan, kStereoRenderingInstancing, !enableStereo360Capture, { "" } },
            { kPlatformAndroid, kShaderCompPlatformGLES20, kStereoRenderingSinglePass, enableStereo360Capture, { "", "STEREO_CUBEMAP_RENDER_ON" } },
            { kPlatformAndroid, kShaderCompPlatformGLES20, kStereoRenderingSinglePass, !enableStereo360Capture, { "" } },
            { kPlatformAndroid, kShaderCompPlatformGLES3Plus, kStereoRenderingSinglePass, enableStereo360Capture, { "", "STEREO_CUBEMAP_RENDER_ON", "STEREO_MULTIVIEW_ON", "STEREO_INSTANCING_ON", "STEREO_CUBEMAP_RENDER_ON STEREO_MULTIVIEW_ON", "STEREO_CUBEMAP_RENDER_ON STEREO_INSTANCING_ON"} },
            { kPlatformAndroid, kShaderCompPlatformGLES3Plus, kStereoRenderingSinglePass, !enableStereo360Capture, { "", "STEREO_MULTIVIEW_ON", "STEREO_INSTANCING_ON" } },
            { kPlatformAndroid, kShaderCompPlatformVulkan, kStereoRenderingSinglePass, enableStereo360Capture, { "", "UNITY_SINGLE_PASS_STEREO", "STEREO_CUBEMAP_RENDER_ON", "STEREO_CUBEMAP_RENDER_ON UNITY_SINGLE_PASS_STEREO" } },
            { kPlatformAndroid, kShaderCompPlatformVulkan, kStereoRenderingSinglePass, !enableStereo360Capture, { "", "UNITY_SINGLE_PASS_STEREO" } },
        };

        for (int i = 0; i < ARRAY_SIZE(testData); i++)
            testCase.WithName(BuildTestNameFromTestParams(testData[i])).WithValues(testData[i]);
    }

    void TestCases_WebGL_BuildGroupPlatform(Testing::TestCaseEmitter<PlatformsOptionsAndKeywordsData>& testCase)
    {
        bool enableStereo360Capture = true;

        PlatformsOptionsAndKeywordsData testData[] =
        {
            { kPlatformWebGL, kShaderCompPlatformGLES20, kStereoRenderingInstancing, enableStereo360Capture, { "", "STEREO_CUBEMAP_RENDER_ON" } },
            { kPlatformWebGL, kShaderCompPlatformGLES20, kStereoRenderingInstancing, !enableStereo360Capture, { "" } },
            { kPlatformWebGL, kShaderCompPlatformGLES3Plus, kStereoRenderingInstancing, enableStereo360Capture, { "", "STEREO_CUBEMAP_RENDER_ON" } },
            { kPlatformWebGL, kShaderCompPlatformGLES3Plus, kStereoRenderingInstancing, !enableStereo360Capture, { "", } },
            { kPlatformWebGL, kShaderCompPlatformGLES20, kStereoRenderingSinglePass, enableStereo360Capture, { "", "STEREO_CUBEMAP_RENDER_ON" } },
            { kPlatformWebGL, kShaderCompPlatformGLES20, kStereoRenderingSinglePass, !enableStereo360Capture, { "" } },
            { kPlatformWebGL, kShaderCompPlatformGLES3Plus, kStereoRenderingSinglePass, enableStereo360Capture, { "", "STEREO_CUBEMAP_RENDER_ON" } },
            { kPlatformWebGL, kShaderCompPlatformGLES3Plus, kStereoRenderingSinglePass, !enableStereo360Capture, { "", } },
        };

        for (int i = 0; i < ARRAY_SIZE(testData); i++)
            testCase.WithName(BuildTestNameFromTestParams(testData[i])).WithValues(testData[i]);
    }

    void TestCases_PS4_BuildGroupPlatform(Testing::TestCaseEmitter<PlatformsOptionsAndKeywordsData>& testCase)
    {
        bool enableStereo360Capture = true;

        PlatformsOptionsAndKeywordsData testData[] =
        {
            { kPlatformPS4, kShaderCompPlatformPS4, kStereoRenderingInstancing, enableStereo360Capture, { "", "STEREO_CUBEMAP_RENDER_ON", "STEREO_INSTANCING_ON", "STEREO_CUBEMAP_RENDER_ON STEREO_INSTANCING_ON" } },
            { kPlatformPS4, kShaderCompPlatformPS4, kStereoRenderingInstancing, !enableStereo360Capture, { "", "STEREO_INSTANCING_ON" } },
            { kPlatformPS4, kShaderCompPlatformPS4, kStereoRenderingSinglePass, enableStereo360Capture, { "", "STEREO_CUBEMAP_RENDER_ON", "UNITY_SINGLE_PASS_STEREO", "STEREO_CUBEMAP_RENDER_ON UNITY_SINGLE_PASS_STEREO" } },
            { kPlatformPS4, kShaderCompPlatformPS4, kStereoRenderingSinglePass, !enableStereo360Capture, { "", "UNITY_SINGLE_PASS_STEREO" } },
        };

        for (int i = 0; i < ARRAY_SIZE(testData); i++)
            testCase.WithName(BuildTestNameFromTestParams(testData[i])).WithValues(testData[i]);
    }

    void TestCases_XBoxOne_BuildGroupPlatform(Testing::TestCaseEmitter<PlatformsOptionsAndKeywordsData>& testCase)
    {
        bool enableStereo360Capture = true;

        PlatformsOptionsAndKeywordsData testData[] =
        {
            { kPlatformXboxOne, kShaderCompPlatformXboxOne, kStereoRenderingInstancing, enableStereo360Capture, { "", "STEREO_CUBEMAP_RENDER_ON" } },
            { kPlatformXboxOne, kShaderCompPlatformXboxOne, kStereoRenderingInstancing, !enableStereo360Capture, { "" } },
            { kPlatformXboxOne, kShaderCompPlatformXboxOneD3D12, kStereoRenderingInstancing, enableStereo360Capture, { "", "STEREO_CUBEMAP_RENDER_ON" } },
            { kPlatformXboxOne, kShaderCompPlatformXboxOneD3D12, kStereoRenderingInstancing, !enableStereo360Capture, { "" } },
            { kPlatformXboxOne, kShaderCompPlatformXboxOne, kStereoRenderingSinglePass, enableStereo360Capture, { "", "STEREO_CUBEMAP_RENDER_ON" } },
            { kPlatformXboxOne, kShaderCompPlatformXboxOne, kStereoRenderingSinglePass, !enableStereo360Capture, { "" } },
            { kPlatformXboxOne, kShaderCompPlatformXboxOneD3D12, kStereoRenderingSinglePass, enableStereo360Capture, { "", "STEREO_CUBEMAP_RENDER_ON" } },
            { kPlatformXboxOne, kShaderCompPlatformXboxOneD3D12, kStereoRenderingSinglePass, !enableStereo360Capture, { "" } },
        };

        for (int i = 0; i < ARRAY_SIZE(testData); i++)
            testCase.WithName(BuildTestNameFromTestParams(testData[i])).WithValues(testData[i]);
    }

    void TestCases_TVOS_BuildGroupPlatform(Testing::TestCaseEmitter<PlatformsOptionsAndKeywordsData>& testCase)
    {
        bool enableStereo360Capture = true;

        PlatformsOptionsAndKeywordsData testData[] =
        {
            { kPlatformtvOS, kShaderCompPlatformMetal, kStereoRenderingInstancing, enableStereo360Capture, { "", "STEREO_CUBEMAP_RENDER_ON" } },
            { kPlatformtvOS, kShaderCompPlatformMetal, kStereoRenderingInstancing, !enableStereo360Capture, { "" } },
            { kPlatformtvOS, kShaderCompPlatformMetal, kStereoRenderingSinglePass, enableStereo360Capture, { "", "STEREO_CUBEMAP_RENDER_ON" } },
            { kPlatformtvOS, kShaderCompPlatformMetal, kStereoRenderingSinglePass, !enableStereo360Capture, { "" } },
        };

        for (int i = 0; i < ARRAY_SIZE(testData); i++)
            testCase.WithName(BuildTestNameFromTestParams(testData[i])).WithValues(testData[i]);
    }

    void TestCases_Facebook_BuildGroupPlatform(Testing::TestCaseEmitter<PlatformsOptionsAndKeywordsData>& testCase)
    {
        bool enableStereo360Capture = true;

        PlatformsOptionsAndKeywordsData testData[] =
        {
            { kPlatformFacebook, kShaderCompPlatformGLES20, kStereoRenderingInstancing, enableStereo360Capture, { "", "STEREO_CUBEMAP_RENDER_ON" } },
            { kPlatformFacebook, kShaderCompPlatformGLES20, kStereoRenderingInstancing, !enableStereo360Capture, { "" } },
            { kPlatformFacebook, kShaderCompPlatformGLES3Plus, kStereoRenderingInstancing, enableStereo360Capture, { "", "STEREO_CUBEMAP_RENDER_ON" } },
            { kPlatformFacebook, kShaderCompPlatformGLES3Plus, kStereoRenderingInstancing, !enableStereo360Capture, { "", } },
            { kPlatformFacebook, kShaderCompPlatformGLES20, kStereoRenderingSinglePass, enableStereo360Capture, { "", "STEREO_CUBEMAP_RENDER_ON" } },
            { kPlatformFacebook, kShaderCompPlatformGLES20, kStereoRenderingSinglePass, !enableStereo360Capture, { "" } },
            { kPlatformFacebook, kShaderCompPlatformGLES3Plus, kStereoRenderingSinglePass, enableStereo360Capture, { "", "STEREO_CUBEMAP_RENDER_ON" } },
            { kPlatformFacebook, kShaderCompPlatformGLES3Plus, kStereoRenderingSinglePass, !enableStereo360Capture, { "", } },
        };

        for (int i = 0; i < ARRAY_SIZE(testData); i++)
            testCase.WithName(BuildTestNameFromTestParams(testData[i])).WithValues(testData[i]);
    }

    void TestCases_Switch_BuildGroupPlatform(Testing::TestCaseEmitter<PlatformsOptionsAndKeywordsData>& testCase)
    {
        bool enableStereo360Capture = true;

        PlatformsOptionsAndKeywordsData testData[] =
        {
            { kPlatformSwitch, kShaderCompPlatformSwitch, kStereoRenderingInstancing, enableStereo360Capture, { "", "STEREO_CUBEMAP_RENDER_ON" } },
            { kPlatformSwitch, kShaderCompPlatformSwitch, kStereoRenderingInstancing, !enableStereo360Capture, { "" } },
            { kPlatformSwitch, kShaderCompPlatformSwitch, kStereoRenderingSinglePass, enableStereo360Capture, { "", "STEREO_CUBEMAP_RENDER_ON" } },
            { kPlatformSwitch, kShaderCompPlatformSwitch, kStereoRenderingSinglePass, !enableStereo360Capture, { "" } },
        };

        for (int i = 0; i < ARRAY_SIZE(testData); i++)
            testCase.WithName(BuildTestNameFromTestParams(testData[i])).WithValues(testData[i]);
    }

    void TestCases_IOS_BuildGroupPlatform(Testing::TestCaseEmitter<PlatformsOptionsAndKeywordsData>& testCase)
    {
        bool enableStereo360Capture = true;

        PlatformsOptionsAndKeywordsData testData[] =
        {
            { kPlatform_iPhone, kShaderCompPlatformGLES20, kStereoRenderingInstancing, enableStereo360Capture, { "", "STEREO_CUBEMAP_RENDER_ON" } },
            { kPlatform_iPhone, kShaderCompPlatformGLES20, kStereoRenderingInstancing, !enableStereo360Capture, { "" } },
            { kPlatform_iPhone, kShaderCompPlatformGLES3Plus, kStereoRenderingInstancing, enableStereo360Capture, { "", "STEREO_CUBEMAP_RENDER_ON"} },
            { kPlatform_iPhone, kShaderCompPlatformGLES3Plus, kStereoRenderingInstancing, !enableStereo360Capture, { ""} },
            { kPlatform_iPhone, kShaderCompPlatformMetal, kStereoRenderingInstancing, enableStereo360Capture, { "", "STEREO_CUBEMAP_RENDER_ON" } },
            { kPlatform_iPhone, kShaderCompPlatformMetal, kStereoRenderingInstancing, !enableStereo360Capture, { "" } },
            { kPlatform_iPhone, kShaderCompPlatformOpenGLCore, kStereoRenderingInstancing, enableStereo360Capture, {"", "STEREO_CUBEMAP_RENDER_ON" } },
            { kPlatform_iPhone, kShaderCompPlatformOpenGLCore, kStereoRenderingInstancing, !enableStereo360Capture, { ""} },
            { kPlatform_iPhone, kShaderCompPlatformGLES20, kStereoRenderingSinglePass, enableStereo360Capture, { "", "STEREO_CUBEMAP_RENDER_ON" } },
            { kPlatform_iPhone, kShaderCompPlatformGLES20, kStereoRenderingSinglePass, !enableStereo360Capture, { "" } },
            { kPlatform_iPhone, kShaderCompPlatformGLES3Plus, kStereoRenderingSinglePass, enableStereo360Capture, { "", "STEREO_CUBEMAP_RENDER_ON", "UNITY_SINGLE_PASS_STEREO", "STEREO_CUBEMAP_RENDER_ON UNITY_SINGLE_PASS_STEREO" } },
            { kPlatform_iPhone, kShaderCompPlatformGLES3Plus, kStereoRenderingSinglePass, !enableStereo360Capture, { "", "UNITY_SINGLE_PASS_STEREO" } },
            { kPlatform_iPhone, kShaderCompPlatformMetal, kStereoRenderingSinglePass, enableStereo360Capture, { "", "STEREO_CUBEMAP_RENDER_ON", "UNITY_SINGLE_PASS_STEREO", "STEREO_CUBEMAP_RENDER_ON UNITY_SINGLE_PASS_STEREO" } },
            { kPlatform_iPhone, kShaderCompPlatformMetal, kStereoRenderingSinglePass, !enableStereo360Capture, { "", "UNITY_SINGLE_PASS_STEREO" } },
            { kPlatform_iPhone, kShaderCompPlatformOpenGLCore, kStereoRenderingSinglePass, enableStereo360Capture, { "", "STEREO_CUBEMAP_RENDER_ON", "UNITY_SINGLE_PASS_STEREO", "STEREO_CUBEMAP_RENDER_ON UNITY_SINGLE_PASS_STEREO" } },
            { kPlatform_iPhone, kShaderCompPlatformOpenGLCore, kStereoRenderingSinglePass, !enableStereo360Capture, { "", "UNITY_SINGLE_PASS_STEREO" } },
        };

        for (int i = 0; i < ARRAY_SIZE(testData); i++)
            testCase.WithName(BuildTestNameFromTestParams(testData[i])).WithValues(testData[i]);
    }

    void ShaderCompilerPlatformSpecificCases(Testing::TestCaseEmitter<PlatformsOptionsAndKeywordsData>& testCase)
    {
        TestCases_Standalone_BuildGroupPlatform(testCase);
        TestCases_Metro_BuildGroupPlatform(testCase);
        TestCases_Android_BuildGroupPlatform(testCase);
        TestCases_IOS_BuildGroupPlatform(testCase);
        TestCases_WebGL_BuildGroupPlatform(testCase);
        TestCases_PS4_BuildGroupPlatform(testCase);
        TestCases_XBoxOne_BuildGroupPlatform(testCase);
        TestCases_TVOS_BuildGroupPlatform(testCase);
        TestCases_Facebook_BuildGroupPlatform(testCase);
        TestCases_Switch_BuildGroupPlatform(testCase);
    }

    PARAMETRIC_TEST(ShaderCompiler_GetPlatformSpecificVariants_ShouldReturnValidVariants, (PlatformsOptionsAndKeywordsData testData), ShaderCompilerPlatformSpecificCases)
    {
        dynamic_array<ShaderKeywordSet> platformVariants(kMemTempAlloc);
        ShaderCompilerGetPlatformSpecificVariants(testData.platformGroup, testData.shaderCompiler, testData.stereoRenderPath, testData.enableStereo360Capture, platformVariants);

        for (int i = 0; i < platformVariants.size(); i++)
        {
            TEMP_STRING platformVariantString;
            keywords::KeywordSetToString(platformVariants[i], platformVariantString);
            CHECK_CONTAINS(testData.expectedPlatformVariants.begin(), testData.expectedPlatformVariants.end(), platformVariantString);
        }

        //In case any new keyword is added
        CHECK_EQUAL(testData.expectedPlatformVariants.size(), platformVariants.size());
    }

    void ObsoleteShaderCompilerPlatforms(Testing::TestCaseEmitter<ShaderCompilerPlatform>& testcase)
    {
        testcase.WithName(ShaderCompilerGetPlatformName(kShaderCompPlatformGL_Obsolete)).WithValues(kShaderCompPlatformGL_Obsolete);
        testcase.WithName(ShaderCompilerGetPlatformName(kShaderCompPlatformD3D9_Obsolete)).WithValues(kShaderCompPlatformD3D9_Obsolete);
        testcase.WithName(ShaderCompilerGetPlatformName(kShaderCompPlatformXbox360_Obsolete)).WithValues(kShaderCompPlatformXbox360_Obsolete);
        testcase.WithName(ShaderCompilerGetPlatformName(kShaderCompPlatformPS3_Obsolete)).WithValues(kShaderCompPlatformPS3_Obsolete);
        testcase.WithName(ShaderCompilerGetPlatformName(kShaderCompPlatformNaCl_Obsolete)).WithValues(kShaderCompPlatformNaCl_Obsolete);
        testcase.WithName(ShaderCompilerGetPlatformName(kShaderCompPlatformFlash_Obsolete)).WithValues(kShaderCompPlatformFlash_Obsolete);
        testcase.WithName(ShaderCompilerGetPlatformName(kShaderCompPlatformD3D11_9x_Obsolete)).WithValues(kShaderCompPlatformD3D11_9x_Obsolete);
        testcase.WithName(ShaderCompilerGetPlatformName(kShaderCompPlatformPSM_Obsolete)).WithValues(kShaderCompPlatformPSM_Obsolete);
        testcase.WithName(ShaderCompilerGetPlatformName(kShaderCompPlatformWiiU_Obsolete)).WithValues(kShaderCompPlatformWiiU_Obsolete);
    }

    void ValidShaderCompilerPlatforms(Testing::TestCaseEmitter<ShaderCompilerPlatform>& testcase)
    {
        testcase.WithName(ShaderCompilerGetPlatformName(kShaderCompPlatformD3D11)).WithValues(kShaderCompPlatformD3D11);
        testcase.WithName(ShaderCompilerGetPlatformName(kShaderCompPlatformGLES20)).WithValues(kShaderCompPlatformGLES20);
        testcase.WithName(ShaderCompilerGetPlatformName(kShaderCompPlatformGLES3Plus)).WithValues(kShaderCompPlatformGLES3Plus);
        testcase.WithName(ShaderCompilerGetPlatformName(kShaderCompPlatformPS4)).WithValues(kShaderCompPlatformPS4);
        testcase.WithName(ShaderCompilerGetPlatformName(kShaderCompPlatformXboxOne)).WithValues(kShaderCompPlatformXboxOne);
        testcase.WithName(ShaderCompilerGetPlatformName(kShaderCompPlatformMetal)).WithValues(kShaderCompPlatformMetal);
        testcase.WithName(ShaderCompilerGetPlatformName(kShaderCompPlatformOpenGLCore)).WithValues(kShaderCompPlatformOpenGLCore);
        testcase.WithName(ShaderCompilerGetPlatformName(kShaderCompPlatformVulkan)).WithValues(kShaderCompPlatformVulkan);
        testcase.WithName(ShaderCompilerGetPlatformName(kShaderCompPlatformSwitch)).WithValues(kShaderCompPlatformSwitch);
        testcase.WithName(ShaderCompilerGetPlatformName(kShaderCompPlatformXboxOneD3D12)).WithValues(kShaderCompPlatformXboxOneD3D12);
    }

    PARAMETRIC_TEST(ObsoleteShaderCompMask_ShouldNotInclude_ValidShaderCompPlatforms, (ShaderCompilerPlatform platform), ValidShaderCompilerPlatforms)
    {
        CHECK((kObsoleteShaderCompPlatformMask & (1 << platform)) == 0);
    }

    PARAMETRIC_TEST(ValidShaderCompMask_ShouldNotInclude_ObsoleteShaderCompPlatforms, (ShaderCompilerPlatform platform), ObsoleteShaderCompilerPlatforms)
    {
        CHECK((kValidShaderCompPlatformMask & (1 << platform)) == 0);
    }

    PARAMETRIC_TEST(ObsoleteShaderCompMask_ShouldInclude_ObsoleteShaderCompPlatforms, (ShaderCompilerPlatform platform), ObsoleteShaderCompilerPlatforms)
    {
        CHECK((kObsoleteShaderCompPlatformMask & (1 << platform)) != 0);
    }

    PARAMETRIC_TEST(ValidShaderCompMask_ShouldInclude_ValidShaderCompPlatforms, (ShaderCompilerPlatform platform), ValidShaderCompilerPlatforms)
    {
        CHECK((kValidShaderCompPlatformMask & (1 << platform)) != 0);
    }
}

#endif // #if ENABLE_UNIT_TESTS
