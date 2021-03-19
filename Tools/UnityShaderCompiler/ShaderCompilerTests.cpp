#include "UnityPrefix.h"

#if ENABLE_UNIT_TESTS

#include "Runtime/Testing/ParametricTest.h"
#include "ShaderCompiler.h"
#include "APIPlugin.h"
#include "Utilities/ProcessIncludes.h"
#include <string>
#include <vector>
#include "Runtime/GfxDevice/metal/MetalShaderBlobHeader.h"
#include "SurfaceCompiler/SurfaceAnalysis.h"
#include "Tools/UnityShaderCompiler/ComputeShaderCompiler.h"

using core::string;

// *** Hack to get HLSLcc tests pulled in from the static lib
namespace SuiteHLSLccTests
{
    extern UnitTest::ListAdder adderControlFlowGraph_Build_Simple_Works;
    extern UnitTest::ListAdder adderControlFlowGraph_Build_If_Works;
    extern UnitTest::ListAdder adderControlFlowGraph_Build_SwitchCase_Works;
    extern UnitTest::ListAdder adderControlFlowGraph_Build_Loop_Works;

    size_t ReallyAnUnusedFunctionToPullInHLSLccTests()
    {
        return
            sizeof(SuiteHLSLccTests::adderControlFlowGraph_Build_Simple_Works)
            + sizeof(SuiteHLSLccTests::adderControlFlowGraph_Build_If_Works)
            + sizeof(SuiteHLSLccTests::adderControlFlowGraph_Build_SwitchCase_Works)
            + sizeof(SuiteHLSLccTests::adderControlFlowGraph_Build_Loop_Works);
    }
}

// </Hack>

ShaderCompileOutputInterface* InvokeComputeCompilerHLSLcc(const ShaderCompileInputInterface* input);

