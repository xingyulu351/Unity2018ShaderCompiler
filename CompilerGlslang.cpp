#include "UnityPrefix.h"
#include "Editor/Src/Utility/TextUtilities.h"
#include "Editor/Src/Utility/d3d11/D3D11ReflectionAPI.h"
#include "External/ShaderCompilers/HLSLcc/include/hlslcc.h"
#include "External/ShaderCompilers/HLSLcc/src/cbstring/bstrlib.h"
#include "Runtime/GfxDevice/vulkan/VKShaderFormat.h"
#include "Runtime/GfxDevice/GLSLUtilities.h"
#include "External/Compression/smol-v/smolv.h"
#include "Runtime/Utilities/HashFunctions.h"
#include "Runtime/Utilities/PathNameUtility.h"
#include "Runtime/Utilities/Word.h"
#include "Runtime/Utilities/ArrayUtility.h"
#include "External/ShaderCompilers/glslang/OGLCompilersDLL/glslang_compile.h"


#include "Tools/UnityShaderCompiler/ComputeShaderCompiler.h"
#include "Tools/UnityShaderCompiler/ShaderCompiler.h"
#include "Tools/UnityShaderCompiler/APIPlugin.h"
#include "Tools/UnityShaderCompiler/Utilities/ProcessIncludes.h"
#include <map>
#include <sstream>
#include <vector>

#ifdef _DEBUG
#define DEBUG_DUMP_GLSLANG_OUTPUT_FILES 1
#else
#define DEBUG_DUMP_GLSLANG_OUTPUT_FILES 0
#endif

#if DEBUG_DUMP_GLSLANG_OUTPUT_FILES
// When dumping debug files, keep the intermediate phases saved so we can dump them to file in case of error
static core::string s_GlslangOriginalSourceString;
static core::string s_GlslangPreprocessedSourceString;
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

static ShaderCompileOutputData* MakeOutputDataGlslang(const ShaderCompileInputInterface* input)
{
    return new ShaderCompileOutputData(input->api, ShaderCompileOutputData::kBinaryOutputData);
}

struct KnownGlslangInput
{
    const char* name;
    ShaderChannel channel;
};

static const KnownGlslangInput kKnownGlslangInputs[] =
{
    { "in_POSITION0", kShaderChannelVertex },
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
};

static void TranslateToGLSL(const ShaderCompileInputInterface* input, core::string& sourceCode, bool isES, GLSLCrossDependencyData *cdData, HLSLccSamplerPrecisionInfo &info)
{
    unsigned int options = HLSLCC_FLAG_INOUT_SEMANTIC_NAMES |
        //      HLSLCC_FLAG_DISABLE_EXPLICIT_LOCATIONS |
        //      HLSLCC_FLAG_DISABLE_GLOBALS_STRUCT |
        //      HLSLCC_FLAG_GLOBAL_CONSTS_NEVER_IN_UBO |
        HLSLCC_FLAG_VULKAN_BINDINGS |
        HLSLCC_FLAG_VULKAN_SPECIALIZATION_CONSTANTS |
        HLSLCC_FLAG_REMOVE_UNUSED_GLOBALS |
        HLSLCC_FLAG_TRANSLATE_MATRICES |
        HLSLCC_FLAG_SEPARABLE_SHADER_OBJECTS |
        HLSLCC_FLAG_UNIFORM_BUFFER_OBJECT;


    // TODO VULKAN is this the target we want?
    const GLLang targetGL = isES ? LANG_ES_310 : LANG_440;

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

    HLSLccReflection dummyRefl;
    compiledOK = TranslateHLSLFromMem(byteCode, options, targetGL, &ext, cdData, info, dummyRefl, &result);
    if (!compiledOK)
    {
#if DEBUG_DUMP_GLSLANG_OUTPUT_FILES
        {
            FILE* f = fopen("debug-glsl-error-in-hlslcc.txt", "wb");
            fputs(s_GlslangPreprocessedSourceString.c_str(), f);
            fclose(f);
        }
        {
            FILE* f = fopen("debug-glsl-error-in-hlslcc-orig.txt", "wb");
            fputs(s_GlslangOriginalSourceString.c_str(), f);
            fclose(f);
        }
#endif

        printf_console("TranslateHLSLFromMem failed! Type: %i Lang: %i Source: %s", result.shaderType, result.GLSLLanguage, result.sourceCode.c_str());
        return;
    }

    sourceCode = result.sourceCode;
}

static bool IsVec4ArrayMatrix(core::string& name, int* rows = NULL, int* cols = NULL)
{
    if (name.compare(0, 10, "hlslcc_mtx") == 0)
    {
        if (rows != NULL)
            *rows *= atoi(core::string(1, name[10]).c_str());
        if (cols != NULL)
            *cols = atoi(core::string(1, name[12]).c_str());
        name.erase(0, 13);
        return true;
    }
    return false;
}

// Unity-side implementation of the reflection interface
class UnityGlslangReflection : public GlslangReflection
{
private:
    const ShaderCompileInputInterface *m_Data;
    ShaderImportErrors& m_Errors;

    struct Constant
    {
        core::string m_Name;
        int m_Location;
        ShaderParamType m_Type;
        int m_Rows;
        int m_Cols;
        bool m_IsMatrix;
        int m_ArraySize;
    };

    struct StructConstant
    {
        core::string m_Name;
        uint32_t m_StructSize;
        int m_Location;
        int m_ArraySize;
        std::map<core::string, Constant> m_Members;
    };

    struct ConstantBuffer
    {
        core::string m_Name;
        int m_BufferSize;
        vk::Binding m_Binding;
        std::map<core::string, Constant> m_Members;
        std::map<core::string, StructConstant> m_Structs;

        void AddOrUpdateMember(ShaderImportErrors& errors, const char* namestr,
            int location,
            ShaderParamType type,
            int rows,
            int cols,
            bool isMatrix,
            int arraySize)
        {
            core::string name(namestr);
            auto memberMap = &m_Members;

            core::string structName, memberName, error;
            int arrayIndex;
            if (glsl::IsStructArrayUniform(namestr, structName, memberName, arrayIndex, error))
            {
                if (!error.empty())
                {
                    errors.AddImportError(Format("GLSL: %s. Ignoring.", error.c_str()), 0, true);
                    return;
                }

                auto structIt = m_Structs.find(structName);

                if (arrayIndex > 0)
                {
                    if (structIt == m_Structs.end())
                    {
                        // we run into a new struct name, yet the current member is not the first array element
                        errors.AddImportError(Format("GLSL: unexpected struct parameter '%s'. Ignoring.", name.c_str()), 0, true);
                    }
                    else
                    {
                        structIt->second.m_ArraySize = std::max(structIt->second.m_ArraySize, arrayIndex + 1);
                        if (arrayIndex == 1)
                        {
                            IsVec4ArrayMatrix(memberName);
                            auto mem = structIt->second.m_Members.find(memberName);
                            DebugAssert(mem != m_Members.end());
                            structIt->second.m_StructSize = location - mem->second.m_Location;
                        }
                    }
                    return;
                }

                if (structIt == m_Structs.end())
                {
                    structIt = m_Structs.insert(std::make_pair(structName, StructConstant())).first;
                    StructConstant &st = structIt->second;
                    st.m_Name = structName;
                    st.m_StructSize = 0;
                    st.m_Location = 0;
                    st.m_ArraySize = 1;
                }

                int oldOffset = structIt->second.m_Location;
                int newOffset = structIt->second.m_Location = std::min(structIt->second.m_Location, location);
                if (oldOffset != newOffset)
                {
                    for (auto sm = structIt->second.m_Members.begin(); sm != structIt->second.m_Members.end(); ++sm)
                        sm->second.m_Location += oldOffset - newOffset;
                }
                name = memberName;
                location -= structIt->second.m_Location;
                memberMap = &structIt->second.m_Members;
            }

            // Strip the possible hlslcc_matX from the start of the name, make the vec4 arrays matrices again
            if (IsVec4ArrayMatrix(name, &rows, &cols))
            {
                isMatrix = true;
                arraySize /= rows;
                if (arraySize == 1)
                    arraySize = 0;
            }

            auto itr = memberMap->find(name);
            if (itr != memberMap->end())
            {
                AssertMsg(itr->second.m_Location == location, "Conflicting constant buffer declarations?");
            }
            else
            {
                Constant& c = memberMap->insert(std::make_pair(name, Constant())).first->second;
                c.m_Name = name;
                c.m_Location = location;
                c.m_Type = type;
                c.m_Rows = rows;
                c.m_Cols = cols;
                c.m_IsMatrix = isMatrix;
                c.m_ArraySize = arraySize;
            }
        }
    };

