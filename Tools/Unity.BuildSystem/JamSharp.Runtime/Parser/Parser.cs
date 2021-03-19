using System;
using System.Collections.Generic;
using System.Linq;
using Unity.BuildTools;
using JamSharp.Runtime;

namespace JamSharp.Runtime.Parser
{
    public enum ParserMode
    {
        Normal,
        QuotesAsLiteral
    }
    public partial class Parser
    {
        static readonly Dictionary<TokenType, Operator> _binaryOperators = new Dictionary<TokenType, Operator>
        {
            {TokenType.AppendOperator, Operator.Append},
            {TokenType.Assignment, Operator.Assignment},
            {TokenType.SubtractOperator, Operator.Subtract},
            {TokenType.In, Operator.In},
            {TokenType.AssignmentIfEmpty, Operator.AssignmentIfEmpty},
            {TokenType.And, Operator.And},
            {TokenType.Or, Operator.Or},
            {TokenType.NotEqual, Operator.NotEqual},
            {TokenType.GreaterThan, Operator.GreaterThan},
            {TokenType.LessThan, Operator.LessThan},
            {TokenType.GreaterThanOrEqual, Operator.GreaterThanOrEqual},
            {TokenType.LessThanOrEqual, Operator.LessThanOrEqual}
        };

        public ScanResult ScanResult { get; }

        public Parser(string input, ParserMode parserMode = ParserMode.Normal)
        {
            ScanResult = new Scanner(input, parserMode).Scan();
        }

        public TopLevel ParseTopLevel()
        {
            try
            {
                var result = new TopLevel { Statements = new NodeList<Node>() };

                while (true)
                {
                    var token = ScanResult.Peek(false);

                    switch (token.tokenType)
                    {
                        case TokenType.Comment:
                            result.Statements.Add(ParseComment());
                            break;
                        case TokenType.WhiteSpace:
                            ScanResult.Next(false);
                            break;
                        default:
                            var statement = ParseStatement();

                            if (statement == null)
                                return result;
                            result.Statements.Add(statement);
                            break;
                    }
                }
            }
            catch (Exception)
            {
                Console.WriteLine("Parsing failed. Previous 50 tokens:");
                var cursor = ScanResult.GetCursor();
                var firstToken = Math.Max(0, cursor - 50);
                ScanResult.SetCursor(firstToken);
                for (int i = firstToken; i != cursor; i++)
                {
                    var token = ScanResult.Next();
                    Console.WriteLine("cursor={0} type={1} literal='{2}'", i, token.tokenType, token.literal);
                }
                throw;
            }
        }

        public NodeList<Node> ParseExpressionList()
        {
            var expressions = new List<Node>();
            while (true)
            {
                var token = ScanResult.Peek();
                if (token.tokenType == TokenType.Comment)
                    expressions.Add(ParseComment());
                else
                {
                    var expression = ParseExpression();
                    if (expression == null)
                        break;
                    expressions.Add(expression);
                }
            }
            return expressions.ToNodeList();
        }

        public Statement ParseStatement()
        {
            var scanToken = ScanResult.Peek();

            switch (scanToken.tokenType)
            {
                case TokenType.EOF:
                case TokenType.Case:
                case TokenType.AccoladeClose:
                    return null;
                case TokenType.If:
                    return ParseIfStatement();
                case TokenType.AccoladeOpen:
                    return ParseBlockStatement();
                case TokenType.Rule:
                    return ParseRuleDeclarationStatement();
                case TokenType.Actions:
                    return ParseActionsDeclarationStatement();
                case TokenType.Return:
                    return ParseReturnStatement();
                case TokenType.Literal:
                case TokenType.VariableDereferencerOpen:
                case TokenType.BracketOpen:
                    return ParseAssignmentOrExpressionStatement();
                case TokenType.On:
                    return ParseOnStatement();
                case TokenType.While:
                    return ParseWhileStatement();
                case TokenType.Include:
                    return ParseIncludeStatement();
                case TokenType.For:
                    return ParseForStatement();
                case TokenType.Break:
                    ScanResult.Next();
                    ScanResult.Next().Is(TokenType.Terminator);
                    return new BreakStatement();
                case TokenType.Continue:
                    ScanResult.Next();
                    ScanResult.Next().Is(TokenType.Terminator);
                    return new ContinueStatement();
                case TokenType.Switch:
                    return ParseSwitchStatement();
                case TokenType.Local:
                    return ParseLocalStatement();
                default:
                    throw new ParsingException("Unexpected token: " + scanToken.tokenType);
            }
        }

