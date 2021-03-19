using System;
using System.Collections.Generic;
using System.IO;
using System.Linq;
using Cs2us.Visitors;
using ICSharpCode.NRefactory.CSharp;
using ICSharpCode.NRefactory.CSharp.Resolver;
using ICSharpCode.NRefactory.TypeSystem;
using AnonymousMethodExpression = ICSharpCode.NRefactory.CSharp.AnonymousMethodExpression;
using Attribute = ICSharpCode.NRefactory.CSharp.Attribute;
using Expression = ICSharpCode.NRefactory.CSharp.Expression;
using ExpressionStatement = ICSharpCode.NRefactory.CSharp.ExpressionStatement;
using ParenthesizedExpression = ICSharpCode.NRefactory.CSharp.ParenthesizedExpression;

namespace Cs2us
{
    public class Cs2UsVisitor : DepthFirstAstVisitor
    {
        private Expression m_LastInitializer = Expression.Null;

        private readonly bool m_ExplicitlyTypedVariables;
        private readonly bool m_UsePragmaStrict;

        public Cs2UsVisitor(bool explicitlyTypedVariables = true, bool usePragmaStrict = true)
        {
            m_ExplicitlyTypedVariables = explicitlyTypedVariables;
            m_UsePragmaStrict = usePragmaStrict;
            m_Output = new TextWriterTokenWriter(m_Writer);
            m_Writer.NewLine = "\n";
        }

        public override void VisitSyntaxTree(SyntaxTree st)
        {
            m_SyntaxTree = st;
            if (m_UsePragmaStrict)
                m_Output.WritePreProcessorDirective(PreProcessorDirectiveType.Pragma, "strict");
            base.VisitSyntaxTree(m_SyntaxTree);
        }

        public override void VisitNamespaceDeclaration(NamespaceDeclaration namespaceDeclaration)
        {
            namespaceDeclaration.Members.AcceptVisitor(this);
        }

        public override void VisitComment(Comment comment)
        {
            m_Output.WriteComment(comment.CommentType, comment.Content);
        }

        public override void VisitVariableDeclarationStatement(VariableDeclarationStatement statement)
        {
            VisitLastCommentIfPresent(statement);
            ProcessVariableDeclaration(statement.Variables, statement.Type, statement.Modifiers);
        }

        public override void VisitFieldDeclaration(FieldDeclaration fieldDeclaration)
        {
            ProcessVariableDeclaration(fieldDeclaration.Variables, fieldDeclaration.ReturnType,
                fieldDeclaration.Modifiers, fieldDeclaration.Attributes);
        }

        public override void VisitParenthesizedExpression(ParenthesizedExpression parenthesizedExpression)
        {
            m_Output.WriteToken(null, "(");
            parenthesizedExpression.Expression.AcceptVisitor(this);
            m_Output.WriteToken(null, ")");
        }

        public override void VisitIfElseStatement(IfElseStatement ifStatement)
        {
            VisitLastCommentIfPresent(ifStatement);
            m_Output.WriteToken(null, "if (");
            ifStatement.Condition.AcceptVisitor(this);
            m_Output.WriteToken(null, ")");
            ifStatement.TrueStatement.AcceptVisitor(this);
            if (ifStatement.FalseStatement.IsNull)
                return;

            m_Output.WriteToken(null, "else");

            ifStatement.FalseStatement.AcceptVisitor(this);
        }

        public override void VisitConditionalExpression(ConditionalExpression conditionalExpression)
        {
            conditionalExpression.Condition.AcceptVisitor(this);
            m_Output.WriteToken(null, " ? ");
            conditionalExpression.TrueExpression.AcceptVisitor(this);
            m_Output.WriteToken(null, " : ");
            conditionalExpression.FalseExpression.AcceptVisitor(this);
        }

