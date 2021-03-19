using ICSharpCode.NRefactory.CSharp;

namespace Cs2us.Visitors
{
    class StatementIndenter : BaseAstVisitor
    {
        private readonly TokenWriter m_Formatter;
        private readonly bool m_IncrementIndentation;

        public StatementIndenter(TokenWriter formatter, bool incrementIndentation)
        {
            m_Formatter = formatter;
            m_IncrementIndentation = incrementIndentation;
        }

        public override void VisitIfElseStatement(IfElseStatement ifElseStatement)
        {
            Format();
        }

        public override void VisitForStatement(ForStatement forStatement)
        {
            Format();
        }

        public override void VisitForeachStatement(ForeachStatement foreachStatement)
        {
            Format();
        }

        public override void VisitWhileStatement(WhileStatement whileStatement)
        {
            Format();
        }

        public override void VisitSwitchStatement(SwitchStatement switchStatement)
        {
            Format();
        }

        public override void VisitCaseLabel(CaseLabel caseLabel)
        {
            Format();
        }

        public override void VisitBreakStatement(BreakStatement breakStatement)
        {
            Format();
        }

        public override void VisitSwitchSection(SwitchSection switchSection)
        {
            Format();
        }

        void Format()
        {
            if (m_IncrementIndentation)
            {
                m_Formatter.Indent();
                m_Formatter.NewLine();
            }
            else
            {
                m_Formatter.Unindent();
            }
        }
    }
}