        IncludeStatement ParseIncludeStatement()
        {
            ScanResult.Next().Is(TokenType.Include);
            var expression = ParseExpression();

            ScanResult.Next().Is(TokenType.Terminator);
            return new IncludeStatement { Expression = expression };
        }

        LocalStatement ParseLocalStatement()
        {
            ScanResult.Next().Is(TokenType.Local);
            var literalExpression = ParseExpression().To<LiteralExpression>();

            var next = ScanResult.Next();
            NodeList<Node> value = new NodeList<Node>();
            if (next.tokenType != TokenType.Terminator)
            {
                next.Is(TokenType.Assignment);
                value = ParseExpressionList();
                ScanResult.Next().Is(TokenType.Terminator);
            }
            return new LocalStatement { Variable = literalExpression, Value = value };
        }

        SwitchStatement ParseSwitchStatement()
        {
            ScanResult.Next().Is(TokenType.Switch);

            var result = new SwitchStatement { Variable = ParseExpression() };

            ScanResult.Next().Is(TokenType.AccoladeOpen);

            var switchCases = new NodeList<SwitchCase>();
            while (true)
            {
                var next = ScanResult.Next();
                if (next.tokenType == TokenType.AccoladeClose)
                    break;
                if (next.tokenType != TokenType.Case)
                    throw new ParsingException();

                var switchCase = new SwitchCase { CaseExpression = ParseExpression().To<LiteralExpression>() };
                ScanResult.Next().Is(TokenType.Colon);

                switchCase.Statements = ParseStatementsAndComments();
                switchCases.Add(switchCase);
            }

            result.Cases = switchCases;
            return result;
        }

        NodeList<Node> ParseStatementsAndComments()
        {
            var statements = new NodeList<Node>();
            while (true)
            {
                var token = ScanResult.Peek(false);
                switch (token.tokenType)
                {
                    case TokenType.Comment:
                        statements.Add(ParseComment());
                        break;
                    case TokenType.WhiteSpace:
                        ScanResult.Next(false);
                        break;
                    default:
                        var statement = ParseStatement();
                        if (statement == null)
                            return statements;
                        statements.Add(statement);
                        break;
                }
            }
        }

        Statement ParseForStatement()
        {
            ScanResult.Next().Is(TokenType.For);
            var loopVariable = ParseExpression();
            ScanResult.Next().Is(TokenType.In);

            return new ForStatement { LoopVariable = loopVariable.To<LiteralExpression>(), List = ParseExpressionList(), Body = ParseBlockStatement() };
        }

        OnStatement ParseOnStatement()
        {
            ScanResult.Next().Is(TokenType.On);
            return new OnStatement { Target = ParseExpression(), Body = ParseStatement() };
        }

        WhileStatement ParseWhileStatement()
        {
            ScanResult.Next().Is(TokenType.While);
            return new WhileStatement { Condition = ParseCondition(), Body = ParseBlockStatement() };
        }

        Statement ParseAssignmentOrExpressionStatement()
        {
            if (IsNextTokenAssignment())
            {
                var leftSideOfAssignment = ParseLeftSideOfAssignment();

                var assignmentToken = ScanResult.Next();
                var right = ParseExpressionList();
                ScanResult.Next().Is(TokenType.Terminator);

                return new AssignmentStatement { Left = leftSideOfAssignment, Right = right, Operator = OperatorFor(assignmentToken.tokenType) };
            }

            var invocationExpression = ParseInvocationExpression();

            ScanResult.Next().Is(TokenType.Terminator);

            return new ExpressionStatement { Expression = invocationExpression };
        }

        InvocationExpression ParseInvocationExpression()
        {
            var hasBracketSyntax = ScanResult.Peek().tokenType == TokenType.BracketOpen;
            if (hasBracketSyntax)
                ScanResult.Next();

            InvocationExpression invocationExpression;
            if (ScanResult.Peek().tokenType == TokenType.On)
            {
                ScanResult.Next();
                var target = ParseExpression();
                var nestedInvocation = ParseInvocationExpression();
                invocationExpression = new InvocationExpression { RuleExpression = nestedInvocation.RuleExpression, Arguments = nestedInvocation.Arguments, OnTarget = target };
            }
            else
                invocationExpression = new InvocationExpression { RuleExpression = ParseExpression(), Arguments = ParseArgumentList() };

            if (hasBracketSyntax)
                ScanResult.Next().Is(TokenType.BracketClose);

            return invocationExpression;
        }

