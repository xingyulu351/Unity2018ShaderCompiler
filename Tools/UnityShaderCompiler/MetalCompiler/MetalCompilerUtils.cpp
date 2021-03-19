#include "UnityPrefix.h"
#include "MetalCompilerUtils.h"
#include "ShaderCompilerClient.h"
#include "Utilities/ShaderImportErrors.h"
#include "Editor/Src/Utility/TextUtilities.h"
#include "Runtime/Core/Format/Format.h"

static inline ShaderParamType ParamTypeFromMetal(core::string_ref metalType, ShaderImportErrors& outErrors, int line)
{
    if (metalType == "float")
        return kShaderParamFloat;
    else if (metalType == "int")
        return kShaderParamInt;
    else if (metalType == "bool")
        return kShaderParamBool;
    else if (metalType == "half")
        return kShaderParamHalf;
    else if (metalType == "short")
        return kShaderParamShort;
    else if (metalType == "uint")
        return kShaderParamUInt;

    outErrors.AddImportError(core::Format("unknown type '{0}'", metalType), line, false, kShaderCompPlatformMetal);
    return kShaderParamFloat;
}

static inline const char* ParamTypeToMetalString(ShaderParamType type)
{
    switch (type)
    {
        case kShaderParamFloat: return "float";
        case kShaderParamInt:   return "int";
        case kShaderParamBool:  return "bool";
        case kShaderParamHalf:  return "half";
        case kShaderParamShort: return "short";
        case kShaderParamUInt:  return "uint";

        default:
            DebugAssertFormatMsg(false, "unknown/unsupported ShaderParamType %u passed", type);
            return "";
    }
}

static inline TextureDimension TextureDimFromMetal(core::string_ref metalType, ShaderImportErrors& outErrors, int line)
{
    if (metalType == "2d_array")
        return kTexDim2DArray;
    else if (metalType == "cube_array")
        return kTexDimCubeArray;
    else if (metalType == "2d")
        return kTexDim2D;
    else if (metalType == "3d")
        return kTexDim3D;
    else if (metalType == "cube")
        return kTexDimCUBE;

    outErrors.AddImportError(core::Format("unknown/unsupported TextureDimension '{0}'", metalType), line, false, kShaderCompPlatformMetal);
    return kTexDim2D;
}

static inline const char* TextureDimToMetalString(TextureDimension dim)
{
    switch (dim)
    {
        case kTexDim2D:         return "2d";
        case kTexDim3D:         return "3d";
        case kTexDimCUBE:       return "cube";
        case kTexDim2DArray:    return "2d_array";
        case kTexDimCubeArray:  return "cube_array";

        default:
            DebugAssertFormatMsg(false, "unknown/unsupported TextureDimension %u passed", dim);
            return "";
    }
}

// XXX(a,b) yyy -> { "XXX(a,b)", "a b" }
struct MetalDirective
{
    core::string    directive;
    core::string    args;
    int             line;
};
static inline MetalDirective ExtractMetalDirective(const core::string& src, size_t directiveStart, ShaderImportErrors& outErrors)
{
    MetalDirective ret = {};
    ret.line = (int)CountLines(src, 0, directiveStart, true);

    const size_t argStart = src.find("(", directiveStart), argEnd = src.find(")", argStart);
    if (argStart == core::string::npos || argEnd == core::string::npos || argEnd == argStart + 1)
    {
        const core::string err = Format("malformed raw metal shader directive '%s'", ExtractRestOfLine(src, directiveStart).c_str());
        outErrors.AddImportError(err, ret.line, false, kShaderCompPlatformMetal);
        return ret;
    }

    ret.directive = src.substr(directiveStart, argEnd - directiveStart + 1);
    ret.args = src.substr(argStart + 1, argEnd - argStart - 1);
    {
        char* argsStr = &ret.args[0];

        size_t tStart = ret.args.find("<", 0);
        size_t tEnd = tStart == core::string::npos ? core::string::npos : ret.args.find(">", tStart);

        for (size_t i = 0, n = ret.args.length(); i < n; ++i)
        {
            bool insideTemplate = false;

            if (tEnd != core::string::npos && i > tEnd)
            {
                tStart = ret.args.find("<", i);
                tEnd = tStart == core::string::npos ? core::string::npos : ret.args.find(">", tStart);
            }
            if (tStart != core::string::npos && tEnd != core::string::npos && tStart < i && i < tEnd)
                insideTemplate = true;

            if (insideTemplate == false && argsStr[i] == ',')
                argsStr[i] = ' ';
        }
    }

    return ret;
}

