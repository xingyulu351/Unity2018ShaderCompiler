#include "UnityPrefix.h"
#include "../Utilities/ShaderImportUtils.h"
#include "../ShaderCompiler.h"
#include "GLESCompilerUtils.h"
#include "Runtime/GfxDevice/GLSLUtilities.h"
#include "Runtime/Utilities/Word.h"
#include "Editor/Src/Utility/TextUtilities.h"

#define GL_VERTEX_ARRAY     0
#define GL_COLOR_ARRAY      1
#define GL_NORMAL_ARRAY     2
#define GL_TEXTURE_ARRAY0   3
#define GL_TEXTURE_ARRAY1   GL_TEXTURE_ARRAY0 + 1
#define GL_TEXTURE_ARRAY2   GL_TEXTURE_ARRAY0 + 2
#define GL_TEXTURE_ARRAY3   GL_TEXTURE_ARRAY0 + 3
#define GL_TEXTURE_ARRAY4   GL_TEXTURE_ARRAY0 + 4
#define GL_TEXTURE_ARRAY5   GL_TEXTURE_ARRAY0 + 5
#define GL_TEXTURE_ARRAY6   GL_TEXTURE_ARRAY0 + 6
#define GL_TEXTURE_ARRAY7   GL_TEXTURE_ARRAY0 + 7


static const UInt32 kAttribLookupTableSize = 12;

static const char* s_GLSLAttributes[] = // kAttribLookupTableSize
{
    "gl_Vertex", "gl_Color", "gl_Normal",
    "gl__unused__", // unused
    "gl_MultiTexCoord0", "gl_MultiTexCoord1", "gl_MultiTexCoord2", "gl_MultiTexCoord3",
    "gl_MultiTexCoord4", "gl_MultiTexCoord5", "gl_MultiTexCoord6", "gl_MultiTexCoord7"
};
CompileTimeAssertArraySize(s_GLSLAttributes, kAttribLookupTableSize);

static const char* s_GLSLESAttributes[kAttribLookupTableSize] = // kAttribLookupTableSize
{
    "_glesVertex", "_glesColor", "_glesNormal",
    "_gles_unused__", // unused
    "_glesMultiTexCoord0", "_glesMultiTexCoord1", "_glesMultiTexCoord2", "_glesMultiTexCoord3",
    "_glesMultiTexCoord4", "_glesMultiTexCoord5", "_glesMultiTexCoord6", "_glesMultiTexCoord7"
};
CompileTimeAssertArraySize(s_GLSLESAttributes, kAttribLookupTableSize);

static const int s_GLESVertexComponents[] = // kAttribLookupTableSize
{
    GL_VERTEX_ARRAY,
    GL_COLOR_ARRAY,
    GL_NORMAL_ARRAY,
    -1 /*kVertexCompTexCoord*/,
    GL_TEXTURE_ARRAY0, GL_TEXTURE_ARRAY1, GL_TEXTURE_ARRAY2, GL_TEXTURE_ARRAY3,
    GL_TEXTURE_ARRAY4, GL_TEXTURE_ARRAY5, GL_TEXTURE_ARRAY6, GL_TEXTURE_ARRAY7
};
CompileTimeAssertArraySize(s_GLESVertexComponents, kAttribLookupTableSize);

// Parses GLSL source code for #version string and return versionnumber and whether that's an ES shader.
// Note: lack of #version tag is treated as #version 120 for backwards compatibility.
// To specify an ES 2.0 shader, use "#version 100"
void ParseGLSLVersion(const core::string &source, int &outVersion, bool &outIsES)
{
    size_t pos = source.find("#version ");
    if (pos == source.npos)
    {
        // Not found?
        outVersion = 120;
        outIsES = false;
        return;
    }

    // Parse the version number
    outVersion = 0;
    outIsES = false;
    core::string::const_iterator itr = source.begin() + pos + 9;
    while (itr != source.end() && *itr != ' ' && *itr != '\n')
    {
        outVersion *= 10;
        const char &in = *itr;
        if (in >= '0' && in <= '9')
        {
            outVersion += (int)(in - '0');
        }
        else
        {
            // Invalid character, parse error. default to 120
            outVersion = 120;
            outIsES = false;
            return;
        }
        itr++;
    }
    if (*itr == ' ')
    {
        // Parse possible 'es'
        if (*(itr + 1) == 'e' && *(itr + 2) == 's')
            outIsES = true;
    }

    // ES 2.0 special case, no 'es'
    if (outVersion == 100)
        outIsES = true;
}

