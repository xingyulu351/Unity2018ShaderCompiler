using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Text.RegularExpressions;
using ICSharpCode.NRefactory.CSharp;

namespace BindingsToCsAndCpp
{
    public class CSBaseMethod : CSUnit
    {
        public bool IsConstructor
        {
            get { return AstNode is ConstructorDeclaration; }
        }

        public string Header;
        public string Footer;
        public string ParamString;
        protected string CleanParameterCall;
        protected string CleanParameterSignature;
        protected List<Tuple<ParameterDeclaration, string, string>> FullDefaultParams = new List<Tuple<ParameterDeclaration, string, string>>();
        protected List<ParameterDeclaration> NormalParams;
        protected string[] NormalParamSigs;
        protected List<ParameterDeclaration> DefaultParams;
        protected List<string> DefaultParamSigs;

        protected const string InternalNameFormat = "INTERNAL_CALL_{0}";

        public CSBaseMethod()
        {
        }

        public CSBaseMethod(string className, string content)
        {
            var decl = ParsingHelpers.SplitSignatureAndBodyFromCustom(content);
            Signature = CSharpSignatureAnalyzer.PrepareSignatureMethod(decl.Item1);
            Body = decl.Item2;
            Header = decl.Item3;
            Footer = decl.Item4;
            ParamString = ParsingHelpers.ExtractString(Signature, "(", ")");

            var expression = CSharpSignatureAnalyzer.AnalyzeMethod(className, Signature);

            if (expression == null)
                throw new InvalidMethodSignatureException(Signature);

            Init(className, expression);

            ParseDefaultArguments();

            AstNode = expression;

            NeedsInternal = Parameters.FirstOrDefault(x => x.IsStruct() && !x.IsOut()) != null || AstReturnType.ShouldRewriteReturnValueAsParameterByReference();
            if (NeedsInternal)
                InternalName = string.Format(InternalNameFormat, Name);
            if (NormalParams.Count == NormalParamSigs.Length && DefaultParams.Count == DefaultParamSigs.Count)
                return;
            // There was an undetected error parsing the param signatures
            if (!(DefaultParams.Count == 0 && DefaultParamSigs.Count == 1 && string.IsNullOrEmpty(DefaultParamSigs[0])))
                // Method with no params will have one empty default param signature
                throw new InvalidMethodSignatureException(Signature);
        }

        public bool HasGenericParameters
        {
            get
            {
                var methodDeclaration = AstNode as MethodDeclaration;
                return methodDeclaration != null && methodDeclaration.Children.OfType<TypeParameterDeclaration>().Any();
            }
        }

        public string GenericParametersString
        {
            get
            {
                var methodDeclaration = AstNode as MethodDeclaration;
                if (methodDeclaration == null)
                    return "";

                var typedParameters = methodDeclaration.Children.OfType<TypeParameterDeclaration>().ToArray();
                if (typedParameters.Length == 0)
                    return "";

                var sb = new StringBuilder();
                sb.Append("<");
                foreach (var typedParameter in typedParameters)
                {
                    sb.Append(typedParameter.Name);
                }
                sb.Append(">");
                return sb.ToString();
            }
        }

        public string ConstraintString
        {
            get
            {
                var constraint = AstNode.Children.OfType<Constraint>().FirstOrDefault();
                return constraint == null ? "" : constraint.ToString();
            }
        }

        protected void ParseDefaultArguments()
        {
            NormalParams = Parameters.TakeWhile(x => !x.HasDefaultExpression() || ParsingHelpers.Enums.Contains(x.Name)).ToList();

            var args = new List<string>();

            var list = Regex.Split(ParamString, @"(\[,+\])");

            foreach (var str in list)
            {
                if (str.StartsWith("[,"))
                {
                    args[args.Count - 1] += str;
                }
                else
                {
                    var temp = str.Split(',');
                    if (args.Count == 0)
                        args.AddRange(temp);
                    else
                    {
                        args[args.Count - 1] += temp[0];
                        if (temp.Length > 1)
                            args.AddRange(temp.Skip(1));
                    }
                }
            }

            args.RemoveAll(s => string.IsNullOrWhiteSpace(s));
            NormalParamSigs = args.Take(NormalParams.Count()).ToArray();
            DefaultParams = Parameters.SkipWhile(x => !x.HasDefaultExpression() || ParsingHelpers.Enums.Contains(x.Name)).ToList();
            DefaultParamSigs = args.Skip(NormalParams.Count()).ToList();

            for (var i = 0; i < DefaultParams.Count; i++)
            {
                var parts = DefaultParamSigs[i].Split('=');
                var idx = parts[0].LastIndexOf(DefaultParams[i].Name) + DefaultParams[i].Name.Length;
                var left = parts[0];
                var right = DefaultParamSigs[i].Substring(idx);

                FullDefaultParams.Add(Tuple.Create(DefaultParams[i], left, right));
            }

            CleanParameterSignature = NormalParamSigs.Union(FullDefaultParams.Select(x => string.Format(" [uei.DefaultValue(\"" + x.Item3.Substring(x.Item3.IndexOf('=') + 1).Trim().Replace("\"", "\\\"") + "\")] " + x.Item2))).Join(",");
            CleanParameterCall = NormalParams.Select(x => GetDirectionForCall(x, 1) + x.Name).Union(DefaultParams.Select(d => GetDirectionForCall(d, 1) + d.Name)).Select(x => x.Trim()).Join(",");
        }
    }
}