UNIT_TEST_SUITE(ShaderCompiler)
{
    struct TestReflectionReport : public ShaderCompilerReflectionReport
    {
        struct Constant
        {
            core::string m_Name;
            int m_Binding;
            ShaderParamType m_Type;
            int m_Rows;
            int m_Cols;
            bool m_Matrix;
            int m_ArraySize;
            bool m_Struct;
        };

        struct ConstantBuffer
        {
            core::string m_Name;
            int m_SizeInBytes;
            int m_VarCount;
            std::vector<Constant> m_Members;
        };

        UInt32 m_InputMask;
        bool m_IgnoreUnboundInputs;
        std::vector<ConstantBuffer> m_ConstantBuffers;
        std::vector<std::pair<int, core::string> > m_ConstantBufferBindings;
        std::vector<Constant> m_FreeConstants;

        TestReflectionReport() : m_InputMask(0), m_IgnoreUnboundInputs(false) {}
        virtual void OnInputBinding(ShaderChannel dataChannel, VertexComponent inputSlot)
        {
            if (m_IgnoreUnboundInputs && inputSlot == kVertexCompNone)
                return;

            m_InputMask |= (1 << dataChannel);
        }

        virtual void OnConstantBuffer(const char* name, int size, int varCount)
        {
            ConstantBuffer cb;
            cb.m_Name = name;
            cb.m_SizeInBytes = size;
            cb.m_VarCount = varCount;
            m_ConstantBuffers.push_back(cb);
        }

        virtual void OnConstant(const char* name, int bindIndex, ShaderParamType dataType, ConstantType constantType, int rows, int cols, int arraySize)
        {
            Constant c;
            c.m_Name = name;
            c.m_Binding = bindIndex;
            c.m_Type = dataType;
            c.m_Rows = rows;
            c.m_Cols = cols;
            c.m_Matrix = constantType == kConstantTypeMatrix;
            c.m_ArraySize = arraySize;
            c.m_Struct = constantType == kConstantTypeStruct;
            if (m_ConstantBuffers.empty())
                m_FreeConstants.push_back(c);
            else
                m_ConstantBuffers.rbegin()->m_Members.push_back(c);
        }

        virtual void OnCBBinding(const char* name, int bindIndex)
        {
            m_ConstantBufferBindings.push_back(std::make_pair(bindIndex, core::string(name)));
        }

        virtual void OnTextureBinding(const char* name, int bindIndex, int samplerIndex, bool multisampled, TextureDimension dim) {}
        virtual void OnInlineSampler(InlineSamplerType sampler, int bindIndex) {}
        virtual void OnUAVBinding(const char *name, int bindIndex, int origIndex) {}
        virtual void OnBufferBinding(const char* name, int bindIndex) {}
        virtual void OnStatsInfo(int alu, int tex, int flow, int tempRegister) {}
    };


    struct ShaderCompilerTestFixture
    {
        ShaderCompilerTestFixture()
        {
            inputData.p.entryName[kProgramVertex] = "vert";
            inputData.p.entryName[kProgramFragment] = "frag";
            inputData.programType = kProgramVertex;
            inputData.programMask = (1 << kProgramVertex) | (1 << kProgramFragment);
            inputData.args.clear();
            inputData.includeContext.includePaths.push_back(".");
            inputData.includeContext.includePaths.push_back(g_IncludesPath + "/CGIncludes");
            inputData.includeContext.redirectedFolders.insert("re/dir", "redirected");
            inputData.startLine = 0;
            inputData.reflectionReport = &refl;
            analysisCache = CreateSurfaceAnalysisCache(inputData.includeContext);

            output = NULL;
            metalFragmentOutput = NULL;

            ClearReadTextFilesCache();
        }

        ~ShaderCompilerTestFixture()
        {
            if (output)
            {
                gPluginDispatcher->ReleaseShader(output);
                output = NULL;
            }
            if (metalFragmentOutput)
            {
                gPluginDispatcher->ReleaseShader(metalFragmentOutput);
                metalFragmentOutput = NULL;
            }
            DestroySurfaceAnalysisCache(analysisCache);
        }

        void TestUndefinedIdentifier(ShaderCompilerPlatform platform)
        {
            if (!gPluginDispatcher->HasCompilerForAPI(platform))
                return;
            core::string s =
                "#line 10\n"
                "float4 vert() : POSITION {\n" // 10
                "  error_at_11 = 0;\n" // 11
                "  return 0;\n"
                "}\n"
                "float4 frag() : COLOR0 { return 0; }";
            inputData.source = s;
            inputData.api = platform;
            output = gPluginDispatcher->CompileShader(&inputData);
            AddErrorsFrom(errors, &inputData, output);
            CHECK_EQUAL(1, errors.size());
            CHECK_EQUAL(11, errors[0].line);
            CHECK_EQUAL("", errors[0].file);
        }

        void TestUndefinedIdentifier_AfterInclude(ShaderCompilerPlatform platform)
        {
            if (!gPluginDispatcher->HasCompilerForAPI(platform))
                return;
            core::string s =
                "#line 10\n"
                "#include \"UnityShaderVariables.cginc\"\n" // 10
                "float4 vert() : POSITION {\n" // 11
                "  error_at_12 = 0;\n" // 12
                "  return 0;\n"
                "}\n"
                "float4 frag() : COLOR0 { return 0; }";
            inputData.source = s;
            inputData.api = platform;
            output = gPluginDispatcher->CompileShader(&inputData);
            AddErrorsFrom(errors, &inputData, output);
            CHECK_EQUAL(1, errors.size());
            CHECK_EQUAL(12, errors[0].line);
            CHECK_EQUAL("", errors[0].file);
        }

        void TestUndefinedIdentifier_AfterLineAndInclude(ShaderCompilerPlatform platform)
        {
            if (!gPluginDispatcher->HasCompilerForAPI(platform))
                return;
            // There was a bug where a, #include, followed by #line, followed by unrelated stuff, followed by #include, would produce wrong line numbers
            // after the include (on GLSL platforms).
            core::string s =
                "#line 10\n"
                "#include \"UnityShaderVariables.cginc\"\n" // 10
                "#line 20\n"
                "float a;\n" // 20
                "float b;\n" // 21
                "#include \"UnityCG.cginc\"\n" // 22
                "float4 vert() : POSITION {\n" // 23
                "  error_at_24 = 0;\n" // 24
                "  return 0;\n"
                "}\n"
                "float4 frag() : COLOR0 { return 0; }";
            inputData.source = s;
            inputData.api = platform;
            output = gPluginDispatcher->CompileShader(&inputData);
            AddErrorsFrom(errors, &inputData, output);
            CHECK_EQUAL(1, errors.size());
            CHECK_EQUAL(24, errors[0].line);
            CHECK_EQUAL("", errors[0].file);
        }

        void TestMissingFunction(ShaderCompilerPlatform platform)
        {
            if (!gPluginDispatcher->HasCompilerForAPI(platform))
                return;
            core::string s =
                "#line 5\n"
                "float4 vert() : POSITION {\n" // 5
                "  return dot(3);\n" // line 6, should produce one error, no dot(a) function
                "}\n"
                "float4 frag() : COLOR0 { return 0; }";
            inputData.source = s;
            inputData.api = platform;
            output = gPluginDispatcher->CompileShader(&inputData);
            AddErrorsFrom(errors, &inputData, output);
            CHECK_EQUAL(1, errors.size());
            CHECK_EQUAL(6, errors[0].line);
            CHECK_EQUAL("", errors[0].file);
        }

        void TestFileReporting(ShaderCompilerPlatform platform)
        {
            if (!gPluginDispatcher->HasCompilerForAPI(platform))
                return;
            core::string s =
                "#line 5 \"foo/bar.txt\"\n"
                "float4 vert() : POSITION {\n" // 5
                "  error_at_5 = 0;\n" // 6
                "  return 0;\n"
                "}\n"
                "float4 frag() : COLOR0 { return 0; }";
            inputData.source = s;
            inputData.api = platform;
            output = gPluginDispatcher->CompileShader(&inputData);
            AddErrorsFrom(errors, &inputData, output);
            CHECK_EQUAL(1, errors.size());
            CHECK_EQUAL(6, errors[0].line);
            CHECK_EQUAL("foo/bar.txt", errors[0].file);
        }

        void InitReflectionReport(ShaderCompilerPlatform platform)
        {
            refl.m_InputMask = 0;
            refl.m_IgnoreUnboundInputs = IsD3D11ShaderCompiler(platform);
        }

        void RunCompilationWithCompileArgs(const char* src, ShaderCompilerPlatform platform, ShaderCompileArg *shaderCompileArgs, size_t numArgs)
        {
            if (!gPluginDispatcher->HasCompilerForAPI(platform))
                return;
            InitReflectionReport(platform);
            inputData.source = src;
            inputData.api = platform;
            inputData.args.clear();
            for (int iArg = 0; iArg < numArgs; ++iArg)
                inputData.args.push_back(shaderCompileArgs[iArg]);

            if (output)
                gPluginDispatcher->ReleaseShader(output);
            output = gPluginDispatcher->CompileShader(&inputData);

            AddErrorsFrom(errors, &inputData, output);

            metalFragmentErrors.clear();
            if (platform == kShaderCompPlatformMetal)
            {
                inputData.programType = kProgramFragment;

                if (metalFragmentOutput)
                    gPluginDispatcher->ReleaseShader(metalFragmentOutput);
                metalFragmentOutput = gPluginDispatcher->CompileShader(&inputData);

                AddErrorsFrom(metalFragmentErrors, &inputData, metalFragmentOutput);
                inputData.programType = kProgramVertex;
            }
        }

        void RunCompilation(const char* src, ShaderCompilerPlatform platform)
        {
            RunCompilationWithCompileArgs(src, platform, NULL, 0);
        }

        bool TestCompilationOk(const char* src, ShaderCompilerPlatform platform, bool expectFail = false)
        {
            return TestCompilationOkWithCompileArg(src, platform, NULL, expectFail);
        }

        bool TestCompilationOkWithCompileArg(const char* src, ShaderCompilerPlatform platform, ShaderCompileArg *shaderCompileArg, bool expectFail = false)
        {
            return TestCompilationOkWithCompileArgs(src, platform, shaderCompileArg, (shaderCompileArg != NULL) ? 1 : 0, expectFail);
        }

        bool TestCompilationOkWithCompileArgs(const char* src, ShaderCompilerPlatform platform, ShaderCompileArg *shaderCompileArgs, size_t numArgs, bool expectFail = false)
        {
            if (!gPluginDispatcher->HasCompilerForAPI(platform))
                return false;

            RunCompilationWithCompileArgs(src, platform, shaderCompileArgs, numArgs);

            bool hadErrors = !errors.empty() || !metalFragmentErrors.empty();
            CHECK_EQUAL(expectFail, hadErrors);
            return !hadErrors;
        }

        bool TestCompilationFailure(const char* src, ShaderCompilerPlatform platform, core::string expectError)
        {
            if (!gPluginDispatcher->HasCompilerForAPI(platform))
                return false;

            RunCompilation(src, platform);

            bool hadErrors = !errors.empty() || !metalFragmentErrors.empty();

            CHECK(hadErrors);
            if (hadErrors)
            {
                CHECK(strstr(errors[0].message.c_str(), expectError.c_str()) != NULL);
            }
            return hadErrors;
        }

        void TestAnalyzeViaCompile(const char* source, ShaderImportErrors& errors)
        {
            ShaderDesc out;
            ShaderArgs args;
            StringArray keywords;
            ShaderCompileRequirements requirements;
            const ShaderCompilerPlatform api = SurfaceData::AnalysisShaderCompilerPlatform;
            SetSurfaceShaderArgs(api, requirements, keywords, args);
            AnalyzeSurfaceShaderViaCompile(source, "frag", 0, inputData.includeContext.includePaths, api, args, out, errors, *analysisCache);
        }

        void TestAnalyzeViaCompileOk(const char* source)
        {
            ShaderImportErrors errors;
            TestAnalyzeViaCompile(source, errors);
            CHECK(!errors.HasErrorsOrWarnings());
        }

        bool TestAnalyzeViaCompileWithFailure(const char* source, core::string expectError)
        {
            ShaderImportErrors errors;
            TestAnalyzeViaCompile(source, errors);

            bool hadErrors = errors.HasErrors();
            CHECK(hadErrors);
            if (hadErrors)
            {
                ShaderImportErrors::ErrorContainer errorMessages = errors.GetErrors();
                core::string errMsg = errorMessages.begin()->message;
                CHECK(strstr(errorMessages.begin()->message.c_str(), expectError.c_str()) != NULL);
            }
            return hadErrors;
        }

        static size_t GetNumErrorsOnly(const ShaderCompileOutputInterface* compileOutput)
        {
            const size_t errorsAndWarningsCount = compileOutput->GetNumErrorsAndWarnings();
            size_t realErrorsCount = 0;
            const char* message;
            const char* file;
            int line;
            bool warning;
            ShaderCompilerPlatform api;
            for (size_t i = 0; i < errorsAndWarningsCount; i++)
            {
                warning = false;
                compileOutput->GetErrorOrWarning(i, message, file, line, warning, api);
                if (!warning)
                    ++realErrorsCount;
            }
            return realErrorsCount;
        }

        bool TestComputeCompilationOk(const char* src, ShaderCompilerPlatform platform, bool expectFail = false, bool treatWarningsAsErrors = true)
        {
            if (!gPluginDispatcher->HasCompilerForAPI(platform))
                return false;

            // Parsing the kernel pragmas happens atm at the importer (editor) side. So do the parsing here
            // to get the required info from the test shaders.
            core::string source(src);
            ComputeImportDirectives importDirectives;
            UInt32 supportedAPIs = 0;
            ParseComputeImportDirectives(source, importDirectives, supportedAPIs);
            bool hadErrors = false;

            for (int i = 0; i < importDirectives.kernels.size(); i++)
            {
                ComputeImportDirectives::Kernel kernel = importDirectives.kernels[i];

                InitReflectionReport(platform);
                inputData.source = source;
                inputData.sourceFilename = "compute shader";
                inputData.api = platform;
                inputData.compileFlags |= importDirectives.compileFlags;
                inputData.p.entryName[0] = kernel.name;
                inputData.args.clear();
                for (int m = 0; m < kernel.macros.size(); m++)
                    inputData.args.push_back(ShaderCompileArg(kernel.macros[m].first, kernel.macros[m].second));

                output = gPluginDispatcher->CompileComputeShader(&inputData);
                AddErrorsFrom(errors, &inputData, output);
                if (treatWarningsAsErrors)
                    hadErrors |= output->GetNumErrorsAndWarnings() != 0;
                else
                    hadErrors |= GetNumErrorsOnly(output) != 0;
                //cleanup done by test fixture
            }
            CHECK_EQUAL(expectFail, hadErrors);
            return !hadErrors;
        }

        bool TestComputeCompilationFailure(const char* src, ShaderCompilerPlatform platform, core::string expectError)
        {
            if (!gPluginDispatcher->HasCompilerForAPI(platform))
                return false;

            // Parsing the kernel pragmas happens atm at the importer (editor) side. So do the parsing here
            // to get the required info from the test shaders.
            core::string source(src);
            ComputeImportDirectives importDirectives;
            UInt32 supportedAPIs = 0;
            ParseComputeImportDirectives(source, importDirectives, supportedAPIs);
            if ((supportedAPIs & (1 << platform)) == 0)
                return false;

            bool hadErrors = false;

            for (int i = 0; i < importDirectives.kernels.size(); i++)
            {
                ComputeImportDirectives::Kernel kernel = importDirectives.kernels[i];

                InitReflectionReport(platform);
                inputData.source = source;
                inputData.sourceFilename = "compute shader";
                inputData.api = platform;
                inputData.compileFlags |= importDirectives.compileFlags;
                inputData.p.entryName[0] = kernel.name;
                inputData.args.clear();
                for (int m = 0; m < kernel.macros.size(); m++)
                    inputData.args.push_back(ShaderCompileArg(kernel.macros[m].first, kernel.macros[m].second));

                output = gPluginDispatcher->CompileComputeShader(&inputData);
                AddErrorsFrom(errors, &inputData, output);
                bool hadErrorKernel = output->GetNumErrorsAndWarnings() != 0;
                hadErrors |= hadErrorKernel;
                CHECK(hadErrorKernel);
                if (hadErrorKernel)
                {
                    CHECK(strstr(errors[0].message.c_str(), expectError.c_str()) != NULL);
                }
                //cleanup done by test fixture
            }
            CHECK(hadErrors);
            return hadErrors;
        }

        void TestComputeTranslationPass(const char* src, ShaderCompilerPlatform platform, bool expectFail = false)
        {
            if (!gPluginDispatcher->HasCompilerForAPI(platform))
                return;

            inputData.api = platform;

            // Only CompilerHLSLcc implements the TranslateComputeKernel API
            // Other platforms should just return NULL
            if (platform != kShaderCompPlatformOpenGLCore &&
                platform != kShaderCompPlatformGLES3Plus &&
                platform != kShaderCompPlatformMetal)
            {
                CHECK_NULL(gPluginDispatcher->TranslateComputeKernel(&inputData));
                return;
            }

            core::string source(src);
            ComputeImportDirectives importDirectives;
            UInt32 supportedAPIs = 0;
            ParseComputeImportDirectives(source, importDirectives, supportedAPIs);

            for (int i = 0; i < importDirectives.kernels.size(); i++)
            {
                ComputeImportDirectives::Kernel kernel = importDirectives.kernels[i];

                refl.m_InputMask = 0;
                inputData.source = source.c_str();
                inputData.sourceFilename = "compute shader";
                inputData.compileFlags |= importDirectives.compileFlags;
                inputData.p.entryName[0] = kernel.name;
                inputData.args.clear();
                for (int m = 0; m < kernel.macros.size(); m++)
                    inputData.args.push_back(ShaderCompileArg(kernel.macros[m].first, kernel.macros[m].second));


                // Then compile the same kernel with two-pass procedure:
                // Compile
                inputData.compileFlags |= kShaderCompFlags_UseSeparateTranslatePass;
                const ShaderCompileOutputInterface* firstPass = gPluginDispatcher->CompileComputeShader(&inputData);
                CHECK_NOT_NULL(firstPass);
                CHECK_EQUAL(0, firstPass->GetNumErrorsAndWarnings()); // This test should never get errors in the compile phase

                // And translate
                inputData.gpuProgramData.assign(firstPass->GetCompiledDataPtr(), firstPass->GetCompiledDataPtr() + firstPass->GetCompiledDataSize());
                output = gPluginDispatcher->TranslateComputeKernel(&inputData);
                CHECK_NOT_NULL(output);
                if (!expectFail)
                {
                    CHECK_EQUAL(0, output->GetNumErrorsAndWarnings());

                    // Compile a reference result with single pass method
                    inputData.gpuProgramData.clear_dealloc();
                    inputData.compileFlags &= ~kShaderCompFlags_UseSeparateTranslatePass;
                    const ShaderCompileOutputInterface* reference = gPluginDispatcher->CompileComputeShader(&inputData);
                    CHECK_NOT_NULL(reference);
                    CHECK_EQUAL(0, reference->GetNumErrorsAndWarnings());

                    // Check that the two pass method did produce identical results
                    size_t outputLen = output->GetCompiledDataSize();
                    CHECK_EQUAL(reference->GetCompiledDataSize(), outputLen);
                    CHECK_ARRAY_EQUAL(reference->GetCompiledDataPtr(), output->GetCompiledDataPtr(), outputLen);

                    if (reference)
                        gPluginDispatcher->ReleaseShader(reference);
                }
                else
                    CHECK_NOT_EQUAL(0, output->GetNumErrorsAndWarnings());

                if (firstPass)
                    gPluginDispatcher->ReleaseShader(firstPass);
                if (output)
                    gPluginDispatcher->ReleaseShader(output);
                output = NULL;
            }
        }

        void TestComputeTranslationPassAllPlatforms(const char* src, bool expectFail = false)
        {
            TestComputeTranslationPass(src, kShaderCompPlatformGLES3Plus, expectFail);
            TestComputeTranslationPass(src, kShaderCompPlatformD3D11, expectFail);
#       if ENABLE_XBOXONE_COMPILER
            TestComputeTranslationPass(src, kShaderCompPlatformXboxOne, expectFail);
#       endif
#       if ENABLE_PSSL_COMPILER
            TestComputeTranslationPass(src, kShaderCompPlatformPS4, expectFail);
#       endif
            TestComputeTranslationPass(src, kShaderCompPlatformMetal, expectFail);
            TestComputeTranslationPass(src, kShaderCompPlatformVulkan, expectFail);
#       if ENABLE_SWITCH_COMPILER
            TestComputeTranslationPass(src, kShaderCompPlatformSwitch, expectFail);
#       endif
        }

        void TestMissingKernel(ShaderCompilerPlatform platform)
        {
            const char *s =
                "#pragma kernel DoesNotExist\n"
                "[numthreads(1, 1, 1)]\n"
                "void CSMain(uint3 id : SV_DispatchThreadID)\n"
                "{\n"
                "}\n";
            TestComputeCompilationOk(s, platform, true);
            CHECK_EQUAL(1, errors.size());
            CHECK_EQUAL(false, errors[0].warning);
            CHECK_EQUAL("Did not find shader kernel 'DoesNotExist' to compile", errors[0].message);
        }

        void TestPreprocessorErrorParsedCorrectly(ShaderCompilerPlatform platform)
        {
            if (!gPluginDispatcher->HasCompilerForAPI(platform))
                return;

            const char* const s =
                "#error PREPROCESSOR_ERROR\n"
                "float4 vert() : POSITION { return 0;}\n"
                "float4 frag() : COLOR0 { return 0; }";

            inputData.source = s;
            inputData.api = platform;
            output = gPluginDispatcher->CompileShader(&inputData);
            AddErrorsFrom(errors, &inputData, output);
            CHECK_EQUAL(1, errors.size());
            CHECK_EQUAL(false, errors[0].warning);
            CHECK_EQUAL("PREPROCESSOR_ERROR", errors[0].message);
        }

        void TestComputeShaderPreprocessorErrorParsedCorrectly(ShaderCompilerPlatform platform)
        {
            if (!gPluginDispatcher->HasCompilerForAPI(platform))
                return;

            const char *s =
                "#error PREPROCESSOR_ERROR\n"
                "#pragma kernel CSMain\n"

                "[numthreads(1, 1, 1)]\n"
                "void CSMain(uint3 id : SV_DispatchThreadID)\n"
                "{\n"
                "}\n";

            InitReflectionReport(platform);
            inputData.source = s;
            inputData.sourceFilename = "compute shader";
            inputData.api = platform;
            output = gPluginDispatcher->CompileComputeShader(&inputData);
            AddErrorsFrom(errors, &inputData, output);
            CHECK_EQUAL(1, errors.size());
            CHECK_EQUAL(false, errors[0].warning);
            CHECK_EQUAL("PREPROCESSOR_ERROR", errors[0].message);
        }

        void RunTest(const char* s, ShaderCompilerPlatform platform, bool expectFail = false)
        {
            if (platform == kShaderCompPlatformNone)
                TestAnalyzeViaCompileOk(s);
            else
                TestCompilationOk(s, platform, expectFail);
        }

        void RunIncludeFailureTest(const char* s, ShaderCompilerPlatform platform, const char* filename)
        {
            core::string err = Format("failed to open source file: '%s'", filename);
            TestCompilationFailure(s, platform, err);
        }

        void AssignOutputString(core::string& out)
        {
            if (output)
                out.assign((const char*)output->GetCompiledDataPtr(), (const char*)output->GetCompiledDataPtr() + output->GetCompiledDataSize());
        }

        core::string GetOutputString(bool isComputeShader = false)
        {
            if (isComputeShader && output)
            {
                ComputeShaderBinary kernelData;
                dynamic_array<UInt8> tempArray = dynamic_array<UInt8>(output->GetCompiledDataSize(), kMemTempAlloc);
                tempArray.assign((UInt8*)output->GetCompiledDataPtr(), (UInt8*)output->GetCompiledDataPtr() + output->GetCompiledDataSize());
                kernelData.ReadFromBuffer(tempArray, kMemTempAlloc);
                return !kernelData.GetKernels().empty() ? core::string((char*)kernelData.GetKernels()[0].code.data()) : core::string();
            }

            return output ? core::string((const char*)output->GetCompiledDataPtr(), output->GetCompiledDataSize()) : core::string();
        }

        core::string GetMetalShaderOutputStringImpl(const ShaderCompileOutputInterface* out)
        {
            if (!out)
                return core::string();

            MetalShaderBlobDesc desc = ExtractMetalShaderBlobDesc(out->GetCompiledDataPtr(), out->GetCompiledDataSize());
            return core::string(desc.code, desc.codeLength);
        }

        core::string GetMetalVertexShaderOutputString()
        {
            return GetMetalShaderOutputStringImpl(output);
        }

        core::string GetMetalFragmentShaderOutputString()
        {
            return GetMetalShaderOutputStringImpl(metalFragmentOutput);
        }

        bool WasUsingCompiler(ShaderCompilationFlags flag)
        {
            return (output && output->compileFlags & flag);
        }

        bool FindConstant(const char* name, ShaderParamType expectedType, bool expectedMatrix) const
        {
            for (size_t i = 0; i < refl.m_FreeConstants.size(); ++i)
            {
                if (refl.m_FreeConstants[i].m_Name == name && refl.m_FreeConstants[i].m_Type == expectedType && refl.m_FreeConstants[i].m_Matrix == expectedMatrix)
                    return true;
            }
            for (size_t i = 0; i < refl.m_ConstantBuffers.size(); ++i)
            {
                const std::vector<TestReflectionReport::Constant>& consts = refl.m_ConstantBuffers[i].m_Members;
                for (size_t j = 0; j < consts.size(); ++j)
                {
                    if (consts[j].m_Name == name && consts[j].m_Type == expectedType && consts[j].m_Matrix == expectedMatrix)
                        return true;
                }
            }
            return false;
        }

        static size_t GetStringOccurences(const core::string& input, const core::string& whatToFind)
        {
            size_t result = 0;
            core::string::size_type pos = input.find(whatToFind);
            while (pos != core::string::npos)
            {
                ++result;
                pos = input.find(whatToFind, pos + 1);
            }
            return result;
        }

        static void TestMinMaxInsideAndOutsideBounds(int maj, int min)
        {
            int combinedVersionNotClamped = 2018 * 100 + maj * 10 + min;
            int combinedVersionClamped = CreateNumericUnityVersion(2018, maj, min);

            CHECK(combinedVersionClamped <= combinedVersionNotClamped);
        }

        ShaderCompileInputData inputData;
        const ShaderCompileOutputInterface* output;
        const ShaderCompileOutputInterface* metalFragmentOutput;
        TestReflectionReport refl;
        std::vector<ShaderImportError> errors;
        std::vector<ShaderImportError> metalFragmentErrors;
        SurfaceAnalysisCache* analysisCache;
    };


    const bool kDoNotExpectFail = false;
    const bool kExpectFail = true;
    const bool kDoNotTreatWarningsAsErrors = false;
    const bool kTreatWarningsAsErrors = true;
    // replace above constants by c++11 strongly typed enums when possible:
    /*
    enum ExpectFail : bool
    {
        kDoNotExpectFail = false,
        kExpectFail = true,
    };
    enum TreatWarningsAsErrors : bool
    {
        kDoNotTreatWarningsAsErrors = false,
        kTreatWarningsAsErrors = false,
    };
    */

    enum PlatformFlags
    {
        kAnalyzer = 1 << 0,
        kCompiler = 1 << 1,
        kCommon = 1 << 2,
        kHasCompute = 1 << 3,
        kCheckSupport = 1 << 4
    };
    ENUM_FLAGS(PlatformFlags);

    struct PlatformDesc
    {
        const char* name;
        ShaderCompilerPlatform platform;
        PlatformFlags flags;
    };

    PlatformDesc kPlatforms[] =
    {
        { "Analyzer",    kShaderCompPlatformNone,       kAnalyzer },
        { "D3D11",       kShaderCompPlatformD3D11,      kCompiler | kCommon | kHasCompute },
        { "GLES20",      kShaderCompPlatformGLES20,     kCompiler | kCommon },
        { "GLES3Plus",   kShaderCompPlatformGLES3Plus,  kCompiler | kCommon | kHasCompute },
        { "OpenGLCore",  kShaderCompPlatformOpenGLCore, kCompiler | kCommon },
        { "Metal",       kShaderCompPlatformMetal,      kCompiler | kCommon | kHasCompute },
        { "Vulkan",      kShaderCompPlatformVulkan,     kCompiler | kCommon | kHasCompute },
#if ENABLE_XBOXONE_COMPILER
        { "XboxOne",     kShaderCompPlatformXboxOne,    kCompiler | kHasCompute | kCheckSupport },
#endif
#if ENABLE_PSSL_COMPILER
        { "PS4",         kShaderCompPlatformPS4,        kCompiler | kHasCompute | kCheckSupport },
#endif
#if ENABLE_SWITCH_COMPILER
        { "Switch",      kShaderCompPlatformSwitch,     kCompiler | kHasCompute | kCheckSupport },
#endif
    };

    void EmitAllPlatformsWithFlags(Testing::TestCaseEmitter<ShaderCompilerPlatform>& platform, PlatformFlags flags)
    {
        for (size_t i = 0; i < ARRAY_SIZE(kPlatforms); ++i)
        {
            const PlatformDesc& desc = kPlatforms[i];
            if (HasAnyFlags(desc.flags, flags))
            {
                if (HasFlag(desc.flags, kCheckSupport) && !gPluginDispatcher->HasCompilerForAPI(desc.platform))
                    continue;

                platform.WithName(desc.name).WithValues(desc.platform);
            }
        }
    }

    void CommonPlatforms(Testing::TestCaseEmitter<ShaderCompilerPlatform>& platform)
    {
        EmitAllPlatformsWithFlags(platform, kCommon);
    }

    void ComputePlatforms(Testing::TestCaseEmitter<ShaderCompilerPlatform>& platform)
    {
        EmitAllPlatformsWithFlags(platform, kHasCompute);
    }

    void AllPlatforms(Testing::TestCaseEmitter<ShaderCompilerPlatform>& platform)
    {
        EmitAllPlatformsWithFlags(platform, kCompiler);
    }

    void AnalyzerAndCommonPlatforms(Testing::TestCaseEmitter<ShaderCompilerPlatform>& platform)
    {
        EmitAllPlatformsWithFlags(platform, kAnalyzer | kCommon);
    }

    void AnalyzerAndAllPlatforms(Testing::TestCaseEmitter<ShaderCompilerPlatform>& platform)
    {
        EmitAllPlatformsWithFlags(platform, kAnalyzer | kCompiler);
    }

    void EmitAllPlatformsFromMask(Testing::TestCaseEmitter<ShaderCompilerPlatform>& platform, UInt32 mask)
    {
        for (size_t i = 0; i < ARRAY_SIZE(kPlatforms); ++i)
        {
            const PlatformDesc& desc = kPlatforms[i];
            if ((1 << desc.platform) & mask)
                platform.WithName(desc.name).WithValues(desc.platform);
        }
    }

    #define PLATFORM_MASK(name)                         (1 << kShaderCompPlatform##name)
    #define PLATFORM_MASK2(name1, name2)                (PLATFORM_MASK(name1) | PLATFORM_MASK(name2))
    #define PLATFORM_MASK3(name1, name2, name3)         (PLATFORM_MASK2(name1, name2) | PLATFORM_MASK(name3))
    #define PLATFORM_MASK4(name1, name2, name3, name4)  (PLATFORM_MASK3(name1, name2, name3) | PLATFORM_MASK(name4))
    #define PLATFORM_MASK5(name1, name2, name3, name4, name5)   (PLATFORM_MASK4(name1, name2, name3, name4) | PLATFORM_MASK(name5))
    #define PLATFORM_MASK6(name1, name2, name3, name4, name5, name6)  (PLATFORM_MASK5(name1, name2, name3, name4, name5) | PLATFORM_MASK(name6))
    #define PLATFORM_ONLY(name)                                 void name##_Only(Testing::TestCaseEmitter<ShaderCompilerPlatform>& platform) { EmitAllPlatformsFromMask(platform, PLATFORM_MASK(name)); }
    #define PLATFORM_GROUP2(name1, name2)               void name1##_##name2(Testing::TestCaseEmitter<ShaderCompilerPlatform>& platform) { EmitAllPlatformsFromMask(platform, PLATFORM_MASK2(name1, name2)); }
    #define PLATFORM_GROUP3(name1, name2, name3)        void name1##_##name2##_##name3(Testing::TestCaseEmitter<ShaderCompilerPlatform>& platform) { EmitAllPlatformsFromMask(platform, PLATFORM_MASK3(name1, name2, name3)); }
    #define PLATFORM_GROUP4(name1, name2, name3, name4) void name1##_##name2##_##name3##_##name4(Testing::TestCaseEmitter<ShaderCompilerPlatform>& platform) { EmitAllPlatformsFromMask(platform, PLATFORM_MASK4(name1, name2, name3, name4)); }
    #define PLATFORM_GROUP5(name1, name2, name3, name4, name5)  void name1##_##name2##_##name3##_##name4##_##name5(Testing::TestCaseEmitter<ShaderCompilerPlatform>& platform) { EmitAllPlatformsFromMask(platform, PLATFORM_MASK5(name1, name2, name3, name4, name5)); }
    #define PLATFORM_GROUP6(name1, name2, name3, name4, name5, name6)  void name1##_##name2##_##name3##_##name4##_##name5##_##name6(Testing::TestCaseEmitter<ShaderCompilerPlatform>& platform) { EmitAllPlatformsFromMask(platform, PLATFORM_MASK6(name1, name2, name3, name4, name5, name6)); }

    PLATFORM_GROUP3(D3D11, OpenGLCore, Metal);
    PLATFORM_GROUP4(D3D11, OpenGLCore, Metal, GLES20);
    PLATFORM_GROUP2(GLES20, GLES3Plus);
    PLATFORM_GROUP3(GLES20, GLES3Plus, OpenGLCore);
    PLATFORM_GROUP3(GLES20, GLES3Plus, Vulkan);
    PLATFORM_GROUP2(GLES3Plus, Metal);
    PLATFORM_GROUP3(GLES3Plus, Metal, Vulkan);
    PLATFORM_GROUP4(GLES3Plus, Metal, Vulkan, Switch);
    PLATFORM_GROUP3(GLES3Plus, Metal, Switch);
    PLATFORM_GROUP2(GLES3Plus, Vulkan);
    PLATFORM_GROUP3(GLES3Plus, Vulkan, Switch);
    PLATFORM_GROUP2(GLES3Plus, Switch);
    PLATFORM_GROUP2(GLES3Plus, OpenGLCore);
    PLATFORM_GROUP3(GLES3Plus, OpenGLCore, Metal);
    PLATFORM_GROUP4(GLES3Plus, OpenGLCore, Metal, Vulkan);
    PLATFORM_GROUP4(GLES3Plus, OpenGLCore, Metal, Switch);
    PLATFORM_GROUP5(GLES3Plus, OpenGLCore, Metal, Vulkan, Switch);
    PLATFORM_GROUP3(GLES3Plus, OpenGLCore, Vulkan);
    PLATFORM_GROUP4(GLES3Plus, OpenGLCore, Vulkan, Switch);
    PLATFORM_GROUP3(GLES3Plus, OpenGLCore, Switch);
    PLATFORM_GROUP2(OpenGLCore, Metal);
    PLATFORM_GROUP3(OpenGLCore, Metal, Vulkan);
    PLATFORM_GROUP3(OpenGLCore, Metal, Switch);
    PLATFORM_GROUP4(OpenGLCore, Metal, Vulkan, Switch);
    PLATFORM_GROUP6(OpenGLCore, Vulkan, D3D11, XboxOne, PS4, Switch);
    PLATFORM_GROUP2(Vulkan, Switch);
    PLATFORM_ONLY(Switch);
    PLATFORM_ONLY(Metal);
    PLATFORM_ONLY(Vulkan);

    PARAMETRIC_TEST_FIXTURE(ShaderCompilerTestFixture, ErrorLineReporting_UndefinedIdentifier, (ShaderCompilerPlatform platform), CommonPlatforms)
    {
        TestUndefinedIdentifier(platform);
    }


    PARAMETRIC_TEST_FIXTURE(ShaderCompilerTestFixture, ErrorLineReporting_UndefinedIdentifier_AfterInclude, (ShaderCompilerPlatform platform), CommonPlatforms)
    {
        TestUndefinedIdentifier_AfterInclude(platform);
    }


    PARAMETRIC_TEST_FIXTURE(ShaderCompilerTestFixture, ErrorLineReporting_UndefinedIdentifier_AfterInclude_ThatIncludesOthers, (ShaderCompilerPlatform platform), CommonPlatforms)
    {
        TestUndefinedIdentifier_AfterLineAndInclude(platform);
    }


    PARAMETRIC_TEST_FIXTURE(ShaderCompilerTestFixture, ErrorLineReporting_MissingFunction, (ShaderCompilerPlatform platform), CommonPlatforms)
    {
        // Some shader compilers report one error on multiple output lines,
        // mostly DX11 one for missing function overloads. Make sure we parse them correctly
        // into a single error.
        TestMissingFunction(platform);
    }


    PARAMETRIC_TEST_FIXTURE(ShaderCompilerTestFixture, ErrorFileReporting, (ShaderCompilerPlatform platform), CommonPlatforms)
    {
        TestFileReporting(platform);
    }

    PARAMETRIC_TEST_FIXTURE(ShaderCompilerTestFixture, PreprocessorErrorInSource_CompilationFails, (ShaderCompilerPlatform platform), AllPlatforms)
    {
        const char* const s =
            "#error PREPROCESSOR_ERROR\n"
            "float4 vert() : POSITION { return 0;}\n"
            "float4 frag() : COLOR0 { return 0; }";

        RunTest(s, platform, true);
    }

    PARAMETRIC_TEST_FIXTURE(ShaderCompilerTestFixture, ComputeShader_PreprocessorErrorInSource_CompilationFails, (ShaderCompilerPlatform platform), ComputePlatforms)
    {
        const char *s =
            "#error PREPROCESSOR_ERROR\n"
            "#pragma kernel CSMain\n"

            "[numthreads(1, 1, 1)]\n"
            "void CSMain(uint3 id : SV_DispatchThreadID)\n"
            "{\n"
            "}\n";

        TestComputeCompilationOk(s, platform, true);
    }

    TEST_FIXTURE(ShaderCompilerTestFixture, ComputeShader_PreprocessorErrorInSource_ErrorMessageIsParsedCorrectly)
    {
        TestComputeShaderPreprocessorErrorParsedCorrectly(kShaderCompPlatformD3D11);
    }

    PARAMETRIC_TEST_FIXTURE(ShaderCompilerTestFixture, GetsInputBindings, (ShaderCompilerPlatform platform), AllPlatforms)
    {
        const char* s =
            "float4 vert(float4 v : POSITION, float3 n : NORMAL, float4 t : TANGENT) : SV_POSITION { return v + t; }\n" // normal unused
            "float4 frag() : COLOR0 { return 0; }";

        const UInt32 expectedBindings = static_cast<UInt32>(kShaderChannelMaskVertex | kShaderChannelMaskTangent);

        if (TestCompilationOk(s, platform))
            CHECK_EQUAL(expectedBindings, refl.m_InputMask);
    }


    PARAMETRIC_TEST_FIXTURE(ShaderCompilerTestFixture, DefaultShaderTarget_Compiled_HasExpectedPlatformShaderVersions, (ShaderCompilerPlatform platform), GLES20_GLES3Plus_OpenGLCore)
    {
        const char* s =
            "float4 vert() : SV_POSITION { return 1; }\n"
            "float4 frag() : SV_Target { return 1; }";
        core::string outputString;

        const char* versionString =
            (platform == kShaderCompPlatformGLES20) ?     "#version 100" :              // GLES2: defaults to version 1.00
            (platform == kShaderCompPlatformGLES3Plus) ?  "#version 300 es" :           // GLES3: defaults to version 3.00
            (platform == kShaderCompPlatformOpenGLCore) ? "#version 150" : "(unknown)"; // GLCore: defaults to version 1.50

        CHECK(TestCompilationOk(s, platform));
        AssignOutputString(outputString);
        CHECK(outputString.find(versionString) != core::string::npos);
    }


    PARAMETRIC_TEST_FIXTURE(ShaderCompilerTestFixture, SM40ShaderTarget_Compiled_HasExpectedPlatformShaderVersions, (ShaderCompilerPlatform platform), GLES20_GLES3Plus_OpenGLCore)
    {
        const char* s =
            "float4 vert() : SV_POSITION { return 1; }\n"
            "float4 frag() : SV_Target { return 1; }";
        inputData.requirements = kShaderRequireShaderModel40;
        core::string outputString;

        const char* versionString =
            (platform == kShaderCompPlatformGLES20) ?     "#version 100" :              // GLES2: defaults to version 1.00
            (platform == kShaderCompPlatformGLES3Plus) ?  "#version 310 es" :           // GLES3: SM4.0 compiles to version 3.10
            (platform == kShaderCompPlatformOpenGLCore) ? "#version 410" : "(unknown)"; // GLCore: SM4.0 compiles to version 4.10

        CHECK(TestCompilationOk(s, platform));
        AssignOutputString(outputString);
        CHECK(outputString.find(versionString) != core::string::npos);
    }


    PARAMETRIC_TEST_FIXTURE(ShaderCompilerTestFixture, GetsInputBindings_ColorSemanticDefinedButNotUsed_ExpectNoColorInInputLayout, (ShaderCompilerPlatform platform), CommonPlatforms)
    {
        const char* s =
            "float4 vert(float4 v : POSITION, float4 color : COLOR) : SV_POSITION { return v; }\n" // color defined, but not referenced
            "float4 frag() : COLOR0 { return 0; }";

        const UInt32 expectedBindings = UInt32(kShaderChannelMaskVertex);

        if (TestCompilationOk(s, platform))
            CHECK_EQUAL(expectedBindings, refl.m_InputMask);
    }

    PARAMETRIC_TEST_FIXTURE(ShaderCompilerTestFixture, ES3_Target50_GetsInputBindings, (ShaderCompilerPlatform platform), GLES20_GLES3Plus_OpenGLCore)
    {
        const char* s =
            "float4 vert(float4 v : POSITION, float3 n : NORMAL, float4 t : TANGENT) : POSITION { return v + t; }\n" // normal unused
            "float4 frag() : COLOR0 { return 0; }";

        const UInt32 expectedBindings = UInt32(kShaderChannelMaskVertex | kShaderChannelMaskTangent);

        inputData.requirements = kShaderRequireShaderModel50;
        if (TestCompilationOk(s, platform))
            CHECK_EQUAL(expectedBindings, refl.m_InputMask);
    }

    PARAMETRIC_TEST_FIXTURE(ShaderCompilerTestFixture, Texture2DLod_CompiledForOpenGLES_ProducesFallbackImplementation, (ShaderCompilerPlatform platform), GLES20_GLES3Plus)
    {
        // many GLES2.0 devices don't have texture2DLod and friends, so in order to avoid
        // console spam we compile a workaround that does a mip-biased texture lookup
        const char* s =
            "sampler2D _MainTex;"
            "float4 vert(float4 v : POSITION, float3 n : NORMAL, float4 t : TANGENT) : POSITION { return v + t; }\n" // normal unused
            "float4 frag() : COLOR0 { return tex2Dlod(_MainTex, float4(0.0, 0.0, 0.0, 0.5)); }";

        core::string outputString;

        TestCompilationOk(s, platform);
        // copy to a string so that we can search.
        AssignOutputString(outputString);
        bool foundTexture2DLodEXT = false;
        if (WasUsingCompiler(kShaderCompFlags_PreferHLSL2GLSL))
        {
            foundTexture2DLodEXT = outputString.find("impl_low_texture2DLodEXT") != outputString.npos;
        }
        else
        {
            foundTexture2DLodEXT = outputString.find("#define texture2DLodEXT texture2D") != outputString.npos;
        }
        CHECK_EQUAL(platform == kShaderCompPlatformGLES20, foundTexture2DLodEXT);
    }


    PARAMETRIC_TEST_FIXTURE(ShaderCompilerTestFixture, Shader_Includes_FileWithUTF8BOM, (ShaderCompilerPlatform platform), AnalyzerAndAllPlatforms)
    {
        const char* fn = "test.cginc";
        const char* si = "\xEF\xBB\xBF float func() { return 3; }\n";
        CHECK(WriteTextToFile(fn, si));
        const char* s =
            "#include \"test.cginc\"\n"
            "float4 vert() : SV_POSITION { return func(); }\n"
            "float4 frag() : COLOR0 { return 0; }";

        RunTest(s, platform);

        DeleteFileAtPath(fn);
    }


    PARAMETRIC_TEST_FIXTURE(ShaderCompilerTestFixture, Shader_Includes_UTF8Path, (ShaderCompilerPlatform platform), AnalyzerAndAllPlatforms)
    {
        const char* fn = "\xD1\x84\xD0\xB0\xD0\xB9\xD0\xBB.cginc";
        const char* si = "float func() { return 3; }\n";
        CHECK(WriteTextToFile(fn, si));
        const char* s =
            "#include \"\xD1\x84\xD0\xB0\xD0\xB9\xD0\xBB.cginc\"\n"
            "float4 vert() : SV_POSITION { return func(); }\n"
            "float4 frag() : COLOR0 { return 0; }";

        RunTest(s, platform);

        DeleteFileAtPath(fn);
    }


    PARAMETRIC_TEST_FIXTURE(ShaderCompilerTestFixture, Shader_Includes_RelativePaths, (ShaderCompilerPlatform platform), AnalyzerAndAllPlatforms)
    {
        const char* fnRoot = "../testParentFromRoot.cginc";
        CHECK(WriteTextToFile(fnRoot, "float funcRoot() { return 3; }\n"));
        const char* fnRoot2 = "../testParentFromRoot2.cginc";
        CHECK(WriteTextToFile(fnRoot2, "float funcRoot2() { return 3; }\n"));
        const char* fnSubdir = "testParentToSubdir.cginc";
        CHECK(WriteTextToFile(fnSubdir, "float funcSubdir() { return 3; }\n"));
        const char* fnSubdir2 = "testParentToSubdir2.cginc";
        CHECK(WriteTextToFile(fnSubdir2, "float funcSubdir2() { return 3; }\n"));

        CreateDirectoryAtPath("subdir");
        inputData.includeContext.includePaths.push_back("subdir");

        const char* s =
            "#include \"../testParentFromRoot.cginc\"\n" // will get one from parent folder, ./../testParentFromRoot.cginc
            "#include \"../testParentFromRoot2.cginc\"\n" // will get one from parent folder, ./../testParentFromRoot2.cginc
            "#include \"../testParentToSubdir.cginc\"\n" // will get one relative to "include path", subdir/../testParentToSubdir.cginc
            "#include \"../testParentToSubdir2.cginc\"\n" // will get one relative to "include path", subdir/../testParentToSubdir2.cginc
            "float4 vert() : SV_POSITION { return funcRoot() + funcRoot2() + funcSubdir() + funcSubdir2(); }\n"
            "float4 frag() : COLOR0 { return 0; }";

        RunTest(s, platform);

        DeleteFileAtPath(fnRoot);
        DeleteFileAtPath(fnRoot2);
        DeleteFileAtPath(fnSubdir);
        DeleteFileAtPath(fnSubdir2);
        DeleteDirectoryAtPath("subdir");
    }

    PARAMETRIC_TEST_FIXTURE(ShaderCompilerTestFixture, Shader_Includes_MultiLevel_RelativePaths_RelativeStart, (ShaderCompilerPlatform platform), AnalyzerAndAllPlatforms)
    {
        CreateDirectoryAtPath("../subdir");
        CreateDirectoryAtPath("../subdir/nested");

        const char* fnFirst = "../subdir/first.cginc";
        CHECK(WriteTextToFile(fnFirst, "#include \"nested/second.cginc\"\n"));

        const char* fnSecond = "../subdir/nested/second.cginc";
        CHECK(WriteTextToFile(fnSecond, "float func() { return 3; }\n"));

        const char* s =
            "#include \"./../subdir/first.cginc\"\n"
            "float4 vert() : SV_POSITION { return func(); }\n"
            "float4 frag() : COLOR0 { return 0; }";

        RunTest(s, platform);

        DeleteFileAtPath(fnFirst);
        DeleteFileAtPath(fnSecond);
        DeleteDirectoryAtPath("../subdir/nested");
        DeleteDirectoryAtPath("../subdir");
    }

    PARAMETRIC_TEST_FIXTURE(ShaderCompilerTestFixture, Shader_Includes_MultiLevel_RelativePaths, (ShaderCompilerPlatform platform), AnalyzerAndAllPlatforms)
    {
        CreateDirectoryAtPath("subdir");
        CreateDirectoryAtPath("subdir/nested");

        const char* fnFirst = "subdir/first.cginc";
        CHECK(WriteTextToFile(fnFirst, "#include \"nested/second.cginc\"\n"));

        const char* fnSecond = "subdir/nested/second.cginc";
        CHECK(WriteTextToFile(fnSecond, "float func() { return 3; }\n"));

        const char* s =
            "#include \"subdir/first.cginc\"\n"
            "float4 vert() : SV_POSITION { return func(); }\n"
            "float4 frag() : COLOR0 { return 0; }";

        RunTest(s, platform);

        DeleteFileAtPath(fnFirst);
        DeleteFileAtPath(fnSecond);
        DeleteDirectoryAtPath("subdir/nested");
        DeleteDirectoryAtPath("subdir");
    }

    PARAMETRIC_TEST_FIXTURE(ShaderCompilerTestFixture, Shader_Includes_RelativeToIncluder_UnderSubdirectory, (ShaderCompilerPlatform platform), AnalyzerAndAllPlatforms)
    {
        CreateDirectoryAtPath("subdir");

        const char* fnFirst = "subdir/first.cginc";
        // this should work as second.cginc isn't in the search path but is relative to subdir/first.cginc
        CHECK(WriteTextToFile(fnFirst, "#include \"second.cginc\"\n"));

        const char* fnSecond = "subdir/second.cginc";
        CHECK(WriteTextToFile(fnSecond, "float func() { return 3; }\n"));

        const char* s =
            "#include \"subdir/first.cginc\"\n"
            "float4 vert() : SV_POSITION { return func(); }\n"
            "float4 frag() : COLOR0 { return 0; }";

        RunTest(s, platform);

        DeleteFileAtPath(fnFirst);
        DeleteFileAtPath(fnSecond);
        DeleteDirectoryAtPath("subdir");
    }

    PARAMETRIC_TEST_FIXTURE(ShaderCompilerTestFixture, Shader_Includes_ChainOfRelativeIncludes, (ShaderCompilerPlatform platform), AnalyzerAndAllPlatforms)
    {
        CreateDirectoryAtPath("subdir");

        const char* fnFirst = "subdir/first.cginc";
        CHECK(WriteTextToFile(fnFirst, "#include \"second.cginc\"\n"));

        const char* fnSecond = "subdir/second.cginc";
        CHECK(WriteTextToFile(fnSecond, "#include \"third.cginc\"\n"));

        const char* fnThird = "subdir/third.cginc";
        CHECK(WriteTextToFile(fnThird, "float func() { return 3; }\n"));

        const char* s =
            "#include \"subdir/first.cginc\"\n"
            "float4 vert() : SV_POSITION { return func(); }\n"
            "float4 frag() : COLOR0 { return 0; }";

        RunTest(s, platform);

        DeleteFileAtPath(fnFirst);
        DeleteFileAtPath(fnSecond);
        DeleteFileAtPath(fnThird);
        DeleteDirectoryAtPath("subdir");
    }

    PARAMETRIC_TEST_FIXTURE(ShaderCompilerTestFixture, Shader_Includes_RelativeToIncluder_HasPriority_OverIncludePath, (ShaderCompilerPlatform platform), AnalyzerAndAllPlatforms)
    {
        CreateDirectoryAtPath("subdir");
        CreateDirectoryAtPath("subdir/nested");

        const char* fnFirst = "subdir/nested/first.cginc";
        // this should be found over the second.cginc in the search path, as it
        // is next to the including file.
        CHECK(WriteTextToFile(fnFirst, "#include \"second.cginc\"\n"));

        const char* fnSecond = "subdir/nested/second.cginc";
        CHECK(WriteTextToFile(fnSecond, "float func() { return 3; }\n"));

        const char* fnRootSecond = "subdir/second.cginc";
        CHECK(WriteTextToFile(fnRootSecond, "error_marker;\n"));

        inputData.includeContext.includePaths.push_back("subdir");

        const char* s =
            "#include \"subdir/nested/first.cginc\"\n"
            "float4 vert() : SV_POSITION { return func(); }\n"
            "float4 frag() : COLOR0 { return 0; }";

        RunTest(s, platform);

        DeleteFileAtPath(fnFirst);
        DeleteFileAtPath(fnSecond);
        DeleteFileAtPath(fnRootSecond);
        DeleteDirectoryAtPath("subdir/nested");
        DeleteDirectoryAtPath("subdir");
    }

    PARAMETRIC_TEST_FIXTURE(ShaderCompilerTestFixture, Shader_Includes_RelativeToIncluder_HasPriority_OverWorkingDir, (ShaderCompilerPlatform platform), AnalyzerAndAllPlatforms)
    {
        CreateDirectoryAtPath("subdir");

        const char* fnFirst = "subdir/first.cginc";
        // this should be found over the second.cginc in the current directory, as it
        // is next to the including file.
        CHECK(WriteTextToFile(fnFirst, "#include \"second.cginc\"\n"));

        const char* fnSecond = "subdir/second.cginc";
        CHECK(WriteTextToFile(fnSecond, "float func() { return 3; }\n"));

        const char* fnRootSecond = "second.cginc";
        CHECK(WriteTextToFile(fnRootSecond, "error_marker;\n"));

        const char* s =
            "#include \"subdir/first.cginc\"\n"
            "float4 vert() : SV_POSITION { return func(); }\n"
            "float4 frag() : COLOR0 { return 0; }";

        RunTest(s, platform);

        DeleteFileAtPath(fnFirst);
        DeleteFileAtPath(fnSecond);
        DeleteFileAtPath(fnRootSecond);
        DeleteDirectoryAtPath("subdir");
    }

    PARAMETRIC_TEST_FIXTURE(ShaderCompilerTestFixture, Shader_Includes_ErrorMessage_NonexistantInclude, (ShaderCompilerPlatform platform), AnalyzerAndAllPlatforms)
    {
        const char* s =
            "#include \"nonexistant.cginc\"\n"
            "float4 vert() : POSITION { return func(); }\n"
            "float4 frag() : COLOR0 { return 0; }";

        RunIncludeFailureTest(s, platform, "nonexistant.cginc");
    }

    PARAMETRIC_TEST_FIXTURE(ShaderCompilerTestFixture, Shader_Includes_ErrorMessage_DontSearchIncludes_RelativeToPreviousInclude, (ShaderCompilerPlatform platform), AnalyzerAndAllPlatforms)
    {
        CreateDirectoryAtPath("subdir");

        const char* fnFirst = "subdir/first.cginc";
        CHECK(WriteTextToFile(fnFirst, "float other() { return 0; }\n"));

        const char* fnSecond = "subdir/second.cginc";
        CHECK(WriteTextToFile(fnSecond, "float func() { return 3; }\n"));

        // should not be found, as it is in subdir/, original algorithm for finding
        // includes would add subdir/ to the include search path
        const char* s =
            "#include \"subdir/first.cginc\"\n"
            "#include \"second.cginc\"\n"
            "float4 vert() : SV_POSITION { return func(); }\n"
            "float4 frag() : COLOR0 { return 0; }";

        RunIncludeFailureTest(s, platform, "second.cginc");

        DeleteFileAtPath(fnFirst);
        DeleteFileAtPath(fnSecond);
        DeleteDirectoryAtPath("subdir");
    }

    PARAMETRIC_TEST_FIXTURE(ShaderCompilerTestFixture, Shader_Includes_ErrorMessage_DontSearchIncludes_RelativeToParent, (ShaderCompilerPlatform platform), AnalyzerAndAllPlatforms)
    {
        // we need two levels here as . is implicitly in the global search path (as project root)
        CreateDirectoryAtPath("subdir");
        CreateDirectoryAtPath("subdir/nested");

        // file tree:
        // subdir/
        // subdir/first.cginc
        // subdir/second.cginc
        // subdir/nested/
        // subdir/nested/third.cginc

        const char* fnFirst = "subdir/first.cginc";
        CHECK(WriteTextToFile(fnFirst, "#include \"nested/third.cginc\"\n"));

        const char* fnSecond = "subdir/second.cginc";
        CHECK(WriteTextToFile(fnSecond, "float func() { return 3; }\n"));

        const char* fnThird = "subdir/nested/third.cginc";
        // this include should fail, as it is not found in either the global
        // search path (in the root) or next to the file (in subdir/nested/)
        // We make sure it won't implicitly also search the parent includer's
        // path (ie. search subdir/ because it's included from subdir/first.cginc)
        CHECK(WriteTextToFile(fnThird, "#include \"second.cginc\"\n"));

        // should not be found, as it is in subdir/, original algorithm for finding
        // includes would add subdir/ to the include search path
        const char* s =
            "#include \"subdir/first.cginc\"\n"
            "float4 vert() : SV_POSITION { return func(); }\n"
            "float4 frag() : COLOR0 { return 0; }";

        RunIncludeFailureTest(s, platform, "second.cginc");

        DeleteFileAtPath(fnFirst);
        DeleteFileAtPath(fnSecond);
        DeleteFileAtPath(fnThird);
        DeleteDirectoryAtPath("subdir/nested");
        DeleteDirectoryAtPath("subdir");
    }

    PARAMETRIC_TEST_FIXTURE(ShaderCompilerTestFixture, Shader_Includes_FilesWithNoTrailingNewline_Work, (ShaderCompilerPlatform platform), AnalyzerAndAllPlatforms)
    {
        const char* fn1 = "IncludeTest1.cginc";
        const char* fn2 = "IncludeTest2.cginc";
        const char* fn3 = "IncludeTest3.cginc";
        CHECK(WriteTextToFile(fn1,
            "#define YES\n"
            "#ifdef YES\n"
            "float func1() { return 3; }\n"
            "#endif"));     // ends with preprocessor directive, without a newline
        CHECK(WriteTextToFile(fn2, "float func2() { return 3; }")); // no newline at end
        CHECK(WriteTextToFile(fn3, "float func3() { return 3; }"));
        const char* s =
            "#include \"IncludeTest1.cginc\"\n"
            "#include \"IncludeTest2.cginc\"\n"
            "#include \"IncludeTest3.cginc\"\n"
            "float4 vert() : SV_POSITION { return func1() + func2() + func3(); }\n"
            "float4 frag() : COLOR0 { return 0; }";

        RunTest(s, platform);

        DeleteFileAtPath(fn1);
        DeleteFileAtPath(fn2);
        DeleteFileAtPath(fn3);
    }

    PARAMETRIC_TEST_FIXTURE(ShaderCompilerTestFixture, Shader_Includes_InRedirectedFolders_Work, (ShaderCompilerPlatform platform), AnalyzerAndAllPlatforms)
    {
        const char* fn1 = "redirected/Include1.cginc";
        const char* fn2 = "IncludeRedirected.cginc";
        const char* fn3 = "redirected/Include2.cginc";
        CreateDirectoryAtPath("redirected");

        CHECK(WriteTextToFile(fn1, "float func1() { return 1; }\n"));
        CHECK(WriteTextToFile(fn2, "#include \"re/dir/Include2.cginc\"\n"));
        CHECK(WriteTextToFile(fn3, "float func2() { return 2; }\n"));
        const char* s =
            "#include \"re/dir/Include1.cginc\"\n"
            "#include \"IncludeRedirected.cginc\"\n"
            "float4 vert() : SV_POSITION { return func1() + func2(); }\n"
            "float4 frag() : COLOR0 { return 0; }";

        RunTest(s, platform);

        DeleteFileAtPath(fn1);
        DeleteFileAtPath(fn2);
        DeleteFileAtPath(fn3);
        DeleteDirectoryAtPath("redirected");
    }

    PARAMETRIC_TEST_FIXTURE(ShaderCompilerTestFixture, HLSLccMetalSamplingInterpolationQualifiers, (ShaderCompilerPlatform platform), D3D11_OpenGLCore_Metal)
    {
        const char *s =
            "#include \"UnityCG.cginc\"\n"
            "struct u2v {\n"
            "\thalf4 vertex : POSITION;\n"
            "\thalf4 color : COLOR;\n"
            "};\n"
            "struct v2f {\n"
            "\thalf4 pos : SV_POSITION;\n"
            "\thalf4 tx0 : TEXCOORD0;\n"
            "\tcentroid half4 tx1 : TEXCOORD1;\n"
            "\tnointerpolation half4 tx2 : TEXCOORD2;\n"
            "\tnoperspective half4 tx3 : TEXCOORD3;\n"
            "\tnoperspective centroid half4 tx4 : TEXCOORD4;\n"
            "\tsample half4 tx5 : TEXCOORD5;\n"
            "\tnoperspective sample half4 tx6 : TEXCOORD6;\n"
            "};\n"
            "v2f vert (u2v v) {\n"
            "\tv2f o;\n"
            "\to.pos = v.vertex;\n"
            "\to.tx0 = 0;\n"
            "\to.tx1 = 0;\n"
            "\to.tx2 = 0;\n"
            "\to.tx3 = 0;\n"
            "\to.tx4 = 0;\n"
            "\to.tx5 = 0;\n"
            "\to.tx6 = 0;\n"
            "\treturn o;\n"
            "}\n"
            "half4 frag(v2f i) : SV_Target\n"
            "{\n"
            "\treturn i.tx0 + i.tx1 + i.tx2 + i.tx3 + i.tx4 + i.tx5 + i.tx6;\n"
            "}\n";

        inputData.requirements = kShaderRequireShaderModel40 | kShaderRequireCubeArray;

        inputData.p.entryName[kProgramVertex] = "vert";
        inputData.p.entryName[kProgramFragment] = "frag";

        TestCompilationOk(s, platform);
        TestCompilationOk(s, kShaderCompPlatformD3D11);
        TestCompilationOk(s, kShaderCompPlatformMetal);

        if (platform == kShaderCompPlatformMetal)
        {
            core::string outputString = GetMetalFragmentShaderOutputString();
            CHECK_NOT_EQUAL(core::string::npos, outputString.find("float4 TEXCOORD0 [[ user(TEXCOORD0) ]]"));
            CHECK_NOT_EQUAL(core::string::npos, outputString.find("float4 TEXCOORD1 [[ user(TEXCOORD1) ]]  [[ centroid_perspective ]]"));
            CHECK_NOT_EQUAL(core::string::npos, outputString.find("float4 TEXCOORD2 [[ user(TEXCOORD2) ]]  [[ flat ]];"));
            CHECK_NOT_EQUAL(core::string::npos, outputString.find("float4 TEXCOORD3 [[ user(TEXCOORD3) ]]  [[ center_no_perspective ]]"));
            CHECK_NOT_EQUAL(core::string::npos, outputString.find("float4 TEXCOORD4 [[ user(TEXCOORD4) ]]  [[ centroid_no_perspective ]]"));
            CHECK_NOT_EQUAL(core::string::npos, outputString.find("float4 TEXCOORD5 [[ user(TEXCOORD5) ]]  [[ sample_perspective ]]"));
            CHECK_NOT_EQUAL(core::string::npos, outputString.find("float4 TEXCOORD6 [[ user(TEXCOORD6) ]]  [[ sample_no_perspective ]]"));
        }
    }

    PARAMETRIC_TEST_FIXTURE(ShaderCompilerTestFixture, HLSLccMetalForceHalf, (ShaderCompilerPlatform platform), D3D11_OpenGLCore_Metal)
    {
        const char *s =
            "#include \"UnityCG.cginc\"\n"
            "static const half4 foo[2] = { half4(1,2,3,4), half4(5,6,7,8) };\n"
            "struct u2v {\n"
            "\thalf4 vertex : POSITION;\n"
            "\thalf4 color : COLOR;\n"
            "};\n"
            "struct v2f {\n"
            "\thalf4 pos : SV_POSITION;\n"
            "\thalf4 color : COLOR;\n"
            "};\n"
            "v2f vert (u2v v) {\n"
            "\tv2f o;\n"
            "\to.pos = v.vertex;\n"
            "\to.color = v.color;\n"
            "\treturn o;\n"
            "}\n"
            "half4 frag(v2f i) : SV_Target\n"
            "{\n"
            "\tint j = (int)i.color.x;\n"
            "\thalf dp = max(0.001f, dot(i.color, i.color));"
            "\treturn (i.color) * half4(0.5h,0.5h,0.5h,0.5h) + foo[j] + dp;\n"
            "}\n";

        inputData.requirements = kShaderRequireShaderModel30;
        inputData.p.entryName[kProgramVertex] = "vert";
        inputData.p.entryName[kProgramFragment] = "frag";

        TestCompilationOk(s, platform);

        if (platform == kShaderCompPlatformMetal)
        {
            core::string outputString = GetMetalFragmentShaderOutputString();
            // Check some instructions if we force half precision if all operands allow it
            // MAX
            CHECK_NOT_EQUAL(core::string::npos, outputString.find("u_xlat0 = max(u_xlat0, 0.00100000005);"));
            // MAD
            CHECK_NOT_EQUAL(core::string::npos, outputString.find("u_xlat1 = fma(input.COLOR0, float4(0.5, 0.5, 0.5, 0.5), ImmCB_0[u_xlati2]);"));

            ShaderCompileArg arg = ShaderCompileArg("SHADER_API_MOBILE", "1");
            TestCompilationOkWithCompileArg(s, kShaderCompPlatformMetal, &arg);

            outputString = GetMetalFragmentShaderOutputString();
            // Check some instructions if we force half precision if all operands allow it
            // MAX
            CHECK_NOT_EQUAL(core::string::npos, outputString.find("u_xlat16_1 = max(u_xlat16_0.x, half(0.00100000005));"));
            // MAD
            CHECK_NOT_EQUAL(core::string::npos, outputString.find("u_xlat16_0 = fma(input.COLOR0, half4(0.5, 0.5, 0.5, 0.5), half4(ImmCB_0[u_xlati3]));"));
        }
    }

    PARAMETRIC_TEST_FIXTURE(ShaderCompilerTestFixture, HLSLccDepthOutput, (ShaderCompilerPlatform platform), D3D11_OpenGLCore_Metal_GLES20)
    {
        const char *s =
            "struct CamMotionVectors {\n"
            "\tfloat4 pos : SV_POSITION;\n"
            "};\n"
            "CamMotionVectors vert(float4 vertex : POSITION, float3 normal : NORMAL) {\n"
            "\tCamMotionVectors o;\n"
            "\to.pos = vertex;\n"
            "\treturn o;\n"
            "}\n"
            "half4 frag(CamMotionVectors i, out float outDepth : SV_Depth) : SV_Target {\n"
            "\toutDepth = 0.5;\n"
            "\thalf2 foo = half2(i.pos.xy);\n"
            "\treturn half4(foo,1,1);\n"
            "}\n";

        inputData.requirements = kShaderRequireShaderModel30;
        inputData.p.entryName[kProgramVertex] = "vert";
        inputData.p.entryName[kProgramFragment] = "frag";
        inputData.compileFlags = kShaderCompFlags_PreferHLSLcc;

        TestCompilationOk(s, platform);

        if (platform == kShaderCompPlatformMetal)
        {
            core::string outputString = GetMetalFragmentShaderOutputString();
            // check if depth output is declared in output structure
            CHECK(outputString.find("[[ depth(any) ]]") != core::string::npos);
            // check if fragment color output component count is maintained
            CHECK(outputString.find("output.SV_Target0.xy") != core::string::npos);
        }

        if (platform == kShaderCompPlatformGLES20 && !WasUsingCompiler(kShaderCompFlags_PreferHLSL2GLSL))
        {
            core::string outputString = GetOutputString();
            // check if depth output is declared in output structure
            CHECK(outputString.find("gl_FragDepthEXT") != core::string::npos);
        }
    }

    PARAMETRIC_TEST_FIXTURE(ShaderCompilerTestFixture, HLSLccDepthLEOutput, (ShaderCompilerPlatform platform), D3D11_OpenGLCore_Metal)
    {
        const char *s =
            "float4 vert(float4 vertex : POSITION) : SV_POSITION {\n"
            "\treturn vertex;\n"
            "}\n"
            "void frag(linear noperspective centroid float4 pos : SV_POSITION, out float4 outColor : SV_Target0, out float outDepth : SV_DepthLessEqual) {\n"
            "\toutDepth = 0.5;\n"
            "\toutColor = float4(pos.x, pos.y,1,1);\n"
            "}\n";

        inputData.requirements = kShaderRequireShaderModel50;
        inputData.p.entryName[kProgramVertex] = "vert";
        inputData.p.entryName[kProgramFragment] = "frag";
        inputData.compileFlags = kShaderCompFlags_PreferHLSLcc;

        TestCompilationOk(s, platform);

        if (platform == kShaderCompPlatformMetal)
        {
            core::string outputString = GetMetalFragmentShaderOutputString();
            CHECK(outputString.find("[[ depth(less) ]]") != core::string::npos);
            CHECK(outputString.find("output.SV_Target0.xy") != core::string::npos);
        }
    }

    PARAMETRIC_TEST_FIXTURE(ShaderCompilerTestFixture, HLSLccDepthGEOutput, (ShaderCompilerPlatform platform), D3D11_OpenGLCore_Metal)
    {
        const char *s =
            "float4 vert(float4 vertex : POSITION) : SV_POSITION {\n"
            "\treturn vertex;\n"
            "}\n"
            "void frag(linear noperspective centroid float4 pos : SV_POSITION, out float4 outColor : SV_Target0, out float outDepth : SV_DepthGreaterEqual) {\n"
            "\toutDepth = 0.5;\n"
            "\toutColor = float4(pos.x, pos.y,1,1);\n"
            "}\n";

        inputData.requirements = kShaderRequireShaderModel50;
        inputData.p.entryName[kProgramVertex] = "vert";
        inputData.p.entryName[kProgramFragment] = "frag";
        inputData.compileFlags = kShaderCompFlags_PreferHLSLcc;

        TestCompilationOk(s, platform);

        if (platform == kShaderCompPlatformMetal)
        {
            core::string outputString = GetMetalFragmentShaderOutputString();
            CHECK(outputString.find("[[ depth(greater) ]]") != core::string::npos);
            CHECK(outputString.find("output.SV_Target0.xy") != core::string::npos);
        }
    }

    TEST_FIXTURE(ShaderCompilerTestFixture, HLSLcc_Advanced_Blend_Works)
    {
        const char *s =
            "#include \"UnityCG.cginc\"\n"
            "UNITY_REQUIRE_ADVANCED_BLEND(screen);\n"
            "UNITY_REQUIRE_ADVANCED_BLEND(overlay);\n"
            "UNITY_REQUIRE_ADVANCED_BLEND(darken);\n"
            "float4 vert() : POSITION {\n"
            "  return float4(1.0, 0.0, 1.0, 1.0);\n"
            "}\n"
            "float4 frag() : COLOR0 { return float4(1.0, 0.5, 0.5, 1.0); }";

        inputData.p.entryName[kProgramVertex] = "vert";
        inputData.p.entryName[kProgramFragment] = "frag";

        TestCompilationOk(s, kShaderCompPlatformGLES3Plus);

        core::string outputString = GetOutputString();
        // check if the correct layout declaration is present
        CHECK(outputString.find("layout(blend_support_screen) out;") != core::string::npos);
        CHECK(outputString.find("layout(blend_support_overlay) out;") != core::string::npos);
        CHECK(outputString.find("layout(blend_support_darken) out;") != core::string::npos);
    }

    PARAMETRIC_TEST_FIXTURE(ShaderCompilerTestFixture, HLSLccMetalFloatDimension, (ShaderCompilerPlatform platform), D3D11_OpenGLCore_Metal)
    {
        const char *s =
            "uniform float _Floats[4];\n"
            "struct u2v {\n"
            "\tfloat4 vertex : POSITION;\n"
            "\thalf2 texcoord : TEXCOORD0;\n"
            "};\n"
            "struct v2f {\n"
            "\thalf4 pos : SV_POSITION;\n"
            "\thalf2 uv  : TEXCOORD0;\n"
            "};\n"
            "v2f vert (u2v v) {\n"
            "\tv2f o;\n"
            "\to.pos = v.vertex;\n"
            "\to.uv = v.texcoord;\n"
            "\treturn o;\n"
            "}\n"
            "half4 frag(v2f i) : SV_Target\n"
            "{\n"
            "\thalf fuv = frac(i.uv.x);\n"
            "\tint index = floor(fuv * 4);\n"
            "\thalf3 color = _Floats[index];\n"
            "\treturn half4(color, 1);\n"
            "}\n";

        inputData.requirements = kShaderRequireShaderModel30;
        inputData.p.entryName[kProgramVertex] = "vert";
        inputData.p.entryName[kProgramFragment] = "frag";

        TestCompilationOk(s, platform);

        if (platform == kShaderCompPlatformMetal)
        {
            core::string outputString = GetMetalFragmentShaderOutputString();
            // check if float arrays are translated as float[] (they will be float4[] for compute shaders for D3D layout compatibility)
            CHECK(outputString.find("float _Floats[4];") != core::string::npos);
        }
    }

    TEST_FIXTURE(ShaderCompilerTestFixture, HLSLcc_LogicalOperators)
    {
        const char *s =
            "float A;\n"
            "float B;\n"
            "float C;\n"
            "struct u2v {\n"
            "\tfloat4 pos : SV_POSITION;\n"
            "};\n"
            "u2v vert(float4 vertex : POSITION) {\n"
            "\tu2v o;\n"
            "\to.pos = vertex;\n"
            "\treturn o;\n"
            "}\n"
            "half4 frag() : SV_Target {\n"
            "\tif ( (A>B && C>B) || (A<-B && C<-B) ) {\n"
            "\t\treturn half4(0,0,0,0);\n"
            "\t} else {\n"
            "\t\treturn half4(1,1,1,1);\n"
            "\t}\n"
            "}\n";

        inputData.requirements = kShaderRequireShaderModel30;
        inputData.p.entryName[kProgramVertex] = "vert";
        inputData.p.entryName[kProgramFragment] = "frag";

        TestCompilationOk(s, kShaderCompPlatformMetal);
        core::string outputString = GetMetalFragmentShaderOutputString();
        CHECK(outputString.find("u_xlatb1.y && u_xlatb1.x") != core::string::npos);
        CHECK(outputString.find("u_xlatb1.x || u_xlatb0.x") != core::string::npos);
    }

    TEST_FIXTURE(ShaderCompilerTestFixture, HLSLcc_BoolUpscaleGeneratesMinusOne_onGLES20)
    {
        const char *s =
            "float A;\n"
            "struct u2v {\n"
            "\tfloat4 pos : SV_POSITION;\n"
            "};\n"
            "u2v vert(float4 vertex : POSITION) {\n"
            "\tu2v o;\n"
            "\to.pos = vertex;\n"
            "\treturn o;\n"
            "}\n"
            "half4 frag() : SV_Target {\n"
            "\tclip(A);\n"
            "\treturn half4(1,1,1,1);\n"
            "}\n";

        inputData.requirements = kShaderRequireShaderModel20;
        inputData.p.entryName[kProgramVertex] = "vert";
        inputData.p.entryName[kProgramFragment] = "frag";

        TestCompilationOk(s, kShaderCompPlatformGLES20);
        core::string outputString = GetOutputString();
        CHECK(outputString.find("int(u_xlatb0) * -1") != core::string::npos);
    }

    TEST_FIXTURE(ShaderCompilerTestFixture, HLSLcc_BoolUpscaleGenerates32BitUInt_onGLCore)
    {
        const char *s =
            "float A;\n"
            "struct u2v {\n"
            "\tfloat4 pos : SV_POSITION;\n"
            "};\n"
            "u2v vert(float4 vertex : POSITION) {\n"
            "\tu2v o;\n"
            "\to.pos = vertex;\n"
            "\treturn o;\n"
            "}\n"
            "half4 frag() : SV_Target {\n"
            "\tclip(A);\n"
            "\treturn half4(1,1,1,1);\n"
            "}\n";

        inputData.requirements = kShaderRequireShaderModel20;
        inputData.p.entryName[kProgramVertex] = "vert";
        inputData.p.entryName[kProgramFragment] = "frag";

        TestCompilationOk(s, kShaderCompPlatformOpenGLCore);
        core::string outputString = GetOutputString();
        CHECK(outputString.find("int(u_xlatb0) * int(0xffffffffu)") != core::string::npos);
    }

    PARAMETRIC_TEST_FIXTURE(ShaderCompilerTestFixture, HLSLcc_InputRedirectUnevenComponents, (ShaderCompilerPlatform platform), OpenGLCore_Metal_Switch)
    {
        const char * s =
            "struct v2f {\n"
            "\tfloat4 uv : TEXCOORD0;\n"
            "\tfloat fog : TEXCOORD1;\n"
            "\thalf   alpha : COLOR;\n"
            "\tfloat4 pos : SV_POSITION;\n"
            "};\n"
            "v2f vert() {\n"
            "\tv2f o;\n"
            "\to.uv = float4(1,2,3,4);\n"
            "\to.fog = 0;\n"
            "\to.alpha = 1;\n"
            "\to.pos = float4(4,3,2,1);\n"
            "\treturn o;\n"
            "}\n"
            "float4 frag(v2f i) : COLOR0 { return float4(i.uv.x, i.fog, i.alpha, i.pos.y); }";

        TestCompilationOk(s, platform);
    }

    TEST_FIXTURE(ShaderCompilerTestFixture, UpwardPromotion)
    {
        const char *s =
            "sampler2D _MainTex;\n"
            "struct u2v {\n"
            "\tfloat4 vertex : POSITION;\n"
            "\thalf2 texcoord : TEXCOORD0;\n"
            "};\n"
            "struct v2f {\n"
            "\thalf4 pos : SV_POSITION;\n"
            "\thalf2 uv  : TEXCOORD0;\n"
            "};\n"
            "v2f vert (u2v v) {\n"
            "\tv2f o;\n"
            "\to.pos = v.vertex;\n"
            "\to.uv = v.texcoord;\n"
            "\treturn o;\n"
            "}\n"
            "half4 frag (v2f i) : SV_Target {\n"
            "\thalf2 a = half2(i.uv);\n"
            "\tbool2 b = step(0.5, a);\n"
            "\thalf2 c = half2(0,0);\n"
            "\tif (b.x) c.x = a.x;\n"
            "\tif (b.y) c.y = a.y;\n"
            "\thalf4 col = tex2Dgrad(_MainTex, i.uv, i.uv.x*0.05, i.uv.y*0.05);\n"
            "\treturn half4(c, col.z, 1);\n"
            "}\n";

        inputData.requirements = kShaderRequireShaderModel30;
        inputData.p.entryName[kProgramVertex] = "vert";
        inputData.p.entryName[kProgramFragment] = "frag";

        TestCompilationOk(s, kShaderCompPlatformGLES20);
        core::string outputString = GetOutputString();

        // Check that step(half,half2) is matched to step(half2,half2) and vectors are not trimmed to scalar.
        if (WasUsingCompiler(kShaderCompFlags_PreferHLSL2GLSL))
        {
            // Original Cg rules in hlsl2glsl would match to step(half,half) which is different from HLSL rules.
            CHECK(outputString.find("tmpvar_2 = bvec2(vec2(greaterThanEqual (xlv_TEXCOORD0, vec2(0.5, 0.5))));") != core::string::npos);
        }
        else
        {
            CHECK(outputString.find("u_xlatb0.xy = greaterThanEqual(vs_TEXCOORD0.xyxx, vec4(0.5, 0.5, 0.0, 0.0)).xy;") != core::string::npos);
        }
        CHECK(outputString.find("texture2DGradEXT") != core::string::npos);

        TestCompilationOk(s, kShaderCompPlatformMetal);
        outputString = GetMetalFragmentShaderOutputString();

        CHECK(outputString.find("output.SV_Target0.xy = select(float2(0.0, 0.0), input.TEXCOORD0.xy, bool2(u_xlatb0.xy));") != core::string::npos);
        CHECK(outputString.find("gradient2d") != core::string::npos);
    }

    TEST_FIXTURE(ShaderCompilerTestFixture, ConstPrecision)
    {
        const char *s =
            "struct u2v {\n"
            "\tfloat4 vertex : POSITION;\n"
            "\thalf2 texcoord : TEXCOORD0;\n"
            "};\n"
            "struct v2f {\n"
            "\thalf4 pos  : SV_POSITION;\n"
            "\thalf2 uv : TEXCOORD0;\n"
            "};\n"
            "static const half3 ha[3] = {\n"
            "\thalf3(1, 2, 3),\n"
            "\thalf3(4, 5, 6),\n"
            "\thalf3(7, 8, 9)\n"
            "};\n"
            "static const float3 fa[3] = {\n"
            "\tfloat3(11, 12, 13),\n"
            "\tfloat3(14, 15, 16),\n"
            "\tfloat3(17, 18, 19)\n"
            "};\n"
            "v2f vert (u2v v) {\n"
            "\tv2f o;\n"
            "\to.pos = v.vertex;\n"
            "\to.uv = v.texcoord;\n"
            "\treturn o;\n"
            "}\n"
            "half4 frag (v2f i) : SV_Target {\n"
            "\thalf3 h = 0;\n"
            "\tfloat3 f = 0;\n"
            "\tfloat3 p = float3(i.uv.xy,1);\n"
            "\tfor (int j = 0; j < (int)(i.uv.x * 3.0); j++) {\n"
            "\t\th += ha[j];\n"
            "\t\tf += fa[j];\n"
            "\t\tf += p * ha[0];\n"
            "\t}\n"
            "\treturn half4(h.xy, (half2)f.xy);\n"
            "}\n";

        inputData.requirements = kShaderRequireShaderModel30;
        inputData.p.entryName[kProgramVertex] = "vert";
        inputData.p.entryName[kProgramFragment] = "frag";

        TestCompilationOk(s, kShaderCompPlatformGLES20);
        if (WasUsingCompiler(kShaderCompFlags_PreferHLSL2GLSL))
        {
            core::string outputString = GetOutputString();

            // Check that constant float precision qualifier is propagated correctly to GLES2.0 shaders
            CHECK(outputString.find("mediump vec3 ha[3];") != core::string::npos);
            CHECK(outputString.find("highp vec3 fa[3];") != core::string::npos);
        }
        TestCompilationOk(s, kShaderCompPlatformMetal);
    }

    TEST_FIXTURE(ShaderCompilerTestFixture, HLSLcc_FracOnConstLiteral_Works)
    {
        // We had a bug in d3dcompiler wrapper (Wodka) where due to non-implemented
        // floorf() import frac() on a constant value would just return the original value,
        // instead of proper fraction.
        const char* s =
            "float4 vert() : SV_POSITION { return 1.0; }\n"
            "float4 frag() : SV_Target { float val = 3.5; return frac(val); }";
        TestCompilationOk(s, kShaderCompPlatformOpenGLCore);
        core::string outputString = GetOutputString();
        CHECK(outputString.find("SV_Target0 = vec4(0.5, 0.5, 0.5, 0.5);") != core::string::npos); // with the bug present, was containing 3.5 instead of 0.5
    }

    PARAMETRIC_TEST_FIXTURE(ShaderCompilerTestFixture, HLSLcc_ConstantBufferReflection, (ShaderCompilerPlatform platform), OpenGLCore_Metal)
    {
        const char * s =
            "struct nester {\n"
            "\tint count;\n"
            "   float4 f4Stuffs;\n"
            "};\n"
            "struct test {\n"
            "\tfloat1 foo;\n"
            "\tfloat2 bar;\n"
            "\tint counter; };\n"
            "cbuffer myCB { float4 ifoo; test _myTest; };"
            "float4 vert() : POSITION {\n"
            "  return ifoo + _myTest.bar.xyxy;\n"
            "}\n"
            "float4 frag() : COLOR0 { return 0; }";

        TestCompilationOk(s, platform);
        if (platform == kShaderCompPlatformMetal)
        {
            CHECK_EQUAL(1, refl.m_ConstantBuffers.size());
            CHECK_EQUAL(16, refl.m_ConstantBuffers[0].m_Members[1].m_Binding);
        }
    }

    // Hlslcc transforms matrices into float arrays, Vulkan reflection wasn't doing name demangling properly when only element 0 was accessed
    TEST_FIXTURE(ShaderCompilerTestFixture, HLSLcc_VulkanMatrixTransformation)
    {
        const char * s =
            "\t\tcbuffer UnityInstancing_PerDraw0\n"
            "\t\t{\n"
            "\t\t\tstruct\n"
            "\t\t{\n"
            "\t\t\tfloat4x4 unity_ObjectToWorldArray;\n"
            "\t\t\tfloat4x4 unity_WorldToObjectArray;\n"
            "\t\t}\n"
            "\t\tunityBuiltins0Array[200];\n"
            "\t\t}\n"
            "uint instID : SV_InstanceID;\n"
            "uint other;\n"
            "float4 vert() : POSITION {\n"
            "    return mul(float4(1.0, 0.0, 1.0, 0.0), unityBuiltins0Array[0].unity_ObjectToWorldArray);\n"
            "}\n"
            "min16float4 frag() : COLOR0 { return min16float4(0.0, 0.0, 0.0, 1.0); }";

        TestCompilationOk(s, kShaderCompPlatformVulkan);
        // Test that reflection doesn't have anything that starts with 'hlslcc_mtx'
        for (int i = 0; i < refl.m_ConstantBuffers.size(); ++i)
        {
            auto &cb = refl.m_ConstantBuffers[i];
            for (int k = 0; k < cb.m_Members.size(); ++k)
            {
                auto &memb = cb.m_Members[k];
                CHECK_EQUAL(memb.m_Name.find("hlslcc_mtx"), core::string::npos);
            }
        }
    }

    TEST_FIXTURE(ShaderCompilerTestFixture, HLSLcc_StaticBranching)
    {
        const char * s =
            "float4 condition;\n"
            "float4 vert() : POSITION {\n"
            "  if(condition.x > condition.y)\n"
            "    return float4(0.0, 0.0, 1.0, 0.0);\n"
            "  else\n"
            "    return float4(1.0, 1.0, 1.0, 1.0);\n"
            "}\n"
            "min16float4 frag() : COLOR0 { return min16float4(0.0, 0.0, 0.0, 1.0); }";

        TestCompilationOk(s, kShaderCompPlatformVulkan);
    }

    // Case 925522: If a depth texture is used with both depth comparison and normal samplers, compiler would hit an assert.
    TEST_FIXTURE(ShaderCompilerTestFixture, HLSLcc_DepthSamplersReflection_WorksOnVulkan)
    {
        const char * s =
            "Texture2D_float _DepthTex;\n"
            "float2 uv;\n"
            "SamplerState smp1_clamp_point;\n"
            "SamplerComparisonState sampler_DepthTex;\n"
            "float4 vert() : POSITION {\n"
            "    return float4(1.0, 1.0, 1.0, 1.0);\n"
            "}\n"
            "float4 frag() : COLOR0 { return _DepthTex.Sample(smp1_clamp_point, uv) + _DepthTex.SampleCmpLevelZero(sampler_DepthTex, uv, 0.5); }";

        TestCompilationOk(s, kShaderCompPlatformVulkan);
    }

    TEST_FIXTURE(ShaderCompilerTestFixture, HLSLcc_ConvertsMatricesToVectorArrays)
    {
        const char * s =
            "float4x4 testMatrix;\n"
            "float4 vert() : POSITION {\n"
            "  return testMatrix[1];\n"
            "}\n"
            "float4 frag() : COLOR0 { return 0; }";

        TestCompilationOk(s, kShaderCompPlatformOpenGLCore);

        core::string outputString = GetOutputString();

        // Just check that the vecs with our magic prefix are created when
        // the original shader contains matrices
        CHECK(outputString.find("vec4 hlslcc_mtx") != core::string::npos);
    }

    TEST_FIXTURE(ShaderCompilerTestFixture, HLSLcc_RecognizesFloatAndHalfSuffixesOnTexturesAndSamplers)
    {
        const char* samplersAndTextures[] =
        {
            "sampler2D",
            "sampler3D",
            "samplerCUBE",
            "Texture2D",
            "Texture2DMS",
            "Texture2DArray",
            "Texture2DMSArray",
            "TextureCube",
            "TextureCubeArray",
            "Texture3D",
        };
        const char* samplerStates[ARRAY_SIZE(samplersAndTextures)] =
        {
            "",
            "",
            "",
            "SamplerState sampler_tex;",
            "SamplerState sampler_tex;",
            "SamplerState sampler_tex;",
            "SamplerState sampler_tex;",
            "SamplerState sampler_tex;",
            "SamplerState sampler_tex;",
            "SamplerState sampler_tex;",
        };
        const char* suffixes[] =
        {
            "",
            "_float",
            "_half"
        };
        const char* suffixToPlatform[ARRAY_SIZE(suffixes)] =
        {
            "mediump",
            "highp",
            "mediump"
        };
        const char* platformOutputSuffix[ARRAY_SIZE(samplersAndTextures)] =
        {
            "sampler2D",
            "sampler3D",
            "samplerCube",
            "sampler2D",
            "sampler2DMS",
            "sampler2DArray",
            "sampler2DMSArray",
            "samplerCube",
            "samplerCubeArray",
            "sampler3D",
        };
        const char* postfixes[ARRAY_SIZE(samplersAndTextures)] =
        {
            "",
            "",
            "",
            "",
            "<float4, 4>",
            "",
            "<float4, 4>",
            "",
            "",
            "",
        };
        const char* sampleCmd[ARRAY_SIZE(samplersAndTextures)] =
        {
            "tex2D(tex, i.uv.xy)",
            "tex3D(tex, i.uv.xyz)",
            "texCUBE(tex, i.uv.xyz)",
            "tex.Sample(sampler_tex, i.uv.xy)",
            "tex.Load(i.uv.xy, 0)",
            "tex.Sample(sampler_tex, i.uv.xyz)",
            "tex.Load(half4(i.uv.xyz, 0), 0)",
            "tex.Sample(sampler_tex, i.uv.xyz)",
            "tex.Sample(sampler_tex, half4(i.uv.xyz, 0))",
            "tex.Sample(sampler_tex, i.uv.xyz)",
        };

        const size_t samplerVariants = ARRAY_SIZE(samplersAndTextures);
        const size_t suffixVariantes = ARRAY_SIZE(suffixes);
        for (size_t i = 0; i < samplerVariants; ++i)
        {
            for (size_t j = 0; j < suffixVariantes; ++j)
            {
                core::string variant(samplersAndTextures[i]);
                variant.append(suffixes[j]);
                variant.append(postfixes[i]);

                core::string source(
                    "struct u2v {\n"
                    "\tfloat4 vertex : POSITION;\n"
                    "\thalf2 texcoord : TEXCOORD0;\n"
                    "};\n"
                    "struct v2f {\n"
                    "\thalf4 pos  : SV_POSITION;\n"
                    "\thalf4 uv : TEXCOORD0;\n"
                    "};\n"
                );
                source.append(variant);
                source.append(" tex;\n");
                source.append(samplerStates[i]);
                source.append('\n');
                source.append(
                    "v2f vert (u2v v) {\n"
                    "\tv2f o;\n"
                    "\to.pos = v.vertex;\n"
                    "\to.uv = half4(v.texcoord, 0, 0);\n"
                    "\treturn o;\n"
                    "}\n"
                    "half4 frag (v2f i) : SV_Target {\n"
                    "\treturn "
                );
                source.append(sampleCmd[i]);
                source.append(";\n}\n");

                TestCompilationOk(source.c_str(), kShaderCompPlatformGLES3Plus);
                core::string outputString = GetOutputString();

                core::string expected("uniform ");
                expected.append(suffixToPlatform[j]);
                expected.append(' ');
                expected.append(platformOutputSuffix[i]);

                CHECK(outputString.find(expected) != core::string::npos);

                errors.clear();
                metalFragmentErrors.clear();

                if (output)
                {
                    gPluginDispatcher->ReleaseShader(output);
                    output = NULL;
                }
                if (metalFragmentOutput)
                {
                    gPluginDispatcher->ReleaseShader(metalFragmentOutput);
                    metalFragmentOutput = NULL;
                }
            }
        }
    }

    PARAMETRIC_TEST_FIXTURE(ShaderCompilerTestFixture, HLSLcc_MatrixArrays, (ShaderCompilerPlatform platform), OpenGLCore_Metal_Switch)
    {
        const char * s =
            "float4x4 mtxArray[8];\n"
            "float4 vert() : POSITION {\n"
            "  return mtxArray[7][2];\n"
            "}\n"
            "float4 frag() : COLOR0 { return 0; }";

        TestCompilationOk(s, platform);
    }

    PARAMETRIC_TEST_FIXTURE(ShaderCompilerTestFixture, HLSLcc_MatricesInStruct, (ShaderCompilerPlatform platform), OpenGLCore_Metal_Switch)
    {
        const char * s =
            "   struct testStruct {\n"
            "    float foo;\n"
            "\t float4x4 mate; \n"
            "    float2 bar;\n"
            "\t float4x4 mateArray[3];\n"
            "\t float blah;\n"
            "   };\n"
            "testStruct s;\n"
            "float4 vert() : POSITION {\n"
            "  return s.mateArray[1][3] * s.foo + s.mate[2] * s.blah + s.bar.xxyy;\n"
            "}\n"
            "float4 frag() : COLOR0 { return 0; }";

        TestCompilationOk(s, platform);
    }

    PARAMETRIC_TEST_FIXTURE(ShaderCompilerTestFixture, HLSLcc_ArrayOfStructs, (ShaderCompilerPlatform platform), OpenGLCore_Metal_Switch)
    {
        const char * s =
            "   struct testStruct {\n"
            "    float foo;\n"
            "\t float4x4 mate; \n"
            "    float2 bar;\n"
            "\t float4x4 mateArray[3];\n"
            "\t float blah;\n"
            "   };\n"
            "testStruct s[3];\n"
            "   struct smallStruct {\n"
            "    float a;\n"
            "    float b;\n"
            "    float c;\n"
            "    float d;\n"
            "   };\n"
            "smallStruct s2[3];\n"
            "struct appdata { float2 uv : TEXCOORD0; uint instanceID : SV_InstanceID; };\n"
            "float4 vert(appdata v) : POSITION {\n"
            "  return s2[v.uv.x * 3].b * s[v.uv.x * 3].mateArray[1][3] * s[v.instanceID].foo + s[2].mate[2] * s[0].blah + s[1].bar.xxyy;\n"
            "}\n"
            "float4 frag() : COLOR0 { return 0; }";

        TestCompilationOk(s, platform);
    }

    TEST_FIXTURE(ShaderCompilerTestFixture, HLSLcc_VulkanSubpassInput)
    {
        const char * s =
            "#include \"UnityCG.cginc\"\n"
            "UNITY_DECLARE_FRAMEBUFFER_INPUT_FLOAT(0);\n"
            "float4 uvws;\n"
            "UNITY_DECLARE_FRAMEBUFFER_INPUT_UINT_MS(1);\n"
            "float4 vert() : POSITION {\n"
            "  return float4(0.0, 1.0, 0.0, 1.0);\n"
            "}\n"
            "float4 frag(float4 a : SV_Position) : COLOR0 { return UNITY_READ_FRAMEBUFFER_INPUT(0, a) + (float)UNITY_READ_FRAMEBUFFER_INPUT_MS(1, (uint)a.x, a); }";

        inputData.requirements = kShaderRequireShaderModel45_ES31;
        TestCompilationOk(s, kShaderCompPlatformVulkan);
    }

    TEST_FIXTURE(ShaderCompilerTestFixture, HLSLcc_MetalSubpassInput)
    {
        const char * s =
            "#include \"UnityCG.cginc\"\n"
            "UNITY_DECLARE_FRAMEBUFFER_INPUT_HALF(0);\n"
            "float4 uvws;\n"
            "UNITY_DECLARE_FRAMEBUFFER_INPUT_UINT_MS(1);\n"
            "float4 vert() : POSITION {\n"
            "  return float4(0.0, 1.0, 0.0, 1.0);\n"
            "}\n"
            "half4 frag(float4 a : SV_Position, uint sid : SV_SampleIndex) : SV_Target { return UNITY_READ_FRAMEBUFFER_INPUT(0, a) + UNITY_READ_FRAMEBUFFER_INPUT_MS(1, sid, a); }";

        inputData.requirements = kShaderRequireShaderModel45_ES31;

        std::vector<ShaderCompileArg> args;
        args.push_back(ShaderCompileArg("SHADER_API_MOBILE", "1"));
        args.push_back(ShaderCompileArg("UNITY_FRAMEBUFFER_FETCH_AVAILABLE", "1"));
        TestCompilationOkWithCompileArgs(s, kShaderCompPlatformMetal, &args[0], args.size());
    }

    PARAMETRIC_TEST_FIXTURE(ShaderCompilerTestFixture, Glslang_CubeArrays, (ShaderCompilerPlatform platform), GLES3Plus_OpenGLCore_Vulkan_Switch)
    {
        // No Metal support yet
        const char * s =
            "TextureCubeArray myTex; SamplerState sampler_myTex;\n"
            "float4 uvws;\n"
            "float4 vert() : POSITION {\n"
            "  return float4(0.0, 1.0, 0.0, 1.0);\n"
            "}\n"
            "float4 frag() : COLOR0 { return myTex.Sample(sampler_myTex, uvws); }";

        inputData.requirements = kShaderRequireShaderModel50;
        TestCompilationOk(s, platform);
    }

    TEST_FIXTURE(ShaderCompilerTestFixture, Glslang_TextureArrayShadowLod)
    {
        const char * s =
            "Texture2DArray myTex; SamplerComparisonState sampler_myTex;\n"
            "float4 uvws;\n"
            "float4 vert() : POSITION {\n"
            "  return float4(0.0, 1.0, 0.0, 1.0);\n"
            "}\n"
            "float4 frag() : COLOR0 { return myTex.SampleCmpLevelZero(sampler_myTex, uvws.xyz, uvws.w); }";

        inputData.requirements = kShaderRequireShaderModel50;
        TestCompilationOk(s, kShaderCompPlatformOpenGLCore);
        TestCompilationOk(s, kShaderCompPlatformGLES3Plus);
        //TestCompilationOk(s, kShaderCompPlatformMetal); // not yet
        TestCompilationOk(s, kShaderCompPlatformVulkan);
    }

    PARAMETRIC_TEST_FIXTURE(ShaderCompilerTestFixture, HLSLcc_ImageBuffers, (ShaderCompilerPlatform platform), GLES3Plus_OpenGLCore_Vulkan) // todo enable GLES3Plus_OpenGLCore_Vulkan_Switch and fix on Switch? (@wilfrid)
    {
        // Re-enable Metal after 1.2 upgrade
        const char * s =
            "RWBuffer<uint> argBuffer;\n"
            "RWBuffer<float> argBuffer2;\n"
            "Buffer<float> readBuffer;\n"
            "Buffer<int> readBuffer2;\n"
            "Buffer<float> roBuf;\n"
            "float4 vert() : POSITION {\n"
            "argBuffer[0] *= 6;\n"
            "argBuffer[1] = 1;\n"
            "argBuffer2[argBuffer[0]] *= 2;\n"
            "argBuffer[3] = 0;\n"
            "uint foo = readBuffer2[0];\n"
            "InterlockedAdd(argBuffer[4], 1, foo);\n"
            "float bar = readBuffer[0];\n"
            "  return float4(0.0, roBuf[argBuffer[3]], 0.0, 1.0);\n"
            "}\n"
            "float4 frag() : COLOR0 { return float4(0.0, 1.0, 0.0, 1.0); }";

        inputData.requirements = kShaderRequireShaderModel50;
        TestCompilationOk(s, platform);
    }


    PARAMETRIC_TEST_FIXTURE(ShaderCompilerTestFixture, HLSLcc_FragCoord, (ShaderCompilerPlatform platform), OpenGLCore_Metal_Switch)
    {
        const char * s =
            "float4 loc;\n"
            "float4 vert() : POSITION {\n"
            "  return loc;\n"
            "}\n"
            "float4 frag(float4 frgCoord : SV_Position) : COLOR0 { return frgCoord; }";

        TestCompilationOk(s, platform);
    }

    TEST_FIXTURE(ShaderCompilerTestFixture, HLSLcc_MetalTex2DMS)
    {
        const char * s =
            "float4 loc;\n"
            "Texture2DMS<float> _MainTex;\n"
            "float4 vert() : POSITION {\n"
            "  return loc;\n"
            "}\n"
            "float4 frag(float4 frgCoord : SV_Position) : COLOR0 {\n"
            "\tuint3 info; _MainTex.GetDimensions(info.x, info.y, info.z);\n"
            "  return _MainTex.Load(uint2(frgCoord.xy), info.z-1);\n"
            "}";

        inputData.requirements = kShaderRequireShaderModel45_ES31;
        TestCompilationOk(s, kShaderCompPlatformOpenGLCore);
        TestCompilationOk(s, kShaderCompPlatformMetal);
    }

    PARAMETRIC_TEST_FIXTURE(ShaderCompilerTestFixture, HLSLcc_OutputRedirect, (ShaderCompilerPlatform platform), OpenGLCore_Metal_Switch)
    {
        const char * s =
            "#include \"UnityCG.cginc\"\n"
            "   struct appdata_t {\n"
            "    float4 vertex : POSITION;\n"
            "    fixed4 color : COLOR;\n"
            "    float2 texcoord : TEXCOORD0;\n"
            "   };\n"
            "\n"
            "   struct v2f {\n"
            "    float4 vertex : SV_POSITION;\n"
            "    fixed alpha : COLOR;\n"
            "    half2 texcoord : TEXCOORD0;\n"
            "   };\n"
            "\n"
            "   sampler2D _MainTex;\n"
            "   float4 _MainTex_ST;\n"
            "   \n"
            "   v2f vert (appdata_t v)\n"
            "   {\n"
            "    v2f o;\n"
            "    o.vertex = mul(UNITY_MATRIX_MVP, v.vertex);\n"
            "    o.texcoord = TRANSFORM_TEX(v.texcoord, _MainTex);\n"
            "    o.alpha = v.color.a;\n"
            "    return o;\n"
            "   }\n"
            "   \n"
            "   fixed4 frag (v2f i) : COLOR\n"
            "   {\n"
            "      return lerp(fixed4(1,1,1,1), tex2D(_MainTex, i.texcoord), i.alpha);\n"
            "   }\n";

        TestCompilationOk(s, platform);
    }

    TEST_FIXTURE(ShaderCompilerTestFixture, HLSLcc_StripUnusedGlobals)
    {
        const char * s =
            "float4 usedVec4;\n"
            "float4 unusedVec4;\n"
            "float2 usedVec2;\n"
            "float unusedFloat;\n"
            "float usedFloat;\n"
            "sampler3D myUsedTex;\n"
            "sampler2D my_unusedTex;\n"
            "struct MyStruct{ float cannotStrip1; float2 used2; };\n"
            "uniform MyStruct myStruct;\n"
            "cbuffer MyCB { float4 cannotStrip2; float2 used3; };\n"
            "float4 vert() : POSITION {\n"
            "  return float4(usedVec4.y, usedVec2.x, usedFloat, myStruct.used2.x + used3.y);\n"
            "}\n"
            "float4 frag() : COLOR0 { return tex3D(myUsedTex, float3(0.5, 0.5, 0.5)); }";

        TestCompilationOk(s, kShaderCompPlatformOpenGLCore);
        core::string outputStr = GetOutputString();

        // Test that we do not get unused variables
        CHECK(strstr(outputStr.c_str(), "unused") == 0);
        // Test that cannotStrip1 and 2 get through
        CHECK(strstr(outputStr.c_str(), "cannotStrip1") != 0);
        CHECK(strstr(outputStr.c_str(), "cannotStrip2") != 0);


        TestCompilationOk(s, kShaderCompPlatformMetal);
        outputStr = GetMetalVertexShaderOutputString();
        CHECK(strstr(outputStr.c_str(), "unused") == 0);
        // Test that cannotStrip1 and 2 get through
        CHECK(strstr(outputStr.c_str(), "cannotStrip1") != 0);
        CHECK(strstr(outputStr.c_str(), "cannotStrip2") != 0);

        TestCompilationOk(s, kShaderCompPlatformVulkan);
        inputData.programType = kProgramFragment;
        TestCompilationOk(s, kShaderCompPlatformVulkan);
    }

    PARAMETRIC_TEST_FIXTURE(ShaderCompilerTestFixture, HLSLcc_Compare, (ShaderCompilerPlatform platform), OpenGLCore_Metal_Switch)
    {
        const char * s =
            "\t\t\t#include \"UnityCG.cginc\"\n"
            "\t\t\t#include \"UnityShaderVariables.cginc\"\n"
            "\t\t\t\n"
            "\t\t\tstruct v2f_surf\n"
            "\t\t\t{\n"
            "\t\t\t\tfloat4 pos : SV_POSITION;\n"
            "\t\t\t\tfloat2 uv : TEXCOORD0;\n"
            "\t\t\t};\n"
            "\n"
            "\t\t\tsampler2D _MainTex;\n"
            "\t\t\tfloat4 _Decode_HDR;\n"
            "\t\t\tfloat _ConvertToLinearSpace;\n"
            "\t\t\tfloat _StaticUV1;\n"
            "\n"
            "\t\t\tv2f_surf vert (appdata_full v)\n"
            "\t\t\t{\n"
            "\t\t\t\tv2f_surf o;\n"
            "\t\t\t\to.pos = UnityObjectToClipPos(v.vertex);\n"
            "\n"
            "\t\t\t\tif (_StaticUV1)\n"
            "\t\t\t\t\to.uv.xy = v.texcoord1.xy * unity_LightmapST.xy + unity_LightmapST.zw;\n"
            "\t\t\t\telse\n"
            "\t\t\t\t\to.uv.xy = v.texcoord2.xy * unity_DynamicLightmapST.xy + unity_DynamicLightmapST.zw;\n"
            "\n"
            "\t\t\t\treturn o;\n"
            "\t\t\t}\n"
            "\n"
            "\t\t\tfloat4 frag (v2f_surf IN) : COLOR\n"
            "\t\t\t{\n"
            "\t\t\t\tfloat4 mainTexSampled = tex2D (_MainTex, IN.uv.xy);\n"
            "\t\t\t\tfloat3 result;\n"
            "\n"
            "\t\t\t\tif (_Decode_HDR.x > 0)\n"
            "\t\t\t\t\tresult = float4 (DecodeHDR(mainTexSampled, _Decode_HDR), 1);\n"
            "\t\t\t\telse\n"
            "\t\t\t\t\tresult = mainTexSampled.rgb;\n"
            "\n"
            "\t\t\t\tif (_ConvertToLinearSpace)\n"
            "\t\t\t\t\tresult = LinearToGammaSpace (result);\n"
            "\n"
            "\t\t\t\treturn float4 (result, 1);\n"
            "\t\t\t}\n";

        TestCompilationOk(s, platform);
    }

    // With the old use-define chain analysis this shader would have taken until the heat death of the universe to compile.
    PARAMETRIC_TEST_FIXTURE(ShaderCompilerTestFixture, HLSLcc_LotsOfBranches, (ShaderCompilerPlatform platform), OpenGLCore_Metal_Switch)
    {
        const char * s =
            "float4 i;\n"
            "sampler2D _MainTex;\n"
            "float4 frag() : COLOR0 {\n"
            "float4 t = i;\n"
            "[unroll] for(int k = 0; k < 100; k++)\n"
            "{\n"
            "\tfloat4 s = float4(0.0, 0.0, 0.0, 0.0);\n"
            "\t[branch] if(t.x > 0.0)\n"
            "\t\ts = tex2D(_MainTex, t.xy);"
            "\tif(s.x > 0.0)\n"
            "\t\tt = t * i;\n"
            "}\n"
            "  return t;\n"
            "}\n"
            "float4 vert() : POSITION { return i; }\n";

        this->inputData.requirements = kShaderRequireShaderModel50;
        TestCompilationOk(s, platform);
    }


    PARAMETRIC_TEST_FIXTURE(ShaderCompilerTestFixture, HLSLcc_GeometryShader, (ShaderCompilerPlatform platform), GLES3Plus_OpenGLCore_Switch)
    {
        const char * s =
            "StructuredBuffer<float2> pointBuffer;\n"
            "struct vs_out {\n"
            "\tfloat4 pos : SV_POSITION;\n"
            "};\n"
            "vs_out vert (uint id : SV_VertexID) {\n"
            "vs_out o;\n"
            "o.pos = float4(pointBuffer[id] * 2.0 - 1.0, 0, 1);\n"
            "return o;\n"
            "}\n"
            "struct gs_out {\n"
            "\tfloat4 pos : SV_POSITION;\n"
            "};\n"
            "[maxvertexcount(4)]\n"
            "void geom(point vs_out input[1], inout TriangleStream<gs_out> outStream)\n"
            "{\n"
            "\tconst float ks = 0.1;\n"
            "\tgs_out output;\n"
            "\toutput.pos = input[0].pos + float4(-ks, ks, 0, 0); outStream.Append(output);\n"
            "\toutput.pos = input[0].pos + float4(ks, ks, 0, 0); outStream.Append(output);\n"
            "\toutput.pos = input[0].pos + float4(-ks, -ks, 0, 0); outStream.Append(output);\n"
            "\toutput.pos = input[0].pos + float4(ks, -ks, 0, 0); outStream.Append(output);\n"
            "\toutStream.RestartStrip();\n"
            "}\n"
            "float4 frag() : COLOR0 { return 0; }";
        this->inputData.p.entryName[kProgramGeometry] = "geom";
        this->inputData.requirements = kShaderRequireShaderModel50;

        TestCompilationOk(s, platform);

        core::string outputStr;
        // Test that we do not get stream declarations
        AssignOutputString(outputStr);
        CHECK(strstr(outputStr.c_str(), "layout(stream =") == NULL);
    }

    PARAMETRIC_TEST_FIXTURE(ShaderCompilerTestFixture, HLSLcc_UAV, (ShaderCompilerPlatform platform), GLES3Plus_Vulkan_Switch)
    {
        const char * s =
            "#define vert_img vert\n"
            "#include \"UnityCG.cginc\"\n"
            "sampler2D _MainTex;\n"
            "RWTexture2D<float4>     _OutputUAV1;\n"
            "RWStructuredBuffer<int> _OutputUAV2;\n"
            "StructuredBuffer<int> _ReadOnlyBuffer;\n"
            "RWTexture2D<float4>     _OutputUAV3;\n"
            "float4 frag(v2f_img i) : COLOR\n"
            "{\n"
            "\tfloat4 col = tex2D(_MainTex, i.uv);\n"
            "\tint2 texLoc;\n"
            "\ttexLoc.x = int(col.r*255.0);\n"
            "\ttexLoc.y = int(col.g*255.0);\n"
            "\t_OutputUAV1[texLoc] = col;\n"
            "\tint bufLoc = Luminance(col).r * 255;\n"
            "\t_OutputUAV2[bufLoc] = 123;\n"
            "\ttexLoc.x = int(col.r*255.0); texLoc.y = 0;\n"
            "\t_OutputUAV3[texLoc] = float4(_ReadOnlyBuffer[texLoc.x], 0, 0, 1);\n"
            "\ttexLoc.x = int(col.g*255.0); texLoc.y = 1;\n"
            "\t_OutputUAV3[texLoc] = float4(0, 1, 0, 1);\n"
            "\ttexLoc.x = int(col.b*255.0); texLoc.y = 2;\n"
            "\t_OutputUAV3[texLoc] = float4(0, 0, 1, 1);\n"
            "\treturn col*1.2;\n"
            "}";

        inputData.requirements = kShaderRequireShaderModel50;
        TestCompilationOk(s, platform);
    }

    PARAMETRIC_TEST_FIXTURE(ShaderCompilerTestFixture, HLSLcc_MOVC, (ShaderCompilerPlatform platform), GLES3Plus_OpenGLCore_Metal)
    {
        const char *s =
            "uniform float4 fromCenter0;\n"
            "uniform float4 fromCenter1;\n"
            "uniform float4 fromCenter2;\n"
            "uniform float4 fromCenter3;\n"
            "uniform float4 unity_ShadowSplitSqRadii;\n"
            "uniform float4 unity_ShadowFadeCenterAndType;\n"
            "uniform float4 wpos;\n"
            "\tfloat4 vert() : POSITION { return float4(0.0, 0.0, 0.0, 1.0f); }\n"

            "float3 UnpackNormalOctQuadEncode(float2 f)\n"
            "{\n"
            "\tfloat3 n = float3(f.x, f.y, 1.0 - abs(f.x) - abs(f.y));\n"
            "\tfloat t = max(-n.z, 0.0);\n"
            "\tn.xy += n.xy >= 0.0 ? -t.xx : t.xx;\n"
            "\treturn normalize(n);\n"
            "}\n"

            "uniform sampler2D _LightTexture0;\n"
            "uniform float4x4 unity_WorldToLight;\n"
            "uniform sampler2D _LightTextureB0;\n"
            "inline float UnitySpotCookie(float4 LightCoord)\n"
            "{\n"
            "return tex2D(_LightTexture0, LightCoord.xy / LightCoord.w + 0.5).w;\n"
            "}\n"
            "inline float UnitySpotAttenuate(float3 LightCoord)\n"
            "{\n"
            "return tex2D(_LightTextureB0, dot(LightCoord, LightCoord).xx).a;\n"
            "}\n"

            "float4 frag() : SV_Target0\n"
            "{\n"
            "\tfloat4 distances2 = float4(dot(fromCenter0, fromCenter0), dot(fromCenter1, fromCenter1), dot(fromCenter2, fromCenter2), dot(fromCenter3, fromCenter3));\n"
            "\tfloat3 mockup = UnpackNormalOctQuadEncode(wpos.xy * 2.0 - 1.0);\n"
            "\tfloat4 weights = float4(distances2 < unity_ShadowSplitSqRadii);\n"
            "\tweights.yzw = saturate(weights.yzw - weights.xyz);\n"
            "\tfloat sphereDist = distance(wpos.xyz, unity_ShadowFadeCenterAndType.xyz);\n"
            "\tfloat4 lightCoord = mul(unity_WorldToLight, float4(wpos.xyz, 1));\n"
            "\tfloat destName = (lightCoord.z > 0) * UnitySpotCookie(lightCoord) * UnitySpotAttenuate(lightCoord.xyz) * fromCenter0.x;\n"
            "\tif (mockup.x == mockup.y && mockup.z == 1.0f)\n"
            "\t\treturn float4(mockup.xyz, sphereDist+destName);\n"
            "\telse\n"
            "\t\treturn float4(weights.yzw, sphereDist+destName);\n"
            "}\n"
        ;

        TestCompilationOk(s, platform);

        if (platform == kShaderCompPlatformMetal)
        {
            core::string outputString = GetMetalFragmentShaderOutputString();
            CHECK_NOT_EQUAL(core::string::npos, outputString.find("hlslcc_movcTemp.x = (u_xlatb2.x) ? (-u_xlat6.x) : u_xlat6.x;"));
        }
        else if (platform == kShaderCompPlatformOpenGLCore)
        {
            core::string outputString = GetOutputString();
            CHECK_NOT_EQUAL(core::string::npos, outputString.find("hlslcc_movcTemp.x = (u_xlatb2.x) ? (-u_xlat8.x) : u_xlat8.x;"));
        }
    }

    PARAMETRIC_TEST_FIXTURE(ShaderCompilerTestFixture, HLSLcc_MOVC_WithDifferentTypes, (ShaderCompilerPlatform platform), GLES3Plus_OpenGLCore_Metal)
    {
        const char *s =
            "#include \"UnityCG.cginc\"\n"
            "uniform sampler2D _LeftTex;\n"
            "uniform sampler2D _RightTex;\n"
            "struct v2f {\n"
            "\tfloat4 pos : POSITION;\n"
            "\tfloat2 uv : TEXCOORD0;\n"
            "};\n"
            "v2f vert( appdata_img v )\n"
            "{\n"
            "\tv2f o;\n"
            "\to.pos = UnityObjectToClipPos (v.vertex);\n"
            "\tfloat2 uv = MultiplyUV( UNITY_MATRIX_TEXTURE0, v.texcoord );\n"
            "\to.uv = uv;\n"
            "\treturn o;\n"
            "}\n"
            "half4 frag (v2f i) : COLOR\n"
            "{\n"
            "\tfloat r, g, b, ratio;\n"
            "\tfloat4 texR = tex2D(_LeftTex, i.uv);\n"
            "\tfloat4 texGB = tex2D(_RightTex, i.uv);\n"
            "\tfloat4 texRGB;\n"
            "\tr=texR.r;\n"
            "\tg=texGB.g;\n"
            "\tb=texGB.b;\n"
            "\tfloat lumR = texR.r * 0.299f;\n"
            "\tfloat lumGB = texR.g * 0.587f + texR.b * 0.114f;\n"
            "\tratio = 1.0;\n"
            "\tif (lumR > 0.001) ratio = lumGB / lumR;\n"
            "\tfloat d = texR.r * ratio * 0.6f;\n"
            "\tif (d > texR.r) {\n"
            "\t\tr = d;\n"
            "\t\tif (r > 0xff) r = 0xff;\n"
            "\t}\n"
            "\tlumR = texGB.r * 0.299f;\n"
            "\tlumGB = texGB.g * 0.587f + texGB.b * 0.114f;\n"
            "\tratio = 1.0;\n"
            "\tif (lumGB > 0.001) ratio = lumR / lumGB;\n"
            "\td = texGB.g * ratio * 0.4f;\n"
            "\tif (d > texGB.g) {\n"
            "\t\tg = d;\n"
            "\t\tif (g > 0xff) g = 0xff;\n"
            "\t}\n"
            "\td = texGB.b * ratio * 0.8f;\n"
            "\tif (d > texGB.b) {\n"
            "\t\tb = d;\n"
            "\t\tif (b > 0xff) b = 0xff;\n"
            "\t}\n"
            "\ttexRGB = float4(texR.r,g,b,1);\n"
            "\treturn texRGB;\n"
            "}\n";

        TestCompilationOk(s, platform);

        core::string outputString;
        if (platform == kShaderCompPlatformMetal)
            outputString = GetMetalFragmentShaderOutputString();
        else
            outputString = GetOutputString();

        CHECK_EQUAL(core::string::npos, outputString.find("hlslcc_movcTemp"));
    }

    PARAMETRIC_TEST_FIXTURE(ShaderCompilerTestFixture, HLSLcc_MRT, (ShaderCompilerPlatform platform), OpenGLCore_Metal_Switch)
    {
        const char *s =
            "\tfloat4 vert() : POSITION { return float4(0.0, 0.0, 0.0, 1.0f); }\n"
            "\tvoid frag(out half4 col0 : COLOR0, out half4 col1 : COLOR1, out half4 col2 : COLOR2, out half4 col3 : COLOR3)\n"
            "\t{\n"
            "\t\tcol0 = half4(1, 1,0, 1);\n"
            "\t\tcol1 = half4(0, 0, 1, 1);\n"
            "\t\tcol2 = half4(0, 1, 1, 1);\n"
            "\t\tcol3 = half4(0, 0, 0 , 1);\n"
            "\t}\n";

        if (platform == kShaderCompPlatformMetal)
        {
            this->inputData.programType = kProgramFragment;
        }
        TestCompilationOk(s, platform);
    }

    TEST_FIXTURE(ShaderCompilerTestFixture, HLSLcc_MRT_UAV)
    {
        const char *s =
            "\tfloat4 vert() : POSITION { return float4(0.0, 0.0, 0.0, 1.0f); }\n"
            "\tRWTexture2D<float4> _OutputTex;\n"
            "\tRWStructuredBuffer<float4> bufColors;\n"
            "\tvoid frag(out half4 col0 : COLOR0, out half4 col1 : COLOR1)\n"
            "\t{\n"
            "\t\t_OutputTex[int2(0,0)] = float4(1, 0, 0, 1);\n"
            "\t\tbufColors[0] = float4(0, 1, 0, 1);\n"
            "\t\tcol0 = half4(1, 1,0, 1);\n"
            "\t\tcol1 = half4(0, 0, 1, 1);\n"
            "\t}\n";

        inputData.requirements = kShaderRequireShaderModel50;
        TestCompilationOk(s, kShaderCompPlatformOpenGLCore);
    }

    PARAMETRIC_TEST_FIXTURE(ShaderCompilerTestFixture, HLSLcc_Clip, (ShaderCompilerPlatform platform), OpenGLCore_Metal_Switch)
    {
        const char *s =
            "\tfloat4 vert() : POSITION { return float4(0.0, 0.0, 0.0, 1.0f); }\n"
            "   uniform float cutoff;\n"
            "   uniform float o_alpha;\n"
            "\tvoid frag(out half4 col0 : COLOR0, out half4 col1 : COLOR1, out half4 col2 : COLOR2, out half4 col3 : COLOR3)\n"
            "\t{\n"
            "       clip(o_alpha - cutoff);\n"
            "\t\tcol0 = half4(1, 1,0, 1);\n"
            "\t\tcol1 = half4(0, 0, 1, 1);\n"
            "\t\tcol2 = half4(0, 1, 1, 1);\n"
            "\t\tcol3 = half4(0, 0, 0 , 1);\n"
            "\t}\n";

        TestCompilationOk(s, platform);
    }

    TEST_FIXTURE(ShaderCompilerTestFixture, HLSLcc_PositionPrecisionHighp)
    {
        const char *s =
            "   half4 vert(float4 vertex : POSITION) : SV_POSITION { return half4(vertex * vertex); }\n"
            "   float4 frag() : SV_Target { return float4(0, 0, 0, 0); }\n";

        TestCompilationOk(s, kShaderCompPlatformMetal);

        // copy to a string so that we can search.
        core::string outputString;
        AssignOutputString(outputString);

        // Check if it doesn't make a type conversion, that could happend if position was assumed as half4
        CHECK_EQUAL(core::string::npos, outputString.find("half4("));
        // Check if position is forced to float4
        CHECK_NOT_EQUAL(core::string::npos, outputString.find("float4 mtl_Position"));
    }

    PARAMETRIC_TEST_FIXTURE(ShaderCompilerTestFixture, HLSLcc_ExplicitDepthTexPrecision, (ShaderCompilerPlatform platform), GLES3Plus_Metal_Switch)
    {
        const char *s =
            "\tfloat4 vert() : POSITION { return float4(0.0, 0.0, 0.0, 1.0f); }\n"
            "   uniform float2 uv;\n"
            "\tsampler2D_float _CameraDepthTexture;\n"
            "\thalf4 frag() : SV_Target\n"
            "\t{\n"
            "\t\thalf depth = tex2D(_CameraDepthTexture, uv).r;\n"
            "\t\treturn half4(depth, depth, depth, depth);\n"
            "\t}\n";

        TestCompilationOk(s, platform);

        if (platform == kShaderCompPlatformGLES3Plus)
        {
            // copy to a string so that we can search.
            core::string outputString;
            AssignOutputString(outputString);

            // Check that it actually ends up as highp sampler (and that the intermediate _highp is removed from the name)
            CHECK(outputString.find("uniform highp sampler2D _CameraDepthTexture;") != core::string::npos);
        }
    }

    TEST_FIXTURE(ShaderCompilerTestFixture, HLSLcc_DefaultDepthTexPrecision)
    {
        const char *s =
            "\tfloat4 vert() : POSITION { return float4(0.0, 0.0, 0.0, 1.0f); }\n"
            "   uniform float2 uv;\n"
            "\tsampler2D _CameraDepthTexture;\n"
            "\tsampler2D _LastCameraDepthTexture;\n"
            "\thalf4 frag() : SV_Target\n"
            "\t{\n"
            "\t\thalf depth = tex2D(_CameraDepthTexture, uv).r;\n"
            "\t\thalf depth2 = tex2D(_LastCameraDepthTexture, uv).r;\n"
            "\t\treturn half4(depth, depth2, depth, depth2);\n"
            "\t}\n";

        TestCompilationOk(s, kShaderCompPlatformGLES3Plus);
        core::string outputString;
        AssignOutputString(outputString);
        CHECK(outputString.find("uniform highp sampler2D _CameraDepthTexture;") != core::string::npos);
        CHECK(outputString.find("uniform highp sampler2D _LastCameraDepthTexture;") != core::string::npos);

        TestCompilationOk(s, kShaderCompPlatformMetal);
        outputString = GetMetalFragmentShaderOutputString();
        CHECK(outputString.find("texture2d<float, access::sample > _CameraDepthTexture") != core::string::npos);
        CHECK(outputString.find("texture2d<float, access::sample > _LastCameraDepthTexture") != core::string::npos);
    }

    TEST_FIXTURE(ShaderCompilerTestFixture, HLSLcc_DefaultMetalTexPrecision)
    {
        const char *s =
            "float4 vert() : POSITION { return float4(0.0, 0.0, 0.0, 1.0f); }\n"
            "uniform float2 uv;\n"
            "sampler2D _MainTex;\n"
            "half4 frag() : SV_Target { return tex2D(_MainTex, uv); }\n";

        TestCompilationOk(s, kShaderCompPlatformGLES3Plus);

        ShaderCompileArg arg = ShaderCompileArg("SHADER_API_MOBILE", "0");
        TestCompilationOkWithCompileArg(s, kShaderCompPlatformMetal, &arg);
        core::string outputString = GetMetalFragmentShaderOutputString();
        CHECK_NOT_EQUAL(core::string::npos, outputString.find("texture2d<float, access::sample > _MainTex"));

        arg = ShaderCompileArg("SHADER_API_MOBILE", "1");
        TestCompilationOkWithCompileArg(s, kShaderCompPlatformMetal, &arg);

        outputString = GetMetalFragmentShaderOutputString();
        CHECK_NOT_EQUAL(core::string::npos, outputString.find("texture2d<half, access::sample > _MainTex"));
    }

    TEST_FIXTURE(ShaderCompilerTestFixture, HLSLcc_Sampler3DPrecision)
    {
        const char *s =
            "\tfloat4 vert() : POSITION { return float4(0.0, 0.0, 0.0, 1.0f); }\n"
            "\tuniform float3 uvw;\n"
            "\tsampler3D_float unity_ProbeVolumeSH;\n"
            "\tfloat4 frag() : SV_Target\n"
            "\t{\n"
            "\t\tfloat3 color = tex3D(unity_ProbeVolumeSH, uvw);\n"
            "\t\treturn float4(color, 1);\n"
            "\t}\n";

        TestCompilationOk(s, kShaderCompPlatformGLES3Plus);
        core::string outputString;
        AssignOutputString(outputString);
        // check if highp sampler is used
        CHECK(outputString.find("uniform highp sampler3D unity_ProbeVolumeSH;") != core::string::npos);

        TestCompilationOk(s, kShaderCompPlatformMetal);
        outputString = GetMetalFragmentShaderOutputString();
        // check if float sampler is used
        CHECK(outputString.find("texture3d<float, access::sample > unity_ProbeVolumeSH [[ texture(0) ]]") != core::string::npos);
    }

    PARAMETRIC_TEST_FIXTURE(ShaderCompilerTestFixture, HLSLcc_CBFormat, (ShaderCompilerPlatform platform), GLES3Plus_Metal_Switch)
    {
        const char *s =
            "\tfloat4 vert() : POSITION { return float4(0.0, 0.0, 0.0, 1.0f); }\n"
            "   uniform float2 uv;\n"
            "   uniform half _Metallic0;\n"
            "   uniform half _Metallic1;\n"
            "   uniform half _Metallic2;\n" // Start of second vec4
            "   uniform half _Metallic3;\n"
            "   uniform half _Metallic4;\n"
            "   uniform half _Metallic5;\n"
            "   uniform half _Metallic6;\n"// Start of third vec4
            "   uniform half _Metallic7;\n"
            "   uniform float2 _uv2;\n"
            "   uniform float3 _spec;\n"
            "   uniform float _smooth;\n"
            "   sampler2D _MainTex;\n"
            "\thalf4 frag() : SV_Target\n"
            "\t{\n"
            "\t\thalf4 col = tex2D(_MainTex, uv);\n"
            "\t\thalf4 a = dot(col, half4(_Metallic0, _Metallic1, _Metallic2, _Metallic3));\n"
            "\t\thalf4 b = dot(col, half4(_Metallic2, _Metallic3, _Metallic4, _Metallic5));\n"
            "\t\tb += dot(col, half4(uv.x, uv.y, _Metallic0, _Metallic1));\n"
            "\t\ta += dot(col, half4(_Metallic6, _Metallic7, _uv2.x, _uv2.y));\n"
            "\t\tb.xyz += dot(col.xyz, half3(_Metallic7, _uv2.x, _uv2.y));\n"
            "       a.xz += half2(_spec.x, _smooth);\n"
            "\t\treturn a+b;\n"
            "\t}\n";

        TestCompilationOk(s, platform);
    }

    PARAMETRIC_TEST_FIXTURE(ShaderCompilerTestFixture, HLSLcc_SamplerSwizzle, (ShaderCompilerPlatform platform), OpenGLCore_Metal_Switch)
    {
        const char *s =

            "\tfloat4 vert() : POSITION { return float4(0.0, 0.0, 0.0, 1.0f); }\n"

            "uniform float4 _MainTex_ST;\n"
            "uniform float4 _Color;\n"
            "uniform float4x4 unity_GUIClipTextureMatrix;\n"

            "sampler2D _MainTex;\n"
            "sampler2D _GUIClipTexture;\n"
            "uniform float2 texcoord;\n"
            "uniform float2 clipUV;\n"

            "half4 frag() : SV_Target\n"
            "{\n"
            "\thalf4 col = _Color;\n"
            "\tcol.a *= tex2D(_MainTex, texcoord).a * tex2D(_GUIClipTexture, clipUV).a;\n"
            "\treturn col;\n"
            "}\n";

        TestCompilationOk(s, platform);
    }

    PARAMETRIC_TEST_FIXTURE(ShaderCompilerTestFixture, HLSLcc_Assign, (ShaderCompilerPlatform platform), OpenGLCore_Metal_Switch)
    {
        const char *s =

            "\tfloat4 vert() : POSITION { return float4(0.0, 0.0, 0.0, 1.0f); }\n"

            "uniform float _Energy;\n"

            "half4 frag() : SV_Target\n"
            "{\n"
            "\tfloat e = _Energy;\n"
            "\treturn half4(e,e,e,0.0);\n"
            "}\n";

        TestCompilationOk(s, platform);
    }

    // Domain shader treated all patch constant and control point inputs as floats. Verify that we can pass through instance ID without float casts.
    TEST_FIXTURE(ShaderCompilerTestFixture, HLSLcc_DSPassThrough)
    {
        const char *s =
            "\tfloat4 vert() : POSITION { return float4(0.0, 0.0, 0.0, 1.0f); }\n"
            "\tfloat4 frag() : SV_Target { return float4(0.0, 0.0, 0.0, 1.0f); }\n"
            "struct DS_OUTPUT\n"
            "{\n"
            "    float4 vPosition        : SV_POSITION;\n"
            "    float3 vWorldPos        : WORLDPOS;\n"
            "    float3 vNormal            : NORMAL;\n"
            "    uint instID : INSTANCEID;\n"
            "};\n"
            "struct HS_CONSTANT_DATA_OUTPUT\n"
            "{\n"
            "    float Edges[4]             : SV_TessFactor;\n"
            "    float Inside[2]            : SV_InsideTessFactor;\n"
            "    uint instID : INSTANCEID;\n"
            "};\n"
            "\n"
            "struct HS_OUTPUT\n"
            "{\n"
            "    float3 vPosition           : BEZIERPOS;\n"
            "    uint anotherInstID : INSTID; \n"
            "};\n"
            "\n"
            "[domain(\"quad\")]\n"
            "DS_OUTPUT domain( HS_CONSTANT_DATA_OUTPUT input, \n"
            "                    float2 UV : SV_DomainLocation,\n"
            "                    const OutputPatch<HS_OUTPUT, 4> bezpatch )\n"
            "{\n"
            "\n"
            "    DS_OUTPUT Output;\n"
            "    Output.vPosition = float4(1.0, 0.0, 0.0, 0.0);\n"
            "    Output.vWorldPos = float3(1.0, 0.0, 0.0);\n"
            "    Output.vNormal = float3(1.0, 0.0, 0.0);\n"
            "    Output.instID = input.instID + bezpatch[0].anotherInstID;\n"
            "\n"
            "    return Output;    \n"
            "}\n";
        inputData.requirements = kShaderRequireShaderModel50;
        inputData.p.entryName[kProgramVertex] = "vert";
        inputData.p.entryName[kProgramFragment] = "frag";
        // inputData.p.entryName[kProgramGeometry] = "geom";
        inputData.p.entryName[kProgramHull] = "";
        inputData.p.entryName[kProgramDomain] = "domain";
        TestCompilationOk(s, kShaderCompPlatformOpenGLCore);

        // We should not be getting bitcasts in this shader.
        core::string outputString = GetOutputString();
        CHECK(outputString.find("floatBitsTo") == core::string::npos);
    }

    PARAMETRIC_TEST_FIXTURE(ShaderCompilerTestFixture, HLSLcc_SinCos, (ShaderCompilerPlatform platform), OpenGLCore_Metal_Switch)
    {
        const char *s =

            "float4 vert() : POSITION { return float4(0.0, 0.0, 0.0, 1.0f); }\n"

            "sampler2D _MainTex;\n"
            "uniform float2 uvOrig;\n"
            "uniform float4 _CenterRadius;\n"
            "uniform float _Angle;\n"

            "float4 frag() : SV_Target\n"
            "{\n"
            "\tfloat2 offset = uvOrig;\n"
            "\tfloat angle = 1.0 - length(offset / _CenterRadius.zw);\n"
            "\tangle = max(0, angle);\n"
            "\tangle = angle * angle * _Angle;\n"
            "\tfloat cosLength, sinLength;\n"
            "\tsincos(angle, sinLength, cosLength);\n"

            "\tfloat2 uv;\n"
            "\tuv.x = cosLength * offset[0] - sinLength * offset[1];\n"
            "\tuv.y = sinLength * offset[0] + cosLength * offset[1];\n"
            "\tuv += _CenterRadius.xy;\n"

            "\treturn tex2D(_MainTex, uv);\n"
            "}\n";

        TestCompilationOk(s, platform);
        if (platform == kShaderCompPlatformMetal)
        {
            inputData.compileFlags |= kShaderCompFlags_DisableFastMath;
            TestCompilationOk(s, platform);

            core::string outputString = GetOutputString();
            CHECK(outputString.find("UNITY_DISABLE_FASTMATH") != core::string::npos);
        }
    }

    PARAMETRIC_TEST_FIXTURE(ShaderCompilerTestFixture, HLSLcc_Tex2DArray, (ShaderCompilerPlatform platform), OpenGLCore_Metal_Switch)
    {
        const char *s =

            "float4 vert() : POSITION { return float4(0.0, 0.0, 0.0, 1.0f); }\n"

            "#include \"UnityCG.cginc\"\n"

            "struct v2f {\n"
            "\tfloat3 uv : TEXCOORD0;\n"
            "\tfloat4 vertex : SV_POSITION;\n"
            "};\n"

            "UNITY_DECLARE_TEX2DARRAY(_MyArr);\n"

            "fixed4 frag (v2f i) : SV_Target {\n"
            "\treturn UNITY_SAMPLE_TEX2DARRAY(_MyArr, i.uv);\n"
            "}\n";

        TestCompilationOk(s, platform);
    }

#if 0
    TEST_FIXTURE(ShaderCompilerTestFixture, HLSLcc_file)
    {
        core::string s = "";
        CHECK(ReadTextFile("/Volumes/Users/aet/Desktop/test.txt", s));
        inputData.requirements = kShaderRequireShaderModel50;
        inputData.p.entryName[kProgramVertex] = "vert";
        inputData.p.entryName[kProgramFragment] = "frag";
        // inputData.p.entryName[kProgramGeometry] = "geom";
        inputData.p.entryName[kProgramHull] = "hull";
        inputData.p.entryName[kProgramDomain] = "domain";

        TestCompilationOk(s.c_str(), kShaderCompPlatformOpenGLCore);
        TestCompilationOk(s.c_str(), kShaderCompPlatformMetal);
    }

#endif

    PARAMETRIC_TEST_FIXTURE(ShaderCompilerTestFixture, HLSLcc_CBAccess, (ShaderCompilerPlatform platform), GLES3Plus_Metal_Switch)
    {
        const char *s =
            "struct u2v {\n"
            "\tfloat4 vertex : POSITION;\n"
            "\thalf2 texcoord : TEXCOORD0;\n"
            "};\n"
            "\n"
            "struct v2f {\n"
            "\tfloat4 pos : SV_POSITION;\n"
            "\thalf2 uv : TEXCOORD0;\n"
            "};\n"
            "\n"
            "sampler2D _MainTex;\n"
            "half4 _OutlineColor, _MainColor;\n"
            "half _OutlineMin, _OutlineMax;\n"
            "half _MainMin, _MainMax;\n"
            "float4x4 UNITY_MATRIX_MVP;\n"
            "\n"
            "v2f vert (u2v v) {\n"
            "\tv2f o;\n"
            "\to.pos = mul(UNITY_MATRIX_MVP, v.vertex);\n"
            "\to.uv = v.texcoord;\n"
            "\treturn o;\n"
            "}\n"
            "\n"
            "half4 frag(v2f i) : SV_TARGET {\n"
            "\thalf4 c;\n"
            "\thalf4 tex = tex2D(_MainTex, i.uv); \n"
            "\thalf value = saturate((tex.a - _MainMin) / (_MainMax - _MainMin));\n"
            "\tc.rgb = lerp(_OutlineColor.rgb, _MainColor.rgb, value);\n"
            "\tc.a = saturate((tex.a - _OutlineMin) / (_OutlineMax - _OutlineMin));\n"
            "\treturn c;\n"
            "}\n";

        TestCompilationOk(s, platform);
    }


    PARAMETRIC_TEST_FIXTURE(ShaderCompilerTestFixture, HLSLcc_ClipDistance, (ShaderCompilerPlatform platform), GLES3Plus_OpenGLCore_Metal_Switch)
    {
        // Simple case, 1 clip plane
        const char *s =
            "#include \"UnityCG.cginc\"\n"
            "struct appdata {\n"
            "\tfloat4 vertex\t\t: POSITION;\n"
            "\tfloat2 uv\t\t\t: TEXCOORD0;\n"
            "};\n"
            "\n"
            "struct v2f {\n"
            "\tfloat4\tvertex\t\t: SV_Position;\n"
            "\tfloat\tclipDistance: SV_ClipDistance;\n"
            "\tfloat2\tuv\t\t\t: TEXCOORD0;\n"
            "};\n"
            "\n"
            "float4\t_Color;\n"
            "float\t_ClipHeight;\n"
            "\n"
            "v2f vert (appdata v) {\n"
            "\tv2f o;\n"
            "\to.vertex = mul(UNITY_MATRIX_MVP, v.vertex);\n"
            "\to.clipDistance = mul(unity_ObjectToWorld, v.vertex).y - _ClipHeight;\n"
            "\to.uv = v.uv;\n"
            "\treturn o;\n"
            "}\n"
            "\t\t\t\n"
            "float4 frag (v2f i) : SV_Target {\n"
            "\treturn _Color;\n"
            "}\n";

        TestCompilationOk(s, platform);
    }

    PARAMETRIC_TEST_FIXTURE(ShaderCompilerTestFixture, HLSLcc_ClipDistance_8Planes, (ShaderCompilerPlatform platform), GLES3Plus_OpenGLCore_Metal_Switch)
    {
        // 8 clip planes
        const char *s =
            "#include \"UnityCG.cginc\"\n"
            "struct appdata {\n"
            "\tfloat4 vertex\t\t: POSITION;\n"
            "\tfloat2 uv\t\t\t: TEXCOORD0;\n"
            "};\n"
            "\n"
            "struct v2f {\n"
            "\tfloat4\tvertex\t\t: SV_Position;\n"
            "\tfloat4\tclipDistance[2]: SV_ClipDistance;\n"
            "\tfloat2\tuv\t\t\t: TEXCOORD0;\n"
            "};\n"
            "\n"
            "float4\t_Color;\n"
            "float\t_ClipHeight;\n"
            "\n"
            "v2f vert (appdata v) {\n"
            "\tv2f o;\n"
            "\to.vertex = mul(UNITY_MATRIX_MVP, v.vertex);\n"
            "\tfloat c = mul(unity_ObjectToWorld, v.vertex).y - _ClipHeight;\n"
            "\to.clipDistance[0] = float4(c, c+1.0, c+2.0, c+3.0);\n"
            "\to.clipDistance[1] = float4(c+4.0, c+5.0, c+6.0, c+7.0);\n"
            "\to.uv = v.uv;\n"
            "\treturn o;\n"
            "}\n"
            "\t\t\t\n"
            "float4 frag (v2f i) : SV_Target {\n"
            "\treturn _Color;\n"
            "}\n";

        TestCompilationOk(s, platform);
    }

    PARAMETRIC_TEST_FIXTURE(ShaderCompilerTestFixture, HLSLcc_ClipDistance_6Planes, (ShaderCompilerPlatform platform), GLES3Plus_OpenGLCore_Metal_Switch)
    {
        // 6 clip planes, divided into 2 vec3's (yes, hlsl is crazy)
        const char *s =
            "#include \"UnityCG.cginc\"\n"
            "struct appdata {\n"
            "\tfloat4 vertex\t\t: POSITION;\n"
            "\tfloat2 uv\t\t\t: TEXCOORD0;\n"
            "};\n"
            "\n"
            "struct v2f {\n"
            "\tfloat4\tvertex\t\t: SV_Position;\n"
            "\tfloat3\tclipDistance[2]: SV_ClipDistance;\n"
            "\tfloat2\tuv\t\t\t: TEXCOORD0;\n"
            "};\n"
            "\n"
            "float4\t_Color;\n"
            "float\t_ClipHeight;\n"
            "\n"
            "v2f vert (appdata v) {\n"
            "\tv2f o;\n"
            "\to.vertex = mul(UNITY_MATRIX_MVP, v.vertex);\n"
            "\tfloat c = mul(unity_ObjectToWorld, v.vertex).y - _ClipHeight;\n"
            "\to.clipDistance[0] = float3(c, c+1.0, c+2.0);\n"
            "\to.clipDistance[1] = float3(c+4.0, c+5.0, c+6.0);\n"
            "\to.uv = v.uv;\n"
            "\treturn o;\n"
            "}\n"
            "\t\t\t\n"
            "float4 frag (v2f i) : SV_Target {\n"
            "\treturn _Color;\n"
            "}\n";

        TestCompilationOk(s, platform);
    }

    PARAMETRIC_TEST_FIXTURE(ShaderCompilerTestFixture, HLSLcc_ForLoop, (ShaderCompilerPlatform platform), OpenGLCore_Metal_Switch)
    {
        const char *s =

            "float4 vert() : POSITION { return float4(0.0, 0.0, 0.0, 1.0f); }\n"

            "sampler2D _MainTex;\n"
            "uniform float2 uvOrig;\n"
            "uniform float4 _CenterRadius;\n"
            "uniform float _Angle;\n"
            "uniform float4 _LightData[4];"

            "float4 frag() : SV_Target\n"
            "{\n"
            "\tfloat2 offset = uvOrig;\n"
            "\tfloat angle = 1.0 - length(offset / _CenterRadius.zw);\n"
            "   for(int i = 0; i < 4; i++) {\n"
            "\tangle = max(0, angle) + _LightData[i].x;\n"
            "\tangle = angle * angle * _Angle;\n"
            "\tfloat cosLength, sinLength;\n"
            "\tsincos(angle, sinLength, cosLength);\n"
            "\toffset += float2(sinLength, cosLength);\n }"
            "\tfloat2 uv;\n"
            "\tuv.x = offset[0] * offset[1];\n"
            "\tuv.y = offset[0] * offset[1];\n"
            "\tuv += _CenterRadius.xy;\n"

            "\treturn tex2D(_MainTex, uv);\n"
            "}\n";

        TestCompilationOk(s, platform);
    }

    TEST_FIXTURE(ShaderCompilerTestFixture, HLSLcc_CorrectNullOp_GLES) // Case 1124159
    {
        const char *s =
            "struct appdata\n"
            "{\n"
            "float4 vertex : POSITION;\n"
            "};\n"

            "struct v2f\n"
            "{\n"
            "float4 vertex : SV_POSITION;\n"
            "};\n"

            "v2f vert (appdata v)\n"
            "{\n"
            "v2f o;\n"
            "o.vertex = v.vertex;\n"
            "return o;\n"
            "}\n"

            "uniform uint input;\n"

            "float4 frag (v2f i) : SV_Target\n"
            "{\n"
            "uint sample_index = input / 3;\n"
            "uint sub_x = sample_index % input;\n"
            "return float4(sub_x, 0.0, 0.0, 1.0);\n"
            "}\n";

        TestCompilationOk(s, kShaderCompPlatformGLES20);

        core::string outputStr;
        AssignOutputString(outputStr);
        CHECK_NULL(strstr(outputStr.c_str(), "//null"));
        CHECK_NOT_NULL(strstr(outputStr.c_str(), "vec4 null;"));
        CHECK_NOT_NULL(strstr(outputStr.c_str(), "null.x"));

        TestCompilationOk(s, kShaderCompPlatformGLES3Plus);
        AssignOutputString(outputStr);
        CHECK_NOT_NULL(strstr(outputStr.c_str(), "//null"));
        CHECK_NULL(strstr(outputStr.c_str(), "vec4 null;"));
    }

    PARAMETRIC_TEST_FIXTURE(ShaderCompilerTestFixture, HLSLcc_NoDuplicateStructs_GLES, (ShaderCompilerPlatform platform), GLES20_GLES3Plus_OpenGLCore) // Case 1099165
    {
        const char *s =
            "struct StructA {\n"
            "float a;\n"
            "};\n"
            "struct StructB {\n"
            "StructA b1;\n"
            "StructA b2;\n"
            "};\n"
            "struct StructC {\n"
            "StructB c1;\n"
            "StructB c2;\n"
            "StructB c3;\n"
            "};\n"

            "StructC testC;\n"
            "struct appdata\n"
            "{\n"
            "float4 vertex : POSITION;\n"
            "};\n"

            "struct v2f\n"
            "{\n"
            "float4 vertex : SV_POSITION;\n"
            "};\n"

            "v2f vert (appdata v)\n"
            "{\n"
            "v2f o;\n"
            "o.vertex = v.vertex;\n"
            "return o;\n"
            "}\n"

            "float4 frag (v2f i) : SV_Target\n"
            "{\n"
            "return float4(testC.c1.b1.a * testC.c2.b2.a, 0.0, 0.0, 1.0);\n"
            "}\n";

        TestCompilationOk(s, platform);
        core::string outputStr;
        AssignOutputString(outputStr);
        CHECK_EQUAL(1, GetStringOccurences(outputStr, "struct b1_Type"));
    }

    PARAMETRIC_TEST_FIXTURE(ShaderCompilerTestFixture, HLSLcc_W, (ShaderCompilerPlatform platform), OpenGLCore_Metal_Switch)
    {
        const char *s =

            "float4 vert() : POSITION { return float4(0.0, 0.0, 0.0, 1.0f); }\n"

            "uniform float3 _WorldNormal;\n"
            "sampler2D _MainTex;\n"
            "uniform float4 unity_SHAr;\n"
            "uniform float4 unity_SHAg;\n"
            "uniform float4 unity_SHAb;\n"
            "uniform float4 unity_SHBr;\n"
            "uniform float4 unity_SHBg;\n"
            "uniform float4 unity_SHBb;\n"
            "uniform float4 unity_SHC;\n"

            "// normal should be normalized, w=1.0\n"
            "float3 ShadeSH9(float4 normal)\n"
            "{\n"
            "float3 x1, x2, x3; \n"
            "\n"
            "\t// Linear + constant polynomial terms\n"
            "\tx1.r = dot(unity_SHAr, normal); \n"
            "\tx1.g = dot(unity_SHAg, normal); \n"
            "\tx1.b = dot(unity_SHAb, normal); \n"
            "\t\n"
            "\t// 4 of the quadratic polynomials\n"
            "\tfloat4 vB = normal.xyzz * normal.yzzx; \n"
            "\tx2.r = dot(unity_SHBr, vB); \n"
            "\tx2.g = dot(unity_SHBg, vB); \n"
            "\tx2.b = dot(unity_SHBb, vB); \n"
            "\t\n"
            "\t// Final quadratic polynomial\n"
            "\tfloat vC = normal.x*normal.x - normal.y*normal.y; \n"
            "\tx3 = unity_SHC.rgb * vC; \n"
            "\treturn x1 + x2 + x3; \n"
            "}\n"

            "float4 frag() : SV_Target\n"
            "{\n"
            "\tfloat3 temp = ShadeSH9(float4(_WorldNormal, 1.0));\n"

            "\treturn tex2D(_MainTex, temp.xz);\n"
            "}\n";

        TestCompilationOk(s, platform);
    }

    TEST_FIXTURE(ShaderCompilerTestFixture, HLSLcc_SM30)
    {
        const char *s =
            "#define DIRECTIONAL\n"
            "#define SHADOWS_CUBE\n"
            "#include \"UnityCG.cginc\"\n"
            "#include \"UnityDeferredLibrary.cginc\"\n"
            "struct appdata {\n"
            "\tfloat4 vertex : POSITION;\n"
            "\tfloat2 texcoord : TEXCOORD0;\n"
            "};\n"
            "struct v2f {\n"
            "\tfloat4 pos : SV_POSITION;\n"
            "\tfloat2 uv : TEXCOORD0;\n"
            "};\n"
            "float4 _CameraNormalsTexture_ST;\n"
            "v2f vert(appdata v)\n"
            "{\n"
            "\tv2f o;\n"
            "\to.pos = mul(UNITY_MATRIX_MVP, v.vertex);\n"
            "\to.uv = TRANSFORM_TEX(v.texcoord, _CameraNormalsTexture);\n"
            "\treturn o;\n"
            "}\n"
            "sampler2D _CameraNormalsTexture;\n"
            "UNITY_DECLARE_SHADOWMAP(_ShadowTex);\n"

            "float4x4 _WorldToCamera;\n"

            "half4 frag(v2f i) : SV_Target\n"
            "{\n"
            "\tfloat d = SAMPLE_DEPTH_TEXTURE(_CameraDepthTexture, i.uv);\n"
            "\tfloat4 n = tex2Dlod(_CameraNormalsTexture, float4(i.uv, 0.0, 0.0)) * 2.0 - 1.0;\n"
            "\tn += tex2Dlod(_CameraDepthTexture, float4(i.uv, 0.0, 0.0)).xxxx;\n"
            "\td = Linear01Depth(d);\n"
            "   n.w = UNITY_SAMPLE_SHADOW(_ShadowTex, n.yzw);"
            "\tn.xyz = mul((float3x3)_WorldToCamera, n.xyz);\n"
            "\tn.z = -n.w;\n"
            "\treturn (d < (1.0 - 1.0 / 65025.0)) ? EncodeDepthNormal(d, n.xyz) : float4(0.5, 0.5, 1.0, 1.0);\n"
            "}\n";
        inputData.requirements = kShaderRequireShaderModel30;
        TestCompilationOk((core::string("#define sampler2D_float sampler2D\n") + s).c_str(), kShaderCompPlatformOpenGLCore);
        // Check that we actually compile stuff in SM3.0 so we won't sneak any bitcasts in.
        // TODO DISABLED FOR NOW: HLSLcc DX9 compilation won't store the input semantics, so we cannot use it now
        //      CHECK(strstr(data.output.c_str(), "intBitsToFloat") == NULL);
        //      CHECK(strstr(data.output.c_str(), "floatBitsTo") == NULL);
        // Check that we require the relevant extension instead

        core::string outputStr;

        AssignOutputString(outputStr);
        CHECK(strstr(outputStr.c_str(), "#version 150") != NULL);
        // Check that there's no precision qualifiers in samplers, this being desktop GL
        CHECK(strstr(outputStr.c_str(), "uniform highp") == NULL);

        TestCompilationOk(s, kShaderCompPlatformGLES3Plus);
        AssignOutputString(outputStr);
        // Check that we get proper precision qualifiers
        CHECK(strstr(outputStr.c_str(), "uniform highp sampler2D _CameraDepthTexture") != NULL);
        CHECK(strstr(outputStr.c_str(), "uniform mediump sampler2D _CameraNormalsTexture") != NULL);

        TestCompilationOk(s, kShaderCompPlatformGLES20);
        AssignOutputString(outputStr);
        // Check that the texlod workarounds are there
        if (WasUsingCompiler(kShaderCompFlags_PreferHLSL2GLSL))
        {
            CHECK(strstr(outputStr.c_str(), "lowp vec4 impl_low_texture2DLodEXT") != NULL);
            CHECK(strstr(outputStr.c_str(), "highp vec4 impl_high_texture2DLodEXT") != NULL);
        }
        else
        {
            CHECK(strstr(outputStr.c_str(), "#define texture2DLodEXT texture2D") != NULL);
        }
    }

    TEST_FIXTURE(ShaderCompilerTestFixture, HLSLcc_LowpIsPromotedToMediumpOnES3AndIsIntactOnES2)
    {
        const char *s =
            "#define DIRECTIONAL\n"
            "#define SHADOWS_CUBE\n"
            "#include \"UnityCG.cginc\"\n"
            "#include \"UnityDeferredLibrary.cginc\"\n"
            "struct appdata {\n"
            "\tfloat4 vertex : POSITION;\n"
            "\tfloat2 texcoord : TEXCOORD0;\n"
            "};\n"
            "struct v2f {\n"
            "\tfloat4 pos : SV_POSITION;\n"
            "\tfloat2 uv : TEXCOORD0;\n"
            "};\n"
            "float4 _CameraNormalsTexture_ST;\n"
            "v2f vert(appdata v)\n"
            "{\n"
            "\tv2f o;\n"
            "\to.pos = mul(UNITY_MATRIX_MVP, v.vertex);\n"
            "\to.uv = TRANSFORM_TEX(v.texcoord, _CameraNormalsTexture);\n"
            "\treturn o;\n"
            "}\n"
            "sampler2D _CameraNormalsTexture;\n"
            "UNITY_DECLARE_SHADOWMAP(_ShadowTex);\n"

            "float4x4 _WorldToCamera;\n"

            "half4 frag(v2f i) : SV_Target\n"
            "{\n"
            "\tfixed d = SAMPLE_DEPTH_TEXTURE(_CameraDepthTexture, i.uv);\n"
            "\tfixed4 n = tex2Dlod(_CameraNormalsTexture, float4(i.uv, 0.0, 0.0)) * 2.0 - 1.0;\n"
            "\tn += tex2Dlod(_CameraDepthTexture, float4(i.uv, 0.0, 0.0)).xxxx;\n"
            "\td = Linear01Depth(d);\n"
            "   n.w = UNITY_SAMPLE_SHADOW(_ShadowTex, n.yzw);"
            "\tn.xyz = mul((float3x3)_WorldToCamera, n.xyz);\n"
            "\tn.z = -n.w;\n"
            "\treturn (d < (1.0 - 1.0 / 65025.0)) ? EncodeDepthNormal(d, n.xyz) : float4(0.5, 0.5, 1.0, 1.0);\n"
            "}\n";
        inputData.requirements = kShaderRequireShaderModel30;

        core::string outputStr;

        TestCompilationOk(s, kShaderCompPlatformGLES3Plus);
        AssignOutputString(outputStr);
        CHECK(strstr(outputStr.c_str(), "lowp vec4") == NULL);
        CHECK(strstr(outputStr.c_str(), "lowp sampler2D") == NULL);
        CHECK(strstr(outputStr.c_str(), "mediump vec4") != NULL);
        CHECK(strstr(outputStr.c_str(), "mediump sampler2D") != NULL);

        TestCompilationOk(s, kShaderCompPlatformGLES20);
        AssignOutputString(outputStr);
        CHECK(strstr(outputStr.c_str(), "lowp vec4") != NULL);
        CHECK(strstr(outputStr.c_str(), "lowp sampler2D") != NULL);
    }

    PARAMETRIC_TEST_FIXTURE(ShaderCompilerTestFixture, HLSLcc_DataTypes, (ShaderCompilerPlatform platform), GLES3Plus_Metal_Vulkan_Switch)
    {
        const char *s =
            "#define DIRECTIONAL\n"
            "#define SHADOWS_CUBE\n"
            "#include \"UnityCG.cginc\"\n"
            "#include \"TerrainEngine.cginc\"\n"
            "struct appdata {\n"
            "\tfloat4 vertex : POSITION;\n"
            "\tfloat2 texcoord : TEXCOORD0;\n"
            "};\n"
            "struct v2f {\n"
            "\tfloat4 pos : SV_POSITION;\n"
            "\tfloat2 uv : TEXCOORD0;\n"
            "   half4 color : COLOR0;\n"
            "};\n"
            "float4 _CameraNormalsTexture_ST;\n"
            "v2f vert(appdata_tree_billboard v)\n"
            "{\n"
            "\tv2f o;\n"
            "\tTerrainBillboardTree(v.vertex, v.texcoord1.xy, v.texcoord.y);\n"
            "\to.pos = mul(UNITY_MATRIX_MVP, v.vertex);\n"
            "\to.uv.x = v.texcoord.x;\n"
            "\to.uv.y = v.texcoord.y > 0;\n"
            "   o.color = v.color;\n"
            "\treturn o;\n"
            "}\n"
            "sampler2D _MainTex;\n"
            "half4 frag(v2f input) : SV_Target\n"
            "{\n"
            "\thalf4 col = tex2D(_MainTex, input.uv);\n"
            "\tcol.rgb *= input.color.rgb;\n"
            "\tclip(col.a);\n"
            "\tUNITY_APPLY_FOG(input.fogCoord, col);\n"
            "\treturn col;\n"
            "}\n";

        TestCompilationOk(s, platform);
    }

    TEST_FIXTURE(ShaderCompilerTestFixture, HLSLccMultiview)
    {
        const char *s =
            "#include \"UnityCG.cginc\"\n"
            "struct appdata {\n"
            "\tfloat4 vertex : POSITION;\n"
            "};\n"
            "struct v2f {\n"
            "\tfloat4 pos : SV_POSITION;\n"
            "\tUNITY_VERTEX_OUTPUT_STEREO\n"
            "};\n"
            "float4x4 eyeMatrices[4];\n"
            "v2f vert(appdata v)\n"
            "{\n"
            "\tv2f o;\n"
            "\tUNITY_INITIALIZE_VERTEX_OUTPUT_STEREO(o);\n"
            "\to.pos = mul(eyeMatrices[unity_StereoEyeIndex], v.vertex);\n"
            "\treturn o;\n"
            "}\n"
            "half4 frag(v2f input) : SV_Target\n"
            "{\n"
            "\tUNITY_SETUP_STEREO_EYE_INDEX_POST_VERTEX(input);\n"
            "\treturn half4(unity_StereoEyeIndex, unity_StereoEyeIndex, unity_StereoEyeIndex, 0.0);\n"
            "}\n";

        std::vector<ShaderCompileArg> args;
        args.push_back(ShaderCompileArg("STEREO_MULTIVIEW_ON", "1"));
        args.push_back(ShaderCompileArg("SHADER_STAGE_VERTEX", "1"));
        TestCompilationOkWithCompileArgs(s, kShaderCompPlatformGLES3Plus, &args[0], args.size());
    }

    PARAMETRIC_TEST_FIXTURE(ShaderCompilerTestFixture, HLSLccFramebufferFetch, (ShaderCompilerPlatform platform), OpenGLCore_Metal_Switch)
    {
        const char *s =
            "#include \"UnityCG.cginc\"\n"
            "struct appdata_t {\n"
            "\tfloat4 vertex : POSITION;\n"
            "\tfloat2 texcoord : TEXCOORD0;\n"
            "\tfloat4 color : COLOR;\n"
            "};\n"
            "struct v2f {\n"
            "\tfloat4 vertex : SV_POSITION;\n"
            "\tfixed4 texcoord : TEXCOORD0;\n"
            "\tfloat4 color : COLOR;\n"
            "};\n"
            "v2f vert (appdata_t v)\n"
            "{\n"
            "\tv2f o;\n"
            "\to.vertex = mul(UNITY_MATRIX_MVP, v.vertex);\n"
            "\to.texcoord.rg = v.texcoord*4.0;\n"
            "\to.texcoord.ba = 0;\n"
            "\to.color = v.color;\n"
            "\to.color.x = 1;\n"
            "\treturn o;\n"
            "\t}\n"
            "#ifdef UNITY_FRAMEBUFFER_FETCH_AVAILABLE\n"
            "void frag (v2f i, in half4 o0 : SV_Target0, out half4 o1 : SV_Target1, inout half4 o2 : SV_Target2)\n"
            "{\n"
            "\ti.texcoord = frac(i.texcoord);\n"
            "\to1 = i.texcoord.xxyy + o2 + o0;\n"
            "\to2 = o0;\n"
            "}\n"
            "#else\n"
            "fixed4 frag (v2f i) : COLOR\n"
            "{\n"
            "\ti.texcoord = frac(i.texcoord);\n"
            "\treturn i.texcoord + i.color.y;\n"
            "}\n"
            "#endif\n";
        inputData.compileFlags = kShaderCompFlags_PreferHLSLcc;

        if (platform == kShaderCompPlatformMetal)
        {
            ShaderCompileArg fetchAvailable = ShaderCompileArg("UNITY_FRAMEBUFFER_FETCH_AVAILABLE", "1");
            core::string outputString = "";

            TestCompilationOkWithCompileArg(s, kShaderCompPlatformGLES20, &fetchAvailable);
            outputString = GetOutputString();

            TestCompilationOkWithCompileArg(s, kShaderCompPlatformGLES3Plus, NULL);
            outputString = GetOutputString();

            TestCompilationOkWithCompileArg(s, platform, &fetchAvailable);
            outputString = GetMetalFragmentShaderOutputString();
        }
        else
            TestCompilationOk(s, platform);
    }

    PARAMETRIC_TEST_FIXTURE(ShaderCompilerTestFixture, HLSLcc_Ftou, (ShaderCompilerPlatform platform), OpenGLCore_Metal_Switch)
    {
        const char *s =
            "uniform float4 _TreeInfo[4];\n"
            "uniform float4 v_color;\n"
            "uniform float4 v_texcoord;\n"
            "uniform float4 _TreeSize[4];\n"
            "uniform float4 _InstanceData;\n"
            "uniform float _CameraXZAngle;\n"
            "uniform float3 _BillboardTangent;\n"
            "uniform float4 _ImageTexCoords[32];\n"
            "float4 vert() : POSITION { return float4(0.0, 0.0, 0.0, 1.0f); }\n"
            "float4 frag() : SV_Target\n"
            "{\n"
            "float treeType = v_color.a * 255.0f; \n"
            "float4 treeInfo = _TreeInfo[treeType]; \n"
            "float4 treeSize = _TreeSize[treeType]; \n"
            ""
            "\n"
            "float3 instanceData = _InstanceData.w > 0 ? _InstanceData.xyz : v_texcoord.xyz; \n"
            "float widthScale = instanceData.x; \n"
            "float heightScale = instanceData.y; \n"
            "float rotation = instanceData.z; \n"

            "float2 percent = v_texcoord.xy; \n"
            "float4 v_vertex = float4(0.0, 0.0, 0.0, 0.0);\n"
            "v_vertex.xyz += (percent.x - 0.5f) * treeSize.x * widthScale * _BillboardTangent; \n"
            "v_vertex.y += (percent.y * treeSize.y + treeSize.z) * heightScale; \n"
            "v_vertex.w = 1.0f; \n"
            "float4 data_mainUV = float4(0.0, 0.0, 0.0, 1.0);\n"
            "float slices = treeInfo.x; \n"
            "float invDelta = treeInfo.y; \n"
            "float angle = _CameraXZAngle + rotation; \n"
            "float imageIndex = fmod(floor(angle * invDelta + 0.5f), slices); \n"
            "float4 imageTexCoords = _ImageTexCoords[treeInfo.z + imageIndex]; \n"
            "if (imageTexCoords.w < 0)\n"
            "{\n"
            "data_mainUV.xy = imageTexCoords.xy - imageTexCoords.zw * percent.yx; \n"
            "}\n"
            "else\n"
            "{\n"
            "data_mainUV.xy = imageTexCoords.xy + imageTexCoords.zw * percent; \n"
            "}\n"
            "return data_mainUV;"
            "}\n";

        TestCompilationOk(s, platform);
    }

    PARAMETRIC_TEST_FIXTURE(ShaderCompilerTestFixture, HLSLcc_WriteMasks, (ShaderCompilerPlatform platform), OpenGLCore_Metal_Switch)
    {
        const char *s =
            "int _Int1;\n"
            "int4 _Int4;\n"
            "float4 _Float4;\n"
            "float4 vert() : POSITION { return float4(0.0, 0.0, 0.0, 1.0f); }\n"
            "float4 frag() : SV_Target\n"
            "{\n"
            "int4 ii = _Int4;\n"
            "ii.x += _Int1;\n"
            "return ii * _Float4;\n"
            "}\n";

        TestCompilationOk(s, platform);
    }

    PARAMETRIC_TEST_FIXTURE(ShaderCompilerTestFixture, HLSLcc_BooleanOps, (ShaderCompilerPlatform platform), GLES3Plus_OpenGLCore_Metal_Switch)
    {
        const char *s =
            "  #include \"UnityCG.cginc\"\n"
            "  \n"
            "  \n"
            "  \tsampler2D _MainTex;\n"
            "  \thalf4 _MainTex_ST;\n"
            "\n"
            "     struct v2f \n"
            "     {\n"
            "\n"
            "        float4  pos : SV_POSITION;\n"
            "        float2  uv : TEXCOORD0;\n"
            "    };\n"
            "    \n"
            "\n"
            "     v2f vert (appdata_base v)\n"
            "    {\n"
            "        v2f o;\n"
            "        o.pos = mul (UNITY_MATRIX_MVP, v.vertex);\n"
            "        o.uv = TRANSFORM_TEX(v.texcoord, _MainTex);\n"
            "        \n"
            "        return o;\n"
            "    }\t\n"
            "\n"
            "   \n"
            "   \tsampler2D _MaskTex; \n"
            "\t\t\n"
            "    half4 frag(v2f i) : COLOR\n"
            "    {\n"
            "        fixed4 col = fixed4(1,1,1,1);\n"
            "      \tfixed4 mask = tex2D(_MaskTex, i.uv);\n"
            "        fixed4 c = col;\n"
            "        \n"
            "        if (mask.a > 0.165 && mask.a <= 0.33)  //Mask1\n"
            "        {\n"
            "\n"
            "\t\t\tc.rgb = fixed3(0,1,0);\n"
            "        }\n"
            "        if (mask.a > 0.33 && mask.a <= 0.495)  //Mask2\n"
            "        {\n"
            "\t\t\tc.rgb = fixed3(1,0,0);\n"
            "\n"
            "        }\n"
            "        return c ;\n"
            "    }\n"
        ;

        TestCompilationOk(s, platform);
    }

    PARAMETRIC_TEST_FIXTURE(ShaderCompilerTestFixture, HLSLcc_AutoExpand, (ShaderCompilerPlatform platform), GLES3Plus_Metal_Switch)
    {
        const char *s =
            "float unity_MaxOutputValue;\n"
            "half3 datain;\n"
            "float boost;\n"
            "float4 vert() : POSITION { return float4(0.0, 0.0, 0.0, 1.0f); }\n"
            "float4 frag() : SV_Target\n"
            "{\n"
            "half4 res = 0;\n"
            "res = half4(datain, 1);\n"
            "res.rgb = clamp(pow(res.rgb, boost), 0, unity_MaxOutputValue);\n"
            "return res;\n"
            "}\n";

        TestCompilationOk(s, platform);
    }

    PARAMETRIC_TEST_FIXTURE(ShaderCompilerTestFixture, HLSLcc_StructuredLoadMat, (ShaderCompilerPlatform platform), OpenGLCore_Metal_Switch)
    {
        const char *s =
            "struct vs_input {\n"
            "\tfloat4 vertex : POSITION;\n"
            "\tfloat3 normal : NORMAL;\n"
            "\tfloat2 texcoord : TEXCOORD0;\n"
            "\tfloat4 color : COLOR; // color.r is used as bone index!\n"
            "};\n"
            "StructuredBuffer<float4x4> buf_BoneMatrices;\n"
            "struct ps_input {\n"
            "\tfloat4 pos : SV_POSITION;\n"
            "\tfloat2 uv : TEXCOORD0;\n"
            "\tfloat4 color : COLOR;\n"
            "};\n"
            "ps_input vert(vs_input v, uint id : SV_VertexID)\n"
            "{\n"
            "\tps_input o;\n"
            "\tint boneIndex = (int)(v.color.r * 255.0f);\n"
            "\tfloat4x4 worldMat = buf_BoneMatrices[boneIndex];\n"
            "\tfloat4 worldPos = mul(worldMat, v.vertex);\n"
            "\to.pos = worldPos;\n"
            "\to.uv = v.texcoord;\n"
            "\treturn o;\n"
            "}\n"
            "sampler2D _MainTex;\n"
            "float4 frag(ps_input i) : COLOR\n"
            "{\n"
            "\treturn tex2D(_MainTex, i.uv) * i.color * 2.0f;\n"
            "}\n";

        inputData.requirements = kShaderRequireShaderModel50;
        TestCompilationOk(s, platform);
    }

    TEST_FIXTURE(ShaderCompilerTestFixture, HLSLcc_AppendStructuredBuffer)
    {
        const char *s =
            "uniform float4 _ScreenParams;\n"
            "float4x4 UNITY_MATRIX_MVP;\n"
            "inline half Luminance(half3 c)\n"
            "{\n"
            "\treturn dot(c, half3(0.22, 0.707, 0.071));\n"
            "}\n"
            "struct appdata {\n"
            "\tfloat4 vertex : POSITION;\n"
            "\tfloat2 texcoord : TEXCOORD0;\n"
            "};\n"
            "struct v2f {\n"
            "\tfloat4 pos : SV_POSITION;\n"
            "\tfloat2 uv : TEXCOORD0;\n"
            "};\n"
            "v2f vert(appdata v)\n"
            "{\n"
            "\tv2f o;\n"
            "\to.pos = mul(UNITY_MATRIX_MVP, v.vertex);\n"
            "\to.uv = v.texcoord;\n"
            "\treturn o;\n"
            "}\n"
            "sampler2D _MainTex;\n"
            "AppendStructuredBuffer<float2> pointBufferOutput : register(u1);\n"
            "half4 frag(v2f i) : COLOR0\n"
            "{\n"
            "\thalf4 c = tex2D(_MainTex, i.uv);\n"
            "\thalf4 c1 = tex2D(_MainTex, i.uv + 2.0 / _ScreenParams.xy);\n"
            "\tfloat lumc = Luminance(c);\n"
            "\tfloat lumc1 = Luminance(c1);\n"
            "\t[branch]\n"
            "\tif (lumc > 0.98 && lumc > lumc1)\n"
            "\t{\n"
            "\t\tpointBufferOutput.Append(i.uv);\n"
            "\t}\n"
            "\treturn c * 0.5;\n"
            "}\n";

        inputData.requirements = kShaderRequireShaderModel50;
        TestCompilationOk(s, kShaderCompPlatformOpenGLCore);
    }

    PARAMETRIC_TEST_FIXTURE(ShaderCompilerTestFixture, HLSLcc_case830427, (ShaderCompilerPlatform platform), OpenGLCore_Metal_Vulkan_Switch)
    {
        const char *s =
            "\t\t\t#include \"UnityCG.cginc\"\n"
            "\n"
            "\t\t\tstruct appdata\n"
            "\t\t\t{\n"
            "\t\t\t\tfloat4 vertex : POSITION;\n"
            "\t\t\t\tfloat2 uv : TEXCOORD0;\n"
            "\t\t\t};\n"
            "\n"
            "\t\t\tstruct v2f\n"
            "\t\t\t{\n"
            "\t\t\t\tfixed2 uv : TEXCOORD0;\n"
            "\t\t\t\tfloat4 vertex : SV_POSITION;\n"
            "\t\t\t};\n"
            "\n"
            "\t\t\tfloat _Flip;\n"
            "\n"
            "\t\t\tv2f vert(appdata v)\n"
            "\t\t\t{\n"
            "\t\t\t\tv2f o;\n"
            "\t\t\t\to.vertex = mul(UNITY_MATRIX_MVP, v.vertex);\n"
            "\t\t\t\to.uv = v.uv * (1 - _Flip) + _Flip * float2(v.uv.x, 1 - v.uv.y);\n"
            "\t\t\t\treturn o;\n"
            "\t\t\t}\n"
            "\n"
            "\t\t\tsampler2D_float _CameraDepthTexture;\n"
            "\n"
            "\t\t\tfixed frag(v2f i) : SV_Depth\n"
            "\t\t\t{\n"
            "\t\t\t\tfixed4 col = tex2D(_CameraDepthTexture, i.uv);\n"
            "\t\t\t\treturn col.r;\n"
            "\t\t\t}\n";

        TestCompilationOk(s, platform);
    }

    PARAMETRIC_TEST_FIXTURE(ShaderCompilerTestFixture, HLSLcc_WarnNonFloatDepthSampler, (ShaderCompilerPlatform platform), OpenGLCore_Metal_Vulkan_Switch)
    {
        const char *s =
            "#include \"UnityCG.cginc\"\n"
            "\n"
            "struct v2f\n"
            "{\n"
            "   float4 pos : SV_POSITION;\n"
            "\tfixed2 uv : TEXCOORD0;\n"
            "};\n"
            "float4 vert() : POSITION { return float4(0.0, 0.0, 0.0, 1.0f); }\n"
            "\n"
            "sampler2D_half _CameraDepthTexture;\n"
            "\n"
            "fixed frag(v2f i) : SV_Target\n"
            "{\n"
            "\tfixed4 col = tex2D(_CameraDepthTexture, i.uv);\n"
            "\treturn col.r;\n"
            "}\n";

        TestCompilationOk(s, kShaderCompPlatformOpenGLCore);

        if (platform == kShaderCompPlatformMetal)
        {
            RunCompilation(s, platform);
            CHECK_EQUAL(0, errors.size());
            CHECK_EQUAL(1, metalFragmentErrors.size());
            CHECK_EQUAL(true, metalFragmentErrors[0].warning);
            CHECK(BeginsWith(metalFragmentErrors[0].message, "_CameraDepthTexture should be float on Metal"));
        }
        else
            TestCompilationOk(s, platform);
    }

    PARAMETRIC_TEST_FIXTURE(ShaderCompilerTestFixture, HLSLcc_ImmediateConstArray, (ShaderCompilerPlatform platform), OpenGLCore_Metal_Vulkan_Switch)
    {
        const char *s =
            "static const int SmallDiscKernelSamples = 7;\n"
            "static const float2 SmallDiscKernel[SmallDiscKernelSamples] =\n"
            "{\n"
            "\tfloat2(0.00, 0.10),\n"
            "\tfloat2(0.01, 0.11),\n"
            "\tfloat2(0.02, 0.12),\n"
            "\tfloat2(0.03, 0.13),\n"
            "\tfloat2(0.04, 0.14),\n"
            "\tfloat2(0.05, 0.15),\n"
            "\tfloat2(0.06, 0.16)\n"
            "};\n"
            "static const int NumDiscSamples = 5;\n"
            "static const float3 DiscKernel[NumDiscSamples] =\n"
            "{\n"
            "\tfloat3(10.0, 10.1, 10.2),\n"
            "\tfloat3(11.0, 11.1, 11.2),\n"
            "\tfloat3(12.0, 12.1, 12.2),\n"
            "\tfloat3(13.0, 13.1, 13.2),\n"
            "\tfloat3(14.0, 14.1, 14.2)\n"
            "};\n"
            "static const float DiscKernel2[NumDiscSamples*2] =\n"
            "{\n"
            "\t100.0,\n"
            "\t101.0,\n"
            "\t102.0,\n"
            "\t103.0,\n"
            "\t104.0,\n"
            "\t105.0,\n"
            "\t106.0,\n"
            "\t107.0,\n"
            "\t108.0,\n"
            "\t109.0\n"
            "};\n"
            "#ifdef SHADER_API_METAL\n"
            "uniform float4 foo[40];\n"
            "#else\n"
            "StructuredBuffer<float4> foo;\n"
            "#endif\n"
            "uniform int myidx;\n"
            "float4 vert() : POSITION { return float4(0.0, 0.0, 0.0, 1.0f); }\n"
            "float4 frag() : SV_Target\n"
            "{\n"
            "float4 res = float4(0.0,0.0,0.0,0.0);\n"
            "for (int i = 0; i < 5; i++){\n"
            "\tres.xy += foo[i] * SmallDiscKernel[i].xy + foo[i+10] * DiscKernel[i].xy + DiscKernel[i].z * DiscKernel2[myidx] ;\n"
            "}\n"
            "return res;\n"
            "}\n";

        TestCompilationOk(s, platform);
    }

    PARAMETRIC_TEST_FIXTURE(ShaderCompilerTestFixture, HLSLcc_GatherOps, (ShaderCompilerPlatform platform), OpenGLCore_Metal_Switch)
    {
        const char *s =
            "Texture2D tex;\n"
            "SamplerState samplertex;\n"
            "Texture2D dtex;\n"
            "SamplerComparisonState samlercmpdtex;\n"
            "struct appdata {\n"
            "\tfloat4 vertex : POSITION;\n"
            "\tfloat2 texcoord : TEXCOORD0;\n"
            "};\n"
            "struct v2f {\n"
            "\tfloat4 pos : SV_POSITION;\n"
            "\tfloat2 uv : TEXCOORD0;\n"
            "};\n"
            "v2f vert(appdata v)\n"
            "{\n"
            "\tv2f o;\n"
            "   o.pos = v.vertex;\n"
            "\to.uv = v.texcoord;\n"
            "\treturn o;\n"
            "}\n"
            "half4 frag(v2f i) : COLOR0\n"
            "{\n"
            "   int2 offset = int2(i.uv);\n"
            "\tfloat4 r = tex.Gather(samplertex, i.uv);\n"
            "\tfloat4 roff = tex.Gather(samplertex, i.uv, int2(2,3));\n"
            "\tfloat4 g = tex.GatherGreen(samplertex, i.uv);\n"
            "\tfloat4 boff = tex.GatherBlue(samplertex, i.uv, offset);\n"
            "\tfloat4 a = tex.GatherAlpha(samplertex, i.uv);\n"
            "\tfloat4 rcmp = dtex.GatherCmp(samlercmpdtex, i.uv, 1.0);\n"
            "\tfloat4 rcmpoff = dtex.GatherCmpRed(samlercmpdtex, i.uv, i.uv.x, int2(4,5));\n"
            "\treturn (r + roff + g + boff + a + rcmp + rcmpoff);\n"
            "}\n";

        inputData.requirements = kShaderRequireShaderModel50;
        TestCompilationOk(s, platform);
    }

    PARAMETRIC_TEST_FIXTURE(ShaderCompilerTestFixture, HLSLcc_HalfSampler, (ShaderCompilerPlatform platform), GLES20_GLES3Plus_Vulkan)
    {
        const char *s =
            "#include \"UnityCG.cginc\"\n"
            "sampler2D_half\t_MainTex;\n"
            "UNITY_DECLARE_TEX2D_HALF(OtherTex);\n"
            "float4 vert() : POSITION { return float4(0.0, 0.0, 0.0, 1.0f); }\n"
            "float4 frag() : SV_Target\n"
            "{\n"
            "half4 col = tex2D(_MainTex, float2(0.0f, 0.0f));\n"
            "half4 col2 = UNITY_SAMPLE_TEX2D(OtherTex, float2(0.0f, 0.0f));\n"
            "return col * col2;\n"
            "}\n";

        TestCompilationOk(s, platform);

        if (platform == kShaderCompPlatformGLES20 || platform == kShaderCompPlatformGLES3Plus)
        {
            // The output shader should contain mediump but no lowp.
            core::string outputString = GetOutputString();
            CHECK(outputString.find("mediump sampler2D") != core::string::npos);
            CHECK(outputString.find("mediump vec4") != core::string::npos);
            CHECK(outputString.find("lowp sampler2D") == core::string::npos);
            CHECK(outputString.find("lowp vec4") == core::string::npos);
        }
    }

    PARAMETRIC_TEST_FIXTURE(ShaderCompilerTestFixture, HLSLcc_PointSize, (ShaderCompilerPlatform platform), GLES3Plus_Vulkan_Switch)
    {
        const char *s =
            "#include \"UnityCG.cginc\"\n"
            "sampler2D_half\t_MainTex;\n"
            "struct v2f {\n"
            "\tfloat4 pos : SV_POSITION;\n"
            "\tfloat sz : PSIZE;\n"
            "};\n"
            "v2f vert() { v2f o; o.pos = float4(0.0, 0.0, 0.0, 1.0f); o.sz = 1.0f; return o; }\n"
            "float4 frag() : SV_Target\n"
            "{\n"
            "half4 col = tex2D(_MainTex, float2(0.0f, 0.0f));\n"
            "return col;\n"
            "}\n";


        TestCompilationOk(s, platform);

        if (platform == kShaderCompPlatformGLES3Plus)
        {
            // The output shader should contain a write to gl_PointSize
            core::string outputString = GetOutputString();
            CHECK(outputString.find("gl_PointSize") != core::string::npos);
        }
    }

    PARAMETRIC_TEST_FIXTURE(ShaderCompilerTestFixture, HLSLcc_TestVertexIDAsVector, (ShaderCompilerPlatform platform), OpenGLCore_Metal)
    {
        // at some point we were skipping any treatment of builtin shader inputs
        // meaning we were loosing all the info about it being scalar/vector
        // we check that using vertexID (scalar) in vector op correctly construct interim vector
        const char *s =
            "struct appdata {\n"
            "\tfloat4 vertex : POSITION;\n"
            "\tuint vertexID : SV_VertexID;\n"
            "};\n"
            "struct v2f {\n"
            "\tfloat4 pos : SV_POSITION;\n"
            "\tfloat2 uv : TEXCOORD0;\n"
            "};\n"
            "v2f vert(appdata v)\n"
            "{\n"
            "\tv2f o;\n"
            "\to.pos = v.vertex;\n"
            "\to.uv = float2(v.vertexID == 1, v.vertexID == 2);\n"
            "\treturn o;\n"
            "}\n"
            "half4 frag(v2f i) : COLOR0\n"
            "{\n"
            "\treturn i.uv.xyxy;\n"
            "}\n";

        TestCompilationOk(s, platform);

        if (platform == kShaderCompPlatformMetal)
        {
            core::string outputString = GetMetalVertexShaderOutputString();
            CHECK(outputString.find("int2(mtl_VertexID)") != core::string::npos);
        }
        else
        {
            core::string outputString = GetOutputString();
            CHECK(outputString.find("ivec4(gl_VertexID)") != core::string::npos);
        }
    }

    TEST_FIXTURE(ShaderCompilerTestFixture, HLSLcc_TranslateVariableNameWithMask_SwizzlesDoNotLeak)
    {
        const char *s =
            "float4 fl4[30];\n"
            "float4 foo;\n"
            "float4 vert() : POSITION { return float4(0.0, 0.0, 0.0, 1.0f); }\n"
            "float4 frag() : SV_Target\n"
            "{\n"
            "float sbi1 = (1.0 - foo.x) * 32;\n"
            "float sbi2 = (1.0 - foo.y) * 32;\n"
            "float4 fl4lerp = lerp(fl4[sbi1], fl4[sbi2], 0.5)\n;"
            "return fl4lerp;\n"
            "}\n";


        TestCompilationOk(s, kShaderCompPlatformOpenGLCore);

        core::string outputString = GetOutputString();
        CHECK(outputString.find(".xfl4") == core::string::npos);
        CHECK(outputString.find(".yfl4") == core::string::npos);
    }

    TEST_FIXTURE(ShaderCompilerTestFixture, HLSLcc_UnormImage)
    {
        const char *s =
            "RWTexture2D<unorm float4> _Input;\n"
            "int2 _Foo;\n"
            "float4 vert() : POSITION { return float4(0.0, 0.0, 0.0, 1.0f); }\n"
            "float4 frag() : SV_Target\n"
            "{\n"
            "float4 data = _Input[_Foo.xy];"
            "return data;\n"
            "}\n";

        TestCompilationOk(s, kShaderCompPlatformVulkan);
    }

    TEST_FIXTURE(ShaderCompilerTestFixture, AvailableTargetFeatureMacro_Target30_Works)
    {
        inputData.requirements = kShaderRequireShaderModel30;
        const char* src =
            "#include \"UnityCG.cginc\"\n"
            "#if "
            "\t(SHADER_API_D3D11 && !SHADER_API_XBOXONE && SHADER_TARGET_AVAILABLE == 40 && SHADER_AVAILABLE_INTEGERS) ||" // on DX11 SM3.0 -> SM4.0 which has integers
            "\t(SHADER_API_GLCORE && SHADER_TARGET_AVAILABLE == 35 && SHADER_AVAILABLE_2DARRAY) ||" // on GL SM3.0 -> SM3.5 which has 2d array
            "\t(SHADER_API_GLES3 && SHADER_TARGET_AVAILABLE == 35 && SHADER_AVAILABLE_2DARRAY) ||" // on GLES3 SM3.0 -> SM3.5 which has 2d array
            "\t(SHADER_API_GLES && SHADER_TARGET_AVAILABLE == 30 && SHADER_AVAILABLE_FRAGCOORD) ||" // on GLES2 SM3.0 stays SM3.0, and always has fragment coord
            "\t(SHADER_API_METAL && SHADER_TARGET_AVAILABLE == 45 && SHADER_AVAILABLE_COMPUTE) ||" // on Metal SM3.0 -> SM4.5 which has compute
            "\t(SHADER_API_VULKAN && SHADER_TARGET_AVAILABLE == 45 && SHADER_AVAILABLE_COMPUTE) ||" // on Vulkan SM3.0 -> SM4.5 which has compute
            "\t(SHADER_API_PSSL && SHADER_TARGET_AVAILABLE == 50 && SHADER_AVAILABLE_TESSELLATION) ||" // on PS4 SM3.0 -> SM5.0 which has tessellation
            "\t(SHADER_API_XBOXONE && SHADER_TARGET_AVAILABLE == 50 && SHADER_AVAILABLE_TESSELLATION) ||" // on XB1 SM3.0 -> SM5.0 which has tessellation
            "\t(SHADER_API_SWITCH && SHADER_TARGET_AVAILABLE == 50 && SHADER_AVAILABLE_TESSELLATION)\n" // on Switch SM3.0 -> SM5.0 which has tessellation
            "\tfloat4 vert() : SV_POSITION { return 0.5f; }\n"
            "\tfloat4 frag() : SV_Target { return 0.5f; }\n"
            "#else\n"
            "\terror_wrong_targets_macros;\n"
            "#endif";
        TestCompilationOk(src, kShaderCompPlatformD3D11);
        TestCompilationOk(src, kShaderCompPlatformOpenGLCore);
        TestCompilationOk(src, kShaderCompPlatformGLES3Plus);
        TestCompilationOk(src, kShaderCompPlatformGLES20);
        TestCompilationOk(src, kShaderCompPlatformMetal);
        TestCompilationOk(src, kShaderCompPlatformVulkan);
#       if ENABLE_XBOXONE_COMPILER
        TestCompilationOk(src, kShaderCompPlatformXboxOne);
#       endif
#       if ENABLE_PSSL_COMPILER
        TestCompilationOk(src, kShaderCompPlatformPS4);
#       endif
#       if ENABLE_SWITCH_COMPILER
        TestCompilationOk(src, kShaderCompPlatformSwitch);
#       endif
    }

    TEST_FIXTURE(ShaderCompilerTestFixture, AvailableTargetFeatureMacro_Target45_Works)
    {
        inputData.requirements = kShaderRequireShaderModel45_ES31;
        const char* src =
            "#include \"UnityCG.cginc\"\n"
            "#if "
            "\t(SHADER_API_D3D11 && !SHADER_API_XBOXONE && SHADER_TARGET_AVAILABLE == 50 && SHADER_AVAILABLE_TESSELLATION) ||" // on DX11 SM4.5 -> SM5.0 which has tessellation
            "\t(SHADER_API_GLCORE && SHADER_TARGET_AVAILABLE == 45 && SHADER_AVAILABLE_COMPUTE) ||" // on GL SM4.5 stays SM4.5
            "\t(SHADER_API_GLES3 && SHADER_TARGET_AVAILABLE == 45 && SHADER_AVAILABLE_COMPUTE) ||" // on GLES3 SM4.5 stays SM4.5
            "\t(SHADER_API_METAL && SHADER_TARGET_AVAILABLE == 45 && SHADER_AVAILABLE_COMPUTE) ||" // on Metal SM4.5 stays SM4.5
            "\t(SHADER_API_VULKAN && SHADER_TARGET_AVAILABLE == 45 && SHADER_AVAILABLE_COMPUTE) ||" // on Vulkan SM4.5 stays SM4.5
            "\t(SHADER_API_PSSL && SHADER_TARGET_AVAILABLE == 50 && SHADER_AVAILABLE_TESSELLATION) ||" // on PS4 SM4.5 -> SM5.0 which has tessellation
            "\t(SHADER_API_SWITCH && SHADER_TARGET_AVAILABLE == 50 && SHADER_AVAILABLE_TESSELLATION) ||" // on Switch SM4.5 -> SM5.0 which has tessellation
            "\t(SHADER_API_XBOXONE && SHADER_TARGET_AVAILABLE == 50 && SHADER_AVAILABLE_TESSELLATION) \n" // on XB1 SM4.5 -> SM5.0 which has tessellation
            "\tfloat4 vert() : SV_POSITION { return 0.5f; }\n"
            "\tfloat4 frag() : SV_Target { return 0.5f; }\n"
            "#else\n"
            "\terror_wrong_targets_macros;\n"
            "#endif";
        TestCompilationOk(src, kShaderCompPlatformD3D11);
        TestCompilationOk(src, kShaderCompPlatformOpenGLCore);
        TestCompilationOk(src, kShaderCompPlatformGLES3Plus);
        // not relevant TestCompilationOk(src, kShaderCompPlatformGLES20);
        TestCompilationOk(src, kShaderCompPlatformMetal);
        TestCompilationOk(src, kShaderCompPlatformVulkan);
#       if ENABLE_XBOXONE_COMPILER
        TestCompilationOk(src, kShaderCompPlatformXboxOne);
#       endif
#       if ENABLE_PSSL_COMPILER
        TestCompilationOk(src, kShaderCompPlatformPS4);
#       endif
#       if ENABLE_SWITCH_COMPILER
        TestCompilationOk(src, kShaderCompPlatformSwitch);
#       endif
    }

    // Do not remove this: it's a debugging helper for easy debugging of single shader compiler failure cases
    /*
    TEST_FIXTURE(ShaderCompilerTestFixture, HLSLcc_File)
    {
        core::string s = "";
        CHECK(ReadTextFile("c:\\work\\test.txt", s));
        TestCompilationOk(s.c_str(), kShaderCompPlatformOpenGLCore);
        TestCompilationOk(s.c_str(), kShaderCompPlatformVulkan);
    }
    */

    PARAMETRIC_TEST_FIXTURE(ShaderCompilerTestFixture, UnitySamplingMacros_Compile, (ShaderCompilerPlatform platform), AnalyzerAndAllPlatforms)
    {
        const char *s =
            "#include \"UnityCG.cginc\"\n"
            "#include \"HLSLSupport.cginc\"\n"
            "float4 vert() : SV_POSITION { return float4(0.0, 0.0, 0.0, 1.0f); }\n"
            "UNITY_DECLARE_TEX2D(tex0);\n"
            "UNITY_DECLARE_TEX2D_NOSAMPLER(tex1);\n"
            "UNITY_DECLARE_TEXCUBE(tex2);\n"
            "UNITY_DECLARE_TEXCUBE_NOSAMPLER(tex3);\n"
            "UNITY_DECLARE_TEX3D(tex4);\n"
            "UNITY_DECLARE_TEX3D_NOSAMPLER(tex5);\n"
            "UNITY_DECLARE_TEX2DARRAY(tex6);\n"
            "UNITY_DECLARE_TEX2DARRAY_NOSAMPLER(tex7);\n"
            "float4 frag() : SV_Target\n"
            "{\n"
            "float4 res = float4(0.0,0.0,0.0,0.0);\n"
            "float3 uv = float3(0, 1, 1);\n"
            "res += UNITY_SAMPLE_TEX2D(tex0, uv.xy);\n"
            "res += UNITY_SAMPLE_TEX2D_SAMPLER(tex1, tex0, uv.xy);\n"
            "res += UNITY_SAMPLE_TEXCUBE(tex2, uv.xyz);\n"
            "res += UNITY_SAMPLE_TEXCUBE_LOD(tex2, uv.xyz, 1.0f);\n"
            "res += UNITY_SAMPLE_TEXCUBE_SAMPLER(tex3, tex2, uv.xyz);\n"
            "res += UNITY_SAMPLE_TEXCUBE_SAMPLER_LOD(tex3, tex2, uv.xyz, 1.0f);\n"
            "res += UNITY_SAMPLE_TEX3D(tex4, uv.xyz);\n"
            "res += UNITY_SAMPLE_TEX3D_LOD(tex4, uv.xyz, 1.0f);\n"
            "res += UNITY_SAMPLE_TEX3D_SAMPLER(tex5, tex4, uv.xyz);\n"
            "res += UNITY_SAMPLE_TEX3D_SAMPLER_LOD(tex5, tex4, uv.xyz, 1.0f);\n"
            "res += UNITY_SAMPLE_TEX2DARRAY(tex6, uv.xyz);\n"
            "res += UNITY_SAMPLE_TEX2DARRAY_LOD(tex6, uv.xyz, 1.0f);\n"
            "res += UNITY_SAMPLE_TEX2DARRAY_SAMPLER(tex7, tex6, uv.xyz);\n"
            "res += UNITY_SAMPLE_TEX2DARRAY_SAMPLER_LOD(tex7, tex6, uv.xyz, 1.0f);\n"
            "return res;\n"
            "}\n";


        TestCompilationOk(s, platform, false);
    }

    const char* kInstancingShader =
        "#define INSTANCING_ON 1\n"
        "#include \"UnityCG.cginc\"\n"
        "#include \"HLSLSupport.cginc\"\n"
        "float4 vert() : POSITION { return float4(0.0, 0.0, 0.0, 1.0f); }\n"
        "\n"
        "UNITY_INSTANCING_BUFFER_START(Props)\n"
        "    UNITY_DEFINE_INSTANCED_PROP(float4x4, _Matrix)\n"
        "    #define _Matrix_arr Props\n"
        "    // from ono.unity3d.com/unity-extra/terrain-editor , project 'Instancing - Example'\n"
        "    UNITY_DEFINE_INSTANCED_PROP(fixed4, _Color)\t// Make _Color an instanced property (i.e. an array)\n"
        "    #define _Color_arr Props\n"
        "    UNITY_DEFINE_INSTANCED_PROP(fixed, _Color2)\t// Make _Color an instanced property (i.e. an array)\n"
        "    #define _Color2_arr Props\n"
        "UNITY_INSTANCING_BUFFER_END(Props)\n"
        "\n"
        "float4 frag() : SV_Target\n"
        "{\n"
        "    float4 res = float4(0.5,0.5,0.5,0.5);\n"
        "\n"
        "    res = UNITY_ACCESS_INSTANCED_PROP(_Color_arr, _Color);\n"
        "    res *= UNITY_ACCESS_INSTANCED_PROP(_Color2_arr, _Color2);\n"
        "    res = mul( UNITY_ACCESS_INSTANCED_PROP(_Matrix_arr, _Matrix) , res);\n"
        "    return res;\n"
        "}\n";

    PARAMETRIC_TEST_FIXTURE(ShaderCompilerTestFixture, ShaderCompiler_UnityInstancing_MacrosCompile, (ShaderCompilerPlatform platform), AllPlatforms)
    {
        inputData.programType = kProgramFragment;
        TestCompilationOk(kInstancingShader, platform);
    }

    PARAMETRIC_TEST_FIXTURE(ShaderCompilerTestFixture, ShaderCompiler_UnityInstancing_ReflectionReportsStructures, (ShaderCompilerPlatform platform), Switch_Only)
    {
        refl.m_ConstantBuffers.clear();
        inputData.programType = kProgramFragment;
        if (TestCompilationOk(kInstancingShader, kShaderCompPlatformSwitch))
        {
            CHECK_EQUAL(1, refl.m_ConstantBuffers.size());
            CHECK_EQUAL(4, refl.m_ConstantBuffers[0].m_Members.size());
            CHECK_EQUAL(true, refl.m_ConstantBuffers[0].m_Members[0].m_Struct);
        }
    }

    TEST_FIXTURE(ShaderCompilerTestFixture, Compute_TranslationPass)
    {
        const char *s =
            "#pragma kernel CSMain\n"
            "RWStructuredBuffer<int> buf;\n"
            "[numthreads(1,1,1)]\n"
            "void CSMain (uint2 id : SV_DispatchThreadID)\n"
            "{\n"
            "    buf[id.x] = id.x;\n"
            "}\n";

        TestComputeTranslationPassAllPlatforms(s);
    }

    TEST_FIXTURE(ShaderCompilerTestFixture, Compute_TranslationPassError)
    {
        const char *s =
            "#pragma kernel CSMain\n"
            "RWTexture2D<int> tex2D;\n"
            "[numthreads(1,1,1)]\n"
            "void CSMain (uint2 id : SV_DispatchThreadID)\n"
            "{\n"
            "    InterlockedAdd(tex2D[id], 1);\n"
            "}\n";
        // Atomic op on a texture is valid HLSL but not allowed in Metal
        // -> The first pass should be ok but the second pass should give out an error
        TestComputeTranslationPass(s, kShaderCompPlatformMetal, true);
    }

    PARAMETRIC_TEST_FIXTURE(ShaderCompilerTestFixture, ComputeShader_Includes_InRedirectedFolders_Work, (ShaderCompilerPlatform platform), ComputePlatforms)
    {
        const char* fn1 = "redirected/Include1.cginc";
        const char* fn2 = "IncludeRedirected.cginc";
        const char* fn3 = "redirected/Include2.cginc";
        CreateDirectoryAtPath("redirected");

        CHECK(WriteTextToFile(fn1, "float func1() { return 1; }\n"));
        CHECK(WriteTextToFile(fn2, "#include \"re/dir/Include2.cginc\"\n"));
        CHECK(WriteTextToFile(fn3, "float func2() { return 2; }\n"));

        const char *s =
            "#include \"re/dir/Include1.cginc\"\n"
            "#include \"IncludeRedirected.cginc\"\n"
            "#pragma kernel CSMain\n"
            "RWStructuredBuffer<float> buf;\n"
            "[numthreads(8,8,1)]\n"
            "void CSMain (uint2 id : SV_DispatchThreadID)\n"
            "{\n"
            "    buf[id.x] = func1() + func2();\n"
            "}\n";

        TestComputeCompilationOk(s, platform);

        DeleteFileAtPath(fn1);
        DeleteFileAtPath(fn2);
        DeleteFileAtPath(fn3);
        DeleteDirectoryAtPath("redirected");
    }

    PARAMETRIC_TEST_FIXTURE(ShaderCompilerTestFixture, HLSLcc_Compute_SampleCountQuery, (ShaderCompilerPlatform platform), OpenGLCore_Metal_Switch)
    {
        const char *s =
            "#pragma kernel CSMain\n"
            "RWStructuredBuffer<int> buf;\n"
            "Texture2DMS<float> tex2DMS;\n"
            "[numthreads(8,8,1)]\n"
            "void CSMain (uint2 id : SV_DispatchThreadID)\n"
            "{\n"
            "    int a, b, c;\n"
            "    tex2DMS.GetDimensions(a, b, c);\n"
            "    buf[id.x] = a+b+c;\n"
            "}\n";

        TestComputeCompilationOk(s, platform);
    }

    PARAMETRIC_TEST_FIXTURE(ShaderCompilerTestFixture, HLSLcc_Compute_ImageSizeQuery, (ShaderCompilerPlatform platform), OpenGLCore_Metal_Switch)
    {
        const char *s =
            "#pragma kernel CSMain\n"
            "RWStructuredBuffer<int> buf;\n"
            "RWTexture2D<float> tex2D;\n"
            "RWTexture3D<float> tex3D;\n"
            "[numthreads(8,8,1)]\n"
            "void CSMain (uint2 id : SV_DispatchThreadID)\n"
            "{\n"
            "    int a, b, d, c, e;\n"
            "    tex2D.GetDimensions(a, b);\n"
            "    tex3D.GetDimensions(c, d, e);\n"
            "    buf[id.x] = a+b+c+d+e;\n"
            "}\n";

        TestComputeCompilationOk(s, platform);
    }

    PARAMETRIC_TEST_FIXTURE(ShaderCompilerTestFixture, HLSLcc_Compute_GlobalBoolsTranslateCorrectly, (ShaderCompilerPlatform platform), OpenGLCore_Metal)
    {
        // The problem being checked here is that global bool parameters should be translated properly.
        // Rather than using the first global bool to appear for all references of a global.
        const char *s =
            "#pragma kernel CSMain\n"
            "bool    bParamTrue;\n"
            "bool    bParamFalse;\n"
            "RWStructuredBuffer<float> buf;\n"
            "[numthreads(1,1,1)]\n"
            "void CSMain (uint2 id : SV_DispatchThreadID)\n"
            "{\n"
            "    int i = 0;\n"
            "    buf[i++] = bParamTrue ? 3.0f : -1.0f;\n"
            "    buf[i++] = bParamFalse ? -1.0f : 4.0f;\n"
            "}\n";

        TestComputeCompilationOk(s, platform);

        // Copy to a string so that we can search.
        core::string outputString;
        AssignOutputString(outputString);

        // Check that the second param is present.
        if (platform == kShaderCompPlatformMetal)
            CHECK(outputString.find("Globals.bParamFalse != 0") != core::string::npos);
        else
            CHECK(outputString.find("(int(bParamFalse) != 0)") != core::string::npos);
    }

    TEST_FIXTURE(ShaderCompilerTestFixture, HLSLcc_Compute_MetalShadows)
    {
        const char *s =
            "#pragma kernel CSMain\n"
            "RWStructuredBuffer<float> buf;\n"
            "Texture2D_float _DepthTex;\n"
            "SamplerComparisonState sampler_DepthTex;\n"
            "[numthreads(1,1,1)]\n"
            "void CSMain (uint2 id : SV_DispatchThreadID)\n"
            "{\n"
            "    buf[0] =  _DepthTex.SampleCmpLevelZero(sampler_DepthTex, float2(0.5, 0.5), 0.5);\n"
            "}\n";

        TestComputeCompilationOk(s, kShaderCompPlatformMetal);

        // Copy to a string so that we can search.
        core::string outputString;
        AssignOutputString(outputString);

        CHECK_NOT_EQUAL(core::string::npos, outputString.find("filter::linear"));
        CHECK_EQUAL(core::string::npos, outputString.find("filter::nearest"));
    }

    TEST_FIXTURE(ShaderCompilerTestFixture, HLSLcc_Compute_MetalBFIWithGroupIndex)
    {
        const char *s =
            "#pragma kernel CSMain\n"
            "RWTexture2D<float4> Result;\n"
            "[numthreads(8,8,1)]\n"
            "void CSMain (uint3 id : SV_DispatchThreadID, uint gid : SV_GroupIndex)\n"
            "{\n"
            "\tuint2 width = uint2(3, 4);\n"
            "\tuint2 offset = uint2(4, 3);\n"
            "\tuint2 bitmask = (((uint2(1,1) << width) - uint2(1,1)) << offset) & 0xffffffff;\n"
            "\tuint2 dst = ((gid << offset) & bitmask) | (uint2(123, 456) & ~bitmask);\n"
            "\tResult[id.xy] = float4(dst.x, dst.y, 0, 0);\n"
            "}\n";

        TestComputeCompilationOk(s, kShaderCompPlatformMetal);

        core::string outputString;
        AssignOutputString(outputString);
        CHECK(outputString.find("bitFieldInsert(uint2(0x3u, 0x4u), uint2(0x4u, 0x3u), uint2(mtl_ThreadIndexInThreadGroup, mtl_ThreadIndexInThreadGroup), uint2(0xbu, 0x180u));") != core::string::npos);
    }

    PARAMETRIC_TEST_FIXTURE(ShaderCompilerTestFixture, HLSLcc_Compute_BitcastAndUpscale, (ShaderCompilerPlatform platform), OpenGLCore_Metal_Switch)
    {
        const char *s =
            "#pragma kernel ResultBufferToTex\n"
            "RWStructuredBuffer<float> resultBuffer;\n"
            "RWTexture2D<float> resultTex;\n"
            "uint resolutionX;\n"
            "float h;\n"
            "[numthreads(8,8,1)]\n"
            "void ResultBufferToTex (uint2 id : SV_DispatchThreadID)\n"
            "{\n"
            "    uint i = id.x + (id.y * resolutionX);\n"
            "    h = saturate(resultBuffer[i]);\n"
            "    resultTex[id] = h;\n"
            "    resultBuffer[i] = h;\n"
            "}\n";

        TestComputeCompilationOk(s, platform);
    }

    PARAMETRIC_TEST_FIXTURE(ShaderCompilerTestFixture, HLSLcc_Compute_ForSimplification, (ShaderCompilerPlatform platform), GLES3Plus_Metal_Vulkan_Switch)
    {
        const char *s =
            "#pragma kernel GenerateChunk\n"
            "\n"
            "struct voxel\n"
            "{\n"
            "\tfloat weight;\n"
            "\tuint type;\n"
            "\tuint id;\n"
            "\tint x;\n"
            "\tint y;\n"
            "\tint z;\n"
            "};\n"
            "\n"
            "struct vert\n"
            "{\n"
            "\tfloat3 pos;\n"
            "\tfloat3 normal;\n"
            "\tfloat2 uv;\n"
            "};\n"
            "\n"
            "voxel newVoxel(float weight, uint type, uint id, float3 xyz)\n"
            "{\n"
            "\tvoxel voxel;\n"
            "\tvoxel.id = id;\n"
            "\tvoxel.weight = weight;\n"
            "\tvoxel.type = type;\n"
            "\n"
            "\tvoxel.x = xyz.x;\n"
            "\tvoxel.y = xyz.y;\n"
            "\tvoxel.z = xyz.z;\n"
            "\treturn voxel;\n"
            "};\n"
            "\n"
            "vert newVert(float3 pos, float3 normal, float2 uv)\n"
            "{\n"
            "\tvert vert;\n"
            "\tvert.pos = pos;\n"
            "\tvert.normal = normal;\n"
            "\tvert.uv = uv;\n"
            "\treturn vert;\n"
            "};\n"
            "\n"
            "int ComputeIndex(int x, int y, int z, int3 max)\n"
            "{\n"
            "\t//todo: figure out how to compute indexes for voxels where x or y or z < 0\n"
            "\treturn x + max.x * (y + max.y * z);\n"
            "\t\n"
            "};\n"
            "\n"
            "int3 from;\n"
            "int3 to;\n"
            "\n"
            "// the voxel generation result buffer\n"
            "RWStructuredBuffer<voxel> voxels;\n"
            "\n"
            "// Test voxel gen from cpu implementation\n"
            "voxel GenerateVoxel (int x, int y, int z, int id, float3 xyz)\n"
            "{\n"
            "\tif ((x >= 1 && x < to.x - 1) &&"
            "        (y >= 1 && y < to.y - 1) &&"
            "            (z >= 1 && z < to.z - 1))\n"
            "\t{\n"
            "\t\t//TODO: consider testing various voxel types later\n"
            "     return newVoxel(1, 1, id, xyz);\n"
            "\t} else\n"
            "\treturn newVoxel(-1, 0, id, xyz);\n"
            "}\n"
            "\n"
            "[numthreads(1,1,1)]\n"
            "void GenerateChunk (uint3 groupThread : SV_GroupThreadID, uint3 group : SV_GroupID)\n"
            "{\n"
            "\t// determine generation loop start and end\n"
            "\t// TODO: should probably not do it this way, instead pass in a chunk \n"
            "\t// size and each thread generate chuynks based on that size\n"
            "\tint3 chunkStart;\n"
            "\tint3 chunkEnd;\n"
            "\n"
            "\tchunkStart = group * float3(8,8,8) + groupThread + from;\n"
            "\tchunkEnd = int3(chunkStart.x + 8, chunkStart.y + 8, chunkStart.z + 8);\n"
            "\n"
            "\t// loop through this portion of the workload\n"
            "\tfor(int x = chunkStart.x; x < chunkEnd.x; x++)\n"
            "\t{\n"
            "\t\tfor(int y = chunkStart.y; y < chunkEnd.y; y++)\n"
            "\t\t{\n"
            "\t\t\tfor(int z = chunkStart.z; z < chunkEnd.z; z++)\n"
            "\t\t\t{\n"
            "\t\t\t\tint i = ComputeIndex(x,y,z, to);\n"
            "\t\t\t\tvoxels[i] = GenerateVoxel(x,y,z,i,float3(x,y,z));\n"
            "\t\t\t}\n"
            "\t\t}\n"
            "\t}\n"
            "}\n";

        TestComputeCompilationOk(s, platform);
    }

    PARAMETRIC_TEST_FIXTURE(ShaderCompilerTestFixture, HLSLcc_Compute_GroupShared, (ShaderCompilerPlatform platform), GLES3Plus_Metal_Switch)
    {
        const char *s =
            "groupshared uint gs[8];\n"
            "StructuredBuffer<int> inBuf;\n"
            "RWStructuredBuffer<int> ignoredOut;\n"
            "#pragma kernel CSMain\n"
            "[numthreads(1,1,1)]\n"
            "void CSMain (uint3 id : SV_DispatchThreadID, RWStructuredBuffer<int> buf)\n"
            "{\n"
            "\tint a, i;\n"
            "\n"
            "\t//init our texture data from int buf\n"
            "\tfor (i = 0; i < 8; i++)\n"
            "\t{\n"
            "\t\tgs[i] = buf[i];\n"
            "\t}\n"
            "\n"
            "\tInterlockedAdd(gs[id.x], 1);\n"
            "\n"
            "\tInterlockedAdd(gs[1], 1);\n"
            "\n"
            "\tInterlockedAdd(gs[id.x + 2], 1, a);\n"
            "\tignoredOut[0] = a;\n"
            "\n"
            "\tInterlockedAdd(gs[3], 1, a);\n"
            "\tignoredOut[1] = a;\n"
            "\n"
            "\tInterlockedAdd(gs[id.x + 4], inBuf[0]);\n"
            "\n"
            "\tInterlockedAdd(gs[5], inBuf[0]);\n"
            "\n"
            "\tInterlockedAdd(gs[id.x + 6], inBuf[0], a);\n"
            "\tignoredOut[2] = a;\n"
            "\n"
            "\tInterlockedAdd(gs[7], inBuf[0], a);\n"
            "\tignoredOut[3] = a;\n"
            "\n"
            "\tfor (i = 0; i < 8; i++)\n"
            "\t{\n"
            "\t\tbuf[i] = gs[i];\n"
            "\t}\n"
            "}\n"
            "\n";

        bool treatWarningsAsErrors = (platform != kShaderCompPlatformSwitch); // Switch's "GLSLc" compiler (version.package 59) will report "[temp variable] might be used before being initialized" warnings: this is expected (and might be fixed in the future, cf. https://unity.slack.com/archives/G3JUQKYV8/p1531381560000009)
        TestComputeCompilationOk(s, platform, kDoNotExpectFail, treatWarningsAsErrors);
    }

    PARAMETRIC_TEST_FIXTURE(ShaderCompilerTestFixture, HLSLcc_Compute_AppendConsume, (ShaderCompilerPlatform platform), GLES3Plus_OpenGLCore_Metal_Vulkan_Switch)
    {
        const char *s =
            "#pragma kernel CSMain\n"
            "#include \"UnityCG.cginc\"\n"
            "Texture2D_half Input : register(t2);\n"
            "struct Particle {\n"
            "\tfloat3 pos;\n"
            "\tfloat3 dir;\n"
            "};\n"

            "AppendStructuredBuffer<Particle> bufDst;\n"
            "ConsumeStructuredBuffer<Particle> bufSrc;\n"

            "[numthreads(1, 1, 1)]\n"
            "void CSMain(uint3 id : SV_DispatchThreadID)\n"
            "{\n"
            "\tif (Input[id.xy].x < -1.0)\n"
            "\t{\n"
            "\t\tParticle p = bufSrc.Consume();\n"
            "\t\tp.pos.xy += Input[id.xy].xy;\n"
            "\t\tbufDst.Append(p);\n"
            "\t}\n"
            "}\n";

        TestComputeCompilationOk(s, platform);
    }

    PARAMETRIC_TEST_FIXTURE(ShaderCompilerTestFixture, HLSLcc_Compute_AppendData, (ShaderCompilerPlatform platform), GLES3Plus_Metal_Switch)
    {
        const char *s =
            "#pragma kernel CSMain\n"
            "struct Item {\n"
            "\tfloat4 data;\n"
            "};\n"

            "AppendStructuredBuffer<Item> bufDst;\n"

            "[numthreads(1, 1, 1)]\n"
            "void CSMain(uint3 id : SV_DispatchThreadID)\n"
            "{\n"
            "\tItem i;\n"
            "\ti.data = float4(id.x, id.y, id.z, 0.5);\n"
            "\tbufDst.Append(i);\n"
            "}\n";

        TestComputeCompilationOk(s, platform);
    }

    PARAMETRIC_TEST_FIXTURE(ShaderCompilerTestFixture, HLSLcc_Compute_ConsumeData, (ShaderCompilerPlatform platform), GLES3Plus_Metal_Switch)
    {
        const char *s =
            "#pragma kernel CSMain\n"
            "RWTexture2D<float4> Result;\n"
            "struct Item {\n"
            "\tfloat4 data;\n"
            "};\n"

            "ConsumeStructuredBuffer<Item> bufSrc;\n"

            "[numthreads(1, 1, 1)]\n"
            "void CSMain(uint3 id : SV_DispatchThreadID)\n"
            "{\n"
            "\tItem i = bufSrc.Consume();\n"
            "\tif (i.data.w == 0.5)\n"
            "\t{\n"
            "\t\tResult[id.xy] = float4(id.x/64.0, id.y/64.0, id.z/64.0, 0.5);\n"
            "\t}\n"
            "}\n";

        TestComputeCompilationOk(s, platform);
    }

    PARAMETRIC_TEST_FIXTURE(ShaderCompilerTestFixture, HLSLcc_Compute_ThreadIdSinCos, (ShaderCompilerPlatform platform), GLES3Plus_Metal_Switch)
    {
        const char *s =
            "#pragma kernel CSMain\n"
            "RWTexture3D<float4> Result;\n"
            "float4 g_Params;\n"

            "[numthreads(8, 8, 8)]\n"
            "void CSMain(uint3 id : SV_DispatchThreadID)\n"
            "{\n"
            "\tfloat4 c;\n"
            "\tc.r = (id.x & id.y & id.z) ? 1.0 : 0.0;\n"
            "\tc.g = sin((id.y * g_Params.z - g_Params.x) * 3.0) * 0.5 + 0.5;\n"
            "\tc.b = id.z * g_Params.z;\n"
            "\tc.a = 1.0;\n"
            "\tResult[id] = c;\n"
            "}\n";

        TestComputeCompilationOk(s, platform);
    }

    TEST_FIXTURE(ShaderCompilerTestFixture, HLSLcc_Compute_ThreadIdSinCos_disable_fastmath)
    {
        const char *s =
            "#pragma kernel CSMain\n"
            "#pragma disable_fastmath\n"
            "RWTexture3D<float4> Result;\n"
            "float4 g_Params;\n"

            "[numthreads(8, 8, 8)]\n"
            "void CSMain(uint3 id : SV_DispatchThreadID)\n"
            "{\n"
            "\tfloat4 c;\n"
            "\tc.r = (id.x & id.y & id.z) ? 1.0 : 0.0;\n"
            "\tc.g = sin((id.y * g_Params.z - g_Params.x) * 3.0) * 0.5 + 0.5;\n"
            "\tc.b = id.z * g_Params.z;\n"
            "\tc.a = 1.0;\n"
            "\tResult[id] = c;\n"
            "}\n";

        TestComputeCompilationOk(s, kShaderCompPlatformMetal);

        core::string outputString = GetOutputString(true);
        CHECK(outputString.find("UNITY_DISABLE_FASTMATH") != core::string::npos);
    }

    PARAMETRIC_TEST_FIXTURE(ShaderCompilerTestFixture, HLSLcc_Compute_ThreadIdTypes, (ShaderCompilerPlatform platform), GLES3Plus_Metal_Switch)
    {
        const char *s =
            "#pragma kernel CSMain\n"
            "RWTexture2D<float4> Result1;\n"
            "RWTexture2D<float4> Result2;\n"
            "RWTexture2D<float4> Result3;\n"

            "[numthreads(8, 8, 8)]\n"
            "void CSMain( uint3 Gid : SV_GroupID, uint3 DTid : SV_DispatchThreadID, uint3 GTid : SV_GroupThreadID, uint GI : SV_GroupIndex )\n"
            "{\n"
            "\tResult1[uint2(GI,0)] = float4(float3(Gid), 0.0);\n"
            "\tResult2[uint2(GI,0)] = float4(float3(DTid), 1.0);\n"
            "\tResult3[uint2(GI,0)] = float4(float3(GTid), 2.0);\n"
            "}\n";

        TestComputeCompilationOk(s, platform);
    }

    PARAMETRIC_TEST_FIXTURE(ShaderCompilerTestFixture, HLSLcc_Compute_LoadAlign1, (ShaderCompilerPlatform platform), GLES3Plus_Metal_Switch)
    {
        const char *s =
            "#pragma kernel CSMain\n"
            "struct layout2 {\n"
            "\tfloat2 r;\n"
            "\tfloat3 g;\n"
            "\tfloat4 b;\n"
            "\tfloat1 a; };\n"
            "RWStructuredBuffer<layout2> buf2;\n"
            "RWTexture2D<float4> Result;\n"

            "[numthreads(1, 1, 1)]\n"
            "void CSMain( uint3 id : SV_DispatchThreadID ) \n"
            "{ \n"
            "\tfloat4 color = float4(1, 1, 1, 1); \n"
            "\tint index = id.x + id.y * 2; \n"
            "\tcolor.a = buf2[index].a.x; \n"
            "\tResult[id.xy] = color; \n"
            "}\n";

        TestComputeCompilationOk(s, platform);
    }

    PARAMETRIC_TEST_FIXTURE(ShaderCompilerTestFixture, HLSLcc_Compute_RWByteAddressBufferStore, (ShaderCompilerPlatform platform), GLES3Plus_Metal_Switch)
    {
        const char *s =
            "#pragma kernel CSMain\n"
            "struct layout1 {\n"
            "\tfloat1 r;\n"
            "\tfloat2 g;\n"
            "\tfloat3 b;\n"
            "\tfloat4 a; };\n"
            "RWStructuredBuffer<layout1> buf1;\n"
            "RWByteAddressBuffer byteBuf;\n"

            "[numthreads(1, 1, 1)]\n"
            "void CSMain( uint3 id : SV_DispatchThreadID ) \n"
            "{ \n"
            "\tlayout1 b1 = buf1[id.x];\n"
            "\tuint address = id.x * 152;\n"
            "\tbyteBuf.Store(address, (uint)asuint(b1.r));\n"
            "\taddress += 4;\n"
            "\tbyteBuf.Store2(address, (uint2)asuint(b1.g));\n"
            "\taddress += 8;\n"
            "\tbyteBuf.Store3(address, (uint3)asuint(b1.b));\n"
            "\taddress += 12;\n"
            "\tbyteBuf.Store4(address, (uint4)asuint(b1.a));\n"
            "\taddress += 16; \n"
            "}\n";

        TestComputeCompilationOk(s, platform);
    }

    PARAMETRIC_TEST_FIXTURE(ShaderCompilerTestFixture, HLSLcc_Compute_RWByteAddressBufferLoad, (ShaderCompilerPlatform platform), GLES3Plus_Metal_Switch)
    {
        const char *s =
            "#pragma kernel CSMain\n"
            "RWByteAddressBuffer byteBuf;\n"
            "RWTexture2D<float4> Result;\n"

            "[numthreads(1, 1, 1)]\n"
            "void CSMain( uint3 id : SV_DispatchThreadID ) \n"
            "{ \n"
            "\tfloat4 rawColor = float4(0,0,0,1);\n"
            "\tuint address = 0;\n"
            "\trawColor = (float)asfloat(byteBuf.Load(address));\n"
            "\tResult[id.xy + uint2(4, 4)] = rawColor;\n"
            "}\n";

        TestComputeCompilationOk(s, platform);
    }

    PARAMETRIC_TEST_FIXTURE(ShaderCompilerTestFixture, HLSLcc_Compute_ByteAddressBufferLoad, (ShaderCompilerPlatform platform), GLES3Plus_Metal_Switch)
    {
        const char *s =
            "#pragma kernel CSMain\n"
            "ByteAddressBuffer byteBuf;\n"
            "RWTexture2D<float4> Result;\n"

            "[numthreads(1, 1, 1)]\n"
            "void CSMain( uint3 id : SV_DispatchThreadID ) \n"
            "{ \n"
            "\tfloat4 rawColor = float4(0,0,0,1);\n"
            "\tuint address = 0;\n"
            "\trawColor = (float)asfloat(byteBuf.Load(address));\n"
            "\tResult[id.xy] = rawColor;\n"
            "}\n";

        TestComputeCompilationOk(s, platform);
    }

    PARAMETRIC_TEST_FIXTURE(ShaderCompilerTestFixture, HLSLcc_Compute_AtomicMemOps, (ShaderCompilerPlatform platform), GLES3Plus_Metal_Switch)
    {
        const char *s =
            "#pragma kernel CSMain\n"
            "struct nester {\n"
            "\tint count; };\n"
            "struct test {\n"
            "\tfloat1 foo;\n"
            "\tfloat2 bar;\n"
            "\tnester nest;\n"
            "\tint counter; };\n"
            "RWStructuredBuffer<int> intBuf;\n"
            "RWStructuredBuffer<test> structBuf;\n"
            "groupshared test gs[2];"

            "[numthreads(1, 1, 1)]\n"
            "void CSMain( uint3 id : SV_DispatchThreadID ) \n"
            "{ \n"
            "\tint intVal;\n"
            "\tint structVal;\n"
            "\tint gsVal;\n"
            "\tgs[0].foo = 0; gs[0].bar = float2(0,0); gs[0].nest.count = 0; gs[0].counter = 0; gs[1] = gs[0];\n"
            "\tGroupMemoryBarrierWithGroupSync();\n"
            "\tInterlockedAdd(structBuf[id.x].counter, 1, structVal);\n"
            "\tInterlockedAdd(gs[id.x].nest.count, 2, gsVal); \n"
            "\tInterlockedAdd(intBuf[id.y], gsVal, intVal);\n"
            "\tInterlockedAdd(structBuf[id.x].nest.count, 4, structVal);\n"
            "}\n";

        TestComputeCompilationOk(s, platform);
    }

    PARAMETRIC_TEST_FIXTURE(ShaderCompilerTestFixture, HLSLcc_Compute_AtomicMemOps2, (ShaderCompilerPlatform platform), GLES3Plus_Metal_Switch)
    {
        const char *s =
            "#pragma kernel CSMain\n"
            "RWStructuredBuffer<int> index;\n"
            "groupshared uint gs[2];\n"
            "[numthreads(1, 1, 1)]\n"
            "void CSMain (uint id : SV_DispatchThreadID, RWStructuredBuffer<int> buf, RWTexture2D<int> tex)\n"
            "{\n"
            "\tgs[0]=0; gs[1]=0;\n"
            "\tGroupMemoryBarrierWithGroupSync();\n"
            "\tint i = index[id];\n"
            "\tInterlockedMin (buf[128], 6);\n"
            "\tInterlockedMin (tex[uint2(128,0)], 6);\n"
            "\tInterlockedMin (tex[uint2(i,0)], 6);\n"
            "\tInterlockedMin (gs[i], 6u);\n"
            "\tInterlockedMin (gs[0], 6u);\n"
            "\ttex[uint2(i+1,0)] = (int)gs[i];\n"
            "}\n";

        TestComputeCompilationOk(s, platform, platform == kShaderCompPlatformMetal); // Expect to fail on Metal as there are no texture atomics
    }

    PARAMETRIC_TEST_FIXTURE(ShaderCompilerTestFixture, HLSLcc_Compute_AtomicMemOps3, (ShaderCompilerPlatform platform), GLES3Plus_Metal_Switch)
    {
        const char *s =
            "#pragma kernel CSMain\n"
            "RWStructuredBuffer<int> index;\n"
            "groupshared uint gs[2];\n"
            "[numthreads(1, 1, 1)]\n"
            "void CSMain (uint id : SV_DispatchThreadID, RWStructuredBuffer<int> buf, RWTexture2D<int> tex)\n"
            "{\n"
            "\tgs[0]=0; gs[1]=0;\n"
            "\tGroupMemoryBarrierWithGroupSync();\n"
            "\tint i = index[id];\n"
            "\tInterlockedMin (buf[128], 6);\n"
            "\tInterlockedMin (gs[i], 6u);\n"
            "\tInterlockedMin (gs[0], 6u);\n"
            "\ttex[uint2(i+1,0)] = (int)gs[i];\n"
            "}\n";

        TestComputeCompilationOk(s, platform);
    }

    PARAMETRIC_TEST_FIXTURE(ShaderCompilerTestFixture, HLSLcc_Compute_AtomicMemOps4, (ShaderCompilerPlatform platform), GLES3Plus_Metal_Switch)
    {
        const char *s =
            "#pragma kernel CSMain\n"
            "[numthreads(1, 1, 1)]\n"
            "void CSMain (uint id : SV_DispatchThreadID, RWTexture2D<int> tex)\n"
            "{\n"
            "\tInterlockedMin (tex[uint2(128u, 64u)], 6);\n"
            "\tInterlockedMin (tex[uint2(id, id + 1u)], 6);\n"
            "}\n";

        TestComputeCompilationOk(s, platform, platform == kShaderCompPlatformMetal); // Expected to fail on Metal
    }

    PARAMETRIC_TEST_FIXTURE(ShaderCompilerTestFixture, HLSLcc_Compute_AtomicMemOps5, (ShaderCompilerPlatform platform), GLES3Plus_Metal_Switch)
    {
        const char *s =
            "#pragma kernel CSMain\n"
            "struct test {\n"
            "\tfloat1 foo;\n"
            "\tfloat2 bar;\n"
            "\tint3 counter; };\n"
            "RWStructuredBuffer<test> sbuf;"
            "[numthreads(1, 1, 1)]\n"
            "void CSMain (uint id : SV_DispatchThreadID, RWStructuredBuffer<uint4> buf)\n"
            "{\n"
            "\tInterlockedAdd (buf[id].x, 1);\n"
            "\tInterlockedAdd (buf[id].z, 2);\n"
            "\tInterlockedAdd (buf[id][id], 3);\n"
            "\tInterlockedAdd (sbuf[id].counter[id], 4);\n"
            "}\n";

        TestComputeCompilationOk(s, platform);
    }

    PARAMETRIC_TEST_FIXTURE(ShaderCompilerTestFixture, HLSLcc_Compute_AtomicMemOps6, (ShaderCompilerPlatform platform), GLES3Plus_Metal_Switch)
    {
        const char *s =
            "#pragma kernel CSMain\n"
            "struct test {\n"
            "\tfloat foo;\n"
            "\tfloat2 bar;\n"
            "\tint counter; };\n"
            "RWStructuredBuffer<test> sbuf;"
            "[numthreads(1, 1, 1)]\n"
            "void CSMain (uint id : SV_DispatchThreadID, RWStructuredBuffer<uint> buf)\n"
            "{\n"
            "\tInterlockedAdd (buf[id*4+0], 1);\n"
            "\tInterlockedAdd (buf[id*4+1], 2);\n"
            "\tInterlockedAdd (buf[id*4 + id], 3);\n"
            "\tInterlockedAdd (sbuf[id].counter, 4);\n"
            "}\n";

        TestComputeCompilationOk(s, platform);
    }

    PARAMETRIC_TEST_FIXTURE(ShaderCompilerTestFixture, HLSLcc_Compute_AtomicMinConservesRWInt, (ShaderCompilerPlatform platform), Switch_Only)
    {
        const char *s =
            "#pragma kernel Find\n"
            "RWStructuredBuffer<int4> _MyBuffer;\n"
            "[numthreads(8, 1, 1)]\n"
            "void Find()\n"
            "{\n"
            "    InterlockedMin(_MyBuffer[0].x, _MyBuffer[0].y);\n"
            "}\n";

        if (platform == kShaderCompPlatformSwitch)
        {
            // check that RWStructuredBuffer<int4> translates to int[4] and not uint[4]

            // extracted from TestComputeCompilationOk:
            core::string source(s);
            ComputeImportDirectives importDirectives;
            UInt32 supportedAPIs = 0;
            ParseComputeImportDirectives(source, importDirectives, supportedAPIs);
            bool hadErrors = false;

            for (int i = 0; i < importDirectives.kernels.size(); i++)
            {
                ComputeImportDirectives::Kernel kernel = importDirectives.kernels[i];

                InitReflectionReport(platform);
                inputData.source = source;
                inputData.sourceFilename = "compute shader";
                inputData.api = platform;
                inputData.compileFlags |= importDirectives.compileFlags;
                inputData.p.entryName[0] = kernel.name;
                inputData.args.clear();
                for (int m = 0; m < kernel.macros.size(); m++)
                    inputData.args.push_back(ShaderCompileArg(kernel.macros[m].first, kernel.macros[m].second));

                // extracted from InvokeComputeCompilerSwitch in PlatformDependent\Switch\CgBatch\PlatformPlugin\Compiler.cpp:
                ShaderCompileInputData localInput;
                localInput.CopyFrom(&inputData);
                localInput.source = "#define SHADER_API_SWITCH 1\n" + localInput.source;
                localInput.api = kShaderCompPlatformOpenGLCore;
                //localInput.target = kShaderTarget50;
                localInput.compileFlags |= kShaderCompFlags_SwitchPlatform;
                localInput.compileFlags |= kShaderCompFlags_EmbedHLSLccDisassembly;
                localInput.compileFlags |= kShaderCompFlags_EmbedHLSLccPreprocessedSource;
                const ShaderCompileInputData* input = &localInput;
                ShaderCompileOutputData* localOutput = (ShaderCompileOutputData*)InvokeComputeCompilerHLSLcc(input);
                if (localOutput->errors.HasErrors())
                    DEBUG_BREAK;
                ComputeShaderBinary glslCompute;
                dynamic_array<UInt8> inputBuffer;
                const UInt8* outputPointer = localOutput->GetCompiledDataPtr();
                inputBuffer.assign(outputPointer, outputPointer + localOutput->GetCompiledDataSize());
                glslCompute.ReadFromBuffer(inputBuffer, kMemShader);
                if (glslCompute.m_Kernels.empty())
                    DEBUG_BREAK;

                core::string generatedGlslSource((const char*)glslCompute.m_Kernels.begin()->code.begin());
#if 0       // Debug dump
                FILE* f = fopen("generated.glsl", "wb");
                fputs(generatedGlslSource.c_str(), f);
                fclose(f);
                //TestComputeCompilationOk(s, platform);
#endif
                // split lines https://stackoverflow.com/a/13172514/758666
                dynamic_array<core::string> lines;
                core::string::size_type pos = 0;
                core::string::size_type prev = 0;
                while ((pos = generatedGlslSource.find("\n", prev)) != std::string::npos)
                {
                    lines.push_back(generatedGlslSource.substr(prev, pos - prev));
                    prev = pos + 1;
                }
                lines.push_back(generatedGlslSource.substr(prev));

                // We need to avoid "uint" in generated glsl code, i.e:
                // instead of generating: atomicMin(      _MyBuffer_buf[int(0)].value[int(0) >> 2]   ,  uint( u_xlati0 )  );
                // we should generate:    atomicMin( int( _MyBuffer_buf[int(0)].value[int(0) >> 2] ) ,        u_xlati0    );
#if DEBUGMODE
                const int errorLine = 32;   // in debug mode HLSLcc inserts instruction opcode comments in the generated glsl
#else
                const int errorLine = 30;
#endif
                core::string::size_type uintPos = lines[errorLine].find("uint");
                CHECK_EQUAL(uintPos, core::string::npos);
            }
        }
        else
        {
            TestComputeTranslationPass(s, platform);
        }
    }

    PARAMETRIC_TEST_FIXTURE(ShaderCompilerTestFixture, HLSLcc_Compute_RWStructuredBuffer_OfInt, (ShaderCompilerPlatform platform), GLES3Plus_Switch)
    {
        const char *s =
            "#pragma kernel CSMain\n"
            "StructuredBuffer<float4> src;\n"
            "RWStructuredBuffer<int4> dst;\n"
            "[numthreads(1, 1, 1)]\n"
            "void CSMain()\n"
            "{\n"
            "    dst[0] = int4(floor(src[0]));\n"
            "}\n";

        TestComputeCompilationOk(s, platform);
    }

    PARAMETRIC_TEST_FIXTURE(ShaderCompilerTestFixture, HLSLcc_Compute_GroupsharedRaw, (ShaderCompilerPlatform platform), GLES3Plus_Metal_Switch)
    {
        const char *s =
            "#pragma kernel CSMain\n"
            "RWStructuredBuffer<float3> buf;\n"
            "groupshared float3 foo;\n"

            "[numthreads(16, 1, 1)]\n"
            "void CSMain( uint tid : SV_GroupIndex ) \n"
            "{ \n"
            "\tif (tid == 0) {\n"
            "\t\tfoo = float3(0, 0, 0);\n"
            "\t\tfor(int i=0; i<16; i++) { foo += buf[i]; }\n"
            "\t}\n"
            "\tGroupMemoryBarrierWithGroupSync();\n"
            "\tbuf[tid] = foo;\n"
            "}\n";

        bool treatWarningsAsErrors = (platform != kShaderCompPlatformSwitch); // Switch's "GLSLc" compiler (version.package 59) will report "[temp variable] might be used before being initialized" warnings: this is expected (and might be fixed in the future, cf. https://unity.slack.com/archives/G3JUQKYV8/p1531381560000009)
        TestComputeCompilationOk(s, platform, kDoNotExpectFail, treatWarningsAsErrors);
    }

    PARAMETRIC_TEST_FIXTURE(ShaderCompilerTestFixture, HLSLcc_Compute_AtomicOnRaw, (ShaderCompilerPlatform platform), GLES3Plus_Metal_Switch)
    {
        const char *s =
            "#pragma kernel CSMain\n"
            "RWStructuredBuffer<uint3> buf;\n"
            "groupshared uint3 foo;\n"

            "[numthreads(16, 1, 1)]\n"
            "void CSMain( uint tid : SV_GroupIndex ) \n"
            "{ \n"
            "\tif (tid == 0) {\n"
            "\t\tfoo = uint3(0, 0, 0);\n"
            "\t\tfor(int i=0; i<16; i++) { \n"
            "\t\tInterlockedAdd (foo.y, buf[i].x); }\n"
            "\t}\n"
            "\tGroupMemoryBarrierWithGroupSync();\n"
            "\tbuf[tid] = foo;\n"
            "}\n";

        bool treatWarningsAsErrors = (platform != kShaderCompPlatformSwitch); // Switch's "GLSLc" compiler (version.package 59) will report "[temp variable] might be used before being initialized" warnings: this is expected (and might be fixed in the future, cf. https://unity.slack.com/archives/G3JUQKYV8/p1531381560000009)
        TestComputeCompilationOk(s, platform, kDoNotExpectFail, treatWarningsAsErrors);
    }

    PARAMETRIC_TEST_FIXTURE(ShaderCompilerTestFixture, HLSLcc_Compute_MulSmallFloat, (ShaderCompilerPlatform platform), GLES3Plus_Metal_Switch)
    {
        const char *s =
            "#pragma kernel CSMain\n"
            "RWStructuredBuffer<float> buf;\n"

            "[numthreads(1, 1, 1)]\n"
            "void CSMain( uint3 id : SV_DispatchThreadID ) \n"
            "{ \n"
            "\tfloat temp = buf[id.x];\n"
            "\tfloat temp2 = temp * 1.0f / 4294967295.0f;\n"
            "\tbuf[id.y] = temp2;\n"
            "}\n";

        TestComputeCompilationOk(s, platform);
    }

    PARAMETRIC_TEST_FIXTURE(ShaderCompilerTestFixture, HLSLcc_Compute_ResourceDimensions, (ShaderCompilerPlatform platform), GLES3Plus_Metal_Switch)
    {
        const char *s =
            "#pragma kernel CSMain\n"
            "RWTexture2D<float4> img;\n"
            "Texture3D<float4> tex;\n"

            "[numthreads(8, 8, 1)]\n"
            "void CSMain( uint2 id : SV_DispatchThreadID ) \n"
            "{ \n"
            "\tfloat tw, th, td, iw, ih;\n"
            "\timg.GetDimensions(iw, ih);"
            "\ttex.GetDimensions(tw, th, td);\n"
            "\timg[id] = float4(tw/iw, th/ih, td, 1.0);\n"
            "}\n";

        TestComputeCompilationOk(s, platform);
    }

    PARAMETRIC_TEST_FIXTURE(ShaderCompilerTestFixture, HLSLcc_Compute_ResourceDimension_HasNoSwizzle, (ShaderCompilerPlatform platform), GLES3Plus_Metal_Switch)
    {
        const char *s =
            "#pragma kernel CSMain\n"

            "[numthreads(8, 8, 1)]\n"
            "void CSMain(uint2 id : SV_DispatchThreadID, RWTexture2D<uint> layers) \n"
            "{ \n"
            "\tuint l = (uint)layers[id];\n"
            "\tuint l2 = 0;\n"
            "\tuint2 dim;\n"
            "\tlayers.GetDimensions(dim.x, dim.y);\n"
            "\tfloat2 p = float2(dim);\n"
            "\tif (p.x*p.y > 1) l2 = l;\n"
            "\telse l2 = l + 1;\n"
            "\tlayers[id] = (int)l2; \n"
            "}\n";

        TestComputeCompilationOk(s, platform);

        if (platform == kShaderCompPlatformGLES3Plus)
        {
            // imageSize func argument should never use swizzles
            core::string outputString = GetOutputString(true);
            CHECK(outputString.find("imageSize(layers.") == core::string::npos);
        }
    }

    PARAMETRIC_TEST_FIXTURE(ShaderCompilerTestFixture, HLSLcc_Compute_TypedUAVLoadTypeResolving_DoesNotCrash, (ShaderCompilerPlatform platform), GLES3Plus_Metal_Switch)
    {
        const char *s =
            "#pragma kernel CSMain\n"

            "[numthreads(8, 8, 1)]\n"
            "void CSMain(uint2 id : SV_DispatchThreadID, RWTexture2D<uint> layers) \n"
            "{ \n"
            "\tuint l = (uint)layers[id];\n"
            "\tuint l2 = 0;\n"
            "\tif (l2 != l) layers[id] = (int)l2;\n"
            "}\n";

        TestComputeCompilationOk(s, platform);
    }

    PARAMETRIC_TEST_FIXTURE(ShaderCompilerTestFixture, HLSLcc_Compute_TexelFetchSwizzle, (ShaderCompilerPlatform platform), GLES3Plus_Metal_Switch)
    {
        const char *s =
            "#pragma kernel CSMain\n"
            "RWTexture2D<float4> img;\n"
            "Texture2D<float4> tex;\n"

            "[numthreads(8, 8, 1)]\n"
            "void CSMain( uint2 id : SV_DispatchThreadID ) \n"
            "{ \n"
            "\tfloat x, y, z;\n"
            "\tx = tex[id].z;\n"
            "\ty = tex[id+1].y;\n"
            "\tz = tex[id+2].x;\n"
            "\timg[id] = float4(x, y, z, 1.0);\n"
            "}\n";

        TestComputeCompilationOk(s, platform);
    }

    PARAMETRIC_TEST_FIXTURE(ShaderCompilerTestFixture, HLSLcc_Compute_StructuredWithDynamicComponent, (ShaderCompilerPlatform platform), GLES3Plus_Metal_Switch)
    {
        const char *s =
            "#pragma kernel CSMain\n"
            "StructuredBuffer<uint4> u4buf;\n"
            "RWStructuredBuffer<float4> f4buf;\n"
            "struct test {\n"
            "\tfloat1 foo;\n"
            "\tfloat2 bar;\n"
            "\tint3 counter; };\n"
            "RWStructuredBuffer<test> sbuf;"

            "[numthreads(1, 1, 1)]\n"
            "void CSMain( uint2 id : SV_DispatchThreadID ) \n"
            "{ \n"
            "\tfloat foo = u4buf[id.x][id.y];\n"
            "\tf4buf[id.x][id.y] = foo;\n"
            "\tsbuf[id.x].counter[id.y] = sbuf[id.y].counter[id.x];"
            "}\n";

        TestComputeCompilationOk(s, platform);
    }

    PARAMETRIC_TEST_FIXTURE(ShaderCompilerTestFixture, HLSLcc_Compute_GlobalBool, (ShaderCompilerPlatform platform), GLES3Plus_Metal_Switch)
    {
        const char *s =
            "#pragma kernel CSMain\n"
            "bool g_Bool;\n"
            "RWStructuredBuffer<float> buf;\n"

            "[numthreads(1, 1, 1)]\n"
            "void CSMain( uint3 id : SV_DispatchThreadID ) \n"
            "{ \n"
            "\tbuf[id.y] = g_Bool ? 1.0 : 0.0;\n"
            "}\n";

        TestComputeCompilationOk(s, platform);
    }

    PARAMETRIC_TEST_FIXTURE(ShaderCompilerTestFixture, HLSLcc_Compute_GlobalStructBool, (ShaderCompilerPlatform platform), GLES3Plus_Metal_Switch)
    {
        const char *s =
            "#pragma kernel CSMain\n"
            "struct test {\n"
            "\tbool b;\n"
            "};\n"
            "bool g_Bool;\n"
            "RWStructuredBuffer<float> buf;\n"

            "[numthreads(1, 1, 1)]\n"
            "void CSMain( uint3 id : SV_DispatchThreadID ) \n"
            "{ \n"
            "\ttest s;\n"
            "\ts.b = g_Bool;\n"
            "\tbuf[id.y] = s.b ? 1.0 : 0.0;\n"
            "}\n";

        TestComputeCompilationOk(s, platform);
    }

    PARAMETRIC_TEST_FIXTURE(ShaderCompilerTestFixture, HLSLcc_Compute_InRWStructuredBuffer_Bool, (ShaderCompilerPlatform platform), GLES3Plus_Metal_Switch)
    {
        const char *s =
            "#pragma kernel CSMain\n"
            "struct test {\n"
            "\tbool b;\n"
            "};\n"
            "RWStructuredBuffer<test> bols;\n"
            "RWStructuredBuffer<float> buf;\n"

            "[numthreads(1, 1, 1)]\n"
            "void CSMain( uint3 id : SV_DispatchThreadID ) \n"
            "{ \n"
            "\tbuf[id.y] = bols[id.x].b ? 1.0 : 0.0;\n"
            "}\n";

        TestComputeCompilationOk(s, platform);
    }

    PARAMETRIC_TEST_FIXTURE(ShaderCompilerTestFixture, HLSLcc_Compute_bitFieldExtract, (ShaderCompilerPlatform platform), OpenGLCore_Metal_Switch)
    {
        // NB. bitFieldExtract int variants are not tested as DX compiler produces performance warning which is
        // treated as error. Still both scalar and vector uint variants are triggered with this snippet
        const char *s =
            "#pragma kernel GetFaces\n"
            "StructuredBuffer<uint> MapVoxels;\n"
            "RWStructuredBuffer<uint> SolidVoxels;\n"
            "RWStructuredBuffer<uint> SolidFaces;\n"
            "[numthreads(32,1,32)]\n"
            "void GetFaces( uint3 tid : SV_GroupThreadID, uint3 gid : SV_GroupID )\n"
            "{\n"
            "  uint voxel = SolidVoxels[0];\n"
            "  uint z = voxel % 256;\n"
            "  voxel /= 256;\n"
            "  uint y = voxel % 256;\n"
            "  voxel /= 256;\n"
            "  uint x = voxel % 256;\n"
            "  uint voxels = MapVoxels[x+y+z];\n"
            "  SolidFaces[tid.x] = (0 == (y%4)) ? (voxels / (256*256*256)) % 256 : voxels % 256;\n"
            "}\n";

        TestComputeCompilationOk(s, platform);
    }

    PARAMETRIC_TEST_FIXTURE(ShaderCompilerTestFixture, HLSLcc_Compute_bitFieldInsert_destMask, (ShaderCompilerPlatform platform), OpenGLCore_Metal_Switch)
    {
        const char *s =
            "#pragma kernel Main\n"
            "RWTexture2D<float4> TempTexture;\n"
            "[numthreads(8, 8, 1)]\n"
            "void Main(uint3 id : SV_DispatchThreadID)\n"
            "{\n"
            "    uint x = id.x & 3;\n"
            "   uint y = ((id.x & 4) << 1) | (id.y & 7);\n"
            "   TempTexture[uint2(x, y)] = float4(id / 8.0, 0);\n"
            "}\n";

        inputData.compileFlags = kShaderCompFlags_PreferHLSLcc;
        TestComputeCompilationOk(s, platform);
    }

    PARAMETRIC_TEST_FIXTURE(ShaderCompilerTestFixture, ComputeShader_BuiltinSamplers, (ShaderCompilerPlatform platform), GLES3Plus_Metal_Switch)
    {
        const char *s =
            "#pragma kernel CSMain\n"
            "Texture2D Input;\n"
            "Texture2D Second;\n"
            "SamplerState samplerInput;\n"
            "SamplerState MyLinearRepeatSampler;\n"
            "SamplerState MyClampPointSampler;\n"
            "RWTexture2D<float4> Result;\n"

            "[numthreads(1, 1, 1)]\n"
            "void CSMain( uint3 id : SV_DispatchThreadID ) \n"
            "{ \n"
            "\tfloat4 foo = Input.SampleLevel(samplerInput, id.xy, 0);\n"
            "\tfoo += Input.SampleLevel(MyLinearRepeatSampler, id.xy, 0);\n"
            "\tfoo += Input.SampleLevel(MyClampPointSampler, id.xy, 0);\n"
            "\tfoo += Second.SampleLevel(MyLinearRepeatSampler, id.xy, 0);\n"
            "\tResult[id.xy] = foo;\n"
            "}\n";

        TestComputeCompilationOk(s, platform);
    }

    PARAMETRIC_TEST_FIXTURE(ShaderCompilerTestFixture, ComputeShader_Float3, (ShaderCompilerPlatform platform), GLES3Plus_Metal_Vulkan_Switch)
    {
        const char *s =
            "#pragma kernel LoadFloat4Tex3DTest\n"
            "#include \"HLSLSupport.cginc\"\n"
            "[numthreads(1,1,1)]\n"
            "void LoadFloat4Tex3DTest (uint3 id : SV_DispatchThreadID, SAMPLER_UNIFORM Texture3D<float4> tex, SAMPLER_UNIFORM RWStructuredBuffer<float4> buf)\n"
            "{\n"
            "\tbuf[0] = tex[id.xyz];\n"
            "\tbuf[1] = tex[int3(1,id.x,0)];\n"
            "\tbuf[2] = tex[int3(0,1,0) + id.xyz];\n"
            "\tbuf[3] = tex[int3(1,1,0)];\n"
            "\tbuf[4] = tex[id.xyz + int3(0,0,1)];\n"
            "\tbuf[5] = tex[int3(1,id.x,1)];\n"
            "\tbuf[6] = tex[int3(0,1,1) + id.xyz];\n"
            "\tbuf[7] = tex[int3(1,1,1)];\n"
            "}\n";
        TestComputeCompilationOk(s, platform);
    }

    PARAMETRIC_TEST_FIXTURE(ShaderCompilerTestFixture, ComputeShader_MalformedBuiltinSamplers, (ShaderCompilerPlatform platform), GLES3Plus_Metal_Switch)
    {
        const char *s =
            "#pragma kernel CSMain\n"
            "Texture2D Input;\n"
            "SamplerState samplerWithInvalidName;\n"
            "RWTexture2D<float4> Result;\n"

            "[numthreads(1, 1, 1)]\n"
            "void CSMain( uint3 id : SV_DispatchThreadID ) \n"
            "{ \n"
            "\tfloat4 foo = Input.SampleLevel(samplerWithInvalidName, id.xy, 0);\n"
            "\tResult[id.xy] = foo;\n"
            "}\n";

        core::string err = "Unrecognized sampler";
        TestComputeCompilationFailure(s, platform, err);
    }

    PARAMETRIC_TEST_FIXTURE(ShaderCompilerTestFixture, ComputeShader_NBodyIntegerOps, (ShaderCompilerPlatform platform), GLES3Plus_Metal_Vulkan_Switch)
    {
        const char *s =
            "// This test is originally a repro case for bug #704497\n"
            "// Added in gfx-tests with permission from the author Arthur Brussee (tinycubestudio@gmail.com)\n"
            "// \n"
            "\n"
            "#pragma kernel EmitParticles\n"
            "#pragma kernel UpdateParticles\n"
            "#define GroupSize 256 //multiple of 64 for nvidia cards\n"
            "struct Particle\n"
            "{\n"
            "\tfloat3 position;\n"
            "    float3 velocity;\n"
            "\tfloat size;\n"
            "\tfloat3 color;\n"
            "};\n"
            "StructuredBuffer<Particle> particlesIn; // added double buffering to avoid mem op order inconsistencies\n"
            "RWStructuredBuffer<Particle> particlesOut;\n"
            "\n"
            "static float softeningSquared = 0.0012500000 * 0.0012500000;\n"
            "static float massDensity = 6.67300f * 0.12f;\n"
            "float dt;\n"
            "uint dimx; \n"
            "uint seed;\n"
            "float maxRadius;\n"
            "float maxSpeed;\n"
            "float maxSize;\n"
            "float minSize;\n"
            "float4 emitterPosition;\n"
            "int numParticles;\n"
            "\n"
            "Texture2D colours;\n"
            "//To sample a texture in a compute shader you need a SamplerState.\n"
            "//To let unity setup this sampler, you _must_ call it samplerMyTexture\n"
            "//So in this case that becomes samplercolours\n"
            "SamplerState samplercolours;\n"
            "\n"
            "//PRNG functions\n"
            "//For more detail how this works, this article explains it very well: http://www.reedbeta.com/blog/2013/01/12/quick-and-easy-gpu-random-numbers-in-d3d11/\n"
            "#define PI2 6.28318530717\n"
            "\n"
            "uint rng_state;\n"
            "\n"
            "uint WangHash(uint id, uint seed)\n"
            "{\n"
            "\t//wang hash, to randomize seed and make sure xorshift isn't coherent.\n"
            "\trng_state = id + seed;\n"
            "    rng_state = (rng_state ^ 61) ^ (rng_state >> 16);\n"
            "    rng_state *= 9;\n"
            "    rng_state = rng_state ^ (rng_state >> 4);\n"
            "    rng_state *= 0x27d4eb2d;\n"
            "    rng_state = rng_state ^ (rng_state >> 15);\n"
            "\treturn rng_state;\n"
            "}\n"
            "\n"
            "uint NextXor()\n"
            "{\n"
            "    // Xorshift algorithm from George Marsaglia's paper\n"
            "    rng_state ^= (rng_state << 13);\n"
            "    rng_state ^= (rng_state >> 17);\n"
            "    rng_state ^= (rng_state << 5);\n"
            "    return rng_state;\n"
            "}\n"
            "//end internal\n"
            "\n"
            "//public Random API\n"
            "float FirstRandom(uint id, uint time)\n"
            "{\n"
            "\treturn WangHash(id, time) * 1.0f /  4294967295.0f;\n"
            "}\n"
            "\n"
            "float Random()\n"
            "{\n"
            "\treturn NextXor() * 1.0f /  4294967295.0f;\n"
            "}\n"
            "\n"
            "float RandomRange(float minVal, float maxVal)\n"
            "{\n"
            "\treturn lerp(minVal, maxVal, Random());\n"
            "}\n"
            "\n"
            "int RandomRangeInt(int minVal, int maxVal)\n"
            "{\n"
            "\treturn minVal + NextXor() % (maxVal - minVal);\n"
            "}\n"
            "\n"
            "float3 RandomInUnitSphere()\n"
            "{\n"
            "\t//Pick a random number, repeat until it is in a unit sphere\n"
            "\tfloat3 rand = float3(\tRandom() * 2.0f - 1.0f, \n"
            "\t\t\t\t\t\t\tRandom() * 2.0f - 1.0f,\n"
            "\t\t\t\t\t\t\tRandom() * 2.0f - 1.0f);\n"
            "\t\n"
            "\t//At max, repick 12 times. At around 16 - 20 times, GPU's start to panick, suspecting the loop is infinite, and often times completely crash... \n"
            "\tint count = 0;\n"
            "\t[loop]\n"
            "\twhile(dot(rand, rand) > 1 && ++count < 12)\n"
            "\t{\n"
            "\t\trand = float3(\tRandom() * 2.0f - 1.0f, \n"
            "\t\t\t\t\t\tRandom() * 2.0f - 1.0f,\n"
            "\t\t\t\t\t\tRandom() * 2.0f - 1.0f);\n"
            "\t}\n"
            "\n"
            "\n"
            "\treturn rand;\n"
            "}\n"
            "\n"
            "\n"
            "\n"
            "groupshared float3 sharedPos[GroupSize];\n"
            "\n"
            "// Body to body interaction, acceleration of the particle at position bi is updated.\n"
            "float3 bodyBodyInteraction(float3 bj, float3 bi, float mass) \n"
            "{\n"
            "    float3 r = bj - bi; \n"
            "\n"
            "    float distSqr = dot(r, r);\n"
            "    distSqr += softeningSquared;\n"
            "\n"
            "    float invDist = 1.0f / sqrt(distSqr);\n"
            "\tfloat invDistCube =  invDist * invDist * invDist;\n"
            "    \n"
            "    float s = mass * invDistCube;\n"
            "\n"
            "\treturn r * s;    \n"
            "}\n"
            "\n"
            "//Kernel to emit the particles and set up inital properties. These are chosen mostly to look good\n"
            "[numthreads(GroupSize,1,1)]\n"
            "void EmitParticles( uint3 Gid : SV_GroupID, uint3 DTid : SV_DispatchThreadID, uint3 GTid : SV_GroupThreadID, uint GI : SV_GroupIndex )\n"
            "{\n"
            "\tif (DTid.x > (uint)numParticles)\n"
            "\t\treturn;//Only emit numParticles amount of particles\n"
            "\n"
            "\tparticlesOut[DTid.x].size = lerp(minSize, maxSize, FirstRandom(DTid.x, seed));\n"
            "\n"
            "\n"
            "\tfloat sp = Random();\n"
            "\n"
            "\tfloat3 offset = sp > 0.5f ? 100.0f : 0.0f;\n"
            "\n"
            "\t//Pick a position on in a random sphere around the emitter\n"
            "\tfloat3 pos =  RandomInUnitSphere() ;\n"
            "\tparticlesOut[DTid.x].position = pos * maxRadius + emitterPosition.xyz + offset;//Emitter position really should be a float3, but unity only has a SetVector for a Vector4 function :(\n"
            "\n"
            "\n"
            "\t//The velocity is partly random\n"
            "\t//And one part (the cross vector) to give each particle a nice spin around the center\n"
            "\tparticlesOut[DTid.x].velocity = (cross(pos, float3(0, 1, 0)) * 6.0f + RandomInUnitSphere()) * maxSpeed;\n"
            "\n"
            "\n"
            "\t//Pick a random point on the colour gradient, and store the colour.\n"
            "\t\n"
            "#if !defined(SHADER_API_PSSL)\n"
            "\tparticlesOut[DTid.x].color = colours.SampleLevel(samplercolours, float2(Random(), 0.0f), 0).rgb;\n"
            "#else\n"
            "\tparticlesOut[DTid.x].color = colours.SampleLOD(samplercolours, float2(Random()), 0).rgb;\n"
            "#endif\t\n"
            "}\n"
            "\n"
            "//The update particle thread\n"
            "[numthreads(GroupSize,1,1)]\n"
            "void UpdateParticles( uint3 Gid : SV_GroupID, uint3 DTid : SV_DispatchThreadID, uint3 GTid : SV_GroupThreadID, uint GI : SV_GroupIndex )\n"
            "{\n"
            "    // Each thread of the CS updates one of the particles\n"
            "\t//Store current particle properties for quick access\n"
            "    float3 pos = particlesIn[DTid.x].position;\n"
            "    float3 vel = particlesIn[DTid.x].velocity;\n"
            "    float3 accel = 0;\n"
            "\tfloat r = particlesIn[DTid.x].size * 0.5f;\n"
            "    float mass = r * r * r * massDensity; \n"
            "\n"
            "    // Update current particle using all other particles\n"
            "    [loop]\n"
            "    for (uint tile = 0; tile < dimx; tile++)\n"
            "    {\n"
            "        // Cache a tile of particles unto shared memory to increase IO efficiency\n"
            "        sharedPos[GI] = particlesIn[tile * GroupSize + GI].position;\n"
            "       \n"
            "        GroupMemoryBarrierWithGroupSync();        \n"
            "\n"
            "        [loop]\n"
            "        for (uint counter = 0; counter < GroupSize; ++counter) \n"
            "        {\n"
            "\t\t\t//used to be manually unrolled 8 times, this seems to perform just as well, and looks a little nicer\n"
            "            accel += bodyBodyInteraction(sharedPos[counter], pos, mass);\n"
            "        }\n"
            "        \n"
            "        GroupMemoryBarrierWithGroupSync();\n"
            "    }  \n"
            "\n"
            "\t//There is no need anymore to deal with phantom particles. They do still exist, but their mass is 0 so they don't contribute to the final result\n"
            "\n"
            "\n"
            "    // Update the velocity and position of current particle using the acceleration computed above\n"
            "    vel += accel * dt; \n"
            "    pos += vel * dt;   \n"
            "\n"
            "\n"
            "\t//Write back to global memory\n"
            "    particlesOut[DTid.x].position = pos;\n"
            "    particlesOut[DTid.x].velocity = vel;\n"
            "}\n"
            "\n";

        TestComputeCompilationOk(s, platform);
    }

    PARAMETRIC_TEST_FIXTURE(ShaderCompilerTestFixture, HLSLcc_Test_Metal_StructuredBufferRead, (ShaderCompilerPlatform platform), OpenGLCore_Metal_Switch)
    {
        const char *s =
            "\n"
            "#include \"UnityCG.cginc\"\n"
            "\n"
            "sampler2D _MainTex;\n"
            "sampler2D _WhiteTex;\n"
            "\n"
            "//This particle struct must match the one in the compute shader exactly\n"
            "struct Particle\n"
            "{\n"
            "\tfloat3 pos;\n"
            "\tfloat3 velocity;\n"
            "\tfloat size;\n"
            "\tfloat3 color;\n"
            "};\n"
            "\n"
            "\n"
            "StructuredBuffer<Particle> particles;\n"
            "\n"
            "\n"
            "//Bufpoints contains the positions of a quad\n"
            "//You were using a geometry shader to expand a point to a billboard\n"
            "//Which is a very sensible way to go, and _should_ be faster\n"
            "//But in my experience it isn't :( I experimented with geometry shaders for TC Particles\n"
            "//And it absolutely wrecked performance. This method seems to be the most performt\n"
            "//Not to mention that it's a little more flexible\n"
            "StructuredBuffer<float2> bufPoints;\n"
            "\n"
            "//Colour tint and glow for HDR\n"
            "float4 _Tint;\n"
            "float _Glow;\n"
            "\n"
            "float _UseVertexCol;\n"
            "\n"
            "struct particle_fragment\n"
            "{\n"
            "\thalf4 pos : SV_POSITION;\n"
            "\thalf2 uv : TEXCOORD0;\n"
            "\thalf3 col : COLOR;\n"
            "};\n"
            "\n"
            "\n"
            "particle_fragment vert (uint id : SV_VertexID, uint inst : SV_InstanceID)\n"
            "{\n"
            "\tparticle_fragment output;\n"
            "\tfloat4 pos = mul(UNITY_MATRIX_VP, float4(particles[inst].pos, 1.0f));\n"
            "\toutput.pos = pos + mul(UNITY_MATRIX_P, float4(bufPoints[id] * particles[inst].size, 0,0));\n"
            "\toutput.uv = bufPoints[id] + 0.5f;\n"
            "\toutput.col = particles[inst].color;\n"
            "\n"
            "\t//If in vertex color mode, sample.\n"
            "\tif (_UseVertexCol > 0){\n"
            "\t\tfloat4 tex = tex2Dlod(_WhiteTex, float4(0.0f, 0.0f, 0.0f, 0.0f)).rgba;\n"
            "\t\toutput.col *= tex.xyz;\n"
            "\n"
            "\t}\n"
            "\t\n"
            "\treturn output;\n"
            "}\n"
            "\n"
            "\n"
            "fixed4 frag (particle_fragment i) : COLOR\n"
            "{\n"
            "\tfloat4 col = tex2Dbias(_MainTex, float4(i.uv, 0, -3)) * _Tint * float4(i.col, 1.0f) * _Glow;\n"
            "\n"
            "\t//If not in vertex color mode sample in pixel shader.\n"
            "\tif (_UseVertexCol <= 0){\n"
            "\t\tfloat4 tex = tex2Dlod(_WhiteTex, float4(0.0f, 0.0f, 0.0f, 0.0f)).rgba;\n"
            "\t\tcol *= tex;\n"
            "\t}\n"
            "\n"
            "\treturn col;\n"
            "}\n"
            "\n";

        inputData.requirements = kShaderRequireShaderModel50;
        TestCompilationOk(s, platform);
    }

    PARAMETRIC_TEST_FIXTURE(ShaderCompilerTestFixture, ShaderCompiler_LegacyLightingMacros_CompileSuccessfully_InForwardBasePass, (ShaderCompilerPlatform platform), CommonPlatforms)
    {
        const char *s =
            "#define UNITY_PASS_FORWARDBASE 1\n"
            "#define POINT 1\n"
            "#include \"UnityCG.cginc\"\n"
            "#include \"AutoLight.cginc\"\n"
            "#include \"Lighting.cginc\"\n"
            "\n"
            "struct VertexInput\n"
            "{\n"
            "float4 vertex : POSITION;\n"
            "float3 normal : NORMAL;\n"
            "};\n"
            "struct VertexOutput\n"
            "{\n"
            "float4 pos : SV_POSITION;\n"
            "float4 posWorld : TEXCOORD0;\n"
            "float3 normalDir : TEXCOORD1;\n"
            "LIGHTING_COORDS(2, 3)\n"
            "};\n"
            "VertexOutput vert(VertexInput v)\n"
            "{\n"
            "VertexOutput o = (VertexOutput)0;\n"
            "o.normalDir = UnityObjectToWorldNormal(v.normal);\n"
            "o.posWorld = mul(unity_ObjectToWorld, v.vertex);\n"
            "float3 lightColor = _LightColor0.rgb;\n"
            "o.pos = UnityObjectToClipPos(v.vertex);\n"
            "TRANSFER_VERTEX_TO_FRAGMENT(o)\n"
            "return o;\n"
            "}\n"
            "float4 frag(VertexOutput i) : COLOR{\n"
            "i.normalDir = normalize(i.normalDir);\n"
            "float3 normalDirection = i.normalDir;\n"
            "float3 lightDirection = normalize(lerp(_WorldSpaceLightPos0.xyz, _WorldSpaceLightPos0.xyz - i.posWorld.xyz,_WorldSpaceLightPos0.w));\n"
            "float3 lightColor = _LightColor0.rgb;\n"
            "float attenuation = LIGHT_ATTENUATION(i);\n"
            "float3 finalColor = (_LightColor0.rgb*attenuation*dot(lightDirection,normalDirection));\n"
            "return fixed4(finalColor * 1,0);\n"
            "};\n";

        inputData.requirements = kShaderRequireShaderModel30;
        TestCompilationOk(s, platform);
    }
    PARAMETRIC_TEST_FIXTURE(ShaderCompilerTestFixture, ShaderCompiler_LegacyLightingMacros_CompileSuccessfully_InForwardAddPass, (ShaderCompilerPlatform platform), CommonPlatforms)
    {
        const char *s =
            "#define UNITY_PASS_FORWARDADD 1\n"
            "#define POINT 1\n"
            "#include \"UnityCG.cginc\"\n"
            "#include \"AutoLight.cginc\"\n"
            "#include \"Lighting.cginc\"\n"
            "\n"
            "struct VertexInput\n"
            "{\n"
            "float4 vertex : POSITION;\n"
            "float3 normal : NORMAL;\n"
            "};\n"
            "struct VertexOutput\n"
            "{\n"
            "float4 pos : SV_POSITION;\n"
            "float4 posWorld : TEXCOORD0;\n"
            "float3 normalDir : TEXCOORD1;\n"
            "LIGHTING_COORDS(2, 3)\n"
            "};\n"
            "VertexOutput vert(VertexInput v)\n"
            "{\n"
            "VertexOutput o = (VertexOutput)0;\n"
            "o.normalDir = UnityObjectToWorldNormal(v.normal);\n"
            "o.posWorld = mul(unity_ObjectToWorld, v.vertex);\n"
            "float3 lightColor = _LightColor0.rgb;\n"
            "o.pos = UnityObjectToClipPos(v.vertex);\n"
            "TRANSFER_VERTEX_TO_FRAGMENT(o)\n"
            "return o;\n"
            "}\n"
            "float4 frag(VertexOutput i) : COLOR{\n"
            "i.normalDir = normalize(i.normalDir);\n"
            "float3 normalDirection = i.normalDir;\n"
            "float3 lightDirection = normalize(lerp(_WorldSpaceLightPos0.xyz, _WorldSpaceLightPos0.xyz - i.posWorld.xyz,_WorldSpaceLightPos0.w));\n"
            "float3 lightColor = _LightColor0.rgb;\n"
            "float attenuation = LIGHT_ATTENUATION(i);\n"
            "float3 finalColor = (_LightColor0.rgb*attenuation*dot(lightDirection,normalDirection));\n"
            "return fixed4(finalColor * 1,0);\n"
            "};\n";

        inputData.requirements = kShaderRequireShaderModel30;
        TestCompilationOk(s, platform);
    }

    TEST_FIXTURE(ShaderCompilerTestFixture, HLSLcc_Compute_LoadStoreFloatTex2D)
    {
        const char *s =
            "#pragma kernel CSMain\n"

            "[numthreads(1, 1, 1)]\n"
            "void CSMain(uint3 id : SV_DispatchThreadID, RWTexture2D<float> tex, RWStructuredBuffer<float> buf)\n"
            "{\n"
            "\ttex[int2(0, 0)] = 0;\n"
            "\ttex[int2(1, 0)] = id.x + 1;\n"
            "\ttex[int2(0, id.x + 1)] = 2;\n"
            "\ttex[id.xy + int2(1, 1)] = id.z + 3;\n"
            "\tAllMemoryBarrier();\n"
            "\tbuf[0] = tex[id.xy];\n"
            "\tbuf[1] = tex[int2(1, id.x)];\n"
            "\tbuf[2] = tex[int2(0, 1) + id.xy];\n"
            "\tbuf[3] = tex[int2(1, 1)];\n"
            "}\n";

        TestComputeCompilationOk(s, kShaderCompPlatformGLES3Plus);
    }

    TEST_FIXTURE(ShaderCompilerTestFixture, ComputeShader_CBVectorArrayLayout)
    {
        const char *s =
            "#pragma kernel CSMain\n"
            "float \tf1ParamArray[2];\n"
            "float2 f2ParamArray[2];\n"
            "float3 f3ParamArray[2];\n"
            "float4 f4ParamArray[2];\n"
            "int4 \ti4ParamArray[2];\n"
            "int3 \ti3ParamArray[2];\n"
            "int2 \ti2ParamArray[2];\n"
            "int \ti1ParamArray[2];\n"
            "[numthreads(1, 1, 1)]\n"
            "void CSMain(uint3 id : SV_DispatchThreadID, RWStructuredBuffer<float> buf, RWStructuredBuffer<float2> buf2)\n"
            "{\n"
            "\tbuf[0] = f1ParamArray[0];\n"
            "\tbuf[1] = f1ParamArray[1];\n"
            "\tbuf[2] = f2ParamArray[0].x;\n"
            "\tbuf[3] = f2ParamArray[0].y;\n"
            "\tfloat2 temp = f2ParamArray[1] + float2(0, 1);\n"
            "\tbuf2[5] = temp;\n"
            "\tbuf[6] = f3ParamArray[0].x;\n"
            "\tbuf[7] = f3ParamArray[0].y;\n"
            "\tbuf[8] = f3ParamArray[0].z;\n"
            "\tbuf2[9] = f3ParamArray[1].xy;\n"
            "\tbuf[11] = f3ParamArray[1].z;\n"
            "\tbuf[12] = f4ParamArray[0].x;\n"
            "\tbuf2[13] = f4ParamArray[0].yz;\n"
            "\tbuf[15] = f4ParamArray[0].w;\n"
            "\tbuf[16] = f4ParamArray[1].x;\n"
            "\tbuf[17] = f4ParamArray[1].y;\n"
            "\ttemp = f4ParamArray[1].zw + float2(1, 0);\n"
            "\tbuf2[18] = temp;\n"
            "\tbuf[20] = (float)i1ParamArray[0];\n"
            "\tbuf[21] = (float)i1ParamArray[1];\n"
            "\ttemp = float2(1,0) * (float)i2ParamArray[0];\n"
            "\tbuf2[23] = temp;\n"
            "\tbuf[24] = (float)i2ParamArray[1].x;\n"
            "\tbuf[25] = (float)i2ParamArray[1].y;\n"
            "\tbuf[26] = (float)i3ParamArray[0].x;\n"
            "\tbuf[27] = (float)i3ParamArray[0].y;\n"
            "\tbuf[28] = (float)i3ParamArray[0].z;\n"
            "\tbuf[29] = (float)i3ParamArray[1].x;\n"
            "\tbuf[30] = (float)i3ParamArray[1].y;\n"
            "\tbuf[31] = (float)i3ParamArray[1].z;\n"
            "\tbuf[32] = (float)i4ParamArray[0].x;\n"
            "\tbuf[33] = (float)i4ParamArray[0].y;\n"
            "\tbuf[34] = (float)i4ParamArray[0].z;\n"
            "\tbuf[35] = (float)i4ParamArray[0].w;\n"
            "\tbuf[36] = (float)i4ParamArray[1].x;\n"
            "\tbuf[37] = (float)i4ParamArray[1].y;\n"
            "\tbuf[38] = (float)i4ParamArray[1].z;\n"
            "\tbuf[39] = (float)i4ParamArray[1].w;\n"
            "}\n";

        TestComputeCompilationOk(s, kShaderCompPlatformMetal);
    }

    TEST_FIXTURE(ShaderCompilerTestFixture, ComputeShader_NonLineSpecificWarning_DoesNotError)
    {
        const char *s =
            "#pragma kernel CSMain\n"
            "RWStructuredBuffer<float> buf;\n"
            "[numthreads(1024, 1, 1)]\n"
            "void CSMain(uint3 id : SV_DispatchThreadID)\n"
            "{\n"
            "\tfloat foo[20];\n"
            "\tfoo[0] = 0;\n"
            "\t[loop]\n"
            "\tfor (int i = 1; i < 20; i++)\n"
            "\t{\n"
            "\t\tfoo[i] = buf[id.x + i] + foo[i-1];\n"
            "\t}\n"
            "\t[loop]\n"
            "\tfor (int j = 0; j < 20; j++)\n"
            "\t{\n"
            "\t\tbuf[id.x + j] = foo[19-j];\n"
            "\t}\n"
            "}\n";

        // This shader should fail with a warning that has no specific line number
        TestComputeCompilationOk(s, kShaderCompPlatformD3D11, true);
        if (!errors.empty())
            CHECK_EQUAL(true, errors[0].warning); // Check that it was recognized as warning, not error
    }

    PARAMETRIC_TEST_FIXTURE(ShaderCompilerTestFixture, ComputeShader_EmptyKernel_DoesNotError, (ShaderCompilerPlatform platform), ComputePlatforms)
    {
        const char *s =
            "#pragma kernel CSMain\n"
            "[numthreads(1, 1, 1)]\n"
            "void CSMain(uint3 id : SV_DispatchThreadID)\n"
            "{\n"
            "}\n";
        TestComputeCompilationOk(s, platform, false);
    }

    PARAMETRIC_TEST_FIXTURE(ShaderCompilerTestFixture, ComputeShader_MissingKernel_ErrorMessage, (ShaderCompilerPlatform platform), ComputePlatforms)
    {
        TestMissingKernel(platform);
    }

    TEST_FIXTURE(ShaderCompilerTestFixture, HLSLcc_Compute_StripComputeShaderSourceVariants)
    {
        #define CG_SNIPPET      "\ncg_goes_here\n"
        #define GLSL_SNIPPET    "\nglsl_goes_here\n"

        const char* s =
            "common_prefix\n"
            "CGPROGRAM" CG_SNIPPET "ENDCG"
            "GLSLPROGRAM" GLSL_SNIPPET "ENDGLSL"
            "common_postfix\n";

        {
            core::string src2 = s;
            ShaderSourceLanguage lang2 = StripComputeShaderSourceVariants(src2, kShaderCompPlatformD3D11);
            CHECK_EQUAL(kShaderSourceLanguageCg, lang2);
            CHECK_EQUAL(core::string("common_prefix\n" CG_SNIPPET "common_postfix\n"), src2);
        }
        {
            core::string src3 = s;
            ShaderSourceLanguage lang3 = StripComputeShaderSourceVariants(src3, kShaderCompPlatformGLES3Plus);
            CHECK_EQUAL(kShaderSourceLanguageGLSL, lang3);
            CHECK_EQUAL(core::string("common_prefix\n" GLSL_SNIPPET "common_postfix\n"), src3);
        }

        #undef GLSL_SNIPPET
        #undef CG_SNIPPET
    }

    TEST_FIXTURE(ShaderCompilerTestFixture, Macro_HDRLIGHTPREPASSON_IsDefined_When_UNITYHDRON_IsDefined)
    {
        // Unity 4.x had HDR_LIGHT_PREPASS_ON keyword in deferred lighting; 5.0 renamed it to UNITY_HDR_ON.
        // However we also need to define the old keyword so that user-written shaders still continue
        // to work as expected.
        // Check if compiling shader with UNITY_HDR_ON defines the legacy one (shader has an error if it's not defined).
        const char* s =
            "float4 frag () : SV_Target {\n"
            "#ifdef HDR_LIGHT_PREPASS_ON\n"
            "  return 1.0;\n"
            "#else\n"
            "  error_shoudl_not_happen;\n"
            "#endif\n"
            "}\n";
        inputData.programType = kProgramFragment;
        if (gPluginDispatcher->HasCompilerForAPI(kShaderCompPlatformD3D11))
        {
            InitReflectionReport(kShaderCompPlatformD3D11);
            inputData.source = MakeShaderSourcePrefix(inputData.p) + s;
            inputData.api = kShaderCompPlatformD3D11;
            inputData.args.clear();
            inputData.args.push_back(ShaderCompileArg("UNITY_HDR_ON", "1"));
            const ShaderCompileOutputInterface* output = gPluginDispatcher->CompileShader(&inputData);
            bool hadErrors = output->GetNumErrorsAndWarnings() > 0;
            gPluginDispatcher->ReleaseShader(output);
            CHECK_EQUAL(false, hadErrors);
        }
    }

    TEST_FIXTURE(ShaderCompilerTestFixture, ShaderMacro_MajorMinorVersionsClampToSingleDigit)
    {
        TestMinMaxInsideAndOutsideBounds(10, 20);
        TestMinMaxInsideAndOutsideBounds(2, 10);
        TestMinMaxInsideAndOutsideBounds(10, 2);
        TestMinMaxInsideAndOutsideBounds(2, 2);
    }

    TEST_FIXTURE(ShaderCompilerTestFixture, HLSLcc_GeometryShaderInterpolatorIsAddedToOutput)
    {
        const char* s =
            "struct appdata { float4 vertex : POSITION; };\n"
            "struct v2f\n"
            "{\n"
            "float4 vertex : SV_POSITION; \n"
            "int example : TEXCOORD0; \n"
            "nointerpolation float other : TEXCOORD1; \n"
            "};\n"
            "appdata vert (appdata v) { return v; }\n"
            "[maxvertexcount(3)]\n"
            "void geom(triangle appdata input[3], inout TriangleStream<v2f> OutputStream)\n"
            "{\n"
            "v2f test = (v2f)0;\n"
            "test.vertex = input[0].vertex + float4(1, 0, 0, 0);\n"
            "test.example = 0;\n"
            "test.other = 0;\n"
            "OutputStream.Append(test);\n"
            "test.vertex = input[0].vertex + float4(1, 1, 0, 0);\n"
            "test.example = 0;\n"
            "test.other = 0;\n"
            "OutputStream.Append(test);\n"
            "test.vertex = input[0].vertex + float4(0, 1, 0, 0);\n"
            "test.example = 1;\n"
            "test.other = 1;\n"
            "OutputStream.Append(test);\n"
            "}\n"
            "float4 frag (v2f i) : SV_Target { return float4(i.example, i.other, 0, 1); }\n";

        this->inputData.p.entryName[kProgramGeometry] = "geom";
        this->inputData.requirements = kShaderRequireShaderModel46_GL41;

        CHECK(TestCompilationOk(s, kShaderCompPlatformOpenGLCore));

        core::string outputString;
        core::string interpolatorString = "layout(location = 1) flat out";
        AssignOutputString(outputString);
        CHECK(outputString.find(interpolatorString) != core::string::npos);
    }

    struct ParseErrorMessagesFixture
    {
        void DoParseErrors(const core::string& listing)
        {
            ShaderImportErrors errorSet;
            ParseErrorMessages(listing, false, "): error ", "", 0, kShaderCompPlatformD3D11, errorSet);
            errors.clear();
            for (ShaderImportErrors::ErrorContainer::const_iterator it = errorSet.GetErrors().begin(); it != errorSet.GetErrors().end(); ++it)
                errors.push_back(*it);
        }

        std::vector<ShaderImportError> errors;
    };

    TEST_FIXTURE(ParseErrorMessagesFixture, ParseErrorMessages_SingleSimpleError_Works)
    {
        DoParseErrors("file.txt(13): error X1234: foo\n");
        CHECK_EQUAL(1, errors.size());
        CHECK_EQUAL("foo", errors[0].message);
        CHECK_EQUAL("file.txt", errors[0].file);
        CHECK_EQUAL(13, errors[0].line);
    }

    TEST_FIXTURE(ParseErrorMessagesFixture, ParseErrorMessages_LineColumnNumbers_Work)
    {
        DoParseErrors(
            "a.txt(10): error X1234: foo\n" // just line number
            "b.txt(25,15): error C1337: bar\n" // line,column
            "c.txt(30,5-15): error C0000: baz\n"); // line,col-col
        CHECK_EQUAL(3, errors.size());

        CHECK_EQUAL("foo", errors[0].message);
        CHECK_EQUAL("a.txt", errors[0].file);
        CHECK_EQUAL(10, errors[0].line);

        CHECK_EQUAL("bar", errors[1].message);
        CHECK_EQUAL("b.txt", errors[1].file);
        CHECK_EQUAL(25, errors[1].line);

        CHECK_EQUAL("baz", errors[2].message);
        CHECK_EQUAL("c.txt", errors[2].file);
        CHECK_EQUAL(30, errors[2].line);
    }

    TEST_FIXTURE(ParseErrorMessagesFixture, ParseErrorMessages_MultiLineErrors_AreMerged)
    {
        // Multi-line errors reported in same file/line should be merged together
        DoParseErrors(
            "file.txt(13): error X1234: No matching foo\n"
            "file.txt(13): error X1234: possible foos are:\n"
            "file.txt(13): error X1234:    bar\n");
        CHECK_EQUAL(1, errors.size());
        CHECK_EQUAL("No matching foo; possible foos are: bar", errors[0].message);
        CHECK_EQUAL("file.txt", errors[0].file);
        CHECK_EQUAL(13, errors[0].line);
    }

    TEST_FIXTURE(ParseErrorMessagesFixture, ParseErrorMessages_IdenticalErrors_AreSkipped)
    {
        // Some compilers report same error on same line multiple times (e.g. Cg PS3), these should be
        // skipped to not confuse people.
        DoParseErrors(
            "file.txt(13): error X1234: No matching foo\n"
            "file.txt(13): error X1234: No matching foo\n"
            "file.txt(13): error X1234: No matching foo\n"
            "file.txt(13): error X1234: But matching bar\n");
        CHECK_EQUAL(1, errors.size());
        CHECK_EQUAL("No matching foo; But matching bar", errors[0].message);
        CHECK_EQUAL("file.txt", errors[0].file);
        CHECK_EQUAL(13, errors[0].line);
    }

    static void CheckSamplerName(const char* name, TextureFilterMode filter, TextureWrapMode wrapU, TextureWrapMode wrapV, TextureWrapMode wrapW, bool compare = false)
    {
        core::string msg;
        InlineSamplerType sampler = ParseInlineSamplerName(name, msg);
        CHECK(!sampler.IsInvalid());
        CHECK(msg.empty());
        CHECK_EQUAL(filter, sampler.flags.filter);
        CHECK_EQUAL(wrapU, sampler.flags.wrapU);
        CHECK_EQUAL(wrapV, sampler.flags.wrapV);
        CHECK_EQUAL(wrapW, sampler.flags.wrapW);
        CHECK_EQUAL(compare, sampler.flags.compare != 0);
    }

    static void CheckSamplerName(const char* name, TextureFilterMode filter, TextureWrapMode wrap, bool compare = false)
    {
        CheckSamplerName(name, filter, wrap, wrap, wrap, compare);
    }

    TEST(ParseInlineSamplerName_Works)
    {
        CheckSamplerName("MyPointClamp", kTexFilterNearest, kTexWrapClamp);
        CheckSamplerName("repeat_linear_sampler", kTexFilterBilinear, kTexWrapRepeat);
        CheckSamplerName("CompareTriLinearClamp", kTexFilterTrilinear, kTexWrapClamp, true);
        CheckSamplerName("mirrorOnce_Point", kTexFilterNearest, kTexWrapMirrorOnce);
        CheckSamplerName("linear_mirror", kTexFilterBilinear, kTexWrapMirror);

        CheckSamplerName("point_clampU", kTexFilterNearest, kTexWrapClamp, kTexWrapRepeat, kTexWrapRepeat); // repeat is default when not specified for VW axes
        CheckSamplerName("smp_Clamp_RepeatV_Point", kTexFilterNearest, kTexWrapClamp, kTexWrapRepeat, kTexWrapClamp); // clamp on all axes, repeat on V
        CheckSamplerName("ClampUClampVClampWRepeatTrilinear", kTexFilterTrilinear, kTexWrapClamp, kTexWrapClamp, kTexWrapClamp); // clamps override repeat since they are axis specific
    }
    TEST(ParseInlineSamplerName_NameDoesHaveFilterAndWrapModes_ReturnsInvalidType)
    {
        core::string msg;
        InlineSamplerType smp;
        CHECK(ParseInlineSamplerName("", msg).IsInvalid());
        CHECK(ParseInlineSamplerName("Foobar", msg).IsInvalid());
        CHECK(ParseInlineSamplerName("sampler_MainTex", msg).IsInvalid());
        CHECK(ParseInlineSamplerName("PointSampler", msg).IsInvalid()); // needs both filter & wrap mode
    }
    TEST(ParseInlineSamplerName_NameHasConflictingModes_ReturnsInvalidType)
    {
        core::string msg;
        InlineSamplerType smp;
        CHECK(ParseInlineSamplerName("linear_point", msg).IsInvalid()); // different filter modes
        CHECK(ParseInlineSamplerName("foobarClampRepeat", msg).IsInvalid()); // different wrap modes
    }

    TEST(OnlyOnePlatformPluginPerPlatform)
    {
        bool platformPlugingExists[kShaderCompPlatformCount];
        for (int p = 0; p < kShaderCompPlatformCount; ++p)
            platformPlugingExists[p] = false;

        for (int i = 0; i < sPlatformPluginCount; ++i)
        {
            if (sPlatformPlugins[i] == NULL)
                continue;

            const unsigned int platforms = sPlatformPlugins[i]->GetShaderPlatforms();
            for (int p = 0; p < kShaderCompPlatformCount; ++p)
            {
                if (platforms & (1 << p))
                {
                    CHECK_EQUAL(false, platformPlugingExists[p]);
                    platformPlugingExists[p] = true;
                }
            }
        }
    }

    // Check that the passthrough shader is generated correctly on the platforms that currently support Tess
    PARAMETRIC_TEST_FIXTURE(ShaderCompilerTestFixture, ShaderCompiler_HullShader_PassThrough, (ShaderCompilerPlatform platform), OpenGLCore_Vulkan_D3D11_XboxOne_PS4_Switch)
    {
        const char *s =
            "struct HS_In\n"
            "{\n"
            "   float4 position : POSITION;\n"
            "   float4 normal : NORMAL;\n"
            "};\n"

            "struct HS_ConstantOut\n"
            "{\n"
            "float TessFactor[3]    : SV_TessFactor;\n"
            "float InsideTessFactor : SV_InsideTessFactor;\n"
            "};\n"

            "struct HS_Out\n"
            "{\n"
            "float4 position : SV_Position;\n"
            "float4 normal : NORMAL;\n"
            "};\n"

            "HS_In vert() { HS_In o; o.position = float4(0.0, 0.0, 0.0, 1.0f); o.normal = float4(0.0f, 1.0f, 0.0f, 0.0f); return o; }\n"
            "float4 frag() : SV_Target { return float4(0.0, 0.0, 0.0, 1.0f); }\n"

            "HS_ConstantOut HSConstant(InputPatch<HS_In, 3> i)\n"
            "{\n"
            "   HS_ConstantOut o = (HS_ConstantOut)0;\n"
            "   o.TessFactor[0] = o.TessFactor[1] = o.TessFactor[2] = 1.0;\n"
            "   o.InsideTessFactor = 1.0;\n"
            "   return o;\n"
            "}\n"

            "[domain(\"tri\")]\n"
            "[partitioning(\"integer\")]\n"
            "[outputtopology(\"triangle_cw\")]\n"
            "[patchconstantfunc(\"HSConstant\")]\n"
            "[outputcontrolpoints(3)]\n"
            "HS_Out hull(InputPatch<HS_In, 3> i, uint uCPID : SV_OutputControlPointID)\n"
            "{\n"
            "   HS_Out o = (HS_Out)0;\n"
            "   o.position = i[uCPID].position;\n"
            "   o.normal = i[uCPID].normal;\n"
            "   return o;\n"
            "}\n";

        inputData.requirements = kShaderRequireShaderModel50;
        inputData.p.entryName[kProgramVertex] = "vert";
        inputData.p.entryName[kProgramFragment] = "frag";
        inputData.p.entryName[kProgramHull] = "hull";
        inputData.p.entryName[kProgramDomain] = "";

        if (platform == kShaderCompPlatformPS4)
        {
            inputData.programMask = (1 << kShaderVertex) | (1 << kShaderFragment) | (1 << kShaderDomain);
        }

        TestCompilationOk(s, platform);
    }

    PARAMETRIC_TEST_FIXTURE(ShaderCompilerTestFixture, ShaderCompiler_Tessellation_PerPatchInputOutputTranslation, (ShaderCompilerPlatform platform), Vulkan_Only)
    {
        const char *s =
            "struct HullInput\n"
            "{\n"
            "   float4 position : POSITION;\n"
            "   float2 culling : COLOR0;\n"
            "};\n"

            "struct HullConstants\n"
            "{\n"
            "   float Edges[4] : SV_TessFactor;\n"
            "   float Inside[2] : SV_InsideTessFactor;\n"
            "};\n"

            "struct DomainInput\n"
            "{\n"
            "   float4 position : POSITION;\n"
            "};\n"

            "struct PixelInput\n"
            "{\n"
            "    float4 position : SV_POSITION;\n"
            "    float3 colour : COLOR0;\n"
            "};\n"
            "sampler2D _AmplitudeTex;\n"
            "HullInput vert(float3 position : POSITION)\n"
            "{\n"
            "    HullInput o;\n"
            "    o.position = float4(position, 0);\n"
            "    o.culling.x = tex2Dlod(_AmplitudeTex, float4(position.xz, 0, 0)).r;\n"
            "    o.culling.y = 91.9f; // epsilon is half_tile_size * sqrt(2)\n"
            "    return o;\n"
            "}\n"

            "HullConstants CalculateHullConstants(InputPatch<HullInput, 4> patch)\n"
            "{\n"
            "    HullConstants o;\n"
            "    // Terrain and frustum culling\n"
            "    if (!any(float4(patch[0].culling.x, patch[1].culling.x, patch[2].culling.x, patch[3].culling.x)) ||\n"
            "        !any(float4(patch[0].culling.y, patch[1].culling.y, patch[2].culling.y, patch[3].culling.y)))\n"
            "    {\n"
            "        o.Edges[0] = o.Edges[1] = o.Edges[2] = o.Edges[3] = o.Inside[0] = o.Inside[1] = 0;\n"
            "    }\n"
            "    else\n"
            "    {\n"
            "        o.Edges[0] = (patch[3].position.x - patch[0].position.x);\n"
            "        o.Edges[1] = (patch[0].position.y - patch[1].position.y);\n"
            "        o.Edges[2] = (patch[1].position.z - patch[2].position.z);\n"
            "        o.Edges[3] = (patch[2].position.w - patch[3].position.w);\n"
            "        o.Inside[0] = o.Inside[1] = min(min(o.Edges[0], o.Edges[1]), min(o.Edges[2], o.Edges[3]));\n"
            "    }\n"
            "    return o;\n"
            "}\n"

            "[domain(\"quad\")]\n"
            "[partitioning(\"fractional_odd\")]\n"
            "[outputtopology(\"triangle_cw\")]\n"
            "[outputcontrolpoints(4)]\n"
            "[patchconstantfunc(\"CalculateHullConstants\")]\n"
            "DomainInput hull(InputPatch<HullInput, 4> patch, uint id : SV_OutputControlPointID)\n"
            "{\n"
            "    DomainInput o;\n"
            "    o.position = patch[id].position;\n"
            "    return o;\n"
            "}\n"

            "[domain(\"quad\")]\n"
            "PixelInput domain(HullConstants constants, const OutputPatch<DomainInput, 4> patch, float2 UV : SV_DomainLocation)\n"
            "{\n"
            "    float3 a = lerp(patch[0].position.xyz, patch[1].position.xyz, UV.x);\n"
            "    float3 b = lerp(patch[3].position.xyz, patch[2].position.xyz, UV.x);\n"
            "    float3 position = lerp(a, b, UV.y);\n"
            "    PixelInput o;\n"
            "    o.position = float4(position, 1);\n"
            "    o.colour = position;\n"
            "    return o;\n"
            "}\n"
            "float4 frag(PixelInput i) : SV_Target { return float4(i.colour, 1.0); }\n"
        ;

        inputData.requirements = kShaderRequireShaderModel50;
        inputData.p.entryName[kProgramVertex] = "vert";
        inputData.p.entryName[kProgramFragment] = "frag";
        inputData.p.entryName[kProgramHull] = "hull";
        inputData.p.entryName[kProgramDomain] = "domain";

        CHECK(TestCompilationOk(s, platform));
    }


    PARAMETRIC_TEST_FIXTURE(ShaderCompilerTestFixture, ShaderCompiler_Tessellation_HullShaderInputsDeclaredOnlyOnce, (ShaderCompilerPlatform platform), GLES3Plus_OpenGLCore)
    {
        const char *s =
            "struct HullInput\n"
            "{\n"
            "   float4 position : INTERNALTESSPOS;\n"
            "   float2 culling : COLOR0;\n"
            "};\n"

            "struct HullConstants\n"
            "{\n"
            "   float Edges[4] : SV_TessFactor;\n"
            "   float Inside[2] : SV_InsideTessFactor;\n"
            "};\n"

            "struct DomainInput\n"
            "{\n"
            "   float4 position : POSITION;\n"
            "};\n"

            "struct PixelInput\n"
            "{\n"
            "    float4 position : SV_POSITION;\n"
            "    float3 colour : COLOR0;\n"
            "};\n"
            "sampler2D _AmplitudeTex;\n"
            "HullInput vert(float3 position : POSITION)\n"
            "{\n"
            "    HullInput o;\n"
            "    o.position = float4(position, 0);\n"
            "    o.culling.x = tex2Dlod(_AmplitudeTex, float4(position.xz, 0, 0)).r;\n"
            "    o.culling.y = 91.9f; // epsilon is half_tile_size * sqrt(2)\n"
            "    return o;\n"
            "}\n"

            "HullConstants CalculateHullConstants(InputPatch<HullInput, 4> patch)\n"
            "{\n"
            "    HullConstants o;\n"
            "    // Terrain and frustum culling\n"
            "    if (!any(float4(patch[0].culling.x, patch[1].culling.x, patch[2].culling.x, patch[3].culling.x)) ||\n"
            "        !any(float4(patch[0].culling.y, patch[1].culling.y, patch[2].culling.y, patch[3].culling.y)))\n"
            "    {\n"
            "        o.Edges[0] = o.Edges[1] = o.Edges[2] = o.Edges[3] = o.Inside[0] = o.Inside[1] = 0;\n"
            "    }\n"
            "    else\n"
            "    {\n"
            "        o.Edges[0] = (patch[3].position.x - patch[0].position.x);\n"
            "        o.Edges[1] = (patch[0].position.y - patch[1].position.y);\n"
            "        o.Edges[2] = (patch[1].position.z - patch[2].position.z);\n"
            "        o.Edges[3] = (patch[2].position.w - patch[3].position.w);\n"
            "        o.Inside[0] = o.Inside[1] = min(min(o.Edges[0], o.Edges[1]), min(o.Edges[2], o.Edges[3]));\n"
            "    }\n"
            "    return o;\n"
            "}\n"

            "[domain(\"quad\")]\n"
            "[partitioning(\"fractional_odd\")]\n"
            "[outputtopology(\"triangle_cw\")]\n"
            "[outputcontrolpoints(4)]\n"
            "[patchconstantfunc(\"CalculateHullConstants\")]\n"
            "DomainInput hull(InputPatch<HullInput, 4> patch, uint id : SV_OutputControlPointID)\n"
            "{\n"
            "    DomainInput o;\n"
            "    o.position = patch[id].position;\n"
            "    return o;\n"
            "}\n"

            "[domain(\"quad\")]\n"
            "PixelInput domain(HullConstants constants, const OutputPatch<DomainInput, 4> patch, float2 UV : SV_DomainLocation)\n"
            "{\n"
            "    float3 a = lerp(patch[0].position.xyz, patch[1].position.xyz, UV.x);\n"
            "    float3 b = lerp(patch[3].position.xyz, patch[2].position.xyz, UV.x);\n"
            "    float3 position = lerp(a, b, UV.y);\n"
            "    PixelInput o;\n"
            "    o.position = float4(position, 1);\n"
            "    o.colour = position;\n"
            "    return o;\n"
            "}\n"
            "float4 frag(PixelInput i) : SV_Target { return float4(i.colour, 1.0); }\n"
        ;

        inputData.requirements = kShaderRequireShaderModel50;
        inputData.p.entryName[kProgramVertex] = "vert";
        inputData.p.entryName[kProgramFragment] = "frag";
        inputData.p.entryName[kProgramHull] = "hull";
        inputData.p.entryName[kProgramDomain] = "domain";

        CHECK(TestCompilationOk(s, platform));

        core::string outputString;
        core::string generatedInput = platform == kShaderCompPlatformGLES3Plus ? "in highp vec4 vs_INTERNALTESSPOS0 []" : "in  vec4 vs_INTERNALTESSPOS0 []";
        AssignOutputString(outputString);

        CHECK_MSG(outputString.find(generatedInput) != core::string::npos, "in vec4 vs_INTERNALTESSPOS0 [] not found in output");  // Check value exists

        // Make sure we only have one declared
        size_t numValues = 0;
        for (size_t pos = 0;; ++pos, ++numValues)
        {
            pos = outputString.find(generatedInput, pos);
            if (pos == core::string::npos)
                break;
        }
        CHECK_MSG(numValues == 1, "in vec4 vs_INTERNALTESSPOS0 [] found more than once in output");
    }
}


#endif // ENABLE_UNIT_TESTS
