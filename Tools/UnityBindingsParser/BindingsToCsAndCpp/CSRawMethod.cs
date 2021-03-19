using System.Linq;
using ICSharpCode.NRefactory.CSharp;

namespace BindingsToCsAndCpp
{
    public class CSRawMethod : CSMethod
    {
        string _originalContent;

        public CSRawMethod(string className, string content, EntityDeclaration expression) : base()
        {
            var decl = ParsingHelpers.SplitSignatureAndBodyFromCustom(content);
            Signature = decl.Item1;
            Body = decl.Item2;
            Header = decl.Item3;
            Footer = decl.Item4;
            ParamString = ParsingHelpers.ExtractString(Signature, "(", ")");
            Parameters = expression.GetParameters().ToList();

            Init(className, expression);

            ParseDefaultArguments();

            AstNode = expression;
            _originalContent = content;
        }

        public override void Generate()
        {
            if (!string.IsNullOrWhiteSpace(Header))
                output.AppendLine(Header);

            // We don't want to generate attributes at this point
            GenerateICall();

            if (!string.IsNullOrWhiteSpace(Footer))
                output.AppendLine(Footer);
        }

        protected override void GenerateICall()
        {
            if (FullDefaultParams.Count == 0)
            {
                GenerateAttributes();

                output.Append(_originalContent.Substring(Header.Length));
                return;
            }

            var whereClause = GetTextBetweenSignatureEndAndBodyStart().Trim();

            GenerateDefaultOverloads(false, whereClause, false);
            Name = Name.Trim();
            GeneratePublic();
        }

        protected override void GeneratePublic()
        {
            const string sig = @"{0} {1}{2} {3}{4}({5})";

            var genericParameters = HasGenericParameters ? GenericParametersString : string.Empty;

            GenerateAttributes();

            output.AppendFormat(sig, Visibility, StaticModifier, ReturnType, Name, genericParameters, CleanParameterSignature);
            output.Append(GetTextBetweenSignatureEndAndBodyStart());
            output.AppendLine(GetBodyText());
        }

        protected override string GetDirectionForCall(ParameterDeclaration parameter, int padding)
        {
            var ret = parameter.GetDirection();
            return string.IsNullOrWhiteSpace(ret) ? string.Empty : ret.PadRight(ret.Length + padding);
        }

        private string GetTextBetweenSignatureEndAndBodyStart()
        {
            var sigEnd = _originalContent.IndexOf(')') + 1;
            var bodyStart = _originalContent.IndexOf('{');
            return _originalContent.Substring(sigEnd, bodyStart - sigEnd);
        }

        private string GetBodyText()
        {
            var bodyStart = _originalContent.IndexOf('{');
            return _originalContent.Substring(bodyStart, _originalContent.LastIndexOf('}') + 1 - bodyStart);
        }
    }
}
