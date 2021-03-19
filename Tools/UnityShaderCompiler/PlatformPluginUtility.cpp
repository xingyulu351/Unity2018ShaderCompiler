#include "UnityPrefix.h"
#include "ShaderCompiler.h"
#include "Editor/Src/Utility/TextUtilities.h"
#include "Runtime/Utilities/File.h"
#include "Runtime/GfxDevice/BuiltinShaderParamsNames.h"
#include "Runtime/GfxDevice/GfxUtilities.h"
#include "Utilities/ProcessIncludes.h"
#include "APIPlugin.h"
#include "PlatformPlugin.h"

const char * kShaderCompPlatformDefines[] =
{
    "SHADER_API_OPENGL_REMOVED",
    "SHADER_API_D3D9_REMOVED",
    "SHADER_API_XBOX360_REMOVED",
    "SHADER_API_PS3_REMOVED",
    "SHADER_API_D3D11",
    "SHADER_API_GLES",
    "SHADER_API_GLES2DESKTOP_REMOVED",
    "SHADER_API_FLASH_REMOVED",
    "SHADER_API_D3D11_9X_REMOVED",
    "SHADER_API_GLES3",
    "SHADER_API_PSP2_REMOVED",
    "SHADER_API_PS4",
    "SHADER_API_XBOXONE",
    "SHADER_API_PSM_REMOVED",
    "SHADER_API_METAL",
    "SHADER_API_GLCORE",
    "SHADER_API_N3DS_REMOVED",
    "SHADER_API_WIIU_REMOVED",
    "SHADER_API_VULKAN",
    "SHADER_API_SWITCH",
    "SHADER_API_XBOXONE",
};

CompileTimeAssertArraySize(kShaderCompPlatformDefines, kShaderCompPlatformCount);

const char* GetCompilerPlatformDefine(ShaderCompilerPlatform api)
{
    if (api < kShaderCompPlatformCount)
        return kShaderCompPlatformDefines[api];

    return "SHADER_API_UNKNOWN";
}

// Features that are *always* supported by each compiler backend platform; i.e. no matter what the target shader compilation
// level is, these features are always there (e.g. on d3d11, SM4.0 is the minspec; a lower target just does not exist)
static const ShaderRequirements kShaderCompPlatformMinSpecFeatures[] =
{
    kShaderRequireNothing,          // unused (was GLLegacy)
    kShaderRequireNothing,          // unused (was D3D9)
    kShaderRequireNothing,          // unused (was Xbox360)
    kShaderRequireNothing,          // unused (was PS3)
    kShaderRequireShaderModel40_PC,    // D3D11
    kShaderRequireBaseShaders | kShaderRequireFragCoord,  // GLES2.0: gl_FragCoord
    kShaderRequireNothing,          // unused (was NaCl)
    kShaderRequireNothing,          // unused (was Flash)
    kShaderRequireShaderModel20,    // D3D11 feature level 9.1
    kShaderRequireShaderModel35_ES3,// GLES3.0+: SM3.5
    kShaderRequireNothing,          // unused (was PSP2)
    kShaderRequireShaderModel50_PC, // PS4
    kShaderRequireShaderModel50_PC, // XboxOne
    kShaderRequireNothing,          // unused (was PSM)
    kShaderRequireShaderModel45_ES31,// Metal: SM4.5
    kShaderRequireShaderModel35_ES3,// GLCore: SM3.5
    kShaderRequireNothing,          // unused (was N3DS)
    kShaderRequireNothing,          // unused (was Wii U)
    kShaderRequireShaderModel45_ES31,   // Vulkan
    kShaderRequireShaderModel45_ES31,   // Switch
    kShaderRequireShaderModel50_PC,     // Xbox One D3D12
};
CompileTimeAssertArraySize(kShaderCompPlatformMinSpecFeatures, kShaderCompPlatformCount);