core::string buildGLSLVersionString(int version, bool isES)
{
    core::string res = Format("#version %d", version);
    if (isES && version != 100)
        res += " es";
    res += "\n";
    return res;
}

static bool HasGLStr(const core::string& source, const std::vector<typename core::string::size_type>& glUnderscorePositions, const char* value, size_t len = 0)
{
    const size_t count = glUnderscorePositions.size();
    const char* sourcePtr = source.c_str();
    const core::string::size_type* indexPtr = &*glUnderscorePositions.begin();
    if (len > 0)
    {
        for (size_t i = 0; i < count; ++i)
        {
            const char* lhs = sourcePtr + indexPtr[i];
            if (strncmp(lhs, value, len) == 0)
                return true;
        }
    }
    else
    {
        for (size_t i = 0; i < count; ++i)
        {
            const char* lhs = sourcePtr + indexPtr[i];
            const char* rhs = value;
            while (*rhs != '\0' && *rhs == *lhs)
            {
                ++rhs;
                ++lhs;
            }
            if (*rhs == '\0')
                return true;
        }
    }

    return false;
}

static bool HasGLStr(const core::string& source, const std::vector<typename core::string::size_type>& glUnderscorePositions, const core::string& value)
{
    return HasGLStr(source, glUnderscorePositions, value.c_str(), value.size());
}

// Generate a prefix for translating the old-style gl_FragData etc to somewhat contemporary stuff
static void GenerateGLSLOutputTranslationPrefix(const core::string& source, bool hasPrecisionQualifier, bool hasLocationQualifier, const std::vector<typename core::string::size_type>& glUnderscorePositions, core::string& prefix)
{
    const bool haveFramebufferFetch = (source.find("#extension GL_EXT_shader_framebuffer_fetch") != core::string::npos);
    const bool haveFragData = HasGLStr(source, glUnderscorePositions, "gl_FragData");
    const bool haveFragColor = HasGLStr(source, glUnderscorePositions, "gl_FragColor");

    if (haveFragColor || haveFragData)
    {
        if (haveFragData)
        {
            prefix += "#define gl_FragData _glesFragData\n";
            if (haveFragColor)
                prefix += "#define gl_FragColor _glesFragData[0]\n";

            // Find out the largest used fragdata index
            int maxFragDataUsed = 7;
            for (; maxFragDataUsed >= 0; maxFragDataUsed--)
            {
                core::string fragDataVariable = Format("gl_FragData[%d]", maxFragDataUsed);
                if (HasGLStr(source, glUnderscorePositions, fragDataVariable))
                {
                    if (hasLocationQualifier)
                        prefix += "layout(location = 0) ";
                    prefix += (haveFramebufferFetch ? "inout " : "out ");
                    if (hasPrecisionQualifier)
                        prefix += "mediump ";
                    prefix += Format("vec4 _glesFragData[%d];\n", maxFragDataUsed + 1);
                    break;
                }
            }
        }
        else
        {
            prefix += "#define gl_FragColor _glesFragColor\n";
            if (hasLocationQualifier)
                prefix += "layout(location = 0) ";
            prefix += (haveFramebufferFetch ? "inout " : "out ");
            if (hasPrecisionQualifier)
                prefix += "mediump ";
            prefix += "vec4 _glesFragColor;\n";
        }
    }
}

