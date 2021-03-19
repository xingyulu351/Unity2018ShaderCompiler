using System;
using System.Linq;
using JamSharp.Runtime.Parser;

namespace JamSharp.Runtime
{
    public class RuntimeExpressionEvaluation
    {
        public static JamList Evaluate(Expression e, GlobalVariables globals)
        {
            var ese = e as ExpansionStyleExpression;
            if (ese != null)
                return EvaluateExpansionStyleExpression(ese, globals);

            var literal = e as LiteralExpression;
            if (literal != null)
                return new JamList(literal.Value);

            var combine = e as CombineExpression;
            if (combine != null)
                return EvaluateCombineExpression(combine, globals);

            var empty = e as EmptyExpression;
            if (empty != null)
                return new JamList();

            throw new ArgumentException($"Evaluate cannot evaluate {e} yet.");
        }

        static JamList EvaluateCombineExpression(CombineExpression combine, GlobalVariables globals)
        {
            var expressions = combine.Elements;

            //slight hack resulting in using the jam language parser for parsing non-jam-language actions. in actions we do not want quotes to be swallowed, however
            //the language parser does this. The runtime evaluator is only used by actions, which makes it safe to here "resurface the quotes".
            foreach (var e in expressions.OfType<LiteralExpression>().Where(l => l.Value.Length == 0))
                e.Value = "\"";

            return BuiltinRules.Combine(expressions.Select(e => Evaluate(e, globals)).ToArray());
        }

        public static JamList EvaluateExpansionStyleExpression(ExpansionStyleExpression expression, GlobalVariables globals)
        {
            JamList variable = Evaluate(expression.VariableExpression, globals);

            var result = expression is VariableDereferenceExpression
                ? globals[variable.Elements.Single()]
                : variable;

            if (expression.IndexerExpression != null)
            {
                result = result.IndexedBy(expression.IndexerExpression.To<LiteralExpression>().Value);
            }

            foreach (var m in expression.Modifiers.OrderBy<VariableDereferenceModifier, int>(m => JamModifierOrderFor(m.Command)))
            {
                switch (m.Command)
                {
                    case 'C':
                        result = result.Escape();
                        break;
                    case 'T':
                        result = result.GetBoundPath();
                        break;
                    case 'J':
                        result = result.JoinWithValue(Evaluate(m.Value, globals));
                        break;
                    case 'B':
                        result = expression.Modifiers.Any(m2 => m2.Command == 'S')
                            ? result.BaseAndSuffix()
                            : result.GetFileNameBase();
                        break;
                    case 'S':
                        if (m.Value != null || !expression.Modifiers.Any(m2 => m2.Command == 'B'))
                        {
                            var tempResult = m.Value == null
                                ? result.GetSuffix()
                                : result.WithSuffix(Evaluate(m.Value, globals));
                            result = tempResult;
                            if (expression.Modifiers.Any(m2 => m2.Command == 'B'))
                                result = result.BaseAndSuffix();
                        }
                        break;
                    case 'D':
                        if (m.Value != null)
                            throw new ArgumentException($"No support for runtime evaluation of {m.Command} modifier with a value");

                        result = result.GetDirectory();
                        break;
                    case '\\':
                        result = result.BackSlashify();
                        break;
                    case '/':
                        result = result.ForwardSlashify();
                        break;
                    case 'N':
                        result = result.JoinWithValue(globals["NEWLINE"].FirstOrDefault() ?? "\n");
                        break;
                    case 'E':
                        result = result.IfEmptyUse(m.Value != null ? Evaluate(m.Value, globals) : new JamList(""));
                        break;
                    default:
                        throw new NotSupportedException($"Modifier {m.Command} not yet supported in runtime evaluation");
                }
            }

            return result;
        }

        public static int JamModifierOrderFor(char command)
        {
            const string order = "EWT  GDBSRM   UL/\\CJXI";
            return order.IndexOf(command);
        }
    }
}
