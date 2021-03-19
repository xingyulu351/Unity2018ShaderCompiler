#pragma once

#include "Tools/UnityShaderCompiler/ShaderCompilerTypes.h"

core::string GenerateGLSLShaderPrefix(const core::string& source, ShaderCompilerProgram glShaderType, bool hasPrecisionQualifier, bool hasLocationQualifier, bool hasInOut);
core::string FixTangentBinding(const core::string& source, bool usesInOut);
size_t FindShaderSplitPoint(const core::string& source);

void FixupGLSLFixedFunctionVariables(core::string& source, core::string& inoutPrefix);

void ParseGLSLVersion(const core::string &source, int &outVersion, bool &outIsES);
core::string buildGLSLVersionString(int version, bool isES);


core::string TranslateGLSLToGLUnifiedShader(const core::string &source);
core::string TranslateGLSLToGLES20(const core::string &source);
core::string TranslateGLSLToGLES30Plus(const core::string &source);

size_t FindAttributeDefinition(bool usesInOut, const core::string& str, const core::string& attributeName,
    core::string* caseSensitiveAttributeName = 0, core::string* definitionStr = 0);