void FixupGLSLFixedFunctionVariables(core::string& source, core::string& inoutPrefix)
{
    // Reroute gl_LightSource and gl_LightModel to actual unity variables.
    // Light source replaced in a very limited form, basically only "gl_LightSource[i]" strings.
    if (source.find("gl_LightSource") != core::string::npos)
    {
        inoutPrefix += "uniform vec4 unity_LightColor[8];\n";
        inoutPrefix += "uniform vec4 unity_LightPosition[8];\n";
        inoutPrefix += "uniform vec4 unity_LightAtten[8];\n";
        inoutPrefix += "uniform vec4 unity_LightSpotDirection[8];\n";
        replace_string(source, "gl_LightSource[i].position", "unity_LightPosition[i]");
        replace_string(source, "gl_LightSource[i].diffuse", "unity_LightColor[i]");
        replace_string(source, "gl_LightSource[i].atten", "unity_LightAtten[i]");
        replace_string(source, "gl_LightSource[i].quadraticAttenuation", "unity_LightAtten[i].z");
        replace_string(source, "gl_LightSource[i].spotDirection", "unity_LightSpotDirection[i]");
    }
    if (source.find("gl_LightModel") != core::string::npos)
    {
        inoutPrefix += "uniform vec4 glstate_lightmodel_ambient;\n";
        replace_string(source, "gl_LightModel.ambient", "glstate_lightmodel_ambient");
    }
}

core::string GenerateGLSLShaderPrefix(const core::string& source, ShaderCompilerProgram glShaderType, bool hasPrecisionQualifier, bool hasLocationQualifier, bool hasInOut)
{
    const char* inputQualifier = hasInOut ? "in" : "attribute";
    core::string prefix("", 0);
    prefix.reserve(4096);

    std::vector<typename core::string::size_type> glUnderscorePositions;
    glUnderscorePositions.reserve(128);

    static core::string glPrefix("gl_");
    core::string::size_type currentPos = source.find(glPrefix);
    while (currentPos != core::string::npos)
    {
        glUnderscorePositions.push_back(currentPos);
        currentPos = source.find(glPrefix, currentPos + glPrefix.size());
    }

    // Add necessary attribute tags
    if (glShaderType == kProgramVertex)
    {
        for (UInt32 j = 0; j < kAttribLookupTableSize; j++)
        {
            if (!HasGLStr(source, glUnderscorePositions, s_GLSLAttributes[j]))
                continue;

            if (s_GLESVertexComponents[j] == GL_NORMAL_ARRAY)
            {
                prefix += Format("#define %s %s\n", s_GLSLAttributes[j], s_GLSLESAttributes[j]);
                prefix += Format("%s vec3 %s;\n", inputQualifier, s_GLSLESAttributes[j]); // vec3 for normals
            }
            else
            {
                prefix += Format("#define %s %s\n", s_GLSLAttributes[j], s_GLSLESAttributes[j]);
                prefix += Format("%s vec4 %s;\n", inputQualifier, s_GLSLESAttributes[j]);
            }
        }
    }

    core::string texcoordPrefix("#define gl_TexCoord _glesTexCoord\n");
    texcoordPrefix += (hasInOut ? ((glShaderType == kProgramVertex) ? "out " : "in ") : "varying ");
    if (hasPrecisionQualifier)
        texcoordPrefix += "highp ";
    // scan for highest texCoord in use
    int maxTexCoordUsed = 7;
    for (; maxTexCoordUsed >= 0; maxTexCoordUsed--)
    {
        core::string texCoordVariable = Format("gl_TexCoord[%d]", maxTexCoordUsed);
        if (HasGLStr(source, glUnderscorePositions, texCoordVariable))
        {
            prefix += texcoordPrefix;
            prefix += Format("vec4 _glesTexCoord[%d];\n", maxTexCoordUsed + 1); // some GPU like Tegra prefer texcoords to be highp
            break;
        }
    }

    if (maxTexCoordUsed < 7)
    {
        if (glShaderType == kProgramVertex)
        {
            if (HasGLStr(source, glUnderscorePositions, "gl_FrontColor"))
            {
                prefix += "#define gl_FrontColor _glesFrontColor\n";
                if (hasInOut)
                    prefix += (glShaderType == kProgramVertex) ? "out " : "in ";
                else
                    prefix += "varying ";
                if (hasPrecisionQualifier)
                    prefix += "lowp ";
                prefix += "vec4 _glesFrontColor;\n";
            }
        }
        if (glShaderType == kProgramFragment)
        {
            if (HasGLStr(source, glUnderscorePositions, "gl_Color"))
            {
                prefix += "#define gl_Color _glesFrontColor\n";
                if (hasInOut)
                    prefix += (glShaderType == kProgramVertex) ? "out " : "in ";
                else
                    prefix += "varying ";
                if (hasPrecisionQualifier)
                    prefix += "lowp ";
                prefix += "vec4 _glesFrontColor;\n";
            }
        }
    }
    else
    {
        printf_console("WARNING: Shader uses too many varyings for this platform, fog and vertex colors will be disabled for this shader");

        // most platforms support only 8 varyings
        // if running out of variables - disable fog/vertex-colors
        if (glShaderType == kProgramVertex)
            prefix += "#define gl_FrontColor vec4 dummyVar\n";
        if (glShaderType == kProgramFragment)
            prefix += "#define gl_Color vec4(1.0, 1.0, 1.0, 1.0)\n";
        prefix += "#define gl_FogFragCoord 1.0\n";
    }

    if (glShaderType == kProgramFragment && hasInOut)
    {
        GenerateGLSLOutputTranslationPrefix(source, hasPrecisionQualifier, hasLocationQualifier, glUnderscorePositions, prefix);
    }

    return prefix;
}

