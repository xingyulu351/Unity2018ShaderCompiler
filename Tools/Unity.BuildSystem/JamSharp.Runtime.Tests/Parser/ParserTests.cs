using System;
using System.Linq;
using JamSharp.Runtime.Parser;
using NUnit.Framework;

namespace JamSharp.Runtime.Tests.Parser
{
    [TestFixture]
    public class ParserTests
    {
        static void AssertLeftIsA_and_RightIsB(AssignmentStatement assignmentStatement)
        {
            var left = (LiteralExpression)assignmentStatement.Left;
            Assert.AreEqual("a", left.Value);

            var right = assignmentStatement.Right;
            Assert.AreEqual(1, right.Length);
            Assert.AreEqual("b", right[0].To<LiteralExpression>().Value);
        }

        static void AssertLeftIsA_And_RightIsB(IfStatement ifStatement)
        {
            Assert.AreEqual("a", ifStatement.Condition.To<BinaryOperatorExpression>().Left.To<VariableDereferenceExpression>().VariableExpression.To<LiteralExpression>().Value);
            Assert.AreEqual("b", ifStatement.Condition.To<BinaryOperatorExpression>().Right[0].To<VariableDereferenceExpression>().VariableExpression.To<LiteralExpression>().Value);
        }

        static void AssertIsRuleInvocationWithName(Node statementNode, string ruleName)
        {
            var statement = statementNode.To<Statement>();
            Assert.AreEqual(ruleName, statement.To<ExpressionStatement>().Expression.To<InvocationExpression>().RuleExpression.To<LiteralExpression>().Value);
        }

        static T ParseCondition<T>(string jamCode) where T : Expression
        {
            return new Runtime.Parser.Parser(jamCode).ParseCondition().To<T>();
        }

        static TExpected ParseStatement<TExpected>(string jamCode) where TExpected : Statement
        {
            var parser = new Runtime.Parser.Parser(jamCode);
            var node = parser.ParseStatement();
            Assert.IsNull(parser.ParseExpression());
            var returnValue = node as TExpected;
            if (returnValue == null)
                throw new ArgumentException($"Expected parser to return type: {typeof(TExpected).Name} but got {node.GetType().Name}");
            return returnValue;
        }

        static TExpected ParseExpression<TExpected>(string jamCode) where TExpected : Expression
        {
            var parser = new Runtime.Parser.Parser(jamCode);
            var node = parser.ParseExpression();
            Assert.IsNull(parser.ParseExpression());
            var returnValue = node as TExpected;
            if (returnValue == null)
                throw new ArgumentException($"Expected parser to return type: {typeof(TExpected).Name} but got {node.GetType().Name}");
            return returnValue;
        }

        static NodeList<Node> ParseExpressionList(string jamCode)
        {
            var parser = new Runtime.Parser.Parser(jamCode);
            var expressionList = parser.ParseExpressionList();
            Assert.IsNull(parser.ParseExpression());
            return expressionList;
        }