static inline MetalDirective ExtractMetalTemplateArgs(const core::string& src, size_t directiveStart, ShaderImportErrors& outErrors)
{
    MetalDirective ret = {};
    ret.line = (int)CountLines(src, 0, directiveStart, true);

    const size_t argStart = src.find("<", directiveStart), argEnd = src.find(">", argStart);
    if (argStart == core::string::npos || argEnd == core::string::npos || argEnd == argStart + 1)
    {
        const core::string err = Format("malformed raw metal shader directive '%s'", ExtractRestOfLine(src, directiveStart).c_str());
        outErrors.AddImportError(err, ret.line, false, kShaderCompPlatformMetal);
        return ret;
    }

    ret.directive = src.substr(directiveStart, argEnd - directiveStart + 1);
    ret.args = src.substr(argStart + 1, argEnd - argStart - 1);

    return ret;
}

static inline void ReplaceMetalDirectiveByFormat(core::string& code, size_t directiveStart, const MetalDirective& directive, const char* format, ...)
{
    va_list va;
    va_start(va, format);
    core::string rep = VFormat(format, va);
    va_end(va);

    code.erase(directiveStart, directive.directive.length());
    code.insert(directiveStart, rep);
}

static const char MetalConstTagSearch[]         = "METAL_CONST_";
static const char MetalVertexInputTagSearch[]   = "METAL_VERTEX_INPUT";
static const char MetalTexInputTagSearch[]      = "METAL_TEX_INPUT";
static const char MetalBufferInputTagSearch[]   = "METAL_BUFFER_INPUT";

static const char MetalBufferInputDataTag[]     = "METAL_BUFFER_INPUT_DATA";

static const char MetalConstVectorTag[] = "METAL_CONST_VECTOR";
static const char MetalConstMatrixTag[] = "METAL_CONST_MATRIX";

static const char MetalTextureTag[] = "texture";
static const char MetalDepthTextureTag[] = "depth";

struct
MetalUniformDesc
{
    char name[65];
    ShaderParamType type;
    unsigned rows, cols;
    bool isMatrix;
};
static inline size_t ParamTypeAlignForMetal(ShaderParamType type)
{
    switch (type)
    {
        case kShaderParamFloat: case kShaderParamInt: case kShaderParamUInt:    return 4;
        case kShaderParamHalf: case kShaderParamShort:                          return 2;
        case kShaderParamBool:                                                  return 1;

        default: DebugAssert(false && "unknown ShaderParamType passed");        return 0;
    }
}

static inline size_t MetalUniformAlign(const MetalUniformDesc& u)
{
    // unity expects vectors to be rows
    // in metal matrix is composed of columns
    const size_t srcComponentCount = u.isMatrix ? u.rows : u.cols;
    const size_t alignComponentCount = srcComponentCount == 3 ? 4 : srcComponentCount;
    return (alignComponentCount * ParamTypeAlignForMetal(u.type));
}

static inline size_t MetalUniformSize(const MetalUniformDesc& u)
{
    const size_t uniformAlign = MetalUniformAlign(u);
    return u.isMatrix ? uniformAlign * u.cols : uniformAlign;
}

// weird i didnt find any aligned-by function in codebase
static inline size_t AlignedBy(size_t value, size_t align)
{
    return (value + align - 1) & (~(align - 1));
}

static MetalUniformDesc ReadMetalUniformDeclaration(const MetalDirective& directive, ShaderImportErrors& outErrors)
{
    MetalUniformDesc ret = {};

    const bool isVectorParam = ::strncmp(directive.directive.c_str(), MetalConstVectorTag, sizeof(MetalConstVectorTag) - 1) == 0;
    const bool isMatrixParam = ::strncmp(directive.directive.c_str(), MetalConstMatrixTag, sizeof(MetalConstMatrixTag) - 1) == 0;
    if (!isVectorParam && !isMatrixParam)
    {
        outErrors.AddImportError(Format("unknown directive '%s'", directive.directive.c_str()), directive.line, false, kShaderCompPlatformMetal);
        return ret;
    }

    char dataTypeString[17];

    ret.isMatrix = isMatrixParam;
    if (isMatrixParam)
        ::sscanf(directive.args.c_str(), "%16s %u %u %64s", dataTypeString, &ret.rows, &ret.cols, ret.name);
    else
        ::sscanf(directive.args.c_str(), "%16s %u %64s", dataTypeString, &ret.cols, ret.name);
    ret.type = ParamTypeFromMetal(dataTypeString, outErrors, directive.line);

    return ret;
}

