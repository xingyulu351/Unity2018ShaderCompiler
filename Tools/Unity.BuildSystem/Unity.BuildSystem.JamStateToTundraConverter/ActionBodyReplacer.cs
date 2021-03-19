using System;
using System.Collections.Generic;
using System.Linq;
using JamSharp.Runtime;
using JamSharp.Runtime.Parser;
using NiceIO;

namespace Unity.BuildSystem.JamStateToTundraConverter
{
    class ActionBodyReplacer
    {
        public IEnumerable<Replacement> FindSpansToReplace(string actionBody)
        {
            for (int i = 0; i < actionBody.Length - 2; i++)
            {
                var isDeref = actionBody[i] == '$' && actionBody[i + 1] == '(';
                var isResponse = actionBody[i] == '@' && actionBody[i + 1] == '(';

                if (!isDeref && !isResponse)
                    continue;

                int parseFrom = FindFirstCharacterAfterWhitespaceBefore(i - 1, actionBody);

                var parserInput = actionBody.Substring(parseFrom);
                var parser = new Parser(parserInput, ParserMode.QuotesAsLiteral);
                var expression = parser.ParseExpression();
                var nextToken = parser.ScanResult.ScanTokens[parser.ScanResult.GetCursor()];
                var charactersRead = nextToken.textLocation;

                var replacement = new Replacement
                {
                    startIndex = parseFrom,
                    expression = expression,
                    endIndex = parseFrom + charactersRead,
                    isResponseReplacement = isResponse
                };

                yield return replacement;

                i = parseFrom + charactersRead;
            }
        }

        int FindFirstCharacterAfterWhitespaceBefore(int searchFrom, string actionBody)
        {
            int i = searchFrom;

            while (true)
            {
                if (i <= 0)
                    return 0;
                var c = actionBody[i];
                if (char.IsWhiteSpace(c) || c == '@')
                    return i + 1;
                i--;
            }
        }

        public class Replacement
        {
            public int startIndex;
            public int endIndex;
            public Expression expression;
            public bool isResponseReplacement;
        }

        public ShellCommand Replace(string actionBody, GlobalVariables globals)
        {
            try
            {
                var responseReplacements = new List<JamStateToTundraConverter.Replacement>();
                foreach (var replacement in FindSpansToReplace(actionBody).Reverse())
                {
                    //In jam actions, you do not write "jam language", but you write raw shell code. You area allowed however to use things like $(TOP).  so the original jam
                    //"recycles" its language parser to try to find these variable dereference expressions, and replace them with the correct value.  It then uses a crazy construct
                    //where it "recycles" the jam language syntax of a LiteralExpansionModifier:   @(hello:U)  (evaluates to HELLO).  You can use that syntax in an action, however
                    //the meaning is completely different from in jam language.   In an action it means "okay, take everything that is inside the @(.....),  and put that into a responsefile
                    //then replace the whole "@(.....)" with the filename of the response file.   In the real world, you often see this resulting in action code that looks like
                    //this: @@($(1))  so that's saying take $(1) and put that into a repsonse file.   then replace @(..)  with the repsonse file name.   so after all expansion
                    //it turns into  @repsonsefile123.rsp.   The additional @ that you often see is not really for jam, but it's because many compilers say "you can use responsefiles, but
                    //if you want to do so, you have to prefix their filename with @ so I understand".
                    //
                    //So we follow the same route that jam does here, and we recycle our actual jam language parser to try to find expressions we can replace.  If we find one of these
                    //crazy LiteralExpansionExpressions,  we evaluate it, remember the contents,  and then we replace it with a LiteralExpression that holds a replacement token, that ShellCommand
                    //knows how to deal with. Instead of writing a full on AST walker with replace functionality, we restrict our search to "the expression we found itself is a LEE" or "the expression we
                    //found is a CombineExpression that has an element which is a LEE".  In practice I've never found another construct in the actions we use to build unity.
                    //
                    //Sorry for having you read through this many pages of explanation, believe me it took longer to figure out than to write down... there's testcoverage for all cases I have run into.

                    var replacementExpression = replacement.expression;

                    var ce = replacementExpression as CombineExpression;

                    if (ce != null && ce.Elements.OfType<LiteralExpansionExpression>().Any())
                        throw new ArgumentException($"{nameof(ActionBodyReplacer)} does not support responsefiles being part of a combine expression, like done here: {replacementExpression}");

                    var replacedExpression = ReplacementExpression(replacementExpression, responseReplacements, globals, replacement.isResponseReplacement);

                    var replacementValue =
                        RuntimeExpressionEvaluation.Evaluate(replacedExpression, globals).ToString();
                    actionBody = actionBody.Substring(0, replacement.startIndex) + replacementValue +
                        actionBody.Substring(replacement.endIndex);
                }

                NPath depFile;
                if (globals["DEPFILE"].Any())
                    depFile = new NPath(globals["DEPFILE"].Single());
                else
                    depFile = null;

                bool hasShowIncludes = globals["SHOWINCLUDES"] == "1";

                return new ShellCommand(actionBody, depFile, responseReplacements.ToArray(), hasShowIncludes: hasShowIncludes);
            }
            catch (Exception)
            {
                Console.WriteLine($"Failed replacing actionbody: {actionBody}");
                throw;
            }
        }

        static Expression ReplacementExpression(Expression replacementExpression, List<JamStateToTundraConverter.Replacement> responseReplacements, GlobalVariables globals, bool isResponse)
        {
            var token = $"REPLACE_TOKEN_{responseReplacements.Count}_";

            string originalName = null;
            var deref = replacementExpression as VariableDereferenceExpression;
            if (deref != null)
            {
                var a = deref.VariableExpression as LiteralExpression;
                originalName = a?.Value;
            }

            responseReplacements.Add(new JamStateToTundraConverter.Replacement(token, RuntimeExpressionEvaluation.Evaluate(replacementExpression, globals).ToString(), isResponse, originalName));
            return new LiteralExpression {Value = token};
        }
    }
}
