using ICSharpCode.NRefactory.CSharp;
using ICSharpCode.NRefactory.PatternMatching;

namespace Cs2us.Visitors
{
    internal class BaseAstVisitor : IAstVisitor
    {
        public virtual void VisitAnonymousMethodExpression(AnonymousMethodExpression anonymousMethodExpression)
        {
        }

        public virtual void VisitUndocumentedExpression(UndocumentedExpression undocumentedExpression)
        {
        }

        public virtual void VisitArrayCreateExpression(ArrayCreateExpression arrayCreateExpression)
        {
        }

        public virtual void VisitArrayInitializerExpression(ArrayInitializerExpression arrayInitializerExpression)
        {
        }

        public virtual void VisitAsExpression(AsExpression asExpression)
        {
        }

        public virtual void VisitAssignmentExpression(AssignmentExpression assignmentExpression)
        {
        }

        public virtual void VisitBaseReferenceExpression(BaseReferenceExpression baseReferenceExpression)
        {
        }

        public virtual void VisitBinaryOperatorExpression(BinaryOperatorExpression binaryOperatorExpression)
        {
        }

        public virtual void VisitCastExpression(CastExpression castExpression)
        {
        }

        public virtual void VisitCheckedExpression(CheckedExpression checkedExpression)
        {
        }

        public virtual void VisitConditionalExpression(ConditionalExpression conditionalExpression)
        {
        }

        public virtual void VisitDefaultValueExpression(DefaultValueExpression defaultValueExpression)
        {
        }

        public virtual void VisitDirectionExpression(DirectionExpression directionExpression)
        {
        }

        public virtual void VisitIdentifierExpression(IdentifierExpression identifierExpression)
        {
        }

        public virtual void VisitIndexerExpression(IndexerExpression indexerExpression)
        {
        }

        public virtual void VisitInvocationExpression(InvocationExpression invocationExpression)
        {
        }

        public virtual void VisitIsExpression(IsExpression isExpression)
        {
        }

        public virtual void VisitLambdaExpression(LambdaExpression lambdaExpression)
        {
        }

        public virtual void VisitMemberReferenceExpression(MemberReferenceExpression memberReferenceExpression)
        {
        }

        public virtual void VisitNamedArgumentExpression(NamedArgumentExpression namedArgumentExpression)
        {
        }

        public virtual void VisitNamedExpression(NamedExpression namedExpression)
        {
        }

        public virtual void VisitNullReferenceExpression(NullReferenceExpression nullReferenceExpression)
        {
        }

        public virtual void VisitObjectCreateExpression(ObjectCreateExpression objectCreateExpression)
        {
        }

        public virtual void VisitAnonymousTypeCreateExpression(AnonymousTypeCreateExpression anonymousTypeCreateExpression)
        {
        }

        public virtual void VisitParenthesizedExpression(ParenthesizedExpression parenthesizedExpression)
        {
        }

        public virtual void VisitPointerReferenceExpression(PointerReferenceExpression pointerReferenceExpression)
        {
        }

        public virtual void VisitPrimitiveExpression(PrimitiveExpression primitiveExpression)
        {
        }

        public virtual void VisitSizeOfExpression(SizeOfExpression sizeOfExpression)
        {
        }

        public virtual void VisitStackAllocExpression(StackAllocExpression stackAllocExpression)
        {
        }

        public virtual void VisitThisReferenceExpression(ThisReferenceExpression thisReferenceExpression)
        {
        }

        public virtual void VisitTypeOfExpression(TypeOfExpression typeOfExpression)
        {
        }

        public virtual void VisitTypeReferenceExpression(TypeReferenceExpression typeReferenceExpression)
        {
        }

        public virtual void VisitUnaryOperatorExpression(UnaryOperatorExpression unaryOperatorExpression)
        {
        }

        public virtual void VisitUncheckedExpression(UncheckedExpression uncheckedExpression)
        {
        }

        public virtual void VisitQueryExpression(QueryExpression queryExpression)
        {
        }

        public virtual void VisitQueryContinuationClause(QueryContinuationClause queryContinuationClause)
        {
        }

        public virtual void VisitQueryFromClause(QueryFromClause queryFromClause)
        {
        }

        public virtual void VisitQueryLetClause(QueryLetClause queryLetClause)
        {
        }

        public virtual void VisitQueryWhereClause(QueryWhereClause queryWhereClause)
        {
        }

