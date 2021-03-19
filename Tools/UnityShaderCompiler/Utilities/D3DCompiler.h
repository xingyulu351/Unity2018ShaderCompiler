// Written by Aras Pranckevicius
// I hereby place this code in the public domain

#pragma once

#include "Editor/Tools/Wodka/wodka_WinHelper.h"

// Copied from c:\Program Files (x86)\Windows Kits\8.0\Include\um\d3d10shader.h

#define D3D10_SHADER_DEBUG                          (1 << 0)
#define D3D10_SHADER_SKIP_VALIDATION                (1 << 1)
#define D3D10_SHADER_SKIP_OPTIMIZATION              (1 << 2)
#define D3D10_SHADER_PACK_MATRIX_ROW_MAJOR          (1 << 3)
#define D3D10_SHADER_PACK_MATRIX_COLUMN_MAJOR       (1 << 4)
#define D3D10_SHADER_PARTIAL_PRECISION              (1 << 5)
#define D3D10_SHADER_FORCE_VS_SOFTWARE_NO_OPT       (1 << 6)
#define D3D10_SHADER_FORCE_PS_SOFTWARE_NO_OPT       (1 << 7)
#define D3D10_SHADER_NO_PRESHADER                   (1 << 8)
#define D3D10_SHADER_AVOID_FLOW_CONTROL             (1 << 9)
#define D3D10_SHADER_PREFER_FLOW_CONTROL            (1 << 10)
#define D3D10_SHADER_ENABLE_STRICTNESS              (1 << 11)
#define D3D10_SHADER_ENABLE_BACKWARDS_COMPATIBILITY (1 << 12)
#define D3D10_SHADER_IEEE_STRICTNESS                (1 << 13)
#define D3D10_SHADER_WARNINGS_ARE_ERRORS            (1 << 18)


// optimization level flags
#define D3D10_SHADER_OPTIMIZATION_LEVEL0            (1 << 14)
#define D3D10_SHADER_OPTIMIZATION_LEVEL1            0
#define D3D10_SHADER_OPTIMIZATION_LEVEL2            ((1 << 14) | (1 << 15))
#define D3D10_SHADER_OPTIMIZATION_LEVEL3            (1 << 15)

enum D3DCOMPILER_STRIP_FLAGS
{
    D3DCOMPILER_STRIP_REFLECTION_DATA = 1,
    D3DCOMPILER_STRIP_DEBUG_INFO      = 2,
    D3DCOMPILER_STRIP_TEST_BLOBS      = 4,
    D3DCOMPILER_STRIP_FORCE_DWORD     = 0x7fffffff,
};

struct D3D11ShaderReflection;

struct D3D10_SHADER_MACRO
{
    const char* Name;
    const char* Definition;
};

enum D3D10_INCLUDE_TYPE
{
    D3D10_INCLUDE_LOCAL,
    D3D10_INCLUDE_SYSTEM,
    // force 32-bit size enum
    D3D10_INCLUDE_FORCE_DWORD = 0x7fffffff
};

#if ENABLE_XBOXONE_COMPILER
// Copied from the XBoxOne XDK file "d3dcompiler_xdk.h"
enum D3D_BLOB_PART
{
    D3D_BLOB_INPUT_SIGNATURE_BLOB,
    D3D_BLOB_OUTPUT_SIGNATURE_BLOB,
    D3D_BLOB_INPUT_AND_OUTPUT_SIGNATURE_BLOB,
    D3D_BLOB_PATCH_CONSTANT_SIGNATURE_BLOB,
    D3D_BLOB_ALL_SIGNATURE_BLOB,
    D3D_BLOB_DEBUG_INFO,
    D3D_BLOB_LEGACY_SHADER,
    D3D_BLOB_XNA_PREPASS_SHADER,
    D3D_BLOB_XNA_SHADER,
    D3D_BLOB_PDB,
    D3D_BLOB_PRIVATE_DATA,
    D3D_BLOB_ROOT_SIGNATURE,
    D3D_BLOB_XBOX_SHADER_HASH,
    D3D_BLOB_XBOX_PDB_PATH,