size_t FindAttributeDefinition(bool usesInOut, const core::string& str, const core::string& attributeName,
    core::string* caseSensitiveAttributeName, core::string* definitionStr)
{
    core::string beginsWith = usesInOut ? "in" : "attribute";

    core::string endsWith = ";";

    Assert(attributeName != "");
    Assert(attributeName != beginsWith);
    Assert(attributeName != endsWith);


    size_t b = 0;
    core::string attrDef = "", attrName = "";
    for (; b != core::string::npos; b += attrDef.size())
    {
        if ((b = FindTokenInText(str, beginsWith, b)) == core::string::npos)
            return core::string::npos;

        size_t e = str.find(endsWith, b);
        if (e == core::string::npos)
            return core::string::npos;

        attrDef = str.substr(b, e - b + endsWith.size());

        size_t paramListEnd = str.find(")", b); // Make sure we are not mathing function input arg
        if (paramListEnd != core::string::npos && paramListEnd < e)
            continue;

        // if we have attribute name we're searching for
        size_t pos;
        if ((pos = ToUpper(attrDef).find(ToUpper(attributeName))) == core::string::npos)
            continue;

        // if found attribute name matches the length
        // NOTE: attribute name can be followed by whitespace
        size_t l = b + pos + attributeName.size();
        if (l != e && !IsSpace(str[l]))
            continue;

        attrName = attrDef.substr(pos, attributeName.size());
        break; // found!
    }

    if (b == core::string::npos)
        return core::string::npos;

    if (caseSensitiveAttributeName)
    {
        *caseSensitiveAttributeName = attrName;
        Assert(ToUpper(*caseSensitiveAttributeName) == ToUpper(attributeName));
    }

    if (definitionStr)
    {
        *definitionStr = attrDef;
    }

    return b;
}

size_t FindShaderSplitPoint(const core::string& source)
{
    // Find last #extension statement
    size_t lastExtensionPos = 0;
    bool extensionFound = false;
    while (true)
    {
        size_t extensionPos = FindTokenInText(source, "#extension ", lastExtensionPos);
        if (extensionPos == core::string::npos)
            break;
        extensionFound = true;
        lastExtensionPos = extensionPos + 1;
    }
    if (extensionFound)
        return SkipToNextLine(source, lastExtensionPos);
    return 0;
}

core::string FixTangentBinding(const core::string& source, bool usesInOut)
{
    const char* inputQualifier = usesInOut ? "in" : "attribute";
    core::string modSource;


    // Somewhere deep inside shader generation we just put attribute TANGENT
    // without ever using it after
    // look for TANGENT twice to be sure we use it
    core::string caseSensitiveTangentName, tangentAttributeDefinition;
    size_t firstTangentUsage =
        FindAttributeDefinition(usesInOut, source, "TANGENT", &caseSensitiveTangentName, &tangentAttributeDefinition);
    size_t afterTangentAttributeDefinition = firstTangentUsage + tangentAttributeDefinition.size();
    if (firstTangentUsage != core::string::npos &&
        source.find(caseSensitiveTangentName, afterTangentAttributeDefinition) != core::string::npos)
    {
        Assert(caseSensitiveTangentName.size() > 0);
        Assert(tangentAttributeDefinition.size() > 0);

        modSource += "#define ";
        modSource += caseSensitiveTangentName;
        modSource += " _glesTANGENT\n";
        modSource += Format("%s vec4 _glesTANGENT;\n", inputQualifier);
    }

    // Add original source:
    // First put any #extension parts of the original source in front,
    // then put our generated defined, then the rest of the source.
    size_t sourceSplitPos = FindShaderSplitPoint(source);
    modSource = source.substr(0, sourceSplitPos) + modSource;
    modSource += source.substr(sourceSplitPos, source.size() - sourceSplitPos);

    replace_string(modSource, "\t", " ");

    // remove original GLSL TANGENT declaration
    replace_string(modSource, tangentAttributeDefinition, "");

    return modSource;
}