        public virtual void VisitQueryJoinClause(QueryJoinClause queryJoinClause)
        {
        }

        public virtual void VisitQueryOrderClause(QueryOrderClause queryOrderClause)
        {
        }

        public virtual void VisitQueryOrdering(QueryOrdering queryOrdering)
        {
        }

        public virtual void VisitQuerySelectClause(QuerySelectClause querySelectClause)
        {
        }

        public virtual void VisitQueryGroupClause(QueryGroupClause queryGroupClause)
        {
        }

        public virtual void VisitAttribute(Attribute attribute)
        {
        }

        public virtual void VisitAttributeSection(AttributeSection attributeSection)
        {
        }

        public virtual void VisitDelegateDeclaration(DelegateDeclaration delegateDeclaration)
        {
        }

        public virtual void VisitNamespaceDeclaration(NamespaceDeclaration namespaceDeclaration)
        {
        }

        public virtual void VisitTypeDeclaration(TypeDeclaration typeDeclaration)
        {
        }

        public virtual void VisitUsingAliasDeclaration(UsingAliasDeclaration usingAliasDeclaration)
        {
        }

        public virtual void VisitUsingDeclaration(UsingDeclaration usingDeclaration)
        {
        }

        public virtual void VisitExternAliasDeclaration(ExternAliasDeclaration externAliasDeclaration)
        {
        }

        public virtual void VisitBlockStatement(BlockStatement blockStatement)
        {
        }

        public virtual void VisitBreakStatement(BreakStatement breakStatement)
        {
        }

        public virtual void VisitCheckedStatement(CheckedStatement checkedStatement)
        {
        }

        public virtual void VisitContinueStatement(ContinueStatement continueStatement)
        {
        }

        public virtual void VisitDoWhileStatement(DoWhileStatement doWhileStatement)
        {
        }

        public virtual void VisitEmptyStatement(EmptyStatement emptyStatement)
        {
        }

        public virtual void VisitExpressionStatement(ExpressionStatement expressionStatement)
        {
        }

        public virtual void VisitFixedStatement(FixedStatement fixedStatement)
        {
        }

        public virtual void VisitForeachStatement(ForeachStatement foreachStatement)
        {
        }

        public virtual void VisitForStatement(ForStatement forStatement)
        {
        }

        public virtual void VisitGotoCaseStatement(GotoCaseStatement gotoCaseStatement)
        {
        }

        public virtual void VisitGotoDefaultStatement(GotoDefaultStatement gotoDefaultStatement)
        {
        }

        public virtual void VisitGotoStatement(GotoStatement gotoStatement)
        {
        }

        public virtual void VisitIfElseStatement(IfElseStatement ifElseStatement)
        {
        }

        public virtual void VisitLabelStatement(LabelStatement labelStatement)
        {
        }

        public virtual void VisitLockStatement(LockStatement lockStatement)
        {
        }

        public virtual void VisitReturnStatement(ReturnStatement returnStatement)
        {
        }

        public virtual void VisitSwitchStatement(SwitchStatement switchStatement)
        {
        }

        public virtual void VisitSwitchSection(SwitchSection switchSection)
        {
        }

        public virtual void VisitCaseLabel(CaseLabel caseLabel)
        {
        }

        public virtual void VisitThrowStatement(ThrowStatement throwStatement)
        {
        }

        public virtual void VisitTryCatchStatement(TryCatchStatement tryCatchStatement)
        {
        }

        public virtual void VisitCatchClause(CatchClause catchClause)
        {
        }

        public virtual void VisitUncheckedStatement(UncheckedStatement uncheckedStatement)
        {
        }

        public virtual void VisitUnsafeStatement(UnsafeStatement unsafeStatement)
        {
        }

        public virtual void VisitUsingStatement(UsingStatement usingStatement)
        {
        }

        public virtual void VisitVariableDeclarationStatement(VariableDeclarationStatement variableDeclarationStatement)
        {
        }

        public virtual void VisitWhileStatement(WhileStatement whileStatement)
        {
        }

        public virtual void VisitYieldBreakStatement(YieldBreakStatement yieldBreakStatement)
        {
        }

        public virtual void VisitYieldReturnStatement(YieldReturnStatement yieldReturnStatement)
        {
        }

        public virtual void VisitAccessor(Accessor accessor)
        {
        }

