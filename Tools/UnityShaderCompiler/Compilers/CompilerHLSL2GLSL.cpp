#include "UnityPrefix.h"
#include "../ShaderCompiler.h"
#include "../PlatformPlugin.h" // for GLSLIncludeHandler
#include "../Utilities/ProcessIncludes.h"
#include "Runtime/Utilities/PathNameUtility.h"

#include "Runtime/Utilities/Word.h"
#include <map>

#include "External/ShaderCompilers/glsl-optimizer/src/glsl/glsl_optimizer.h"
#include "External/ShaderCompilers/hlsl2glslfork/include/hlsl2glsl.h"

#include "../GLES20Compiler/GLESCompilerUtils.h"

//#ifdef _DEBUG
//#define DEBUG_DUMP_HLSL2GLSL_OUTPUT_FILES 1
//#else
#define DEBUG_DUMP_HLSL2GLSL_OUTPUT_FILES 0
//#endif

#define USE_HLSLCC_FOR_GLES20 0

using core::string;
using std::vector;


extern ShaderGpuProgramType GetGLSLGpuProgramType(ShaderRequirements requirements, ShaderCompilerPlatform platform);


static glslopt_ctx* s_GlslOptContext;
static glslopt_ctx* s_GlslOptContextES;

static ShaderCompileOutputData* MakeOutputDataHLSL2GLSL(const ShaderCompileInputInterface* input)
{
    return new ShaderCompileOutputData(input->api, ShaderCompileOutputData::kStringOutputData);
}

static void AddGLSLDefineHLSL2GLSL(core::string& src)
{
    src.insert(0, "#define SHADER_TARGET_GLSL\n");
}

static core::string GetGLSLVersionString(ShaderCompilerPlatform api)
{
    switch (api)
    {
        case kShaderCompPlatformGLES20:
            return "#version 100\n";
        case kShaderCompPlatformGLES3Plus:
            return "#version 300 es\n"; // Just in case, not really used.
        default:
            return "#version 120\n";
    }
}

struct KnownHLSL2GLSLInput
{
    const char* name;
    ShaderChannel channel;
};

static const KnownHLSL2GLSLInput kKnownHLSL2GLSLInputs[] =
{
    { "_glesVertex", kShaderChannelVertex },
    { "_glesNormal", kShaderChannelNormal },
    { "_glesTANGENT", kShaderChannelTangent },
    { "_glesColor", kShaderChannelColor },
    { "_glesMultiTexCoord0", kShaderChannelTexCoord0 },
    { "_glesMultiTexCoord1", kShaderChannelTexCoord1 },
    { "_glesMultiTexCoord2", kShaderChannelTexCoord2 },
    { "_glesMultiTexCoord3", kShaderChannelTexCoord3 },
    { "_glesMultiTexCoord4", kShaderChannelTexCoord4 },
    { "_glesMultiTexCoord5", kShaderChannelTexCoord5 },
    { "_glesMultiTexCoord6", kShaderChannelTexCoord6 },
    { "_glesMultiTexCoord7", kShaderChannelTexCoord7 },

    { "gl_Vertex", kShaderChannelVertex },
    { "gl_Normal", kShaderChannelNormal },
    { "TANGENT", kShaderChannelTangent },
    { "gl_Color", kShaderChannelColor },
    { "gl_MultiTexCoord0", kShaderChannelTexCoord0 },
    { "gl_MultiTexCoord1", kShaderChannelTexCoord1 },
    { "gl_MultiTexCoord2", kShaderChannelTexCoord2 },
    { "gl_MultiTexCoord3", kShaderChannelTexCoord3 },
    { "gl_MultiTexCoord4", kShaderChannelTexCoord4 },
    { "gl_MultiTexCoord5", kShaderChannelTexCoord5 },
    { "gl_MultiTexCoord6", kShaderChannelTexCoord6 },
    { "gl_MultiTexCoord7", kShaderChannelTexCoord7 },
};


