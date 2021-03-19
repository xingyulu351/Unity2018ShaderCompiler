#pragma once

#include <vector>
#include <string>

#include "Editor/Src/Utility/TextUtilities.h"
#include "Runtime/GfxDevice/GfxDeviceTypes.h"
#include "Runtime/Serialize/TransferFunctions/StreamedBinaryWrite.h"
#include "Runtime/Serialize/TransferFunctions/StreamedBinaryRead.h"
#include "Runtime/Serialize/SerializationCaching/MemoryCacheWriter.h"
#include "Runtime/Serialize/SerializationCaching/MemoryCacheReader.h"
#include "Runtime/Shaders/ComputeShader.h"
#include "Runtime/Utilities/dynamic_array.h"
#include "Tools/UnityShaderCompiler/ShaderCompiler.h"

struct ComputeImportDirectives
{
    DECLARE_SERIALIZE_NO_PPTR(ComputeImportDirectives)
    typedef std::pair<core::string, core::string> Macro;
    typedef std::vector<Macro>           MacroList;

    struct Kernel
    {
        DECLARE_SERIALIZE_NO_PPTR(Kernel)
        core::string  name;
        MacroList macros;
    };
    std::vector<Kernel> kernels;
    ShaderCompilationFlags compileFlags;

    ComputeImportDirectives();
};

inline ComputeImportDirectives::ComputeImportDirectives() : compileFlags(kShaderCompFlags_None) {}

template<typename TransferFunc>
inline void ComputeImportDirectives::Transfer(TransferFunc & func)
{
    func.TransferSTLStyleArray(kernels);
    TransferEnumWithNameForceIntSize(func, compileFlags, "compileFlags", kNoTransferFlags);

    func.Align();
}

template<typename TransferFunc>
inline void ComputeImportDirectives::Kernel::Transfer(TransferFunc & func)
{
    func.Transfer(name , "name");

    SInt32 size = (SInt32)macros.size();
    func.Transfer(size, "size");
    if (func.IsWriting())
    {
        MacroList::iterator end = macros.end();
        for (MacroList::iterator i = macros.begin()
             ; i != end
             ; ++i)
        {
            func.Transfer(i->first  , "macroa");
            func.Transfer(i->second , "macrob");
        }
    }
    else
    {
        if (size > 0)
        {
            macros.reserve(size);
            for (int i = 0; i < size; ++i)
            {
                Macro x;
                func.Transfer(x.first , "macroa");
                func.Transfer(x.second , "macrob");
                macros.push_back(x);
            }
        }
    }
}

class ComputeShaderBinary
{
public:
    ComputeShaderCBListOfLists m_KernelCBs;
    ComputeShader::KernelArray m_Kernels;
    ComputeImportDirectives    m_ImportDirectives;
    UInt8                      m_ResourcesResolved;

    ComputeShaderBinary() :
        m_ResourcesResolved(true) {}

    ComputeShader::KernelArray & GetKernels()
    { return m_Kernels; }

    ComputeImportDirectives & GetImportDirectives()
    { return m_ImportDirectives; }

    ComputeShaderCBListOfLists & GetKernelCBs()
    { return m_KernelCBs; }

    template<typename TransferFunc>
    void Transfer(TransferFunc & transfer)
    {
        transfer.Transfer(m_ImportDirectives , "importDirectives");
        transfer.TransferSTLStyleArray(m_KernelCBs);
        transfer.TransferSTLStyleArray(m_Kernels);
        TRANSFER(m_ResourcesResolved);
    }

    void WriteToBuffer(dynamic_array<UInt8> & outputBuffer)
    {
        StreamedBinaryWrite writeStream;
        CachedWriter& writeCache = writeStream.Init(kNoTransferInstructionFlags, BuildTargetSelection::NoTarget());

        MemoryCacheWriter memoryCache(outputBuffer);
        writeCache.InitWrite(memoryCache);

        writeStream.Transfer(*this , "ComputeShader");
        writeCache.CompleteWriting();
    }

