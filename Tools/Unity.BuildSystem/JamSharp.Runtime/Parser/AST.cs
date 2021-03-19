using System;
using System.Collections;
using System.Collections.Generic;

namespace JamSharp.Runtime.Parser
{
    public class Node
    {
        readonly Dictionary<Role, Node> m_Children = new Dictionary<Role, Node>();

        public Node Parent { get; set; }

        public virtual IEnumerable<Node> MyChildren => m_Children.Values;

        protected void SetChild<T>(Role<T> role, T child) where T : Node
        {
            m_Children[role] = child;
            if (child != null)
                child.Parent = this;
        }

        protected T GetChild<T>(Role<T> role) where T : Node
        {
            Node result;
            m_Children.TryGetValue(role, out result);
            return (T)result;
        }

        public virtual IEnumerable<T> GetAllChildrenOfType<T>() where T : Node
        {
            foreach (var child in MyChildren)
            {
                if (child == null)
                    continue;
                if (child is T)
                    yield return (T)child;
                foreach (var c in child.GetAllChildrenOfType<T>())
                    yield return c;
            }
        }
    }

    public class NodeList<T> : Node, IEnumerable<T> where T : Node
    {
        readonly List<T> m_Elements = new List<T>();

        public NodeList(IEnumerable<T> values)
        {
            m_Elements = new List<T>(values);
            foreach (var e in m_Elements)
                e.Parent = this;
        }

        public NodeList() {}

        public int Length => m_Elements.Count;

        public override IEnumerable<Node> MyChildren => m_Elements;

        public T this[int i] => m_Elements[i];

        public IEnumerator<T> GetEnumerator()
        {
            return m_Elements.GetEnumerator();
        }

        IEnumerator IEnumerable.GetEnumerator()
        {
            return GetEnumerator();
        }

        public void Add(T t)
        {
            m_Elements.Add(t);
            t.Parent = this;
        }

        public void InsertAt(int i, T node)
        {
            m_Elements.Insert(i, node);
            node.Parent = this;
        }

        public void ReplaceAt(int i, T node)
        {
            m_Elements[i].Parent = null;
            node.Parent = this;
            m_Elements[i] = node;
        }
    }

    public class TopLevel : Node
    {
        public NodeList<Node> Statements
        {
            get { return GetChild(Roles.Statements); }
            set { SetChild(Roles.Statements, value); }
        }
    }

    public class Expression : Node {}

    public class Statement : Node {}

    public class Comment : Node
    {
        public string Content { get; set; }
    }

    public class IfStatement : Statement
    {
        public Expression Condition
        {
            get { return GetChild(Roles.Condition); }
            set { SetChild(Roles.Condition, value); }
        }

        public BlockStatement Body
        {
            get { return GetChild(Roles.Body); }
            set { SetChild(Roles.Body, value); }
        }

        public Statement Else
        {
            get { return GetChild(Roles.Else); }
            set { SetChild(Roles.Else, value); }
        }
    }

    public class IncludeStatement : Statement
    {
        public Expression Expression
        {
            get { return GetChild(Roles.Expression); }
            set { SetChild(Roles.Expression, value); }
        }
    }

    public class BlockStatement : Statement
    {
        public NodeList<Node> Statements
        {
            get { return GetChild(Roles.Statements); }
            set { SetChild(Roles.Statements, value); }
        }
    }

    public class ReturnStatement : Statement
    {
        public NodeList<Node> ReturnExpression
        {
            get { return GetChild(Roles.ReturnExpression); }
            set { SetChild(Roles.ReturnExpression, value); }
        }
    }

    public class ExpressionStatement : Statement
    {
        public Expression Expression
        {
            get { return GetChild(Roles.Expression); }
            set { SetChild(Roles.Expression, value); }
        }
    }

    public class SwitchStatement : Statement
    {
        public Expression Variable
        {
            get { return GetChild(Roles.Variable); }
            set { SetChild(Roles.Variable, value); }
        }

        public NodeList<SwitchCase> Cases
        {
            get { return GetChild(Roles.SwitchCases); }
            set { SetChild(Roles.SwitchCases, value); }
        }
    }

    public class SwitchCase : Node
    {
        public LiteralExpression CaseExpression
        {
            get { return GetChild(Roles.CaseExpression); }
            set { SetChild(Roles.CaseExpression, value); }
        }