static void OutputGlslInputBindings_Optimizer(glslopt_shader* shader, ShaderCompilerReflectionReport* reflectionReport)
{
    const int inputCount = glslopt_shader_get_input_count(shader);
    for (int i = 0; i < inputCount; ++i)
    {
        const char* name;
        glslopt_basic_type type;
        glslopt_precision prec;
        int vecSize, matSize, arrSize, location;
        glslopt_shader_get_input_desc(shader, i, &name, &type, &prec, &vecSize, &matSize, &arrSize, &location);

        for (int j = 0; j < sizeof(kKnownHLSL2GLSLInputs) / sizeof(kKnownHLSL2GLSLInputs[0]); ++j)
        {
            if (!strcmp(name, kKnownHLSL2GLSLInputs[j].name))
            {
                reflectionReport->OnInputBinding(kKnownHLSL2GLSLInputs[j].channel, kVertexCompNone);
                break;
            }
        }
    }
}

static bool OptimizeGLSL(glslopt_ctx* ctx, core::string& source, ShaderCompilerProgram type, ShaderCompilerReflectionReport* reflectionReport)
{
    static glslopt_shader_type kTypes[kProgramCount] = { kGlslOptShaderVertex, kGlslOptShaderFragment, (glslopt_shader_type) - 1, (glslopt_shader_type) - 1, (glslopt_shader_type) - 1, (glslopt_shader_type) - 1 };
    glslopt_shader* shader = glslopt_optimize(ctx, kTypes[type], source.c_str(), 0);
    bool ok = true;
    if (!glslopt_get_status(shader))
    {
        source += "/* HLSL2GLSL - NOTE: GLSL optimization failed\n";
        source += glslopt_get_log(shader);
        source += "*/\n";
        ok = false;
    }
    else
    {
        source = glslopt_get_output(shader);
        if (type == kProgramVertex && reflectionReport)
            OutputGlslInputBindings_Optimizer(shader, reflectionReport);
        if (reflectionReport)
        {
            int statsAlu, statsTex, statsFlow;
            glslopt_shader_get_stats(shader, &statsAlu, &statsTex, &statsFlow);
            reflectionReport->OnStatsInfo(statsAlu, statsTex, statsFlow, 0);
        }
    }
    glslopt_shader_delete(shader);

    return ok;
}

static ETargetVersion GetTargetVersion(ShaderCompilerPlatform shaderApi)
{
    switch (shaderApi)
    {
        case kShaderCompPlatformGLES20:
            return ETargetGLSL_ES_100;
        case kShaderCompPlatformGLES3Plus:
            return ETargetGLSL_ES_300;

        default:
            return ETargetGLSL_110;
    }
}

static void ParseHLSL2GLSLErrors(ShHandle handle, GLSLIncludeHandler &handler, const ShaderCompileInputInterface* input, ShaderImportErrors& errors)
{
    const core::string infoLog = Hlsl2Glsl_GetInfoLog(handle);
    ParseErrorMessages(infoLog, false, "): ERROR: ", input->GetEntryName(), input->startLine, input->api, errors);
    ParseErrorMessages(infoLog, false, "): INTERNAL ERROR: ", input->GetEntryName(), input->startLine, input->api, errors);
    ParseErrorMessages(infoLog, false, "): UNKNOWN ERROR: ", input->GetEntryName(), input->startLine, input->api, errors);
    ParseErrorMessages(infoLog, true, "): WARNING: ", input->GetEntryName(), input->startLine, input->api, errors);

    handler.UpdateErrorsWithFullPathnames(errors);
}