        Expression ParseLeftSideOfAssignment()
        {
            var cursor = ScanResult.GetCursor();
            ParseExpression();
            var nextScanToken = ScanResult.Next();
            ScanResult.SetCursor(cursor);

            if (nextScanToken.tokenType == TokenType.On)
            {
                var variable = ParseExpression();
                ScanResult.Next().Is(TokenType.On);
                var targets = ParseExpressionList();
                return new VariableOnTargetExpression { Variable = variable, Targets = targets };
            }

            if (nextScanToken.tokenType == TokenType.Assignment || nextScanToken.tokenType == TokenType.AppendOperator || nextScanToken.tokenType == TokenType.SubtractOperator || nextScanToken.tokenType == TokenType.AssignmentIfEmpty)
            {
                return ParseExpression();
            }

            throw new ParsingException();
        }

        bool IsNextTokenAssignment()
        {
            var cursor = ScanResult.GetCursor();
            /*var leftSide = */
            ParseExpression();
            var nextScanToken = ScanResult.Next();
            ScanResult.SetCursor(cursor);

            switch (nextScanToken.tokenType)
            {
                case TokenType.Assignment:
                case TokenType.AppendOperator:
                case TokenType.SubtractOperator:
                case TokenType.On:
                case TokenType.AssignmentIfEmpty:

                    return true;
            }
            return false;
        }

        Statement ParseReturnStatement()
        {
            ScanResult.Next().Is(TokenType.Return);
            var returnExpression = ParseExpressionList();
            ScanResult.Next().Is(TokenType.Terminator);

            return new ReturnStatement { ReturnExpression = returnExpression };
        }

        Statement ParseActionsDeclarationStatement()
        {
            ScanResult.Next().Is(TokenType.Actions);
            var expressionList = ParseExpressionList();

            var accoladeOpen = ScanResult.Next();
            if (accoladeOpen.tokenType != TokenType.AccoladeOpen)
                throw new ParsingException();

            var result = new ActionsDeclarationStatement
            {
                Name = expressionList.Last().To<LiteralExpression>().Value,
                Modifiers = expressionList.Take(expressionList.Length - 1).OfType<Expression>().ToNodeList(),
                Actions = ScanResult.Next().literal
            };

            ScanResult.Next().Is(TokenType.AccoladeClose);
            return result;
        }

        Statement ParseRuleDeclarationStatement()
        {
            ScanResult.Next().Is(TokenType.Rule);
            var ruleName = ParseExpression();
            var arguments = ParseArgumentList().ToArray();
            var body = ParseStatement();

            var ruleNameStr = ruleName.To<LiteralExpression>().Value;
            return new RuleDeclarationStatement
            {
                Name = ruleNameStr,
                Arguments = arguments.SelectMany(ele => ele.Cast<LiteralExpression>()).Select(le => le.Value).ToArray(),
                Body = (BlockStatement)body
            };
        }

        BlockStatement ParseBlockStatement()
        {
            ScanResult.Next().Is(TokenType.AccoladeOpen);
            var statements = new List<Node>();
            while (true)
            {
                var peek = ScanResult.Peek(false);

                switch (peek.tokenType)
                {
                    case TokenType.AccoladeClose:
                        ScanResult.Next(false);
                        return new BlockStatement { Statements = statements.ToNodeList() };
                    case TokenType.Comment:
                        statements.Add(ParseComment());
                        break;
                    case TokenType.WhiteSpace:
                        ScanResult.Next(false);
                        continue;
                    default:
                        statements.Add(ParseStatement());
                        break;
                }
            }
        }

        Comment ParseComment()
        {
            var token = ScanResult.Next();
            return new Comment { Content = token.literal };
        }

        Statement ParseIfStatement()
        {
            ScanResult.Next().Is(TokenType.If);

            var expression = ParseCondition();

            Comment comment = null;
            var peek = ScanResult.Peek();
            if (peek.tokenType == TokenType.Comment)
            {
                comment = ParseComment();
            }

            var ifStatement = new IfStatement { Condition = expression, Body = ParseBlockStatement() };

            if (comment != null)
                ifStatement.Body.Statements.InsertAt(0, comment);

            peek = ScanResult.Peek();
            if (peek.tokenType == TokenType.Else)
            {
                ScanResult.Next();
                ifStatement.Else = ParseStatement();
            }

            return ifStatement;
        }