    struct TextureBinding
    {
        core::string m_Name;
        vk::Binding m_Binding;
        TextureDimension m_Dimension;
        uint32_t m_OrigBindIndex;
        bool m_IsUAV;
        bool m_MultiSampled;
        uint32_t m_SamplerBinding;
    };

    struct SamplerBinding
    {
        core::string m_Name;
        vk::Binding m_Binding;
    };

    struct BufferBinding
    {
        core::string m_Name;
        vk::Binding m_Binding;
        uint32_t m_OrigBindIndex;
        bool m_IsUAV;
    };

    struct SpecializationConstant
    {
        core::string m_Name;
        vk::Binding m_Binding;
    };

    // In order to catch which shader stage uses each resource, we'll need to defer calling the actual callbacks
    // We also need to do this because glslang reflection prunes unused cbuffer members from each stage,
    // but different shader stages may use different members from same cbuffer.
    std::map<core::string, ConstantBuffer> m_CBs;
    std::map<core::string, TextureBinding> m_Textures;
    std::map<core::string, SamplerBinding> m_Samplers;
    std::map<core::string, BufferBinding> m_Buffers;
    std::map<int, SpecializationConstant> m_SpecializationConstants;

    ConstantBuffer *FindOrCreateConstantBuffer(const core::string &name, size_t size)
    {
        auto itr = m_CBs.find(name);

        if (itr != m_CBs.end())
        {
            AssertMsg(itr->second.m_BufferSize == size, "Conflicting Constant buffer sizes across shader stages?");
            return &itr->second;
        }

        ConstantBuffer cb;
        cb.m_Name = name;
        cb.m_BufferSize = (int)size;
        cb.m_Members.clear();
        cb.m_Binding.FromUint(0);
        return &m_CBs.insert(std::make_pair(name, cb)).first->second;
    }

    void AddOrUpdateSpecializationConstant(const core::string &name, int bindLocation)
    {
        auto itr = m_SpecializationConstants.find(bindLocation);

        if (itr != m_SpecializationConstants.end())
        {
            AssertMsg(itr->second.m_Name == name, "Conflicting specialization constants across shader stages?");
            // Update shader stage flags
            itr->second.m_Binding.m_ShaderStages |= (1 << m_CurrentStage);
            return;
        }

        SpecializationConstant sc;
        sc.m_Name = name;
        sc.m_Binding.FromUint(0);
        sc.m_Binding.m_Binding = bindLocation;
        sc.m_Binding.m_ShaderStages = (1 << m_CurrentStage);
        m_SpecializationConstants.insert(std::make_pair(bindLocation, sc));
    }

    ShaderCompilerProgram m_CurrentStage;
    ConstantBuffer *m_CurrentConstantBuffer; // The CB we're currently filling
    size_t m_MembersLeftInCurrentCB;
public:

    UnityGlslangReflection(const ShaderCompileInputInterface *data, ShaderImportErrors& errors)
        : m_Data(data),
        m_Errors(errors),
        m_CurrentStage(kProgramVertex),
        m_CBs(),
        m_Textures(),
        m_Samplers(),
        m_Buffers(),
        m_CurrentConstantBuffer(NULL)
    {}

    // Flush all deferred reflection info calls. Constant buffers for Globals (P/VGlobals etc) have the shaderID appended to the end.
    void Flush(const core::string &shaderID)
    {
        // Specialization constants
        for (auto scItr = m_SpecializationConstants.begin(); scItr != m_SpecializationConstants.end(); scItr++)
        {
            SpecializationConstant &sc = scItr->second;
            m_Data->reflectionReport->OnConstant(sc.m_Name.c_str(), sc.m_Binding.ToUint(), kShaderParamInt, kConstantTypeDefault, 1, 1, 0);
        }

        for (auto cbItr = m_CBs.begin(); cbItr != m_CBs.end(); cbItr++)
        {
            ConstantBuffer &cb = cbItr->second;
            core::string cbName = cb.m_Name;
            if (cbName.find("Globals") != core::string::npos)
                cbName += shaderID;

            m_Data->reflectionReport->OnConstantBuffer(cbName.c_str(), cb.m_BufferSize, (int)cb.m_Members.size());

            for (auto sItr = cb.m_Structs.begin(); sItr != cb.m_Structs.end(); ++sItr)
            {
                StructConstant& st = sItr->second;
                m_Data->reflectionReport->OnConstant(st.m_Name.c_str(), st.m_Location, kShaderParamFloat, kConstantTypeStruct, st.m_StructSize, 1, st.m_ArraySize);
                for (auto mItr = st.m_Members.begin(); mItr != st.m_Members.end(); ++mItr)
                {
                    Constant& c = mItr->second;
                    core::string structMemName = st.m_Name + "." + c.m_Name;
                    m_Data->reflectionReport->OnConstant(structMemName.c_str(), c.m_Location, c.m_Type, c.m_IsMatrix ? kConstantTypeMatrix : kConstantTypeDefault, c.m_Rows, c.m_Cols, c.m_ArraySize);
                }
            }

            ///
            for (auto cItr = cb.m_Members.begin(); cItr != cb.m_Members.end(); cItr++)
            {
                Constant &c = cItr->second;
                m_Data->reflectionReport->OnConstant(c.m_Name.c_str(), c.m_Location, c.m_Type, c.m_IsMatrix ? kConstantTypeMatrix : kConstantTypeDefault, c.m_Rows, c.m_Cols, c.m_ArraySize);
            }
            if (cb.m_Binding.ToUint() != 0)
                m_Data->reflectionReport->OnCBBinding(cbName.c_str(), (int)cb.m_Binding.ToUint());
        }

        // Samplers. Also find the textures they correspond to
        for (auto sItr = m_Samplers.begin(); sItr != m_Samplers.end(); sItr++)
        {
            SamplerBinding &sb = sItr->second;
            bool samplerMatched = false;
            for (auto tItr = m_Textures.begin(); tItr != m_Textures.end(); tItr++)
            {
                TextureBinding &tb = tItr->second;
                if (CheckSamplerAndTextureNameMatch(tb.m_Name, sb.m_Name))
                {
                    tb.m_SamplerBinding = sb.m_Binding.ToUint();
                    samplerMatched = true;
                }
            }

            if (!samplerMatched)
            {
                core::string samplerMsg;
                InlineSamplerType inlineSampler = ParseInlineSamplerName(sb.m_Name, samplerMsg);
                if (!inlineSampler.IsInvalid())
                {
                    m_Data->reflectionReport->OnInlineSampler(inlineSampler, sb.m_Binding.ToUint());
                }
                else
                {
                    core::string msg = Format("%s program '%s': %s.\n",
                        kProgramTypeNames[m_Data->programType], m_Data->GetEntryName(), samplerMsg.c_str());
                    m_Errors.AddImportError(msg, 1, false, kShaderCompPlatformVulkan);
                }
            }
        }

        // Textures
        for (auto tItr = m_Textures.begin(); tItr != m_Textures.end(); tItr++)
        {
            TextureBinding &tb = tItr->second;
            if (!tb.m_IsUAV)
                m_Data->reflectionReport->OnTextureBinding(tb.m_Name.c_str(), (int)tb.m_Binding.ToUint(), (int)tb.m_SamplerBinding, tb.m_MultiSampled, tb.m_Dimension);
            else
                m_Data->reflectionReport->OnUAVBinding(tb.m_Name.c_str(), (int)tb.m_Binding.ToUint(), (int)tb.m_OrigBindIndex);
        }

        // Buffers
        for (auto bItr = m_Buffers.begin(); bItr != m_Buffers.end(); bItr++)
        {
            BufferBinding &bb = bItr->second;
            if (!bb.m_IsUAV)
                m_Data->reflectionReport->OnBufferBinding(bb.m_Name.c_str(), (int)bb.m_Binding.ToUint());
            else
            {
                m_Data->reflectionReport->OnUAVBinding(bb.m_Name.c_str(), (int)bb.m_Binding.ToUint(), (int)bb.m_OrigBindIndex);
            }
        }

        m_CBs.clear();
        m_Textures.clear();
        m_SpecializationConstants.clear();
    }

