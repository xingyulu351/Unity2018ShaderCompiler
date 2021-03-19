#include "UnityPrefix.h"
#include "Editor/Src/Utility/TextUtilities.h"
#include "Editor/Src/Utility/d3d11/D3D11ReflectionAPI.h"
#include "External/ShaderCompilers/HLSLcc/include/hlslcc.h"
#include "External/ShaderCompilers/HLSLcc/src/cbstring/bstrlib.h"
#include "Runtime/GfxDevice/GpuProgram.h"
#include "Runtime/GfxDevice/opengles/ApiEnumGLES.h"
#include "Runtime/Utilities/PathNameUtility.h"
#include "Runtime/Utilities/Word.h"
#include "Runtime/Utilities/ArrayUtility.h"
#include "External/ShaderCompilers/mojoshader/mojoshader.h"

#include "Tools/UnityShaderCompiler/ComputeShaderCompiler.h"
#include "Tools/UnityShaderCompiler/ShaderCompiler.h"
#include "Tools/UnityShaderCompiler/APIPlugin.h"
#include "Tools/UnityShaderCompiler/Utilities/ProcessIncludes.h"
#include "Tools/UnityShaderCompiler/MetalCompiler/MetalCompilerUtils.h"
#include <map>
#include <sstream>
#include <cstdio>
#include <cstdlib>

#ifdef _DEBUG
#define DEBUG_DUMP_HLSLcc_OUTPUT_FILES 1
#else
#define DEBUG_DUMP_HLSLcc_OUTPUT_FILES 0
#endif

#if DEBUG_DUMP_HLSLcc_OUTPUT_FILES
// When dumping debug files, keep the intermediate phases saved so we can dump them to file in case of error
static core::string s_HLSLccOriginalSourceString;
static core::string s_HLSLccPreprocessedSourceString;
#endif

#ifndef GL_VERTEX_SHADER
#define GL_VERTEX_SHADER            0x8B31
#endif
#ifndef GL_FRAGMENT_SHADER
#define GL_FRAGMENT_SHADER          0x8B30
#endif
#ifndef GL_GEOMETRY_SHADER
#define GL_GEOMETRY_SHADER          0x8DD9
#endif
#ifndef GL_TESS_EVALUATION_SHADER
#define GL_TESS_EVALUATION_SHADER   0x8E87
#endif
#ifndef GL_TESS_CONTROL_SHADER
#define GL_TESS_CONTROL_SHADER      0x8E88
#endif
#ifndef GL_COMPUTE_SHADER
#define GL_COMPUTE_SHADER           0x91B9
#endif

using core::string;
using std::vector;


extern ShaderGpuProgramType GetGLSLGpuProgramType(ShaderRequirements requirements, ShaderCompilerPlatform platform);

static ShaderCompileOutputData* MakeOutputDataHLSLcc(const ShaderCompileInputInterface* input)
{
    return new ShaderCompileOutputData(input->api, ShaderCompileOutputData::kStringOutputData);
}

struct KnownHLSLccInput
{
    const char* name;
    ShaderChannel channel;
};

static const KnownHLSLccInput kKnownHLSLccInputs[] =
{
    { "in_POSITION0", kShaderChannelVertex },
    { "in_VCOLOR0", kShaderChannelColor },
    { "in_NORMAL0", kShaderChannelNormal },
    { "in_TANGENT0", kShaderChannelTangent },
    { "in_COLOR0", kShaderChannelColor },
    { "in_TEXCOORD0", kShaderChannelTexCoord0 },
    { "in_TEXCOORD1", kShaderChannelTexCoord1 },
    { "in_TEXCOORD2", kShaderChannelTexCoord2 },
    { "in_TEXCOORD3", kShaderChannelTexCoord3 },
    { "in_TEXCOORD4", kShaderChannelTexCoord4 },
    { "in_TEXCOORD5", kShaderChannelTexCoord5 },
    { "in_TEXCOORD6", kShaderChannelTexCoord6 },
    { "in_TEXCOORD7", kShaderChannelTexCoord7 },
    // Metal inputs don't have the prefix
    { "POSITION0", kShaderChannelVertex },
    { "COLOR0", kShaderChannelColor },
    { "NORMAL0", kShaderChannelNormal },
    { "TANGENT0", kShaderChannelTangent },
    { "VCOLOR0", kShaderChannelColor },
    { "TEXCOORD0", kShaderChannelTexCoord0 },
    { "TEXCOORD1", kShaderChannelTexCoord1 },
    { "TEXCOORD2", kShaderChannelTexCoord2 },
    { "TEXCOORD3", kShaderChannelTexCoord3 },
    { "TEXCOORD4", kShaderChannelTexCoord4 },
    { "TEXCOORD5", kShaderChannelTexCoord5 },
    { "TEXCOORD6", kShaderChannelTexCoord6 },
    { "TEXCOORD7", kShaderChannelTexCoord7 },
};

static const char* kIncorrectHLSLccInputsWithSVPrefix[] =
{
    "in_SV_POSITION0",
    "SV_POSITION0"
};

// With HLSLcc, avoid breaking existing legacy shaders that use COLOR for fragment shader output,
// see if the shader does something where we should enable UNITY_FRAMEBUFFER_FETCH_ENABLED

static bool ParseNeedForFramebufferFetchEnabled(const core::string& src)
{
    const core::string kUnityFramebufferFetchAvailableToken("UNITY_FRAMEBUFFER_FETCH_AVAILABLE");

    size_t pos;
    pos = FindTokenInText(src, kUnityFramebufferFetchAvailableToken, 0);
    if (pos != core::string::npos)
    {
        // shader does something with it, assume things
        return true;
    }

    const core::string kInOutToken("inout");
    const core::string kCommaToken(",");
    const core::string kSVTargetToken("SV_Target");

    // try finding something that matches something along of "inout <type> <var> : SV_Target" oneliner
    pos = FindTokenInText(src, kInOutToken, 0);
    while (pos != core::string::npos)
    {
        // only match inout exactly
        if (!IsTabSpace(src[pos + kInOutToken.size()]))
        {
            // find next pinout
            pos += kInOutToken.size();
            pos = FindTokenInText(src, kInOutToken, pos);
            continue;
        }

        // extract line
        core::string line = ExtractRestOfLine(src, pos + kInOutToken.size(), true);
        core::string inoutLine = kInOutToken + line;

        size_t linePos = inoutLine.find(kCommaToken, 0);
        if (linePos != core::string::npos)
            inoutLine = inoutLine.substr(0, linePos);

        linePos = inoutLine.find(kSVTargetToken, 0);
        if (linePos != core::string::npos)
        {
            return true;
        }

        // find next inout
        pos += kInOutToken.size();
        pos = FindTokenInText(src, kInOutToken, pos);
    }

    return false;
}

/** Maximum of two values: */
#define HLSLCC_MAX2(A, B)   ( (A)>(B) ? (A) : (B) )

static void GetMetalTypeSize(const ShaderParamType type, int rows, int cols, bool isMatrix, int arraySize, int& size, int& alignment, bool isComputeShader)
{
    const bool isBool = (type == kShaderParamBool);
    const bool isHalf = (type == kShaderParamHalf);

    const int asize = HLSLCC_MAX2(arraySize, 1);

    size = isBool ? 1 : (isHalf ? 2 : 4);
    alignment = HLSLCC_MAX2(size, 1);

    int vsize = cols;
    // Arrays are padded to float4 size to match DX layout.
    // float3 etc in Metal has both sizeof and alignment same as float4
    if ((isComputeShader && asize > 1) || vsize == 3)
        vsize = 4;

    size *= vsize;
    alignment *= vsize;

    const int msize = isMatrix ? rows : 1;
    size *= msize;

    size *= asize;
}

// Our bridge class between Unity shader importer's reflection interface and the one in HLSLcc's
class UnityHLSLccReflection : public HLSLccReflection
{
private:
    struct ConstantInfo
    {
        ConstantInfo(const core::string &name, int bindIndex, ShaderParamType dataType, ConstantType constantType, int rows, int cols, int arraySize)
            : m_Name(name), m_BindIndex(bindIndex), m_DataType(dataType), m_ConstantType(constantType), m_Rows(rows), m_Cols(cols), m_ArraySize(arraySize) {}
        core::string m_Name;
        int m_BindIndex;
        ShaderParamType m_DataType;
        ConstantType m_ConstantType;
        int m_Rows;
        int m_Cols;
        int m_ArraySize;
    };

public:
    UnityHLSLccReflection(const ShaderCompileInputInterface *data, ShaderImportErrors &errors, MetalShaderBlobHeader &metalMetadata) : m_Data(data), m_HasErrors(false), m_Errors(errors), m_MetalMetadata(metalMetadata) {}

    virtual void OnDiagnostics(const std::string &error, int line, bool isError)
    {
        if (isError)
            m_HasErrors = true;
        m_Errors.AddImportError(error, m_Data->GetSourceFilename(), line, !isError, m_Data->api);
    }

    virtual void OnInputBinding(const std::string &name, int bindIndex)
    {
        bool isGLES = (m_Data->api == kShaderCompPlatformOpenGLCore || m_Data->api == kShaderCompPlatformGLES20 || m_Data->api == kShaderCompPlatformGLES3Plus);
        if (m_Data->reflectionReport == NULL)
            return;

        bool matched = false;
        for (int j = 0; j < ARRAY_SIZE(kKnownHLSLccInputs); ++j)
        {
            if (strcmp(name.c_str(), kKnownHLSLccInputs[j].name) == 0)
            {
                if (isGLES)
                    m_Data->reflectionReport->OnInputBinding(kKnownHLSLccInputs[j].channel, kVertexCompNone);
                else // Metal needs the bind point
                    m_Data->reflectionReport->OnInputBinding(kKnownHLSLccInputs[j].channel, (VertexComponent)(kVertexCompAttrib0 + bindIndex));
                matched = true;
                break;
            }
        }
        if (!matched)
        {
            // We ignore system value input semantic, but for the case of SV_POSITION, warn users
            // about it so they don't accidentally use it in their shaders
            for (int j = 0; j < ARRAY_SIZE(kIncorrectHLSLccInputsWithSVPrefix); ++j)
            {
                if (std::strcmp(name.c_str(), kIncorrectHLSLccInputsWithSVPrefix[j]) == 0)
                {
                    std::string msg = Format("Input semantic SV_POSITION is used; you probably want POSITION instead");
                    OnDiagnostics(msg, 0, false);
                }
            }
        }
    }

