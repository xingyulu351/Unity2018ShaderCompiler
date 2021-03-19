using Boo.Lang.Compiler.Ast;
using Boo.Lang.Compiler.Steps;
using System;

namespace UnityExampleConverter
{
    [Serializable]
    public class RenameArrayDeclaration : AbstractTransformerCompilerStep
    {
        public override void Run()
        {
            Visit(CompileUnit);
        }

        public override void OnMethodInvocationExpression(MethodInvocationExpression node)
        {
            if (node == null || !(node.Target is GenericReferenceExpression)) return;
            var genericReferenceExpression = (GenericReferenceExpression)node.Target;
            if (!(genericReferenceExpression.Target is MemberReferenceExpression)) return;
            var memberReferenceExpression = (MemberReferenceExpression)genericReferenceExpression.Target;
            if (!(memberReferenceExpression.Target is MemberReferenceExpression)) return;
            var memberReferenceExpression2 = (MemberReferenceExpression)memberReferenceExpression.Target;
            if (!(memberReferenceExpression2.Target is MemberReferenceExpression)) return;
            var memberReferenceExpression3 = (MemberReferenceExpression)memberReferenceExpression2.Target;
            if (!(memberReferenceExpression3.Target is ReferenceExpression)) return;
            var referenceExpression = (ReferenceExpression)memberReferenceExpression3.Target;
            if (referenceExpression.Name != "Boo" || memberReferenceExpression3.Name != "Lang" ||
                memberReferenceExpression2.Name != "Builtins" || memberReferenceExpression.Name != "array" ||
                1 != genericReferenceExpression.GenericArguments.Count) return;
            var node2 = genericReferenceExpression.GenericArguments[0];
            if (1 != node.Arguments.Count || !(node.Arguments[0] is CastExpression)) return;
            var castExpression = (CastExpression)node.Arguments[0];
            var target = castExpression.Target;
            if (!(castExpression.Type is SimpleTypeReference)) return;
            var simpleTypeReference = (SimpleTypeReference)castExpression.Type;
            if (simpleTypeReference.Name != "int") return;
            var methodInvocationExpression = new MethodInvocationExpression(LexicalInfo.Empty);
            var arg_255_0 = methodInvocationExpression;
            var genericReferenceExpression2 = new GenericReferenceExpression(LexicalInfo.Empty);
            var arg_226_0 = genericReferenceExpression2;
            var referenceExpression2 = new ReferenceExpression(LexicalInfo.Empty) {Name = "array"};
            arg_226_0.Target = referenceExpression2;
            genericReferenceExpression2.GenericArguments = TypeReferenceCollection.FromArray(new TypeReference[]
            {
                TypeReference.Lift(node2)
            });
            arg_255_0.Target = genericReferenceExpression2;
            methodInvocationExpression.Arguments = ExpressionCollection.FromArray(new Expression[]
            {
                Expression.Lift(target)
            });
            ReplaceCurrentNode(methodInvocationExpression);
        }
    }
}
