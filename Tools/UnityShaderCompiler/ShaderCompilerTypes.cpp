// please note that this file should be compiled both in editor and in shader compiler
// because it contains definitions for extern symbols in ShaderCompilerTypes.h

#include "UnityPrefix.h"
#include "ShaderCompilerTypes.h"

#include "Runtime/Utilities/StaticAssert.h"
#include <string.h>

const char*     kShaderLangBeginTag[]   = { "CGPROGRAM", "GLSLPROGRAM", "METALPROGRAM", "HLSLPROGRAM" };
const char*     kShaderLangEndTag[]     = { "ENDCG", "ENDGLSL", "ENDMETAL", "ENDHLSL" };
const char*     kShaderLangIncludeTag[] = { "CGINCLUDE", "GLSLINCLUDE", "METALINCLUDE", "HLSLINCLUDE" };

const size_t    kShaderLangBeginTagLength[]     = { ::strlen(kShaderLangBeginTag[0]), ::strlen(kShaderLangBeginTag[1]), ::strlen(kShaderLangBeginTag[2]), ::strlen(kShaderLangBeginTag[3]) };
const size_t    kShaderLangEndTagLength[]       = { ::strlen(kShaderLangEndTag[0]), ::strlen(kShaderLangEndTag[1]), ::strlen(kShaderLangEndTag[2]), ::strlen(kShaderLangEndTag[3]) };
const size_t    kShaderLangIncludeTagLength[]   = { ::strlen(kShaderLangIncludeTag[0]), ::strlen(kShaderLangIncludeTag[1]), ::strlen(kShaderLangIncludeTag[2]), ::strlen(kShaderLangIncludeTag[3]) };

CompileTimeAssertArraySize(kShaderLangBeginTag, kShaderSourceLanguageCount);
CompileTimeAssertArraySize(kShaderLangEndTag, kShaderSourceLanguageCount);
CompileTimeAssertArraySize(kShaderLangIncludeTag, kShaderSourceLanguageCount);
CompileTimeAssertArraySize(kShaderLangBeginTagLength, kShaderSourceLanguageCount);
CompileTimeAssertArraySize(kShaderLangEndTagLength, kShaderSourceLanguageCount);
CompileTimeAssertArraySize(kShaderLangIncludeTagLength, kShaderSourceLanguageCount);
