#include "UnityPrefix.h"
#include "Tools/UnityShaderCompiler/ShaderCompiler.h"
#include "Tools/UnityShaderCompiler/PlatformPlugin.h" // for HLSLIncludeHandler

#include "Runtime/Utilities/PathNameUtility.h"
#include "Runtime/Utilities/Word.h"
#include "Runtime/Utilities/ArrayUtility.h"
#include "Runtime/Utilities/StaticAssert.h"
#include "Editor/Src/Utility/d3d11/D3D11ReflectionAPI.h"
#include "Editor/Src/Utility/TextUtilities.h"
#include "Runtime/Shaders/ComputeShader.h"
#include "Runtime/GfxDevice/directx/D3DRootSignature.h"
#include "Runtime/GfxDevice/InstancingUtilities.h"
#include "Tools/UnityShaderCompiler/ComputeShaderCompiler.h"
#include "Tools/UnityShaderCompiler/APIPlugin.h"
#include "Tools/UnityShaderCompiler/Utilities/ProcessIncludes.h"
#include "Tools/UnityShaderCompiler/Utilities/D3DCompiler.h"


static const char* kHLSLProgramTargets40[] = {"vs_4_0", "ps_4_0", NULL, NULL, "gs_4_0", NULL};
ShaderGpuProgramType kHLSLProgramTypes40[] = {kShaderGpuProgramDX11VertexSM40, kShaderGpuProgramDX11PixelSM40, kShaderGpuProgramUnknown, kShaderGpuProgramUnknown, kShaderGpuProgramDX11GeometrySM40, kShaderGpuProgramUnknown};
CompileTimeAssertArraySize(kHLSLProgramTargets40, kProgramCount);
CompileTimeAssertArraySize(kHLSLProgramTypes40, kProgramCount);

static const char* kHLSLProgramTargets50[] = {"vs_5_0", "ps_5_0", "hs_5_0", "ds_5_0", "gs_5_0", NULL};
ShaderGpuProgramType kHLSLProgramTypes50[] = {kShaderGpuProgramDX11VertexSM50, kShaderGpuProgramDX11PixelSM50, kShaderGpuProgramDX11HullSM50, kShaderGpuProgramDX11DomainSM50, kShaderGpuProgramDX11GeometrySM50, kShaderGpuProgramUnknown};
CompileTimeAssertArraySize(kHLSLProgramTargets50, kProgramCount);
CompileTimeAssertArraySize(kHLSLProgramTypes50, kProgramCount);


static void GetCompileParametersFromInput(const ShaderCompileInputInterface* input, core::string &targetName, ShaderRequirements& outTargetFeatures, ShaderGpuProgramType &gpuProgramType, DWORD &flags)
{
    const ShaderRequirements requirements = input->requirements;
    const ShaderCompilerProgram programType = input->programType;

    if (input->api == kShaderCompPlatformGLES20)
    {
        // Extra optimization for low-end mobile
        flags |= D3D10_SHADER_OPTIMIZATION_LEVEL3;
    }

    // Regular DX11 API case: pick between SM4.0 & SM5.0 based on requirements
    ShaderRequirements requiredAboveSM40 = requirements & ~kShaderRequireShaderModel40_PC;
    if (requiredAboveSM40 != 0)
    {
        // anything required above SM4.0 --> use SM5.0
        targetName = kHLSLProgramTargets50[programType];
        gpuProgramType = kHLSLProgramTypes50[programType];
        outTargetFeatures = kShaderRequireShaderModel50_PC;
    }
    else
    {
        targetName = kHLSLProgramTargets40[programType];
        gpuProgramType = kHLSLProgramTypes40[programType];
        outTargetFeatures = kShaderRequireShaderModel40_PC;
    }
}

static ShaderCompileOutputData* MakeOutputDataHLSL(const ShaderCompileInputInterface* input)
{
    return new ShaderCompileOutputData(input->api, ShaderCompileOutputData::kBinaryOutputData);
}

struct TextureSamplerBind
{
    int         textureBind;
    core::string    textureName;
    D3D10_SRV_DIMENSION dimension;
    int         samplerBind;
    bool        multisampled;
};