        public NodeList<Node> Statements
        {
            get { return GetChild(Roles.Statements); }
            set { SetChild(Roles.Statements, value); }
        }
    }

    public class LiteralExpression : Expression
    {
        public string Value { get; set; }
    }

    public class EmptyExpression : Expression {}

    public class CombineExpression : Expression
    {
        public NodeList<Expression> Elements
        {
            get { return GetChild(Roles.Elements); }
            set { SetChild(Roles.Elements, value); }
        }
    }

    public class ExpansionStyleExpression : Expression
    {
        public Expression VariableExpression
        {
            get { return GetChild(Roles.Variable); }
            set { SetChild(Roles.Variable, value); }
        }

        public NodeList<VariableDereferenceModifier> Modifiers
        {
            get { return GetChild(Roles.Modifiers); }
            set { SetChild(Roles.Modifiers, value); }
        }

        public Expression IndexerExpression
        {
            get { return GetChild(Roles.IndexerExpression); }
            set { SetChild(Roles.IndexerExpression, value); }
        }
    }

    public class VariableDereferenceExpression : ExpansionStyleExpression {}

    public class LiteralExpansionExpression : ExpansionStyleExpression {}

    public class VariableDereferenceModifier : Node
    {
        public char Command { get; set; }

        public Expression Value
        {
            get { return GetChild(Roles.Value); }
            set { SetChild(Roles.Value, value); }
        }
    }

    public class BinaryOperatorExpression : Expression
    {
        public Expression Left
        {
            get { return GetChild(Roles.Left); }
            set { SetChild(Roles.Left, value); }
        }

        public NodeList<Node> Right
        {
            get { return GetChild(Roles.Right); }
            set { SetChild(Roles.Right, value); }
        }

        public Operator Operator { get; set; }
    }

    public class NotOperatorExpression : Expression
    {
        public Expression Expression
        {
            get { return GetChild(Roles.Value); }
            set { SetChild(Roles.Value, value); }
        }
    }

    public enum Operator
    {
        Assignment,
        Append,
        Subtract,
        In,
        AssignmentIfEmpty,
        And,
        Or,
        NotEqual,
        LessThan,
        GreaterThan,
        Not,
        GreaterThanOrEqual,
        LessThanOrEqual
    }

    public class RuleDeclarationStatement : Statement
    {
        public string Name { get; set; }

        public BlockStatement Body
        {
            get { return GetChild(Roles.Body); }
            set { SetChild(Roles.Body, value); }
        }

        public string[] Arguments { get; set; }
    }

    public class ActionsDeclarationStatement : Statement
    {
        public string Name { get; set; }

        public NodeList<Expression> Modifiers
        {
            get { return GetChild(Roles.ActionModifiers); }
            set { SetChild(Roles.ActionModifiers, value); }
        }

        public string Actions { get; set; }
    }

    public class OnStatement : Statement
    {
        public Expression Target
        {
            get { return GetChild(Roles.Target); }
            set { SetChild(Roles.Target, value); }
        }

        public Statement Body
        {
            get { return GetChild(Roles.SingleStatementBody); }
            set { SetChild(Roles.SingleStatementBody, value); }
        }
    }

    public class AssignmentStatement : Statement
    {
        public Expression Left
        {
            get { return GetChild(Roles.Left); }
            set { SetChild(Roles.Left, value); }
        }

        public NodeList<Node> Right
        {
            get { return GetChild(Roles.Right); }
            set { SetChild(Roles.Right, value); }
        }

        public Operator Operator { get; set; }
    }

    public class WhileStatement : Statement
    {
        public Expression Condition
        {
            get { return GetChild(Roles.Condition); }
            set { SetChild(Roles.Condition, value); }
        }

        public BlockStatement Body
        {
            get { return GetChild(Roles.Body); }
            set { SetChild(Roles.Body, value); }
        }
    }

    public class ForStatement : Statement
    {
        public LiteralExpression LoopVariable
        {
            get { return GetChild(Roles.LoopVariable); }
            set { SetChild(Roles.LoopVariable, value); }
        }

        public NodeList<Node> List
        {
            get { return GetChild(Roles.List); }
            set { SetChild(Roles.List, value); }
        }

        public BlockStatement Body
        {
            get { return GetChild(Roles.Body); }
            set { SetChild(Roles.Body, value); }
        }
    }

    public class BreakStatement : Statement {}

    public class ContinueStatement : Statement {}