    void SetCurrentShaderStage(ShaderCompilerProgram newStage)
    {
        m_CurrentStage = newStage;
    }

    virtual void OnInputBinding(const char* name, int bindIndex)
    {
        if (m_Data->reflectionReport == NULL)
            return;

        for (int j = 0; j < sizeof(kKnownGlslangInputs) / sizeof(kKnownGlslangInputs[0]); ++j)
        {
            if (!strcmp(name, kKnownGlslangInputs[j].name))
            {
                // These do not need to be deferred
                m_Data->reflectionReport->OnInputBinding(kKnownGlslangInputs[j].channel, (VertexComponent)(kVertexCompAttrib0 + bindIndex));
                break;
            }
        }
    }

    virtual void OnConstantBuffer(const char* name, size_t bufferSize, size_t memberCount)
    {
        if (!m_Data || !m_Data->reflectionReport)
            return;
        //      m_Data->reflectionReport->OnConstantBuffer(name.c_str(), (int)bufferSize, (int)memberCount);
        m_CurrentConstantBuffer = FindOrCreateConstantBuffer(name, bufferSize);
        m_MembersLeftInCurrentCB = memberCount;
    }

    virtual void OnConstant(const char* name, int bindIndex, VarType cType, int rows, int cols, bool isMatrix, int arraySize)
    {
        if (!m_Data || !m_Data->reflectionReport)
            return;

        ShaderParamType vType = kShaderParamFloat;
        switch (cType)
        {
            case GlslangReflection::VT_FLOAT:
                vType = kShaderParamFloat;
                break;

            case GlslangReflection::VT_HALF:
                vType = kShaderParamHalf;
                break;

            case GlslangReflection::VT_INT:
                vType = kShaderParamInt;
                break;

            case GlslangReflection::VT_SHORT:
                vType = kShaderParamShort;

            default:
                AssertMsg(0, "Unknown Shader param type?");
                break;
        }

        if (m_CurrentConstantBuffer == NULL)
        {
            // If not inside any constant buffer, this is a specialization constant, store it appropriately
            AddOrUpdateSpecializationConstant(name, bindIndex);
        }
        else
        {
            m_CurrentConstantBuffer->AddOrUpdateMember(m_Errors, name, bindIndex, vType, rows, cols, isMatrix, arraySize);
            m_MembersLeftInCurrentCB--;
            if (m_MembersLeftInCurrentCB == 0)
                m_CurrentConstantBuffer = nullptr;
        }
    }

    virtual void OnConstantBufferBinding(const char* name, int bindIndex)
    {
        if (!m_Data || !m_Data->reflectionReport)
            return;

        //      m_Data->reflectionReport->OnCBBinding(name.c_str(), bindIndex);

        auto itr = m_CBs.find(name);
        AssertMsg(itr != m_CBs.end(), "Attempting to bind an unknown constant buffer?");

        vk::Binding newBind;
        newBind.FromUint((uint32_t)bindIndex);
        newBind.m_ShaderStages = (1 << m_CurrentStage);

        ConstantBuffer &cb = itr->second;
        if (cb.m_Binding.m_ShaderStages == 0)
        {
            // Copy initial binding info
            cb.m_Binding = newBind;
        }
        else
        {
            AssertMsg(cb.m_Binding.m_Binding == newBind.m_Binding, "CB bindings don't match across shader stages");
            AssertMsg(cb.m_Binding.m_Set == newBind.m_Set, "CB bindings don't match across shader stages");

            // Add the shader stage from the new bind
            cb.m_Binding.m_ShaderStages |= newBind.m_ShaderStages;
        }
    }

    virtual void OnSamplerBinding(const char* name, int bindIndex)
    {
        if (!m_Data || !m_Data->reflectionReport)
            return;

        vk::Binding newBind;
        newBind.FromUint((uint32_t)bindIndex);
        newBind.m_ShaderStages = (1 << m_CurrentStage);

        auto itr = m_Samplers.find(name);
        if (itr != m_Samplers.end())
        {
            SamplerBinding &sb = itr->second;
            AssertMsg(sb.m_Binding.m_Binding == newBind.m_Binding, "Sampler bindings don't match across shader stages");
            AssertMsg(sb.m_Binding.m_Set == newBind.m_Set, "Sampler bindings don't match across shader stages");

            // Merge stage flags
            sb.m_Binding.m_ShaderStages |= newBind.m_ShaderStages;
        }
        else
        {
            SamplerBinding &sb = m_Samplers.insert(std::make_pair(std::string(name), SamplerBinding())).first->second;
            sb.m_Name = name;
            sb.m_Binding = newBind;
        }
    }

    virtual void OnTextureBinding(const char* name, int bindIndex, bool multisampled, TexDimension dim, bool isUAV)
    {
        if (!m_Data || !m_Data->reflectionReport)
            return;
        TextureDimension tDim = kTexDim2D;
        switch (dim)
        {
            case GlslangReflection::TD_2D:
                tDim = kTexDim2D;
                break;
            case GlslangReflection::TD_2DARRAY:
                tDim = kTexDim2DArray;
                break;
            case GlslangReflection::TD_2DSHADOW:
                tDim = kTexDim2D;
                break;
            case GlslangReflection::TD_3D:
                tDim = kTexDim3D;
                break;
            case GlslangReflection::TD_CUBE:
                tDim = kTexDimCUBE;
                break;
            case GlslangReflection::TD_CUBEARRAY:
                tDim = kTexDimCubeArray;
                break;
            case GlslangReflection::TD_CUBESHADOW:
                tDim = kTexDimCUBE;
                break;
            case GlslangReflection::TD_1D:
            default:
                AssertMsg(0, "Unsupported texture dimension");
                break;
        }

        vk::Binding newBind;
        newBind.FromUint((uint32_t)bindIndex);
        newBind.m_ShaderStages = (1 << m_CurrentStage);

        core::string modName = name;
        uint32_t origBindIndex = bindIndex;
        // Scan for the original bind index marker, if UAV
        if (isUAV)
        {
            const char *marker = "_origX%dX";
            size_t startLoc = modName.find("_origX");
            if ((startLoc != core::string::npos) && (sscanf(modName.c_str() + startLoc, marker, &origBindIndex) == 1))
            {
                // Get rid of all markers
                while ((startLoc = modName.find("_origX")) != core::string::npos)
                {
                    size_t endLoc = modName.find('X', startLoc + 6);
                    if (endLoc == core::string::npos)
                        break;
                    modName.erase(startLoc, endLoc - startLoc + 1);
                }
            }
        }

        // Shadow samplers have 'hlslcc_zcmp" prefix, get rid of it here, but still use the prefix for table lookup to prevent missing bindings
        core::string keyName = modName;
        if (modName.find("hlslcc_zcmp") == 0)
        {
            modName.erase(0, 11);
        }

        auto itr = m_Textures.find(keyName);
        if (itr != m_Textures.end())
        {
            TextureBinding &tb = itr->second;
            AssertMsg(tb.m_Dimension == tDim, "Texture bindings don't match");
            AssertMsg(tb.m_IsUAV == isUAV, "Texture bindings don't match");

            AssertMsg(tb.m_Binding.m_Binding == newBind.m_Binding, "CB bindings don't match across shader stages");
            AssertMsg(tb.m_Binding.m_Set == newBind.m_Set, "CB bindings don't match across shader stages");

            // Merge stage flags
            tb.m_Binding.m_ShaderStages |= newBind.m_ShaderStages;
        }
        else
        {
            TextureBinding &tb = m_Textures.insert(std::make_pair(keyName, TextureBinding())).first->second;
            tb.m_Name = modName;
            tb.m_Binding = newBind;
            tb.m_Dimension = tDim;
            tb.m_IsUAV = isUAV;
            tb.m_MultiSampled = multisampled;
            tb.m_OrigBindIndex = origBindIndex;
            tb.m_SamplerBinding = -1;
        }
    }