static bool ReportConstantVarOfBasicTypes(ShaderCompilerReflectionReport* reflectionReport, const D3D11_SHADER_TYPE_DESC& tdesc, const char* name, int offset, int size, int& inoutMaxSizeUsed)
{
    if (tdesc.Class == D3D10_SVC_MATRIX_COLUMNS && tdesc.Type == D3D10_SVT_FLOAT && tdesc.Columns == 4)
    {
        // 4-column column-major matrix and array (float1x4, float2x4, float3x4, float4x4) that all pad to 16 elements.
        reflectionReport->OnConstant(name, offset, kShaderParamFloat, kConstantTypeMatrix, tdesc.Rows, tdesc.Columns, tdesc.Elements);
        inoutMaxSizeUsed = std::max(inoutMaxSizeUsed, offset + size);
        return true;
    }
    else if (tdesc.Class == D3D10_SVC_VECTOR && (tdesc.Type == D3D10_SVT_FLOAT || tdesc.Type == D3D10_SVT_INT || tdesc.Type == D3D10_SVT_UINT || tdesc.Type == D3D10_SVT_BOOL) && tdesc.Rows == 1)
    {
        // float/int/uint/bool vector and array (float1, float2, float3, float4) that all pad to 4 elements.
        ShaderParamType propType = kShaderParamTypeCount;
        if (tdesc.Type == D3D10_SVT_FLOAT)
            propType = kShaderParamFloat;
        else if (tdesc.Type == D3D10_SVT_BOOL)
            propType = kShaderParamBool;
        else
            propType = kShaderParamInt;
        reflectionReport->OnConstant(name, offset, propType, kConstantTypeDefault, 1, tdesc.Columns, tdesc.Elements);
        inoutMaxSizeUsed = std::max(inoutMaxSizeUsed, offset + size);
        return true;
    }
    else if (tdesc.Class == D3D10_SVC_SCALAR && (tdesc.Type == D3D10_SVT_FLOAT || tdesc.Type == D3D10_SVT_INT || tdesc.Type == D3D10_SVT_UINT || tdesc.Type == D3D10_SVT_BOOL) && tdesc.Columns == 1 && tdesc.Rows == 1)
    {
        // float/int/uint/bool scalar and array.
        ShaderParamType propType = kShaderParamTypeCount;
        if (tdesc.Type == D3D10_SVT_FLOAT)
            propType = kShaderParamFloat;
        else if (tdesc.Type == D3D10_SVT_BOOL)
            propType = kShaderParamBool;
        else
            propType = kShaderParamInt;
        reflectionReport->OnConstant(name, offset, propType, kConstantTypeDefault, 1, 1, tdesc.Elements);
        inoutMaxSizeUsed = std::max(inoutMaxSizeUsed, offset + size);
        return true;
    }

    return false;
}