        public override void VisitBinaryOperatorExpression(BinaryOperatorExpression binaryOperatorExpression)
        {
            binaryOperatorExpression.Left.AcceptVisitor(this);
            var op = binaryOperatorExpression.OperatorToken.Role.ToString();
            m_Output.WriteToken(null, string.Format(" {0} ", op));
            binaryOperatorExpression.Right.AcceptVisitor(this);
        }

        public override void VisitUnaryOperatorExpression(UnaryOperatorExpression unaryOperatorExpression)
        {
            bool isPostOperator = IsPostOperator(unaryOperatorExpression.Operator);
            if (!isPostOperator)
            {
                m_Output.WriteToken(null, OperatorString(unaryOperatorExpression));
                unaryOperatorExpression.Expression.AcceptVisitor(this);
            }
            else
            {
                unaryOperatorExpression.Expression.AcceptVisitor(this);
                m_Output.WriteToken(null, OperatorString(unaryOperatorExpression));
            }
        }

        public override void VisitExpressionStatement(ExpressionStatement expressionStatement)
        {
            VisitLastCommentIfPresent(expressionStatement);
            ApplyFormatingIfRequired(expressionStatement, true);
            expressionStatement.Expression.AcceptVisitor(this);
            ApplyFormatingIfRequired(expressionStatement, false);
            WriteSemicolonAndNewline();
        }

        public override void VisitAssignmentExpression(AssignmentExpression assignmentExpression)
        {
            assignmentExpression.Left.AcceptVisitor(this);
            m_Output.WriteToken(null, string.Format(" {0} ", assignmentExpression.OperatorToken));
            assignmentExpression.Right.AcceptVisitor(this);
        }

        public override void VisitVariableInitializer(VariableInitializer variableInitializer)
        {
            VisitLastCommentIfPresent(variableInitializer);
            m_LastInitializer = variableInitializer.Initializer;
            m_Output.WriteToken(null, variableInitializer.Name);
        }

        public override void VisitIdentifier(Identifier identifier)
        {
            m_Output.WriteToken(null, identifier.Name);
        }

        public override void VisitAsExpression(AsExpression asExpression)
        {
            asExpression.Expression.AcceptVisitor(this);
            m_Output.WriteToken(null, " as ");
            asExpression.Type.AcceptVisitor(this);
        }

        public override void VisitNullReferenceExpression(NullReferenceExpression nullReferenceExpression)
        {
            m_Output.WriteToken(null, "null");
        }

        public override void VisitObjectCreateExpression(ObjectCreateExpression objectCreateExpression)
        {
            m_Output.WriteToken(null, "new ");
            objectCreateExpression.Type.AcceptVisitor(this);

            VisitListWithSeparator(objectCreateExpression.Arguments, "(", ")");
        }

        public override void VisitMemberReferenceExpression(MemberReferenceExpression memberReferenceExpression)
        {
            memberReferenceExpression.Target.AcceptVisitor(this);
            m_Output.WriteToken(null, ".");
            m_Output.WriteToken(null, memberReferenceExpression.MemberName);
            VisitNonEmptyListWithSeparator(memberReferenceExpression.TypeArguments, ".<", ">");
        }

        public override void VisitPrimitiveType(PrimitiveType primitiveType)
        {
            var primitiveResult = primitiveType.AcceptVisitor(new PrimitiveTypeExtractor());
            m_Output.WriteToken(null, primitiveResult);
        }

        public override void VisitPrimitiveExpression(PrimitiveExpression primitiveExpression)
        {
            m_Output.WriteToken(null, primitiveExpression.LiteralValue);
        }

        public override void VisitArraySpecifier(ArraySpecifier arraySpecifier)
        {
            m_Output.WriteToken(null, "[");
            m_Output.WriteToken(null, new string(',', arraySpecifier.Dimensions - 1));
            m_Output.WriteToken(null, "]");
        }