// All possibly supported features by each compiler backend platform (if a feature is not there, that means said platform can not do it at all)
static const ShaderRequirements kShaderCompPlatformFeatures[] =
{
    kShaderRequireNothing,          // unused (was GLLegacy)
    kShaderRequireNothing,          // unused (was D3D9)
    kShaderRequireNothing,          // unused (was Xbox360)
    kShaderRequireNothing,          // unused (was PS3)
    kShaderRequireShaderModel50_PC | kShaderRequireSparseTex,    // D3D11: SM5.0 + sparse textures
    kShaderRequireShaderModel30 | kShaderRequireMRT4, // GLES2.0: SM3.0 + MRT (via extensions)
    kShaderRequireNothing,          // unused (was NaCl)
    kShaderRequireNothing,          // unused (was Flash)
    kShaderRequireShaderModel25_93, // D3D11 feature level 9.3 (almost SM3, but only 8 interpolators, no TexLOD, no ClipPos input)
    kShaderRequireShaderModel50_PC | kShaderRequireFramebufferFetch,    // GLES3.0+: SM5.0 (via ES3.1+AEP) + framebuffer fetch
    kShaderRequireNothing,          // unused (was PSP2)
    kShaderRequireShaderModel50_PC, // PS4
    kShaderRequireShaderModel50_PC, // XboxOne
    kShaderRequireNothing,          // unused (was PSM)
    kShaderRequireShaderModel50_Metal | kShaderRequireFramebufferFetch | kShaderRequireInterpolators32 | kShaderRequireMRT8 | kShaderRequireCubeArray, // Metal (SM5 without geometry shader)
    kShaderRequireShaderModel50_PC, // GLCore
    kShaderRequireNothing,          // unused (was N3DS)
    kShaderRequireNothing,          // unused (was Wii U)
    kShaderRequireShaderModel50_PC, // Vulkan
    kShaderRequireShaderModel50_PC, // Switch
    kShaderRequireShaderModel50_PC, // Xbox One D3D12
};
CompileTimeAssertArraySize(kShaderCompPlatformFeatures, kShaderCompPlatformCount);

ShaderRequirements GetCompilePlatformMinSpecFeatures(ShaderCompilerPlatform api)
{
    Assert(api >= 0 && api < kShaderCompPlatformCount);
    return kShaderCompPlatformMinSpecFeatures[api];
}

ShaderRequirements GetCompilePlatformSupportedFeatures(ShaderCompilerPlatform api)
{
    Assert(api >= 0 && api < kShaderCompPlatformCount);
    return kShaderCompPlatformFeatures[api];
}