    virtual bool OnConstantBuffer(const std::string &name, size_t bufferSize, size_t memberCount)
    {
        if (m_Data->reflectionReport == NULL)
            return true;
        const bool isMetal = (m_Data->api == kShaderCompPlatformMetal);
        if (!isMetal)
            return true; // Nothing to do

        m_CurrCBPos = 0;
        m_CBContents.clear();
        m_CurrCBName = name;
        m_CurrCBMemberCount = 0;
        m_CurrCBTotalMembers = memberCount;

        //      m_Data->reflectionReport->OnConstantBuffer(name.c_str(), bufferSize, memberCount);
        return true;
    }

    #define HLSLCC_ALIGN(A, B)   ( ((A) + (B) - 1) & ~((B) - 1) )

    virtual bool OnConstant(const std::string &name, int bindIndex, SHADER_VARIABLE_TYPE cType, int rows, int cols, bool isMatrix, int arraySize, bool isUsed)
    {
        if (m_Data->reflectionReport == NULL)
            return true;

        ConstantType constantType = cType == SVT_VOID ? kConstantTypeStruct : (isMatrix ? kConstantTypeMatrix : kConstantTypeDefault);
        bool isStructArrayMember = constantType != kConstantTypeStruct && name.find("[].") != std::string::npos;

        if (constantType == kConstantTypeStruct)
        {
            // Struct is reported immediately after all it's members have been reported.
            // struct member alignment rules are same as in c

            DebugAssert(arraySize > 1);
            core::string prefix = name + "[].";

            size_t memberBegin = m_CBContents.size();
            for (; memberBegin > 0; --memberBegin)
            {
                if (m_CBContents[memberBegin - 1].m_Name.compare(0, prefix.length(), prefix) != 0)
                    break;
            }
            if (memberBegin == m_CBContents.size())
            {
                // empty struct..?
                AssertMsg(false, "Empty struct");
            }
            else
            {
                int align = 0, offset = 0;
                for (size_t i = memberBegin; i < m_CBContents.size(); ++i)
                {
                    ConstantInfo& ci = m_CBContents[i]; int msize, malign;
                    GetMetalTypeSize(ci.m_DataType, ci.m_Rows, ci.m_Cols, ci.m_ConstantType == kConstantTypeMatrix, ci.m_ArraySize, msize, malign, false);
                    ci.m_Name.erase(prefix.length() - 3, 2); // remove "[]"

                    const int memberOffset = HLSLCC_ALIGN(offset, malign); // make sure we correctly align current member
                    ci.m_BindIndex = memberOffset, offset = memberOffset + msize;
                    align = HLSLCC_MAX2(align, malign);
                }
                m_CurrCBPos = HLSLCC_ALIGN(m_CurrCBPos, align);
                int stride = HLSLCC_ALIGN(offset, align);
                // Unity expects structs to be reported ahead of all its members.
                if (isUsed)
                    m_CBContents.insert(m_CBContents.begin() + memberBegin, ConstantInfo(name, m_CurrCBPos, kShaderParamFloat, constantType, stride, 1, arraySize));
                m_CurrCBPos += stride * HLSLCC_MAX2(arraySize, 1);
            }
        }
        else
        {
            ShaderParamType type;
            switch (cType)
            {
                default:
                    AssertMsg(0, "Unknown or unsupported shader param type");
                    return true;
                case SVT_BOOL:
                    type = kShaderParamBool;
                    break;
                case SVT_FLOAT:
                    type = kShaderParamFloat;
                    break;
                case SVT_FLOAT16:
                case SVT_FLOAT10:
                    type = kShaderParamHalf;
                    break;
                case SVT_INT:
                    type = kShaderParamInt;
                    break;
                case SVT_UINT:
                    type = kShaderParamUInt;
                    break;
            }

            int loc = m_CurrCBPos;
            if (!isStructArrayMember) // only update m_CurrCBPos when the constant is not a struct member
            {
                int align, size;
                GetMetalTypeSize(type, rows, cols, isMatrix, arraySize, size, align, false);
                loc = HLSLCC_ALIGN(m_CurrCBPos, align);
                m_CurrCBPos = loc + size;
            }
            if (isUsed)
                m_CBContents.push_back(ConstantInfo(name, loc, type, constantType, rows, cols, arraySize));
            m_CurrCBMemberCount++;
        }

        if (!isStructArrayMember && m_CurrCBMemberCount == m_CurrCBTotalMembers)
        {
            // Flush only when we are not in the middle of a struct declaration
            m_Data->reflectionReport->OnConstantBuffer(m_CurrCBName.c_str(), m_CurrCBPos, (int)m_CurrCBTotalMembers);
            for (size_t i = 0; i < m_CBContents.size(); i++)
            {
                ConstantInfo &ci = m_CBContents[i];
                m_Data->reflectionReport->OnConstant(ci.m_Name.c_str(), ci.m_BindIndex, ci.m_DataType, ci.m_ConstantType, ci.m_Rows, ci.m_Cols, ci.m_ArraySize);
            }
            m_CurrCBPos = 0;
            m_CBContents.clear();
            m_CurrCBName = "";
            m_CurrCBMemberCount = 0;
            m_CurrCBTotalMembers = 0;
        }
        //      m_Data->reflectionReport->OnConstant(name.c_str(), bindIndex, type, rows, cols, isMatrix, arraySize);
        return true;
    }

    virtual void OnConstantBufferBinding(const std::string &name, int bindIndex)
    {
        if (m_Data->reflectionReport == NULL)
            return;
        m_Data->reflectionReport->OnCBBinding(name.c_str(), bindIndex);
    }

    static inline TextureDimension GetTextureDimension(HLSLCC_TEX_DIMENSION dim)
    {
        switch (dim)
        {
            case TD_2D:         return kTexDim2D;
            case TD_2DARRAY:    return kTexDim2DArray;
            case TD_3D:         return kTexDim3D;
            case TD_CUBE:       return kTexDimCUBE;
            case TD_CUBEARRAY:  return kTexDimCubeArray;
            default: AssertMsg(0, "Unknown or unsupported texture dimension type");
        }
        return kTexDimNone;
    }

    virtual void OnTextureBinding(const std::string &name, int bindIndex, int samplerIndex, bool multisampled, HLSLCC_TEX_DIMENSION dim, bool isUAV)
    {
        if (m_Data->reflectionReport == NULL)
            return;
        m_Data->reflectionReport->OnTextureBinding(name.c_str(), bindIndex, samplerIndex, multisampled, GetTextureDimension(dim));
    }

    virtual void OnBufferBinding(const std::string &name, int bindIndex, bool isUAV)
    {
        if (m_Data->reflectionReport == NULL)
            return;
        m_Data->reflectionReport->OnBufferBinding(name.c_str(), bindIndex);
    }

    virtual void OnTessellationInfo(uint32_t tessPartitionMode, uint32_t tessOutputWindingOrder, uint32_t tessMaxFactor, uint32_t tessNumPatchesInThreadGroup)
    {
        m_MetalMetadata.tessPartitionMode = tessPartitionMode;
        m_MetalMetadata.tessOutputWindingOrder = tessOutputWindingOrder;
        m_MetalMetadata.tessMaxFactor = tessMaxFactor;
        m_MetalMetadata.tessNumPatchesInThreadGroup = tessNumPatchesInThreadGroup;
    }

    virtual void OnTessellationKernelInfo(uint32_t patchKernelBufferCount)
    {
        m_MetalMetadata.patchKernelBufferCount = patchKernelBufferCount;
    }

    bool HasErrors() const { return m_HasErrors; }

private:
    const ShaderCompileInputInterface *m_Data;
    ShaderImportErrors &m_Errors;
    bool m_HasErrors;
    MetalShaderBlobHeader &m_MetalMetadata;

    // Deferred constant buffer reflections (we need to see all the members before we know the actual size on Metal)
    core::string m_CurrCBName;
    int m_CurrCBPos;
    size_t m_CurrCBMemberCount;
    size_t m_CurrCBTotalMembers;
    std::vector<ConstantInfo> m_CBContents;
};

// 2D and 3D work for both samplers and Textures
// CUBE is sampler only
// Everything else is Texture only
static const char * kKnownSamplerTypes[] =
{
    "2D_half",
    "2D_float",
    "2D",
    "CUBE_half",
    "CUBE_float",
    "CUBE",
    "3D_half",
    "3D_float",
    "3D",

    "2DMS_half",
    "2DMS_float",
    "2DMS",
    "Cube_half",
    "Cube_float",
    "Cube",

    "2DArray_half",
    "2DArray_float",
    "2DArray",
    "2DMSArray_half",
    "2DMSArray_float",
    "2DMSArray",
    "CubeArray_half",
    "CubeArray_float",
    "CubeArray",
};

static bool isAllowedNameChar(char input)
{
    return (input == '_') || (input >= '0' && input <= '9') || (input >= 'a' && input <= 'z') || (input >= 'A' && input <= 'Z');
}