        public virtual void VisitConstructorDeclaration(ConstructorDeclaration constructorDeclaration)
        {
        }

        public virtual void VisitConstructorInitializer(ConstructorInitializer constructorInitializer)
        {
        }

        public virtual void VisitDestructorDeclaration(DestructorDeclaration destructorDeclaration)
        {
        }

        public virtual void VisitEnumMemberDeclaration(EnumMemberDeclaration enumMemberDeclaration)
        {
        }

        public virtual void VisitEventDeclaration(EventDeclaration eventDeclaration)
        {
        }

        public virtual void VisitCustomEventDeclaration(CustomEventDeclaration customEventDeclaration)
        {
        }

        public virtual void VisitFieldDeclaration(FieldDeclaration fieldDeclaration)
        {
        }

        public virtual void VisitIndexerDeclaration(IndexerDeclaration indexerDeclaration)
        {
        }

        public virtual void VisitMethodDeclaration(MethodDeclaration methodDeclaration)
        {
        }

        public virtual void VisitOperatorDeclaration(OperatorDeclaration operatorDeclaration)
        {
        }

        public virtual void VisitParameterDeclaration(ParameterDeclaration parameterDeclaration)
        {
        }

        public virtual void VisitPropertyDeclaration(PropertyDeclaration propertyDeclaration)
        {
        }

        public virtual void VisitVariableInitializer(VariableInitializer variableInitializer)
        {
        }

        public virtual void VisitFixedFieldDeclaration(FixedFieldDeclaration fixedFieldDeclaration)
        {
        }

        public virtual void VisitFixedVariableInitializer(FixedVariableInitializer fixedVariableInitializer)
        {
        }

        public virtual void VisitSyntaxTree(SyntaxTree syntaxTree)
        {
        }

        public virtual void VisitSimpleType(SimpleType simpleType)
        {
        }

        public virtual void VisitMemberType(MemberType memberType)
        {
        }

        public virtual void VisitComposedType(ComposedType composedType)
        {
        }

        public virtual void VisitArraySpecifier(ArraySpecifier arraySpecifier)
        {
        }

        public virtual void VisitPrimitiveType(PrimitiveType primitiveType)
        {
        }

        public virtual void VisitComment(Comment comment)
        {
        }

        public virtual void VisitNewLine(NewLineNode newLineNode)
        {
        }

        public virtual void VisitWhitespace(WhitespaceNode whitespaceNode)
        {
        }

        public virtual void VisitText(TextNode textNode)
        {
        }

        public virtual void VisitPreProcessorDirective(PreProcessorDirective preProcessorDirective)
        {
        }

        public virtual void VisitDocumentationReference(DocumentationReference documentationReference)
        {
        }

        public virtual void VisitTypeParameterDeclaration(TypeParameterDeclaration typeParameterDeclaration)
        {
        }

        public virtual void VisitConstraint(Constraint constraint)
        {
        }

        public virtual void VisitCSharpTokenNode(CSharpTokenNode cSharpTokenNode)
        {
        }

        public virtual void VisitIdentifier(Identifier identifier)
        {
        }

        public virtual void VisitPatternPlaceholder(AstNode placeholder, Pattern pattern)
        {
        }

        public virtual void VisitErrorNode(AstNode errorNode)
        {
        }

        public virtual void VisitNullNode(AstNode nullNode)
        {
        }
    }

    internal class BaseAstVisitor<T> : IAstVisitor<T>
    {
        virtual public T VisitAnonymousMethodExpression(AnonymousMethodExpression anonymousMethodExpression)
        {
            return default(T);
        }

        virtual public T VisitUndocumentedExpression(UndocumentedExpression undocumentedExpression)
        {
            return default(T);
        }

        virtual public T VisitArrayCreateExpression(ArrayCreateExpression arrayCreateExpression)
        {
            return default(T);
        }

        virtual public T VisitArrayInitializerExpression(ArrayInitializerExpression arrayInitializerExpression)
        {
            return default(T);
        }

        virtual public T VisitAsExpression(AsExpression asExpression)
        {
            return default(T);
        }

        virtual public T VisitAssignmentExpression(AssignmentExpression assignmentExpression)
        {
            return default(T);
        }

        virtual public T VisitBaseReferenceExpression(BaseReferenceExpression baseReferenceExpression)
        {
            return default(T);
        }