static void AddShaderTargetRequirementsMacrosImpl(ShaderRequirements requirements, const core::string& targetMacroName, const core::string& featureMacroPrefix, ShaderArgs& inoutArgs)
{
    ShaderTargetLevel target = GetApproximateShaderTargetLevelFromRequirementsMask(requirements);
    int version = 30;
    switch (target)
    {
        case kShaderTarget20:       version = 20; break;
        case kShaderTarget25_93:    version = 25; break;// DX11 FL9.3 ~ level 25
        case kShaderTarget30:       version = 30; break;
        case kShaderTarget35_ES3:   version = 35; break;// ES 3.0 ~ level 35
        case kShaderTarget40:       version = 40; break;
        case kShaderTarget45_ES31:  version = 45; break;// ES 3.1 ~ level 45
        case kShaderTarget46_GL41:  version = 46; break;// GL 4.1 ~ level 46
        case kShaderTarget50:       version = 50; break;
        default: Assert(false);
    }
    inoutArgs.push_back(ShaderCompileArg(targetMacroName, IntToString(version)));

    if (HasFlag(requirements, kShaderRequireInterpolators10))
        inoutArgs.push_back(ShaderCompileArg(featureMacroPrefix + "INTERPOLATORS10", "1"));
    if (HasFlag(requirements, kShaderRequireInterpolators15Integers))
    {
        inoutArgs.push_back(ShaderCompileArg(featureMacroPrefix + "INTERPOLATORS15", "1"));
        inoutArgs.push_back(ShaderCompileArg(featureMacroPrefix + "INTEGERS", "1"));
    }
    if (HasFlag(requirements, kShaderRequireInterpolators32))
        inoutArgs.push_back(ShaderCompileArg(featureMacroPrefix + "INTERPOLATORS32", "1"));
    if (HasFlag(requirements, kShaderRequireMRT4))
        inoutArgs.push_back(ShaderCompileArg(featureMacroPrefix + "MRT4", "1"));
    if (HasFlag(requirements, kShaderRequireDerivatives))
        inoutArgs.push_back(ShaderCompileArg(featureMacroPrefix + "DERIVATIVES", "1"));
    if (HasFlag(requirements, kShaderRequireSampleLOD))
        inoutArgs.push_back(ShaderCompileArg(featureMacroPrefix + "SAMPLELOD", "1"));
    if (HasFlag(requirements, kShaderRequireFragCoord))
        inoutArgs.push_back(ShaderCompileArg(featureMacroPrefix + "FRAGCOORD", "1"));
    if (HasFlag(requirements, kShaderRequire2DArray))
        inoutArgs.push_back(ShaderCompileArg(featureMacroPrefix + "2DARRAY", "1"));
    if (HasFlag(requirements, kShaderRequireInstancing))
        inoutArgs.push_back(ShaderCompileArg(featureMacroPrefix + "INSTANCING", "1"));
    if (HasFlag(requirements, kShaderRequireGeometry))
        inoutArgs.push_back(ShaderCompileArg(featureMacroPrefix + "GEOMETRY", "1"));
    if (HasFlag(requirements, kShaderRequireCubeArray))
        inoutArgs.push_back(ShaderCompileArg(featureMacroPrefix + "CUBEARRAY", "1"));
    if (HasFlag(requirements, kShaderRequireCompute))
        inoutArgs.push_back(ShaderCompileArg(featureMacroPrefix + "COMPUTE", "1"));
    if (HasFlag(requirements, kShaderRequireRandomWrite))
        inoutArgs.push_back(ShaderCompileArg(featureMacroPrefix + "RANDOMWRITE", "1"));
    if (HasFlag(requirements, kShaderRequireTessHW))
        inoutArgs.push_back(ShaderCompileArg(featureMacroPrefix + "TESSHW", "1"));
    if (HasFlag(requirements, kShaderRequireTessellation))
        inoutArgs.push_back(ShaderCompileArg(featureMacroPrefix + "TESSELLATION", "1"));
    if (HasFlag(requirements, kShaderRequireSparseTex))
        inoutArgs.push_back(ShaderCompileArg(featureMacroPrefix + "SPARSETEX", "1"));
    if (HasFlag(requirements, kShaderRequireFramebufferFetch))
        inoutArgs.push_back(ShaderCompileArg(featureMacroPrefix + "FRAMEBUFFERFETCH", "1"));
    if (HasFlag(requirements, kShaderRequireMRT8))
        inoutArgs.push_back(ShaderCompileArg(featureMacroPrefix + "MRT8", "1"));
    if (HasFlag(requirements, kShaderRequireMSAATex))
        inoutArgs.push_back(ShaderCompileArg(featureMacroPrefix + "MSAATEX", "1"));
}

void AddShaderTargetRequirementsMacros(ShaderRequirements requirements, ShaderArgs& inoutArgs)
{
    AddShaderTargetRequirementsMacrosImpl(requirements, "SHADER_TARGET", "SHADER_REQUIRE_", inoutArgs);
}

void AddAvailableShaderTargetRequirementsMacros(ShaderRequirements requirements, ShaderArgs& inoutArgs)
{
    AddShaderTargetRequirementsMacrosImpl(requirements, "SHADER_TARGET_AVAILABLE", "SHADER_AVAILABLE_", inoutArgs);
}