    virtual void OnBufferBinding(const char* name, int bindIndex, bool isUAV, TexelBufferType texelBufferType)
    {
        if (!m_Data || !m_Data->reflectionReport)
            return;

        vk::Binding newBind;
        newBind.FromUint((uint32_t)bindIndex);
        newBind.m_ShaderStages = (1 << m_CurrentStage);
        newBind.m_IsBuffer = isUAV ? 1 : 0;
        newBind.m_TexelBufferFormat = texelBufferType;

        uint32_t origBindIndex = bindIndex;
        // Scan for the original bind index marker, if UAV
        core::string modName = name;
        if (isUAV)
        {
            const char *marker = "_origX%dX";
            size_t startLoc = modName.find("_origX");
            // Skip counter buffers
            if ((startLoc != core::string::npos) && (modName.find("X_counterBuf") == core::string::npos))
            {
                if (sscanf(modName.c_str() + startLoc, marker, &origBindIndex) == 1)
                {
                    // Get rid of all markers
                    while ((startLoc = modName.find("_origX")) != core::string::npos)
                    {
                        size_t endLoc = modName.find('X', startLoc + 6);
                        if (endLoc == core::string::npos)
                            break;
                        modName.erase(startLoc, endLoc - startLoc + 1);
                    }
                }
            }
        }
        auto itr = m_Buffers.find(modName);
        if (itr != m_Buffers.end())
        {
            BufferBinding &bb = itr->second;
            AssertMsg(bb.m_IsUAV == isUAV, "Buffer bindings don't match");

            AssertMsg(bb.m_Binding.m_Binding == newBind.m_Binding, "Buffer bindings don't match across shader stages");
            AssertMsg(bb.m_Binding.m_Set == newBind.m_Set, "Buffer bindings don't match across shader stages");

            // Merge stage flags
            bb.m_Binding.m_ShaderStages |= newBind.m_ShaderStages;
        }
        else
        {
            BufferBinding &bb = m_Buffers.insert(std::make_pair(modName, BufferBinding())).first->second;
            bb.m_Name = modName;
            bb.m_Binding = newBind;
            bb.m_IsUAV = isUAV;
            bb.m_OrigBindIndex = origBindIndex;
        }
    }

    virtual void OnThreadGroupSize(unsigned int xSize, unsigned int ySize, unsigned int zSize) {}
};

static void CompileGLSLToSPIRV(const ShaderCompileInputInterface *data, core::string glsltext, std::vector<unsigned int> &spirvwords, ShaderImportErrors &errors, GlslangReflection &refl, bool isCompute = false)
{
    GlslangShaderLanguage langs[] =
    {
        GlslangVertex,
        GlslangFragment,
        GlslangTessControl,
        GlslangTessEvaluation,
        GlslangGeometry,
        GlslangVertex, // surface
    };
    CompileTimeAssertArraySize(langs, kProgramCount);

    GlslangCompileContext *ctx = CompileShader(isCompute ? GlslangCompute : langs[data->programType], glsltext.c_str(), &refl);

    if (ctx == NULL)
    {
        // even if there is an error, CompileShader should always return a context,
        // but just in case..
        errors.GetErrors().insert(ShaderImportError("Internal error calling glslang compiler", "", 0, false, kShaderCompPlatformVulkan));
        return;
    }

    if (!IsShaderCompiled(ctx))
    {
        const char* msg = GetShaderCompileErrors(ctx);
        ParseErrorMessagesPostfix(msg, false, "INTERNAL ERROR: ", data->GetEntryName(), data->startLine, data->api, errors);
        ParseErrorMessagesPostfix(msg, false, "UNIMPLEMENTED: ", data->GetEntryName(), data->startLine, data->api, errors);
        ParseErrorMessagesPostfix(msg, false, "ERROR: ", data->GetEntryName(), data->startLine, data->api, errors);
        ParseErrorMessagesPostfix(msg, true, "WARNING: ", data->GetEntryName(), data->startLine, data->api, errors);
        ParseErrorMessagesPostfix(msg, true, "NOTE: ", data->GetEntryName(), data->startLine, data->api, errors);

#if DEBUG_DUMP_GLSLANG_OUTPUT_FILES
        {
            FILE* f = fopen("debug-glsl-error-in-glslang.txt", "wb");
            fputs(glsltext.c_str(), f);
            fclose(f);
        }
#endif

        DestroyShaderContext(ctx);
        return;
    }

    spirvwords.resize(GetShaderSPIRVLength(ctx));
    if (!spirvwords.empty())
    {
        memcpy(&spirvwords[0], GetShaderSPIRVWords(ctx), spirvwords.size() * sizeof(unsigned int));
    }

    DestroyShaderContext(ctx);
}

namespace HLSLcc
{
    // In CompilerHLSLcc.cpp
    bool ProcessSamplerPrecisions(ShaderCompileInputData* localInput, core::string& preprocessedOut, ShaderCompileOutputData* output, HLSLccSamplerPrecisionInfo& samplers);
}

static bool SpirvDebugNameFilter(const char* name)
{
    if (!name)
        return false;

    // There are issues in some released Vulkan implementations on Adreno and Mali (case 963224 and sub-cases)
    // The most reliable workaround so far is to preserve OpName of vertex shader outputs.
    return BeginsWith(name, "vs_TEXCOORD");
}