// Find the offset of the next sampler declaration
// we want to support both dx9-style: sampler2D, sampler3D, samplerCUBE
// and dx11-style: Texture2D/Texture3D/TextureCube + SamplerState
// Note this will fail for exotic declarations, or ones that contain /* */ comments between sampler and the sampler name.
static std::size_t findNextSamplerDeclaration(const core::string &input, std::size_t startPos, core::string &outSamplerType, core::string &outSamplerName)
{
    using std::size_t;
    using core::string;

    size_t nextSampler = input.find("sampler", startPos);
    size_t nextTexture = input.find("Texture", startPos);

    size_t parsePos = startPos;
    while (parsePos < input.length())
    {
        if (parsePos > nextSampler)
            nextSampler = input.find("sampler", parsePos);
        if (parsePos > nextTexture)
            nextTexture = input.find("Texture", parsePos);

        // Find next "sampler" or "Texture", return if not found
        parsePos = std::min(nextSampler, nextTexture);
        if (parsePos == string::npos)
            return core::string::npos;

        // Walk backwards until hit either previous ; or beginning of the file or line, allow only whitespaces and tabs in between
        size_t lineStart = parsePos;
        bool isValidDeclaration = true;
        while (lineStart > 0 && input[lineStart - 1] != ';' && input[lineStart - 1] != '\n' && input[lineStart - 1] != '\r')
        {
            char in = input[lineStart - 1];
            if (!(in == ' ' || in == '\t'))
            {
                // special case: allow 'uniform' in the line, some of our shaders seem to still use that
                if (lineStart >= 7 && input.substr(lineStart - 7, 7) == "uniform")
                {
                    lineStart -= 7;
                    continue;
                }
                isValidDeclaration = false;
                break;
            }
            lineStart--;
        }
        if (!isValidDeclaration)
        {
            parsePos++;
            continue;
        }


        // Walk back to the beginning of the line and check that it isn't a preprocessor macro
        lineStart = parsePos;
        while (lineStart > 0 && input[lineStart - 1] != '\n' && input[lineStart - 1] != '\r')
        {
            lineStart--;
        }

        bool isPreprocessorCommand = false;
        while (lineStart < parsePos)
        {
            if (input[lineStart] == '#')
            {
                isPreprocessorCommand = true;
                break;
            }
            if (!(input[lineStart] == ' ' || input[lineStart] == '\t'))
                break;

            lineStart++;
        }
        if (isPreprocessorCommand)
        {
            parsePos++;
            continue;
        }

        size_t wordEndPos = parsePos;
        while (isAllowedNameChar(input[wordEndPos]))
            ++wordEndPos;
        size_t samplerTypeLength = wordEndPos - parsePos;
        const size_t kPrefixLength = 7; // Both `Texture` and `sampler` are 7 chars long
        const char* samplerType = input.c_str() + parsePos;
        // Check that the sampler type is actually valid
        bool nameValid = false;
        for (int i = 0; i < ARRAY_SIZE(kKnownSamplerTypes); i++)
        {
            if (strncmp(samplerType + kPrefixLength, kKnownSamplerTypes[i], samplerTypeLength - kPrefixLength) == 0)
            {
                nameValid = true;
                break;
            }
        }
        if (!nameValid)
        {
            parsePos++;
            continue;
        }

        // Skip possible <floatX, Y> declarations for Tex2DMS and Tex2DMSArray
        while (input[wordEndPos] == ' ' || input[wordEndPos] == '\t' || input[wordEndPos] == '\n' || input[wordEndPos] == '\r')
            ++wordEndPos;

        if (input[wordEndPos] == '<')
        {
            while (input[wordEndPos] != '>')
                ++wordEndPos;
            ++wordEndPos;
        }

        size_t nameStartPos = wordEndPos;
        while (!isAllowedNameChar(input[nameStartPos]))
            ++nameStartPos;
        // Check the chars in between, only allow whitespaces and tabs
        for (size_t i = wordEndPos; i < nameStartPos; i++)
        {
            if (input[i] != ' ' && input[i] != '\t')
            {
                nameValid = false;
                break;
            }
        }
        if (!nameValid)
        {
            parsePos++;
            continue;
        }

        // OK, we probably have a good sampler declaration, parse the name as well
        size_t nameEndPos = nameStartPos;
        while (isAllowedNameChar(input[nameEndPos]))
            ++nameEndPos;

        outSamplerType.assign(samplerType, samplerTypeLength);
        outSamplerName = input.substr(nameStartPos, nameEndPos - nameStartPos);
        return parsePos;
    }
    return core::string::npos;
}

// As a "replace all", but make sure we only mess with actual identifiers, not substrings of something larger
static void replace_identifier(core::string &input, const core::string &from, const core::string &to)
{
    using std::size_t;
    using core::string;

    size_t parsePos = 0;

    while (parsePos < input.length())
    {
        parsePos = input.find(from, parsePos);
        if (parsePos == string::npos)
            return;

        // Check that the preceding character is not a-z,A-Z,0-9 or an underscore
        if (parsePos != 0)
        {
            char prevChar = input[parsePos - 1];
            if (isAllowedNameChar(prevChar))
            {
                // Not a proper match, continue searching
                parsePos++;
                continue;
            }
        }

        // Check that the identifier ends where it should
        if (isAllowedNameChar(input[parsePos + from.length()]))
        {
            parsePos++;
            continue;
        }

        // Ok this is a proper identifier, do the replace
        input.replace(parsePos, from.length(), to);

        parsePos++; // Continue search
    }
}

static const IncludeSearchContext *s_MojoshaderIncludeContext;
static IncludePaths s_MojoshaderPathStack;

void SetIncludeContext(const IncludeSearchContext *includeContext)
{
    s_MojoshaderIncludeContext = includeContext;
}

static int ReadIncludeFile(core::string path, const char **outdata, unsigned int *outbytes, MOJOSHADER_malloc m, void *d)
{
    core::string contents;
    if (ReadTextFile(path, contents))
    {
        char* data = (char*)m((int)contents.size(), d);
        if (data == NULL)
            return 0; // File found but no contents, stop search

        s_MojoshaderPathStack.push_back(DeleteLastPathNameComponent(path));
        memcpy(data, contents.c_str(), contents.size());
        *outdata = data;
        *outbytes = (unsigned int)contents.size();
        return 1; // Found, stop search
    }
    return -1; // Not found, must continue search
}

int MojoshaderIncludeOpen(MOJOSHADER_includeType inctype,
    const char *fname, const char *parentfname, const char *parent,
    const char **outdata,
    unsigned int *outbytes,
    size_t* outDataHash,
    void* userdata,
    MOJOSHADER_malloc m, MOJOSHADER_free f, void *d)
{
    // first try searching relative to current path in stack
    if (!s_MojoshaderPathStack.empty() && !s_MojoshaderPathStack.back().empty())
    {
        core::string fn = AppendPathName(s_MojoshaderPathStack.back(), fname);
        int res = ReadIncludeFile(fn, outdata, outbytes, m, d);
        if (res > 0)
            return res;
    }

    // then try paths relative to given include paths
    for (size_t i = 0; i < s_MojoshaderIncludeContext->includePaths.size(); ++i)
    {
        core::string fn = AppendPathName(s_MojoshaderIncludeContext->includePaths[i], fname);
        int res = ReadIncludeFile(fn, outdata, outbytes, m, d);
        if (res > 0)
            return res;
    }

    core::string fn(fname);
    ConvertSeparatorsToUnity(fn);
    core::string redirectedPath = ResolveRedirectedPath(fn, s_MojoshaderIncludeContext->redirectedFolders);
    if (!redirectedPath.empty())
    {
        int res = ReadIncludeFile(redirectedPath, outdata, outbytes, m, d);
        if (res > 0)
            return res;
    }

    // file not found
    return 0;
}

void MojoshaderIncludeClose(const char *data, size_t dataSize, void* userdata, MOJOSHADER_malloc m,
    MOJOSHADER_free f, void *d)
{
    s_MojoshaderPathStack.pop_back();
    f((void*)data, d);
}

static bool EndsWith(const core::string& str, const core::string& with)
{
    return strncmp(str.c_str() + str.size() - with.size(), with.c_str(), with.size()) == 0;
}

static core::string kFloatSamplerSuffix("_float");
static core::string kHalfSamplerSuffix("_half");

// Find all sampler declarations from the source and fill up the SamplerList vector
void GatherSamplerPrecisions(core::string &source, HLSLccSamplerPrecisionInfo &samplers, REFLECT_RESOURCE_PRECISION defaultPrecision)
{
    core::string samplerType, samplerName;
    std::size_t parsePos = 0;

    while (parsePos < source.length())
    {
        parsePos = findNextSamplerDeclaration(source, parsePos, samplerType, samplerName);
        if (parsePos == core::string::npos)
            break;

        if (EndsWith(samplerType, kFloatSamplerSuffix))
        {
            // Add the sampler name to the list
            samplers.insert(std::make_pair(samplerName, REFLECT_RESOURCE_PRECISION_HIGHP));
        }
        else if (EndsWith(samplerType, kHalfSamplerSuffix))
        {
            // Add the sampler name to the list
            samplers.insert(std::make_pair(samplerName, REFLECT_RESOURCE_PRECISION_MEDIUMP));
        }
        else if (samplerName == "_CameraDepthTexture" || samplerName == "_LastCameraDepthTexture")
        {
            // Set precision on the builtin camera depth textures. On Metal, depth textures must always be float.
            // Also using the default lowp for depth on GLES would be a bad idea -> highp instead.
            samplers.insert(std::make_pair(samplerName, REFLECT_RESOURCE_PRECISION_HIGHP));
        }
        else
        {
            // Use provided precision as default
            // Should be medium for iOS Metal, high for macOS Metal, low otherwise
            // Add the sampler name to the list
            samplers.insert(std::make_pair(samplerName, defaultPrecision));
        }
        parsePos++; // Bump the parse pos to the next character to find the next sampler declaration
    }
}

