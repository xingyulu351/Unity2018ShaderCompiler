#pragma once

#include "ShaderCompilerTypes.h"

inline bool SupportsInstancing(ShaderCompilerPlatform compiler)
{
    switch (compiler)
    {
        case kShaderCompPlatformD3D11:
        case kShaderCompPlatformGLES3Plus:
        case kShaderCompPlatformOpenGLCore:
        case kShaderCompPlatformPS4:
        case kShaderCompPlatformMetal:
        case kShaderCompPlatformVulkan:
        case kShaderCompPlatformXboxOne:
        case kShaderCompPlatformXboxOneD3D12:
        case kShaderCompPlatformSwitch:
            return true;
        default:
            return false;
    }
}

inline bool SupportsSinglePassStereo(ShaderCompilerPlatform compiler)
{
    switch (compiler)
    {
        case kShaderCompPlatformD3D11:
        case kShaderCompPlatformPS4:
        case kShaderCompPlatformGLES3Plus:
        case kShaderCompPlatformOpenGLCore:
        case kShaderCompPlatformMetal:
        case kShaderCompPlatformVulkan:
            return true;
        default:
            return false;
    }
}

inline bool SupportsStereoInstancing(ShaderCompilerPlatform compiler)
{
    switch (compiler)
    {
        case kShaderCompPlatformD3D11:
        case kShaderCompPlatformPS4:
        case kShaderCompPlatformOpenGLCore:
        case kShaderCompPlatformGLES3Plus:
            // Metal technically supports this, but layered rendering has a limitation
            // with MSAA, so the preferred optimization path is to go with instancing
            // against double-wide RT's on Metal platform until the MSAA limitation
            // is fixed by Apple.
            // case kShaderCompPlatformMetal:
            return true;

        default:
            return false;
    }
}

inline bool SupportsStereoMultiview(ShaderCompilerPlatform compiler)
{
    return (compiler == kShaderCompPlatformGLES3Plus);
}