static void FindParametersHLSL11(const void* shaderCode, size_t shaderSize, const core::string& programName, ShaderCompilerProgram programType, ShaderCompilerReflectionReport* reflectionReport, int startLine, ShaderImportErrors& outErrors, std::vector<D3D11_SHADER_INPUT_BIND_DESC>& outBindDescs, D3D11_PRIMITIVE& outInputPrimitive)
{
    D3D11ShaderReflection* reflector = NULL;
    HRESULT hr = gPluginDispatcher->GetD3DCompiler()->D3DReflect(shaderCode, shaderSize, (void**)&reflector);
    if (FAILED_IMPL(hr))
    {
        outErrors.AddImportError("Failed to get HLSL Reflection interface", startLine, false, kShaderCompPlatformD3D11);
        return;
    }

    D3D11_SHADER_DESC shaderDesc;
    reflector->GetDesc(&shaderDesc);

    // store geometry shader input type
    outInputPrimitive = (programType == kProgramGeometry) ? shaderDesc.InputPrimitive : D3D11_PRIMITIVE_UNDEFINED;

    // input parameters for a vertex shader
    if (programType == kProgramVertex)
    {
        for (UINT i = 0; i < shaderDesc.InputParameters; ++i)
        {
            D3D11_SIGNATURE_PARAMETER_DESC desc;
            reflector->GetInputParameterDesc(i, &desc);
            bool used = (desc.ReadWriteMask != 0);

            if (desc.SemanticIndex == 0 && !strcasecmp(desc.SemanticName, "POSITION"))
            {
                reflectionReport->OnInputBinding(kShaderChannelVertex, used ? kVertexCompVertex : kVertexCompNone);
            }
            else if (desc.SemanticIndex == 0 && !strcasecmp(desc.SemanticName, "NORMAL"))
            {
                reflectionReport->OnInputBinding(kShaderChannelNormal, used ? kVertexCompNormal : kVertexCompNone);
            }
            else if (desc.SemanticIndex == 0 && !strcasecmp(desc.SemanticName, "TANGENT"))
            {
                reflectionReport->OnInputBinding(kShaderChannelTangent, used ? kVertexCompTangent : kVertexCompNone);
            }
            else if (desc.SemanticIndex == 0 && !strcasecmp(desc.SemanticName, "COLOR"))
            {
                reflectionReport->OnInputBinding(kShaderChannelColor, used ? kVertexCompColor : kVertexCompNone);
            }
            else if (desc.SemanticIndex < kMaxTexCoordShaderChannels  && !strcasecmp(desc.SemanticName, "TEXCOORD"))
            {
                reflectionReport->OnInputBinding(static_cast<ShaderChannel>(kShaderChannelTexCoord0 + desc.SemanticIndex), used ? static_cast<VertexComponent>(kVertexCompTexCoord0 + desc.SemanticIndex) : kVertexCompNone);
            }
            else if (!strncmp(desc.SemanticName, "SV_", 3))
            {
                // system value input semantic, ignore (except SV_POSITION; warn about that
                // so that people don't accidentally use it in their shaders)
                if (!strcasecmp(desc.SemanticName, "SV_POSITION"))
                {
                    core::string msg = Format("Vertex program '%s': input semantic SV_POSITION is used; you probably want POSITION instead", programName.c_str());
                    outErrors.AddImportError(msg, startLine, true, kShaderCompPlatformD3D11);
                }
            }
            else if (desc.SemanticIndex == 1 && !strcasecmp(desc.SemanticName, "POSITION"))
            {
                // permit POSITION1 for motion blur on skinned objects; automatically bound by VBO
            }
            else
            {
                core::string msg = Format("%s program '%s': unknown input semantics %s/%i\n",
                    kProgramTypeNames[programType], programName.c_str(), desc.SemanticName, desc.SemanticIndex);

                outErrors.AddImportError(msg, startLine, false, kShaderCompPlatformD3D11);
            }
        }
    }

    // constant buffers
    for (UINT i = 0; i < shaderDesc.ConstantBuffers; ++i)
    {
        D3D11ShaderReflectionConstantBuffer* cb = reflector->GetConstantBufferByIndex(i);
        D3D11_SHADER_BUFFER_DESC desc;
        cb->GetDesc(&desc);
        if (desc.Type == D3D11_CT_CBUFFER)
        {
            reflectionReport->OnConstantBuffer(desc.Name, desc.Size, desc.Variables);
            int maxSizeUsed = 0; // tracked only for debugging now
            for (UINT j = 0; j < desc.Variables; ++j)
            {
                D3D11ShaderReflectionVariable* var = cb->GetVariableByIndex(j);
                D3D11_SHADER_VARIABLE_DESC vdesc;
                var->GetDesc(&vdesc);
                if (!(vdesc.uFlags & D3D10_SVF_USED))
                    continue;
                D3D11_SHADER_TYPE_DESC tdesc;
                var->GetType()->GetDesc(&tdesc);

                if (tdesc.Class == D3D10_SVC_STRUCT)
                {
                    if (!IsInstancingConstantBufferName(desc.Name))
                    {
                        core::string msg = Format("%s program '%s': Struct variable '%s' is ignored. Only instancing constant buffers can have struct variables\n",
                            kProgramTypeNames[programType], programName.c_str(), vdesc.Name);
                        outErrors.AddImportError(msg, startLine, false, kShaderCompPlatformD3D11);
                        continue;
                    }

                    // report the struct variable itself, with the arraySize (e.g. for AOS instancing buffer)
                    UINT stride = vdesc.Size;
                    if (tdesc.Elements > 1)
                        stride = ((vdesc.Size + 15) & ~15) / tdesc.Elements; // round the size up to 16-byte boundary
                    reflectionReport->OnConstant(vdesc.Name, vdesc.StartOffset, kShaderParamFloat, kConstantTypeStruct, stride, 1, tdesc.Elements);
                    // current we don't support nested struct (top-level only).
                    for (UINT i = 0; i < tdesc.Members; ++i)
                    {
                        D3D11_SHADER_TYPE_DESC memberTDesc;
                        var->GetType()->GetMemberTypeByIndex(i)->GetDesc(&memberTDesc);
                        core::string varName = Format("%s.%s", vdesc.Name, var->GetType()->GetMemberTypeName(i));
                        if (!ReportConstantVarOfBasicTypes(reflectionReport, memberTDesc, varName.c_str(), memberTDesc.Offset, 0, maxSizeUsed))
                        {
                            core::string msg = Format("%s program '%s': Ignore instancing property '%s' because the data type is not supported.\n",
                                kProgramTypeNames[programType], programName.c_str(), varName.c_str());
                            outErrors.AddImportError(msg, startLine, true, kShaderCompPlatformD3D11);
                        }
                    }
                }
                else if (!ReportConstantVarOfBasicTypes(reflectionReport, tdesc, vdesc.Name, vdesc.StartOffset, vdesc.Size, maxSizeUsed))
                {
                    //cbLocals += Format("// unknown %s Class=%i Type=%i Cols=%i Rows=%i Size=%i\n", name, tdesc.Class, tdesc.Type, tdesc.Columns, tdesc.Rows, size);
                }
            }
        }
        else if (desc.Type == D3D11_CT_RESOURCE_BIND_INFO)
        {
            //@TODO: no idea what to do with them; things like StructuredBuffer<foo> produce it.
        }
        else
        {
            core::string msg = Format("%s program '%s': only scalar constant buffers supported right now\n",
                kProgramTypeNames[programType], programName.c_str());
            outErrors.AddImportError(msg, startLine, false, kShaderCompPlatformD3D11);
        }
    }
    // resources
    std::vector<TextureSamplerBind> textureSamplers;

    // Go over textures used & record their info
    for (UINT i = 0; i < shaderDesc.BoundResources; ++i)
    {
        D3D11_SHADER_INPUT_BIND_DESC desc;
        reflector->GetResourceBindingDesc(i, &desc);
        if (desc.Type == D3D10_SIT_TEXTURE || desc.Type == D3D11_SIT_STRUCTURED || desc.Type == D3D11_SIT_BYTEADDRESS)
        {
            TextureSamplerBind tex;
            tex.textureBind = desc.BindPoint;
            tex.textureName = desc.Name;
            tex.dimension = desc.Dimension;
            tex.samplerBind = -1;
            tex.multisampled = (desc.NumSamples != 0xffffffff);
            textureSamplers.push_back(tex);
        }
    }

    // Go over samplers & find which textures they correspond to
    for (UINT i = 0; i < shaderDesc.BoundResources; ++i)
    {
        D3D11_SHADER_INPUT_BIND_DESC desc;
        reflector->GetResourceBindingDesc(i, &desc);
        if (desc.Type == D3D10_SIT_SAMPLER)
        {
            bool samplerMatched = false;
            for (size_t j = 0; j < textureSamplers.size(); ++j)
            {
                if (CheckSamplerAndTextureNameMatch(textureSamplers[j].textureName.c_str(), desc.Name))
                {
                    textureSamplers[j].samplerBind = desc.BindPoint;
                    samplerMatched = true;
                }
            }

            if (!samplerMatched)
            {
                core::string samplerMsg;
                InlineSamplerType inlineSampler = ParseInlineSamplerName(desc.Name, samplerMsg);
                if (!inlineSampler.IsInvalid())
                {
                    reflectionReport->OnInlineSampler(inlineSampler, desc.BindPoint);
                }
                else
                {
                    core::string msg = Format("%s program '%s': %s.\n",
                        kProgramTypeNames[programType], programName.c_str(), samplerMsg.c_str());
                    outErrors.AddImportError(msg, startLine, false, kShaderCompPlatformD3D11);
                }
            }
        }
    }

    // Go over other resources & emit their info
    outBindDescs.resize(shaderDesc.BoundResources);
    for (UINT i = 0; i < shaderDesc.BoundResources; ++i)
    {
        D3D11_SHADER_INPUT_BIND_DESC desc;
        reflector->GetResourceBindingDesc(i, &desc);

        outBindDescs[i] = desc; // Store for output

        switch (desc.Type)
        {
            case D3D10_SIT_CBUFFER:
                reflectionReport->OnCBBinding(desc.Name, desc.BindPoint);
                break;
            case D3D10_SIT_SAMPLER:
            case D3D10_SIT_TEXTURE:
            case D3D11_SIT_STRUCTURED:
            case D3D11_SIT_BYTEADDRESS:
                break;

            case D3D11_SIT_UAV_RWTYPED:
            case D3D11_SIT_UAV_RWSTRUCTURED:
            case D3D11_SIT_UAV_RWBYTEADDRESS:
            case D3D11_SIT_UAV_APPEND_STRUCTURED:
            case D3D11_SIT_UAV_CONSUME_STRUCTURED:
            case D3D11_SIT_UAV_RWSTRUCTURED_WITH_COUNTER:
                // UAVs in a shader; they are more like render targets. Just move on;
                // the code will have to deal with their indices manually.
                break;

            default:
                core::string msg = Format("%s program '%s': unrecognized resource binding %i\n",
                    kProgramTypeNames[programType], programName.c_str(), desc.Type);
                outErrors.AddImportError(msg, startLine, false, kShaderCompPlatformD3D11);
        }
    }

    // Go over textures & samplers and emit their info
    for (size_t j = 0; j < textureSamplers.size(); ++j)
    {
        TextureSamplerBind& tex = textureSamplers[j];
        if (tex.dimension == D3D10_SRV_DIMENSION_BUFFER)
        {
            reflectionReport->OnBufferBinding(tex.textureName.c_str(), tex.textureBind);
            continue;
        }
        TextureDimension dim = kTexDimUnknown;
        switch (tex.dimension)
        {
            case D3D10_SRV_DIMENSION_TEXTURE2D: dim = kTexDim2D; break;
            case D3D10_SRV_DIMENSION_TEXTURE2DMS: dim = kTexDim2D; break;
            case D3D10_SRV_DIMENSION_TEXTURE3D: dim = kTexDim3D; break;
            case D3D10_SRV_DIMENSION_TEXTURECUBE: dim = kTexDimCUBE; break;
            case D3D10_SRV_DIMENSION_TEXTURE2DARRAY: dim = kTexDim2DArray; break;
            case D3D10_SRV_DIMENSION_TEXTURE2DMSARRAY: dim = kTexDim2DArray; break;
            case D3D10_SRV_DIMENSION_TEXTURECUBEARRAY: dim = kTexDimCubeArray; break;
            default:
                // unknown sampler type: error
                core::string msg = Format("%s program '%s': unrecognized sampler dimension (%i) in %s",
                    kProgramTypeNames[programType], programName.c_str(), tex.dimension, tex.textureName.c_str());
                outErrors.AddImportError(msg, startLine, false, kShaderCompPlatformD3D11);
                break;
        }
        if (dim != kTexDimUnknown)
        {
            reflectionReport->OnTextureBinding(tex.textureName.c_str(), tex.textureBind, tex.samplerBind, tex.multisampled, dim);
        }
    }

    // stats
    reflectionReport->OnStatsInfo(
        shaderDesc.FloatInstructionCount + shaderDesc.IntInstructionCount + shaderDesc.UintInstructionCount,
        shaderDesc.TextureNormalInstructions,
        shaderDesc.StaticFlowControlCount + shaderDesc.DynamicFlowControlCount - 1, // all shaders seem to have "RET", don't count that
        shaderDesc.TempRegisterCount);

    reflector->Release();
}