static void ReplaceVaryingKeywordsWithInOut(core::string& vs, core::string& fs)
{
    // Space at the end of strings to prevent mid-string replaces
    replace_string(vs, "attribute ", "in ");
    replace_string(vs, "varying ", "out ");
    replace_string(fs, "varying ", "in ");
}

// Replaces old-style 'texture2D' -> 'texture'
static void RemoveTextureSamplerQualifiers(core::string & source)
{
    // in GLES3 and GLSL >= 150, texture2D/3D/CUBE etc are now just "texture", deduced from sampler type.
    replace_string(source, "texture1D", "texture");
    replace_string(source, "texture2D", "texture");
    replace_string(source, "texture3D", "texture");
    replace_string(source, "textureCUBE", "texture");
    replace_string(source, "shadow1D", "texture");
    replace_string(source, "shadow2D", "texture");
    replace_string(source, "shadow1DProj", "textureProj");
    replace_string(source, "shadow2DProj", "textureProj");
    replace_string(source, "shadow1DLod", "textureLod");
    replace_string(source, "shadow2DLod", "textureLod");
    replace_string(source, "shadow1DProjLod", "textureProjLod");
    replace_string(source, "shadow2DProjLod", "textureProjLod");
}

core::string TranslateGLSLToGLES20(const core::string &source)
{
    int ver;
    bool isES;
    ParseGLSLVersion(source, ver, isES);

    // Already an ES shader?
    if (isES)
    {
        // 3.0 and later not supported
        if (ver > 100)
            return core::string("");
        // Otherwise pass through as is
        return source;
    }

    // Desktop shader, version >120 doesn't translate
    if (ver > 120)
    {
        // Too new, not going to work
        return core::string("");
    }

    ver = 100;
    isES = true;

    // We're left with legacy GL shader. Translate as much as we can.
    core::string remainder1, remainder2;
    core::string    vertexShaderSource = glsl::ExtractDefineBlock("VERTEX", source, &remainder1);
    core::string    fragmentShaderSource = glsl::ExtractDefineBlock("FRAGMENT", remainder1, &remainder2);

    vertexShaderSource = FixTangentBinding(vertexShaderSource, false);

    vertexShaderSource.insert(FindShaderSplitPoint(vertexShaderSource), GenerateGLSLShaderPrefix(vertexShaderSource, kProgramVertex, true, false, false));
    fragmentShaderSource.insert(FindShaderSplitPoint(fragmentShaderSource), GenerateGLSLShaderPrefix(fragmentShaderSource, kProgramFragment, true, false, false) +
        "#ifdef GL_FRAGMENT_PRECISION_HIGH\nprecision highp float;\n#else\nprecision mediump float;\n#endif\nprecision highp int;\n");

    core::string output = buildGLSLVersionString(ver, isES)
        + remainder2
        + "\n#ifdef VERTEX\n"
        + vertexShaderSource
        + "\n#endif\n#ifdef FRAGMENT\n"
        + fragmentShaderSource
        + "\n#endif";

    // OpenGLES2.0 has no distinction between 2D and Rect textures
    replace_string(output, "texture2DRect", "texture2D");
    replace_string(output, "sampler2DRect", "sampler2D");

    return output;
}

// Quick'n'dirty desktop GLSL to GLSL ES version mapping.
static int mapDesktopGLSLVersionToES(int in)
{
    if (in <= 330)
        return 300;
    else
        return 310;
}

