namespace BindingsToCsAndCpp
{
    public class CSBaseProperty : CSUnit
    {
        public const string InternalNameFormat = "INTERNAL_{0}_{1}";
        public string Getter { get; set; }
        public string Setter { get; set; }

        public string GetterInternalName;
        public string SetterInternalName;
        public string GetterName;
        public string SetterName;

        public bool HasSetter { get { return Setter != null; }}

        public CSBaseProperty(string className, string content)
        {
            var decl = ParsingHelpers.SplitSignatureGetterAndSetterFromCustomProp(content);
            var signature = decl.Item1;
            var declaration = CSharpSignatureAnalyzer.AnalyzeProperty(signature);
            Signature = decl.Item1;
            Getter = decl.Item2;
            Setter = decl.Item3;
            GetterName = "get_" + Name;
            SetterName = "set_" + Name;

            Init(className, declaration);
        }
    }
}