static void FindBindingCountHLSL11(const std::vector<D3D11_SHADER_INPUT_BIND_DESC>& bindDescs, BYTE& srv, BYTE& cbv, BYTE& smp, BYTE& uav)
{
    for (size_t i = 0; i < bindDescs.size(); ++i)
    {
        const D3D11_SHADER_INPUT_BIND_DESC& desc = bindDescs[i];

        switch (desc.Type)
        {
            case D3D10_SIT_CBUFFER:
                cbv = std::max<BYTE>(cbv, desc.BindPoint + 1);
                break;
            case D3D10_SIT_SAMPLER:
                smp = std::max<BYTE>(smp, desc.BindPoint + 1);
                break;
            case D3D10_SIT_TEXTURE:
            case D3D11_SIT_STRUCTURED:
            case D3D11_SIT_BYTEADDRESS:
                srv = std::max<BYTE>(srv, desc.BindPoint + 1);
                break;
            case D3D11_SIT_UAV_RWTYPED:
            case D3D11_SIT_UAV_RWSTRUCTURED:
            case D3D11_SIT_UAV_RWBYTEADDRESS:
            case D3D11_SIT_UAV_APPEND_STRUCTURED:
            case D3D11_SIT_UAV_CONSUME_STRUCTURED:
            case D3D11_SIT_UAV_RWSTRUCTURED_WITH_COUNTER:
                uav = std::max<BYTE>(uav, desc.BindPoint + 1);
                break;
            case D3D10_SIT_TBUFFER:
                // not supported yet
                break;
        }
    }
}