bool ProcessMetalUniforms(core::string& code, const char* globalCbName, ShaderCompilerReflectionReport* reflectionReport, ShaderImportErrors& outErrors)
{
    std::vector<MetalUniformDesc> uniformList;
    size_t totalUniformSize = 0;

    size_t uniformDeclPos = code.find(MetalConstTagSearch, 0, sizeof(MetalConstTagSearch) - 1);
    while (uniformDeclPos != core::string::npos)
    {
        MetalDirective directive = ExtractMetalDirective(code, uniformDeclPos, outErrors);
        MetalUniformDesc u = ReadMetalUniformDeclaration(directive, outErrors);
        if (outErrors.HasErrors())
            return false;

        uniformList.push_back(u);
        totalUniformSize = AlignedBy(totalUniformSize, MetalUniformAlign(u)) + MetalUniformSize(u);

        if (u.isMatrix)
            ReplaceMetalDirectiveByFormat(code, uniformDeclPos, directive, "%s%ux%u %s", ParamTypeToMetalString(u.type), u.cols, u.rows, u.name);
        else if (u.cols > 1)
            ReplaceMetalDirectiveByFormat(code, uniformDeclPos, directive, "%s%u %s", ParamTypeToMetalString(u.type), u.cols, u.name);
        else
            ReplaceMetalDirectiveByFormat(code, uniformDeclPos, directive, "%s %s", ParamTypeToMetalString(u.type), u.name);

        uniformDeclPos = code.find(MetalConstTagSearch, uniformDeclPos, sizeof(MetalConstTagSearch) - 1);
    }

    reflectionReport->OnConstantBuffer(globalCbName, (int)totalUniformSize, (int)uniformList.size());
    reflectionReport->OnCBBinding(globalCbName, 0);

    size_t curUniformOffset = 0;
    for (size_t i = 0, n = uniformList.size(); i < n; ++i)
    {
        const MetalUniformDesc& u = uniformList[i];
        curUniformOffset = AlignedBy(curUniformOffset, MetalUniformAlign(u));
        reflectionReport->OnConstant(u.name, (int)curUniformOffset, u.type, u.isMatrix ? kConstantTypeMatrix : kConstantTypeDefault, u.rows, u.cols, 0);
        curUniformOffset += MetalUniformSize(u);
    }

    return true;
}

bool ProcessMetalVertexInputs(core::string& code, ShaderCompilerReflectionReport* reflectionReport, ShaderImportErrors& outErrors)
{
    size_t attrDeclPos = code.find(MetalVertexInputTagSearch, 0, sizeof(MetalVertexInputTagSearch) - 1);
    while (attrDeclPos != core::string::npos)
    {
        MetalDirective directive = ExtractMetalDirective(code, attrDeclPos, outErrors);
        if (outErrors.HasErrors())
            return false;

        unsigned attrIndex = (unsigned)::atoi(directive.args.c_str());
        reflectionReport->OnInputBinding(ShaderChannel(kShaderChannelVertex + attrIndex), VertexComponent(kVertexCompAttrib0 + attrIndex));
        ReplaceMetalDirectiveByFormat(code, attrDeclPos, directive, "[[attribute(%u)]]", attrIndex);

        attrDeclPos = code.find(MetalVertexInputTagSearch, attrDeclPos, sizeof(MetalVertexInputTagSearch) - 1);
    }

    return true;
}

struct
MetalTextureDesc
{
    char name[65];
    int index;
    TextureDimension dim;
    ShaderParamType type;
    core::string typeArgs;
    bool isDepth;
};
static MetalTextureDesc ReadMetalTextureDeclaration(MetalDirective directive, ShaderImportErrors& outErrors)
{
    MetalTextureDesc ret = {};

    const bool isTexture = ::strncmp(directive.args.c_str(), MetalTextureTag, sizeof(MetalTextureTag) - 1) == 0;
    const bool isDepthTexture = ::strncmp(directive.args.c_str(), MetalDepthTextureTag, sizeof(MetalDepthTextureTag) - 1) == 0;
    if (!isTexture && !isDepthTexture)
    {
        outErrors.AddImportError(Format("unknown directive '%s'", directive.directive.c_str()), directive.line, false, kShaderCompPlatformMetal);
        return ret;
    }

    const size_t argStart = directive.args.find("<", 0), argEnd = directive.args.find(">", argStart);
    if (!argStart || !argEnd)
    {
        outErrors.AddImportError(Format("unknown directive '%s'", directive.directive.c_str()), directive.line, false, kShaderCompPlatformMetal);
        return ret;
    }

    core::string textureTypeString = directive.args.substr(0, argStart);

    if (isDepthTexture)
        textureTypeString = textureTypeString.substr(strlen(MetalDepthTextureTag));
    else
        textureTypeString = textureTypeString.substr(strlen(MetalTextureTag));

    ::sscanf(directive.args.c_str() + argEnd + 1 , "%u %64s", &ret.index, ret.name);

    ret.isDepth = isDepthTexture;
    ret.dim = TextureDimFromMetal(textureTypeString, outErrors, directive.line);

    MetalDirective templateArgs = ExtractMetalTemplateArgs(directive.args, argStart, outErrors);
    ret.typeArgs = templateArgs.args;

    size_t firstMatch = templateArgs.args.find(',');
    ret.type = ParamTypeFromMetal(templateArgs.args.substr(0, firstMatch), outErrors, directive.line);

    return ret;
}