namespace HLSLcc
{
    // Pre-processes the source (output for other purposes too), detects samplers and gathers information
    // about them. Then adds defines overriding the precision including sampler types to with standard ones.
    bool ProcessSamplerPrecisions(ShaderCompileInputData* localInput, core::string& preprocessedOut, ShaderCompileOutputData* output, HLSLccSamplerPrecisionInfo& samplers)
    {
        core::string source = localInput->source;

        const bool isES = (localInput->api == kShaderCompPlatformGLES20 || localInput->api == kShaderCompPlatformGLES3Plus);
        const bool isMetal = (localInput->api == kShaderCompPlatformMetal);
        const bool isVulkan = (localInput->api == kShaderCompPlatformVulkan);
        const bool isSwitch = (localInput->compileFlags & kShaderCompFlags_SwitchPlatform) != 0;

        if (isES || isMetal || isVulkan || isSwitch)
        {
            bool isDesktopMetal = isMetal;
            for (int iArg = 0; iArg < localInput->args.size(); ++iArg)
            {
                if (localInput->args[iArg].name.find("SHADER_API_MOBILE") != core::string::npos && localInput->args[iArg].value != "0")
                {
                    isDesktopMetal = false;
                }
            }

            if (localInput->api == kShaderCompPlatformGLES20)
            {
                // A more complete set of overrides is maintained in HLSLSupport.cginc,
                // but in case we're compiling against runtime tests or some other scenario
                // that doesn't include it, just provide the basic set that should be enough

                if (source.find("HLSLSupport.cginc") == source.npos)
                {
                    localInput->args.push_back(ShaderCompileArg("uint", "int"));
                    localInput->args.push_back(ShaderCompileArg("uint1", "int1"));
                    localInput->args.push_back(ShaderCompileArg("uint2", "int2"));
                    localInput->args.push_back(ShaderCompileArg("uint3", "int3"));
                    localInput->args.push_back(ShaderCompileArg("uint4", "int4"));
                }
            }

            // Run Mojoshader preprocessor on the source
            std::vector<MOJOSHADER_preprocessorDefine> defines;
            for (size_t i = 0; i < localInput->args.size(); i++)
            {
                MOJOSHADER_preprocessorDefine d;
                d.identifier = localInput->args[i].name.c_str();
                d.definition = localInput->args[i].value.c_str();
                defines.push_back(d);
            }
            MOJOSHADER_preprocessorDefine d;
            d.identifier = kShaderCompPlatformDefines[localInput->api];
            d.definition = "1";
            defines.push_back(d);

            SetIncludeContext(&localInput->includeContext);
            const MOJOSHADER_preprocessData *ppData = MOJOSHADER_preprocess("", source.c_str(), (unsigned int)source.length(), &defines[0], (unsigned int)defines.size(), MojoshaderIncludeOpen, MojoshaderIncludeClose, NULL, NULL, NULL, NULL);
            if (ppData->error_count != 0)
            {
                for (int i = 0; i < ppData->error_count; i++)
                    output->errors.AddImportError(ppData->errors[i].error, ppData->errors[i].filename, ppData->errors[i].error_position, false, localInput->api);

                return false;
            }
            preprocessedOut.assign(ppData->output);

            REFLECT_RESOURCE_PRECISION precision = isDesktopMetal ? REFLECT_RESOURCE_PRECISION_HIGHP : (isMetal || localInput->api == kShaderCompPlatformGLES3Plus) ? REFLECT_RESOURCE_PRECISION_MEDIUMP : REFLECT_RESOURCE_PRECISION_LOWP;
            GatherSamplerPrecisions(preprocessedOut, samplers, precision);

            MOJOSHADER_freePreprocessData(ppData);
        }

        // Now add a few extra arguments to disable float and half samplers
        localInput->args.push_back(ShaderCompileArg("sampler2D_float", "sampler2D"));
        localInput->args.push_back(ShaderCompileArg("sampler2D_half", "sampler2D"));
        localInput->args.push_back(ShaderCompileArg("samplerCUBE_float", "samplerCUBE"));
        localInput->args.push_back(ShaderCompileArg("samplerCUBE_half", "samplerCUBE"));
        localInput->args.push_back(ShaderCompileArg("sampler3D_float", "sampler3D"));
        localInput->args.push_back(ShaderCompileArg("sampler3D_half", "sampler3D"));
        localInput->args.push_back(ShaderCompileArg("Texture2D_float", "Texture2D"));
        localInput->args.push_back(ShaderCompileArg("Texture2D_half", "Texture2D"));
        localInput->args.push_back(ShaderCompileArg("Texture2DMS_float", "Texture2DMS"));
        localInput->args.push_back(ShaderCompileArg("Texture2DMS_half", "Texture2DMS"));
        localInput->args.push_back(ShaderCompileArg("TextureCube_float", "TextureCube"));
        localInput->args.push_back(ShaderCompileArg("TextureCube_half", "TextureCube"));
        localInput->args.push_back(ShaderCompileArg("Texture3D_float", "Texture3D"));
        localInput->args.push_back(ShaderCompileArg("Texture3D_half", "Texture3D"));
        localInput->args.push_back(ShaderCompileArg("Texture2DArray_float", "Texture2DArray"));
        localInput->args.push_back(ShaderCompileArg("Texture2DArray_half", "Texture2DArray"));
        localInput->args.push_back(ShaderCompileArg("Texture2DMSArray_float", "Texture2DMSArray"));
        localInput->args.push_back(ShaderCompileArg("Texture2DMSArray_half", "Texture2DMSArray"));
        localInput->args.push_back(ShaderCompileArg("TextureCubeArray_float", "TextureCubeArray"));
        localInput->args.push_back(ShaderCompileArg("TextureCubeArray_half", "TextureCubeArray"));

        return true;
    }
}

#if __APPLE__
static void PerformMetalVerification(const ShaderCompileInputInterface* input, ShaderCompileOutputData* output,
    const core::string& source)
{
    // Editor needs to be started with this environment and it will be inherited by the UnityShaderCompiler
    if (!getenv("UNITY_METAL_VALIDATE_SHADERS"))
        return;

    const ShaderCompileInputData *localInput = (const ShaderCompileInputData*)input;

    // Validate the shader against metal compiler
    //
    // Desktop/Mobile potentially have different feature sets,
    // so probe if SHADER_API_MOBILE is defined
    char tempName[] = "/tmp/mtl.XXXXXX";
    int fd = mkstemp(tempName);

    core::string metalPath = core::string(tempName) + ".metal";

    std::FILE *t = std::fopen(metalPath.c_str(), "wb");
    close(fd);
    unlink(tempName);

    std::fputs(source.c_str(), t);
    std::fclose(t);

    bool isMobile = false;
    std::string mobileString = kShaderCompPlatformDefineMobile;
    std::string framebufferFetchAvailable = "UNITY_FRAMEBUFFER_FETCH_AVAILABLE";

    for (int i = 0, n = localInput->args.size(); i < n; i++)
    {
        if (localInput->args[i].name == mobileString || localInput->args[i].name == framebufferFetchAvailable)
        {
            isMobile = true;
            break;
        }
    }

    std::string cmdLine = "xcrun ";
    cmdLine += isMobile ? "-sdk iphoneos metal -std=ios-metal1.2 " : "-sdk macosx metal -std=osx-metal1.2 ";
    cmdLine += "-Wall -Wextra -ferror-limit=1000 ";
    cmdLine += metalPath;
    cmdLine += " -o ";
    cmdLine += metalPath + ".o";
    if (system(cmdLine.c_str()) != 0)
    {
        output->errors.AddImportError("Metal verification failed!", metalPath, false, input->api);
        return;
    }

    unlink(metalPath.c_str());
    unlink((metalPath + ".o").c_str());
}

#endif


static GLLang GetGLTargetVersion(ShaderRequirements requirements, ShaderCompilerPlatform platform)
{
    ShaderGpuProgramType type = GetGLSLGpuProgramType(requirements, platform);
    switch (type)
    {
        case kShaderGpuProgramGLES31AEP:
        case kShaderGpuProgramGLES31:
            return LANG_ES_310;
        case kShaderGpuProgramGLES3:
            return LANG_ES_300;
        case kShaderGpuProgramGLES:
            return LANG_ES_100;

        case kShaderGpuProgramGLCore43:
            return LANG_420;
        case kShaderGpuProgramGLCore41:
            return LANG_410;
        case kShaderGpuProgramGLCore32:
            if (HasAnyFlags(requirements, kShaderRequireInstancing | kShaderRequire2DArray | kShaderRequireInterpolators15Integers))
            {
                return LANG_330;
            }
            return LANG_150;
        default:
            return LANG_150;
    }
}

static void TranslateToGLSL(const ShaderCompileInputInterface* input, core::string& sourceCode, GLSLCrossDependencyData *cdData, HLSLccSamplerPrecisionInfo &info, ShaderImportErrors& errors, MetalShaderBlobHeader &metalMetadata)
{
    const bool isES2 = (input->api == kShaderCompPlatformGLES20);
    const bool isES3 = (input->api == kShaderCompPlatformGLES3Plus);
    const bool isES = (isES2 || isES3);

    // We cannot have explicit locations, we're binding them at runtime.
    unsigned int options = HLSLCC_FLAG_INOUT_SEMANTIC_NAMES |
        HLSLCC_FLAG_DISABLE_EXPLICIT_LOCATIONS |
        //                          HLSLCC_FLAG_WRAP_UBO |
        HLSLCC_FLAG_DISABLE_GLOBALS_STRUCT |
        HLSLCC_FLAG_GLOBAL_CONSTS_NEVER_IN_UBO |
        HLSLCC_FLAG_REMOVE_UNUSED_GLOBALS |
        HLSLCC_FLAG_TRANSLATE_MATRICES |
        HLSLCC_FLAG_UNIFORM_BUFFER_OBJECT;

    if (input->api == kShaderCompPlatformMetal && input->programType != kProgramFragment)
    {
        // vertex/hull/domain shaders get merged into vertex shader
        if ((cdData->ui32ProgramStages & PS_FLAG_HULL_SHADER) != 0 || (cdData->ui32ProgramStages & PS_FLAG_DOMAIN_SHADER) != 0)
        {
            options &= ~HLSLCC_FLAG_REMOVE_UNUSED_GLOBALS;
            options |= HLSLCC_FLAG_METAL_TESSELLATION;
        }
    }

    if (isES2)
        options &= ~HLSLCC_FLAG_UNIFORM_BUFFER_OBJECT;

    if (isES3)
        options |= HLSLCC_FLAG_GLES31_IMAGE_QUALIFIERS;

    if (!HasFlag(input->compileFlags, kShaderCompFlags_MetalUsePointFilterForShadows))
        options |= HLSLCC_FLAG_METAL_SHADOW_SAMPLER_LINEAR;

    if (HasFlag(input->compileFlags, kShaderCompFlags_SwitchPlatform))
    {
        options |= HLSLCC_FLAG_NVN_TARGET |
            HLSLCC_FLAG_AVOID_SHADER_ATOMIC_COUNTERS |
            HLSLCC_FLAG_UNIFORM_BUFFER_OBJECT_WITH_INSTANCE_NAME |
            HLSLCC_FLAG_COMBINE_TEXTURE_SAMPLERS |
            HLSLCC_FLAG_SEPARABLE_SHADER_OBJECTS;
        //"uniforms in shaders compiled for NVN must be part of a uniform block or be of sampler or of image type"
        options &= ~HLSLCC_FLAG_GLOBAL_CONSTS_NEVER_IN_UBO;
    }

    if (HasFlag(input->compileFlags, kShaderCompFlags_FramebufferFetch))
        options |= HLSLCC_FLAG_SHADER_FRAMEBUFFER_FETCH;

    if (HasFlag(input->compileFlags, kShaderCompFlags_DisableFastMath))
        options |= HLSLCC_FLAG_DISABLE_FASTMATH;

    GLLang targetGL = LANG_METAL;
    if (input->api == kShaderCompPlatformGLES20)
        targetGL = LANG_ES_100;
    else if (input->api != kShaderCompPlatformMetal)
        targetGL = GetGLTargetVersion(input->requirements, input->api);

    int compiledOK = 0;
    GLSLShader result;

    switch (input->programType)
    {
        case kProgramHull:
            options |= HLSLCC_FLAG_TESS_ENABLED;
            break;
        case kProgramDomain:
            options |= HLSLCC_FLAG_TESS_ENABLED;
            break;
        case kProgramGeometry:
            options |= HLSLCC_FLAG_GS_ENABLED;
            break;
        default:
            break;
    }

    const char *byteCode = (const char*)input->GetCompiledDataPtr();
    Assert(byteCode);
    Assert(*byteCode == '\0'); // first should be a byte indicating no root signature
    byteCode++;

    GlExtensions ext;
    ext.ARB_explicit_attrib_location = !isES;
    // Buggy at the moment
    ext.ARB_explicit_uniform_location = 0;//(targetGL == LANG_ES_300 || targetGL == LANG_ES_310) || targetGL >= LANG_150;
    ext.ARB_shading_language_420pack = targetGL >= LANG_420;
    ext.EXT_shader_framebuffer_fetch = HasFlag(input->compileFlags, kShaderCompFlags_FramebufferFetch);

    UnityHLSLccReflection refl(input, errors, metalMetadata);
    compiledOK = TranslateHLSLFromMem(byteCode, options, targetGL, &ext, cdData, info, refl, &result);
    if (!compiledOK)
    {
#if DEBUG_DUMP_HLSLcc_OUTPUT_FILES
        {
            FILE* f = fopen("debug-glsl-error-in-hlslcc.txt", "wb");
            fputs(s_HLSLccPreprocessedSourceString.c_str(), f);
            fclose(f);
        }
        {
            FILE* f = fopen("debug-glsl-error-in-hlslcc-orig.txt", "wb");
            fputs(s_HLSLccOriginalSourceString.c_str(), f);
            fclose(f);
        }
#endif

        printf_console("TranslateHLSLFromMem failed! Type: %i Lang: %i Source: %s", result.shaderType, result.GLSLLanguage, result.sourceCode.c_str());
        return;
    }

    sourceCode = result.sourceCode;
}

