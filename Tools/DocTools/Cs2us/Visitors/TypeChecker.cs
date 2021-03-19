using ICSharpCode.NRefactory.CSharp;

namespace Cs2us.Visitors
{
    internal class TypeChecker : BaseAstVisitor<bool>
    {
        private string typeName;

        public TypeChecker(string typeName)
        {
            this.typeName = typeName;
        }

        public override bool VisitSimpleType(SimpleType simpleType)
        {
            return simpleType.Identifier == typeName;
        }
    }
}