core::string GetGLSLDefinesString(const ShaderCompileInputInterface* input, ShaderCompilerPlatform api)
{
    core::string ret;

    size_t numArgs = input->GetNumShaderArgs();

    // arguments passed from above
    for (size_t i = 0; i < numArgs; i++)
    {
        core::string argName(input->GetShaderArgName(i));
        ret += "#ifndef " + argName + "\n";
        ret += "    #define " + argName + " ";
        ret +=                core::string(input->GetShaderArgValue(i)) + "\n";
        ret += "#endif\n";
    }

    // feature availability macros
    ShaderRequirements availableRequirements = input->requirements | GetCompilePlatformMinSpecFeatures(api);
    ShaderArgs availableMacros;
    AddAvailableShaderTargetRequirementsMacros(availableRequirements, availableMacros);
    for (size_t i = 0; i < availableMacros.size(); i++)
    {
        ret += "#ifndef " + availableMacros[i].name + "\n";
        ret += "    #define " + availableMacros[i].name + " " + availableMacros[i].value + "\n";
        ret += "#endif\n";
    }

    // platform macro
    {
        core::string platformDefineName(kShaderCompPlatformDefines[api]);
        ret += "#ifndef " + platformDefineName + "\n";
        ret += "    #define " + platformDefineName + " 1\n";
        ret += "#endif\n";
    }

    return ret;
}

//moved these here to make them available to SwitchCgBatchPlugin (which calls InvokeCompilerHLSLcc (which calls these))
void MassageShaderCompilerErrors(ShaderImportErrors& errors, ShaderCompilerPlatform platform, ShaderRequirements requirements)
{
    if (!errors.HasErrorsOrWarnings())
        return;

    ShaderImportErrors::ErrorContainer newErrors;

    for (ShaderImportErrors::ErrorContainer::const_iterator it = errors.GetErrors().begin(); it != errors.GetErrors().end(); ++it)
    {
        ShaderImportError err = *it;

        if (!err.warning)
        {
            // Detect likely missing INTERNAL_DATA from surface shader input struct
            // and make more clear error message
            if (BeginsWith(err.message, "invalid subscript 'internalSurfaceTtoW0'") ||
                BeginsWith(err.message, "'internalSurfaceTtoW0' :  no such field in structure"))
            {
                err.message = "Surface shader Input structure needs INTERNAL_DATA for this WorldNormalVector or WorldReflectionVector usage";
            }
        }

        newErrors.insert(err);
    }

    errors.GetErrors().swap(newErrors);
}

void AddErrorsFrom(ShaderImportErrors& errors, const ShaderCompileInputInterface* input, const ShaderCompileOutputInterface* output)
{
    size_t numErrors = output->GetNumErrorsAndWarnings();
    for (size_t i = 0; i < numErrors; i++)
    {
        const char* message;
        const char* file;
        int line;
        bool warning;
        ShaderCompilerPlatform api;

        output->GetErrorOrWarning(i, message, file, line, warning, api);
        errors.AddImportError(message, file, line, warning, api);
    }

    MassageShaderCompilerErrors(errors, input->api, input->requirements);
}

void AddErrorsFrom(std::vector<ShaderImportError>& errors, const ShaderCompileInputInterface* input, const ShaderCompileOutputInterface* output)
{
    ShaderImportErrors errorSet;
    AddErrorsFrom(errorSet, input, output);

    errors.clear();
    for (ShaderImportErrors::ErrorContainer::const_iterator it = errorSet.GetErrors().begin(); it != errorSet.GetErrors().end(); ++it)
        errors.push_back(*it);
}