    public class LocalStatement : Statement
    {
        public LiteralExpression Variable
        {
            get { return GetChild(Roles.LocalVariable); }
            set { SetChild(Roles.LocalVariable, value); }
        }

        public NodeList<Node> Value
        {
            get { return GetChild(Roles.Right); }
            set { SetChild(Roles.Right, value); }
        }
    }

    public class InvocationExpression : Expression
    {
        public Expression RuleExpression
        {
            get { return GetChild(Roles.RuleName); }
            set { SetChild(Roles.RuleName, value); }
        }

        public NodeList<NodeList<Node>> Arguments
        {
            get { return GetChild(Roles.Arguments); }
            set { SetChild(Roles.Arguments, value); }
        }

        public Expression OnTarget
        {
            get { return GetChild(Roles.Target); }
            set { SetChild(Roles.Target, value); }
        }
    }

    public class VariableOnTargetExpression : Expression
    {
        public Expression Variable
        {
            get { return GetChild(Roles.Variable); }
            set { SetChild(Roles.Variable, value); }
        }

        public NodeList<Node> Targets
        {
            get { return GetChild(Roles.Targets); }
            set { SetChild(Roles.Targets, value); }
        }
    }

    public static class ASTExtensions
    {
        public static T To<T>(this Node @this) where T : Node
        {
            if (@this == null)
                throw new InvalidCastException();
            if (!(@this is T))
                throw new InvalidCastException($"Expected node type {typeof(T).Name} but got: {@this.GetType().Name}");
            return (T)@this;
        }

        public static T To<T>(this Expression @this) where T : Expression
        {
            if (@this == null)
                throw new InvalidCastException();
            if (!(@this is T))
                throw new InvalidCastException($"Expected expression type {typeof(T).Name} but got: {@this.GetType().Name}");
            return (T)@this;
        }

        public static T To<T>(this Statement @this) where T : Statement
        {
            if (@this == null)
                throw new InvalidCastException();
            if (!(@this is T))
                throw new InvalidCastException($"Expected statement type {typeof(T).Name} but got: {@this.GetType().Name}");
            return (T)@this;
        }

        public static NodeList<T> ToNodeList<T>(this IEnumerable<T> values) where T : Node
        {
            return new NodeList<T>(values);
        }
    }

    public class Role {}

    public class Role<T> : Role {}

    static class Roles
    {
        public static readonly Role<BlockStatement> Body = new Role<BlockStatement>();
        public static readonly Role<Statement> Else = new Role<Statement>();
        public static readonly Role<Expression> Condition = new Role<Expression>();
        public static readonly Role<NodeList<Node>> Statements = new Role<NodeList<Node>>();
        public static readonly Role<NodeList<Node>> ReturnExpression = new Role<NodeList<Node>>();
        public static readonly Role<Expression> Expression = new Role<Expression>();
        public static readonly Role<Expression> Variable = new Role<Expression>();
        public static readonly Role<LiteralExpression> LocalVariable = new Role<LiteralExpression>();
        public static readonly Role<NodeList<SwitchCase>> SwitchCases = new Role<NodeList<SwitchCase>>();
        public static readonly Role<LiteralExpression> CaseExpression = new Role<LiteralExpression>();
        public static readonly Role<NodeList<Expression>> Elements = new Role<NodeList<Expression>>();
        public static readonly Role<NodeList<VariableDereferenceModifier>> Modifiers = new Role<NodeList<VariableDereferenceModifier>>();
        public static readonly Role<Expression> IndexerExpression = new Role<Expression>();
        public static readonly Role<Expression> Value = new Role<Expression>();
        public static readonly Role<Expression> Left = new Role<Expression>();
        public static readonly Role<NodeList<Node>> Right = new Role<NodeList<Node>>();
        public static readonly Role<NodeList<Expression>> ActionModifiers = new Role<NodeList<Expression>>();
        public static readonly Role<Expression> Target = new Role<Expression>();
        public static readonly Role<NodeList<Node>> Targets = new Role<NodeList<Node>>();
        public static readonly Role<Statement> SingleStatementBody = new Role<Statement>();
        public static readonly Role<LiteralExpression> LoopVariable = new Role<LiteralExpression>();
        public static readonly Role<NodeList<Node>> List = new Role<NodeList<Node>>();
        public static readonly Role<Expression> RuleName = new Role<Expression>();
        public static readonly Role<NodeList<NodeList<Node>>> Arguments = new Role<NodeList<NodeList<Node>>>();
    }
}