ShaderCompileOutputInterface* InvokeCompilerGlslang(const ShaderCompileInputInterface* input)
{
    Assert(input->api == kShaderCompPlatformVulkan);

    ShaderCompileOutputData *output = MakeOutputDataGlslang(input);

    GLSLCrossDependencyData cdData;

    if (input->programType != kProgramVertex)
    {
        // actual fragment/hull/ program was emitted as part of vertex shader;
        // just output dummy header for the other ones
        output->gpuProgramType = kShaderGpuProgramSPIRV;
        return output;
    }

    // check
    if (input->GetEntryName(kProgramVertex)[0] == 0 || input->GetEntryName(kProgramFragment)[0] == 0)
    {
        output->errors.AddImportError("Compiling for Vulkan requires both vertex & fragment programs", input->startLine, false, input->api);
        return output;
    }

    // Build the program stages flags. We'll always have pixel and vertex shaders anyway.
    cdData.ui32ProgramStages = PS_FLAG_VERTEX_SHADER | PS_FLAG_PIXEL_SHADER;
    if (input->GetEntryName(kProgramHull)[0] != 0)
        cdData.ui32ProgramStages |= PS_FLAG_HULL_SHADER;

    if (input->GetEntryName(kProgramDomain)[0] != 0)
        cdData.ui32ProgramStages |= PS_FLAG_DOMAIN_SHADER;

    if (input->GetEntryName(kProgramGeometry)[0] != 0)
        cdData.ui32ProgramStages |= PS_FLAG_GEOMETRY_SHADER;

    static const ShaderCompilerProgram kShaderCompilationOrder[kProgramCount] =
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

    // Zero-initialize the header
    dynamic_array<UInt8> ourOutput;
    ourOutput.resize_initialized(sizeof(vk::ShaderSetHeader), 0);
    vk::ShaderFlagBits shaderFlags = 0;
    UnityGlslangReflection refl(input, output->errors);

    bool isES = false; // We always compile in desktop mode now.
    core::string hlslIn = input->GetSource();
    // Detect the use of SV_ClipDistance and update flags accordingly
    if (hlslIn.find("SV_ClipDistance") != core::string::npos)
    {
        shaderFlags |= vk::kShaderUsesClipDistance;
    }

    for (int k = 0; k < kProgramCount; k++)
    {
        const ShaderCompilerProgram programType = kShaderCompilationOrder[k];
        if (input->GetEntryName(programType)[0] == 0)
            continue;

        ShaderCompileInputData localInput;
        localInput.CopyFrom(input);
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

        // Add available feature macros
        ShaderRequirements availableRequirements = localInput.requirements | GetCompilePlatformMinSpecFeatures(localInput.api);
        AddAvailableShaderTargetRequirementsMacros(availableRequirements, localInput.args);
        localInput.compileFlags |= kShaderCompFlags_DoNotAddShaderAvailableMacros;

        refl.SetCurrentShaderStage(programType);

        localInput.programType = programType;

        HLSLccSamplerPrecisionInfo samplers;
        core::string ppdSource;
        if (!HLSLcc::ProcessSamplerPrecisions(&localInput, ppdSource, output, samplers))
        {
#if DEBUG_DUMP_GLSLANG_OUTPUT_FILES
            FILE* f = fopen("debug-glsl-error-in-preprocessor.txt", "wb");
            fputs(ppdSource.c_str(), f);
            fclose(f);
#endif
            return output;
        }

#if DEBUG_DUMP_GLSLANG_OUTPUT_FILES
        s_GlslangOriginalSourceString = localInput.source;
        s_GlslangPreprocessedSourceString = ppdSource;
#endif
        localInput.requirements = kShaderRequireShaderModel50_PC;

        const ShaderCompileOutputInterface* localOutput = NULL;
        if (gPluginDispatcher->HasCompilerForAPI(kShaderCompPlatformD3D11))
            localOutput = gPluginDispatcher->Get(kShaderCompPlatformD3D11)->CompileShader(&localInput);


        if (localOutput == NULL)
        {
            output->errors.AddImportError("Could not call HLSL compiler - missing plugin?", input->startLine, false, input->api);
            return output;
        }
        const UInt8* hlslBytecode = localOutput->GetCompiledDataPtr();

        if (localOutput->HasErrors() || hlslBytecode == NULL)
        {
#if DEBUG_DUMP_GLSLANG_OUTPUT_FILES
            {
                FILE* f = fopen("debug-glsl-error-in-d3dcompile.txt", "wb");
                fputs(s_GlslangPreprocessedSourceString.c_str(), f);
                fclose(f);
            }
            {
                FILE* f = fopen("debug-glsl-error-in-d3dcompile-orig.txt", "wb");
                fputs(s_GlslangOriginalSourceString.c_str(), f);
                fclose(f);
            }
#endif
            AddErrorsFrom(output->errors, &localInput, localOutput);
            gPluginDispatcher->Get(kShaderCompPlatformD3D11)->ReleaseShader(localOutput);
            return output;
        }

        localInput.gpuProgramData.assign(hlslBytecode, hlslBytecode + localOutput->GetCompiledDataSize());
        core::string sourceCode;
        TranslateToGLSL(&localInput, sourceCode, isES, &cdData, samplers);
        gPluginDispatcher->Get(kShaderCompPlatformD3D11)->ReleaseShader(localOutput);

        core::string glsltext;
        glsltext = sourceCode;

        if ((programType == kProgramGeometry) ||
            (programType == kProgramDomain && input->GetEntryName(kProgramGeometry)[0] == 0) ||
            (programType == kProgramVertex && input->GetEntryName(kProgramGeometry)[0] == 0 && input->GetEntryName(kProgramDomain)[0] == 0))
        {
            // Make Vulkan Viewport transformation similar to DirectX
            // TODO: Figure out if there is a better solution.
            if (programType == kProgramGeometry)
                replace_string(glsltext, "EmitVertex();", "gl_Position.y = -gl_Position.y; EmitVertex(); gl_Position.y = -gl_Position.y;");
            else
                replace_string(glsltext, "return;", "gl_Position.y = -gl_Position.y; return;");
        }

        // Check the fragment shader for specific attributes: depth write and discard
        if (programType == kProgramFragment)
        {
            if (glsltext.find("gl_FragDepth") != core::string::npos && glsltext.find("(depth_unchanged)") ==  core::string::npos)
                shaderFlags |= vk::kFragmentShaderModifiesDepthBit;
            if (glsltext.find("discard;") != core::string::npos)
                shaderFlags |= vk::kFragmentShaderHasDiscardBit;
            if (glsltext.find("image2D ") != core::string::npos)
                shaderFlags |= vk::kFragmentShaderHasUAVs;
            if (glsltext.find("image2DMS ") != core::string::npos)
                shaderFlags |= vk::kFragmentShaderHasUAVs;
            if (glsltext.find("image2DMSArray ") != core::string::npos)
                shaderFlags |= vk::kFragmentShaderHasUAVs;
            if (glsltext.find("image2DArray ") != core::string::npos)
                shaderFlags |= vk::kFragmentShaderHasUAVs;
            if (glsltext.find("image3D ") != core::string::npos)
                shaderFlags |= vk::kFragmentShaderHasUAVs;
            if (glsltext.find("imageCube ") != core::string::npos)
                shaderFlags |= vk::kFragmentShaderHasUAVs;
            if (glsltext.find("imageCubeArray ") != core::string::npos)
                shaderFlags |= vk::kFragmentShaderHasUAVs;
            if (glsltext.find("imageBuffer ") != core::string::npos)
                shaderFlags |= vk::kFragmentShaderHasUAVs;
        }

        std::vector<unsigned int> spirvwords;
        CompileGLSLToSPIRV(&localInput, glsltext, spirvwords, output->errors, refl);

        if (output->errors.HasErrors())
        {
            return output;
        }

        smolv::ByteArray smolvData;
        const UInt8* code = reinterpret_cast<const UInt8*>(&spirvwords[0]);
        const uint32_t spirvCodeSize = static_cast<uint32_t>(spirvwords.size() * sizeof(uint32_t));
        uint32_t compressedCodeSize = spirvCodeSize;
        // Debug builds won't strip debug info.
        uint32_t flags = UNITY_RELEASE ? smolv::StripDebugInfoBit : 0;

        if (smolv::Encode(code, spirvCodeSize, smolvData, flags, SpirvDebugNameFilter))
        {
            code = &smolvData[0];
            compressedCodeSize = static_cast<uint32_t>(smolvData.size());
        }

        // Update header
        vk::ShaderSetHeader* setHeader = reinterpret_cast<vk::ShaderSetHeader*>(ourOutput.data());
        setHeader->flags = shaderFlags;
        vk::ShaderFormatHeader& header = setHeader->shaders[programType];
        header.size = compressedCodeSize;
        header.offsetBytes = static_cast<UInt32>(ourOutput.size());

        ourOutput.insert(ourOutput.end(), code, code + compressedCodeSize);
    }
    // Calculate hash for the shader string and use that as the shader ID for globals cb
    uint32_t hash = ComputeHash32(&ourOutput[0], ourOutput.size());
    std::ostringstream oss;
    oss << hash;
    refl.Flush(oss.str());
    output->gpuProgramType = kShaderGpuProgramSPIRV;
    output->binaryOutput.swap(ourOutput);

    return output;
}

// Unity-side implementation of the reflection interface
class UnityComputeGlslangReflection : public GlslangReflection
{
private:
    ComputeShaderBinary *m_Data;
    int m_KernelIdx;

    struct Constant
    {
        core::string m_Name;
        int m_Location;
        ShaderParamType m_Type;
        int m_Rows;
        int m_Cols;
        bool m_IsMatrix;
        int m_ArraySize;
    };

    struct ConstantBuffer
    {
        core::string m_Name;
        int m_BufferSize;
        vk::Binding m_Binding;
        std::map<core::string, Constant> m_Members;

