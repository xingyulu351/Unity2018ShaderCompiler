#include "UnityPrefix.h"
#include "D3DCompilerWrapper.h"
#include "Editor/Tools/Wodka/wodka_KnownImports.h"
#include <stdio.h>
#include <stdlib.h>
#if WODKA_WINDOWS
#include "PlatformDependent/Win/PathUnicodeConversion.h"
#include <errhandlingapi.h>
#endif // #if WODKA_WINDOWS

// LOAD_LIBRARY_SEARCH_DLL_LOAD_DIR from Win8 SDK. Defined here as we don't want to depend on the Win8 SDK (yet).
// On Windows 7, Windows Server 2008 R2, Windows Vista, and Windows Server 2008: This value requires KB2533623 to be installed.
#ifndef LOAD_LIBRARY_SEARCH_DLL_LOAD_DIR
#define LOAD_LIBRARY_SEARCH_DLL_LOAD_DIR    0x00000100
#endif

struct GUIDImpl
{
    DWORD Data1;
    WORD  Data2;
    WORD  Data3;
    BYTE  Data4[8];
};

static GUIDImpl kIID_ID3D11ShaderReflection = {0x8d536ca1, 0x0cca, 0x4956, {0xa8, 0x37, 0x78, 0x69, 0x63, 0x75, 0x55, 0x84}}; // d3dcompiler_46 and d3dcompiler_47
//static GUIDImpl kIID_ID3D11ShaderReflection = {0x0a233719, 0x3960, 0x4578, {0x9d, 0x7c, 0x20, 0x3b, 0x8b, 0x1d, 0x9c, 0xc1}}; // d3dcompiler_43


char* ReadFile(const char* filename, unsigned* outSize)
{
#   if WODKA_WINDOWS
    core::wstring widePath;
    ConvertUnityPathName(filename, widePath);
    FILE* f = _wfopen(widePath.c_str(), L"rb");
#   else // if PLATFORM_WIN
    FILE* f = fopen(filename, "rb");
#   endif // !if PLATFORM_WIN

    if (!f)
        return NULL;
    fseek(f, 0, SEEK_END);
    size_t size = ftell(f);
    fseek(f, 0, SEEK_SET);

    char* buf = (char*)malloc(size);
    if (!buf)
        return NULL;

    size_t res = fread(buf, 1, size, f);
    if (res != size)
    {
        free(buf);
        return NULL;
    }

    fclose(f);
    *outSize = (unsigned)size;
    return buf;
}

D3DCompilerWrapper::Exports::Exports()
{
    memset(this, 0, sizeof(*this));
}

const char* D3DCompilerWrapper::Initialize(const char* dllName, bool searchDLLLoadDir)
{
    m_UseRealDLL = false;

    m_Exports = Exports();

    unsigned dllSize;
    char* dllData = ReadFile(dllName, &dllSize);
    if (!dllData)
    {
        return "Failed to read D3DCompiler DLL file";
    }

    const char* errorMsg = NULL;
    errorMsg = PESetupFS();
    if (errorMsg != NULL)
        return errorMsg;

    InitializeWodkaImports();
    m_Dll = PELoadLibrary(dllData, dllSize, kWodkaKnownImports, kWodkaKnownImportsCount, &errorMsg);
    free(dllData);

    if (m_Dll)
    {
        m_Exports.compileFunc = (D3DCompileFunc)PEGetProcAddress(m_Dll, "D3DCompile");
        m_Exports.disassembleFunc = (D3DDisassembleFunc)PEGetProcAddress(m_Dll, "D3DDisassemble");
        m_Exports.assembleFunc = (D3DAssembleFunc)PEGetProcAddress(m_Dll, "D3DAssemble");
        m_Exports.preprocessFunc = (D3DPreprocessFunc)PEGetProcAddress(m_Dll, "D3DPreprocess");
        m_Exports.reflectFunc = (D3DReflectFunc)PEGetProcAddress(m_Dll, "D3DReflect");
        m_Exports.stripFunc = (D3DStripShaderFunc)PEGetProcAddress(m_Dll, "D3DStripShader");
        m_Exports.getBlobPartFunc = (D3DGetBlobPartFunc)PEGetProcAddress(m_Dll, "D3DGetBlobPart");
    }

#if WODKA_WINDOWS
    core::wstring dllNameWide;
    ConvertUTF8ToWideString(dllName, dllNameWide);

    DWORD loadLibraryFlags = searchDLLLoadDir ? LOAD_LIBRARY_SEARCH_DLL_LOAD_DIR : 0;
    HMODULE m = LoadLibraryExW(dllNameWide.c_str(), NULL, loadLibraryFlags);
    m_RealDll = m;

    if (m_RealDll)
    {
        m_RealExports.compileFunc = (D3DCompileFunc)GetProcAddress(m, "D3DCompile");
        m_RealExports.disassembleFunc = (D3DDisassembleFunc)GetProcAddress(m, "D3DDisassemble");
        m_RealExports.assembleFunc = (D3DAssembleFunc)GetProcAddress(m, "D3DAssemble");
        m_RealExports.preprocessFunc = (D3DPreprocessFunc)GetProcAddress(m, "D3DPreprocess");
        m_RealExports.reflectFunc = (D3DReflectFunc)GetProcAddress(m, "D3DReflect");
        m_RealExports.stripFunc = (D3DStripShaderFunc)GetProcAddress(m, "D3DStripShader");
        m_RealExports.getBlobPartFunc = (D3DGetBlobPartFunc)GetProcAddress(m, "D3DGetBlobPart");
    }
    else
    {
        DWORD error = GetLastError();
        printf("Failed loading dll [%s], code [0x%x]\n", dllName, error);
    }
#endif

    return errorMsg;
}