bool ProcessMetalTextures(core::string& code, ShaderCompilerReflectionReport* reflectionReport, ShaderImportErrors& outErrors)
{
    size_t texDeclPos = code.find(MetalTexInputTagSearch, 0, sizeof(MetalTexInputTagSearch) - 1);
    while (texDeclPos != core::string::npos)
    {
        MetalDirective directive = ExtractMetalDirective(code, texDeclPos, outErrors);
        MetalTextureDesc t = ReadMetalTextureDeclaration(directive, outErrors);
        if (outErrors.HasErrors())
            return false;

        reflectionReport->OnTextureBinding(t.name, t.index, t.index, false, t.dim);
        ReplaceMetalDirectiveByFormat(
            code, texDeclPos, directive,
            "%s%s<%s> %s [[texture(%u)]], sampler sampler_%s [[sampler(%u)]]",
            t.isDepth ? "depth" : "texture", TextureDimToMetalString(t.dim), t.typeArgs.c_str(), t.name, t.index, t.name, t.index
        );

        texDeclPos = code.find(MetalTexInputTagSearch, texDeclPos, sizeof(MetalTexInputTagSearch) - 1);
    }

    return true;
}

bool ProcessMetalBuffers(core::string& code, ShaderCompilerReflectionReport* reflectionReport, ShaderImportErrors& outErrors)
{
    size_t bufInputPos = code.find(MetalBufferInputTagSearch, 0, sizeof(MetalBufferInputTagSearch) - 1);
    while (bufInputPos != core::string::npos)
    {
        MetalDirective directive = ExtractMetalDirective(code, bufInputPos, outErrors);
        if (outErrors.HasErrors())
            return false;

        if (directive.directive.starts_with(MetalBufferInputDataTag))
        {
            char type[65] = {}, name[65] = {};
            if (sscanf(directive.args.c_str(), "%64s %64s", type, name) != 2)
                return false;

            ReplaceMetalDirectiveByFormat(code, bufInputPos, directive,
                "const device %s* %s = reinterpret_cast<const device %s*>(reinterpret_cast<const device atomic_uint*>(%s_inbuf) + 1);\n",
                type, name, type, name);
        }
        else
        {
            char type[65] = {}, name[65] = {}; int slot;
            if (sscanf(directive.args.c_str(), "%64s %d %64s", type, &slot, name) != 3)
                return false;

            reflectionReport->OnBufferBinding(name, slot);
            ReplaceMetalDirectiveByFormat(code, bufInputPos, directive, "const device %s* %s_inbuf [[buffer(%d)]]", type, name, slot);
        }


        bufInputPos = code.find(MetalBufferInputTagSearch, bufInputPos, sizeof(MetalBufferInputTagSearch) - 1);
    }

    return true;
}

void WriteShaderSnippetBlob(std::vector<unsigned char>& out, const char* entryPoint, const core::string& code, MetalShaderBlobHeader *metadata)
{
    MetalShaderBlobHeader hdr;

    if (metadata)
        hdr = *metadata;

    const size_t entryPointLen  = ::strlen(entryPoint) + 1;
    const size_t codeOffset     = hdr.size + entryPointLen;
    const size_t totalSize      = codeOffset + code.length();

    out.resize(totalSize);

    hdr.shaderCodeOffset = (uint32_t)codeOffset;

    ::memcpy(&out[0], &hdr, sizeof(hdr));
    ::memcpy(&out[sizeof(MetalShaderBlobHeader)], entryPoint, entryPointLen);
    ::memcpy(&out[codeOffset], code.c_str(), code.length());
}