        public override void VisitBlockStatement(BlockStatement blockStatement)
        {
            var shouldIndent = ShouldIndent(blockStatement);
            if (shouldIndent)
            {
                m_Output.WriteToken(null, " {");
                m_Output.Indent();
                m_Output.NewLine();
            }

            blockStatement.Statements.AcceptVisitor(this);
            VisitLastCommentIfPresent(blockStatement.LastChild);

            if (shouldIndent)
            {
                m_Output.Unindent();
                m_Output.WriteToken(null, "}");
                if (!IsBodyOfAnonymousMethodExpression(blockStatement))
                {
                    m_Output.NewLine();
                }
            }
        }

        private static bool IsBodyOfAnonymousMethodExpression(BlockStatement blockStatement)
        {
            var annotation = blockStatement.Annotation(typeof(string));
            return blockStatement.GetParent<AnonymousMethodExpression>() == blockStatement.Parent
                || annotation != null && annotation.ToString() == InjectedLambdaBodyAnnotation;
        }

        public override void VisitArrayCreateExpression(ArrayCreateExpression arrayCreateExpression)
        {
            if (arrayCreateExpression.Initializer.IsNull)
            {
                m_Output.WriteToken(null, "new ");
                arrayCreateExpression.Type.AcceptVisitor(this);
                VisitListWithSeparator(arrayCreateExpression.Arguments, "[", "]");
            }
            else
            {
                arrayCreateExpression.Initializer.AcceptVisitor(this);
            }
        }

        public override void VisitArrayInitializerExpression(ArrayInitializerExpression arrayInitializerExpression)
        {
            VisitListWithSeparator(arrayInitializerExpression.Elements, "[", "]");
        }

        public override void VisitMethodDeclaration(MethodDeclaration methodDeclaration)
        {
            VisitLastCommentIfPresent(methodDeclaration);
            methodDeclaration.Attributes.AcceptVisitor(this);
            if (ShouldIncludeMethodDeclaration(methodDeclaration))
            {
                WriteModifiers(methodDeclaration.Modifiers);
                m_Output.WriteToken(null, "function ");
                m_Output.WriteToken(null, methodDeclaration.Name);
                VisitListWithSeparator(methodDeclaration.Parameters, "(", ")", (parameter, sep) =>
                {
                    parameter.Attributes.AcceptVisitor(this);
                    m_Output.WriteToken(null, string.Format("{0}{1}", sep, parameter.Name));
                    WriteTypeIfRequired(parameter.Type, true);
                });
                //do not write return type for functions in UnityScript, it becomes too noisy
                //WriteTypeIfRequired(methodDeclaration.ReturnType);
            }
            methodDeclaration.Body.AcceptVisitor(this);
        }

        public override void VisitReturnStatement(ReturnStatement returnStatement)
        {
            m_Output.WriteToken(null, "return ");
            returnStatement.Expression.AcceptVisitor(this);
            WriteSemicolonAndNewline();
        }

        public override void VisitTypeDeclaration(TypeDeclaration typeDeclaration)
        {
            typeDeclaration.Attributes.AcceptVisitor(this);
            bool includeTypeDeclInJs = ShouldIncludeClassDeclaration(typeDeclaration);
            if (includeTypeDeclInJs)
            {
                WriteModifiers(typeDeclaration.Modifiers);
                switch (typeDeclaration.ClassType)
                {
                    case ClassType.Class:
                        m_Output.WriteToken(null, "class ");
                        break;

                    case ClassType.Enum:
                        m_Output.WriteToken(null, "enum ");
                        break;

                    case ClassType.Struct:
                        m_Output.WriteToken(null, "class ");
                        break;
                    case ClassType.Interface:
                        m_Output.WriteToken(null, "interface ");
                        break;

                    default:
                        throw new NotImplementedException("VisitTypeDeclaration" + typeDeclaration);
                }

                m_Output.WriteToken(null, typeDeclaration.Name);
                if (typeDeclaration.ClassType == ClassType.Struct)
                    m_Output.WriteToken(null, " extends System.ValueType");
                else
                {
                    var colonToken = typeDeclaration.ColonToken;
                    if (colonToken.ToString() != "")
                    {
                        var nextSibling = colonToken.NextSibling.ToString();
                        var isInterface = nextSibling.Length > 1 && nextSibling[0] == 'I' && Char.IsUpper(nextSibling[1]);
                        m_Output.WriteToken(null, isInterface ? " implements " : " extends ");
                        var nextSiblingGenericWithDot = ConvertGenericIfNeeded(nextSibling);
                        m_Output.WriteToken(null, nextSiblingGenericWithDot);
                    }
                }
                m_Output.WriteToken(null, " {");
                m_Output.Indent();
                m_Output.NewLine();
            }

            typeDeclaration.Members.AcceptVisitor(this);

            VisitLastCommentIfPresent(typeDeclaration.LastChild);
            if (includeTypeDeclInJs)
            {
                m_Output.Unindent();
                m_Output.WriteToken(null, "}");
                m_Output.NewLine();
            }
        }