    // Test parts are only produced by special compiler versions and so
    // are usually not present in shaders.
    D3D_BLOB_TEST_ALTERNATE_SHADER = 0x8000,
    D3D_BLOB_TEST_COMPILE_DETAILS,
    D3D_BLOB_TEST_COMPILE_PERF,
    D3D_BLOB_TEST_COMPILE_REPORT,
};
#else
// Copied from c:\Program Files (x86)\Windows Kits\8.0\Include\um\d3dcompiler.h
enum D3D_BLOB_PART
{
    D3D_BLOB_INPUT_SIGNATURE_BLOB,
    D3D_BLOB_OUTPUT_SIGNATURE_BLOB,
    D3D_BLOB_INPUT_AND_OUTPUT_SIGNATURE_BLOB,
    D3D_BLOB_PATCH_CONSTANT_SIGNATURE_BLOB,
    D3D_BLOB_ALL_SIGNATURE_BLOB,
    D3D_BLOB_DEBUG_INFO,
    D3D_BLOB_LEGACY_SHADER,
    D3D_BLOB_XNA_PREPASS_SHADER,
    D3D_BLOB_XNA_SHADER,
    D3D_BLOB_PDB,
    D3D_BLOB_PRIVATE_DATA,

    // Test parts are only produced by special compiler versions and so
    // are usually not present in shaders.
    D3D_BLOB_TEST_ALTERNATE_SHADER = 0x8000,
    D3D_BLOB_TEST_COMPILE_DETAILS,
    D3D_BLOB_TEST_COMPILE_PERF,
    D3D_BLOB_TEST_COMPILE_REPORT,
};
#endif

struct GUIDImpl;

struct D3D10Blob
{
    virtual HRESULT WINAPI_IMPL QueryInterface(const GUIDImpl& iid, void** ppv) = 0;
    virtual ULONG WINAPI_IMPL AddRef() = 0;
    virtual ULONG WINAPI_IMPL Release() = 0;
    virtual void* WINAPI_IMPL GetBufferPointer() = 0;
    virtual SIZE_T WINAPI_IMPL GetBufferSize() = 0;
};

struct D3D10Include
{
    virtual HRESULT WINAPI_IMPL Open(D3D10_INCLUDE_TYPE IncludeType, const char* pFileName, const void* pParentData, const void* *ppData, UINT *pBytes) = 0;
    virtual HRESULT WINAPI_IMPL Close(const void* pData) = 0;
};

struct D3DCompiler
{
    virtual bool IsValid() const = 0;
    virtual void SetUseRealDLL(bool v) = 0;
    virtual bool GetUseRealDLL() const = 0;

    virtual HRESULT D3DCompile(
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
        D3D10Blob** ppErrorMsgs) = 0;

    virtual HRESULT D3DAssemble(
        const void* srcData,
        SIZE_T srcDataSize,
        UINT flags,
        D3D10Blob** ppCode,
        D3D10Blob** ppErrorMsgs) = 0;

    virtual HRESULT D3DDisassemble(
        const void* pSrcData,
        SIZE_T SrcDataSize,
        UINT flags,
        const char* szComments,
        D3D10Blob** ppDisassembly) = 0;

    virtual HRESULT D3DPreprocess(
        const void* pSrcData,
        unsigned long SrcDataSize,
        const char* pSourceName,
        const D3D10_SHADER_MACRO* pDefines,
        D3D10Include* pInclude,
        D3D10Blob** ppCodeText,
        D3D10Blob** ppErrorMsgs) = 0;

    virtual HRESULT D3DReflect(
        const void* src,
        SIZE_T size,
        void** reflector) = 0;

    virtual HRESULT D3DStripShader(const void* pShaderBytecode,
        SIZE_T BytecodeLength,
        UINT uStripFlags,
        D3D10Blob** ppStrippedBlob) = 0;

    virtual HRESULT D3DGetBlobPart(
        const void* pSrcData,
        SIZE_T SrcDataSize,
        D3D_BLOB_PART Part,
        UINT Flags,
        D3D10Blob** ppPart) = 0;
};
