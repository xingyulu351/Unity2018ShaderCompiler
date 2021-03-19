using System;
using ICSharpCode.NRefactory.CSharp;
using ICSharpCode.NRefactory.TypeSystem;

namespace Cs2us.Visitors
{
    public class PrimitiveTypeExtractor : DepthFirstAstVisitor<string>
    {
        public override string VisitPrimitiveType(PrimitiveType primitiveType)
        {
            switch (primitiveType.KnownTypeCode)
            {
                case KnownTypeCode.Object:
                    return "Object";
                case KnownTypeCode.String:
                    return "String";
                case KnownTypeCode.Int32:
                    return "int";
                case KnownTypeCode.Int16:
                    return "short";
                case KnownTypeCode.Single:
                    return "float";
                case KnownTypeCode.Double:
                    return "double";
                case KnownTypeCode.Char:
                    return "char";
                case KnownTypeCode.Boolean:
                    return "boolean";
                case KnownTypeCode.Void:
                    return "void";
                default:
                    throw new NotImplementedException("type " + primitiveType.KnownTypeCode + " not supported");
            }
        }
    }
}
