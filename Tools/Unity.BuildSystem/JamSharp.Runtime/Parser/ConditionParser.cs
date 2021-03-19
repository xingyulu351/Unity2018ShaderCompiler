using System;
using System.Collections.Generic;
using System.Linq;
using Unity.BuildTools;
using JamSharp.Runtime;

namespace JamSharp.Runtime.Parser
{
    partial class Parser
    {
        public Expression ParseCondition()
        {
            var stack = new Stack<IEvaluationStackNode>();
            Operator? mostRecentOperator = null;

            while (true)
            {
                var nextToken = ScanResult.Peek();
                if (nextToken.literal == "(")
                {
                    stack.Push(new ParenthesisOpenNode());
                    ScanResult.Next();
                    continue;
                }

                if (nextToken.tokenType == TokenType.Not)
                {
                    stack.Push(new OperatorNode(Operator.Not));
                    ScanResult.Next();
                    mostRecentOperator = Operator.Not;
                    continue;
                }

                stack.Push(ParseValue(mostRecentOperator));

                nextToken = ScanResult.Peek();
                var nextTokenType = nextToken.tokenType;

                if (nextToken.tokenType == TokenType.ParenthesisClose)
                {
                    CollapseStack(stack);
                    ScanResult.Next();
                    nextTokenType = ScanResult.Peek().tokenType;
                }

                if (!Parser.IsBinaryOperator(nextTokenType))
                {
                    CollapseStack(stack);
                    return ((ValueNode)stack.Single()).Expressions.Single();
                }

                ScanResult.Next();

                var nextOperator = Parser.OperatorFor(nextTokenType);
                if (mostRecentOperator.HasValue)
                {
                    var nextPrecedence = PrecedenceFor(nextOperator);
                    var mostRecentPrecedence = PrecedenceFor(mostRecentOperator.Value);
                    if (nextPrecedence < mostRecentPrecedence)
                        CollapseStack(stack);
                }

                stack.Push(new OperatorNode(nextOperator));
                mostRecentOperator = nextOperator;
            }
        }

        void CollapseStack(Stack<IEvaluationStackNode> stack)
        {
            while (stack.Count() > 1)
            {
                if (stack.Peek() is ParenthesisOpenNode)
                {
                    stack.Pop();
                    return;
                }

                var rightNode = (ValueNode)stack.Pop();
                var evaluationStackNode = stack.Pop();
                if (evaluationStackNode is ParenthesisOpenNode)
                {
                    stack.Push(rightNode);
                    return;
                }

                var @operator = (OperatorNode)evaluationStackNode;
                if (IsUnaryOperator(@operator.Operator))
                {
                    stack.Push(new ValueNode(new NodeList<Expression> { new NotOperatorExpression { Expression = rightNode.Expressions.Single() } }));
                    continue;
                }
                var left = ((ValueNode)stack.Pop()).Expressions;

                var boe = new BinaryOperatorExpression { Left = left.Single(), Operator = @operator.Operator, Right = new NodeList<Node>(rightNode.Expressions) };
                stack.Push(new ValueNode(new NodeList<Expression> { boe }));
            }
        }

        static bool IsUnaryOperator(Operator @operator)
        {
            return @operator == Operator.Not;
        }

        ValueNode ParseValue(Operator? @operator)
        {
            if (@operator == Operator.In)
                return new ValueNode(new NodeList<Expression>(Enumerable.OfType<Expression>(ParseExpressionList())));

            var expression = ParseExpression();
            if (expression == null)
                throw new Exception();
            return new ValueNode(new NodeList<Expression> { expression });
        }

        static readonly IEnumerable<Operator> k_OperatorPrecedence = new[] { Operator.Or, Operator.And, Operator.Subtract, Operator.AssignmentIfEmpty, Operator.Append, Operator.Not, Operator.In, Operator.Assignment, Operator.NotEqual, Operator.GreaterThan, Operator.LessThan };

        static int PrecedenceFor(Operator o)
        {
            int precedenceFor = k_OperatorPrecedence.IndexOf(o);
            if (precedenceFor == -1)
                throw new NotSupportedException();
            return precedenceFor;
        }

        interface IEvaluationStackNode {}

        class ValueNode : IEvaluationStackNode
        {
            public readonly NodeList<Expression> Expressions;

            public ValueNode(NodeList<Expression> expressions)
            {
                Expressions = expressions;
            }
        }

        class ParenthesisOpenNode : IEvaluationStackNode {}

        class OperatorNode : IEvaluationStackNode
        {
            public readonly Operator Operator;

            public OperatorNode(Operator @operator)
            {
                Operator = @operator;
            }
        }
    }
}