        private static string ConvertGenericIfNeeded(string nextSibling)
        {
            string nextSiblingGenericWithDot = nextSibling;
            var firstAngleBracket = nextSibling.IndexOf('<');
            if (firstAngleBracket > 0)
                nextSiblingGenericWithDot = nextSibling.Insert(firstAngleBracket, ".");
            return nextSiblingGenericWithDot;
        }

        public override void VisitSimpleType(SimpleType simpleType)
        {
            if (simpleType.TypeArguments.Count == 0 || (simpleType.Identifier != "Func" && simpleType.Identifier != "Action"))
            {
                m_Output.WriteToken(null, simpleType.Identifier);
                if (simpleType.TypeArguments.Count > 0)
                {
                    m_Output.WriteToken(null, ".<");
                    bool first = true;
                    foreach (var arg in simpleType.TypeArguments)
                    {
                        if (!first)
                        {
                            m_Output.WriteToken(null, ",");
                        }
                        else
                        {
                            first = false;
                        }
                        arg.AcceptVisitor(this);
                    }
                    m_Output.WriteToken(null, ">");
                }
                return;
            }

            m_Output.WriteToken(null, "function");
            if (simpleType.Identifier == "Func")
            {
                var returnType = simpleType.TypeArguments.Last();
                var parameters = simpleType.TypeArguments.TakeWhile((type, index) => index < simpleType.TypeArguments.Count - 1);
                VisitListWithSeparator(parameters, "(", ")");
                WriteTypeIfRequired(returnType);
            }
            else
            {
                VisitListWithSeparator(simpleType.TypeArguments, "(", ")");
                WriteTypeIfRequired(new SimpleType("void"));
            }
        }

        public override void VisitForStatement(ForStatement forStatement)
        {
            VisitLastCommentIfPresent(forStatement);
            ValidateForeach(forStatement);

            m_Output.WriteToken(null, "for (");

            m_IgnoreFormattingAndSemicolon = true;

            forStatement.Initializers.AcceptVisitor(this);
            m_Output.WriteToken(null, "; ");
            forStatement.Condition.AcceptVisitor(this);
            m_Output.WriteToken(null, "; ");
            forStatement.Iterators.AcceptVisitor(this);
            m_Output.WriteToken(null, ")");

            m_IgnoreFormattingAndSemicolon = false;

            forStatement.EmbeddedStatement.AcceptVisitor(this);
        }

        public override void VisitWhileStatement(WhileStatement whileStatement)
        {
            m_Output.WriteToken(null, "while ( ");

            whileStatement.Condition.AcceptVisitor(this);
            m_Output.WriteToken(null, " )");
            whileStatement.EmbeddedStatement.AcceptVisitor(this);
        }