    template<typename T>
    void ReadFromBuffer(T & inputBuffer, MemLabelRef memLabel)
    {
        StreamedBinaryRead readStream;
        CachedReader & readCache = readStream.Init(kNoTransferInstructionFlags, memLabel);

        MemoryCacheReader memoryCache(inputBuffer);
        readCache.InitRead(memoryCache, 0, inputBuffer.size());

        readStream.Transfer(*this, "ComputeShader");
        readCache.End();
    }
};

// Compute shader container. Holds all versions of a compiled compute shader.
struct ComputeShaderData
{
    DECLARE_SERIALIZE_NO_PPTR(ComputeShaderData)
    core::string               name;
    dynamic_array<UInt8> data;
};

template<typename TransferFunc>
inline void ComputeShaderData::Transfer(TransferFunc & transfer)
{
    transfer.Transfer(name , "ShaderType");
    TRANSFER(data);
}

class ComputeShaderList
{
public:
    typedef std::vector<ComputeShaderData>      tDataArray;

    DECLARE_SERIALIZE_NO_PPTR(ComputeShaderList)
    ComputeShaderList() {}

    const tDataArray& GetShadersList() const { return shaders; }

    template<typename T>
    void AddComputeShader(T & data, core::string name)
    {
        using namespace std;
        shaders.push_back(ComputeShaderData());
        ComputeShaderData & odata = shaders.back();

        typename T::iterator it  = data.begin();
        typename T::iterator end = data.end();
        odata.data.reserve(data.size());
        for (; it != end; ++it)
        {
            odata.data.push_back(*it);
        }
        odata.name = name;
    }

    bool GetComputeShader(ComputeShaderBinary & data, const char * name, MemLabelRef label)
    {
        for (unsigned int i = 0; i < shaders.size(); ++i)
        {
            if (shaders[i].name == name)
            {
                data.ReadFromBuffer(shaders[i].data, label);
                return true;
            }
        }
        return false;
    }

    void WriteToBuffer(dynamic_array<UInt8> & outputBuffer)
    {
        using namespace std;
        StreamedBinaryWrite writeStream;
        CachedWriter & writeCache = writeStream.Init(kNoTransferInstructionFlags, BuildTargetSelection::NoTarget());

        MemoryCacheWriter memoryCache(outputBuffer);
        writeCache.InitWrite(memoryCache);

        writeStream.Transfer(*this , "ComputeShaders");
        writeCache.CompleteWriting();
    }

    template<typename T>
    void ReadFromBuffer(T & inputBuffer, MemLabelRef label)
    {
        StreamedBinaryRead readStream;
        CachedReader & readCache = readStream.Init(kNoTransferInstructionFlags, label);

        MemoryCacheReader memoryCache(inputBuffer);
        readCache.InitRead(memoryCache, 0, inputBuffer.size());

        readStream.Transfer(*this, "ComputeShaders");
        readCache.End();
    }

private:
    tDataArray                                    shaders;
};

template<typename TransferFunc>
inline void ComputeShaderList::Transfer(TransferFunc & transfer)
{
    transfer.TransferSTLStyleArray(shaders);
}