void ParseErrorMessages(const core::string& listing, bool warnings, const core::string& token, const core::string& entry, int startLine, ShaderCompilerPlatform api, ShaderImportErrors& outErrors)
{
    // Cg error lines look like:
    // Temp/tempfile.cg(16) : error C1008: undefined variable "aaa"
    // (0) : error C3001: no program defined

    // HLSL error lines look like:
    // (11): warning X3568: 'vertex' : unknown pragma ignored
    // (12): warning X3568: 'fragment' : unknown pragma ignored
    // (26): error X3502: Microcode Compiler 'vert': input parameter 'v' missing semantics
    // or
    // (11,3): warning X3568: 'vertex' : unknown pragma ignored
    // (11,3-13): error X3004: foobar
    //
    // Some error messages are on multiple lines, e.g.
    // filename(11,3-13): error X1234: No matching function overload
    // (11,3-13): error X1234: Possible candidates are:
    // (11,3-13): error X1234:     foo bar baz
    // We want to merge them all into one.

    size_t pos = listing.find(token, 0);
    if (pos == core::string::npos)
        return;

    core::string prevMessage;
    core::string prevFile;
    core::string prevLineText;
    int prevLine = -1;

    while (pos != core::string::npos)
    {
        // before token comes: line or line,column or line,column-column
        int numberPos = static_cast<int>(pos) - 1; // note: must be signed!
        while (numberPos >= 0 && (IsDigit(listing[numberPos]) || listing[numberPos] == ',' || listing[numberPos] == '-'))
            --numberPos;

        // before the line location comes optional filename
        const int fileEndPos = numberPos;
        int filePos = fileEndPos; // note: must be signed!
        while (filePos >= 0 && !IsSpace(listing[filePos]))
            --filePos;
        ++filePos;
        core::string fileText = listing.substr(filePos, fileEndPos - filePos);
        ConvertSeparatorsToUnity(fileText);

        ++numberPos;
        core::string lineNumberText = listing.substr(numberPos, pos - numberPos);
        int finalLineNumber = StringToInt(lineNumberText);
        if (finalLineNumber == 0)
            finalLineNumber = startLine;

        // extract error message (rest of line after token)
        core::string message = ExtractRestOfLine(listing, pos + token.size());

        // Cg & HLSL put error codes right here, which are in the form of "C0501: " etc. If we match that, then just strip it.
        if (message.size() > 7)
        {
            if (IsAlpha(message[0]) && IsDigit(message[1]) && IsDigit(message[2]) && IsDigit(message[3]) && IsDigit(message[4]) && message[5] == ':' && message[6] == ' ')
                message.erase(0, 7);
        }

        if (finalLineNumber == 0 && (message.find("no program defined") != core::string::npos))
        {
            message = "Did not find shader function '" + entry + "' to compile";
            finalLineNumber = startLine;
        }

        message = Trim(message);

        // We either want to merge with previous error in case error location matches exactly,
        // or start a new error.
        if (!prevMessage.empty() && prevLine == finalLineNumber && prevFile == fileText && lineNumberText == prevLineText)
        {
            // If current message is exactly like previous one, then skip adding it
            if (message != prevMessage)
            {
                char lastChar = prevMessage[prevMessage.size() - 1];
                if (IsAlpha(lastChar) || IsDigit(lastChar))
                    prevMessage += ';';
                prevMessage += ' ';
                prevMessage += message;
            }
        }
        else
        {
            // add previous error if we had any
            if (!prevMessage.empty())
                outErrors.AddImportError(prevMessage, prevFile, prevLine, warnings, api);
            prevMessage = message;
            prevFile = fileText;
            prevLine = finalLineNumber;
            prevLineText = lineNumberText;
        }

        pos = listing.find(token, pos + token.size());
    }

    // add previous error if we had any
    if (!prevMessage.empty())
        outErrors.AddImportError(prevMessage, prevFile, prevLine, warnings, api);
}