        void AddOrUpdateMember(const char* namestr,
            int location,
            ShaderParamType type,
            int rows,
            int cols,
            bool isMatrix,
            int arraySize)
        {
            core::string name(namestr);
            // Strip the possible hlslcc_matX from the start of the name, make the vec4 arrays matrices again
            if (name.substr(0, 10) == "hlslcc_mtx")
            {
                rows *= atoi(core::string(1, name[10]).c_str());
                cols = atoi(core::string(1, name[12]).c_str());
                name.erase(0, 13);
                isMatrix = true;
                arraySize /= rows;
                if (arraySize == 1)
                    arraySize = 0;
            }

            auto itr = m_Members.find(name);
            Constant *c = nullptr;
            if (itr != m_Members.end())
            {
                c = &itr->second;
                AssertMsg(c->m_Location == location, "Conflicting constant buffer declarations?");
            }
            else
            {
                c = &m_Members.insert(std::make_pair(name, Constant())).first->second;
                c->m_Name = name;
                c->m_Location = location;
                c->m_Type = type;
                c->m_Rows = rows;
                c->m_Cols = cols;
                c->m_IsMatrix = isMatrix;
                c->m_ArraySize = arraySize;
            }
        }
    };

    struct TextureBinding
    {
        core::string m_Name;
        vk::Binding m_Binding;
        TextureDimension m_Dimension;
        uint32_t m_OrigBindIndex;
        bool m_IsUAV;
        bool m_MultiSampled;
        uint32_t m_SamplerBinding;
    };

    struct SamplerBinding
    {
        core::string m_Name;
        vk::Binding m_Binding;
    };

    struct BufferBinding
    {
        core::string m_Name;
        vk::Binding m_Binding;
        uint32_t m_OrigBindIndex;
        bool m_IsUAV;
    };

    // In order to catch which shader stage uses each resource, we'll need to defer calling the actual callbacks
    // We also need to do this because glslang reflection prunes unused cbuffer members from each stage,
    // but different shader stages may use different members from same cbuffer.
    std::map<core::string, ConstantBuffer> m_CBs;
    std::map<core::string, TextureBinding> m_Textures;
    std::map<core::string, SamplerBinding> m_Samplers;
    std::map<core::string, BufferBinding> m_Buffers;

    ConstantBuffer *FindOrCreateConstantBuffer(const core::string &name, size_t size)
    {
        auto itr = m_CBs.find(name);

        if (itr != m_CBs.end())
        {
            AssertMsg(itr->second.m_BufferSize == size, "Conflicting Constant buffer sizes across shader stages?");
            return &itr->second;
        }

        ConstantBuffer cb;
        cb.m_Name = name;
        cb.m_BufferSize = (int)size;
        cb.m_Members.clear();
        cb.m_Binding.FromUint(0);
        return &m_CBs.insert(std::make_pair(name, cb)).first->second;
    }

    ConstantBuffer *m_CurrentConstantBuffer; // The CB we're currently filling
    size_t m_MembersLeftInCurrentCB;
    ShaderImportErrors &m_Errors;
public:

    UnityComputeGlslangReflection(ComputeShaderBinary *data, int kernelIdx, ShaderImportErrors &err) : m_Data(data), m_KernelIdx(kernelIdx), m_Errors(err), m_CBs(), m_Textures(), m_Buffers(), m_CurrentConstantBuffer(NULL) {}

    // Flush all deferred reflection info calls
    void Flush()
    {
        ComputeShader::CBArray cblist;
        for (auto cbItr = m_CBs.begin(); cbItr != m_CBs.end(); cbItr++)
        {
            ConstantBuffer &cb = cbItr->second;
            ComputeShaderCB ccb;
            ComputeShaderResource cbr;

            cbr.name.SetName(cb.m_Name.c_str());
            cbr.bindPoint = cb.m_Binding.ToUint();
            m_Data->m_Kernels[m_KernelIdx].cbs.push_back(cbr);

            ccb.name.SetName(cb.m_Name.c_str());
            ccb.byteSize = cb.m_BufferSize;

            for (auto cItr = cb.m_Members.begin(); cItr != cb.m_Members.end(); cItr++)
            {
                Constant &c = cItr->second;
                ComputeShaderParam csp;
                csp.name.SetName(c.m_Name.c_str());
                csp.offset = c.m_Location;
                csp.type = c.m_Type;
                csp.rowCount = c.m_Rows;
                csp.colCount = c.m_Cols;
                csp.arraySize = c.m_ArraySize;

                ccb.params.push_back(csp);
            }
            cblist.push_back(ccb);
        }

        m_Data->m_KernelCBs.push_back(cblist);

        std::vector<core::string> texNames;
        // Textures
        for (auto tItr = m_Textures.begin(); tItr != m_Textures.end(); tItr++)
        {
            TextureBinding &tb = tItr->second;
            if (!tb.m_IsUAV)
            {
                ComputeShaderResource cbr;
                cbr.name.SetName(tb.m_Name.c_str());
                cbr.bindPoint = tb.m_Binding.ToUint();
                cbr.samplerBindPoint = -1;
                cbr.texDimension = tb.m_Dimension;
                m_Data->m_Kernels[m_KernelIdx].textures.push_back(cbr);
                texNames.push_back(tb.m_Name);
            }
            else
            {
                ComputeShaderResource cbr;
                cbr.name.SetName(tb.m_Name.c_str());
                cbr.bindPoint = tb.m_Binding.ToUint();
                cbr.samplerBindPoint = -1;
                cbr.texDimension = tb.m_Dimension;
                m_Data->m_Kernels[m_KernelIdx].outBuffers.push_back(cbr);
            }
        }

        // Samplers
        for (auto sItr = m_Samplers.begin(); sItr != m_Samplers.end(); sItr++)
        {
            SamplerBinding &sb = sItr->second;
            bool samplerMatched = false;
            for (size_t j = 0; j < m_Data->m_Kernels[m_KernelIdx].textures.size(); ++j)
            {
                if (CheckSamplerAndTextureNameMatch(texNames[j], sb.m_Name))
                {
                    m_Data->m_Kernels[m_KernelIdx].textures[j].samplerBindPoint = sb.m_Binding.ToUint();
                    samplerMatched = true;
                }
            }
            if (!samplerMatched)
            {
                ComputeShaderBuiltinSampler cbSampler;
                core::string msg;
                cbSampler.sampler = ParseInlineSamplerName(sb.m_Name, msg);
                cbSampler.bindPoint = sb.m_Binding.ToUint();

                if (!cbSampler.sampler.IsInvalid())
                    m_Data->m_Kernels[m_KernelIdx].builtinSamplers.push_back(cbSampler);
                else
                    m_Errors.AddImportError(msg, 0, false);
            }
        }

        // Buffers
        for (auto bItr = m_Buffers.begin(); bItr != m_Buffers.end(); bItr++)
        {
            BufferBinding &bb = bItr->second;
            if (!bb.m_IsUAV)
            {
                ComputeShaderResource cbr;
                cbr.name.SetName(bb.m_Name.c_str());
                cbr.bindPoint = bb.m_Binding.ToUint();
                m_Data->m_Kernels[m_KernelIdx].inBuffers.push_back(cbr);
            }
            else
            {
                ComputeShaderResource cbr;
                cbr.name.SetName(bb.m_Name.c_str());
                cbr.bindPoint = bb.m_Binding.ToUint();
                m_Data->m_Kernels[m_KernelIdx].outBuffers.push_back(cbr);
            }
        }

        m_CBs.clear();
        m_Textures.clear();
    }

    virtual void OnInputBinding(const char* name, int bindIndex) {}

    virtual void OnConstantBuffer(const char* name, size_t bufferSize, size_t memberCount)
    {
        m_CurrentConstantBuffer = FindOrCreateConstantBuffer(name, bufferSize);
        m_MembersLeftInCurrentCB = memberCount;
    }

    virtual void OnConstant(const char* name, int bindIndex, VarType cType, int rows, int cols, bool isMatrix, int arraySize)
    {
        ShaderParamType vType = kShaderParamFloat;
        switch (cType)
        {
            case GlslangReflection::VT_FLOAT:
                vType = kShaderParamFloat;
                break;

            case GlslangReflection::VT_HALF:
                vType = kShaderParamHalf;
                break;

            case GlslangReflection::VT_INT:
                vType = kShaderParamInt;
                break;

            case GlslangReflection::VT_SHORT:
                vType = kShaderParamShort;

            default:
                AssertMsg(0, "Unknown Shader param type?");
                break;
        }

        //      m_Data->reflectionReport->OnConstant(name.c_str(), bindIndex, vType, rows, cols, isMatrix, arraySize);

        AssertMsg(m_CurrentConstantBuffer != NULL, "Global uniforms not allowed in Vulkan, should never happen");

        m_CurrentConstantBuffer->AddOrUpdateMember(name, bindIndex, vType, rows, cols, isMatrix, arraySize);
        m_MembersLeftInCurrentCB--;
        if (m_MembersLeftInCurrentCB == 0)
            m_CurrentConstantBuffer = nullptr;
    }