ShaderCompileOutputInterface* InvokeCompilerHLSLcc(const ShaderCompileInputInterface* input)
{
    ShaderCompileOutputData* output = MakeOutputDataHLSLcc(input);

    Assert(input->api == kShaderCompPlatformOpenGLCore || input->api == kShaderCompPlatformGLES20 || input->api == kShaderCompPlatformGLES3Plus || input->api == kShaderCompPlatformMetal);

    const bool isES2 = (input->api == kShaderCompPlatformGLES20);
    const bool isES3 = (input->api == kShaderCompPlatformGLES3Plus);
    const bool isES = (isES2 || isES3);
    const bool isMetal = (input->api == kShaderCompPlatformMetal);

    const ShaderRequirements requirements = input->requirements;
    GLSLCrossDependencyData cdData;

    if (input->programType != kProgramVertex && !isMetal)
    {
        // actual fragment/hull/domain/geom program was emitted as part of vertex shader;
        // just output dummy header for fragment one
        output->gpuProgramType = GetGLSLGpuProgramType(requirements, input->api);
        return output;
    }

    if ((input->programType == kProgramHull || input->programType == kProgramDomain) && isMetal)
    {
        // vertex/hull/domain shaders are combined into a vertex shader and fragment shader is separate
        output->gpuProgramType = kShaderGpuProgramMetalVS;
        return output;
    }

    // check
    if (input->GetEntryName(kProgramVertex)[0] == 0 || input->GetEntryName(kProgramFragment)[0] == 0)
    {
        output->errors.AddImportError("Compiling for GLSL and Metal requires both vertex & fragment programs", input->startLine, false, input->api);
        return output;
    }
    core::string glslVertText, glslFragText, glslGeomText, glslEvalText, glslControlText;

    // Build the program stages flags. We'll always have pixel and vertex shaders anyway.
    cdData.ui32ProgramStages = PS_FLAG_VERTEX_SHADER | PS_FLAG_PIXEL_SHADER;
    if (input->GetEntryName(kProgramHull)[0] != 0)
        cdData.ui32ProgramStages |= PS_FLAG_HULL_SHADER;

    if (input->GetEntryName(kProgramDomain)[0] != 0)
        cdData.ui32ProgramStages |= PS_FLAG_DOMAIN_SHADER;

    if (input->GetEntryName(kProgramGeometry)[0] != 0)
        cdData.ui32ProgramStages |= PS_FLAG_GEOMETRY_SHADER;

    static const int kShaderCompilationOrder[kProgramCount] =
    {
        kProgramFragment,
        kProgramVertex,
        kProgramHull,
        kProgramDomain,
        kProgramGeometry,
        kProgramSurface
    };

    static const char* kShaderTypeDefines[kProgramCount] =
    {
        "SHADER_STAGE_FRAGMENT",
        "SHADER_STAGE_VERTEX",
        "SHADER_STAGE_HULL",
        "SHADER_STAGE_DOMAIN",
        "SHADER_STAGE_GEOMETRY",
        ""
    };

    bool framebufferFetchEnabled = false;
    if (kShaderPlatformFramebufferFetch & (1 << input->api))
    {
        framebufferFetchEnabled = ParseNeedForFramebufferFetchEnabled(core::string(input->GetSource(), input->GetSourceLength()));
    }

    MetalShaderBlobHeader metalMetadata;

    for (int k = 0; k < kProgramCount; k++)
    {
        int i = kShaderCompilationOrder[k];
        if (isMetal)
        {
            // On Metal, fragment shader is separate, vertex/hull/domain shaders get combined into vertex shader
            if (!(input->programType == kProgramVertex || input->programType == kProgramFragment))
                break;

            if (input->programType == kProgramFragment && i != input->programType)
                break;

            if (input->programType == kProgramVertex && i == kProgramFragment)
                continue;

            // Shouldn't happen anyway
            if (i == kProgramGeometry)
                continue;
        }
        if (input->GetEntryName((ShaderCompilerProgram)i)[0] == 0)
            continue;

        ShaderCompileInputData localInput;
        localInput.CopyFrom(input);

        localInput.programType = (ShaderCompilerProgram)i;

        // Pass a hint for choosing the preferred compiler, in case there's a need for supporting multiple
        // compiler backends for migrational, testing or whatever purposes with the shader target
        localInput.args.push_back(ShaderCompileArg("UNITY_PREFER_HLSLCC", ""));

        // Add (possibly replace) the SHADER_STAGE define
        bool stageDefineAdded = false;
        for (int iArg = 0; iArg < localInput.args.size(); ++iArg)
        {
            if (localInput.args[iArg].name.find("SHADER_STAGE") != core::string::npos)
            {
                localInput.args[iArg].name = kShaderTypeDefines[k];
                stageDefineAdded = true;
                break;
            }
        }
        if (!stageDefineAdded)
            localInput.args.push_back(ShaderCompileArg(kShaderTypeDefines[k], "1"));

        if (framebufferFetchEnabled)
        {
            localInput.compileFlags |= kShaderCompFlags_FramebufferFetch;
            localInput.args.push_back(ShaderCompileArg("UNITY_FRAMEBUFFER_FETCH_ENABLED", "1"));
        }

        // Add available feature macros
        ShaderRequirements availableRequirements = localInput.requirements | GetCompilePlatformMinSpecFeatures(localInput.api);
        AddAvailableShaderTargetRequirementsMacros(availableRequirements, localInput.args);
        localInput.compileFlags |= kShaderCompFlags_DoNotAddShaderAvailableMacros;

        HLSLccSamplerPrecisionInfo samplers;
        core::string ppdSource;
        if (!HLSLcc::ProcessSamplerPrecisions(&localInput, ppdSource, output, samplers))
        {
#if DEBUG_DUMP_HLSLcc_OUTPUT_FILES
            FILE* f = fopen("debug-glsl-error-in-preprocessor.txt", "wb");
            fputs(ppdSource.c_str(), f);
            fclose(f);
#endif
            return output;
        }

#if DEBUG_DUMP_HLSLcc_OUTPUT_FILES
        s_HLSLccOriginalSourceString = localInput.source;
        s_HLSLccPreprocessedSourceString = ppdSource;
#endif

        if (isES && localInput.programType == kProgramFragment)
        {
            // Search for the input for the special token for enabling the extended blend extension
            size_t pos = 0;

            while (pos != core::string::npos)
            {
                pos = ppdSource.find("hlslcc_blend_support_", pos);
                if (pos != core::string::npos)
                {
                    size_t end = ppdSource.find_first_of(" \t\n;", pos);
                    if (end != core::string::npos)
                    {
                        // Clip the "hlslcc_"
                        core::string blendMode = ppdSource.substr(pos + 7, end - pos - 7);
                        cdData.m_ExtBlendModes.push_back(blendMode);
                    }
                    pos++; // Bump pos so we don't search this same one again
                }
            }
        }

        // ES & Metal uses partial precision declarations, so needs SM5.0 in HLSL bytecode translation,
        // will restore it after that.
        localInput.requirements = isES || isMetal ? kShaderRequireShaderModel50_PC : requirements;

        const ShaderCompileOutputInterface* localOutput = NULL;
        if (gPluginDispatcher->HasCompilerForAPI(kShaderCompPlatformD3D11))
            localOutput = gPluginDispatcher->Get(kShaderCompPlatformD3D11)->CompileShader(&localInput);

        localInput.requirements = requirements; // restore to original requirements

        if (localOutput == NULL)
        {
            output->errors.AddImportError("Could not call HLSL compiler - missing plugin?", input->startLine, false, input->api);
            return output;
        }

        const UInt8* hlslBytecode = localOutput->GetCompiledDataPtr();

        if (localOutput->HasErrors() || hlslBytecode == NULL)
        {
#if DEBUG_DUMP_HLSLcc_OUTPUT_FILES
            {
                FILE* f = fopen("debug-glsl-error-in-d3dcompile.txt", "wb");
                if (f)
                {
                    fputs(s_HLSLccPreprocessedSourceString.c_str(), f);
                    fclose(f);
                }
            }
            {
                FILE* f = fopen("debug-glsl-error-in-d3dcompile-orig.txt", "wb");
                if (f)
                {
                    fputs(s_HLSLccOriginalSourceString.c_str(), f);
                    fclose(f);
                }
            }
#endif
            AddErrorsFrom(output->errors, &localInput, localOutput);
            gPluginDispatcher->Get(kShaderCompPlatformD3D11)->ReleaseShader(localOutput);
            return output;
        }

        localInput.gpuProgramData.assign(hlslBytecode, hlslBytecode + localOutput->GetCompiledDataSize());

        core::string sourceCode;
        TranslateToGLSL(&localInput, sourceCode, &cdData, samplers, output->errors, metalMetadata);

        if (HasFlag(input->compileFlags, kShaderCompFlags_EmbedHLSLccDisassembly))
        {
            gPluginDispatcher->Get(kShaderCompPlatformD3D11)->ReleaseShader(localOutput);

            localOutput = gPluginDispatcher->Get(kShaderCompPlatformD3D11)->DisassembleShader(&localInput);

            sourceCode += "\n#if 0\n";
            sourceCode += localOutput->GetDisassembly();
            sourceCode += "\n#endif\n";
        }

        if (HasFlag(input->compileFlags, kShaderCompFlags_EmbedHLSLccPreprocessedSource))
        {
            if (!ppdSource.empty())
            {
                sourceCode += "\n#if 0  // Preprocessed HLSL source\n";
                sourceCode += ppdSource;
                sourceCode += "\n#endif\n";
            }
        }

        gPluginDispatcher->Get(kShaderCompPlatformD3D11)->ReleaseShader(localOutput);

        switch ((ShaderCompilerProgram)i)
        {
            case kProgramVertex:
                glslVertText = sourceCode;
                break;
            case kProgramFragment:
                glslFragText = sourceCode;
                break;
            case kProgramHull:
                glslControlText = sourceCode;
                break;
            case kProgramDomain:
                glslEvalText = sourceCode;
                break;
            case kProgramGeometry:
                glslGeomText = sourceCode;
                break;
            default:    // To shut up the compiler
                break;
        }

#if 0 && DEBUG_DUMP_HLSLcc_OUTPUT_FILES
        {
            FILE* f = fopen("debug-glsl-4.txt", "wb");
            fputs(sourceCode.c_str(), f);
            fclose(f);
        }
#endif

        // both vertex & fragment texts will have common prefix with structures & whatnot
        if (isMetal)
        {
            output->gpuProgramType = input->programType == kProgramFragment ? kShaderGpuProgramMetalFS : kShaderGpuProgramMetalVS;

            core::string outputString = input->programType == kProgramFragment ? glslFragText : glslVertText;
            if (!glslControlText.empty())
                outputString += "// SHADER_STAGE_HULL_begin\n" + glslControlText + "\n// SHADER_STAGE_HULL_end\n";
            if (!glslEvalText.empty())
                outputString += "// SHADER_STAGE_DOMAIN_begin\n" + glslEvalText + "\n// SHADER_STAGE_DOMAIN_end\n";

            tReturnedVector result;
            WriteShaderSnippetBlob(result, "xlatMtlMain", outputString, &metalMetadata);
            output->stringOutput = core::string(result.begin(), result.end());
        }
        else
        {
            output->gpuProgramType = GetGLSLGpuProgramType(requirements, input->api);

            core::string outputString = "#ifdef VERTEX\n"   + glslVertText + "\n#endif\n";
            outputString += "#ifdef FRAGMENT\n" + glslFragText + "\n#endif\n";
            if (!glslControlText.empty())
                outputString += "#ifdef HULL\n" + glslControlText + "\n#endif\n";
            if (!glslEvalText.empty())
                outputString += "#ifdef DOMAIN\n" + glslEvalText + "\n#endif\n";
            if (!glslGeomText.empty())
                outputString += "#ifdef GEOMETRY\n" + glslGeomText + "\n#endif\n";

            output->stringOutput = outputString;
        }

#if 0 && DEBUG_DUMP_HLSLcc_OUTPUT_FILES
        {
            FILE* f = fopen("debug-glsl-final.txt", "wb");
            fputs(output->stringOutput.c_str(), f);
            fclose(f);
        }
#endif
    }

#if defined(__APPLE__) && defined(_DEBUG)
    if (isMetal)
    {
        MetalShaderBlobDesc desc = ExtractMetalShaderBlobDesc(output->GetCompiledDataPtr(), output->GetCompiledDataSize());
        core::string source = core::string(desc.code, desc.codeLength);
        PerformMetalVerification(input, output, source);
    }
#endif

    return output;
}

