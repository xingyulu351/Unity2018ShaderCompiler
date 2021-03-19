namespace BindingsToCsAndCpp
{
    public class CSAutoMethod : CSMethod
    {
        public CSAutoMethod(string className, string content) : base()
        {
            var decl = ParsingHelpers.SplitSignatureAndBodyFromCustom(content);
            Signature = decl.Item1;
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

            InternalName = string.Format(InternalNameFormat, Name);
        }

        protected override void GenerateICall()
        {
            GeneratePublic();
            GenerateDefaultOverloads(false, string.Empty, true);
            GeneratePrivateICall();
        }
    }
}
