using Cs2us.Visitors;
using ICSharpCode.NRefactory.CSharp;

namespace Cs2us
{
    internal class TypeExtractor : BaseAstVisitor<string>
    {
        public override string VisitSimpleType(SimpleType simpleType)
        {
            return simpleType.Identifier;
        }
    }
}