ShaderCompileOutputData* InvokeCompilerHLSL(const ShaderCompileInputInterface* input)
{
    ShaderCompileOutputData* output = MakeOutputDataHLSL(input);

    const bool oldUseRealDll = gPluginDispatcher->GetD3DCompiler()->GetUseRealDLL();
    Assert(input->api == kShaderCompPlatformD3D11 || input->api == kShaderCompPlatformGLES20 || input->api == kShaderCompPlatformGLES3Plus || input->api == kShaderCompPlatformOpenGLCore || input->api == kShaderCompPlatformMetal || input->api == kShaderCompPlatformVulkan);

    if (!gPluginDispatcher->GetD3DCompiler()->IsValid())
    {
        output->errors.AddImportError("D3D shader compiler could not be loaded", input->startLine, false, input->api);
        return output;
    }

    size_t numArgs = input->GetNumShaderArgs();

    HRESULT hr;
    D3D10Blob *shader = NULL;
    D3D10Blob *errors = NULL;
    D3D10Blob* strippedShader = NULL;

    const bool isFromHLSLcc = input->api == kShaderCompPlatformGLES20 || input->api == kShaderCompPlatformGLES3Plus || input->api == kShaderCompPlatformOpenGLCore || input->api == kShaderCompPlatformMetal || input->api == kShaderCompPlatformVulkan;
    const bool enableD3D11Debug = HasFlag(input->compileFlags, kShaderCompFlags_EnableDebugInfoD3D11);
    //bool isFromHLSLcc = !findParams;

    HLSLIncludeHandler includes(input);

    core::string source(input->GetSource(), input->GetSourceLength());
    Assert(input->programType != kProgramSurface);

    DWORD flags = D3D10_SHADER_ENABLE_BACKWARDS_COMPATIBILITY;

    // Figure out flags & target model to compile to
    core::string targetName;
    ShaderRequirements availableFeatures = kShaderRequireNothing;
    GetCompileParametersFromInput(input, targetName, availableFeatures, output->gpuProgramType, flags);
    ShaderArgs availableFeatureArgs;
    if (!HasFlag(input->compileFlags, kShaderCompFlags_DoNotAddShaderAvailableMacros))
        AddAvailableShaderTargetRequirementsMacros(input->requirements | availableFeatures, availableFeatureArgs);

    // Convert input arguments to HLSL compiler macro definitions
    D3D10_SHADER_MACRO* macros = new D3D10_SHADER_MACRO[numArgs + availableFeatureArgs.size() + 2];
    size_t argIndex = 0;
    for (size_t i = 0; i < numArgs; ++i, ++argIndex)
    {
        macros[argIndex].Name = input->GetShaderArgName(i);
        macros[argIndex].Definition = input->GetShaderArgValue(i);
    }
    for (size_t i = 0; i < availableFeatureArgs.size(); ++i, ++argIndex)
    {
        macros[argIndex].Name = availableFeatureArgs[i].name.c_str();
        macros[argIndex].Definition = availableFeatureArgs[i].value.c_str();
    }
    macros[argIndex].Name = GetCompilerPlatformDefine(input->api);  macros[argIndex].Definition = "1"; argIndex++;
    macros[argIndex].Name = NULL; macros[argIndex].Definition = NULL;


    //  if (isFromHLSLcc) // TODO Evaluate whether this is a good idea
    //      flags |= D3D10_SHADER_OPTIMIZATION_LEVEL3 | D3D10_SHADER_PREFER_FLOW_CONTROL;

    const char* dummyFileName = "";
    if (enableD3D11Debug)
    {
        // This only works on windows, due to needing actual DLL
#       if PLATFORM_WIN
        dummyFileName = "temp.hlsl",  // !! Visual Studio 2013 Graphics Analyzer requires a valid file name when showing shader source. If you leave an empty file name, it will show only assembly
        // Crashes on Windows if we don't use real Dll, Aras said that probably some function doesn't get loaded
        gPluginDispatcher->GetD3DCompiler()->SetUseRealDLL(true);
        flags |= D3D10_SHADER_DEBUG;
        // Comment #line keywords, because it messes up D3D11 shader debugger
        replace_string(source, "#line", "//#line");
#       endif // if PLATFORM_WIN
    }

    hr = gPluginDispatcher->GetD3DCompiler()->D3DCompile(
        source.c_str(),
        (unsigned long)source.size(),
        dummyFileName,
        macros,
        &includes,
        input->GetEntryName(),
        targetName.c_str(),
        flags,
        0,
        &shader,
        &errors);
    delete[] macros;

    const BYTE* shaderCode;
    DWORD shaderSize;
    std::vector<D3D11_SHADER_INPUT_BIND_DESC> bindDescs;
    D3D11_PRIMITIVE gsInputPrimitiveType = D3D11_PRIMITIVE_UNDEFINED;

    // Check errors
    if (FAILED_IMPL(hr) || !shader)
    {
        bool reportUnknownErrors = false;
        core::string errorMsg;
        if (errors)
        {
            core::string msg(reinterpret_cast<const char*>(errors->GetBufferPointer()), errors->GetBufferSize());
            // printf ("%s", msg.c_str());
            // print errors and warnings from listing
            ParseErrorMessages(msg, false, "): fatal error ", input->GetEntryName(), input->startLine, input->api, output->errors);
            ParseErrorMessages(msg, false, "): error ", input->GetEntryName(), input->startLine, input->api, output->errors);
            ParseErrorMessages(msg, false, "): error: ", input->GetEntryName(), input->startLine, input->api, output->errors);
            ParseErrorMessages(msg, true, "): warning ", input->GetEntryName(), input->startLine, input->api, output->errors);
            ParseErrorMessagesStartLine(msg, "error ", input->GetEntryName(), input->startLine, input->api, output->errors);
            errorMsg = msg;

            ParseErrorMessagesStartLine(msg, "internal error:", input->GetEntryName(), input->startLine, input->api, output->errors);

            includes.UpdateErrorsWithFullPathnames(output->errors);
        }
        else
            reportUnknownErrors = true;

        if (!output->errors.HasErrorsOrWarnings())
            reportUnknownErrors = true;

        if (reportUnknownErrors)
        {
            output->errors.AddImportError(Format("Compilation failed (other error) '%s'", errorMsg.c_str()).c_str(), input->startLine, false, input->api);
        }
        goto _cleanup;
    }

    // Check warnings (report them back if compilation was successful but emitted warnings)
    if (errors)
    {
        core::string msg(reinterpret_cast<const char*>(errors->GetBufferPointer()), errors->GetBufferSize());
        ParseErrorMessages(msg, true, "): warning ", input->GetEntryName(), input->startLine, input->api, output->errors);
    }

    // Get compiled shader
    Assert(shader);
    shaderCode = reinterpret_cast<const BYTE*>(shader->GetBufferPointer());
    shaderSize = shader->GetBufferSize();

    if (!isFromHLSLcc)
    {
        // Figure out symbols
        FindParametersHLSL11(shaderCode, shaderSize, input->GetEntryName(), input->programType, input->reflectionReport, input->startLine, output->errors, bindDescs, gsInputPrimitiveType);
        if (output->errors.HasErrors())
        {
            goto _cleanup;
        }
    }

    // Strip reflect/debug information (unless we're emitting debug info)
    if (!enableD3D11Debug)
    {
        UINT flags = D3DCOMPILER_STRIP_DEBUG_INFO | D3DCOMPILER_STRIP_TEST_BLOBS;
        if (!isFromHLSLcc)
            flags |= D3DCOMPILER_STRIP_REFLECTION_DATA;

        hr = gPluginDispatcher->GetD3DCompiler()->D3DStripShader(shaderCode, shaderSize, flags, &strippedShader);
        if (SUCCEEDED_IMPL(hr))
        {
            shaderCode = reinterpret_cast<const BYTE*>(strippedShader->GetBufferPointer());
            shaderSize = strippedShader->GetBufferSize();
        }
    }

    // Encode shader output
    if (isFromHLSLcc)
    {
        const BYTE rootSigPresent = 0;

        output->binaryOutput.resize_uninitialized(shaderSize + 1);
        output->binaryOutput[0] = rootSigPresent;
        memcpy(&output->binaryOutput[1], shaderCode, shaderSize);
    }
    else
    {
        const BYTE rootSigPresent = 1;
        output->binaryOutput.push_back(rootSigPresent);

        BYTE srv = 0, cbv = 0, smp = 0, uav = 0;
        FindBindingCountHLSL11(bindDescs, srv, cbv, smp, uav);
        output->binaryOutput.push_back(srv);
        output->binaryOutput.push_back(cbv);
        output->binaryOutput.push_back(smp);
        output->binaryOutput.push_back(uav);
        output->binaryOutput.push_back((BYTE)gsInputPrimitiveType);

        // if changing the above output, make sure to update kD3DRootSignatureHeaderSize in
        // D3DRootSignature.h to match the size.

        const size_t offset = output->binaryOutput.size();
        output->binaryOutput.resize_uninitialized(offset + shaderSize);
        memcpy(&output->binaryOutput[offset], shaderCode, shaderSize);
    }

_cleanup:
    gPluginDispatcher->GetD3DCompiler()->SetUseRealDLL(oldUseRealDll);
    if (strippedShader)
        strippedShader->Release();
    if (shader)
        shader->Release();
    if (errors)
        errors->Release();

    return output;
}

