#pragma once

#include "External/ShaderCompilers/mojoshader/mojoshader.h"
#include "Runtime/Allocator/LinearAllocator.h"

struct EvalASTData
{
    EvalASTData() { value.i = 0; isFloat = false; }
    union { SInt64 i; double f; } value;
    bool isFloat;
};

// Evaluate shader AST expression tree for a constant expression value.
// Returns true if this is a constant expression; in that case EvalASTData
// is filled with a value.
bool EvalASTExpression(const MOJOSHADER_astNode* expr, EvalASTData& data);


class MojoShaderAllocator
{
public:
    MojoShaderAllocator() : m_Alloc(64 * 1024, kMemShader) {}
    static void* Alloc(int bytes, void *data);
    static void Free(void *ptr, void *data);
private:
    ForwardLinearAllocator m_Alloc;
};