        public override void VisitSwitchStatement(SwitchStatement switchStatement)
        {
            VisitLastCommentIfPresent(switchStatement);
            m_Output.WriteToken(null, "switch (");

            switchStatement.Expression.AcceptVisitor(this);
            m_Output.WriteToken(null, ")");
            m_Output.WriteToken(null, " {");
            m_Output.Indent();
            m_Output.NewLine();

            ApplyFormatingIfRequired(switchStatement, true);

            foreach (var sec in switchStatement.SwitchSections)
                sec.AcceptVisitor(this);

            ApplyFormatingIfRequired(switchStatement, false);
            m_Output.Unindent();
            m_Output.WriteToken(null, "}");
            m_Output.NewLine();
        }

        public override void VisitSwitchSection(SwitchSection switchSection)
        {
            VisitLastCommentIfPresent(switchSection);
            base.VisitSwitchSection(switchSection);
        }

        public override void VisitCaseLabel(CaseLabel caseLabel)
        {
            if (!caseLabel.Expression.IsNull)
            {
                m_Output.WriteToken(null, "case ");
                caseLabel.Expression.AcceptVisitor(this);
                m_Output.WriteToken(null, ":");
            }
            else
            {
                m_Output.WriteToken(null, "default:");
            }
        }

        public override void VisitBreakStatement(BreakStatement breakStatement)
        {
            ApplyFormatingIfRequired(breakStatement, true);
            m_Output.WriteToken(null, "break;");
            m_Output.NewLine();
            ApplyFormatingIfRequired(breakStatement, false);
        }

        public override void VisitForeachStatement(ForeachStatement foreachStatement)
        {
            VisitLastCommentIfPresent(foreachStatement);
            m_Output.WriteToken(null, string.Format("for (var {0}", foreachStatement.VariableName));
            WriteTypeIfRequired(foreachStatement.VariableType);
            m_Output.WriteToken(null, " in ");
            foreachStatement.InExpression.AcceptVisitor(this);
            m_Output.WriteToken(null, ")");
            foreachStatement.EmbeddedStatement.AcceptVisitor(this);
        }

        private static void ValidateForeach(ForStatement forStatement)
        {
            if (forStatement.Initializers.Count == 1 &&
                forStatement.Initializers.ElementAt(0).GetChildrenByRole(Roles.Variable).Count > 1)
            {
                var startLocation = forStatement.Initializers.FirstOrNullObject().StartLocation;
                var endLocation = forStatement.Initializers.LastOrNullObject().EndLocation;

                forStatement.FirstChild.Remove();
                throw new Exception(string.Format("Multiple initializers not supported ({1}, {2}):\r\n{0}", forStatement,
                    startLocation, endLocation));
            }
        }

        public override void VisitIndexerExpression(IndexerExpression indexerExpression)
        {
            indexerExpression.Target.AcceptVisitor(this);
            VisitListWithSeparator(indexerExpression.Arguments, "[", "]");
        }

        public override void VisitAnonymousMethodExpression(AnonymousMethodExpression anonymousMethodExpression)
        {
            m_Output.WriteToken(null, "function()");
            anonymousMethodExpression.Body.AcceptVisitor(this);
        }

        public override void VisitMemberType(MemberType memberType)
        {
            memberType.Target.AcceptVisitor(this);
            m_Output.WriteToken(null, string.Format(".{0}", memberType.MemberName));
            VisitNonEmptyListWithSeparator(memberType.TypeArguments, ".<", ">");
        }

        public override void VisitIdentifierExpression(IdentifierExpression identifierExpression)
        {
            var idExpressionGeneric = ConvertGenericIfNeeded(identifierExpression.ToString());
            m_Output.WriteToken(null, idExpressionGeneric);
        }

        public override void VisitInvocationExpression(InvocationExpression expression)
        {
            expression.Target.AcceptVisitor(this);
            VisitListWithSeparator(expression.Arguments, "(", ")");
        }

        public override void VisitThisReferenceExpression(ThisReferenceExpression thisReferenceExpression)
        {
            m_Output.WriteToken(null, "this");
        }