ShaderCompileOutputData* InvokeDisassemblerHLSL(const ShaderCompileInputInterface* input)
{
    ShaderCompileOutputData* output = MakeOutputDataHLSL(input);

    Assert(input->api == kShaderCompPlatformD3D11 || input->api == kShaderCompPlatformGLES20 || input->api == kShaderCompPlatformGLES3Plus || input->api == kShaderCompPlatformOpenGLCore || input->api == kShaderCompPlatformMetal || input->api == kShaderCompPlatformVulkan);
    if (!gPluginDispatcher->GetD3DCompiler()->IsValid())
    {
        output->errors.AddImportError("D3D11 shader compiler could not be loaded", input->startLine, false, input->api);
        return output;
    }

    const bool oldUseRealDll = gPluginDispatcher->GetD3DCompiler()->GetUseRealDLL();
#if PLATFORM_WIN
    gPluginDispatcher->GetD3DCompiler()->SetUseRealDLL(true);
#else
    gPluginDispatcher->GetD3DCompiler()->SetUseRealDLL(false);
#endif

    HRESULT hr;
    D3D10Blob *asmBuffer = NULL;

    const UINT D3D_DISASM_ENABLE_INSTRUCTION_NUMBERING = 0x00000004;

    const UInt8* shaderDataStart = input->GetCompiledDataPtr();
    const UInt8* shaderDataEnd = shaderDataStart + input->GetCompiledDataSize();

    if (shaderDataStart == 0 || shaderDataEnd == shaderDataStart)
    {
        output->disassembly = "// Invalid bytecode blob found";
        return output;
    }

    // Skip past root signature (first byte indicates present,
    // if present several more bytes to skip for the signature).
    // Root signature data might not exist for compute shaders.
    if (shaderDataStart[0] == 1)
        shaderDataStart += 1 + kD3DRootSignatureHeaderSize;
    else if (shaderDataStart[0] == 0)
        shaderDataStart += 1;

    if (shaderDataStart >= shaderDataEnd)
    {
        output->disassembly = "// Invalid bytecode blob found";
        return output;
    }

    hr = gPluginDispatcher->GetD3DCompiler()->D3DDisassemble(
        shaderDataStart,
        shaderDataEnd - shaderDataStart,
        D3D_DISASM_ENABLE_INSTRUCTION_NUMBERING,
        NULL,
        &asmBuffer);

    if (SUCCEEDED_IMPL(hr) && asmBuffer)
        output->disassembly = (char*)asmBuffer->GetBufferPointer();
    else
        output->disassembly = "// Failed to disassemble shader";

    if (asmBuffer)
        asmBuffer->Release();

    gPluginDispatcher->GetD3DCompiler()->SetUseRealDLL(oldUseRealDll);

    return output;
}