static GLLang SelectTargetLanguage(ShaderCompilerPlatform api)
{
    switch (api)
    {
        case kShaderCompPlatformGLES20:
            return LANG_ES_100;
        case kShaderCompPlatformGLES3Plus:
            return LANG_ES_310;
        case kShaderCompPlatformOpenGLCore:
            return LANG_420;
        case kShaderCompPlatformMetal:
            return LANG_METAL;
        case kShaderCompPlatformSwitch:
            return LANG_440; // Switch actually supports up to GLSL 4.5
        default:
            Assert(0 && "Unknown Shader compiler platform passed to HLSLcc");
            return LANG_DEFAULT;
    }
}

// Reflection interface implementation for compute shaders to fill the kernel info structs
class UnityHLSLccComputeReflection : public HLSLccReflection
{
private:
    struct ConstantInfo
    {
        ConstantInfo(const core::string &name, int bindIndex, ShaderParamType type, int rows, int cols, bool isMatrix, int arraySize)
            : m_Name(name), m_BindIndex(bindIndex), m_Type(type), m_Rows(rows), m_Cols(cols), m_IsMatrix(isMatrix), m_ArraySize(arraySize) {}
        core::string m_Name;
        int m_BindIndex;
        ShaderParamType m_Type;
        int m_Rows;
        int m_Cols;
        bool m_IsMatrix;
        int m_ArraySize;
    };

    // Look through the CB names from DX reflection data and return one matching to the name (or NULL if not found)
    ComputeShaderCB* GetRequiredConstantBufferInfo(const core::string &name)
    {
        core::string prefixedName = "$"; // Handle $Globals case
        prefixedName.append(name);

        for (size_t i = 0, n = m_RequiredConstants.size(); i < n; ++i)
        {
            if (m_RequiredConstants[i].name == ShaderLab::FastPropertyName(name) ||
                m_RequiredConstants[i].name == ShaderLab::FastPropertyName(prefixedName))
            {
                return &m_RequiredConstants[i];
            }
        }

        return NULL;
    }

public:
    UnityHLSLccComputeReflection(ComputeShaderBinary *data, size_t kernelIndex, ShaderImportErrors &errors, ComputeShader::CBArray requiredConstants) : m_Data(data), m_KernelIndex(kernelIndex), m_HasErrors(false), m_Errors(errors), m_RequiredConstants(requiredConstants)
    {
        m_ThreadGroupSize[0] = m_ThreadGroupSize[1] = m_ThreadGroupSize[2] = 0;
    }

    virtual void OnDiagnostics(const std::string &error, int line, bool isError)
    {
        if (isError)
            m_HasErrors = true;

        m_Errors.AddImportError(error, line, !isError);
    }

    virtual void OnInputBinding(const std::string &name, int bindIndex)
    {
        AssertMsg(0, "Input binding should not happen in compute shader");
    }

    virtual bool OnConstantBuffer(const std::string &name, size_t bufferSize, size_t memberCount)
    {
        // See if we actually need this CB
        m_CurrCBRequired = GetRequiredConstantBufferInfo(name);

        m_CurrCBPos = 0;
        m_CBContents.clear();
        m_CurrCBName = name;
        m_CurrCBMemberCount = 0;
        m_CurrCBTotalMembers = memberCount;

        return (m_CurrCBRequired != NULL);
    }

    virtual bool OnConstant(const std::string &name, int bindIndex, SHADER_VARIABLE_TYPE cType, int rows, int cols, bool isMatrix, int arraySize, bool isUsed)
    {
        bool isRequired = false;
        // If current CB was on the list, check if this constant is in the DX reflection name list too
        if (m_CurrCBRequired != NULL)
        {
            for (size_t i = 0, n = m_CurrCBRequired->params.size(); i < n; ++i)
            {
                if (m_CurrCBRequired->params[i].name == ShaderLab::FastPropertyName(name))
                {
                    isRequired = true;
                    break;
                }
            }
        }

        // Add only constants that are actually required
        if (isRequired)
        {
            ShaderParamType type = kShaderParamFloat;
            switch (cType)
            {
                default:
                    AssertMsg(0, "Unknown or unsupported shader param type");
                    break;
                case SVT_BOOL:
                    type = kShaderParamBool;
                    break;
                case SVT_FLOAT:
                    type = kShaderParamFloat;
                    break;
                case SVT_FLOAT16:
                case SVT_FLOAT10:
                    type = kShaderParamHalf;
                    break;
                case SVT_INT:
                    type = kShaderParamInt;
                    break;
                case SVT_UINT:
                    type = kShaderParamUInt;
                    break;
            }

            int size, align;
            GetMetalTypeSize(type, rows, cols, isMatrix, arraySize, size, align, true);

            size_t loc = m_CurrCBPos;
            loc = (loc + align - 1) & ~(align - 1); // align it

            m_CurrCBPos = loc + size;

            m_CBContents.push_back(ConstantInfo(name, (int)loc, type, rows, cols, isMatrix, arraySize));
            m_CurrCBMemberCount++;
        }
        else // If this constant was skipped we need to reduce the total count to be able to flush correctly
        {
            m_CurrCBTotalMembers--;
        }

        if (m_CurrCBMemberCount == m_CurrCBTotalMembers)
        {
            // Flush
            ComputeShader::CBArray& kernelCBs = m_Data->m_KernelCBs[m_KernelIndex];
            ComputeShaderCB newCB;
            newCB.name = ShaderLab::FastPropertyName(m_CurrCBName);
            newCB.byteSize = (int)m_CurrCBPos;
            newCB.params.reserve(m_CurrCBTotalMembers);

            for (size_t i = 0; i < m_CBContents.size(); i++)
            {
                ConstantInfo &ci = m_CBContents[i];

                ComputeShaderCB& currentCB = newCB;
                ComputeShaderParam newParam;
                newParam.name = ShaderLab::FastPropertyName(ci.m_Name);
                newParam.type = ci.m_Type;
                newParam.offset = ci.m_BindIndex;
                newParam.arraySize = ci.m_ArraySize;
                newParam.rowCount = ci.m_Rows;
                newParam.colCount = ci.m_Cols;
                currentCB.params.push_back(newParam);
            }

            kernelCBs.push_back(newCB);

            m_CurrCBPos = 0;
            m_CurrCBRequired = NULL;
            m_CBContents.clear();
            m_CurrCBName = "";
            m_CurrCBMemberCount = 0;
            m_CurrCBTotalMembers = 0;
        }

        return isRequired;
    }