    virtual void OnConstantBufferBinding(const char* name, int bindIndex)
    {
        auto itr = m_CBs.find(name);
        AssertMsg(itr != m_CBs.end(), "Attempting to bind an unknown constant buffer?");

        vk::Binding newBind;
        newBind.FromUint((uint32_t)bindIndex);
        newBind.m_ShaderStages = 0;

        ConstantBuffer &cb = itr->second;
        if (cb.m_Binding.m_ShaderStages == 0)
        {
            // Copy initial binding info
            cb.m_Binding = newBind;
        }
        else
        {
            AssertMsg(cb.m_Binding.m_Binding == newBind.m_Binding, "CB bindings don't match across shader stages");
            AssertMsg(cb.m_Binding.m_Set == newBind.m_Set, "CB bindings don't match across shader stages");

            // Add the shader stage from the new bind
            cb.m_Binding.m_ShaderStages |= newBind.m_ShaderStages;
        }
    }

    virtual void OnSamplerBinding(const char* name, int bindIndex)
    {
        vk::Binding newBind;
        newBind.FromUint((uint32_t)bindIndex);
        newBind.m_ShaderStages = 0;

        auto itr = m_Samplers.find(name);
        if (itr != m_Samplers.end())
        {
            SamplerBinding &sb = itr->second;
            AssertMsg(sb.m_Binding.m_Binding == newBind.m_Binding, "Sampler bindings don't match across shader stages");
            AssertMsg(sb.m_Binding.m_Set == newBind.m_Set, "Sampler bindings don't match across shader stages");

            // Merge stage flags
            sb.m_Binding.m_ShaderStages |= newBind.m_ShaderStages;
        }
        else
        {
            SamplerBinding &sb = m_Samplers.insert(std::make_pair(std::string(name), SamplerBinding())).first->second;
            sb.m_Name = name;
            sb.m_Binding = newBind;
        }
    }

    virtual void OnTextureBinding(const char* name, int bindIndex, bool multisampled, TexDimension dim, bool isUAV)
    {
        TextureDimension tDim = kTexDim2D;
        switch (dim)
        {
            case GlslangReflection::TD_2D:
                tDim = kTexDim2D;
                break;
            case GlslangReflection::TD_2DARRAY:
                tDim = kTexDim2DArray;
                break;
            case GlslangReflection::TD_2DSHADOW:
                tDim = kTexDim2D;
                break;
            case GlslangReflection::TD_3D:
                tDim = kTexDim3D;
                break;
            case GlslangReflection::TD_CUBE:
                tDim = kTexDimCUBE;
                break;
            case GlslangReflection::TD_CUBESHADOW:
                tDim = kTexDimCUBE;
                break;
            case GlslangReflection::TD_CUBEARRAY:
                tDim = kTexDimCubeArray;
                break;
            case GlslangReflection::TD_1D:
            default:
                AssertMsg(0, "Unsupported texture dimension");
                break;
        }

        vk::Binding newBind;
        newBind.FromUint((uint32_t)bindIndex);
        newBind.m_ShaderStages = 0;
        newBind.m_IsBuffer = isUAV ? 1 : 0;

        // Shadow samplers have 'hlslcc_zcmp" prefix, get rid of it here
        core::string modName = name;
        if (modName.find("hlslcc_zcmp") == 0)
        {
            modName.erase(0, 11);
        }

        uint32_t origBindIndex = bindIndex;
        // Scan for the original bind index marker, if UAV
        if (isUAV)
        {
            const char *marker = "_origX%dX";
            size_t startLoc = modName.find("_origX");
            if ((startLoc != core::string::npos) && (sscanf(modName.c_str() + startLoc, marker, &origBindIndex) == 1))
            {
                // Get rid of all markers
                while ((startLoc = modName.find("_origX")) != core::string::npos)
                {
                    size_t endLoc = modName.find('X', startLoc + 6);
                    if (endLoc == core::string::npos)
                        break;
                    modName.erase(startLoc, endLoc - startLoc + 1);
                }
            }
        }

        auto itr = m_Textures.find(modName);
        if (itr != m_Textures.end())
        {
            TextureBinding &tb = itr->second;
            AssertMsg(tb.m_Dimension == tDim, "Texture bindings don't match");
            AssertMsg(tb.m_IsUAV == isUAV, "Texture bindings don't match");

            AssertMsg(tb.m_Binding.m_Binding == newBind.m_Binding, "CB bindings don't match across shader stages");
            AssertMsg(tb.m_Binding.m_Set == newBind.m_Set, "CB bindings don't match across shader stages");

            // Merge stage flags
            tb.m_Binding.m_ShaderStages |= newBind.m_ShaderStages;
        }
        else
        {
            TextureBinding &tb = m_Textures.insert(std::make_pair(modName, TextureBinding())).first->second;
            tb.m_Name = modName;
            tb.m_Binding = newBind;
            tb.m_Dimension = tDim;
            tb.m_IsUAV = isUAV;
            tb.m_MultiSampled = multisampled;
            tb.m_OrigBindIndex = origBindIndex;
            tb.m_SamplerBinding = -1;
        }
    }

    virtual void OnBufferBinding(const char* name, int bindIndex, bool isUAV, TexelBufferType texelBufferType)
    {
        vk::Binding newBind;
        newBind.FromUint((uint32_t)bindIndex);
        newBind.m_ShaderStages = 0;
        newBind.m_IsBuffer = isUAV ? 1 : 0;
        newBind.m_TexelBufferFormat = texelBufferType;

        uint32_t origBindIndex = bindIndex;
        // Scan for the original bind index marker, if UAV
        core::string modName = name;
        if (isUAV)
        {
            const char *marker = "_origX%dX";
            size_t startLoc = modName.find("_origX");
            // Skip counter buffers
            if ((startLoc != core::string::npos) && (modName.find("X_counterBuf") == core::string::npos))
            {
                if (sscanf(modName.c_str() + startLoc, marker, &origBindIndex) == 1)
                {
                    // Get rid of all markers
                    while ((startLoc = modName.find("_origX")) != core::string::npos)
                    {
                        size_t endLoc = modName.find('X', startLoc + 6);
                        if (endLoc == core::string::npos)
                            break;
                        modName.erase(startLoc, endLoc - startLoc + 1);
                    }
                }
            }
        }
        auto itr = m_Buffers.find(modName);
        if (itr != m_Buffers.end())
        {
            BufferBinding &bb = itr->second;
            AssertMsg(bb.m_IsUAV == isUAV, "Buffer bindings don't match");

            AssertMsg(bb.m_Binding.m_Binding == newBind.m_Binding, "Buffer bindings don't match across shader stages");
            AssertMsg(bb.m_Binding.m_Set == newBind.m_Set, "Buffer bindings don't match across shader stages");

            // Merge stage flags
            bb.m_Binding.m_ShaderStages |= newBind.m_ShaderStages;
        }
        else
        {
            BufferBinding &bb = m_Buffers.insert(std::make_pair(modName, BufferBinding())).first->second;
            bb.m_Name = modName;
            bb.m_Binding = newBind;
            bb.m_IsUAV = isUAV;
            bb.m_OrigBindIndex = origBindIndex;
        }
    }

    virtual void OnThreadGroupSize(unsigned int xSize, unsigned int ySize, unsigned int zSize)
    {
        m_Data->m_Kernels[m_KernelIdx].threadGroupSize[0] = xSize;
        m_Data->m_Kernels[m_KernelIdx].threadGroupSize[1] = ySize;
        m_Data->m_Kernels[m_KernelIdx].threadGroupSize[2] = zSize;
    }
};


