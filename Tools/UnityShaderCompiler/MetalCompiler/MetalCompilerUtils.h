#pragma once

#include "Tools/UnityShaderCompiler/ShaderCompilerTypes.h"
#include "Runtime/GfxDevice/GfxDeviceTypes.h"
#include "Runtime/GfxDevice/metal/MetalShaderBlobHeader.h"

#include <string>
#include <stdint.h>


class ShaderCompilerReflectionReport;
class ShaderImportErrors;

// uniforms: for now we have just one constant buffer ($Globals)
// all uniforms are expected to use one of
// METAL_CONST_VECTOR(type, dim, name)
// METAL_CONST_MATRIX(type, rows, cols, name)
// they will be read in order and layed out in order (we to acknowledge different align rules)

bool ProcessMetalUniforms(core::string& code, const char* globalCbName, ShaderCompilerReflectionReport* reflectionReport, ShaderImportErrors& outErrors);

// vertex input: we didnt yet decided if we should help more with attribute index mapping to ShaderChannel
// METAL_VERTEX_INPUT(index)
// we expect:
// 0 - position
// 1 - normal
// 2 - tangent
// 3 - color
// 4-7 - texcoord

bool ProcessMetalVertexInputs(core::string& code, ShaderCompilerReflectionReport* reflectionReport, ShaderImportErrors& outErrors);

// textures: we strive to be consistent with cg defines: meaning we define sampler and texture in one go
// sampler will be named sampler_<texture name> and have same index as texture
// METAL_TEX_INPUT(metal type, index, name)

bool ProcessMetalTextures(core::string& code, ShaderCompilerReflectionReport* reflectionReport, ShaderImportErrors& outErrors);

// buffers:
// METAL_BUFFER_INPUT(data type, index, name)

bool ProcessMetalBuffers(core::string& code, ShaderCompilerReflectionReport* reflectionReport, ShaderImportErrors& outErrors);

// tiny helper to simplify writing metal compiled shader snippet (as we need to do it in two places)

void WriteShaderSnippetBlob(std::vector<unsigned char>& out, const char* entryPoint, const core::string& code, MetalShaderBlobHeader *metadata = NULL);