    virtual void OnConstantBufferBinding(const std::string &name, int bindIndex)
    {
        // Add bindings for only those CBs that are actually in use
        if (GetRequiredConstantBufferInfo(name) == NULL)
            return;

        ComputeShaderKernel& kernel = m_Data->GetKernels()[m_KernelIndex];
        ComputeShaderResource newCBBinding;
        newCBBinding.name = ShaderLab::FastPropertyName(name);
        newCBBinding.bindPoint = bindIndex;
        kernel.cbs.push_back(newCBBinding);
    }

    static inline TextureDimension GetTextureDimension(HLSLCC_TEX_DIMENSION dim)
    {
        switch (dim)
        {
            case TD_2D:         return kTexDim2D;
            case TD_2DARRAY:    return kTexDim2DArray;
            case TD_3D:         return kTexDim3D;
            case TD_CUBE:       return kTexDimCUBE;
            case TD_CUBEARRAY:  return kTexDimCubeArray;
            default: AssertMsg(0, "Unknown or unsupported texture dimension type");
        }
        return kTexDimNone;
    }

    virtual void OnTextureBinding(const std::string &name, int bindIndex, int samplerIndex, bool multisampled, HLSLCC_TEX_DIMENSION dim, bool isUAV)
    {
        ComputeShaderKernel& kernel = m_Data->GetKernels()[m_KernelIndex];
        ComputeShaderResource newTexture;
        newTexture.name = ShaderLab::FastPropertyName(name);
        newTexture.bindPoint = bindIndex;
        newTexture.samplerBindPoint = samplerIndex;
        newTexture.texDimension = GetTextureDimension(dim);

        if (isUAV)
            kernel.outBuffers.push_back(newTexture);
        else
            kernel.textures.push_back(newTexture);
    }

    virtual void OnBufferBinding(const std::string &name, int bindIndex, bool isUAV)
    {
        ComputeShaderKernel& kernel = m_Data->GetKernels()[m_KernelIndex];
        ComputeShaderResource newBuffer;
        newBuffer.name = ShaderLab::FastPropertyName(name);
        newBuffer.bindPoint = bindIndex;
        if (isUAV)
            kernel.outBuffers.push_back(newBuffer);
        else
            kernel.inBuffers.push_back(newBuffer);
    }

    virtual void OnThreadGroupSize(unsigned int xSize, unsigned int ySize, unsigned int zSize)
    {
        m_ThreadGroupSize[0] = xSize;
        m_ThreadGroupSize[1] = ySize;
        m_ThreadGroupSize[2] = zSize;
    }

    const unsigned int* GetThreadGroupSize() { return m_ThreadGroupSize; }
    bool HasErrors() const { return m_HasErrors; }

private:
    ComputeShaderBinary *m_Data;
    size_t m_KernelIndex;
    unsigned int m_ThreadGroupSize[3];
    ShaderImportErrors &m_Errors;
    bool m_HasErrors;
    ComputeShader::CBArray m_RequiredConstants;

    // Deferred constant buffer reflections (we need to see all the members before we know the actual size on Metal)
    core::string m_CurrCBName;
    ComputeShaderCB *m_CurrCBRequired;
    size_t m_CurrCBPos;
    size_t m_CurrCBMemberCount;
    size_t m_CurrCBTotalMembers;
    std::vector<ConstantInfo> m_CBContents;
};

void TranslateComputeKernelImpl(const ShaderCompileInputInterface* input, ShaderCompileOutputData* output, ComputeShaderBinary* data);

ShaderCompileOutputInterface* InvokeComputeCompilerHLSLcc(const ShaderCompileInputInterface* input)
{
    // create as binary output data since we write data into binaryOutput from common compute compiling path
    ShaderCompileOutputData* output = new ShaderCompileOutputData(input->api, ShaderCompileOutputData::kBinaryOutputData);
    ComputeShaderBinary data;

    if (input->sourceLanguage == kShaderSourceLanguageCg || input->sourceLanguage == kShaderSourceLanguageHLSL)
    {
        // Run Mojoshader preprocessor on the source and gather sampler precision data
        ShaderCompileInputData localInput;
        localInput.CopyFrom(input);
        localInput.compileFlags |= kShaderCompFlags_DisableStripping;

        HLSLccSamplerPrecisionInfo samplers;
        core::string ppdSource;
        if (!HLSLcc::ProcessSamplerPrecisions(&localInput, ppdSource, output, samplers))
        {
#if DEBUG_DUMP_HLSLcc_OUTPUT_FILES
            FILE* f = fopen("debug-glsl-error-in-preprocessor.txt", "wb");
            fputs(ppdSource.c_str(), f);
            fclose(f);
#endif
            return output;
        }

        CompileShaderKernel(gPluginDispatcher->GetD3DCompiler(), &localInput, data.m_KernelCBs, data.m_Kernels, output->errors);

        // Skip the translate step here if the flag for separate translate pass is set (Metal needs it atm)
        if ((input->compileFlags & kShaderCompFlags_UseSeparateTranslatePass) == 0)
            TranslateComputeKernelImpl(input, output, &data);

        if (HasFlag(input->compileFlags, kShaderCompFlags_EmbedHLSLccPreprocessedSource))
        {
            if (!ppdSource.empty())
            {
                ppdSource = "\n#if 0  // Preprocessed HLSL source\n" + ppdSource + "\n#endif\n";
                ComputeShaderKernel& kernel = data.m_Kernels[0];
                dynamic_array<UInt8>::iterator beforeNullTerminator = &(kernel.code.back());
                const UInt8* ppdSourceCstr = reinterpret_cast<const UInt8*>(ppdSource.c_str());
                kernel.code.insert(beforeNullTerminator, ppdSourceCstr, ppdSourceCstr + ppdSource.length());
            }
        }

        data.WriteToBuffer(output->binaryOutput);
    }
    // GLSL source does not go through HLSLcc, rename pragma defined entrypoint to main and return the modified source code
    else if (input->sourceLanguage == kShaderSourceLanguageGLSL)
    {
        output->errors.Clear();

        ComputeShaderKernel kernel;
        kernel.name.SetName(input->GetEntryName((ShaderCompilerProgram)0));
        data.m_ResourcesResolved = false;

        core::string tmp(input->GetSource());
        core::string entrypoint("void ");
        entrypoint.append(kernel.name.GetName());

        size_t pos = tmp.find(entrypoint);
        if (pos != string::npos)
            tmp.replace(pos, entrypoint.length(), core::string("void main"));
        else
            output->errors.AddImportError("Kernel entrypoint not found", 0, false);

        kernel.code.resize_uninitialized(tmp.length() + 1);
        memcpy(kernel.code.data(), tmp.c_str(), tmp.length() + 1);

        data.m_Kernels.push_back(kernel);
        data.m_KernelCBs.push_back(ComputeShader::CBArray());
        data.WriteToBuffer(output->binaryOutput);
    }
    else
    {
        AssertMsg(false, "Unknown compute shader language");
        output->errors.AddImportError("Unknown compute shader language", 0, false);
    }
    return output;
}

ShaderCompileOutputInterface* InvokeComputeTranslatorHLSLcc(const ShaderCompileInputInterface* input)
{
    ShaderCompileOutputData* output = new ShaderCompileOutputData(input->api, ShaderCompileOutputData::kBinaryOutputData);
    ShaderCompileInputData* inputData = (ShaderCompileInputData*)input;
    ComputeShaderBinary data;

    data.ReadFromBuffer(inputData->gpuProgramData, kMemTempAlloc);
    TranslateComputeKernelImpl(input, output, &data);
    data.WriteToBuffer(output->binaryOutput);

    return output;
}