ShaderCompileOutputData* InvokeComputeCompilerHLSL(const ShaderCompileInputInterface* input)
{
    ShaderCompileOutputData* output = MakeOutputDataHLSL(input);

    ComputeShaderBinary data;
    dynamic_array<UInt8> outputBuffer;

    CompileShaderKernel(gPluginDispatcher->GetD3DCompiler(),
        input,
        data.m_KernelCBs,
        data.m_Kernels,
        output->errors);

    data.WriteToBuffer(output->binaryOutput);
    return output;
}

struct HLSLPluginInterface : public PlatformPluginInterface
{
    virtual unsigned int GetShaderPlatforms() { return (1 << kShaderCompPlatformD3D11); }
    virtual const char* GetPlatformName() { return "hlsl"; }

    virtual int  RunPluginUnitTests(const char* includesPath) { return 0; }

    virtual void Shutdown()
    {
        // uncomment if compiler is moved to a dll
        // RegisterRuntimeInitializeAndCleanup::ExecuteCleanup();
    }

    virtual const ShaderCompileOutputInterface* CompileShader(const ShaderCompileInputInterface *input)
    {
        return InvokeCompilerHLSL(input);
    }

    virtual const ShaderCompileOutputInterface* DisassembleShader(const ShaderCompileInputInterface *input)
    {
        return InvokeDisassemblerHLSL(input);
    }