ShaderCompileOutputInterface * InvokeComputeCompilerGlslang(const ShaderCompileInputInterface *input)
{
    // create as binary output data since we write data into binaryOutput
    ShaderCompileOutputData* output = MakeOutputDataGlslang(input);
    ComputeShaderBinary data;

    const GLLang targetGL = LANG_440;
    GlExtensions ext;
    ext.ARB_explicit_attrib_location = 0;
    ext.ARB_explicit_uniform_location = 0;
    ext.ARB_shading_language_420pack = 0;

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
#if DEBUG_DUMP_GLSLANG_OUTPUT_FILES
            FILE* f = fopen("debug-glsl-error-in-preprocessor.txt", "wb");
            fputs(ppdSource.c_str(), f);
            fclose(f);
#endif
            return output;
        }

        // Save kernelCBs so DX compiler won't mess with them
        ComputeShaderCBListOfLists prevCBs = data.m_KernelCBs;
        CompileShaderKernel(gPluginDispatcher->GetD3DCompiler(), &localInput, data.m_KernelCBs, data.m_Kernels, output->errors);
        data.m_KernelCBs = prevCBs;

        if (!data.m_Kernels.empty() && data.m_Kernels[0].code.size() > 0)
        {
            DebugAssert(data.m_Kernels.size() == 1);
            GLSLShader result;
            ComputeShaderKernel* kernel = &data.m_Kernels[0];
            unsigned int flags = HLSLCC_FLAG_UNIFORM_BUFFER_OBJECT | HLSLCC_FLAG_TRANSLATE_MATRICES;
            flags |= HLSLCC_FLAG_GLES31_IMAGE_QUALIFIERS | HLSLCC_FLAG_VULKAN_BINDINGS | HLSLCC_FLAG_COMBINE_TEXTURE_SAMPLERS;

            HLSLccReflection dummyRefl;
            GLSLCrossDependencyData cdData;
            int compiledOK = TranslateHLSLFromMem((const char*)kernel->code.data(), flags, targetGL, &ext, &cdData, samplers, dummyRefl, &result);
            if (!compiledOK)
            {
                printf_console("TranslateHLSLFromMem failed! Type: %i Lang: %i Source: %s", result.shaderType, result.GLSLLanguage, result.sourceCode.c_str());
            }

            // Dump built-in sampler info retrieved from hlsl reflection as it doesn't match generated texture-sampler pairs.
            // Instead generate a sampler list with an entry for each texture present (InlineSamplerType::kCount if no separate sampler for that texture).
            kernel->builtinSamplers.clear_dealloc();
            kernel->cbs.clear_dealloc();
            kernel->inBuffers.clear_dealloc();
            kernel->outBuffers.clear_dealloc();
            kernel->textures.clear_dealloc();

            std::vector<unsigned int> spirvwords;
            UnityComputeGlslangReflection crefl(&data, 0, output->errors);
            CompileGLSLToSPIRV(input, result.sourceCode, spirvwords, output->errors, crefl, true);
            crefl.Flush();

            if (output->errors.HasErrors())
            {
                return output;
            }

            size_t srcLen = spirvwords.size() * sizeof(UInt32);
            kernel->code.resize_uninitialized(srcLen);
            memcpy(kernel->code.data(), &spirvwords[0], srcLen);
        }
        data.m_ResourcesResolved = true;
    }
    else
    {
        AssertMsg(false, "Unknown compute shader language");
        output->errors.AddImportError("Unknown compute shader language", 0, false);
    }

    data.WriteToBuffer(output->binaryOutput);

    return output;
}

ShaderCompileOutputData * InvokeCompilerDisassembleSPIRV(const ShaderCompileInputInterface* input)
{
    Assert(input->api == kShaderCompPlatformVulkan);
    ShaderCompileOutputData* output = MakeOutputDataGlslang(input);

    // compute
    if (input->programMask == 0 && input->gpuProgramType == kShaderGpuProgramUnknown)
    {
        const char* disassembly = DisassembleSPIRV(static_cast<int>(input->GetCompiledDataSize()) / 4, reinterpret_cast<const unsigned int*>(input->GetCompiledDataPtr()));
        if (disassembly)
        {
            output->disassembly = disassembly;
            DestroyDisassemblyString(disassembly);
        }
        else
        {
            output->disassembly = " ";
        }
        return output;
    }

    dynamic_array<UInt8> uncompressedShaderSetBinaryBuffer(kMemTempAlloc);
    const vk::ShaderSetHeader* shaderSet = reinterpret_cast<const vk::ShaderSetHeader*>(input->GetCompiledDataPtr());
    if (!shaderSet)
    {
        output->disassembly = " ";
        return output;
    }

    const char * programNames[] = {"Vertex", "Fragment", "Hull", "Domain", "Geometry"};
    for (int i = kProgramVertex; i <= kProgramGeometry; i++)
    {
        dynamic_array<UInt8> uncompressedShaderBinaryBuffer(kMemTempAlloc);

        output->disassembly.append("Disassembly for ");
        output->disassembly.append(programNames[i]);
        output->disassembly.append(":\n");

        vk::SpirvData spirv(kMemTempAlloc);
        vk::DecompressShader(shaderSet, static_cast<VKShaderType>(i), spirv);
        if (spirv.empty())
        {
            output->disassembly.append("Not present.\n\n");
            break;
        }

        const char* disassembly = DisassembleSPIRV((int)spirv.size(), reinterpret_cast<const unsigned int*>(spirv.data()));

        if (disassembly == NULL)
        {
            output->disassembly.append("Error disassembling SPIR-V\n");
            break;
        }

        output->disassembly.append(disassembly);
        output->disassembly.append("\n");

        DestroyDisassemblyString(disassembly);
    }
    return output;
}

void InitializeCompilerGlslang(const core::string& appPath)
{
    // TODO: Do we need to make sure D3D11 compiler is initialized?
    InitializeGlslang();
}

void ShutdownCompilerGlslang()
{
    ShutdownGlslang();
}

struct GlslangPluginInterface : public PlatformPluginInterface
{
    GlslangPluginInterface() : PlatformPluginInterface()
    {
        InitializeGlslang();
    }

    virtual unsigned int GetShaderPlatforms()
    {
        return (1 << kShaderCompPlatformVulkan);
    }

    virtual const char* GetPlatformName() { return "glslang"; }

    virtual int  RunPluginUnitTests(const char* includesPath) { return 0; }

    virtual void Shutdown()
    {
        ShutdownGlslang();
        // uncomment if compiler is moved to a dll
        // RegisterRuntimeInitializeAndCleanup::ExecuteCleanup();
    }

    virtual const ShaderCompileOutputInterface* CompileShader(const ShaderCompileInputInterface *input)
    {
        return InvokeCompilerGlslang(input);
    }

    virtual const ShaderCompileOutputInterface* DisassembleShader(const ShaderCompileInputInterface *input)
    {
        return InvokeCompilerDisassembleSPIRV(input);
    }

    virtual const ShaderCompileOutputInterface* CompileComputeShader(const ShaderCompileInputInterface *input)
    {
        return InvokeComputeCompilerGlslang(input);
    }

    virtual void ReleaseShader(const ShaderCompileOutputInterface* data)
    {
        delete data;
    }
};

static GlslangPluginInterface gGlslangPlugin;

// PLUGINAPI PlatformPluginInterface * PLUGINCONVENTION  GetPluginInterface(int cgBatchVersion, PlatformPluginDispatcher* dispatcher)
PlatformPluginInterface* GetGlslangPluginInterface(int cgBatchVersion, PlatformPluginDispatcher* dispatcher)
{
    // if it's moved to a dll, uncomment this and corresponding cleanup in HLSLPluginInterface::Shutdown
    // critical for correct handling of FastPropertyNames in compute shaders from shader compilers in DLLs
    // RegisterRuntimeInitializeAndCleanup::ExecuteInitializations();

    const int cgBatchVersion_min = 0x0002;
    const int cgBatchVersion_max = 0x0002;

    // user version to plugin to be rejected by cgbatch or reject itself
    if (cgBatchVersion > cgBatchVersion_max || cgBatchVersion < cgBatchVersion_min)
        return NULL;

    return &gGlslangPlugin;
}
