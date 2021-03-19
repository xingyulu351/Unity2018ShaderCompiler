namespace BindingsToCsAndCpp
{
    internal class CSAutoProperty : CSProperty
    {
        public override bool IsPointer
        {
            get
            {
                return base.IsPointer;
            }
            set
            {
                base.IsPointer = value;
                RecreateBody();
                SetNames(value);
            }
        }

        void RecreateBody()
        {
            var prop = ParsingHelpers.ParseAutoProp(OriginalContent);
            if (IsPointer)
            {
                Getter = string.Format("return Scripting::ScriptingWrapperFor(self->{0}());", prop.Item2);
                if (!string.IsNullOrEmpty(prop.Item3))
                    Setter = string.Format("\nself->{0} (val);", prop.Item3);
            }
            else
            {
                Getter = string.Format("return self->{0} ();", prop.Item2);
                if (!string.IsNullOrEmpty(prop.Item3))
                    Setter = string.Format("\nself->{0} (value);", prop.Item3);
            }
        }

        public override string StaticModifier { get { return base.StaticModifier.Trim(); } }

        public CSAutoProperty(string className, string content) : base(className, ParsingHelpers.RecreateAutoProp(content))
        {
            OriginalContent = content;
            SetNames();
        }

        void SetNames(bool isPointer = false)
        {
            NeedsInternal = AstReturnType.ShouldRewriteReturnValueAsParameterByReference() && !isPointer;
            var format = InternalNameFormat;
            if (!NeedsInternal)
                format = "{0}_{1}";
            GetterInternalName = string.Format(format, "get", Name);
            SetterInternalName = string.Format(format, "set", Name);
        }
    }
}
