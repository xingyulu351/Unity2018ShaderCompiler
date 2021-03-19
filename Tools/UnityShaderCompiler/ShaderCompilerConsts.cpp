#include "UnityPrefix.h"
#include "ShaderCompiler.h"
#include "Runtime/GfxDevice/GfxDeviceTypes.h"


const char* kSCShaderChannelNames[] =
{
    "vertex",
    "normal",
    "color",
    "texcoord",
    "texcoord1",
    "texcoord2",
    "texcoord3",
    "tangent",
    "blendweight",
    "blendindices",
};
CompileTimeAssertArraySize(kSCShaderChannelNames, kShaderChannelCount);

const char* kSCVertexCompNames[kVertexCompCount] =
{
    "Vertex",
    "Color",
    "Normal",
    "TexCoord",
    "TexCoord0", "TexCoord1", "TexCoord2", "TexCoord3", "TexCoord4", "TexCoord5", "TexCoord6", "TexCoord7",
    "ATTR0", "ATTR1", "ATTR2", "ATTR3", "ATTR4", "ATTR5", "ATTR6", "ATTR7",
    "ATTR8", "ATTR9", "ATTR10", "ATTR11", "ATTR12", "ATTR13", "ATTR14", "ATTR15"
};

const char* kSCTextureDimensionNames[] =
{
    "",         // kTexDimNone
    "",         // kTexDimAny
    "2D",       // kTexDim2D
    "3D",       // kTexDim3D
    "CUBE",     // kTexDimCUBE
    "2DArray",  // kTexDim2DArray
    "CubeArray",// kTexDimCubeArray
};
CompileTimeAssertArraySize(kSCTextureDimensionNames, kTexDimCount);