void ParseErrorMessagesPostfix(const core::string& listing, bool warnings, const core::string& token, const core::string& entry, int startLine, ShaderCompilerPlatform api, ShaderImportErrors& outErrors, int adjustLinesBy)
{
    // glslang error lines look like:
    // ERROR: 0:5: 'foobar' syntax error
    using namespace std;

    size_t pos = listing.find(token, 0);
    if (pos == core::string::npos)
        return;

    while (pos != core::string::npos)
    {
        pos += token.size();

        // after token comes: fileIndex:line
        size_t numberPos = pos;
        while (IsDigit(listing[numberPos]))
            ++numberPos;

        // could be a meta-message like 'ERROR: 1 compilation errors.  No code generated.'
        if (listing[numberPos] != ':')
        {
            pos = listing.find(token, numberPos);
            continue;
        }

        ++numberPos; // skip the :
        pos = numberPos;

        while (IsDigit(listing[numberPos]))
            ++numberPos;

        core::string lineNumberText = listing.substr(pos, numberPos - pos);
        int finalLineNumber = StringToInt(lineNumberText) + adjustLinesBy;
        if (finalLineNumber == 0)
            finalLineNumber = startLine;

        pos = numberPos;

        ++pos; // skip the :

        // extract error message (rest of line after token)
        core::string message = ExtractRestOfLine(listing, pos);

        message = Trim(message);

        outErrors.AddImportError(message, finalLineNumber, warnings, api);

        pos = listing.find(token, pos);
    }
}

void ParseErrorMessagesStartLine(const core::string& listing, const core::string& token, const core::string& entry, int startLine, ShaderCompilerPlatform api, ShaderImportErrors& outErrors)
{
    ParseErrorMessagesStartLine(listing, false, token, entry, startLine, api, outErrors);
}

void ParseErrorMessagesStartLine(const core::string& listing, bool warnings, const core::string& token, const core::string& entry, int startLine, ShaderCompilerPlatform api, ShaderImportErrors& outErrors)
{
    // Some HLSL error lines look like:
    // error X3502: Foo Bar
    // or
    // warning X4714: Baa For

    size_t pos = listing.find(token, 0);
    if (pos == core::string::npos)
        return;

    core::string message;
    while (pos != core::string::npos)
    {
        // check if this is actually the error (at start or preceded by newline)
        if (pos > 0)
        {
            char c = listing[pos - 1];
            if (c != '\n' && c != '\r')
                goto _next;
        }

        // extract error message (rest of line)
        message = ExtractRestOfLine(listing, pos);

        if (message.find("entrypoint not found") != core::string::npos)
        {
            message = "Did not find shader kernel '" + entry + "' to compile";
        }
        else
        {
            message = "Program '" + entry + "', " + message;
        }

        outErrors.AddImportError(message, startLine, warnings, api);

    _next:
        pos = listing.find(token, pos + token.size());
    }
}

struct SemanticToUnity
{
    const char* semantic;
    ShaderChannel channel;
    VertexComponent comp;
};

static const SemanticToUnity kSemanticToChannel[] =
{
    { "POSITION",   kShaderChannelVertex,   kVertexCompVertex },
    { "POSITION0",  kShaderChannelVertex,   kVertexCompVertex },
    { "NORMAL",     kShaderChannelNormal,   kVertexCompNormal },
    { "NORMAL0",    kShaderChannelNormal,   kVertexCompNormal },
    { "TANGENT",    kShaderChannelTangent,  kVertexCompTangent },
    { "TANGENT0",   kShaderChannelTangent,  kVertexCompTangent },
    { "COLOR",      kShaderChannelColor,    kVertexCompColor },
    { "COLOR0",     kShaderChannelColor,    kVertexCompColor },
    { "TEXCOORD0",  kShaderChannelTexCoord0, kVertexCompTexCoord0 },
    { "TEXCOORD1",  kShaderChannelTexCoord1, kVertexCompTexCoord1 },
    { "TEXCOORD2",  kShaderChannelTexCoord2, kVertexCompTexCoord2 },
    { "TEXCOORD3",  kShaderChannelTexCoord3, kVertexCompTexCoord3 },
    { "TEXCOORD4",  kShaderChannelTexCoord4, kVertexCompTexCoord4 },
    { "TEXCOORD5",  kShaderChannelTexCoord5, kVertexCompTexCoord5 },
    { "TEXCOORD6",  kShaderChannelTexCoord6, kVertexCompTexCoord6 },
    { "TEXCOORD7",  kShaderChannelTexCoord7, kVertexCompTexCoord7 },
};