static void CompileHLSL2GLSL(const core::string& source, const core::string &defines, const ShaderCompileInputInterface *input, ShaderCompileOutputData* output)
{
    core::string glslVertText, glslFragText;

    ShHandle parsers[2] =
    {
        Hlsl2Glsl_ConstructCompiler(EShLangVertex),
        Hlsl2Glsl_ConstructCompiler(EShLangFragment)
    };

    GLSLIncludeHandler handler(input, output->errors, input->api);
    Hlsl2Glsl_ParseCallbacks callbacks;

    handler.SetupHlsl2GlslCallbacks(callbacks);

    const char* sourceStr = source.c_str();
    const int options = ETranslateOpForceBuiltinAttribNames;
    const ETargetVersion targetGL = GetTargetVersion(input->api);

    if (!Hlsl2Glsl_Parse(parsers[0], sourceStr, targetGL, &callbacks, options))
    {
        ParseHLSL2GLSLErrors(parsers[0], handler, input, output->errors);
        goto _cleanup;
    }
    if (!Hlsl2Glsl_Parse(parsers[1], sourceStr, targetGL, &callbacks, options))
    {
        ParseHLSL2GLSLErrors(parsers[1], handler, input, output->errors);
        goto _cleanup;
    }

    static EAttribSemantic kAttribSemantic[] =
    {
        EAttrSemTangent,
    };
    static const char* kAttribString[] =
    {
        "TANGENT",
    };
    Hlsl2Glsl_SetUserAttributeNames(parsers[0], kAttribSemantic, kAttribString, 1);
    Hlsl2Glsl_SetUserAttributeNames(parsers[1], kAttribSemantic, kAttribString, 1);

    if (!Hlsl2Glsl_Translate(parsers[0], input->GetEntryName(kProgramVertex), targetGL, options))
    {
        ParseHLSL2GLSLErrors(parsers[0], handler, input, output->errors);
        goto _cleanup;
    }
    glslVertText = Hlsl2Glsl_GetShader(parsers[0]);
    if (!Hlsl2Glsl_Translate(parsers[1], input->GetEntryName(kProgramFragment), targetGL, options))
    {
        ParseHLSL2GLSLErrors(parsers[1], handler, input, output->errors);
        goto _cleanup;
    }
    glslFragText = Hlsl2Glsl_GetShader(parsers[1]);

    if (input->api == kShaderCompPlatformGLES20 || input->api == kShaderCompPlatformGLES3Plus)
    {
        core::string prefix = GenerateGLSLShaderPrefix(glslVertText, kProgramVertex, true, input->api != kShaderCompPlatformGLES20, input->api != kShaderCompPlatformGLES20);
        glslVertText.insert(FindShaderSplitPoint(glslVertText), prefix);
        glslVertText = FixTangentBinding(glslVertText, input->api != kShaderCompPlatformGLES20);

        prefix = GenerateGLSLShaderPrefix(glslFragText, kProgramFragment, true, input->api != kShaderCompPlatformGLES20, input->api != kShaderCompPlatformGLES20);
        glslFragText.insert(FindShaderSplitPoint(glslFragText), prefix);
    }

    // optimize resulting GLSL
    if (!HasFlag(input->compileFlags, kShaderCompFlags_SkipGLSLOptimize))
    {
        // need to re-add defines here, for platform specific differences coming at the glsl level
        // (after initial preprocessing). currently this is only xll_matrixindexdynamic().
        glslVertText = defines + glslVertText;
        glslFragText = defines + glslFragText;

        #if DEBUG_DUMP_HLSL2GLSL_OUTPUT_FILES
        {
            FILE* f = fopen("debug-glsl-3.txt", "wb");
            fputs(glslVertText.c_str(), f);
            fputs(glslFragText.c_str(), f);
            fclose(f);
        }
        #endif
        glslopt_ctx* ctx = NULL;
        switch (targetGL)
        {
            case ETargetGLSL_ES_100:
                ctx = s_GlslOptContextES;
                break;
            default:
                ctx = s_GlslOptContext;
                break;
        }
        OptimizeGLSL(ctx, glslVertText, kProgramVertex, input->reflectionReport);
        OptimizeGLSL(ctx, glslFragText, kProgramFragment, input->reflectionReport);

        #if DEBUG_DUMP_HLSL2GLSL_OUTPUT_FILES
        {
            FILE* f = fopen("debug-glsl-3o.txt", "wb");
            fputs(glslVertText.c_str(), f);
            fputs(glslFragText.c_str(), f);
            fclose(f);
        }
        #endif
    }

    // both vertex & fragment texts will have common prefix with structures & whatnot
    {
        output->gpuProgramType = GetGLSLGpuProgramType(input->requirements, input->api);
        core::string str = GetGLSLVersionString(input->api) + "\n";
        str += "#ifdef VERTEX\n";
        str += glslVertText + "\n#endif\n";
        str += "#ifdef FRAGMENT\n";
        if (input->api == kShaderCompPlatformGLES20 || input->api == kShaderCompPlatformGLES3Plus)
        {
            size_t precisionPoint = FindShaderSplitPoint(glslFragText);
            str.append(glslFragText.c_str(), precisionPoint);
            if (input->api == kShaderCompPlatformGLES20)
                str += "#ifdef GL_FRAGMENT_PRECISION_HIGH\nprecision highp float;\n#else\nprecision mediump float;\n#endif\nprecision highp int;\n";
            else
                str += "precision highp float;\nprecision highp int;\n";
            str.append(glslFragText.c_str() + precisionPoint, glslFragText.length() - precisionPoint);
        }
        else
            str += glslFragText;
        str += "\n#endif\n";

        output->stringOutput = str;
    }

_cleanup:
    Hlsl2Glsl_DestructCompiler(parsers[0]);
    Hlsl2Glsl_DestructCompiler(parsers[1]);
}

