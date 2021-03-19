using System;
using System.Linq;
using ICSharpCode.NRefactory.CSharp;

namespace Cs2us
{
    public class Cs2UsUtils
    {
        public static string ConvertStuff(string input, bool explicitlyTypedVariables, bool usePragmaStrict)
        {
            var parser = new CSharpParser();
            var exps = parser.ParseStatements(input);
            var cs2UsVisitor = new Cs2UsVisitor(explicitlyTypedVariables, usePragmaStrict);
            exps.First().AcceptVisitor(cs2UsVisitor);
            return TrimLastNewline(cs2UsVisitor.Text);
        }

        public static string ConvertClass(string source, bool explicitlyTypedVariables, bool usePragmaStrict)
        {
            var parser = new CSharpParser();
            var syntaxTree = parser.Parse(source);
            var cs2UsVisitor = new Cs2UsVisitor(explicitlyTypedVariables, usePragmaStrict);
            if (syntaxTree.Errors.Count > 0)
            {
                throw new Exception(syntaxTree.Errors.Aggregate("", (acc, curr) => acc + "\r\n" + curr.Message + " " + curr.Region));
            }
            syntaxTree.FileName = "ParsedCs2us.cs";
            syntaxTree.AcceptVisitor(cs2UsVisitor);
            return TrimLastNewline(cs2UsVisitor.Text);
        }

        private static string TrimLastNewline(string text)
        {
            return text.TrimEnd('\r', '\n');
        }
    }
}
