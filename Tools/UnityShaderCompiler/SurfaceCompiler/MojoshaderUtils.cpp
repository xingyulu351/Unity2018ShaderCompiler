#include "UnityPrefix.h"
#include "MojoshaderUtils.h"

static inline bool IsUnaryOp(MOJOSHADER_astNodeType op)
{
    return op > MOJOSHADER_AST_OP_START_RANGE_UNARY && op < MOJOSHADER_AST_OP_END_RANGE_UNARY;
}

static inline bool IsBinaryOp(MOJOSHADER_astNodeType op)
{
    return op > MOJOSHADER_AST_OP_START_RANGE_BINARY && op < MOJOSHADER_AST_OP_END_RANGE_BINARY;
}

bool EvalASTExpression(const MOJOSHADER_astNode* expr, EvalASTData& data)
{
    const MOJOSHADER_astNodeType op = expr->ast.type;

    if (IsUnaryOp(op))
    {
        if (!EvalASTExpression((const MOJOSHADER_astNode*)expr->unary.operand, data))
            return false;

        if (data.isFloat)
        {
            // floats
            switch (op)
            {
                case MOJOSHADER_AST_OP_NEGATE:      data.value.f = -data.value.f; return true;
                case MOJOSHADER_AST_OP_NOT:         data.value.f = !data.value.f;  return true;
                case MOJOSHADER_AST_OP_COMPLEMENT:  return false; // can't do complement on floats
                case MOJOSHADER_AST_OP_CAST:
                    if (expr->unary.datatype->type == MOJOSHADER_AST_DATATYPE_INT)
                    {
                        data.isFloat = false;
                        data.value.i = (SInt64)data.value.f;
                        return true;
                    }
                    if (expr->unary.datatype->type == MOJOSHADER_AST_DATATYPE_FLOAT)
                        return true; // nothing to do
                    return false; // not implemented
                default: break;
            }
        }
        else
        {
            // integers
            switch (op)
            {
                case MOJOSHADER_AST_OP_NEGATE:      data.value.i = -data.value.i; return true;
                case MOJOSHADER_AST_OP_NOT:         data.value.i = !data.value.i; return true;
                case MOJOSHADER_AST_OP_COMPLEMENT:  data.value.i = ~data.value.i; return true;
                case MOJOSHADER_AST_OP_CAST:
                    if (expr->unary.datatype->type == MOJOSHADER_AST_DATATYPE_FLOAT)
                    {
                        data.isFloat = true;
                        data.value.f = data.value.i;
                        return true;
                    }
                    if (expr->unary.datatype->type == MOJOSHADER_AST_DATATYPE_INT)
                        return true; // nothing to do
                    return false; // not implemented
                default: break;
            }
        }
        return false; // should not happen; fail
    }
    else if (IsBinaryOp(op))
    {
        EvalASTData data2;
        if (!EvalASTExpression((const MOJOSHADER_astNode*)expr->binary.left, data) ||
            !EvalASTExpression((const MOJOSHADER_astNode*)expr->binary.right, data2))
        {
            return false;
        }

        // upgrade to float if either side is float
        if (data.isFloat || data2.isFloat)
        {
            if (!data.isFloat)
                data.value.f = (double)data.value.i;
            if (!data2.isFloat)
                data2.value.f = (double)data2.value.i;
            data.isFloat = data2.isFloat = true;
        }

        // these binary operations are not supported in constant expression
        switch (op)
        {
            case MOJOSHADER_AST_OP_COMMA:
                return false;
            case MOJOSHADER_AST_OP_ASSIGN:
            case MOJOSHADER_AST_OP_MULASSIGN:
            case MOJOSHADER_AST_OP_DIVASSIGN:
            case MOJOSHADER_AST_OP_MODASSIGN:
            case MOJOSHADER_AST_OP_ADDASSIGN:
            case MOJOSHADER_AST_OP_SUBASSIGN:
            case MOJOSHADER_AST_OP_LSHIFTASSIGN:
            case MOJOSHADER_AST_OP_RSHIFTASSIGN:
            case MOJOSHADER_AST_OP_ANDASSIGN:
            case MOJOSHADER_AST_OP_XORASSIGN:
            case MOJOSHADER_AST_OP_ORASSIGN:
                return false;
            default: break;
        }

        if (data.isFloat)
        {
            // float path
            switch (op)
            {
                case MOJOSHADER_AST_OP_MULTIPLY:            data.value.f *= data2.value.f; return true;
                case MOJOSHADER_AST_OP_DIVIDE:              data.value.f /= data2.value.f; return true;
                case MOJOSHADER_AST_OP_ADD:                 data.value.f += data2.value.f; return true;
                case MOJOSHADER_AST_OP_SUBTRACT:            data.value.f -= data2.value.f; return true;
                case MOJOSHADER_AST_OP_LESSTHAN:            data.isFloat = false; data.value.i = data.value.f < data2.value.f; return true;
                case MOJOSHADER_AST_OP_GREATERTHAN:         data.isFloat = false; data.value.i = data.value.f > data2.value.f; return true;
                case MOJOSHADER_AST_OP_LESSTHANOREQUAL:     data.isFloat = false; data.value.i = data.value.f <= data2.value.f; return true;
                case MOJOSHADER_AST_OP_GREATERTHANOREQUAL:  data.isFloat = false; data.value.i = data.value.f >= data2.value.f; return true;
                case MOJOSHADER_AST_OP_EQUAL:               data.isFloat = false; data.value.i = data.value.f == data2.value.f; return true;
                case MOJOSHADER_AST_OP_NOTEQUAL:            data.isFloat = false; data.value.i = data.value.f != data2.value.f; return true;
                case MOJOSHADER_AST_OP_LOGICALAND:          data.isFloat = false; data.value.i = data.value.f && data2.value.f; return true;
                case MOJOSHADER_AST_OP_LOGICALOR:           data.isFloat = false; data.value.i = data.value.f || data2.value.f; return true;
                case MOJOSHADER_AST_OP_LSHIFT:
                case MOJOSHADER_AST_OP_RSHIFT:
                case MOJOSHADER_AST_OP_MODULO:
                case MOJOSHADER_AST_OP_BINARYAND:
                case MOJOSHADER_AST_OP_BINARYXOR:
                case MOJOSHADER_AST_OP_BINARYOR:
                    return false; // integer operation on a float
                default: break;
            }
        }
        else
        {
            // integer path
            switch (op)
            {
                case MOJOSHADER_AST_OP_MULTIPLY:            data.value.i *= data2.value.i; return true;
                case MOJOSHADER_AST_OP_DIVIDE:              data.value.i /= data2.value.i; return true;
                case MOJOSHADER_AST_OP_ADD:                 data.value.i += data2.value.i; return true;
                case MOJOSHADER_AST_OP_SUBTRACT:            data.value.i -= data2.value.i; return true;
                case MOJOSHADER_AST_OP_LESSTHAN:            data.value.i = data.value.i < data2.value.i; return true;
                case MOJOSHADER_AST_OP_GREATERTHAN:         data.value.i = data.value.i > data2.value.i; return true;
                case MOJOSHADER_AST_OP_LESSTHANOREQUAL:     data.value.i = data.value.i <= data2.value.i; return true;
                case MOJOSHADER_AST_OP_GREATERTHANOREQUAL:  data.value.i = data.value.i >= data2.value.i; return true;
                case MOJOSHADER_AST_OP_EQUAL:               data.value.i = data.value.i == data2.value.i; return true;
                case MOJOSHADER_AST_OP_NOTEQUAL:            data.value.i = data.value.i != data2.value.i; return true;
                case MOJOSHADER_AST_OP_LOGICALAND:          data.value.i = data.value.i && data2.value.i; return true;
                case MOJOSHADER_AST_OP_LOGICALOR:           data.value.i = data.value.i || data2.value.i; return true;
                case MOJOSHADER_AST_OP_LSHIFT:              data.value.i = data.value.i << data2.value.i; return true;
                case MOJOSHADER_AST_OP_RSHIFT:              data.value.i = data.value.i >> data2.value.i; return true;
                case MOJOSHADER_AST_OP_MODULO:              data.value.i = data.value.i % data2.value.i; return true;
                case MOJOSHADER_AST_OP_BINARYAND:           data.value.i = data.value.i & data2.value.i; return true;
                case MOJOSHADER_AST_OP_BINARYXOR:           data.value.i = data.value.i ^ data2.value.i; return true;
                case MOJOSHADER_AST_OP_BINARYOR:            data.value.i = data.value.i | data2.value.i; return true;
                default: break;
            }
        }
        return false; // should not happen, fail
    }
    else if (op == MOJOSHADER_AST_OP_CONDITIONAL) // the only ternary op we have
    {
        Assert(op == MOJOSHADER_AST_OP_CONDITIONAL);
        EvalASTData data2;
        EvalASTData data3;

        if (!EvalASTExpression((const MOJOSHADER_astNode*)expr->ternary.left, data) ||
            !EvalASTExpression((const MOJOSHADER_astNode*)expr->ternary.center, data2) ||
            !EvalASTExpression((const MOJOSHADER_astNode*)expr->ternary.right, data3))
        {
            return false;
        }

        // first operand should be bool
        if (data.isFloat)
        {
            data.isFloat = false;
            data.value.i = (SInt64)data3.value.f;
        }

        // upgrade to float if either side is float
        if (data2.isFloat || data3.isFloat)
        {
            if (!data2.isFloat)
                data2.value.f = (double)data2.value.i;
            if (!data3.isFloat)
                data3.value.f = (double)data3.value.i;
            data2.isFloat = data3.isFloat = 1;
        }

        data.isFloat = data2.isFloat;
        if (data.isFloat)
            data.value.f = data.value.i ? data2.value.f : data3.value.f;
        else
            data.value.i = data.value.i ? data2.value.i : data3.value.i;
        return true;
    }
    else
    {
        // it could be just a literal value
        switch (op)
        {
            case MOJOSHADER_AST_OP_INT_LITERAL:     data.isFloat = false; data.value.i = expr->intliteral.value; return true;
            case MOJOSHADER_AST_OP_FLOAT_LITERAL:   data.isFloat = true; data.value.f = expr->floatliteral.value; return true;
            case MOJOSHADER_AST_OP_BOOLEAN_LITERAL: data.isFloat = false; data.value.i = expr->boolliteral.value ? 1 : 0; return true;
            default: break;
        }
    }

    return false; // not constant, or unhandled
}

void* MojoShaderAllocator::Alloc(int bytes, void *data)
{
    MojoShaderAllocator* self = (MojoShaderAllocator*)data;
    return self->m_Alloc.allocate(bytes, 8);
}

void MojoShaderAllocator::Free(void *ptr, void *data)
{
    MojoShaderAllocator* self = (MojoShaderAllocator*)data;
    self->m_Alloc.deallocate(ptr);
}