ShaderCompileOutputData* InvokeCompilerGLSLOld(const ShaderCompileInputInterface *input)
{
    ShaderCompileOutputData* output = MakeOutputDataHLSL2GLSL(input);

    Assert(!(input->api != kShaderCompPlatformGLES20 && input->api != kShaderCompPlatformGLES3Plus));
    if (input->programType != kProgramVertex)
    {
        // actual fragment program was emitted as part of vertex shader;
        // just output dummy header for fragment one
        output->gpuProgramType = GetGLSLGpuProgramType(input->requirements, input->api);
        return output;
    }

    // check
    if (input->GetEntryName(kProgramVertex)[0] == 0 || input->GetEntryName(kProgramFragment)[0] == 0)
    {
        output->errors.AddImportError("Compiling for GLSL requires both vertex & fragment programs", input->startLine, false, input->api);
        return output;
    }

    core::string source(input->GetSource(), input->GetSourceLength());

    AddGLSLDefineHLSL2GLSL(source);

    core::string defines = GetGLSLDefinesString(input, input->api);
    source = defines + source;

    #if DEBUG_DUMP_HLSL2GLSL_OUTPUT_FILES
    {
        FILE* f = fopen("debug-glsl-1.txt", "wb");
        fputs(source.c_str(), f);
        fclose(f);
    }
    #endif

    CompileHLSL2GLSL(source, defines, input, output);

    #if DEBUG_DUMP_HLSL2GLSL_OUTPUT_FILES
    {
        FILE* f = fopen("debug-glsl-4.txt", "wb");
        fputs(output->stringOutput.c_str(), f);
        fclose(f);
    }
    #endif

    return output;
}

struct HLSL2GLSLPluginInterface : public PlatformPluginInterface
{
    // NB this is on purpose - hlsl2glsl is being deprecated and should not be used for anything
    // NB apart from case of gles2 hlslcc opt-out
    virtual unsigned int GetShaderPlatforms() { return 0; }
    virtual const char* GetPlatformName() { return "hlsl2glsl"; }

    virtual int  RunPluginUnitTests(const char* includesPath)
    {
        // if compiler is moved to a dll - this should run the tests at the bottom of this file
        return 0;
    }

    virtual void Shutdown()
    {
        Hlsl2Glsl_Shutdown();
        glslopt_cleanup(s_GlslOptContext);
        glslopt_cleanup(s_GlslOptContextES);

        // uncomment if compiler is moved to a dll
        // RegisterRuntimeInitializeAndCleanup::ExecuteCleanup();
    }

    virtual const ShaderCompileOutputInterface* CompileShader(const ShaderCompileInputInterface *input)
    {
        ShaderCompileOutputData* output = InvokeCompilerGLSLOld(input);
        output->compileFlags |= kShaderCompFlags_PreferHLSL2GLSL;

        if (input->api == kShaderCompPlatformGLES20 && !output->errors.HasErrors())
            output->gpuProgramType = kShaderGpuProgramGLES;

        return output;
    }

    virtual const ShaderCompileOutputInterface* DisassembleShader(const ShaderCompileInputInterface *input)
    {
        // shader data is just text, so disassembly is trivial

        ShaderCompileOutputData* output = MakeOutputDataHLSL2GLSL(input);

        const UInt8* compiledData = input->GetCompiledDataPtr();

        if (compiledData)
            output->disassembly.assign(compiledData, compiledData + input->GetCompiledDataSize());
        else
            output->disassembly = "// All GLSL source is contained within the vertex program";

        return output;
    }

    virtual const ShaderCompileOutputInterface* CompileComputeShader(const ShaderCompileInputInterface *input)
    {
        ShaderCompileOutputData* output = MakeOutputDataHLSL2GLSL(input);

        output->errors.AddImportError("Compute shader compilation not supported on this API",
            input->GetSourceFilename(), input->startLine, false, input->api);

        return output;
    }

    virtual void ReleaseShader(const ShaderCompileOutputInterface* data)
    {
        delete data;
    }
};

static HLSL2GLSLPluginInterface gHLSL2GLSLPlugin;