        virtual public T VisitBinaryOperatorExpression(BinaryOperatorExpression binaryOperatorExpression)
        {
            return default(T);
        }

        virtual public T VisitCastExpression(CastExpression castExpression)
        {
            return default(T);
        }

        virtual public T VisitCheckedExpression(CheckedExpression checkedExpression)
        {
            return default(T);
        }

        virtual public T VisitConditionalExpression(ConditionalExpression conditionalExpression)
        {
            return default(T);
        }

        virtual public T VisitDefaultValueExpression(DefaultValueExpression defaultValueExpression)
        {
            return default(T);
        }

        virtual public T VisitDirectionExpression(DirectionExpression directionExpression)
        {
            return default(T);
        }

        virtual public T VisitIdentifierExpression(IdentifierExpression identifierExpression)
        {
            return default(T);
        }

        virtual public T VisitIndexerExpression(IndexerExpression indexerExpression)
        {
            return default(T);
        }

        virtual public T VisitInvocationExpression(InvocationExpression invocationExpression)
        {
            return default(T);
        }

        virtual public T VisitIsExpression(IsExpression isExpression)
        {
            return default(T);
        }

        virtual public T VisitLambdaExpression(LambdaExpression lambdaExpression)
        {
            return default(T);
        }

        virtual public T VisitMemberReferenceExpression(MemberReferenceExpression memberReferenceExpression)
        {
            return default(T);
        }

        virtual public T VisitNamedArgumentExpression(NamedArgumentExpression namedArgumentExpression)
        {
            return default(T);
        }

        virtual public T VisitNamedExpression(NamedExpression namedExpression)
        {
            return default(T);
        }

        virtual public T VisitNullReferenceExpression(NullReferenceExpression nullReferenceExpression)
        {
            return default(T);
        }

        virtual public T VisitObjectCreateExpression(ObjectCreateExpression objectCreateExpression)
        {
            return default(T);
        }

        virtual public T VisitAnonymousTypeCreateExpression(AnonymousTypeCreateExpression anonymousTypeCreateExpression)
        {
            return default(T);
        }

        virtual public T VisitParenthesizedExpression(ParenthesizedExpression parenthesizedExpression)
        {
            return default(T);
        }

        virtual public T VisitPointerReferenceExpression(PointerReferenceExpression pointerReferenceExpression)
        {
            return default(T);
        }

        virtual public T VisitPrimitiveExpression(PrimitiveExpression primitiveExpression)
        {
            return default(T);
        }

        virtual public T VisitSizeOfExpression(SizeOfExpression sizeOfExpression)
        {
            return default(T);
        }

        virtual public T VisitStackAllocExpression(StackAllocExpression stackAllocExpression)
        {
            return default(T);
        }

        virtual public T VisitThisReferenceExpression(ThisReferenceExpression thisReferenceExpression)
        {
            return default(T);
        }

        virtual public T VisitTypeOfExpression(TypeOfExpression typeOfExpression)
        {
            return default(T);
        }

        virtual public T VisitTypeReferenceExpression(TypeReferenceExpression typeReferenceExpression)
        {
            return default(T);
        }

        virtual public T VisitUnaryOperatorExpression(UnaryOperatorExpression unaryOperatorExpression)
        {
            return default(T);
        }

        virtual public T VisitUncheckedExpression(UncheckedExpression uncheckedExpression)
        {
            return default(T);
        }

        virtual public T VisitQueryExpression(QueryExpression queryExpression)
        {
            return default(T);
        }

        virtual public T VisitQueryContinuationClause(QueryContinuationClause queryContinuationClause)
        {
            return default(T);
        }

        virtual public T VisitQueryFromClause(QueryFromClause queryFromClause)
        {
            return default(T);
        }

        virtual public T VisitQueryLetClause(QueryLetClause queryLetClause)
        {
            return default(T);
        }

        virtual public T VisitQueryWhereClause(QueryWhereClause queryWhereClause)
        {
            return default(T);
        }

        virtual public T VisitQueryJoinClause(QueryJoinClause queryJoinClause)
        {
            return default(T);
        }

        virtual public T VisitQueryOrderClause(QueryOrderClause queryOrderClause)
        {
            return default(T);
        }

        virtual public T VisitQueryOrdering(QueryOrdering queryOrdering)
        {
            return default(T);
        }