void ChannelFromSemantic(const char* semantic, ShaderChannel* outShaderChannel, VertexComponent* outVertexComponent)
{
    for (int i = 0; i < ARRAY_SIZE(kSemanticToChannel); ++i)
    {
        if (!strcmp(semantic, kSemanticToChannel[i].semantic))
        {
            *outVertexComponent = kSemanticToChannel[i].comp;
            *outShaderChannel = kSemanticToChannel[i].channel;
            return;
        }
    }

    *outShaderChannel = kShaderChannelNone;
    *outVertexComponent = kVertexCompNone;
}

static const char* const kShaderChannelName[] =
{
    "VERTEX",
    "NORMAL",
    "TANGENT",
    "COLOR",
    "TEXCOORD",
    "TEXCOORD1",
    "TEXCOORD2",
    "TEXCOORD3",
    "TEXCOORD4",
    "TEXCOORD5",
    "TEXCOORD6",
    "TEXCOORD7",
    "BLENDWEIGHT",
    "BLENDINDICES",
};
CompileTimeAssertArraySize(kShaderChannelName, kShaderChannelCount);

bool IsValidChannel(const char* name)
{
    for (int i = 0; i < sizeof(kShaderChannelName) / sizeof(kShaderChannelName[0]); ++i)
    {
        if (StrIEquals(name, kShaderChannelName[i]))
            return true;
    }
    return false;
}

void MassageConstantName(char* name)
{
    size_t len = strlen(name);

    for (size_t i = 0; i < len; ++i)
    {
        // remove array indices, so _ObjSpaceLightPos[0] becomes _ObjSpaceLightPos0
        if (name[i] == '[' || name[i] == ']')
        {
            for (size_t j = i; j < len - 1; j++)
                name[j] = name[j + 1];
            --i;
        }
    }
}

static int ParseInlineSamplerWrapMode(const core::string& samplerName, const core::string& wrapName)
{
    int res = 0;
    const bool hasWrap = (samplerName.find(wrapName) != core::string::npos);
    if (!hasWrap)
        return res;

    const bool hasU = (samplerName.find(wrapName + 'u') != core::string::npos);
    const bool hasV = (samplerName.find(wrapName + 'v') != core::string::npos);
    const bool hasW = (samplerName.find(wrapName + 'w') != core::string::npos);

    if (hasWrap)
        res |= 1;
    if (hasU)
        res |= 2;
    if (hasV)
        res |= 4;
    if (hasW)
        res |= 8;
    return res;
}