// PLUGINAPI PlatformPluginInterface * PLUGINCONVENTION  GetPluginInterface(int cgBatchVersion, PlatformPluginDispatcher* dispatcher)
PlatformPluginInterface* GetHLSL2GLSLPluginInterface(int cgBatchVersion, PlatformPluginDispatcher* dispatcher)
{
    // if it's moved to a dll, uncomment this and corresponding cleanup in HLSL2GLSLPluginInterface::Shutdown
    // critical for correct handling of FastPropertyNames in compute shaders from shader compilers in DLLs
    // RegisterRuntimeInitializeAndCleanup::ExecuteInitializations();

    const int cgBatchVersion_min = 0x0002;
    const int cgBatchVersion_max = 0x0002;

    // user version to plugin to be rejected by cgbatch or reject itself
    if (cgBatchVersion > cgBatchVersion_max || cgBatchVersion < cgBatchVersion_min)
        return NULL;

    Hlsl2Glsl_Initialize();
    s_GlslOptContext = glslopt_initialize(kGlslTargetOpenGL);
    s_GlslOptContextES = glslopt_initialize(kGlslTargetOpenGLES20);

    return &gHLSL2GLSLPlugin;
}

#if ENABLE_UNIT_TESTS
#include "Runtime/Testing/Testing.h"
// HLSL2GLSL used to have problems (asserts) when doing a no-op matrix casts.
// Ensure this never happens again.
INTEGRATION_TEST_SUITE(CompilerHLSL2GLSL)
{
    TEST(hlsl2glslCanHandleNoopMatrixConversions)
    {
        const char* source =
            "float4x4 mvp;\n"
            "float4 vert (float4 pos : POSITION) : POSITION {\n"
            "return mul ((half4x4)mvp, pos);\n" // <-- (half4x4)mvp was causing troubles
            "}";

        ShHandle parser = Hlsl2Glsl_ConstructCompiler(EShLangVertex);
        CHECK(Hlsl2Glsl_Parse(parser, source, ETargetGLSL_ES_100, NULL, ETranslateOpForceBuiltinAttribNames) != 0);
        CHECK(Hlsl2Glsl_Translate(parser, "vert", ETargetGLSL_ES_100, ETranslateOpForceBuiltinAttribNames) != 0);
        core::string output = Hlsl2Glsl_GetShader(parser);

        core::string expected =
            "uniform highp mat4 mvp;\n"
            "#line 2\n"
            "highp vec4 vert( in highp vec4 pos ) {\n"
            "    #line 3\n"
            "    return (mvp * pos);\n"
            "}\n"
            "void main() {\n"
            "    highp vec4 xl_retval;\n"
            "    xl_retval = vert( vec4(gl_Vertex));\n"
            "    gl_Position = vec4(xl_retval);\n"
            "}\n";
        CHECK_EQUAL(expected, output);

        Hlsl2Glsl_DestructCompiler(parser);
    }
    TEST(PositionSemanticForcesHihgp)
    {
        const char* source =
            "float4x4 mvp;\n"
            "void vert(half4 inPos:POSITION, out half4 outPos:POSITION)\n"
            "{\n"
            "    outPos = mul(mvp, inPos);\n"
            "}\n";

        ShHandle parser = Hlsl2Glsl_ConstructCompiler(EShLangVertex);
        CHECK(Hlsl2Glsl_Parse(parser, source, ETargetGLSL_ES_100, NULL, ETranslateOpForceBuiltinAttribNames) != 0);
        CHECK(Hlsl2Glsl_Translate(parser, "vert", ETargetGLSL_ES_100, ETranslateOpForceBuiltinAttribNames) != 0);
        core::string output = Hlsl2Glsl_GetShader(parser);

        core::string expected =
            "uniform highp mat4 mvp;\n"
            "#line 2\n"
            "void vert( in highp vec4 inPos, out highp vec4 outPos ) {\n"
            "    #line 4\n"
            "    outPos = (mvp * inPos);\n"
            "}\n"
            "void main() {\n"
            "    highp vec4 xlt_outPos;\n"
            "    vert( vec4(gl_Vertex), xlt_outPos);\n"
            "    gl_Position = vec4(xlt_outPos);\n"
            "}\n";
        CHECK_EQUAL(expected, output);

        Hlsl2Glsl_DestructCompiler(parser);
    }
}

#endif // ENABLE_UNIT_TESTS