        public Expression ParseExpression()
        {
            var scanToken = ScanResult.Peek();
            var tokenType = scanToken.tokenType;
            if (IsBinaryOperator(tokenType) && tokenType != TokenType.In)
                return null;

            switch (tokenType)
            {
                case TokenType.EOF:
                case TokenType.Colon:
                case TokenType.Terminator:
                case TokenType.ParenthesisClose:
                case TokenType.BracketClose:
                    return null;
                case TokenType.VariableDereferencerOpen:
                case TokenType.LiteralExpansionOpen:
                    return ScanForCombineExpression(ParseExpansionStyleExpression());
                case TokenType.AccoladeOpen:
                    return null;
                case TokenType.BracketOpen:
                    return ParseInvocationExpression();
                case TokenType.Not:
                    ScanResult.Next();
                    var expression = ParseExpression();
                    if (expression is BinaryOperatorExpression)
                        throw new ParsingException("see IfStatementWithNegationAndRightSide test");
                    return new NotOperatorExpression { Expression = expression };
                default:
                    return ScanForCombineExpression(new LiteralExpression { Value = ScanResult.Next().literal });
            }
        }

        public ExpansionStyleExpression ParseExpansionStyleExpression()
        {
            var token = ScanResult.Next();

            var result = token.tokenType == TokenType.VariableDereferencerOpen
                ? (ExpansionStyleExpression) new VariableDereferenceExpression()
                : new LiteralExpansionExpression();

            result.VariableExpression = ParseExpression();

            var next = ScanResult.Next();

            if (next.tokenType == TokenType.BracketOpen)
            {
                result.IndexerExpression = ParseExpression();
                if (ScanResult.Next().tokenType != TokenType.BracketClose)
                    throw new ParsingException("Expected bracket close while parsing variable dereference expressions' indexer");
                next = ScanResult.Next();
            }

            var modifiers = new NodeList<VariableDereferenceModifier>();

            if (next.tokenType == TokenType.Colon)
            {
                while (true)
                {
                    var modifier = ScanResult.Next();
                    if (modifier.tokenType == TokenType.Colon)
                        continue;

                    if (modifier.tokenType == TokenType.ParenthesisClose)
                    {
                        next = modifier;
                        break;
                    }

                    modifier.Is(TokenType.VariableExpansionModifier);

                    var peek = ScanResult.Peek();
                    Expression modifierValue = null;
                    if (peek.tokenType == TokenType.Assignment)
                    {
                        ScanResult.Next();
                        modifierValue = ParseExpression() ?? new EmptyExpression();
                    }

                    modifiers.Add(new VariableDereferenceModifier { Command = modifier.literal[0], Value = modifierValue });
                }
            }
            result.Modifiers = modifiers;

            next.Is(TokenType.ParenthesisClose);

            return result;
        }

        public static bool IsBinaryOperator(TokenType tokenType)
        {
            return _binaryOperators.ContainsKey(tokenType);
        }

        static Operator OperatorFor(TokenType tokenType)
        {
            return _binaryOperators[tokenType];
        }

        Expression ScanForCombineExpression(Expression firstExpression)
        {
            var combined = new NodeList<Expression>();

            if (firstExpression is CombineExpression)
                foreach (var node in ((CombineExpression)firstExpression).Elements)
                    combined.Add(node);
            else
                combined.Add(firstExpression);

            while (true)
            {
                var peek = ScanResult.Peek(false);
                if (peek.tokenType == TokenType.EOF || (peek.tokenType != TokenType.Literal &&
                                                        peek.tokenType != TokenType.VariableDereferencerOpen &&
                                                        peek.tokenType != TokenType.LiteralExpansionOpen))
                {
                    if (combined.Length > 1)
                        return new CombineExpression { Elements = combined };
                    else
                        return firstExpression;
                }

                var tail = ParseExpression();
                var combineExpressionTail = tail as CombineExpression;

                if (combineExpressionTail != null)
                    foreach (var node in combineExpressionTail.Elements)
                        combined.Add(node);
                else
                    combined.Add(tail);
            }
        }

        NodeList<NodeList<Node>> ParseArgumentList()
        {
            var expressionLists = new NodeList<NodeList<Node>>();
            while (true)
            {
                expressionLists.Add(ParseExpressionList());

                var nextToken = ScanResult.Peek();
                if (nextToken.tokenType == TokenType.Colon)
                {
                    ScanResult.Next();
                    continue;
                }

                break;
            }

            return expressionLists;
        }
    }

    public class ParsingException : Exception
    {
        public ParsingException(string message)
            : base(message) {}

        public ParsingException() {}
    }
}