// Currently needed for both importing and compiling.
inline bool ParseComputeImportDirectives(core::string& src, ComputeImportDirectives& outParams, UInt32& supportedAPIs)
{
    size_t pos;

    // by default we consider all as supported
    supportedAPIs = 0xffffffff;

    // go over all pragma statements
    bool hadAnyKnownPragmas = false;
    const core::string kPragmaToken("#pragma");
    pos = FindTokenInText(src, kPragmaToken, 0);
    while (pos != core::string::npos)
    {
        bool knownPragma = false;

        // only match #pragma exactly
        if (!IsTabSpace(src[pos + kPragmaToken.size()]))
        {
            // find next pragma
            pos += kPragmaToken.size();
            pos = FindTokenInText(src, kPragmaToken, pos);
            continue;
        }

        // extract line
        core::string line = ExtractRestOfLine(src, pos + kPragmaToken.size());
        core::string pragmaLine = kPragmaToken + line;

        size_t linePos = 0;
        // skip whitespace
        linePos = SkipWhiteSpace(line, linePos);

        // read pragma name
        core::string pragmaName = ReadNonWhiteSpace(line, linePos);
        // skip whitespace after pragma name
        linePos = SkipWhiteSpace(line, linePos);

        if (pragmaName == "kernel")
        {
            // compute kernel entry point
            ComputeImportDirectives::Kernel kernel;
            kernel.name = ReadNonWhiteSpace(line, linePos);

            // read optional preprocessor defines following the name
            while (true)
            {
                linePos = SkipWhiteSpace(line, linePos);
                core::string macroString = ReadNonWhiteSpace(line, linePos);
                if (macroString.empty())
                    break;
                ComputeImportDirectives::Macro macro;
                size_t equalsPos = macroString.find('=');
                if (equalsPos == core::string::npos)
                {
                    macro.first = macroString;
                    macro.second.clear();
                }
                else
                {
                    macro.first = macroString.substr(0, equalsPos);
                    macro.second = macroString.substr(equalsPos + 1, macroString.size() - equalsPos - 1);
                }
                kernel.macros.push_back(macro);
            }


            outParams.kernels.push_back(kernel);
            knownPragma = true;
        }
        else if (pragmaName == "enable_d3d11_debug_symbols")
        {
            // enable DX11 debug info
            outParams.compileFlags |= kShaderCompFlags_EnableDebugInfoD3D11;
            knownPragma = true;
        }
        else if (pragmaName == "pssl2")
        {
            outParams.compileFlags |= kShaderCompFlags_PSSL2; // support pssl2 format compute shaders
            knownPragma = true;
        }
        else if (pragmaName == "hlslcc_bytecode_disassembly")
        {
            // embed HLSLcc bytecode disassembly to generated GLSL/Metal shader source
            outParams.compileFlags |= kShaderCompFlags_EmbedHLSLccDisassembly;
            knownPragma = true;
        }
        else if (pragmaName == "disable_fastmath")
        {
            // disable fastmath
            outParams.compileFlags |= kShaderCompFlags_DisableFastMath;
            knownPragma = true;
        }
        else if (pragmaName == "only_renderers")
        {
            // only given renderers
            supportedAPIs = 0;
            while (true)
            {
                core::string api = ReadNonWhiteSpace(line, linePos);
                if (api.empty())
                    break;
                linePos = SkipWhiteSpace(line, linePos);
                int mask = FindShaderPlatforms(api);
                if (mask != -1)
                {
                    supportedAPIs |= mask;
                }
            }
            knownPragma = true;
        }
        else if (pragmaName == "exclude_renderers")
        {
            // exclude given renderers
            while (true)
            {
                core::string api = ReadNonWhiteSpace(line, linePos);
                if (api.empty())
                    break;
                linePos = SkipWhiteSpace(line, linePos);
                int mask = FindShaderPlatforms(api);
                if (mask != -1)
                {
                    supportedAPIs &= ~mask;
                }
            }
            knownPragma = true;
        }

        if (knownPragma)
        {
            hadAnyKnownPragmas = true;

            // comment it out so that it does not confuse actual compute shader compiler
            src.insert(pos, "//");
            pos += 2;
        }

        // find next pragma
        pos += kPragmaToken.size();
        pos = FindTokenInText(src, kPragmaToken, pos);
    }

    // If the shader APIs include XboxOne then allow XBoxOneD3D12
    // On Xbox, unlike PC, we distinguish between 11 and 12 due to the root signature/precompilation process
    // But all shader source code is the same. Adding this prevents users having to add xboxone_d3d12 into pragma *_renderers statements
    if (supportedAPIs & (1 << kShaderCompPlatformXboxOne))
    {
        supportedAPIs |= (1 << kShaderCompPlatformXboxOneD3D12);
    }

    return hadAnyKnownPragmas;
}

struct D3DCompiler;

extern void CompileShaderKernel(D3DCompiler* compiler, const ShaderCompileInputInterface* input, ComputeShaderCBListOfLists & outCBs, ComputeShaderKernelList & outKernels, ShaderImportErrors& errors);