        public override void VisitLambdaExpression(LambdaExpression lambda)
        {
            m_Output.WriteToken(null, "function");
            VisitListWithSeparator(lambda.Parameters, "(", ")");

            if (lambda.Body as BlockStatement == null)
            {
                var nrefactoryUtils = new NRefactoryUtils();
                var resolved = nrefactoryUtils.Resolve(m_SyntaxTree, lambda) as LambdaResolveResult;
                var expression = lambda.Body as InvocationExpression;

                var newBlock = new BlockStatement();
                lambda.Body.Remove();

                var se = resolved.ReturnType.Kind == TypeKind.Void
                    ? (Statement) new ExpressionStatement(expression)
                    : new ReturnStatement(expression);

                newBlock.Statements.Add(se);
                lambda.Body = newBlock;

                newBlock.AddAnnotation(InjectedLambdaBodyAnnotation);
            }

            lambda.Body.AcceptVisitor(this);
        }

        public override void VisitAttribute(Attribute attribute)
        {
            m_Output.WriteToken(null, "@");
            m_Output.WriteToken(null, attribute.Type.AcceptVisitor(new TypeExtractor()));

            VisitNonEmptyListWithSeparator(attribute.Arguments, "(", ")");
            if (attribute.Parent != null && !(attribute.Parent.Parent is ParameterDeclaration))
                m_Output.NewLine();
            else
            {
                m_Output.WriteToken(null, " ");
            }
        }

        public override void VisitTryCatchStatement(TryCatchStatement tryCatchStatement)
        {
            VisitLastCommentIfPresent(tryCatchStatement);
            m_Output.WriteToken(null, tryCatchStatement.TryToken.ToString());
            tryCatchStatement.TryBlock.AcceptVisitor(this);

            foreach (var clause in tryCatchStatement.CatchClauses)
            {
                VisitLastCommentIfPresent(clause);
                m_Output.WriteToken(null, clause.CatchToken.ToString());
                m_Output.WriteToken(null, "(");
                if (clause.VariableNameToken.Name != "")
                {
                    clause.VariableNameToken.AcceptVisitor(this);
                    m_Output.WriteToken(null, ": "); //space between type and name
                }
                clause.Type.AcceptVisitor(this);
                m_Output.WriteToken(null, ")");
                clause.Body.AcceptVisitor(this);
            }
        }

        // Ignored using declarations
        public override void VisitUsingDeclaration(UsingDeclaration usingDeclaration)
        {
        }

        public override void VisitEnumMemberDeclaration(EnumMemberDeclaration enumMember)
        {
            VisitLastCommentIfPresent(enumMember);
            m_Output.WriteToken(null, enumMember.Name);
            ProcessMemberInitializer(enumMember.Initializer);
            m_Output.WriteToken(null, ",");
            m_Output.NewLine();
        }

        private void VisitLastCommentIfPresent(AstNode astNode)
        {
            AstNode curMember = astNode.PrevSibling;
            if (curMember == null)
                return;
            while (curMember.Role == Roles.NewLine)
                curMember = curMember.PrevSibling;
            if (curMember.Role == Roles.Comment)
                curMember.AcceptVisitor(this);
        }

        #region PRIVATE FUNCTIONS

        private void ProcessVariableDeclaration(AstNodeCollection<VariableInitializer> variables, AstType type, Modifiers modifiers, IEnumerable<AttributeSection> attSectionCollection = null)
        {
            if (attSectionCollection != null)
                foreach (var section in attSectionCollection)
                    VisitAttributeSection(section);
            WriteModifiers(modifiers);
            m_Output.WriteToken(null, "var ");
            variables.AcceptVisitor(this);
            var isParameter = variables.All(v => v.Parent as ParameterDeclaration != null);
            WriteTypeIfRequired(type, isParameter);
            VisitVariableInitializer();
            WriteSemicolonAndNewline();
        }