        virtual public T VisitQuerySelectClause(QuerySelectClause querySelectClause)
        {
            return default(T);
        }

        virtual public T VisitQueryGroupClause(QueryGroupClause queryGroupClause)
        {
            return default(T);
        }

        virtual public T VisitAttribute(Attribute attribute)
        {
            return default(T);
        }

        virtual public T VisitAttributeSection(AttributeSection attributeSection)
        {
            return default(T);
        }

        virtual public T VisitDelegateDeclaration(DelegateDeclaration delegateDeclaration)
        {
            return default(T);
        }

        virtual public T VisitNamespaceDeclaration(NamespaceDeclaration namespaceDeclaration)
        {
            return default(T);
        }

        virtual public T VisitTypeDeclaration(TypeDeclaration typeDeclaration)
        {
            return default(T);
        }

        virtual public T VisitUsingAliasDeclaration(UsingAliasDeclaration usingAliasDeclaration)
        {
            return default(T);
        }

        virtual public T VisitUsingDeclaration(UsingDeclaration usingDeclaration)
        {
            return default(T);
        }

        virtual public T VisitExternAliasDeclaration(ExternAliasDeclaration externAliasDeclaration)
        {
            return default(T);
        }

        virtual public T VisitBlockStatement(BlockStatement blockStatement)
        {
            return default(T);
        }

        virtual public T VisitBreakStatement(BreakStatement breakStatement)
        {
            return default(T);
        }

        virtual public T VisitCheckedStatement(CheckedStatement checkedStatement)
        {
            return default(T);
        }

        virtual public T VisitContinueStatement(ContinueStatement continueStatement)
        {
            return default(T);
        }

        virtual public T VisitDoWhileStatement(DoWhileStatement doWhileStatement)
        {
            return default(T);
        }

        virtual public T VisitEmptyStatement(EmptyStatement emptyStatement)
        {
            return default(T);
        }

        virtual public T VisitExpressionStatement(ExpressionStatement expressionStatement)
        {
            return default(T);
        }

        virtual public T VisitFixedStatement(FixedStatement fixedStatement)
        {
            return default(T);
        }

        virtual public T VisitForeachStatement(ForeachStatement foreachStatement)
        {
            return default(T);
        }

        virtual public T VisitForStatement(ForStatement forStatement)
        {
            return default(T);
        }

        virtual public T VisitGotoCaseStatement(GotoCaseStatement gotoCaseStatement)
        {
            return default(T);
        }

        virtual public T VisitGotoDefaultStatement(GotoDefaultStatement gotoDefaultStatement)
        {
            return default(T);
        }

        virtual public T VisitGotoStatement(GotoStatement gotoStatement)
        {
            return default(T);
        }

        virtual public T VisitIfElseStatement(IfElseStatement ifElseStatement)
        {
            return default(T);
        }

        virtual public T VisitLabelStatement(LabelStatement labelStatement)
        {
            return default(T);
        }

        virtual public T VisitLockStatement(LockStatement lockStatement)
        {
            return default(T);
        }

        virtual public T VisitReturnStatement(ReturnStatement returnStatement)
        {
            return default(T);
        }

        virtual public T VisitSwitchStatement(SwitchStatement switchStatement)
        {
            return default(T);
        }

        virtual public T VisitSwitchSection(SwitchSection switchSection)
        {
            return default(T);
        }

        virtual public T VisitCaseLabel(CaseLabel caseLabel)
        {
            return default(T);
        }

        virtual public T VisitThrowStatement(ThrowStatement throwStatement)
        {
            return default(T);
        }

        virtual public T VisitTryCatchStatement(TryCatchStatement tryCatchStatement)
        {
            return default(T);
        }

        virtual public T VisitCatchClause(CatchClause catchClause)
        {
            return default(T);
        }

        virtual public T VisitUncheckedStatement(UncheckedStatement uncheckedStatement)
        {
            return default(T);
        }

        virtual public T VisitUnsafeStatement(UnsafeStatement unsafeStatement)
        {
            return default(T);
        }

        virtual public T VisitUsingStatement(UsingStatement usingStatement)
        {
            return default(T);
        }

        virtual public T VisitVariableDeclarationStatement(VariableDeclarationStatement variableDeclarationStatement)
        {
            return default(T);
        }

        virtual public T VisitWhileStatement(WhileStatement whileStatement)
        {
            return default(T);
        }

