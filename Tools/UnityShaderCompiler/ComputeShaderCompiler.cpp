#include "UnityPrefix.h"

#include "artifacts/generated/Configuration/UnityConfigureVersion.gen.h"
#include "Editor/Src/Utility/TextUtilities.h"
#include "Editor/Src/Utility/d3d11/D3D11ReflectionAPI.h"
#include "Runtime/Shaders/ShaderImpl/ShaderErrors.h"
#include "Runtime/Utilities/File.h"
#include "Runtime/Utilities/FileUtilities.h"
#include "Runtime/Utilities/Utility.h"

#include "Tools/UnityShaderCompiler/ComputeShaderCompiler.h"
#include "Tools/UnityShaderCompiler/PlatformPlugin.h" // for HLSLIncludeHandler
#include "Tools/UnityShaderCompiler/Utilities/D3DCompiler.h"
#include "Tools/UnityShaderCompiler/Utilities/ProcessIncludes.h"
#include "Tools/UnityShaderCompiler/Utilities/ShaderImportUtils.h"
#include "Tools/UnityShaderCompiler/Utilities/ShaderImportErrors.h"

#if PLATFORM_WIN
#include <winerror.h>
#endif


void ReflectComputeShader(D3DCompiler* compiler, ShaderCompilerPlatform platform, const void* shaderCode, size_t shaderSize, ComputeShaderKernel& outKernel, ComputeShader::CBArray & outCBs, ShaderImportErrors& outErrors)
{
    D3D11ShaderReflection* reflector = NULL;
    HRESULT hr = compiler->D3DReflect(shaderCode, shaderSize, (void**)&reflector);
    if (FAILED_IMPL(hr))
    {
        outErrors.AddImportError("Failed to retrieve ComputeShader information", 0, false);
        return;
    }

    // grab thread group sizes
    reflector->GetThreadGroupSize(&outKernel.threadGroupSize[0], &outKernel.threadGroupSize[1], &outKernel.threadGroupSize[2]);

    D3D11_SHADER_DESC shaderDesc;
    reflector->GetDesc(&shaderDesc);
    // constant buffers
    for (UINT i = 0; i < shaderDesc.ConstantBuffers; ++i)
    {
        D3D11ShaderReflectionConstantBuffer* cb = reflector->GetConstantBufferByIndex(i);
        D3D11_SHADER_BUFFER_DESC desc;
        cb->GetDesc(&desc);
        if (desc.Type == D3D11_CT_CBUFFER)
        {
            ComputeShaderCB shaderCB;
            shaderCB.byteSize = desc.Size;
            shaderCB.name.SetName(desc.Name);

            for (UINT j = 0; j < desc.Variables; ++j)
            {
                D3D11ShaderReflectionVariable* var = cb->GetVariableByIndex(j);
                D3D11_SHADER_VARIABLE_DESC vdesc;
                var->GetDesc(&vdesc);
                if (!(vdesc.uFlags & D3D10_SVF_USED))
                    continue;
                D3D11_SHADER_TYPE_DESC tdesc;
                var->GetType()->GetDesc(&tdesc);
                ComputeShaderParam param;
                param.offset = vdesc.StartOffset;
                param.name.SetName(vdesc.Name);
                switch (tdesc.Type)
                {
                    case D3D10_SVT_FLOAT: param.type = kShaderParamFloat; break;
                    case D3D10_SVT_INT: param.type = kShaderParamInt; break;
                    case D3D10_SVT_UINT: param.type = kShaderParamUInt; break;
                    case D3D10_SVT_BOOL: param.type = kShaderParamBool; break;
                    default:
                        outErrors.AddImportError(Format("Unknown parameter type (%d) for %s", tdesc.Type, vdesc.Name), 0, false);
                        continue;
                }
                if (tdesc.Class == D3D10_SVC_MATRIX_COLUMNS || tdesc.Class == D3D10_SVC_VECTOR || tdesc.Class == D3D10_SVC_SCALAR)
                {
                    param.arraySize = tdesc.Elements;
                    param.colCount = tdesc.Columns;
                    param.rowCount = tdesc.Rows;
                    shaderCB.params.push_back(param);
                }
                else
                {
                    outErrors.AddImportError(Format("Unknown parameter type class (%d) for %s", tdesc.Class, vdesc.Name), 0, false);
                }
            }
            outCBs.push_back(shaderCB);
        }
        else if (desc.Type == D3D11_CT_RESOURCE_BIND_INFO)
        {
            //@TODO: no idea what to do with them; things like RWStructuredBuffer<float4> Result : register( u0 ) in
            // DX SDK HDRToneMappingCS11/FilterCS.hlsl produce that
        }
        else
        {
            outErrors.AddImportError(Format("Only scalar constant buffers are supported: %s", desc.Name), 0, false);
        }
    }

    // count texture/buffer UAVs for possible warning below
    UInt32 outTextures = 0;
    UInt32 outBuffers = 0;

    // resources, except samplers
    std::vector<core::string> textureResourceNames;
    for (UINT i = 0; i < shaderDesc.BoundResources; ++i)
    {
        D3D11_SHADER_INPUT_BIND_DESC desc;
        reflector->GetResourceBindingDesc(i, &desc);
        ComputeShaderResource res;
        res.name.SetName(desc.Name);
        res.bindPoint = desc.BindPoint;
        switch (desc.Dimension)
        {
            case D3D10_SRV_DIMENSION_TEXTURE2D: res.texDimension = kTexDim2D; break;
            case D3D10_SRV_DIMENSION_TEXTURE2DMS: res.texDimension = kTexDim2D; break;
            case D3D10_SRV_DIMENSION_TEXTURE3D: res.texDimension = kTexDim3D; break;
            case D3D10_SRV_DIMENSION_TEXTURECUBE: res.texDimension = kTexDimCUBE; break;
            case D3D10_SRV_DIMENSION_TEXTURE2DARRAY: res.texDimension = kTexDim2DArray; break;
            case D3D10_SRV_DIMENSION_TEXTURE2DMSARRAY: res.texDimension = kTexDim2DArray; break;
            case D3D10_SRV_DIMENSION_TEXTURECUBEARRAY: res.texDimension = kTexDimCubeArray; break;
            default: break;
        }

        switch (desc.Type)
        {
            case D3D10_SIT_TEXTURE:
                // Buffer<T>
                if (desc.Dimension == D3D10_SRV_DIMENSION_BUFFER)
                {
                    outKernel.inBuffers.push_back(res);
                }
                else
                {
                    res.samplerBindPoint = -1; // no sampler by default
                    outKernel.textures.push_back(res);
                    textureResourceNames.push_back(desc.Name);
                }
                break;
            case D3D10_SIT_SAMPLER: break; // skip samplers for now
            case D3D10_SIT_CBUFFER: outKernel.cbs.push_back(res); break;
            case D3D11_SIT_STRUCTURED: outKernel.inBuffers.push_back(res); break;
            case D3D11_SIT_BYTEADDRESS: outKernel.inBuffers.push_back(res); break;
            case D3D11_SIT_UAV_RWTYPED:
            case D3D11_SIT_UAV_RWSTRUCTURED:
            case D3D11_SIT_UAV_RWBYTEADDRESS:
            case D3D11_SIT_UAV_APPEND_STRUCTURED:
            case D3D11_SIT_UAV_CONSUME_STRUCTURED:
            case D3D11_SIT_UAV_RWSTRUCTURED_WITH_COUNTER:
                if (desc.Dimension == D3D10_SRV_DIMENSION_BUFFER)
                    outBuffers++;
                else
                    outTextures++;
                outKernel.outBuffers.push_back(res); break;
            default:
                core::string msg = Format("Unrecognized resource binding type (%i) for %s", desc.Type, desc.Name);
                outErrors.AddImportError(msg, 0, false);
                continue;
        }
    }

    // enforce maximum of D3D11_PS_CS_UAV_REGISTER_COUNT = 8 UAVs bound,
    // which is the limit before D3D11.1
    if (platform == kShaderCompPlatformD3D11 && outKernel.outBuffers.size() > 8)
    {
        UInt32 outUAVs = (int)outKernel.outBuffers.size();
        const char *texturePlural = outTextures == 1 ? "" : "s";
        const char *bufferPlural = outBuffers == 1 ? "" : "s";
        core::string msg = Format("Shader uses %u UAVs (%u texture%s and %u buffer%s), which is more than the 8 maximum currently supported on D3D11.0."
            "This is OK if you are only targeting D3D11.1 platforms",
            outUAVs, outTextures, texturePlural, outBuffers, bufferPlural);
        outErrors.AddImportError(msg, 0, true, kShaderCompPlatformD3D11);
    }

    // sampler resources (to match them up with any previous texture resources)
    for (UINT i = 0; i < shaderDesc.BoundResources; ++i)
    {
        D3D11_SHADER_INPUT_BIND_DESC desc;
        reflector->GetResourceBindingDesc(i, &desc);
        if (desc.Type != D3D10_SIT_SAMPLER)
            continue; // only interested in samplers now

        core::string samplerName = desc.Name;

        Assert(outKernel.textures.size() == textureResourceNames.size());
        bool matchedTexture = false;
        for (size_t j = 0; j < outKernel.textures.size(); ++j)
        {
            if (CheckSamplerAndTextureNameMatch(textureResourceNames[j], samplerName))
            {
                outKernel.textures[j].samplerBindPoint = desc.BindPoint;
                matchedTexture = true;
            }
        }
        if (!matchedTexture)
        {
            ComputeShaderBuiltinSampler cbSampler;
            core::string msg;
            cbSampler.sampler = ParseInlineSamplerName(samplerName, msg);
            cbSampler.bindPoint = desc.BindPoint;

            if (!cbSampler.sampler.IsInvalid())
                outKernel.builtinSamplers.push_back(cbSampler);
            else
                outErrors.AddImportError(msg, 0, false);
        }
    }

    SAFE_RELEASE(reflector);
}