core::string TranslateGLSLToGLES30Plus(const core::string &source)
{
    int ver;
    bool isES;
    ParseGLSLVersion(source, ver, isES);

    // Already an ES shader? Or a modern GLSL language?
    if (isES || ver > 120)
    {
        // Just make sure the #version string is in place
        core::string res = source;
        if (!isES)
        {
            ver = mapDesktopGLSLVersionToES(ver);
        }
        replace_string(res, "#version", "//version");
        return buildGLSLVersionString(ver, isES) + res;
    }

    ver = 300;
    isES = true;

    // We're left with legacy GL shader. Translate as much as we can.
    core::string remainder1, remainder2;
    core::string    vertexShaderSource = glsl::ExtractDefineBlock("VERTEX", source, &remainder1);
    core::string    fragmentShaderSource = glsl::ExtractDefineBlock("FRAGMENT", remainder1, &remainder2);


    ReplaceVaryingKeywordsWithInOut(vertexShaderSource, fragmentShaderSource);

    vertexShaderSource = FixTangentBinding(vertexShaderSource, true);

    replace_string(vertexShaderSource, "#version", "//version");
    replace_string(fragmentShaderSource, "#version", "//version");

    // in GLES3, texture2D/3D/CUBE etc are now just "texture", deduced from sampler type.
    replace_string(vertexShaderSource, "texture2D", "texture");
    replace_string(fragmentShaderSource, "texture2D", "texture");
    replace_string(vertexShaderSource, "texture3D", "texture");
    replace_string(fragmentShaderSource, "texture3D", "texture");
    replace_string(vertexShaderSource, "textureCUBE", "texture");
    replace_string(fragmentShaderSource, "textureCUBE", "texture");


    vertexShaderSource.insert(FindShaderSplitPoint(vertexShaderSource), GenerateGLSLShaderPrefix(vertexShaderSource, kProgramVertex, true, true, true));
    fragmentShaderSource.insert(FindShaderSplitPoint(fragmentShaderSource), GenerateGLSLShaderPrefix(fragmentShaderSource, kProgramFragment, true, true, true) + "\nprecision highp float;\n");

    core::string output = buildGLSLVersionString(ver, isES)
        + remainder2
        + "\n#ifdef VERTEX\n"
        + vertexShaderSource
        + "\n#endif\n#ifdef FRAGMENT\n"
        + fragmentShaderSource
        + "\n#endif";

    return output;
}

core::string TranslateGLSLToGLUnifiedShader(const core::string &source)
{
    int ver;
    bool isES;
    ParseGLSLVersion(source, ver, isES);

    if (ver >= 150)
    {
        // If the version is >=150, we can take the shader in as-is.
        // Anyway, we're done, return original source
        return source;
    }

    // Converting ES shaders basically just means upgrading the version number and dropping ES
    if (isES)
    {
        isES = false;
        if (ver < 300)
            ver = 150;
        else if (ver == 300)
            ver = 330;
        else
            ver = 420;

        core::string res = source;
        replace_string(res, "#version", "//version");
        res = buildGLSLVersionString(ver, false) + res;
        return res;
    }

    // We're left with legacy GL shader. Translate as much as we can.
    core::string remainder1, remainder2;
    core::string vertexShaderSource = glsl::ExtractDefineBlock("VERTEX", source, &remainder1);
    core::string fragmentShaderSource = glsl::ExtractDefineBlock("FRAGMENT", remainder1, &remainder2);

    ReplaceVaryingKeywordsWithInOut(vertexShaderSource, fragmentShaderSource);

    vertexShaderSource = FixTangentBinding(vertexShaderSource, true);

    replace_string(vertexShaderSource, "#version", "//version");
    replace_string(fragmentShaderSource, "#version", "//version");

    if (ver == 120 && !isES)
        ver = 150;

    vertexShaderSource.insert(FindShaderSplitPoint(vertexShaderSource), GenerateGLSLShaderPrefix(vertexShaderSource, kProgramVertex, false, ver > 150, true));
    fragmentShaderSource.insert(FindShaderSplitPoint(fragmentShaderSource), GenerateGLSLShaderPrefix(fragmentShaderSource, kProgramFragment, false, ver > 150, true));

    core::string output = buildGLSLVersionString(ver, isES)
        + remainder2
        + "\n#ifdef VERTEX\n"
        + vertexShaderSource
        + "\n#endif\n#ifdef FRAGMENT\n"
        + fragmentShaderSource
        + "\n#endif";

    RemoveTextureSamplerQualifiers(output);
    return output;
}