// Note: if changing or extending behavior of this function, make sure to match
// the changes in Metal shader translator (External/ShaderCompilers/HLSLcc/src/toMetalDeclaration.cpp, EmitInlineSampler)
InlineSamplerType ParseInlineSamplerName(core::string samplerName, core::string& errorMsg)
{
    InlineSamplerType res;

    samplerName = ToLower(samplerName);
    // filter modes
    const bool hasPoint = (samplerName.find("point") != core::string::npos);
    const bool hasTrilinear = (samplerName.find("trilinear") != core::string::npos);
    const bool hasLinear = (samplerName.find("linear") != core::string::npos);
    const bool hasAnyFilter = hasPoint || hasTrilinear || hasLinear;

    // wrap modes
    const int bitsClamp = ParseInlineSamplerWrapMode(samplerName, "clamp");
    const int bitsRepeat = ParseInlineSamplerWrapMode(samplerName, "repeat");
    const int bitsMirror = ParseInlineSamplerWrapMode(samplerName, "mirror");
    const int bitsMirrorOnce = ParseInlineSamplerWrapMode(samplerName, "mirroronce");

    const bool hasAnyWrap = bitsClamp != 0 || bitsRepeat != 0 || bitsMirror != 0 || bitsMirrorOnce != 0;

    // depth comparison
    const bool hasCompare = (samplerName.find("compare") != core::string::npos);

    // name must contain a filter mode and a wrap mode at least
    if (!hasAnyFilter || !hasAnyWrap)
    {
        errorMsg = Format("Unrecognized sampler '%s' - does not match any texture and is not a recognized inline name (should contain filter and wrap modes)", samplerName.c_str());
        res.SetInvalid();
        return res;
    }

    // check for some conflicting modes
    if (hasPoint && hasLinear)
    {
        errorMsg = Format("Unrecognized inline sampler name '%s' - can't have both 'point' and 'linear'", samplerName.c_str());
        res.SetInvalid();
        return res;
    }
    if (bitsClamp == 1 && bitsRepeat == 1)
    {
        errorMsg = Format("Unrecognized inline sampler name '%s' - can't have both 'clamp' and 'repeat'", samplerName.c_str());
        res.SetInvalid();
        return res;
    }
    if (bitsClamp == 1 && bitsMirror == 1)
    {
        errorMsg = Format("Unrecognized inline sampler name '%s' - can't have both 'clamp' and 'mirror'", samplerName.c_str());
        res.SetInvalid();
        return res;
    }

    // we have a valid sampler name here; construct the bits
    if (hasTrilinear)
        res.flags.filter = kTexFilterTrilinear;
    else if (hasLinear)
        res.flags.filter = kTexFilterBilinear;
    else
        res.flags.filter = kTexFilterNearest;

    if (bitsClamp == 1)
        res.flags.wrapU = res.flags.wrapV = res.flags.wrapW = kTexWrapClamp;
    else if (bitsRepeat == 1)
        res.flags.wrapU = res.flags.wrapV = res.flags.wrapW = kTexWrapRepeat;
    else if (bitsMirrorOnce == 1)
        res.flags.wrapU = res.flags.wrapV = res.flags.wrapW = kTexWrapMirrorOnce;
    else if (bitsMirror == 1)
        res.flags.wrapU = res.flags.wrapV = res.flags.wrapW = kTexWrapMirror;

    if ((bitsClamp & 2) != 0)
        res.flags.wrapU = kTexWrapClamp;
    if ((bitsClamp & 4) != 0)
        res.flags.wrapV = kTexWrapClamp;
    if ((bitsClamp & 8) != 0)
        res.flags.wrapW = kTexWrapClamp;

    if ((bitsRepeat & 2) != 0)
        res.flags.wrapU = kTexWrapRepeat;
    if ((bitsRepeat & 4) != 0)
        res.flags.wrapV = kTexWrapRepeat;
    if ((bitsRepeat & 8) != 0)
        res.flags.wrapW = kTexWrapRepeat;

    if ((bitsMirrorOnce & 2) != 0)
        res.flags.wrapU = kTexWrapMirrorOnce;
    if ((bitsMirrorOnce & 4) != 0)
        res.flags.wrapV = kTexWrapMirrorOnce;
    if ((bitsMirrorOnce & 8) != 0)
        res.flags.wrapW = kTexWrapMirrorOnce;

    if ((bitsMirror & 2) != 0)
        res.flags.wrapU = kTexWrapMirror;
    if ((bitsMirror & 4) != 0)
        res.flags.wrapV = kTexWrapMirror;
    if ((bitsMirror & 8) != 0)
        res.flags.wrapW = kTexWrapMirror;

    if (hasCompare)
        res.flags.compare = 1;

    return res;
}

bool CheckSamplerAndTextureNameMatch(const char* textureName, const char* samplerName)
{
    if (StrEquals(samplerName, textureName))
        return true;
    if (BeginsWith(samplerName, "sampler") && StrEquals(samplerName + 7, textureName))
        return true;
    if (BeginsWith(samplerName, "sampler_") && StrEquals(samplerName + 8, textureName))
        return true;
    return false;
}