void CompileShaderKernel(D3DCompiler* compiler, const ShaderCompileInputInterface* input, ComputeShaderCBListOfLists & outCBs, ComputeShaderKernelList & outKernels, ShaderImportErrors& errors)
{
    HRESULT hr;
    D3D10Blob *d3dshader = NULL;
    D3D10Blob *d3derrors = NULL;
    D3D10Blob* d3dStrippedShader = NULL;

    const ShaderCompilerPlatform platform = input->api;
    core::string pathName = input->GetSourceFilename();
    core::string fileName = GetLastPathNameComponent(pathName);
    core::string source = input->GetSource();
    core::string kernelName = input->GetEntryName((ShaderCompilerProgram)0);

    HLSLIncludeHandler includeHandler(input);

    const size_t macroCount = input->GetNumShaderArgs();

    D3D10_SHADER_MACRO* macros = new D3D10_SHADER_MACRO[macroCount + 2];

    for (size_t i = 0; i < macroCount; ++i)
    {
        macros[i].Name = input->GetShaderArgName(i);
        macros[i].Definition = input->GetShaderArgValue(i);
    }

    // API used
    macros[macroCount + 0].Name = GetCompilerPlatformDefine(input->api);
    macros[macroCount + 0].Definition = "1";
    // end of macros marker
    macros[macroCount + 1].Name = NULL;
    macros[macroCount + 1].Definition = NULL;

    const void* ptrSource = source.c_str();
    size_t lenSource = source.size();

    UINT compileFlags = D3D10_SHADER_ENABLE_BACKWARDS_COMPATIBILITY;

    const bool enableDebugCompilation = PLATFORM_WIN && HasFlag(input->compileFlags, kShaderCompFlags_EnableDebugInfoD3D11);
    if (!enableDebugCompilation && (platform == kShaderCompPlatformMetal || platform == kShaderCompPlatformVulkan || platform == kShaderCompPlatformOpenGLCore))
    {
        compileFlags |= D3D10_SHADER_PREFER_FLOW_CONTROL | D3D10_SHADER_OPTIMIZATION_LEVEL3;
    }

    // Same reasoning as for non-compute shaders applies here:
    //    debug compilation requires actual dll, which only works on windows (also see comments in InvokeCompilerHLSL11)
    //
    const bool oldUseRealDll = compiler->GetUseRealDLL();
#if PLATFORM_WIN
    core::string lineStrippedSource;
    if (HasFlag(input->compileFlags, kShaderCompFlags_EnableDebugInfoD3D11))
    {
        compileFlags |= D3D10_SHADER_DEBUG;
        compiler->SetUseRealDLL(true);

        // comment #line keywords, because they mess up D3D11 shader debugger
        lineStrippedSource = source;
        replace_string(lineStrippedSource, "#line", "//#line");
        ptrSource = lineStrippedSource.c_str();
        lenSource = lineStrippedSource.size();
    }
#endif

    const char* targetName = "cs_5_0";
    hr = compiler->D3DCompile(
        ptrSource,
        (unsigned long)lenSource,
        fileName.c_str(),
        macros,
        &includeHandler,
        kernelName.c_str(),
        targetName,
        compileFlags,     // | D3D10_SHADER_DEBUG | D3D10_SHADER_PREFER_FLOW_CONTROL | D3D10_SHADER_SKIP_OPTIMIZATION,
        0,
        &d3dshader,
        &d3derrors);

    if (d3derrors)
    {
        core::string msg(reinterpret_cast<const char*>(d3derrors->GetBufferPointer()), d3derrors->GetBufferSize());
        ParseErrorMessages(msg, false, "): fatal error ", kernelName, 0, platform, errors);
        ParseErrorMessages(msg, false, "): error "      , kernelName, 0, platform, errors);
        ParseErrorMessages(msg, false, "): error: "     , kernelName, 0, platform, errors);
        ParseErrorMessages(msg, true , "): warning "    , kernelName, 0, platform, errors);
        ParseErrorMessagesStartLine(msg, true, "warning ", kernelName, 0, platform, errors);
        ParseErrorMessagesStartLine(msg, "error ", kernelName, 0, platform, errors);
        if (!errors.HasErrorsOrWarnings())
        {
            core::string outerror = Format("Compilation failed for kernel '%s' [0x%08x - unknown error] '%s'", kernelName.c_str(), hr, msg.c_str());
            errors.AddImportError(outerror.c_str(), 0, false);
        }
    }
    else if (FAILED_IMPL(hr))
    {
        const char * hr_name = "unknown error";
#if PLATFORM_WIN
        switch (hr)
        {
            //case D3D10_ERROR_FILE_NOT_FOUND:                hr_name = "D3D10_ERROR_FILE_NOT_FOUND";                break;
            //case D3D10_ERROR_TOO_MANY_UNIQUE_STATE_OBJECTS: hr_name = "D3D10_ERROR_TOO_MANY_UNIQUE_STATE_OBJECTS"; break;
            //case D3DERR_INVALIDCALL:                        hr_name = "D3DERR_INVALIDCALL";                        break;
            //case D3DERR_WASSTILLDRAWING:                    hr_name = "D3DERR_WASSTILLDRAWING";                    break;
            case E_FAIL:                                    hr_name = "Attempted to create a device with the debug layer enabled and the layer is not installed.";    break;// E_FAIL
            case E_INVALIDARG:                              hr_name = "An invalid parameter was passed to the compiler";                                              break;// E_INVALIDARG
            case E_OUTOFMEMORY:                             hr_name = "Could not allocate sufficient memory to complete the compilation.";                            break;// E_OUTOFMEMORY
            case S_FALSE:                                   hr_name = "S_FALSE";                                                                                      break;
            default: break;
        }
#endif
        SAFE_RELEASE(d3dshader);
        core::string outerror = Format("Compilation failed [0x%08x - %s]", hr, hr_name);
        errors.AddImportError(outerror.c_str(), 0, false);
    }

    if (d3dshader)
    {
        // Get compiled shader
        const BYTE* shaderCode = reinterpret_cast<const BYTE*>(d3dshader->GetBufferPointer());
        DWORD shaderSize = d3dshader->GetBufferSize();

        ComputeShaderKernel shaderKernel;
        shaderKernel.name.SetName(kernelName.c_str());

        // Figure out parameters, buffers etc.
        ComputeShader::CBArray cbs;
        ReflectComputeShader(compiler, platform, shaderCode, shaderSize, shaderKernel, cbs, errors);

        // Strip reflect/debug information (never strip if debug flag is set)
        if (!HasFlag(input->compileFlags, kShaderCompFlags_DisableStripping)
#if PLATFORM_WIN
            && !HasFlag(input->compileFlags, kShaderCompFlags_EnableDebugInfoD3D11)
#endif
        )
        {
            hr = compiler->D3DStripShader(shaderCode, shaderSize, D3DCOMPILER_STRIP_REFLECTION_DATA | D3DCOMPILER_STRIP_DEBUG_INFO | D3DCOMPILER_STRIP_TEST_BLOBS, &d3dStrippedShader);
            if (SUCCEEDED_IMPL(hr))
            {
                shaderCode = reinterpret_cast<const BYTE*>(d3dStrippedShader->GetBufferPointer());
                shaderSize = d3dStrippedShader->GetBufferSize();
            }
        }

        shaderKernel.code.assign(shaderCode, shaderCode + shaderSize);

        outKernels.push_back(shaderKernel);
        outCBs.push_back(cbs);
    }
    SAFE_RELEASE(d3derrors);
    SAFE_RELEASE(d3dshader);
    SAFE_RELEASE(d3dStrippedShader);
    compiler->SetUseRealDLL(oldUseRealDll);
}