void D3DCompilerWrapper::Shutdown()
{
    if (m_Dll)
    {
        PEFreeLibrary(m_Dll);
        m_Dll = NULL;
    }
}

HRESULT D3DCompilerWrapper::D3DCompile(
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
    D3D10Blob** ppErrorMsgs)
{
    Exports& exports = GetExports();
    if (!exports.IsValid())
        return -1;
    PESetupFS();

    HRESULT hr = exports.compileFunc(pSrcData, SrcDataSize, pFileName, pDefines, pInclude, pEntrypoint, pTarget, Flags1, Flags2, ppCode, ppErrorMsgs);

    return hr;
}

HRESULT D3DCompilerWrapper::D3DAssemble(
    const void* srcData,
    SIZE_T srcDataSize,
    UINT flags,
    D3D10Blob** ppCode,
    D3D10Blob** ppErrorMsgs)
{
    Exports& exports = GetExports();
    if (!exports.IsValid())
        return -1;
    PESetupFS();

    HRESULT hr = exports.assembleFunc(srcData, srcDataSize, NULL, NULL, NULL, flags, ppCode, ppErrorMsgs);
    return hr;
}

HRESULT D3DCompilerWrapper::D3DDisassemble(
    const void* pSrcData,
    SIZE_T SrcDataSize,
    UINT flags,
    const char* szComments,
    D3D10Blob** ppDisassembly)
{
    Exports& exports = GetExports();
    if (!exports.IsValid())
        return -1;
    PESetupFS();

    HRESULT hr = exports.disassembleFunc(pSrcData, SrcDataSize, flags, szComments, ppDisassembly);
    return hr;
}

HRESULT D3DCompilerWrapper::D3DPreprocess(
    const void* pSrcData,
    unsigned long SrcDataSize,
    const char* pSourceName,
    const D3D10_SHADER_MACRO* pDefines,
    D3D10Include* pInclude,
    D3D10Blob** ppCodeText,
    D3D10Blob** ppErrorMsgs)
{
    Exports& exports = GetExports();
    if (!exports.IsValid())
        return -1;
    PESetupFS();

    HRESULT hr = exports.preprocessFunc(pSrcData, SrcDataSize, pSourceName, pDefines, pInclude, ppCodeText, ppErrorMsgs);

    return hr;
}

HRESULT D3DCompilerWrapper::D3DReflect(
    const void* src,
    SIZE_T size,
    void** reflector)
{
    Exports& exports = GetExports();
    if (!exports.IsValid())
        return -1;
    PESetupFS();

    HRESULT hr = exports.reflectFunc(src, size, kIID_ID3D11ShaderReflection, reflector);
    return hr;
}

HRESULT D3DCompilerWrapper::D3DStripShader(const void* pShaderBytecode,
    SIZE_T BytecodeLength,
    UINT uStripFlags,
    D3D10Blob** ppStrippedBlob)
{
    Exports& exports = GetExports();
    if (!exports.IsValid())
        return -1;
    PESetupFS();

    HRESULT hr = exports.stripFunc(pShaderBytecode, BytecodeLength, uStripFlags, ppStrippedBlob);
    return hr;
}

HRESULT D3DCompilerWrapper::D3DGetBlobPart(
    const void* pSrcData,
    SIZE_T SrcDataSize,
    D3D_BLOB_PART Part,
    UINT Flags,
    D3D10Blob** ppPart)
{
    Exports& exports = GetExports();
    if (!exports.IsValid())
        return -1;
    PESetupFS();

    HRESULT hr = exports.getBlobPartFunc(pSrcData, SrcDataSize, Part, Flags, ppPart);
    return hr;
}