    virtual const ShaderCompileOutputInterface* CompileComputeShader(const ShaderCompileInputInterface *input)
    {
        return InvokeComputeCompilerHLSL(input);
    }

    virtual void ReleaseShader(const ShaderCompileOutputInterface* data)
    {
        delete data;
    }
};

static HLSLPluginInterface gHLSLPlugin;

// PLUGINAPI PlatformPluginInterface * PLUGINCONVENTION  GetPluginInterface(int cgBatchVersion, PlatformPluginDispatcher* dispatcher)
PlatformPluginInterface* GetHLSLPluginInterface(int cgBatchVersion, PlatformPluginDispatcher* dispatcher)
{
    // if it's moved to a dll, uncomment this and corresponding cleanup in HLSLPluginInterface::Shutdown
    // critical for correct handling of FastPropertyNames in compute shaders from shader compilers in DLLs
    // RegisterRuntimeInitializeAndCleanup::ExecuteInitializations();

    const int cgBatchVersion_min = 0x0002;
    const int cgBatchVersion_max = 0x0002;

    // user version to plugin to be rejected by cgbatch or reject itself
    if (cgBatchVersion > cgBatchVersion_max || cgBatchVersion < cgBatchVersion_min)
        return NULL;

    return &gHLSLPlugin;
}
