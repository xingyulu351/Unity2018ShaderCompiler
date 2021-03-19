// Written by Aras Pranckevicius
// I hereby place this code in the public domain

#pragma once

#include "D3DCompiler.h"

#define kD3DCompilerDLL "D3Dcompiler_47.dll"

struct PEModule;

class D3DCompilerWrapper : public D3DCompiler
{
public:
    // Returns error message on failure
    const char* Initialize(const char* dllName, bool searchDLLLoadDir = false);
    void Shutdown();

    // implement D3DCompiler interface
    bool IsValid() const { return m_Exports.IsValid(); }
    void SetUseRealDLL(bool v) { m_UseRealDLL = v; }
    bool GetUseRealDLL() const { return m_UseRealDLL; }

    HRESULT D3DCompile(
        const void* pSrcData,
        unsigned long SrcDataSize,
        const char* pFileName,
        const D3D10_SHADER_MACRO* pDefines,
        D3D10Include* pInclude,
        const char* pEntrypoint,
        const char* pTarget,
        unsigned int Flags1,
        unsigned int Flags2,
        D3D10Blob** ppCode,
        D3D10Blob** ppErrorMsgs);

    HRESULT D3DAssemble(
        const void* srcData,
        SIZE_T srcDataSize,
        UINT flags,
        D3D10Blob** ppCode,
        D3D10Blob** ppErrorMsgs);

    HRESULT D3DDisassemble(
        const void* pSrcData,
        SIZE_T SrcDataSize,
        UINT flags,
        const char* szComments,
        D3D10Blob** ppDisassembly);

    HRESULT D3DPreprocess(
        const void* pSrcData,
        unsigned long SrcDataSize,
        const char* pSourceName,
        const D3D10_SHADER_MACRO* pDefines,
        D3D10Include* pInclude,
        D3D10Blob** ppCodeText,
        D3D10Blob** ppErrorMsgs);

    HRESULT D3DReflect(
        const void* src,
        SIZE_T size,
        void** reflector);

    HRESULT D3DStripShader(const void* pShaderBytecode,
        SIZE_T BytecodeLength,
        UINT uStripFlags,
        D3D10Blob** ppStrippedBlob);

    HRESULT D3DGetBlobPart(
        const void* pSrcData,
        SIZE_T SrcDataSize,
        D3D_BLOB_PART Part,
        UINT Flags,
        D3D10Blob** ppPart);

private:
    typedef HRESULT (WINAPI_IMPL * D3DCompileFunc)(
        const void* pSrcData,
        unsigned long SrcDataSize,
        const char* pFileName,
        const D3D10_SHADER_MACRO* pDefines,
        D3D10Include* pInclude,
        const char* pEntrypoint,
        const char* pTarget,
        unsigned int Flags1,
        unsigned int Flags2,
        D3D10Blob** ppCode,
        D3D10Blob** ppErrorMsgs);
    typedef HRESULT (WINAPI_IMPL * D3DDisassembleFunc)(
        const void* pSrcData,
        SIZE_T SrcDataSize,
        UINT flags,
        const char* szComments,
        D3D10Blob** ppDisassembly);
    typedef HRESULT (WINAPI_IMPL * D3DAssembleFunc)(
        const void* srcData,
        SIZE_T srcDataSize,
        const char* sourceName,
        const D3D10_SHADER_MACRO* defines,
        D3D10Include* include,
        UINT flags,
        D3D10Blob** ppCode,
        D3D10Blob** ppErrorMsgs);
    typedef HRESULT (WINAPI_IMPL * D3DPreprocessFunc)(
        const void* pSrcData,
        unsigned long SrcDataSize,
        const char* pSourceName,
        const D3D10_SHADER_MACRO* pDefines,
        D3D10Include* pInclude,
        D3D10Blob** ppCodeText,
        D3D10Blob** ppErrorMsgs);
    typedef HRESULT (WINAPI_IMPL * D3DReflectFunc)(
        const void* src,
        SIZE_T size,
        const GUIDImpl& iface,
        void** reflector);
    typedef HRESULT (WINAPI_IMPL * D3DStripShaderFunc)(const void* pShaderBytecode,
        SIZE_T BytecodeLength,
        UINT uStripFlags,
        D3D10Blob** ppStrippedBlob);
    typedef HRESULT (WINAPI_IMPL * D3DGetBlobPartFunc)(
        const void* pSrcData,
        SIZE_T SrcDataSize,
        D3D_BLOB_PART Part,
        UINT Flags,
        D3D10Blob** ppPart);

    struct Exports
    {
        Exports();
        bool IsValid() const { return compileFunc != 0 && disassembleFunc != 0 && assembleFunc != 0 && preprocessFunc != 0 && reflectFunc != 0 && stripFunc != 0; }
        D3DCompileFunc      compileFunc;
        D3DDisassembleFunc  disassembleFunc;
        D3DAssembleFunc     assembleFunc;
        D3DPreprocessFunc   preprocessFunc;
        D3DReflectFunc      reflectFunc;
        D3DStripShaderFunc  stripFunc;
        D3DGetBlobPartFunc  getBlobPartFunc;
    };

    Exports& GetExports()
    {
        #if WODKA_WINDOWS
        if (m_UseRealDLL)
            return m_RealExports;
        #endif
        return m_Exports;
    }

    PEModule*   m_Dll;
    Exports     m_Exports;

    #if WODKA_WINDOWS
    void*   m_RealDll;
    Exports m_RealExports;
    #endif
    bool    m_UseRealDLL;
};