void TranslateComputeKernelImpl(const ShaderCompileInputInterface* input, ShaderCompileOutputData* output, ComputeShaderBinary* data)
{
    const bool isES2 = (input->api == kShaderCompPlatformGLES20);
    const bool isES3 = (input->api == kShaderCompPlatformGLES3Plus);
    const bool isES = (isES2 || isES3);
    bool isSwitch = HasFlag(input->compileFlags, kShaderCompFlags_SwitchPlatform);
    const GLLang targetGL = SelectTargetLanguage(input->api);
    GlExtensions ext;
    ext.ARB_explicit_attrib_location = 0;
    ext.ARB_explicit_uniform_location = 0; // (targetGL == LANG_ES_300 || targetGL == LANG_ES_310) || targetGL >= LANG_150;
    ext.ARB_shading_language_420pack = targetGL >= LANG_420;

    if (data->m_Kernels.empty() || data->m_Kernels[0].code.empty())
        return;

    DebugAssert(data->m_Kernels.size() == 1);
    DebugAssert(data->m_KernelCBs.size() == 1);

    GLSLShader result;
    ComputeShaderKernel* kernel = &data->m_Kernels[0];
    unsigned int flags = HLSLCC_FLAG_UNIFORM_BUFFER_OBJECT | HLSLCC_FLAG_COMBINE_TEXTURE_SAMPLERS | HLSLCC_FLAG_TRANSLATE_MATRICES;
    // Can't do this: all kernels in a shader must have identical constant buffers | HLSLCC_FLAG_REMOVE_UNUSED_GLOBALS;
    if (isES2)
        flags &= ~HLSLCC_FLAG_UNIFORM_BUFFER_OBJECT;
    if (isES3)
        flags |= HLSLCC_FLAG_GLES31_IMAGE_QUALIFIERS;
    if (isSwitch)
        flags |= HLSLCC_FLAG_NVN_TARGET | HLSLCC_FLAG_AVOID_SHADER_ATOMIC_COUNTERS | HLSLCC_FLAG_UNIFORM_BUFFER_OBJECT_WITH_INSTANCE_NAME;

    if (HasFlag(input->compileFlags, kShaderCompFlags_DisableFastMath))
        flags |= HLSLCC_FLAG_DISABLE_FASTMATH;

    ComputeShader::CBArray constantsInUse(data->m_KernelCBs[0]);

    if (input->api == kShaderCompPlatformMetal)
    {
        // Discard DX reflection as we'll get valid data with the HLSLcc reflection
        data->m_KernelCBs[0].clear_dealloc();
        kernel->textures.clear_dealloc();
        kernel->inBuffers.clear_dealloc();
        kernel->outBuffers.clear_dealloc();
        kernel->builtinSamplers.clear_dealloc();
    }

    HLSLccSamplerPrecisionInfo samplers;
    UnityHLSLccComputeReflection refl(data, 0, output->errors, constantsInUse);
    int compiledOK = TranslateHLSLFromMem((const char*)kernel->code.data(), flags, targetGL, &ext, NULL, samplers, refl, &result);
    if (!compiledOK || refl.HasErrors())
    {
        printf_console("TranslateHLSLFromMem failed! Type: %i Lang: %i Source: %s", result.shaderType, result.GLSLLanguage, result.sourceCode.c_str());
        return;
    }

#if defined(__APPLE__) && defined(_DEBUG)
    if (input->api == kShaderCompPlatformMetal)
    {
        PerformMetalVerification(input, output, result.sourceCode);
    }
#endif

    size_t srcLen = result.sourceCode.length() + 1; // include null terminator
    size_t metaDataLen = (input->api == kShaderCompPlatformMetal ? 3 * sizeof(unsigned int) : 0); // Metal thread group size is passed as metadata with the source

    core::string disassemblyString;
    if (HasFlag(input->compileFlags, kShaderCompFlags_EmbedHLSLccDisassembly))
    {
        D3D10Blob* d3dDisassembledShader = NULL;
        HRESULT hr = gPluginDispatcher->GetD3DCompiler()->D3DDisassemble(kernel->code.data(), kernel->code.size(), 0 , 0, &d3dDisassembledShader);
        if (SUCCEEDED_IMPL(hr))
        {
            disassemblyString += "\n#if 0\n";
            size_t disassemblyLenMinusNullTerminator = d3dDisassembledShader->GetBufferSize() - 1;
            disassemblyString.insert(disassemblyString.length(), reinterpret_cast<const char*>(d3dDisassembledShader->GetBufferPointer()), disassemblyLenMinusNullTerminator);
            disassemblyString += "\n#endif\n";
        }
        SAFE_RELEASE(d3dDisassembledShader);
    }

    kernel->code.resize_uninitialized(srcLen + metaDataLen);
    memcpy(kernel->code.data(), result.sourceCode.c_str(), srcLen);

    if (input->api == kShaderCompPlatformMetal)
    {
        memcpy(kernel->code.data() + srcLen, refl.GetThreadGroupSize(), metaDataLen);
    }

    if (!disassemblyString.empty())
    {
        dynamic_array<UInt8>::iterator beforeNullTerminator = &(kernel->code.back());
        const UInt8* disassembly = reinterpret_cast<const UInt8*>(disassemblyString.c_str());
        kernel->code.insert(beforeNullTerminator, disassembly, disassembly + disassemblyString.length());
    }

    if (input->api == kShaderCompPlatformOpenGLCore || isES)
    {
        // Dump built-in sampler info retrieved from hlsl reflection as it doesn't match generated texture-sampler pairs.
        // Instead generate a sampler list with an entry for each texture present (InlineSamplerType::kCount if no separate sampler for that texture).
        kernel->builtinSamplers.clear_dealloc();

        ComputeShaderBuiltinSampler newSampler;
        int currentTextureUnit = 0;
        // Reset texture bind points to sequential texture units starting from zero (just to make things easier)
        for (size_t t = 0, nt = kernel->textures.size(); t < nt; t++)
        {
            kernel->textures[t].bindPoint = currentTextureUnit++;
            kernel->textures[t].generatedName = kernel->textures[t].name;

            // These textures don't have separate samplers
            newSampler.sampler.SetInvalid();
            newSampler.bindPoint = 0;
            kernel->builtinSamplers.push_back(newSampler);
        }

        // Add texture entries and corresponding built-in samplers for generated texture-sampler pairs
        for (size_t p = 0; p < result.textureSamplers.size(); p++)
        {
            ComputeShaderResource newTex;
            InlineSamplerType samplerState;
            samplerState.SetInvalid();
            core::string tName = result.textureSamplers[p];
            char *textureName = (char*)tName.c_str();
            newTex.generatedName.SetName(textureName); // gles will see this generated texture-sampler pair name

            // HLSLcc prefixes shadow samplers with 'hlslcc_zcmp', so ignore that part if found in the name
            if (strstr(textureName, "hlslcc_zcmp") == textureName)
                textureName = textureName + 11;

            char* delimiter = strstr(textureName, "TEX_with_SMP"); // HLSLcc generates <texture>TEX_with_SMP<sampler> names
            if (delimiter != NULL)
            {
                *delimiter = '\0';
                newTex.name.SetName(textureName); // user points only to the texture part of the name

                char* samplerName = delimiter + 12;

                // sampler(_)<texture> refers to texture's own sampler state
                if (!CheckSamplerAndTextureNameMatch(textureName, samplerName))
                {
                    core::string errorMsg;
                    samplerState = ParseInlineSamplerName(core::string(samplerName), errorMsg);
                    if (samplerState.IsInvalid() && !errorMsg.empty())
                        output->errors.AddImportError(errorMsg, input->startLine, false, input->api);
                }
            }

            if (samplerState.IsInvalid())
            {
                // No separate sampler. Find the original texture and use the same bind point.
                for (size_t t = 0, nt = kernel->textures.size(); t < nt; t++)
                {
                    if (kernel->textures[t].name == newTex.name)
                    {
                        newTex.bindPoint = kernel->textures[t].bindPoint;
                        break;
                    }
                }

                newSampler.sampler.SetInvalid();
                newSampler.bindPoint = 0;
            }
            else
            {
                // Reserve a new texture unit and set it to both texture and sampler info
                newTex.bindPoint = currentTextureUnit++;

                newSampler.sampler = samplerState;
                newSampler.bindPoint = newTex.bindPoint;
            }

            kernel->textures.push_back(newTex);
            kernel->builtinSamplers.push_back(newSampler);
        }

        // Check number of buffer bindings
        size_t numBuffers = kernel->inBuffers.size();
        for (size_t b = 0; b < kernel->outBuffers.size(); ++b)
        {
            if (!(kernel->outBuffers[b].bindPoint & 0x80000000))
                numBuffers++;
        }

        if (numBuffers > gl::kMaxShaderStorageBufferBindings) // our internal limit
        {
            output->errors.AddImportError("Buffer count exceeding hard limit. No known hw supports this shader. Consider combining separate buffers into one containing struct elements.", 0, false, input->api);
        }
        else
        {
            if ((isES && numBuffers > 4) || (!isES && numBuffers > 8)) // defaults from GL/GLES spec
                output->errors.AddImportError("Buffer count exceeding default GL_MAX_SHADER_STORAGE_BUFFER_BINDINGS. Shader might not work on all hw. Consider combining separate buffers into one containing struct elements.", 0, true, input->api);
        }
    }

    data->m_ResourcesResolved = true;
}

struct HLSLccPluginInterface : public PlatformPluginInterface
{
    virtual unsigned int GetShaderPlatforms()
    {
        return (1 << kShaderCompPlatformOpenGLCore) | (1 << kShaderCompPlatformGLES3Plus)
            | (1 << kShaderCompPlatformGLES20) | (1 << kShaderCompPlatformMetal);
    }

    virtual const char* GetPlatformName() { return "hlslcc"; }

    virtual int  RunPluginUnitTests(const char* includesPath) { return 0; }

    virtual void Shutdown()
    {
        // uncomment if compiler is moved to a dll
        // RegisterRuntimeInitializeAndCleanup::ExecuteCleanup();
    }

    virtual const ShaderCompileOutputInterface* CompileShader(const ShaderCompileInputInterface *input)
    {
        // for some reason we added both pragmas prefer_hlslcc and prefer_hlsl2glsl
        // and both support list of platforms (i honestly see zero uses cases for all that)
        // to simplify our lives check only if asked for gles2 to be compiled with hlsl2glsl
        const bool forceHLSL2GLSL = HasFlag(input->compileFlags, kShaderCompFlags_PreferHLSL2GLSL);
        if (forceHLSL2GLSL)
            return gPluginDispatcher->GetHLSL2GLSLPlugin()->CompileShader(input);

        return InvokeCompilerHLSLcc(input);
    }

    virtual const ShaderCompileOutputInterface* DisassembleShader(const ShaderCompileInputInterface *input)
    {
        // shader data is just text, so disassembly is trivial

        ShaderCompileOutputData* output = MakeOutputDataHLSLcc(input);

        const UInt8* compiledData = input->GetCompiledDataPtr();

        if (compiledData)
        {
            if (input->api == kShaderCompPlatformMetal)
            {
                MetalShaderBlobDesc desc = ExtractMetalShaderBlobDesc(compiledData, input->GetCompiledDataSize());
                output->disassembly.assign(desc.code, desc.codeLength);
            }
            else
                output->disassembly.assign(compiledData, compiledData + input->GetCompiledDataSize());
        }
        else
            output->disassembly = "// All GLSL source is contained within the vertex program";
        return output;
    }

    virtual const ShaderCompileOutputInterface* CompileComputeShader(const ShaderCompileInputInterface *input)
    {
        return InvokeComputeCompilerHLSLcc(input);
    }

    virtual const ShaderCompileOutputInterface* TranslateComputeKernel(const ShaderCompileInputInterface *input)
    {
        return InvokeComputeTranslatorHLSLcc(input);
    }

    virtual void ReleaseShader(const ShaderCompileOutputInterface* data)
    {
        delete data;
    }
};

static HLSLccPluginInterface gHLSLccPlugin;

// PLUGINAPI PlatformPluginInterface * PLUGINCONVENTION  GetPluginInterface(int cgBatchVersion, PlatformPluginDispatcher* dispatcher)
PlatformPluginInterface* GetHLSLccPluginInterface(int cgBatchVersion, PlatformPluginDispatcher* dispatcher)
{
    // if it's moved to a dll, uncomment this and corresponding cleanup in HLSLPluginInterface::Shutdown
    // critical for correct handling of FastPropertyNames in compute shaders from shader compilers in DLLs
    // RegisterRuntimeInitializeAndCleanup::ExecuteInitializations();

    const int cgBatchVersion_min = 0x0002;
    const int cgBatchVersion_max = 0x0002;

    // user version to plugin to be rejected by cgbatch or reject itself
    if (cgBatchVersion > cgBatchVersion_max || cgBatchVersion < cgBatchVersion_min)
        return NULL;

    return &gHLSLccPlugin;
}