        [Test]
        public void ActionsDeclarationStatement()
        {
            ActionsDeclarationStatement actionsDeclarationStatement = ParseStatement<ActionsDeclarationStatement>(@"
actions response myactionname
{
    echo something
    echo somethingelse
    echo ""harry""
    echo \pieter
}");
            Assert.AreEqual("myactionname", actionsDeclarationStatement.Name);
            CollectionAssert.AreEqual(new[] { "response" }, actionsDeclarationStatement.Modifiers.Cast<LiteralExpression>().Select(le => le.Value));

            Assert.IsTrue(actionsDeclarationStatement.Actions.Contains("echo something"));
            Assert.IsTrue(actionsDeclarationStatement.Actions.Contains("echo somethingelse"));

            //preserve quotes:
            Assert.IsTrue(actionsDeclarationStatement.Actions.Contains("echo \"harry\""));

            //preserve backslash
            Assert.IsTrue(actionsDeclarationStatement.Actions.Contains("echo \\pieter"));
        }

        [Test]
        public void AppendOperator()
        {
            var assignmentStatement = ParseStatement<AssignmentStatement>("a += 3 ;");
            Assert.IsTrue(assignmentStatement.Operator == Runtime.Parser.Operator.Append);
        }

        [Test]
        public void Assignment()
        {
            var assignmentExpression = ParseStatement<AssignmentStatement>("a = b ;");

            AssertLeftIsA_and_RightIsB(assignmentExpression);
            Assert.AreEqual(Runtime.Parser.Operator.Assignment, assignmentExpression.Operator);
        }

        [Test]
        public void AssignmentIfEmpty()
        {
            var assignmentExpression = ParseStatement<AssignmentStatement>("a ?= b ;");

            AssertLeftIsA_and_RightIsB(assignmentExpression);
            Assert.AreEqual(Runtime.Parser.Operator.AssignmentIfEmpty, assignmentExpression.Operator);
        }

        [Test]
        public void AssignToDynamicVar()
        {
            var assignmentStatement = ParseStatement<AssignmentStatement>("$(myvar) = 1 2 3 ;");
            Assert.AreEqual("myvar", assignmentStatement.Left.To<VariableDereferenceExpression>().VariableExpression.To<LiteralExpression>().Value);
        }

        [Test]
        public void BinaryOperatorExpression()
        {
            Assert.AreEqual(Runtime.Parser.Operator.Assignment, ParseCondition<BinaryOperatorExpression>("a = b").Operator);
            Assert.AreEqual(Runtime.Parser.Operator.NotEqual, ParseCondition<BinaryOperatorExpression>("a != b").Operator);
            Assert.AreEqual(Runtime.Parser.Operator.In, ParseCondition<BinaryOperatorExpression>("a in b").Operator);
            Assert.AreEqual(Runtime.Parser.Operator.GreaterThan, ParseCondition<BinaryOperatorExpression>("a > b").Operator);
            Assert.AreEqual(Runtime.Parser.Operator.LessThan, ParseCondition<BinaryOperatorExpression>("a < b").Operator);
            Assert.AreEqual(Runtime.Parser.Operator.GreaterThanOrEqual, ParseCondition<BinaryOperatorExpression>("a >= b").Operator);
            Assert.AreEqual(Runtime.Parser.Operator.LessThanOrEqual, ParseCondition<BinaryOperatorExpression>("a <= b").Operator);
        }

        [Test]
        public void BlockStatement()
        {
            var blockStatement = ParseStatement<BlockStatement>("{ Echo ; }");

            Assert.AreEqual(1, blockStatement.Statements.Length);

            var invocationExpression = blockStatement.Statements[0].To<ExpressionStatement>().Expression.To<InvocationExpression>();
            Assert.AreEqual("Echo", invocationExpression.RuleExpression.To<LiteralExpression>().Value);
        }

        [Test]
        public void BreakStatement()
        {
            ParseStatement<BreakStatement>("break ;");
        }

        [Test]
        public void CombineExpression()
        {
            var combineExpression = ParseExpression<CombineExpression>("$(b)c$(d)");

            Assert.AreEqual(3, combineExpression.Elements.Length);
            Assert.IsTrue(combineExpression.Elements[0] is VariableDereferenceExpression);
            Assert.IsTrue(combineExpression.Elements[1] is LiteralExpression);
            Assert.IsTrue(combineExpression.Elements[2] is VariableDereferenceExpression);
        }

        [Test]
        public void CombineExpressionInQuotes()
        {
            var combineExpression = ParseExpression<CombineExpression>("\"$(b) $(d)\"");

            Assert.AreEqual(5, combineExpression.Elements.Length);
            Assert.IsTrue(combineExpression.Elements[0] is LiteralExpression);
            Assert.IsTrue(combineExpression.Elements[1] is VariableDereferenceExpression);
            Assert.IsTrue(combineExpression.Elements[2] is LiteralExpression);
            Assert.IsTrue(combineExpression.Elements[3] is VariableDereferenceExpression);
            Assert.IsTrue(combineExpression.Elements[4] is LiteralExpression);
        }

        [Test]
        public void LiteralsConnectedWithQuotes()
        {
            var literal = ParseExpression<LiteralExpression>(@"hello"" ""world");
            Assert.AreEqual(@"hello world", literal.Value);
        }

        [Test]
        public void LiteralConnectedWithSpecialChar()
        {
            var literal = ParseExpression<LiteralExpression>(@"""hello"":");
            Assert.AreEqual("hello:", literal.Value);
        }

        [Test]
        public void CombineExpressionQuotesMixedInExpansion()
        {
            var combineExpression = ParseExpression<CombineExpression>(@"$(""hello)""");

            Assert.AreEqual(2, combineExpression.Elements.Length);
            Assert.AreEqual(typeof(VariableDereferenceExpression), combineExpression.Elements[0].GetType());
            Assert.AreEqual(typeof(LiteralExpression), combineExpression.Elements[1].GetType());
            Assert.AreEqual("hello", ((LiteralExpression)((VariableDereferenceExpression)combineExpression.Elements[0]).VariableExpression).Value);
        }

        [Test]
        public void CombineExpressionWithClosingParenthesis()
        {
            var combineExpression = ParseExpression<CombineExpression>("(SOURCES$(colon))");
            Assert.AreEqual("(SOURCES", combineExpression.Elements[0].To<LiteralExpression>().Value);
            Assert.AreEqual("colon", combineExpression.Elements[1].To<VariableDereferenceExpression>().VariableExpression.To<LiteralExpression>().Value);
            Assert.AreEqual(")", combineExpression.Elements[2].To<LiteralExpression>().Value);
        }

        [Test]
        public void CombineExpressionWithClosingParenthesisInVariableDereference()
        {
            var variableDereferenceExpression = ParseExpression<VariableDereferenceExpression>("$($(dollar)(SOURCES$(colon)\\):A)");
            Assert.AreEqual('A', variableDereferenceExpression.Modifiers.Single().Command);
            Assert.AreEqual(")", variableDereferenceExpression.VariableExpression.To<CombineExpression>().Elements.Last().To<LiteralExpression>().Value);
        }

        [Test]
        public void Comment()
        {
            var blockStatement = ParseStatement<BlockStatement>(@"
{
#mycomment
}
");
            var comment = blockStatement.MyChildren.Single().MyChildren.Single().To<Comment>();
            Assert.AreEqual("mycomment", comment.Content);
        }

        [Test]
        public void Complex()
        {
            ParseExpression<Expression>("$(a):$(hello)/");
        }

        [Test]
        public void ContinueStatement()
        {
            ParseStatement<ContinueStatement>("continue ;");
        }

        [Test]
        public void DoubleDereference()
        {
            var deref = ParseExpression<VariableDereferenceExpression>("$($(myvar))");

            Assert.AreEqual("myvar", deref.VariableExpression.To<VariableDereferenceExpression>().VariableExpression.To<LiteralExpression>().Value);
        }

        [Test]
        public void VariableDerefExpressionFollowedByNonJamLanguage()
        {
            var parser = new Runtime.Parser.Parser("$(dingdong:TJ=;) with some shell code that is surely not jam");
            var deref = parser.ParseExpression().To<VariableDereferenceExpression>();
            Assert.AreEqual("dingdong", deref.VariableExpression.To<LiteralExpression>().Value);

            var parserScanResult = parser.ScanResult;
            Assert.AreEqual(16, parserScanResult.ScanTokens[parserScanResult.GetCursor()].textLocation);
        }

        [Test]
        public void DynamicRuleInvocationInBrackets()
        {
            var invocationExpression = ParseExpression<InvocationExpression>("[ $(mydynamicrule) ]");

            Assert.AreEqual("mydynamicrule", invocationExpression.RuleExpression.To<VariableDereferenceExpression>().VariableExpression.To<LiteralExpression>().Value);
        }

        [Test]
        public void EmptyBlockStatement()
        {
            Assert.AreEqual(0, ParseStatement<BlockStatement>("{ }").Statements.Length);
        }

        [Test]
        public void ExpressionListWithComment()
        {
            var assignment = ParseStatement<AssignmentStatement>(@"
harry =
   one #amazing
   two #even better #yeah
;
");
            var values = assignment.Right;

            Assert.AreEqual(4, values.Length);

            values[0].To<LiteralExpression>();
            Assert.AreEqual("amazing", values[1].To<Comment>().Content);
            Assert.AreEqual("even better #yeah", values[3].To<Comment>().Content);
        }

        [Test]
        public void ExpressionListWithOnLiteral()
        {
            //the tricky part here is that we don't misqualify the "on" as a on keyword like in "myvar on target = bla"
            CollectionAssert.AreEqual(new[] { "I", "am", "in", "on", "else", "boat" }, ParseExpressionList("I am in on else boat").Cast<LiteralExpression>().Select(le => le.Value));
        }

        [Test]
        public void ForLoop()
        {
            var forStatement = ParseStatement<ForStatement>("for myvar in $(mylist) { } ");
            Assert.AreEqual("myvar", forStatement.LoopVariable.To<LiteralExpression>().Value);
            Assert.IsTrue(forStatement.List[0] is VariableDereferenceExpression);
            Assert.AreEqual(0, forStatement.Body.Statements.Length);
        }

        [Test]
        public void ForwardToBackslashExpansionModifier()
        {
            var jam = @"$(winRTLegacyDll:\\\\:C) ;";
            ParseExpression<VariableDereferenceExpression>(jam);
        }

        [Test]
        public void IfStatement()
        {
            var ifStatement = ParseStatement<IfStatement>("if $(somevar) { }");
            Assert.AreEqual(0, ifStatement.Body.Statements.Length);

            Assert.AreEqual(ifStatement, ifStatement.Body.Parent);
        }

        [Test]
        public void IfStatementWithAndOperator()
        {
            var ifStatement = ParseStatement<IfStatement>("if $(a) && $(b) { }");
            Assert.AreEqual(Runtime.Parser.Operator.And, ifStatement.Condition.To<BinaryOperatorExpression>().Operator);
            AssertLeftIsA_And_RightIsB(ifStatement);
        }

        [Test]
        public void IfStatementWithBinaryOperatorCondition()
        {
            var ifStatement = ParseStatement<IfStatement>("if $(somevar) = 3 { }");
            Assert.AreEqual(Runtime.Parser.Operator.Assignment, ifStatement.Condition.To<BinaryOperatorExpression>().Operator);
            Assert.AreEqual("3", ifStatement.Condition.To<BinaryOperatorExpression>().Right[0].To<LiteralExpression>().Value);

            Assert.AreEqual(0, ifStatement.Body.Statements.Length);
        }

        [Test]
        public void IfStatementWithBinaryOperatorConditionWithInExpressions()
        {
            var ifStatement = ParseStatement<IfStatement>("if shared in $(OPTIONS) || module in $(OPTIONS) { }");

            var binaryExpression = ifStatement.Condition.To<BinaryOperatorExpression>();
            Assert.AreEqual(Runtime.Parser.Operator.Or, binaryExpression.Operator);

            var left = binaryExpression.Left.To<BinaryOperatorExpression>();
            var right = binaryExpression.Right.Single().To<BinaryOperatorExpression>();
            Assert.AreEqual(Runtime.Parser.Operator.In, left.Operator);
            Assert.AreEqual(Runtime.Parser.Operator.In, right.Operator);

            Assert.AreEqual("shared", left.Left.To<LiteralExpression>().Value);
            Assert.AreEqual("module", right.Left.To<LiteralExpression>().Value);
        }

        [Test]
        public void IfStatementWithNegatedCondition()
        {
            var ifStatement = ParseStatement<IfStatement>("if ! $(somevar) { }");
            Assert.IsTrue(ifStatement.Condition.To<NotOperatorExpression>().Expression is VariableDereferenceExpression);
        }

        [Test]
        [Ignore("investigate failure after parenthesis support")]
        public void IfStatementWithNegationAndRightSide()
        {
            //jam is crazy.  if you do:
            //
            //myvar = 123 ;
            //if ! $(myvar) = 321 { }
            //
            //that condition is not considered true. the ! does not apply to the equals expression somehow.  for now we're just going to make the parser throw on any case
            //where ! is used with a non trivial condition, and hope our jam program doesn't actually use this construct.

            Assert.Throws<ParsingException>(() => ParseStatement<IfStatement>("if ! $(somevar) = 321 { }"));
        }

        [Test]
        public void IfStatementWithNotEqualOperator()
        {
            var ifStatement = ParseStatement<IfStatement>("if $(a) != $(b) { }");
            Assert.AreEqual(Runtime.Parser.Operator.NotEqual, ifStatement.Condition.To<BinaryOperatorExpression>().Operator);
            AssertLeftIsA_And_RightIsB(ifStatement);
        }

        [Test]
        public void IfStatementWithOrOperator()
        {
            var ifStatement = ParseStatement<IfStatement>("if $(a) || $(b) { }");
            Assert.AreEqual(Runtime.Parser.Operator.Or, ifStatement.Condition.To<BinaryOperatorExpression>().Operator);
            Assert.AreEqual("a", ifStatement.Condition.To<BinaryOperatorExpression>().Left.To<VariableDereferenceExpression>().VariableExpression.To<LiteralExpression>().Value);
            Assert.AreEqual("b", ifStatement.Condition.To<BinaryOperatorExpression>().Right[0].To<VariableDereferenceExpression>().VariableExpression.To<LiteralExpression>().Value);
        }

        [Test]
        public void IfStatementWithTwoNestedBinaryOperatorExpressins()
        {
            var ifStatement = ParseStatement<IfStatement>("if $(a) = 3 && $(b) = 4 { }");

            var binaryExpression = ifStatement.Condition.To<BinaryOperatorExpression>();
            Assert.AreEqual(Runtime.Parser.Operator.And, binaryExpression.Operator);

            var left = binaryExpression.Left.To<BinaryOperatorExpression>();
            var right = binaryExpression.Right.Single().To<BinaryOperatorExpression>();
            Assert.AreEqual(Runtime.Parser.Operator.Assignment, left.Operator);
            Assert.AreEqual(Runtime.Parser.Operator.Assignment, right.Operator);

            Assert.AreEqual("3", left.Right.Single().To<LiteralExpression>().Value);
            Assert.AreEqual("4", right.Right.Single().To<LiteralExpression>().Value);
        }

        [Test]
        public void IfWithElseStatement()
        {
            var ifStatement = ParseStatement<IfStatement>("if $(somevar) { } else { Echo ; }");
            Assert.AreEqual("Echo", ifStatement.Else.To<BlockStatement>().Statements[0].To<ExpressionStatement>().Expression.To<InvocationExpression>().RuleExpression.To<LiteralExpression>().Value);
        }

        [Test]
        public void IfWithElseStatementThatsNotBlockStatement()
        {
            var ifStatement = ParseStatement<IfStatement>("if $(somevar) { } else Echo ;");
            Assert.AreEqual("Echo", ifStatement.Else.To<ExpressionStatement>().Expression.To<InvocationExpression>().RuleExpression.To<LiteralExpression>().Value);
        }

        [Test]
        public void IncludeStatement()
        {
            var includeStatement = ParseStatement<IncludeStatement>("include myfile ;");
            Assert.AreEqual("myfile", includeStatement.Expression.To<LiteralExpression>().Value);
        }

        [Test]
        public void InCondition()
        {
            var condition = ParseCondition<BinaryOperatorExpression>("$(myvar) in MAC PC WINDOWS");

            Assert.AreEqual("myvar", condition.Left.To<VariableDereferenceExpression>().VariableExpression.To<LiteralExpression>().Value);
            Assert.AreEqual(Runtime.Parser.Operator.In, condition.Operator);
            Assert.AreEqual(3, condition.Right.Length);
        }

        [Test]
        public void InOperatorFollowedByParenthesisClose()
        {
            ParseCondition<Expression>("! ( hello in bla )");
        }

        [Test]
        public void InvocationExpressionStatementWithBrackets()
        {
            var expressionStatement = ParseStatement<ExpressionStatement>("[ MyRule myarg ] ;");
            var invocationExpression = expressionStatement.Expression.To<InvocationExpression>();

            Assert.AreEqual("MyRule", invocationExpression.RuleExpression.To<LiteralExpression>().Value);

            Assert.AreEqual(1, invocationExpression.Arguments.Length);
            Assert.AreEqual("myarg", invocationExpression.Arguments[0][0].To<LiteralExpression>().Value);
        }

        [Test]
        public void LiteralExpansionExpression()
        {
            var result = ParseExpression<LiteralExpansionExpression>("@(myname:S=exe)");
            Assert.AreEqual("myname", result.VariableExpression.To<LiteralExpression>().Value);
        }

        [Test]
        public void LocalVariableDeclaration()
        {
            var localStatement = ParseStatement<LocalStatement>("local a ;");
            Assert.AreEqual("a", localStatement.Variable.Value);
            Assert.AreEqual(0, localStatement.Value.Length);
        }

        [Test]
        public void LocalVariableDeclaration2()
        {
            ParseStatement<IfStatement>(@"if [ GetListCount ] > 1 { }");
        }

        [Test]
        public void LocalVariableDeclarationWithValue()
        {
            var localStatement = ParseStatement<LocalStatement>("local a = 3 ;");
            Assert.AreEqual("a", localStatement.Variable.Value);
            Assert.AreEqual("3", localStatement.Value[0].To<LiteralExpression>().Value);
        }

        [Test]
        public void LooksLikeExpansinoModifierButIsNot()
        {
            ParseStatement<BlockStatement>("{ local grist = $(TARGET):RPX ; Echo hello ; }");
        }

        [Test]
        public void Modifier_With_Empty_Value()
        {
            var variableDereferenceExpression = ParseExpression<VariableDereferenceExpression>("$(harry:S=)");
            Assert.IsInstanceOf<EmptyExpression>(variableDereferenceExpression.Modifiers.Single().Value);
        }

        [Test]
        public void Modifier_With_No_Value()
        {
            var variableDereferenceExpression = ParseExpression<VariableDereferenceExpression>("$(harry:S)");
            Assert.IsNull(variableDereferenceExpression.Modifiers.Single().Value);
        }

        [Test]
        public void ModifierWithQuotedWhiteSpaceAsStatement()
        {
            var jam = @"
    SCE_ROOT_DIR = $(SCE_ROOT_DIR:J="" "") ;
";

            ParseStatement<AssignmentStatement>(jam);
        }

        [Test]
        public void NestedVariableDereference()
        {
            var variableDereferenceExpression = ParseExpression<VariableDereferenceExpression>("$($(myvar))");
            var nestedVariableDereferenceExpression = variableDereferenceExpression.VariableExpression.To<VariableDereferenceExpression>();
            Assert.AreEqual("myvar", nestedVariableDereferenceExpression.VariableExpression.To<LiteralExpression>().Value);
        }

        [Test]
        public void NotOperatorPrecedence()
        {
            var ifStatement = ParseStatement<IfStatement>("if ( ! $(a) || ! $(b) ) && $(c) { }");

            var condition = ifStatement.Condition;

            var binary = condition.To<BinaryOperatorExpression>();
            Assert.AreEqual(Runtime.Parser.Operator.And, binary.Operator);

            var orExpr = binary.Left.To<BinaryOperatorExpression>();

            orExpr.Left.To<NotOperatorExpression>();
            orExpr.Right[0].To<NotOperatorExpression>();
        }

        [Test]
        public void OnStatement()
        {
            //the tricky part here is that we don't misqualify the "on" as a on keyword like in "myvar on target = bla"
            var onStatement = ParseStatement<OnStatement>("on a { } ");

            Assert.AreEqual("a", onStatement.Target.To<LiteralExpression>().Value);

            Assert.AreEqual(0, onStatement.Body.To<BlockStatement>().Statements.Length);
        }

        [Test]
        public void OnStatementInsideInvocation()
        {
            var invocationExpression = ParseExpression<InvocationExpression>("[ on a Booya 1 : 2 ] ");

            Assert.AreEqual("Booya", invocationExpression.RuleExpression.To<LiteralExpression>().Value);
            Assert.AreEqual("a", invocationExpression.OnTarget.To<LiteralExpression>().Value);
            Assert.AreEqual(2, invocationExpression.Arguments.Length);
        }

        [Test]
        public void OnStatementWithoutBlockStatement()
        {
            //the tricky part here is that we don't misqualify the "on" as a on keyword like in "myvar on target = bla"
            var onStatement = ParseStatement<OnStatement>("on $(TARGET) linkFlags += -shared - fPIC ; ");

            Assert.AreEqual("TARGET", onStatement.Target.To<VariableDereferenceExpression>().VariableExpression.To<LiteralExpression>().Value);
            onStatement.Body.To<AssignmentStatement>();
        }

        [Test]
        public void Parenthesis()
        {
            ParseCondition<BinaryOperatorExpression>("( $(cs) || $(generatedCs) )");

            Assert.AreEqual(Runtime.Parser.Operator.And, ParseCondition<BinaryOperatorExpression>("( a || b ) && c").Operator);
            Assert.AreEqual(Runtime.Parser.Operator.And, ParseCondition<BinaryOperatorExpression>("a && ( b || c )").Operator);
            Assert.AreEqual(Runtime.Parser.Operator.GreaterThan, ParseCondition<BinaryOperatorExpression>("a > ( b && c )").Operator);
        }

        [Test]
        public void ParseExpressionListTest()
        {
            var expressionList = ParseExpressionList("[ MD5 myvalue] harry");
            Assert.AreEqual(2, expressionList.Length);
            Assert.IsTrue(expressionList[0] is InvocationExpression);
            Assert.IsTrue(expressionList[1] is LiteralExpression);
        }

        [Test]
        public void Precedence()
        {
            Assert.AreEqual(Runtime.Parser.Operator.Or, ParseCondition<BinaryOperatorExpression>("a || b && c").Operator);
            Assert.AreEqual(Runtime.Parser.Operator.Or, ParseCondition<BinaryOperatorExpression>("a && b || c").Operator);
            Assert.AreEqual(Runtime.Parser.Operator.And, ParseCondition<BinaryOperatorExpression>("a > b && c").Operator);
        }

        [Test]
        public void ReturnStatement()
        {
            var node = ParseStatement<ReturnStatement>("return 123 ;");

            Assert.AreEqual("123", node.ReturnExpression[0].To<LiteralExpression>().Value);
        }

        [Test]
        public void ReturnStatementWithMultipleValues()
        {
            var returnStatement = ParseStatement<ReturnStatement>("return 123 harry ;");

            var expressions = returnStatement.ReturnExpression.Cast<LiteralExpression>().ToArray();
            Assert.AreEqual("123", expressions[0].Value);
            Assert.AreEqual("harry", expressions[1].Value);
        }

        [Test]
        public void RuleDeclaration()
        {
            var ruleDeclaration = ParseStatement<RuleDeclarationStatement>("rule myrule arg1 : arg2 { Echo $(arg1) ; }");

            Assert.AreEqual("myrule", ruleDeclaration.Name);

            CollectionAssert.AreEqual(new[] { "arg1", "arg2" }, ruleDeclaration.Arguments);

            Assert.AreEqual(1, ruleDeclaration.Body.Statements.Length);

            var invocation = ruleDeclaration.Body.Statements[0].To<ExpressionStatement>().Expression.To<InvocationExpression>();

            Assert.IsTrue(invocation.Arguments.All(a => a.Parent == invocation.Arguments));

            Assert.AreEqual(invocation, invocation.Arguments.Parent);
        }

        [Test]
        public void RuleInvocationWithArgumentContainingColon()
        {
            var expressionStatement = ParseStatement<ExpressionStatement>("MyRule my:funky:arg0 ;");
            var invocationExpression = expressionStatement.Expression.To<InvocationExpression>();

            Assert.AreEqual(1, invocationExpression.Arguments.Length);
            Assert.AreEqual("my:funky:arg0", invocationExpression.Arguments[0].Single().To<LiteralExpression>().Value);
        }

        [Test]
        public void SimpleInvocationExpression()
        {
            var expressionStatement = ParseStatement<ExpressionStatement>("somerule ;");
            var invocationExpression = expressionStatement.Expression.To<InvocationExpression>();
            Assert.AreEqual("somerule", invocationExpression.RuleExpression.To<LiteralExpression>().Value);
        }

        [Test]
        public void SimpleInvocationWithMultiValueArgument()
        {
            var expressionStatement = ParseStatement<ExpressionStatement>("input a b c ;");

            var invocationExpression = expressionStatement.Expression.To<InvocationExpression>();

            Assert.AreEqual(1, invocationExpression.Arguments.Length);

            var expressionList = invocationExpression.Arguments[0];
            Assert.AreEqual(3, expressionList.Length);

            var literalExpressions = expressionList.Cast<LiteralExpression>().ToArray();
            Assert.AreEqual("a", literalExpressions[0].Value);
            Assert.AreEqual("b", literalExpressions[1].Value);
            Assert.AreEqual("c", literalExpressions[2].Value);
        }

        [Test]
        public void SimpleInvocationWithOneLiteralArguments()
        {
            var node = ParseStatement<ExpressionStatement>("input a ;");

            var invocationExpression = node.Expression.To<InvocationExpression>();

            Assert.AreEqual(1, invocationExpression.Arguments.Length);

            var expressionList = invocationExpression.Arguments[0];
            Assert.AreEqual(1, expressionList.Length);
            Assert.AreEqual("a", expressionList[0].To<LiteralExpression>().Value);
        }

        [Test]
        public void SimpleInvocationWithTwoLiteralArguments()
        {
            var node = ParseStatement<ExpressionStatement>("input a : b ;");

            var invocationExpression = (InvocationExpression)node.Expression;

            Assert.AreEqual(2, invocationExpression.Arguments.Length);

            Assert.AreEqual("a", invocationExpression.Arguments[0][0].To<LiteralExpression>().Value);
            Assert.AreEqual("b", invocationExpression.Arguments[1][0].To<LiteralExpression>().Value);
        }

        [Test]
        public void Subtract()
        {
            var assignmentStatement = ParseStatement<AssignmentStatement>("a -= b ;");

            AssertLeftIsA_and_RightIsB(assignmentStatement);
            Assert.AreEqual(Runtime.Parser.Operator.Subtract, assignmentStatement.Operator);
        }

        [Test]
        public void SwitchStatement()
        {
            var switchStatement = ParseStatement<SwitchStatement>(@"switch $(myvar) {
  case a :
     hello ;
  case b :
     there ;
     #mycomment
     sailor ;
}");
            Assert.AreEqual("myvar", switchStatement.Variable.To<VariableDereferenceExpression>().VariableExpression.To<LiteralExpression>().Value);

            Assert.AreEqual(2, switchStatement.Cases.Length);

            var case0 = switchStatement.Cases[0];
            Assert.AreEqual("a", case0.CaseExpression.Value);
            Assert.AreEqual(1, case0.Statements.Length);
            AssertIsRuleInvocationWithName(case0.Statements[0], "hello");

            var case1 = switchStatement.Cases[1];
            Assert.AreEqual("b", case1.CaseExpression.Value);
            Assert.AreEqual(3, case1.Statements.Length);

            AssertIsRuleInvocationWithName(case1.Statements[0], "there");
            Assert.AreEqual("mycomment", case1.Statements[1].To<Comment>().Content);
            AssertIsRuleInvocationWithName(case1.Statements[2], "sailor");
        }

        [Test]
        public void VariableDereference()
        {
            var variableDereferenceExpression = ParseExpression<VariableDereferenceExpression>("$(myvar)");
            Assert.AreEqual("myvar", variableDereferenceExpression.VariableExpression.To<LiteralExpression>().Value);
        }

        [Test]
        public void VariableDereferenceWithIndexer()
        {
            var variableDereferenceExpression = ParseExpression<VariableDereferenceExpression>("$(myvar[123])");
            Assert.AreEqual("myvar", variableDereferenceExpression.VariableExpression.To<LiteralExpression>().Value);
            Assert.AreEqual("123", variableDereferenceExpression.IndexerExpression.To<LiteralExpression>().Value);
        }

        [Test]
        public void VariableExpansionModifiers()
        {
            var variableDereferenceExpression = ParseExpression<VariableDereferenceExpression>("$(harry:BS)");

            Assert.AreEqual("harry", variableDereferenceExpression.VariableExpression.To<LiteralExpression>().Value);

            Assert.AreEqual(2, variableDereferenceExpression.Modifiers.Length);
            Assert.AreEqual('B', variableDereferenceExpression.Modifiers[0].Command);
            Assert.AreEqual('S', variableDereferenceExpression.Modifiers[1].Command);
        }

        [Test]
        public void VariableExpansionModifiersWithEmptyValue()
        {
            var variableDereferenceExpression = ParseExpression<VariableDereferenceExpression>("$(harry:B=)");

            Assert.AreEqual("harry", variableDereferenceExpression.VariableExpression.To<LiteralExpression>().Value);

            Assert.AreEqual(1, variableDereferenceExpression.Modifiers.Length);
            Assert.AreEqual('B', variableDereferenceExpression.Modifiers[0].Command);
            Assert.IsInstanceOf<EmptyExpression>(variableDereferenceExpression.Modifiers[0].Value);
        }

        [Test]
        public void VariableExpansionModifiersWithNonLiteralValue()
        {
            var variableDereferenceExpression = ParseExpression<VariableDereferenceExpression>("$(harry:B=$(pietje))");

            Assert.AreEqual("harry", variableDereferenceExpression.VariableExpression.To<LiteralExpression>().Value);

            Assert.AreEqual(1, variableDereferenceExpression.Modifiers.Length);
            Assert.AreEqual('B', variableDereferenceExpression.Modifiers[0].Command);

            var value = variableDereferenceExpression.Modifiers[0].Value.To<VariableDereferenceExpression>();
            Assert.AreEqual("pietje", value.VariableExpression.To<LiteralExpression>().Value);
        }

        [Test]
        public void VariableExpansionModifiersWithValue()
        {
            var variableDereferenceExpression = ParseExpression<VariableDereferenceExpression>("$(harry:B=value:S)");

            Assert.AreEqual("harry", variableDereferenceExpression.VariableExpression.To<LiteralExpression>().Value);

            Assert.AreEqual(2, variableDereferenceExpression.Modifiers.Length);
            Assert.AreEqual('B', variableDereferenceExpression.Modifiers[0].Command);
            Assert.AreEqual("value", variableDereferenceExpression.Modifiers[0].Value.To<LiteralExpression>().Value);

            Assert.AreEqual('S', variableDereferenceExpression.Modifiers[1].Command);
            Assert.IsNull(variableDereferenceExpression.Modifiers[1].Value);
        }

        [Test]
        public void VariableExpansionModifierWithRegex()
        {
            //var variableDereferenceExpression
        }

        [Test]
        public void VariableOnTargetBeforeMaybeAssignment()
        {
            //the tricky part here is that we don't misqualify the "on" as a on keyword like in "myvar on target = bla"
            var variableOnTarget = ParseStatement<AssignmentStatement>("myvar on mytarget ?= 3 ;").Left.To<VariableOnTargetExpression>();
            Assert.AreEqual("mytarget", variableOnTarget.Targets.Single().To<LiteralExpression>().Value);
        }

        [Test]
        public void WhileStatement()
        {
            var whileStatement = ParseStatement<WhileStatement>("while $(mylist) { } ");
            Assert.IsTrue(whileStatement.Condition is VariableDereferenceExpression);
            Assert.AreEqual(0, whileStatement.Body.Statements.Length);
        }

        [Test]
        public void QuotesAsLiteralsMode()
        {
            var parser = new Runtime.Parser.Parser("--assembly=\"pieter\"", ParserMode.QuotesAsLiteral);
            var literalExpression = parser.ParseExpression().To<LiteralExpression>();
            Assert.AreEqual("--assembly=\"pieter\"", literalExpression.Value);
        }
    }
}
