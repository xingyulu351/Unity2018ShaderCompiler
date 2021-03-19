#pragma once

// include the shared information with the plugin
#include "PlatformPlugin.h"
#include "ShaderCompiler.h"

// Implementation of the input data interface used for passing data to the shader compile job
struct ShaderCompileInputData : ShaderCompileInputInterface
{
    ShaderCompileInputData() {}

    // copy from an input interface
    void CopyFrom(const ShaderCompileInputInterface *input)
    {
        // copy POD in ShaderCompileInputInterface
        reflectionReport = input->reflectionReport;
        startLine = input->startLine;
        programType = input->programType;
        gpuProgramType = input->gpuProgramType;
        api = input->api;
        requirements = input->requirements;
        sourceLanguage = input->sourceLanguage;
        compileFlags = input->compileFlags;

        // copy complex data
        source.assign(input->GetSource(), input->GetSourceLength());
        sourceFilename.assign(input->GetSourceFilename(), input->GetSourceFilenameLength());

        p.compileFlags = input->compileFlags;
        for (int i = 0; i < kProgramCount; i++)
            p.entryName[i].assign(input->GetEntryName((ShaderCompilerProgram)i), input->GetEntryNameLength((ShaderCompilerProgram)i));

        includeContext = *input->GetIncludeSearchContext();

        const UInt8* compiledData = input->GetCompiledDataPtr();
        if (compiledData)
            gpuProgramData.assign(compiledData, compiledData + input->GetCompiledDataSize());

        size_t numArgs = input->GetNumShaderArgs();
        args.reserve(numArgs);
        for (size_t i = 0; i < numArgs; i++)
            args.push_back(ShaderCompileArg(core::string(input->GetShaderArgName(i), input->GetShaderArgNameLength(i)), core::string(input->GetShaderArgValue(i), input->GetShaderArgValueLength(i))));
    }

    virtual const char* GetSource() const { return source.c_str(); }
    virtual size_t GetSourceLength() const { return source.size(); }
    virtual const char* GetSourceFilename() const { return sourceFilename.c_str(); }
    virtual size_t GetSourceFilenameLength() const { return sourceFilename.size(); }

    virtual const char* GetEntryName(ShaderCompilerProgram type) const { return p.entryName[type].c_str(); }
    virtual size_t GetEntryNameLength(ShaderCompilerProgram type) const { return p.entryName[type].size(); }
    virtual const char* GetEntryName() const { return programType < kProgramCount ? p.entryName[programType].c_str() : ""; }

    virtual const IncludeSearchContext* GetIncludeSearchContext() const
    {
        return &includeContext;
    }

    virtual const UInt8* GetCompiledDataPtr() const { return gpuProgramData.data(); }
    virtual size_t GetCompiledDataSize() const { return gpuProgramData.size(); }

    virtual size_t GetNumShaderArgs() const { return args.size(); }
    virtual const char* GetShaderArgName(size_t index) const
    {
        return index < args.size() ? args[index].name.c_str() : "";
    }

    virtual size_t GetShaderArgNameLength(size_t index) const
    {
        return index < args.size() ? args[index].name.size() : 0;
    }

    virtual const char* GetShaderArgValue(size_t index) const
    {
        return index < args.size() ? args[index].value.c_str() : "";
    }

    virtual size_t GetShaderArgValueLength(size_t index) const
    {
        return index < args.size() ? args[index].value.size() : 0;
    }

    core::string source;
    core::string sourceFilename;
    ShaderCompileParams p;
    IncludeSearchContext includeContext;
    dynamic_array<UInt8> gpuProgramData;
    ShaderArgs args;
};

extern int sPlatformPluginCount;
extern PlatformPluginInterface* sPlatformPlugins[MAX_PLATFORM_PLUGINS];

void ReadPlatformPlugins(const core::string & path);

// Run the unit tests tests within a plugin.
int RunPluginUnitTests(const char* includesPath);