        private static bool ShouldIncludeClassDeclaration(EntityDeclaration typeDeclaration)
        {
            return typeDeclaration.Name != DummyClassName;
        }

        private static bool ShouldIncludeMethodDeclaration(MethodDeclaration methodDeclaration)
        {
            return methodDeclaration.Name != DummyMethodName;
        }

        private static bool ShouldIndent(AstNode ast)
        {
            var declaration = ast.Parent as MethodDeclaration;
            return declaration == null || declaration.Name != DummyMethodName;
        }

        private bool ShouldUseExplicitTypes(AstType type, bool isParameter)
        {
            return m_ExplicitlyTypedVariables && (isParameter || !type.AcceptVisitor(new TypeChecker("Action")));
        }

        private void WriteSemicolonAndNewline()
        {
            if (m_IgnoreFormattingAndSemicolon) return;

            m_Output.WriteToken(null, ";");
            m_Output.NewLine();
        }

        private void WriteTypeIfRequired(AstType type, bool isParameter = false)
        {
            if (ShouldUseExplicitTypes(type, isParameter))
            {
                m_Output.WriteToken(null, ": ");
                type.AcceptVisitor(this);
            }
        }

        private void VisitNonEmptyListWithSeparator<T>(IEnumerable<T> elements, string openBrace, string closeBrace,
            Action<T, string> itemProcessor = null) where T : AstNode
        {
            if (elements == null || !elements.Any()) return;

            VisitListWithSeparator(elements, openBrace, closeBrace, itemProcessor);
        }

        private void VisitListWithSeparator<T>(IEnumerable<T> elements, string openBrace, string closeBrace, Action<T, string> itemProcessor = null) where T : AstNode
        {
            itemProcessor = itemProcessor ?? delegate(T item, string s)
            {
                m_Output.WriteToken(null, s);
                item.AcceptVisitor(this);
            };

            m_Output.WriteToken(null, openBrace);

            var sep = "";
            foreach (var element in elements)
            {
                itemProcessor(element, sep);
                sep = ", ";
            }
            m_Output.WriteToken(null, closeBrace);
        }

        private void VisitVariableInitializer()
        {
            ProcessMemberInitializer(m_LastInitializer);
            m_LastInitializer = Expression.Null;
        }

        private void ProcessMemberInitializer(Expression initializer)
        {
            if (initializer.IsNull) return;

            m_Output.WriteToken(null, " = ");
            initializer.AcceptVisitor(this);
        }

        private void WriteModifiers(Modifiers modifiers)
        {
            foreach (var m in CSharpModifierToken.AllModifiers)
            {
                if ((modifiers & m) != m)
                    continue;

                var keyword = (m == Modifiers.Sealed) ? "final" : CSharpModifierToken.GetModifierName(m);
                m_Output.WriteKeyword(null, keyword);
                m_Output.Space();
            }
        }

        private void ApplyFormatingIfRequired(AstNode astNode, bool increment)
        {
            if (m_IgnoreFormattingAndSemicolon) return;

            astNode.Parent.AcceptVisitor(new StatementIndenter(m_Output, increment));
        }

        private static bool IsPostOperator(UnaryOperatorType opType)
        {
            return opType == UnaryOperatorType.PostDecrement || opType == UnaryOperatorType.PostIncrement;
        }

        private static string OperatorString(UnaryOperatorExpression op)
        {
            return op.OperatorToken.Role.ToString();
        }

        internal string Text
        {
            get { return m_Writer.ToString(); }
        }

        private readonly TextWriter m_Writer = new StringWriter();
        private readonly TextWriterTokenWriter m_Output;
        private SyntaxTree m_SyntaxTree;
        private const string InjectedLambdaBodyAnnotation = "InjectedLambdaBody";
        private const string DummyClassName = "ExampleClass";
        private const string DummyMethodName = "Example";
        private bool m_IgnoreFormattingAndSemicolon = false;

        #endregion
    }
}