        virtual public T VisitYieldBreakStatement(YieldBreakStatement yieldBreakStatement)
        {
            return default(T);
        }

        virtual public T VisitYieldReturnStatement(YieldReturnStatement yieldReturnStatement)
        {
            return default(T);
        }

        virtual public T VisitAccessor(Accessor accessor)
        {
            return default(T);
        }

        virtual public T VisitConstructorDeclaration(ConstructorDeclaration constructorDeclaration)
        {
            return default(T);
        }

        virtual public T VisitConstructorInitializer(ConstructorInitializer constructorInitializer)
        {
            return default(T);
        }

        virtual public T VisitDestructorDeclaration(DestructorDeclaration destructorDeclaration)
        {
            return default(T);
        }

        virtual public T VisitEnumMemberDeclaration(EnumMemberDeclaration enumMemberDeclaration)
        {
            return default(T);
        }

        virtual public T VisitEventDeclaration(EventDeclaration eventDeclaration)
        {
            return default(T);
        }

        virtual public T VisitCustomEventDeclaration(CustomEventDeclaration customEventDeclaration)
        {
            return default(T);
        }

        virtual public T VisitFieldDeclaration(FieldDeclaration fieldDeclaration)
        {
            return default(T);
        }

        virtual public T VisitIndexerDeclaration(IndexerDeclaration indexerDeclaration)
        {
            return default(T);
        }

        virtual public T VisitMethodDeclaration(MethodDeclaration methodDeclaration)
        {
            return default(T);
        }

        virtual public T VisitOperatorDeclaration(OperatorDeclaration operatorDeclaration)
        {
            return default(T);
        }

        virtual public T VisitParameterDeclaration(ParameterDeclaration parameterDeclaration)
        {
            return default(T);
        }

        virtual public T VisitPropertyDeclaration(PropertyDeclaration propertyDeclaration)
        {
            return default(T);
        }

        virtual public T VisitVariableInitializer(VariableInitializer variableInitializer)
        {
            return default(T);
        }

        virtual public T VisitFixedFieldDeclaration(FixedFieldDeclaration fixedFieldDeclaration)
        {
            return default(T);
        }

        virtual public T VisitFixedVariableInitializer(FixedVariableInitializer fixedVariableInitializer)
        {
            return default(T);
        }

        virtual public T VisitSyntaxTree(SyntaxTree syntaxTree)
        {
            return default(T);
        }

        virtual public T VisitSimpleType(SimpleType simpleType)
        {
            return default(T);
        }

        virtual public T VisitMemberType(MemberType memberType)
        {
            return default(T);
        }

        virtual public T VisitComposedType(ComposedType composedType)
        {
            return default(T);
        }

        virtual public T VisitArraySpecifier(ArraySpecifier arraySpecifier)
        {
            return default(T);
        }

        virtual public T VisitPrimitiveType(PrimitiveType primitiveType)
        {
            return default(T);
        }

        virtual public T VisitComment(Comment comment)
        {
            return default(T);
        }

        virtual public T VisitWhitespace(WhitespaceNode whitespaceNode)
        {
            return default(T);
        }

        virtual public T VisitText(TextNode textNode)
        {
            return default(T);
        }

        virtual public T VisitNewLine(NewLineNode newLineNode)
        {
            return default(T);
        }

        virtual public T VisitPreProcessorDirective(PreProcessorDirective preProcessorDirective)
        {
            return default(T);
        }

        virtual public T VisitDocumentationReference(DocumentationReference documentationReference)
        {
            return default(T);
        }

        virtual public T VisitTypeParameterDeclaration(TypeParameterDeclaration typeParameterDeclaration)
        {
            return default(T);
        }

        virtual public T VisitConstraint(Constraint constraint)
        {
            return default(T);
        }

        virtual public T VisitCSharpTokenNode(CSharpTokenNode cSharpTokenNode)
        {
            return default(T);
        }

        virtual public T VisitIdentifier(Identifier identifier)
        {
            return default(T);
        }

        virtual public T VisitPatternPlaceholder(AstNode placeholder, Pattern pattern)
        {
            return default(T);
        }

        virtual public T VisitErrorNode(AstNode errorNode)
        {
            return default(T);
        }

        virtual public T VisitNullNode(AstNode errorNode)
        {
            return default(T);
        }
    }
}
